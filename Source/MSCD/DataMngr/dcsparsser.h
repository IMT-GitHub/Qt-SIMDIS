#ifndef DCSPARSSER_H
#define DCSPARSSER_H

#include <DCS.h>

#include <QMap>
#include <QObject>
#include <QThread>

#include "../Common/Struct.h"

class RecvDCS;
class DCSParsser : public QObject {
  Q_OBJECT
 public:
  DCSParsser();
  ~DCSParsser();
  void SetDCSRecv(QVector<DCSConfig> vDCSConfig);

 public Q_SLOTS:
  void slot_RecvDCSData(DCSBaseData* data, int nSiteID);

 signals:
  void signal_AddPlatform(AddPlatform stAddPlatform);
  void signal_UpdatePlatform(UpdatePlatform stUpdatePlatform);

 private:
  void SetRecvInit();
  void SetPlatformHeader(DCSPlatformHeader* data, int nSiteID);
  void SetPlatformData(DCSPlatformData* data, int nSiteID);

 private:
  QVector<QPair<RecvDCS*, QThread*>> m_vRecvDCS;
  QMap<int, DCSBaseData*> m_mapDCSData;
  QMap<int, AddPlatform> m_mapHeaderData;
  int m_nDispLimit;
  QMap<int, UpdatePlatform> m_mapLogData;  // DispLimit
};

#endif  // DCSPARSSER_H
