#ifndef DATAMNGR_H
#define DATAMNGR_H

#include <QMap>
#include <QMutex>>
#include <QObject>

#include "../KDIS/KDIS/PDU/Entity_Info_Interaction/Entity_State_PDU.h"
#include "Constants/InterfaceType.h"
#include "adsbparser.h"
#include "parsser.h"
#include "DCS.h"

class DataMngr : public QObject {
  Q_OBJECT;

 public:
  DataMngr();
  ~DataMngr();
  void InitData();

  //QMap<QString, mode_s_Decode> GetADSBData();
  QMap<int, std::pair<DCSPlatformHeader, DCSPlatformData>> GetADSBData();
  QMap<int, std::pair<DCSPlatformHeader, DCSPlatformData>> GetDISData();
  QMap<int, std::pair<DCSPlatformHeader, DCSPlatformData>> GetNMEA0183Data();
  QMap<int, std::pair<DCSPlatformHeader, DCSPlatformData>> GetAISData(){ return m_mapAISData; };

  void SetSADSBSampleData();
  
  void SetSADSBPlatformPosition(int nKey, DCSPoint point);

  void SetInterpolate(int nKey, bool bInterpolate);
  void SetInterpolateAIS(int nKey, bool bInterpolate);

 private:
  double yeartime();

 public Q_SLOTS:
  void SetADSBData(ADSBParser::mode_s_msg data);
  void SetDISData(KDIS::PDU::Entity_State_PDU *Entity);
  void SetAISData(LTSS_Parsser::AISPlatformData data);
  //void SetNMEA0183Data(NMEA0183POS stNMEA0183);
  // DCSPlatformHeader platheader;

 private:

  DCSHeaderContainer dcsheaders_;
  QMap<QString, mode_s_Decode>      m_mapADSBData;  
  QMap<QString, int>  m_mapADSBDCSID;
  QMap<int, std::pair<DCSPlatformHeader, DCSPlatformData>> m_mapDSCADSBData;

  QMap<int, KDIS::PDU::Entity_State_PDU> m_mapDISData2;
  
  QMap<int, int> m_mapAIS_DCSID;
  QMap<int, std::pair<DCSPlatformHeader, DCSPlatformData>> m_mapAISData;
  QMap<int, std::pair<DCSPlatformHeader, DCSPlatformData>> m_mapDISData;
  QMap<int, std::pair<DCSPlatformHeader, DCSPlatformData>> m_mapNMEAData;

  int m_nDCSId;

 public:
  QMutex m_DISMutex;
};

#endif  // DATAMNGR_H
