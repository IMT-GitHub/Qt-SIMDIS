#include "datamngr.h"

#include <QDebug>

static const double dSampleLat = 36.220569;
static const double dSampleLon = 125.73585;

DataMngr::DataMngr() 
{

}

DataMngr::~DataMngr() {}

void DataMngr::InitData() {
  m_nDCSId = 0;
  m_mapADSBData.clear();
  m_mapNMEAData.clear();
}

void DataMngr::SetADSBData(ADSBParser::mode_s_msg data) {
  QString strHexID = QString("%1%2%3")
                         .arg(data.aa1, 0, 16)
                         .arg(data.aa2, 0, 16)
                         .arg(data.aa3, 0, 16);
  auto itt = m_mapADSBData.find(strHexID);

  if (itt != m_mapADSBData.end()) {
    itt->RecvTime = QDateTime::currentDateTime();

    if (data.flight[0] != 0x00) {
      itt->strCallSign = QString("%1").arg(data.flight);
    }

    if (data.altitude != 0) {
      itt->nAlt = data.altitude * UTILS::CU_FT2M;  // feet to meter
      itt->SetRawPos(data.fflag, data.raw_latitude, data.raw_longitude);
    }

    if (data.heading != 0) {
      itt->nHeading = data.heading;
    }

    // QString strLog = QString(
    //                      "Update data(%1) : callsing(%2) lat(%3) lon(%4) "
    //                      "alt(%5) heading(%6)")
    //                      .arg(itt->strHexID)
    //                      .arg(itt->strCallSign)
    //                      .arg(itt->Pos.dLat)
    //                      .arg(itt->Pos.dLon)
    //                      .arg(itt->nAlt)
    //                      .arg(itt->nHeading);

    //qDebug() << strLog;

    DCSPlatformData platdata;
    // DCS container for a platform object.  Specifies call sign, 3D model to display, initial position, etc.
    DCSPlatformHeader platheader;

   auto dcsiditt = m_mapADSBDCSID.find(strHexID);

   if(dcsiditt == m_mapADSBDCSID.end())
   {

      return;
   }

    auto ittDCS = m_mapDSCADSBData.find(*dcsiditt);

    if(ittDCS == m_mapDSCADSBData.end())
    {
      return;  
    }

    if(strlen(ittDCS->first.callsign_) <=0 && itt->strCallSign.length() > 0)
    {

      strcpy( ittDCS->first.callsign_, itt->strCallSign.toStdString().c_str());
      
      

    }

    if(itt->Pos.dLat > 35 && itt->Pos.dLon > 126 && ittDCS->first.interpolate_ == true )
    {
      ittDCS->first.referenceframe_.latorigin_ = itt->Pos.dLat * UTILS::CU_DEG2RAD ;  // ori_lat
      ittDCS->first.referenceframe_.lonorigin_ = itt->Pos.dLon * UTILS::CU_DEG2RAD;  // ori_lon
      ittDCS->first.referenceframe_.altorigin_ = itt->nAlt;  // itt->nAlt;

      ittDCS->first.platdata_ = ittDCS->second;

    }

    ittDCS->second.time_ = yeartime();
    // CreateUpdateData
    ittDCS->second.position_.set(itt->Pos.dLat * UTILS::CU_DEG2RAD, itt->Pos.dLon * UTILS::CU_DEG2RAD,
                            itt->nAlt);  // lla
    
    // Heading roll pich to euler 변환필요..
    ittDCS->second.orientation_.set(itt->nHeading * UTILS::CU_DEG2RAD, 0.0,
                              0.0);         // 자세
    ittDCS->second.velocity_.set(0.0, 0.0, 0.0);  // 속도
    ittDCS->second.accel_.set(0.0, 0.0, 0.0);     // 가속도
    
    
    m_mapDSCADSBData[ittDCS.key()] = std::make_pair(ittDCS->first, ittDCS->second);
  } 
  else  // add
  {
    
      mode_s_Decode dcsData;
      dcsData.strHexID = strHexID;
      dcsData.nDCSId = m_nDCSId;
      m_nDCSId++;

      dcsData.RecvTime = QDateTime::currentDateTime();

      if (data.flight[0] != 0x00) {
        dcsData.strCallSign = QString("%1").arg(data.flight);
      }

      if (data.altitude != 0) {
        dcsData.nAlt = data.altitude * UTILS::CU_FT2M;  // feet to meter

        dcsData.SetRawPos(data.fflag, data.raw_latitude, data.raw_longitude);
      }

      if (data.heading != 0) {
        dcsData.nHeading = data.heading;
      }
        
      DCSPlatformData platdata;
      // DCS container for a platform object.  Specifies call sign, 3D model to display, initial position, etc.
      DCSPlatformHeader platheader;

      // CreateUpdateData
      platdata.id_ = m_nDCSId;
      platdata.time_ = yeartime();
      platdata.position_.set(itt->Pos.dLat * UTILS::CU_DEG2RAD, itt->Pos.dLon * UTILS::CU_DEG2RAD,
                              itt->nAlt);  // lla
      
      // Heading roll pich to euler 변환필요..
      platdata.orientation_.set(itt->nHeading * UTILS::CU_DEG2RAD, 0.0,
                                0.0);         // 자세
      platdata.velocity_.set(0.0, 0.0, 0.0);  // 속도
      platdata.accel_.set(0.0, 0.0, 0.0);     // 가속도
      // CreateHeader data
      platheader.id_ = m_nDCSId;  // DCS id
      // platheader.iconname_
      strncpy(platheader.iconname_, "p-3c_orion_nrl",
              sizeof(platheader.iconname_) - 1);
      platheader.callsign_;  // = itt->strCallSign;

      if(itt->strCallSign.isNull())
        strncpy(platheader.callsign_, itt->strCallSign.toStdString().c_str(),
                sizeof(platheader.callsign_) - 1);
      else
        strncpy(platheader.callsign_ , "",1);

      platheader.dims_.set(0.0, 0.0, 0.0);
      platheader.offset_.set(0.0, 0.0, 0.0);
      platheader.orientationoffset_.set(0.0, 0.0, 0.0);
      platheader.referenceframe_.latorigin_ = itt->Pos.dLat * UTILS::CU_DEG2RAD ;  // ori_lat
      platheader.referenceframe_.lonorigin_ = itt->Pos.dLon * UTILS::CU_DEG2RAD;  // ori_lon
      platheader.referenceframe_.altorigin_ = itt->nAlt;  // itt->nAlt;
      platheader.referenceframe_.coordsystem_ = DCSCOORD_LLA;
      platheader.color_ = 0;
      platheader.interpolate_ = true;

      
      m_mapADSBData.insert(strHexID, dcsData);
      m_mapADSBDCSID.insert(strHexID, m_nDCSId);
      m_mapDSCADSBData.insert((KDIS::KUINT16)m_nDCSId, std::make_pair(platheader, platdata));
      
  }
}

/**
 * @brief SetAISData  Ship GPS Infomation DCSPlatform type Save 
 * 
 * @param data : AIS 파서로 변경된 데이터 
 */
void DataMngr::SetAISData(LTSS_Parsser::AISPlatformData data)
{

  if(data.identity == 0 )
    return ;
  DCSPlatformData platdata;
  // DCS container for a platform object.  Specifies call sign, 3D model to display, initial position, etc.
  DCSPlatformHeader platheader;
  
  
  auto ittid = m_mapAIS_DCSID.find(data.identity);
  if(ittid == m_mapAIS_DCSID.end())
  {
    // dcs id plus
    m_nDCSId++;

    // CreateUpdateData
    platdata.id_ = m_nDCSId;
    platdata.time_ = yeartime();
    platdata.position_.set(data.Lat * UTILS::CU_DEG2RAD, data.Lon * UTILS::CU_DEG2RAD,
                            data.alt);  // lla
    
    // Heading roll pich to 
    platdata.orientation_.set(data.Heading * UTILS::CU_DEG2RAD, 0.0,
                              0.0);         // 자세
    platdata.velocity_.set(0.0, 0.0, 0.0);  // 속도
    platdata.accel_.set(0.0, 0.0, 0.0);     // 가속도
    // CreateHeader data
    platheader.id_ = m_nDCSId;  // DCS id
    // platheader.iconname_
    strncpy(platheader.iconname_, "boghammar",
            sizeof(platheader.iconname_) - 1);
    platheader.callsign_;  // = itt->strCallSign;

    if(strlen(data.callSige) )
      strncpy(platheader.callsign_, data.callSige, sizeof(data.callSige)) ;
    else
      strncpy(platheader.callsign_ , "", 1);

    platheader.dims_.set(0.0, 0.0, 0.0);
    platheader.offset_.set(0.0, 0.0, 0.0);
    platheader.orientationoffset_.set(0.0, 0.0, 0.0);
    platheader.referenceframe_.latorigin_ = data.Lat * UTILS::CU_DEG2RAD; // ori_lat
    platheader.referenceframe_.lonorigin_ =  data.Lon * UTILS::CU_DEG2RAD;  // ori_lon
    platheader.referenceframe_.altorigin_ = 0;  // itt->nAlt;
    platheader.referenceframe_.coordsystem_ = DCSCOORD_LLA;
    platheader.color_ = 0;
    platheader.interpolate_ = true;

    platheader.platdata_ = platdata;

    m_mapAIS_DCSID.insert(data.identity, m_nDCSId);
    m_mapAISData.insert((KDIS::KUINT16)m_nDCSId, std::make_pair(platheader, platdata));
  }
  else  // upate platform  data
  {
    auto ittdata = m_mapAISData.value(ittid.value());

    if(ittdata.first.id_)
    {
      platdata = ittdata.second;
      platdata.time_ = yeartime();
      platdata.position_.set(data.Lat * UTILS::CU_DEG2RAD, data.Lon * UTILS::CU_DEG2RAD,
                            data.alt);  // lla
    
      // Heading roll pich to 
      platdata.orientation_.set(data.Heading * UTILS::CU_DEG2RAD, 0.0,
                              0.0);         // 자세
      platdata.velocity_.set(0.0, 0.0, 0.0);  // 속도
      platdata.accel_.set(0.0, 0.0, 0.0);     // 가속도

      m_mapAISData[ittid.value()].second = platdata;
    }    
  }
}


/**
 * @brief 
 * 
 * @return QMap<int, std::pair<DCSPlatformHeader, DCSPlatformData>> 
 */
QMap<int, std::pair<DCSPlatformHeader, DCSPlatformData>>  DataMngr::GetADSBData() 
{ 
      return m_mapDSCADSBData; 
  
}

void DataMngr::SetDISData(KDIS::PDU::Entity_State_PDU* Entity) {
  auto entityID =
      Entity->GetEntityIdentifier()
          .GetEntityID();  // Entity->GetEntityIdentifier().GetEntityID();
  auto itt = m_mapDISData.find(entityID);
  DCSPlatformData data;
  data.id_ =
      Entity->GetEntityIdentifier().GetEntityID() + 1000;  // itt->nDCSId;
  data.time_ = yeartime();

  // KFLOAT64 Lat = 40.664, Lon = -122.63, Alt = 615;  // West coast of
  // USA
  KDIS::KFLOAT64 Lat = 0, Lon = 0, Alt = 0;  // West coast of USA
  KDIS::UTILS::GeocentricToGeodetic(
      Entity->GetEntityLocation().GetX(), Entity->GetEntityLocation().GetY(),
      Entity->GetEntityLocation().GetZ(), Lat, Lon, Alt, KDIS::UTILS::WGS_1984);

  data.position_.set(KDIS::UTILS::DegToRad(Lat), KDIS::UTILS::DegToRad(Lon),
                     Alt);  // lla

  
  // Heading roll pich to euler 변환필요..
  data.orientation_.set(
      Entity->GetEntityOrientation().GetPsiInRadians(),
      Entity->GetEntityOrientation().GetThetaInRadians(),
      Entity->GetEntityOrientation().GetPhiInRadians());  // 자세

  data.velocity_.set(0.0, 0.0, 0.0);  // 속도
  data.accel_.set(0.0, 0.0, 0.0);     // 가속도

  if (itt != m_mapDISData.end()) {
    // itt->second.SetEntityLocation(Entity->GetEntityLocation());
    itt.value().second = data;

  } else {
    // m_mapDISData2.insert(entityID, *Entity);
    DCSPlatformHeader header;
    memset(&header, 0, sizeof(header));
    header.id_ = Entity->GetEntityIdentifier().GetEntityID() + 1000;  // DCS id
    // header.iconname_
    strncpy(header.iconname_, "p-3c_orion_nrl", sizeof(header.iconname_) - 1);
    header.callsign_;  // = itt->strCallSign;
    // strncpy(header.callsign_,
    // Entity->GetEntityMarking().GetAsString().c_str(),
    //         sizeof(header.callsign_) - 1);

    header.dims_.set(0.0, 0.0, 0.0);
    header.offset_.set(0.0, 0.0, 0.0);
    header.orientationoffset_.set(0.0, 0.0, 0.0);

    KDIS::UTILS::GeocentricToGeodetic(Entity->GetEntityLocation().GetX(),
                                      Entity->GetEntityLocation().GetY(),
                                      Entity->GetEntityLocation().GetZ(), Lat,
                                      Lon, Alt, KDIS::UTILS::WGS_1984);

    header.referenceframe_.latorigin_ = KDIS::UTILS::DegToRad(Lat);  // ori_lat
    header.referenceframe_.lonorigin_ = KDIS::UTILS::DegToRad(Lon);  // ori_lon
    header.referenceframe_.altorigin_ = Alt;  // itt->nAlt;
    header.referenceframe_.coordsystem_ = DCSCOORD_LLA;
    header.color_ = 0;

    m_mapDISData.insert(entityID, std::make_pair(header, data));
  }
}

QMap<int, std::pair<DCSPlatformHeader, DCSPlatformData>>
DataMngr::GetDISData() {
  return m_mapDISData;
}

// void DataMngr::SetNMEA0183Data(NMEA0183POS stNMEA0183) {
//   // 우선 NMEA0183은 단일로 취급, Header 정보때문에 확인
//   int nID = 4000;

//   DCSPlatformData data;
//   data.id_ = nID;
//   data.time_ = yeartime();
//   data.position_.set(KDIS::UTILS::DegToRad(stNMEA0183.flat),
//                      KDIS::UTILS::DegToRad(stNMEA0183.flon),
//                      0);  // lla

//   data.orientation_.set(KDIS::UTILS::DegToRad(stNMEA0183.fcourse), 0.0, 0.0);
//   data.velocity_.set(stNMEA0183.fspeed, 0.0,
//                      0.0);  // 속도값만 알고있음, 경로, 속도로 계산은 가능할듯
//   data.accel_.set(0.0, 0.0, 0.0);
//   auto itt = m_mapNMEAData.find(nID);
//   if (itt != m_mapNMEAData.end()) {
//     // Data Update
//     itt.value().second = data;
//   } else {
//     // 최초, Header 입력
//     DCSPlatformHeader header;
//     memset(&header, 0, sizeof(header));
//     header.id_ = nID;
//     strncpy(header.iconname_, "p-3c_orion_nrl", sizeof(header.iconname_) - 1);
//     header.callsign_;  // = itt->strCallSign;
//     header.dims_.set(0.0, 0.0, 0.0);
//     header.offset_.set(0.0, 0.0, 0.0);
//     header.orientationoffset_.set(0.0, 0.0, 0.0);

//     header.referenceframe_.latorigin_ =
//         KDIS::UTILS::DegToRad(stNMEA0183.flat);  // ori_lat
//     header.referenceframe_.lonorigin_ =
//         KDIS::UTILS::DegToRad(stNMEA0183.flon);  // ori_lon
//     header.referenceframe_.altorigin_ = 0;       // itt->nAlt;
//     header.referenceframe_.coordsystem_ = DCSCOORD_LLA;
//     m_mapNMEAData.insert(nID, std::make_pair(header, data));
//   }
// }

// QMap<int, std::pair<DCSPlatformHeader, DCSPlatformData>>
// DataMngr::GetNMEA0183Data() {
//   return m_mapNMEAData;
// }

double DataMngr::yeartime() {
  struct timeval tp;

  // get the current system time, using timezone value of 0
  // returns UTC time
  gettimeofday(&tp, 0);

  // put system time into a tm struct
  time_t t(tp.tv_sec);
  struct tm* p_time = gmtime(&t);

  // assemble a UTC "system time"
  unsigned int p_secs = (unsigned int)(p_time->tm_sec) +
                        (((unsigned int)(p_time->tm_min)) * 60) +
                        (((unsigned int)(p_time->tm_hour)) * 60 * 60) +
                        (((unsigned int)(p_time->tm_yday)) * 60 * 60 * 24);

  return (p_secs + (tp.tv_usec * 1e-06));
}


void DataMngr::SetSADSBSampleData()
{
  mode_s_Decode dcsData;
  dcsData.strHexID = "a0a0a0";
  dcsData.nDCSId = m_nDCSId;
  m_nDCSId++;

  dcsData.RecvTime = QDateTime::currentDateTime();

  dcsData.strCallSign ="aaaa";
  
  dcsData.nAlt = 100 * UTILS::CU_FT2M;  // feet to meter

  
  dcsData.nHeading = 123;
    
  DCSPlatformData platdata;
  // DCS container for a platform object.  Specifies call sign, 3D model to display, initial position, etc.
  DCSPlatformHeader platheader;

  // CreateUpdateData
  platdata.id_ = m_nDCSId;
  platdata.time_ = yeartime();
  platdata.position_.set(dSampleLat * UTILS::CU_DEG2RAD, dSampleLon * UTILS::CU_DEG2RAD, 100);  // lla
  
  // Heading roll pich to euler 변환필요..
  platdata.orientation_.set(123 * UTILS::CU_DEG2RAD, 0.0, 0.0);         // 자세

  platdata.velocity_.set(0.0, 0.0, 0.0);  // 속도

  platdata.accel_.set(0.0, 0.0, 0.0);     // 가속도

  // CreateHeader data
  platheader.id_ = m_nDCSId;  // DCS id

  // platheader.iconname_
  strncpy(platheader.iconname_, "p-3c_orion_nrl", sizeof(platheader.iconname_) - 1);

  platheader.callsign_;  // = itt->strCallSign;

  strncpy(platheader.callsign_ , "sample data",1);

  platheader.dims_.set(0.0, 0.0, 0.0);
  platheader.offset_.set(0.0, 0.0, 0.0);
  platheader.orientationoffset_.set(0.0, 0.0, 0.0);
  platheader.referenceframe_.latorigin_ = dSampleLon * UTILS::CU_DEG2RAD ;  // ori_lat
  platheader.referenceframe_.lonorigin_ = dSampleLon * UTILS::CU_DEG2RAD;  // ori_lon
  platheader.referenceframe_.altorigin_ = 0;  // itt->nAlt;
  platheader.referenceframe_.coordsystem_ = DCSCOORD_LLA;
  platheader.color_ = 0;
  platheader.interpolate_ = true;
  platheader.platdata_ = platdata;

  m_mapADSBData.insert(dcsData.strHexID, dcsData);
  m_mapADSBDCSID.insert(dcsData.strHexID, m_nDCSId);
  m_mapDSCADSBData.insert((KDIS::KUINT16)m_nDCSId, std::make_pair(platheader, platdata));
}

void DataMngr::SetSADSBPlatformPosition(int nKey, DCSPoint point)
{

  auto itt = m_mapDSCADSBData.value(nKey);

  itt.second.position_ =  point;

  m_mapDSCADSBData[nKey].second = itt.second;

}

void DataMngr::SetInterpolate(int nKey, bool bInterpolate)
{
  auto itt =  m_mapDSCADSBData.value(nKey);

  if(itt.first.id_)
  {
    itt.first.interpolate_ = bInterpolate;
    //auto ittreturn =  m_mapDSCADSBData.value(nKey, itt);
    m_mapDSCADSBData[nKey].first = itt.first;

    //qDebug() << "itter interpolate " <<itt.first.interpolate_ << "aaa" <<  m_mapDSCADSBData[nKey].first.interpolate_;
  }


}


void DataMngr::SetInterpolateAIS(int nKey, bool bInterpolate)
{
  auto itt =  m_mapAISData.value(nKey);

  if(itt.first.id_)
  {
    itt.first.interpolate_ = bInterpolate;
    //auto ittreturn =  m_mapDSCADSBData.value(nKey, itt);
    m_mapAISData[nKey].first = itt.first;

    qDebug() << "itter interpolate " <<itt.first.interpolate_ << "aaa" <<  m_mapDSCADSBData[nKey].first.interpolate_;
  }


}