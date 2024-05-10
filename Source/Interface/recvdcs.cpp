#include "recvdcs.h"

#include <QDebug>

RecvDCS::RecvDCS(QString strIP, int nPort, int nSiteID)
    : m_strIP(strIP),
      m_nPort(nPort),
      m_nSiteID(nSiteID),
      m_bRecv(true),
      m_bThread(true) {}

RecvDCS::~RecvDCS() {}

void RecvDCS::start() {
  // qDebug() << "Start DCS Clinet";

  while (m_bThread) {
    // 연결 실패시 재연결을 하기위한 재시도
    discover = DCSDisc::DCSDiscover();
    info = DCSCom::DCSClientInfo();
    time = DCSTimeHeader();
    scenario = DCSScenarioHeader();
    headers = DCSCom::HeaderList();
    data = nullptr;

    // bool bReturn = SetClient();
    if (m_bRecv == true) SetClient();

    /*
    if (bReturn == 0)  // 연결 안됨, 3초 뒤 재연결
    {
      Sleep(1000);
    }
*/
  }
  emit finished();
}

void RecvDCS::stop() {
  m_bThread = false;
  m_bRecv = false;
}

bool RecvDCS::SetClient() {
  // 소켓 초기화
  if (!initializeSocket()) {
    char errormsg[256];
    // qDebug() << "Error initializing socket: " << getSockErr();
    getSockErrString(errormsg, 256);
    // qDebug() << "System error message: " << errormsg;
    return 0;
  }

  if (!DCSDisc::discoverClientInit(discover, 1, NULL)) {
    char errormsg[256];

    // qDebug() << "Error initializing multicast socket for server discovery: "
    // << getSockErr();
    getSockErrString(errormsg, 256);
    // qDebug() << "System error message: " << errormsg;
    return 0;
  }

  // Set IP/Port
  sprintf(discover.addr, "%s", (const char*)m_strIP.toStdString().c_str());
  discover.port = m_nPort;

  // qDebug() << "Set DCS IP : " << discover.addr
  //          << "Set DCS PORT : " << discover.port;
  /*
    DCSDisc::discoverRequest(discover);
    qDebug() << "Looking for a server";

    // Wait 2 minutes for the response
    if (!discover.respond->readReady(20, 0)) {
      qDebug() << "No server found.  Shutting down.";
      finalizeSocket();
      return 0;
    }
  */
  if (!DCSCom::clientConnect(info, discover.port, discover.addr, NULL)) {
    qDebug() << "Failed to connect to server: " << getSockErr();

    DCSDisc::discoverFree(discover);
    finalizeSocket();
    return 0;

  } else {
    qDebug() << "Connected to server";
  }

  if (!DCSCom::clientNegotiate(info, time, scenario, headers)) {
    // qDebug() << "Failed to negotiate connection: " << getSockErr();
    DCSCom::clientDisconnect(info);
    DCSDisc::discoverFree(discover);
    finalizeSocket();
    return 0;

  } else {
    char addr[DCS_ADDRSTRLEN];

    // qDebug() << "Connection settings: " << info.datatype << " "
    //          << info.datasock->getPort() << " "
    //          << info.datasock->getAddress(addr, sizeof(addr));
  }

  if (!DCSCom::clientRequest(info, DCSREQUEST_DATAMODE)) {
    // qDebug() << "DCSREQUEST_DATAMODE failed";
  }
  if (!DCSCom::clientRequest(info, DCSREQUEST_TIME)) {
    // qDebug() << "DCSREQUEST_TIME failed";
  }
  if (!DCSCom::clientRequest(info, DCSREQUEST_SCENARIO)) {
    // qDebug() << "DCSREQUEST_SCENARIO failed";
  }
  if (!DCSCom::clientRequest(info, DCSREQUEST_HEADERS)) {
    // qDebug() << "DCSREQUEST_HEADERS failed";
  }
  if (!DCSCom::clientRequest(info, DCSREQUEST_KEEPALIVE)) {
    // qDebug() << "DCSREQUEST_KEEPALIVE failed";
  }

  while (m_bRecv) {
    if (info.servsock->readReady(0, 5000)) {
      int8_t byte;
      int32_t result = info.servsock->read(&byte, sizeof(byte));
      if (result == SOCKET_ERROR || result == 0) {
        // qDebug() << "Lost connection to server";
        m_bRecv = false;
        break;  // continue;
      }

      switch (byte) {
        case DCSREQUEST_DATAMODE: {
          // We requested the data mode from the server.
          // Server sends 1 byte for mode type (Unicast,Broadcast,Multicast).
          // Followed 2 bytes for port.
          // Followed by DCS_ADDRSTRLEN bytes as a string containing address.
          // If the type is Unicast, the server sends it's own address incase
          // the client would like to 'connect' to that address.
          int8_t datatype;
          uint16_t dataport;
          char dataaddr[DCS_ADDRSTRLEN];
          info.servsock->readAll(&datatype, sizeof(datatype));
          info.servsock->readAll(&dataport, sizeof(dataport));
          dataport = ntohs(dataport);
          info.servsock->readAll(dataaddr, sizeof(dataaddr));
          break;
        }
        case DCSREQUEST_TIME:
          // We requested the time header.
          data = DCSCom::readTCPData(info);
          Q_EMIT signal_RecvDCSData(data, m_nSiteID);
          break;
        case DCSREQUEST_SCENARIO:
          // We requested the scenario header.
          data = DCSCom::readTCPData(info);
          Q_EMIT signal_RecvDCSData(data, m_nSiteID);
          break;
        case DCSREQUEST_EVENT:
          // The server sent us an event.
          data = DCSCom::readTCPData(info);
          Q_EMIT signal_RecvDCSData(data, m_nSiteID);
          break;
        case DCSREQUEST_HEADER:
          // The server sent us a new header.
          data = DCSCom::readTCPData(info);
          Q_EMIT signal_RecvDCSData(data, m_nSiteID);
          break;
        case DCSREQUEST_HEADERS: {
          // Server is responding to our request for the headers.
          // It first sends the total number of headers.
          // Then sends each header.
          int32_t total;

          info.servsock->readAll(&total, sizeof(total));
          total = ntohl(total);

          while (total-- > 0) {
            data = DCSCom::readTCPData(info);
            Q_EMIT signal_RecvDCSData(data, m_nSiteID);
          }
          break;
        }
        case DCSREQUEST_DISCONNECT:
          // qDebug() << "Server sent disconnect";
          m_bRecv = false;
          break;
        case DCSREQUEST_KEEPALIVE:
          break;
      }
    }

    // Test Data socket - a real application would probably want to read
    // more than one at a time.
    int limit = 100;
    while (info.datasock->readReady(0, 5000) && --limit > 0) {
      // We have something
      data = DCSCom::readUDPData(info);

      // Make sure it wasn't junk on the network that we want to ignore.
      if (data) {
        Q_EMIT signal_RecvDCSData(data, m_nSiteID);
      }
    }
  }

  // qDebug() << "Disconnecting DCS";
  DCSCom::clientDisconnect(info);
  DCSDisc::discoverFree(discover);
  finalizeSocket();

  return 1;
}
