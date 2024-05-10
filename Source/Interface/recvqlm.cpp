#include "recvqlm.h"

#include <QDebug>

RecvQLM::RecvQLM(QHostAddress ip, int port, int nSiteID, QString strCallSign)
    : m_nSiteID(nSiteID), m_strCallSign(strCallSign) {
  SetInit(ip, port);
}
RecvQLM::~RecvQLM() {
  disconnect(m_pSocket, SIGNAL(readyRead()), this, SLOT(SetRecvQLM()));
  m_pSocket->close();
  delete m_pSocket;
  m_pSocket = nullptr;
}

void RecvQLM::SetRecvQLM() {
  QByteArray buffer;

  if (m_pSocket != nullptr) {
    buffer.resize(m_pSocket->pendingDatagramSize());
 Socket->pendingDatagramSize();

    QHostAddress sender;
    quint16 senderPort;

    m_pSocket->readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);

    QLMHeader stQLMHeader;
    memcpy(&stQLMHeader, buffer.constData(), sizeof(QLMHeader));

    QByteArray DataBuffer;
    // qDebug() << "QLM Header in DataSize : " << stQLMHeader.nDataSize;
    DataBuffer.resize(stQLMHeader.nDataSize);
    memcpy(DataBuffer.data(), buffer.constData() + sizeof(QLMHeader),
           stQLMHeader.nDataSize);
    emit signal_RecvQLMData(DataBuffer, m_nSiteID, m_strCallSign);
  }
}

void RecvQLM::SetInit(QHostAddress ip, int port) {
  // qDebug() << "Start UDP";
  m_pSocket = new QUdpSocket();
  auto bBind = m_pSocket->bind(ip, port, QUdpSocket::ShareAddress);
  connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(SetRecvQLM()));
}
