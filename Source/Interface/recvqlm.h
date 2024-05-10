#ifndef RECVQLM_H
#define RECVQLM_H

#include <QObject>
#include <QUdpSocket>

class RecvQLM : public QObject {
  Q_OBJECT;

  typedef struct _QLMHeader {
    char temp1[4];
    int nDataSize; 
    char temp2[4];
    char temp3[4];

    _QLMHeader() {
      memset(&temp1, 0, 4);
      nDataSize = 0;
      memset(&temp2, 0, 4);
      memset(&temp3, 0, 4);
    }
  } QLMHeader;

 public:
  RecvQLM(QHostAddress ip, int port, int nSiteID = 0, QString strCallSign = "");
  ~RecvQLM();

 private:
  int m_nSiteID;
  QString m_strCallSign;
  QUdpSocket *m_pSocket;

 private:
  void SetInit(QHostAddress ip, int port);

 signals:
  void signal_RecvQLMData(QByteArray buffer, int nSiteID, QString strCallSign);

 private Q_SLOTS:
  void SetRecvQLM();
};

#endif  // RECVQLM_H
