#ifndef QLMPARSSER_H
#define QLMPARSSER_H

#include <QMap>
#include <QObject>

#include "../Common/Struct.h"
class RecvQLM;
class QLMParsser : public QObject {
  Q_OBJECT
 public:
  QLMParsser();
  ~QLMParsser();
  // void SetScenario(QVector<TDPSConfig> vTDPSConfig);
  void SetQLMRecv(QVector<TDPSConfig> vTDPSConfig);

 private:
  void SetRecvInit();
  AddPlatform SetPlatformAdd(UpdatePlatform stUpdatePlatform,
                             QString strCallSign);

 public Q_SLOTS:
  void slot_RecvQLMData(QByteArray buffer, int nSiteID, QString strCallSign);

 signals:
  void signal_AddPlatform(AddPlatform stAddPlatform);
  void signal_UpdatePlatform(UpdatePlatform stUpdatePlatform);

 private:
  QVector<RecvQLM*> m_vRecvQLM;
  QMap<int, AddPlatform> m_mapQLMData;

  double m_dTest;
  int m_nDispLimit;
  QMap<int, UpdatePlatform> m_mapLogData;  // DispLimit
};

#endif  // QLMPARSSER_H
