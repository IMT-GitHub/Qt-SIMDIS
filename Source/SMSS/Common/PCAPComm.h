#pragma once

#include <QString>
#include <QThread>
#include <functional>
#include <vector>

#include "pcap.h"

#ifdef WIN32

#else
  #define UINT32 unsigned int
  #define TCHAR unsigned char
  #define UCHAR unsigned char
  #define USHORT unsigned short
  #define UINT unsigned int
  #define BYTE unsigned char
  #define CHAR char
  #define INT32 int
#endif




namespace PCAPcomm {
/* 4 bytes IP address */
typedef struct ip_address {
  UCHAR byte1;
  UCHAR byte2;
  UCHAR byte3;
  UCHAR byte4;
} ip_address;

/* IPv4 header */
typedef struct ip_header {
  UCHAR ver_ihl;          // Version (4 bits) + Internet header length (4 bits)
  UCHAR tos;              // Type of service
  USHORT tlen;            // Total length
  USHORT identification;  // Identification
  USHORT flags_fo;        // Flags (3 bits) + Fragment offset (13 bits)
  UCHAR ttl;              // Time to live
  UCHAR proto;            // Protocol
  USHORT crc;             // Header checksum
  ip_address saddr;       // Source address
  ip_address daddr;       // Destination address
  UINT op_pad;            // Option + Padding
} ip_header;

/* UDP header*/
typedef struct udp_header {
  USHORT sport;  // Source port
  USHORT dport;  // Destination port
  USHORT len;    // Datagram length
  USHORT crc;    // Checksum
} udp_header;

typedef std::function<void(const struct pcap_pkthdr* header, const UCHAR*)>
    CALLBACK_PCAP_FUNC;

typedef struct PACKET_INFO {
  UINT uLength;
  ip_address SourceAddress;
  USHORT uSourcePort;
  ip_address DestinationAddress;
  USHORT uDestinationPort;
  pcap_pkthdr pcapHeader;
  BYTE* pData;

  void Initialize() {
    uLength = 0U;
    SourceAddress.byte1 = 0U;
    SourceAddress.byte2 = 0U;
    SourceAddress.byte3 = 0U;
    SourceAddress.byte4 = 0U;
    uSourcePort = 0U;
    DestinationAddress.byte1 = 0U;
    DestinationAddress.byte2 = 0U;
    DestinationAddress.byte3 = 0U;
    DestinationAddress.byte4 = 0U;
    uDestinationPort = 0U;
    memset(&pcapHeader, 0x00, sizeof(pcap_pkthdr));
    pData = NULL;
  }

  BYTE* GetBody() { return pData + 42U; }
};

typedef struct PROCESS_INFO {
  CALLBACK_PCAP_FUNC callbackPcapFunction;
  pcap_dumper_t* pDumpfile;

  void Initialize() {
    callbackPcapFunction = NULL;
    pDumpfile = NULL;
  }
};

class CPCAPThrad : public QThread
{
    Q_OBJECT;

  public:
    CPCAPThrad();
    ~CPCAPThrad();

  // Attributes
  private:
    UINT32 m_uBODY_INTERVAL;
    UINT32 m_uNetMask;
    pcap_t* m_pAdhandle;
    pcap_if_t m_deviceInfo;
    pcap_dumper_t* m_pDumpFile;
    std::vector<PACKET_INFO> m_vtReadPacketInfo;
    CALLBACK_PCAP_FUNC m_callbackPcapFunction;
  #ifdef UNICODE
    std::wstring m_strFilter;
    std::wstring m_strDumpName;
  #else
    std::string m_strFilter;
    std::string m_strDumpName;
  #endif  // UNICODE

  // Functions
  private:
    bool LoadNpcapDlls();


  protected:
    virtual void run() override;

  public:
    void Initialize();
    void Destroy();

    CPCAPThrad* GetPtr();
    void GetFindAllDevice(std::vector<pcap_if_t>& vtDeviceInfo);
    bool SelectDevice(pcap_if_t deviceInfo);
    pcap_if_t GetDeviceInformation();
    TCHAR* GetFilterInformation();
    TCHAR* GetDumpName();

    bool SetFilter(TCHAR* filter);
    void SetCallbackFunction(CALLBACK_PCAP_FUNC func);
    bool StartCapture();
    void FinishCapture();

    bool CreateDumpFile(TCHAR* fileName);
    bool CreateDumpFile(QString fileName);
    void ReadDumpFile(TCHAR* fileName);
    void GetPacketInformation(std::vector<PACKET_INFO>& vtPacketInfo);

  signals:
    bool signal_Send(int nSite, QByteArray buffer);


  };
}  // namespace PCAPcomm
