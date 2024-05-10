#include "datamngr.h"

#include <DCS.h>

#include <QDebug>

#include "../Common/Global.h"
#include "../Operation/MissionControl/dcslist.h"
#include "../Operation/MissionControl/tdpslist.h"
#include "../Operation/operationmain.h"
#include "dcsparsser.h"
#include "qlmparsser.h"
#include "simData/DataStore.h"
#include "simUtil/ExampleResources.h"
DataMngr::DataMngr(simData::DataStore& dataStore, OperationMain& operationMain)
    : m_dataStore(dataStore),
      m_operationMain(operationMain),
      m_pTDPSList(nullptr),
      m_pDCSList(nullptr),
      m_pQLMParsser(nullptr),
      m_pDCSParsser(nullptr),
      m_nTDPSDispLimit(0),
      m_nDCSDispLimit(0) {
  m_mapID.clear();
  m_mapUpdateData.clear();
  m_mapLogData.clear();

  SetTimer();
}

DataMngr::~DataMngr() {
  m_timer.stop();
  SetQLMParsserDelete();
  SetDCSParsserDelete();
  m_mapLogData.clear();
  m_mapID.clear();
}

void DataMngr::SetQLMParsserDelete() {
  if (m_pQLMParsser != nullptr) {
    disconnect(m_pQLMParsser, SIGNAL(signal_AddPlatform(AddPlatform)), this,
               SLOT(slot_AddPlatform(AddPlatform)));
    disconnect(m_pQLMParsser, SIGNAL(signal_UpdatePlatform(UpdatePlatform)),
               this, SLOT(slot_UpdatePlatform(UpdatePlatform)));

    disconnect(m_pQLMParsser, SIGNAL(signal_UpdatePlatform(UpdatePlatform)),
               m_pTDPSList, SLOT(slot_UpdateInfo(UpdatePlatform)));

    disconnect(m_pTDPSList, SIGNAL(signal_UpdatePref(int, PlatfromPref)), this,
               SLOT(slot_UpdatePref(int, PlatfromPref)));

    disconnect(m_pTDPSList, SIGNAL(signal_ReqestLogData(int)), this,
               SLOT(slot_ReqestLogData(int)));
    disconnect(this,
               SIGNAL(signal_ResponseQLMLogData(int, QVector<UpdatePlatform>)),
               m_pTDPSList,
               SLOT(slot_ResponseQLMLogData(int, QVector<UpdatePlatform>)));

    delete m_pQLMParsser;
    m_pQLMParsser = nullptr;
  }
}

void DataMngr::SetDCSParsserDelete() {
  if (m_pDCSParsser != nullptr) {
    disconnect(m_pDCSParsser, SIGNAL(signal_AddPlatform(AddPlatform)), this,
               SLOT(slot_AddPlatform(AddPlatform)));
    disconnect(m_pDCSParsser, SIGNAL(signal_UpdatePlatform(UpdatePlatform)),
               this, SLOT(slot_UpdatePlatform(UpdatePlatform)));

    disconnect(m_pDCSParsser, SIGNAL(signal_UpdatePlatform(UpdatePlatform)),
               m_pDCSList, SLOT(slot_UpdateInfo(UpdatePlatform)));

    disconnect(m_pDCSList, SIGNAL(signal_UpdatePref(int, PlatfromPref)), this,
               SLOT(slot_UpdatePref(int, PlatfromPref)));

    disconnect(m_pDCSList, SIGNAL(signal_ReqestLogData(int)), this,
               SLOT(slot_ReqestLogData(int)));
    disconnect(this,
               SIGNAL(signal_ResponseDCSLogData(int, QVector<UpdatePlatform>)),
               m_pDCSList,
               SLOT(slot_ResponseDCSLogData(int, QVector<UpdatePlatform>)));

    delete m_pDCSParsser;
    m_pDCSParsser = nullptr;
  }
}

void DataMngr::SetClearData() {
  m_mapPref.clear();
  m_mapUpdateData.clear();
  m_dataStore.clear();

  m_mapLogData.clear();
  m_mapID.clear();

  SetQLMParsserDelete();
  SetDCSParsserDelete();
}

void DataMngr::SetMissionStart(ScenarioInfo stScenarioInfo) {
  SetClearData();

  m_nTDPSDispLimit = stScenarioInfo.nTDPSDispLimit;
  m_nDCSDispLimit = stScenarioInfo.nDCSDispLimit;
  SetInitQLMParsser(stScenarioInfo.vTDPSConfig);
  SetInitDCSParsser(stScenarioInfo.vDCSConfig);
}

void DataMngr::SetMissionStop() {
  // 연결해제 및 데이터 삭제
  SetClearData();
}
/**
 * @brief DataMngr::SetTimer
 * 3차원 전시를 위한 데이터 갱신설정
 */
void DataMngr::SetTimer() {
  connect(&m_timer, SIGNAL(timeout()), this, SLOT(slot_TimeOut()));
  // m_timer.start(1000);
  m_timer.start(100);
}

/**
 * @brief DataMngr::SetInitQLMParsser
 * 데이터 파서 연결설정
 * QLM, DCS...
 */
void DataMngr::SetInitQLMParsser(QVector<TDPSConfig> vTDPSConfig) {
  // 시나리오 Preferance 정보 설정
  for (auto itt : vTDPSConfig) {
    PlatfromPref stPlatfromPref;
    stPlatfromPref.strTrackType = itt.strTrackType;
    stPlatfromPref.strTrackColor = itt.strTrackColor;
    stPlatfromPref.nTrackLength = itt.nTrackLength;
    stPlatfromPref.nTrackWidth = itt.nTrackWidth;
    stPlatfromPref.bDynamic = itt.bDynamic;
    m_mapPref.insert(itt.nSiteID, stPlatfromPref);
  }

  // QLM Parsser로 시나리오 정보 전송
  if (m_pQLMParsser == nullptr) {
    m_pQLMParsser = new QLMParsser();

    m_pQLMParsser->SetQLMRecv(vTDPSConfig);

    // QLM Data 3D Display Add And Update
    connect(m_pQLMParsser, SIGNAL(signal_AddPlatform(AddPlatform)), this,
            SLOT(slot_AddPlatform(AddPlatform)));
    connect(m_pQLMParsser, SIGNAL(signal_UpdatePlatform(UpdatePlatform)), this,
            SLOT(slot_UpdatePlatform(UpdatePlatform)));

    // QLM Data Status Display Update
    m_pTDPSList = m_operationMain.GetTDPSList();

    if (m_pTDPSList != nullptr) {
      // 상태정보 갱신용
      connect(m_pQLMParsser, SIGNAL(signal_UpdatePlatform(UpdatePlatform)),
              m_pTDPSList, SLOT(slot_UpdateInfo(UpdatePlatform)));

      // 플랫폼 표시설정
      connect(m_pTDPSList, SIGNAL(signal_UpdatePref(int, PlatfromPref)), this,
              SLOT(slot_UpdatePref(int, PlatfromPref)));

      // 로그정보
      connect(m_pTDPSList, SIGNAL(signal_ReqestLogData(int)), this,
              SLOT(slot_ReqestLogData(int)));
      connect(this,
              SIGNAL(signal_ResponseQLMLogData(int, QVector<UpdatePlatform>)),
              m_pTDPSList,
              SLOT(slot_ResponseQLMLogData(int, QVector<UpdatePlatform>)));
    }
  }
}

void DataMngr::SetInitDCSParsser(QVector<DCSConfig> vDCSConfig) {
  for (auto itt : vDCSConfig) {
    PlatfromPref stPlatfromPref;
    stPlatfromPref.strTrackType = itt.strTrackType;
    stPlatfromPref.strTrackColor = itt.strTrackColor;
    stPlatfromPref.nTrackLength = itt.nTrackLength;
    stPlatfromPref.nTrackWidth = itt.nTrackWidth;
    stPlatfromPref.bDynamic = itt.bDynamic;
    m_mapPref.insert(itt.nSiteID, stPlatfromPref);
  }

  if (m_pDCSParsser == nullptr) {
    m_pDCSParsser = new DCSParsser();

    m_pDCSParsser->SetDCSRecv(vDCSConfig);

    // QLM Data 3D Display Add And Update
    connect(m_pDCSParsser, SIGNAL(signal_AddPlatform(AddPlatform)), this,
            SLOT(slot_AddPlatform(AddPlatform)));
    connect(m_pDCSParsser, SIGNAL(signal_UpdatePlatform(UpdatePlatform)), this,
            SLOT(slot_UpdatePlatform(UpdatePlatform)));

    // DCS Data Status Display Update
    m_pDCSList = m_operationMain.GetDCSList();

    if (m_pDCSList != nullptr) {
      // 상태정보 갱신용
      connect(m_pDCSParsser, SIGNAL(signal_UpdatePlatform(UpdatePlatform)),
              m_pDCSList, SLOT(slot_UpdateInfo(UpdatePlatform)));

      // 플랫폼 표시설정
      connect(m_pDCSList, SIGNAL(signal_UpdatePref(int, PlatfromPref)), this,
              SLOT(slot_UpdatePref(int, PlatfromPref)));

      // 로그정보
      connect(m_pDCSList, SIGNAL(signal_ReqestLogData(int)), this,
              SLOT(slot_ReqestLogData(int)));
      connect(this,
              SIGNAL(signal_ResponseDCSLogData(int, QVector<UpdatePlatform>)),
              m_pDCSList,
              SLOT(slot_ResponseDCSLogData(int, QVector<UpdatePlatform>)));
    }
  }
}

/**
 * @brief DataMngr::slot_AddPlatform
 * @param stAddPlatform
 * Passer에 연결되는 SIGNAL/SLOT
 * 초기정보(DCS-Header, QLM 최초입력정보)설정
 * Platform Infor 정보 구성
 */
void DataMngr::slot_AddPlatform(AddPlatform stAddPlatform) {
  // create the platform

  auto itt = m_mapID.find(stAddPlatform.nID);
  if (itt == m_mapID.end()) {
    qDebug() << "Add Platform ID : " << stAddPlatform.nID
             << " Site : " << stAddPlatform.nSiteID;
    int hostId = 0;  // SIMDIS ID
    simData::DataStore::Transaction xaction;
    simData::PlatformProperties* props = m_dataStore.addPlatform(&xaction);
    hostId = props->id();
    xaction.complete(&props);

    // configure initial preferences
    // simData::DataStore::Transaction xaction;
    simData::PlatformPrefs* prefs =
        m_dataStore.mutable_platformPrefs(hostId, &xaction);

    prefs->set_icon(stAddPlatform.strIconName.toLocal8Bit().data());
    prefs->set_scale(1.0);  // large so we can see it

    auto ittfind = m_mapPref.find(stAddPlatform.nSiteID);
    if (ittfind != m_mapPref.end()) {
      if (ittfind.value().bTrack) {
        prefs->mutable_trackprefs()->set_tracklength(
            ittfind.value().nTrackLength);
        prefs->mutable_trackprefs()->set_linewidth(ittfind.value().nTrackWidth);

        unsigned int unColor = GetTrackColor(ittfind.value().strTrackColor);
        prefs->mutable_trackprefs()->set_trackcolor(unColor);
        prefs->mutable_trackprefs()->set_trackoverridecolor(unColor);
        prefs->mutable_trackprefs()->set_usetrackoverridecolor(true);

        prefs->mutable_trackprefs()->set_trackdrawmode(
            static_cast<simData::TrackPrefs_Mode>(
                GetTrackType(ittfind.value().strTrackType)));
      } else {
        prefs->mutable_trackprefs()->set_trackdrawmode(
            simData::TrackPrefs_Mode_OFF);
      }
      // dynamic
      prefs->set_dynamicscale(ittfind.value().bDynamic);

      // label
      prefs->mutable_commonprefs()->mutable_labelprefs()->set_draw(
          ittfind.value().bLabel);
      prefs->mutable_commonprefs()->mutable_labelprefs()->set_offsety(-10);
      prefs->mutable_commonprefs()
          ->mutable_labelprefs()
          ->set_overlayfontpointsize(10);
      prefs->mutable_commonprefs()->mutable_labelprefs()->set_backdroptype(
          simData::BDT_SHADOW_BOTTOM_RIGHT);
      prefs->mutable_commonprefs()->mutable_labelprefs()->set_textoutline(
          simData::TO_THICK);

      // draw
      prefs->mutable_commonprefs()->set_draw(ittfind.value().bDraw);
    }

    prefs->mutable_commonprefs()->set_name(
        stAddPlatform.strCallSign.toLocal8Bit().data());

    // prefs->mutable_commonprefs()->clear_datalimittime();
    // prefs->mutable_commonprefs()->set_datalimitpoints(2000);
    //  qDebug() << "SetName : " <<
    //  stAddPlatform.strCallSign.toLocal8Bit().data();

    xaction.complete(&prefs);

    m_mapID.insert(stAddPlatform.nID, hostId);
  }
}

/**
 * @brief DataMngr::slot_UpdatePref
 * @param nID
 * @param stPlatfromPref
 * 트랙 전시 설정 변경
 */
void DataMngr::slot_UpdatePref(int nID, PlatfromPref stPlatfromPref) {
  auto ittfind = m_mapID.find(nID);
  if (ittfind != m_mapID.end()) {
    simData::DataStore::Transaction xaction;
    simData::PlatformPrefs* prefs =
        m_dataStore.mutable_platformPrefs(ittfind.value(), &xaction);

    if (stPlatfromPref.bTrack) {
      // track route size
      prefs->mutable_trackprefs()->set_tracklength(stPlatfromPref.nTrackLength);
      prefs->mutable_trackprefs()->set_linewidth(stPlatfromPref.nTrackWidth);

      // track route color
      unsigned int unColor = GetTrackColor(stPlatfromPref.strTrackColor);
      prefs->mutable_trackprefs()->set_trackcolor(unColor);
      prefs->mutable_trackprefs()->set_trackoverridecolor(unColor);
      prefs->mutable_trackprefs()->set_usetrackoverridecolor(true);

      // track route type
      prefs->mutable_trackprefs()->set_trackdrawmode(
          static_cast<simData::TrackPrefs_Mode>(
              GetTrackType(stPlatfromPref.strTrackType)));
    } else {
      prefs->mutable_trackprefs()->set_trackdrawmode(
          simData::TrackPrefs_Mode_OFF);
    }

    // dynamic
    prefs->set_dynamicscale(stPlatfromPref.bDynamic);

    // draw
    prefs->mutable_commonprefs()->set_draw(stPlatfromPref.bDraw);

    // label
    prefs->mutable_commonprefs()->mutable_labelprefs()->set_draw(
        stPlatfromPref.bLabel);
    prefs->mutable_commonprefs()->mutable_labelprefs()->set_offsety(-10);
    prefs->mutable_commonprefs()
        ->mutable_labelprefs()
        ->set_overlayfontpointsize(10);
    prefs->mutable_commonprefs()->mutable_labelprefs()->set_backdroptype(
        simData::BDT_SHADOW_BOTTOM_RIGHT);
    prefs->mutable_commonprefs()->mutable_labelprefs()->set_textoutline(
        simData::TO_THICK);

    xaction.complete(&prefs);
  }
}

/**
 * @brief DataMngr::slot_UpdatePlatform
 * @param stUpdatePlatform
 * Passer에 열결되는 SIGNAL/SLOT
 * Platform Data 갱신
 */
void DataMngr::slot_UpdatePlatform(UpdatePlatform stUpdatePlatform) {
  m_mapUpdateData.insert(stUpdatePlatform.nID, stUpdatePlatform);
  // Update UI (InfoList)

  auto ittfind = m_mapLogData.find(stUpdatePlatform.nID);
  if (ittfind == m_mapLogData.end()) {
    QVector<UpdatePlatform> vData;
    vData.clear();
    vData.append(stUpdatePlatform);
    m_mapLogData.insert(stUpdatePlatform.nID, vData);
  } else {
    if (stUpdatePlatform.nSiteID < DCS_Site_ID) {
      if (ittfind.value().count() >= m_nTDPSDispLimit)  // QLM DispLimit
        ittfind.value().pop_front();
    } else {
      if (ittfind.value().count() >= m_nDCSDispLimit)  // DCS DispLimit
        ittfind.value().pop_front();
    }
    ittfind.value().append(stUpdatePlatform);
  }
}

/**
 * @brief DataMngr::slot_TimeOut
 * 3차원 가시화 정보 업데이트
 */
void DataMngr::slot_TimeOut() {
  // Update 3D Display

  for (auto ittUpdate : m_mapUpdateData) {
    auto ittFind = m_mapID.find(ittUpdate.nID);
    // Add 된 Platform 정보 확인 및 SIMDIS Host ID 확인
    if (ittFind != m_mapID.end()) {
      int nHostID = ittFind.value();

      simData::DataStore::Transaction t;
      simData::PlatformUpdate* u = m_dataStore.addPlatformUpdate(nHostID, &t);

      // ECEF로 좌표변환(SIMDIS는 ECEF로 좌표설정)
      bool bD2R = false;
      if (ittUpdate.nSiteID < DCS_Site_ID) {
        bD2R = true;
      }
      auto lla = ittUpdate.gf_GetLLAPos(bD2R);
      simCore::Coordinate ecef;
      simCore::CoordinateConverter::convertGeodeticToEcef(lla, ecef);
      // recdData.setAcceleration()
      if (u != nullptr) {
        u->set_time(ittUpdate.time);
        u->set_x(ecef.x());
        u->set_y(ecef.y());
        u->set_z(ecef.z());
        u->set_psi(ecef.psi());
        u->set_theta(ecef.theta());
        u->set_phi(ecef.phi());
        t.complete(&u);
      }
    }
  }

  m_dataStore.update(SetTime());
}

void DataMngr::slot_ReqestLogData(int nID) {
  auto ittfind = m_mapLogData.find(nID);
  if (ittfind != m_mapLogData.end()) {
    if (nID < DCS_Site_ID)
      emit signal_ResponseQLMLogData(ittfind.value()[0].nSiteID,
                                     ittfind.value());
    else
      emit signal_ResponseDCSLogData(ittfind.value()[0].nSiteID,
                                     ittfind.value());
  }
}

unsigned int DataMngr::GetTrackColor(QString strColor) {
  unsigned int unColor = 0x000000ff;

  // rgba
  if (strColor.compare(TRACK_COLOR_CYAN) == 0) {
    unColor = 0x00ffffff;
  }

  if (strColor.compare(TRACK_COLOR_GREEN) == 0) {
    unColor = 0x00ff00ff;
  }

  if (strColor.compare(TRACK_COLOR_MAGENTA) == 0) {
    unColor = 0xff00ffff;
  }

  if (strColor.compare(TRACK_COLOR_YELLOW) == 0) {
    unColor = 0xffff00ff;
  }

  if (strColor.compare(TRACK_COLOR_RED) == 0) {
    unColor = 0xff0000ff;
  }

  if (strColor.compare(TRACK_COLOR_WHITE) == 0) {
    unColor = 0xffffffff;
  }

  if (strColor.compare(TRACK_COLOR_BLACK) == 0) {
    unColor = 0x000000ff;
  }

  return unColor;
}

int DataMngr::GetTrackType(QString strType) {
  int nType = 0;

  // rgba
  if (strType.compare(TRACK_TYPE_POINTS) == 0) {
    nType = 1;
  }

  if (strType.compare(TRACK_TYPE_LINE) == 0) {
    nType = 2;
  }

  if (strType.compare(TRACK_TYPE_RIBON) == 0) {
    nType = 3;
  }

  if (strType.compare(TRACK_TYPE_BRIDGE) == 0) {
    nType = 4;
  }

  return nType;
}
