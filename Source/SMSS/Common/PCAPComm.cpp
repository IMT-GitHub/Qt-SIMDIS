// #include "pch.h"
#include "PCAPComm.h"

#ifdef WIN32
  #include <tchar.h>
  #pragma comment(lib, "ws2_32.lib")
  #pragma comment(lib, "wpcap.lib")
#else
  #pragma comment(lib, "ws2_32.lib")
  #pragma comment(lib, "libpcap.lib")
#endif

/*
//https://nmap.org/npcap/ : pcap 사이트
//https://foxtrotin.tistory.com/326 : pcap 프로그래밍-TCP, UDP 패킷 분석 TCP,
UDP reader
*/


#define _CRT_SECURE_NO_WARNINGS

namespace PCAPcomm {
CPCAPThrad::CPCAPThrad() {
  m_uBODY_INTERVAL = 42U;
  Initialize();

#ifdef _WIN32
  /* Load Npcap and its functions. */
  if (!LoadNpcapDlls()) {
    fprintf(stderr, "Couldn't load Npcap\n");
    exit(1);
  }
#endif
}

CPCAPThrad::~CPCAPThrad() { Initialize(); }

/**
 * @author  bieber
 * @brief   pcap 데이터 수집 콜백 함수
 * @remarks
 * @param   param
 * @param   header
 * @param   pkt_data
 * @return  void
 * @date    2022-04-20
 */
void packet_handler(/*UCHAR* param*/ UCHAR* param,
                    const struct pcap_pkthdr* header, const UCHAR* pkt_data) {
  PROCESS_INFO* pProcessInfo = reinterpret_cast<PROCESS_INFO*>(param);

  if (pProcessInfo != NULL) {
    /* save the packet on the dump file */
    if (pProcessInfo->pDumpfile != NULL) {
      pcap_dump(reinterpret_cast<UCHAR*>(pProcessInfo->pDumpfile), header,
                pkt_data);
    }

    if (pProcessInfo->callbackPcapFunction != NULL) {
      pProcessInfo->callbackPcapFunction(header, pkt_data);
    }
  }
}

/**
 * @author  bieber
 * @brief   pcap 데이터 처리 콜백 함수
 * @remarks
 * @param   vtReadPacketInfo
 * @param   header
 * @param   pkt_data
 * @return  void
 * @date    2022-04-20
 */
void dispatcher_handler(u_char* vtReadPacketInfo,
                        const struct pcap_pkthdr* header,
                        const u_char* pkt_data) {
  struct tm* ltime;
  CHAR timestr[16];
  ip_header* ih;
  udp_header* uh;
  UINT32 ip_len;
  USHORT sport;
  USHORT dport;
  time_t local_tv_sec;

  /* retireve the position of the ip header */
  ih = (ip_header*)(pkt_data + 14);  // length of ethernet header

  /* retireve the position of the udp header */
  ip_len = (ih->ver_ihl & 0xf) * 4;
  uh = (udp_header*)((UCHAR*)ih + ip_len);

  /* convert from network byte order to host byte order */
  sport = ntohs(uh->sport);
  dport = ntohs(uh->dport);

  PACKET_INFO packetInfor;
  packetInfor.Initialize();
  packetInfor.uLength = header->caplen + 1U;
  packetInfor.SourceAddress.byte1 = ih->saddr.byte1;
  packetInfor.SourceAddress.byte2 = ih->saddr.byte2;
  packetInfor.SourceAddress.byte3 = ih->saddr.byte3;
  packetInfor.SourceAddress.byte4 = ih->saddr.byte4;
  packetInfor.uSourcePort = sport;
  packetInfor.DestinationAddress.byte1 = ih->daddr.byte1;
  packetInfor.DestinationAddress.byte2 = ih->daddr.byte2;
  packetInfor.DestinationAddress.byte3 = ih->daddr.byte3;
  packetInfor.DestinationAddress.byte4 = ih->daddr.byte4;
  packetInfor.uDestinationPort = dport;

  memcpy(&packetInfor.pcapHeader, header, sizeof(pcap_pkthdr));
  packetInfor.pData = new BYTE[packetInfor.uLength];
  memset(packetInfor.pData, 0x00,
         static_cast<size_t>(packetInfor.uLength) * sizeof(BYTE));

  for (UINT32 i = 0U; i < packetInfor.uLength; i++) {
    memcpy(packetInfor.pData + i, &pkt_data[i], sizeof(BYTE));
  }

  reinterpret_cast<std::vector<PACKET_INFO>*>(vtReadPacketInfo)
      ->push_back(packetInfor);
}

/**
 * @author  bieber
 * @brief   NPCAP dll 불러오기
 * @remarks
 * @return  bool
 * @date    2022-04-20
 */
bool CPCAPThrad::LoadNpcapDlls() {


  bool bRet = true;
  TCHAR npcap_dir[512];
  UINT len;
#ifdef WIN32
  len = GetSystemDirectory(npcap_dir, 480);
  if (!len) {
    fprintf(stderr, "Error in GetSystemDirectory: %x", GetLastError());
    bRet = false;
  }

  if (bRet) {
    _tcscat_s(npcap_dir, 512, _T("\\Npcap"));
    if (SetDllDirectory(npcap_dir) == 0) {
      fprintf(stderr, "Error in SetDllDirectory: %x", GetLastError());
      bRet = false;
    }
  }
#endif
  return bRet;
}

/**
 * @author  bieber
 * @brief   초기화
 * @remarks
 * @return  void
 * @date    2022-04-20
 */
void CPCAPThrad::Initialize() {
  m_uNetMask = 0xffffff;
  m_pAdhandle = NULL;
  memset(&m_deviceInfo, 0x00, sizeof(pcap_if_t));
  m_pDumpFile = NULL;
  m_callbackPcapFunction = NULL;
  m_strFilter = "";
  m_strDumpName = "";
  for (size_t i = 0U; i < m_vtReadPacketInfo.size(); i++) {
    delete m_vtReadPacketInfo[i].pData;
  }
  m_vtReadPacketInfo.clear();
}

/**
 * @author  bieber
 * @brief   객체 소멸 처리
 * @remarks
 * @return  void
 * @date    2022-04-20
 */
void CPCAPThrad::Destroy() { Initialize(); }

/**
 * @author  bieber
 * @brief   객체 포인터 반환
 * @remarks
 * @return  CPCAPThrad *
 * @date    2022-04-20
 */
CPCAPThrad* CPCAPThrad::GetPtr() { return this; }

/**
 * @author  bieber
 * @brief   디바이스 정보 반환
 * @remarks
 * @param   vtDeviceInfo
 * @return  void
 * @date    2022-04-20
 */
void CPCAPThrad::GetFindAllDevice(std::vector<pcap_if_t>& vtDeviceInfo) {
  pcap_if_t* alldevs;
  CHAR errbuf[PCAP_ERRBUF_SIZE];

  /* Retrieve the device list */
  if (pcap_findalldevs(&alldevs, errbuf) == -1) {
    fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
    exit(1);
  }

  /* Print the list */
  INT32 i = 0;
  for (pcap_if_t* d = alldevs; d; d = d->next) {
    pcap_if_t data = *d;
    vtDeviceInfo.push_back(data);
  }
}

/**
 * @author  bieber
 * @brief   디바이스 선택
 * @remarks
 * @param   deviceInfo
 * @return  bool
 * @date    2022-04-20
 */
bool CPCAPThrad::SelectDevice(pcap_if_t deviceInfo) {
  bool bRet = false;
  char errbuf[PCAP_ERRBUF_SIZE];

  /* Open the adapter */
  if ((m_pAdhandle =
           pcap_open_live(deviceInfo.name,  // name of the device
                          65536,            // portion of the packet to capture.
                                  // 65536 grants that the whole packet will be
                                  // captured on all the MACs.
                          1,     // promiscuous mode (nonzero means promiscuous)
                          1000,  // read timeout
                          errbuf  // error buffer
                          )) == NULL) {
    fprintf(stderr, "\nUnable to open the adapter: %s\n", errbuf);
  } else {
    bRet = true;
  }

#ifdef WIN32
  /* Check the link layer. We support only Ethernet for simplicity. */
  if (pcap_datalink(m_pAdhandle) != DLT_EN10MB) {
    fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
  } else {
    bRet = true;
  }

  if (deviceInfo.addresses != NULL) {
    /* Retrieve the mask of the first address of the interface */
    m_uNetMask =
        reinterpret_cast<struct sockaddr_in*>(deviceInfo.addresses->netmask)
            ->sin_addr. S_un.S_addr;
    bRet = true;
  } else {
    /* If the interface is without addresses we suppose to be in a C class
     * network */
    m_uNetMask = 0xffffff;
  }
#endif
  m_deviceInfo = deviceInfo;

  return bRet;
}

/**
 * @author  bieber
 * @brief   디바이스 정보 반환
 * @remarks
 * @return  pcap_if_t
 * @date    2022-04-20
 */
pcap_if_t CPCAPThrad::GetDeviceInformation() { return m_deviceInfo; }

/**
 * @author  bieber
 * @brief   필터 정보 반환
 * @remarks
 * @return  TCHAR *
 * @date    2022-04-20
 */
TCHAR* CPCAPThrad::GetFilterInformation() { return (TCHAR*)m_strFilter.c_str(); }

/**
 * @author  bieber
 * @brief   덤프파일 명 반환
 * @remarks
 * @return  TCHAR *
 * @date    2022-04-20
 */
TCHAR* CPCAPThrad::GetDumpName() { return (TCHAR*)m_strDumpName.c_str(); }

/**
 * @author  bieber
 * @brief   필터 설정
 * @remarks
 * @param   packet_filter
 * @return  bool
 * @date    2022-04-20
 */
bool CPCAPThrad::SetFilter(TCHAR* packet_filter) {
  bool bRet = false;

  QString qstring = QString::fromLocal8Bit((const char*)packet_filter);

  if (m_pAdhandle != NULL) {
    struct bpf_program fcode;
    // compile the filter
#ifdef UNICODE
    if (pcap_compile(m_pAdhandle, &fcode,
                     qstring.toLocal8Bit().constData() /*CW2A(packet_filter)*/,
                     1, m_uNetMask) < 0)
#else
    if (pcap_compile(m_pAdhandle, &fcode, (char *)packet_filter, 1, m_uNetMask) < 0)
#endif
    {
      fprintf(stderr,
              "\nUnable to compile the packet filter. Check the syntax.\n");
    } else {
      // m_strFilter = packet_filter;
      //_tprintf(_T("\npcap filter : %s"), m_strFilter.c_str());

      bRet = true;
    }

    // set the filter
    if (pcap_setfilter(m_pAdhandle, &fcode) < 0) {
      fprintf(stderr, "\nError setting the filter.\n");
    } else {
      bRet = true;
    }
  }

  return bRet;
}

/**
 * @author  bieber
 * @brief   콜백함수 설정
 * @remarks
 * @param   func
 * @return  void
 * @date    2022-04-20
 */
void CPCAPThrad::SetCallbackFunction(CALLBACK_PCAP_FUNC func) {
  m_callbackPcapFunction = std::move(func);
}

/**
 * @author  bieber
 * @brief   캡처 시작
 * @remarks
 * @return  bool
 * @date    2022-04-20
 */
bool CPCAPThrad::StartCapture() {
  bool bRet = false;
  if (m_pAdhandle != NULL) {
    printf("\nDevicename %s, listening on %s...\n", m_deviceInfo.name, m_deviceInfo.description);

    PROCESS_INFO processInfo;
    processInfo.pDumpfile = m_pDumpFile;
    processInfo.callbackPcapFunction = m_callbackPcapFunction;

    /* start the capture */
    INT32 nRet = pcap_loop(m_pAdhandle, 0, packet_handler,
                           reinterpret_cast<unsigned char*>(&processInfo));

    bRet = true;
  }

  return bRet;
}

/**
 * @author  bieber
 * @brief   캡처 종료
 * @remarks
 * @return  void
 * @date    2022-04-20
 */
void CPCAPThrad::FinishCapture() {
  if (m_pAdhandle != NULL) {
    printf("\nlistening off %s...\n", m_deviceInfo.description);

    pcap_breakloop(m_pAdhandle);
    pcap_close(m_pAdhandle);
    m_pAdhandle = NULL;

    pcap_dump_close(m_pDumpFile);
    m_pDumpFile = NULL;
  }
}

/**
 * @author  bieber
 * @brief   덤프파일 생성
 * @remarks
 * @param   fileName(TCHAR*)
 * @return  bool
 * @date    2022-04-20
 */
bool CPCAPThrad::CreateDumpFile(TCHAR* fileName) {
  bool bRet = false;

  QString qstring = QString::fromLocal8Bit((const char*)fileName);
  /* Open the dump file */
#ifdef UNICODE
  m_pDumpFile = pcap_dump_open(
      m_pAdhandle, qstring.toLocal8Bit().constData() /*CW2A(fileName)*/);
#else
  m_pDumpFile = pcap_dump_open(m_pAdhandle, (char *)fileName);
#endif  // UNICODE

  if (m_pDumpFile == NULL) {
    fprintf(stderr, "\nError opening output file\n");
  } else {
    //m_strDumpName = fileName;
    //_tprintf(_T("\nCreate pcap Dump File : %s\n"), m_strDumpName.c_str());

    bRet = true;
  }

  return bRet;
}

/**
 * @author  bieber
 * @brief   덤프파일 생성
 * @remarks
 * @param   fileName(QString)
 * @return  bool
 * @date    2022-04-20
 */
bool CPCAPThrad::CreateDumpFile(QString fileName) {
  bool bRet = false;

  /* Open the dump file */
#ifdef UNICODE
  m_pDumpFile = pcap_dump_open(
      m_pAdhandle, fileName.toLocal8Bit().constData() /*CW2A(fileName)*/);
#else
  m_pDumpFile = pcap_dump_open(m_pAdhandle, fileName.toLocal8Bit().data());
#endif  // UNICODE

  if (m_pDumpFile == NULL) {
    fprintf(stderr, "\nError opening output file\n");
  } else {
    m_strDumpName = fileName.toStdString();
    //_tprintf(_T("\nCreate pcap Dump File : %s\n"), m_strDumpName.c_str());

    bRet = true;
  }

  return bRet;
}

/**
 * @author  bieber
 * @brief   덤프파일 읽기
 * @remarks
 * @param   fileDirectory
 * @return  void
 * @date    2022-04-20
 */
void CPCAPThrad::ReadDumpFile(TCHAR* fileDirectory) {
  pcap_t* fp;
  char errbuf[PCAP_ERRBUF_SIZE];

  QString qstring = QString::fromLocal8Bit((const char*)fileDirectory);

  /* Open the capture file */
#ifdef UNICODE
  std::wstring strFileDirectory = fileDirectory;
  if ((fp = pcap_open_offline(
           qstring.toLocal8Bit()
               .constData() /*CW2A(fileDirectory)*/,  // name of the device
#else
  std::string strFileDirectory = (char *)fileDirectory;
  if ((fp = pcap_open_offline(strFileDirectory.c_str(),  // name of the device
#endif
           errbuf /*error buffer*/)) == NULL) {
    printf("\nUnable to open the file : %s.\n", strFileDirectory.c_str());
  } else {
    for (size_t i = 0U; i < m_vtReadPacketInfo.size(); i++) {
      delete m_vtReadPacketInfo[i].pData;
    }
    m_vtReadPacketInfo.clear();

    /* read and dispatch packets until EOF is reached */
    pcap_loop(fp, 0, dispatcher_handler,
              reinterpret_cast<u_char*>(&m_vtReadPacketInfo));
    pcap_close(fp);
  }
}

/**
 * @author  bieber
 * @brief   패킷정보 반환
 * @remarks
 * @param   vtPacketInfo
 * @return  void
 * @date    2022-04-20
 */
void CPCAPThrad::GetPacketInformation(std::vector<PACKET_INFO>& vtPacketInfo) {
  vtPacketInfo = m_vtReadPacketInfo;
}

void CPCAPThrad::run()
{
  // packet parser 

}
}  // namespace PCAPcomm


