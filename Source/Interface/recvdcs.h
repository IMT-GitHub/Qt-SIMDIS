#ifndef RECVDCS_H
#define RECVDCS_H

#include <stdio.h>

#include <QObject>
#include <QTimer>
#include <sstream>

#include "DCS.h"

class RecvDCS : public QObject {
  Q_OBJECT

 public:
  RecvDCS(QString strIP = "", int nPort = 0, int nSiteID = 0);
  ~RecvDCS();

 public Q_SLOTS:
  void start();
  void stop();

 Q_SIGNALS:
  void signal_RecvDCSData(DCSBaseData* data, int nSiteID);
  void finished();

 private:
  bool SetClient();

 private:
  int m_nPort;
  QString m_strIP;
  bool m_bThread;
  bool m_bRecv;
  int m_nSiteID;

  DCSDisc::DCSDiscover discover;
  DCSCom::DCSClientInfo info;
  DCSTimeHeader time;
  DCSScenarioHeader scenario;
  DCSCom::HeaderList headers;
  DCSBaseData* data;
};

#endif  // RECVDCS_H
