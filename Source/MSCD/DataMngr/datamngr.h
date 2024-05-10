#ifndef DATAMNGR_H
#define DATAMNGR_H

#include <QMap>
#include <QObject>
// #include <QPair>
#include <QThread>
#include <QTimer>
#include <QVector>

#include "../Common/Struct.h"

namespace simData {
class DataStore;
}

class TDPSList;
class DCSList;
class OperationMain;
class QLMParsser;
class DCSParsser;
class DataMngr : public QObject {
  Q_OBJECT
 public:
  DataMngr(simData::DataStore& dataStore, OperationMain& operationMain);
  ~DataMngr();
  void SetMissionStart(ScenarioInfo stScenarioInfo);
  void SetMissionStop();

 private:
  void SetInitQLMParsser(QVector<TDPSConfig> vTDPSConfig);
  void SetInitDCSParsser(QVector<DCSConfig> vDCSConfig);

  void SetTimer();
  int GetTrackType(QString strType);
  unsigned int GetTrackColor(QString strColor);

  void SetQLMParsserDelete();
  void SetDCSParsserDelete();

  void SetClearData();

 public Q_SLOTS:
  void slot_AddPlatform(AddPlatform stAddPlatform);
  void slot_UpdatePlatform(UpdatePlatform stUpdatePlatform);
  // void slot_UpdatePrefs(PlatfromPref stPlatfromPref);
  void slot_TimeOut();
  void slot_UpdatePref(int nID, PlatfromPref stPlatfromPref);
  void slot_ReqestLogData(int nID);

 signals:
  void signal_ResponseDCSLogData(int nSiteID, QVector<UpdatePlatform> vLogData);
  void signal_ResponseQLMLogData(int nSiteID, QVector<UpdatePlatform> vLogData);

 private:
  QTimer m_timer;

  QLMParsser* m_pQLMParsser;
  DCSParsser* m_pDCSParsser;
  QMap<int, int> m_mapID;  // Key : Recv ID, Value : SIMDIS HostID
  QMap<int, UpdatePlatform>
      m_mapUpdateData;  // Key : Recv ID, Value : UpdateData

  // SIMDIS
  simData::DataStore& m_dataStore;  ///< The DataStore that hold all the data
  OperationMain& m_operationMain;
  TDPSList* m_pTDPSList;
  DCSList* m_pDCSList;
  // 시나리오를 통한 초기 전시정보
  QMap<int, PlatfromPref> m_mapPref;

  // DispLimit에 따른 Log 정보
  QMap<int, QVector<UpdatePlatform>> m_mapLogData;
  int m_nTDPSDispLimit;
  int m_nDCSDispLimit;
};

#endif  // DATAMNGR_H
