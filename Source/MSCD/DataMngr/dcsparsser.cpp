#include "dcsparsser.h"

#include "../../Interface/recvdcs.h"
#include "simUtil/ExampleResources.h"
DCSParsser::DCSParsser() : m_nDispLimit(0) {
  SetRecvInit();
  m_mapHeaderData.clear();
}

DCSParsser::~DCSParsser() { SetRecvInit(); }

void DCSParsser::SetRecvInit() {
  for (auto itt : m_vRecvDCS) {
    if (itt.first != nullptr && itt.second != nullptr) {
      itt.first->stop();

      disconnect(itt.second, SIGNAL(started()), itt.first, SLOT(start()));
      disconnect(itt.first, SIGNAL(finished()), itt.second, SLOT(quit()));
      disconnect(itt.first, SIGNAL(finished()), itt.first, SLOT(deleteLater()));
      disconnect(itt.second, SIGNAL(finished()), itt.second,
                 SLOT(deleteLater()));

      disconnect(itt.first, SIGNAL(signal_RecvDCSData(DCSBaseData*, int)), this,
                 SLOT(slot_RecvDCSData(DCSBaseData*, int)));
    }

    itt.second->quit();
    itt.second->wait();

    delete itt.first;
    itt.first = nullptr;

    delete itt.second;
    itt.second = nullptr;
  }

  m_vRecvDCS.clear();
  m_mapDCSData.clear();
  m_mapLogData.clear();
  m_mapHeaderData.clear();
}

void DCSParsser::SetDCSRecv(QVector<DCSConfig> vDCSConfig) {
  SetRecvInit();

  // 시나리오 파일을 통해 DCS 갯수 설정
  for (auto itt : vDCSConfig) {
    RecvDCS* pRecvDCS = new RecvDCS(itt.strTCPIP, itt.nTCPPort, itt.nSiteID);

    // Thread 설정
    QThread* pThread = new QThread;
    pRecvDCS->moveToThread(pThread);
    connect(pThread, SIGNAL(started()), pRecvDCS, SLOT(start()));
    connect(pRecvDCS, SIGNAL(finished()), pThread, SLOT(quit()));
    connect(pRecvDCS, SIGNAL(finished()), pRecvDCS, SLOT(deleteLater()));
    connect(pThread, SIGNAL(finished()), pThread, SLOT(deleteLater()));

    // 데이터 수신 처리
    connect(pRecvDCS, SIGNAL(signal_RecvDCSData(DCSBaseData*, int)), this,
            SLOT(slot_RecvDCSData(DCSBaseData*, int)));

    pThread->start();

    m_vRecvDCS.append(QPair(pRecvDCS, pThread));
  }
}

void DCSParsser::slot_RecvDCSData(DCSBaseData* data, int nSiteID) {
  switch (data->getType()) {
    case DCSPLATFORMHEADER:
      SetPlatformHeader((DCSPlatformHeader*)data, nSiteID);
      break;
    case DCSPLATFORMDATA:
      SetPlatformData((DCSPlatformData*)data, nSiteID);
      break;
      /*
    case DCSBEAMHEADER:
      SetBeamHeader((DCSBeamHeader*)data, nRecvID);
      break;
    case DCSBEAMDATA:
      SetBeamData((DCSBeamData*)data, nRecvID);
      break;
    case DCSGATEHEADER:
      SetGateHeader((DCSGateHeader*)data, nRecvID);
      break;
    case DCSGATEDATA:
      SetGateData((DCSGateData*)data, nRecvID);
      break;
    case DCSSCOPEDATA:
      SetScopeData((DCSScopeData*)data, nRecvID);
      break;
    case DCSTIMEHEADER:
      SetTimeHeader((DCSTimeHeader*)data, nRecvID);
      break;
    case DCSTIMEDATA:
      SetTimeData((DCSTimeData*)data, nRecvID);
      break;
    case DCSSCENARIOHEADER:
      SetScenarioHeader((DCSScenarioHeader*)data, nRecvID);
      break;
    case DCSSCENARIODATA:
      SetScenarioData((DCSScenarioData*)data, nRecvID);
      break;
    case DCSEVENT:
      SetEvent((DCSEvent*)data, nRecvID);
      break;
    case DCSGENERICDATA:
      SetGenericData((DCSGenericData*)data, nRecvID);
      break;
    case DCSCATEGORYDATA:
      SetCategoryData((DCSCategoryData*)data, nRecvID);
      break;
    case DCSUNKNOWN:
    */
    default:
      break;
  }

  delete data;
  data = nullptr;
}

void DCSParsser::SetPlatformHeader(DCSPlatformHeader* data, int nSiteID) {
  AddPlatform stAddPlatform;
  stAddPlatform.nID = nSiteID + data->id_;
  stAddPlatform.nSiteID = nSiteID;

  stAddPlatform.strSiteName = "";
  stAddPlatform.strIconName = EXAMPLE_AIRPLANE_ICON;  // data->iconname_;
  stAddPlatform.strCallSign = data->callsign_;

  stAddPlatform.ncoordSys = data->referenceframe_.coordsystem_;

  stAddPlatform.dRefLat = data->referenceframe_.latorigin_;
  stAddPlatform.dRefLon = data->referenceframe_.lonorigin_;
  stAddPlatform.dRefAlt = data->referenceframe_.altorigin_;

  stAddPlatform.dRefOffsetX = data->referenceframe_.tangentOffsetX_;
  stAddPlatform.dRefOffsetY = data->referenceframe_.tangentOffsetY_;
  stAddPlatform.dRefAngle = data->referenceframe_.tangentAngle_;

  auto ittfind = m_mapHeaderData.find(stAddPlatform.nID);
  if (ittfind == m_mapHeaderData.end())
    m_mapHeaderData.insert(stAddPlatform.nID, stAddPlatform);

  emit signal_AddPlatform(stAddPlatform);
}

void DCSParsser::SetPlatformData(DCSPlatformData* data, int nSiteID) {
  UpdatePlatform stUpdatePlatform;

  stUpdatePlatform.nID = nSiteID + data->id_;
  stUpdatePlatform.nSiteID = nSiteID;

  stUpdatePlatform.dTag[UpdatePlatform::LAT] = data->position_.x;
  stUpdatePlatform.dTag[UpdatePlatform::LON] = data->position_.y;
  stUpdatePlatform.dTag[UpdatePlatform::ALT] = data->position_.z;

  stUpdatePlatform.dTag[UpdatePlatform::YAW] = data->orientation_.x;
  stUpdatePlatform.dTag[UpdatePlatform::PITCH] = data->orientation_.y;
  stUpdatePlatform.dTag[UpdatePlatform::ROLL] = data->orientation_.z;

  stUpdatePlatform.dTag[UpdatePlatform::X] = data->velocity_.x;
  stUpdatePlatform.dTag[UpdatePlatform::Y] = data->velocity_.y;
  stUpdatePlatform.dTag[UpdatePlatform::Z] = data->velocity_.z;

  stUpdatePlatform.time = data->time_;

  auto ittfind = m_mapHeaderData.find(stUpdatePlatform.nID);
  if (ittfind != m_mapHeaderData.end()) {
    stUpdatePlatform.stAddPlatform = ittfind.value();
    emit signal_UpdatePlatform(stUpdatePlatform);
  }
}
