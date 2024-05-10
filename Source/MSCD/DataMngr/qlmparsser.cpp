#include "qlmparsser.h"

#include "../../Interface/recvqlm.h"
#include "../Common/Global.h"
#include "simUtil/ExampleResources.h"

QLMParsser::QLMParsser() : m_nDispLimit(0) {
  m_dTest = 0.0f;
  SetRecvInit();
}

QLMParsser::~QLMParsser() { SetRecvInit(); }

/**
 * @brief QLMParsser::SetRecvInit
 * 수신정보 초기화
 */
void QLMParsser::SetRecvInit() {
  for (auto itt : m_vRecvQLM) {
    if (itt != nullptr) {
      disconnect(itt, SIGNAL(signal_RecvQLMData(QByteArray, int, QString)),
                 this, SLOT(slot_RecvQLMData(QByteArray, int, QString)));
      delete itt;
      itt = nullptr;
    }
  }

  m_vRecvQLM.clear();
  m_mapQLMData.clear();
  m_mapLogData.clear();
}

/**
 * @brief QLMParsser::SetQLMRecv
 * @param vTDPSConfig
 * 시나리오 기반 수신설정
 */
void QLMParsser::SetQLMRecv(QVector<TDPSConfig> vTDPSConfig) {
  SetRecvInit();

  // 시나리오 파일을 통해 QLM 갯수 설정
  for (auto itt : vTDPSConfig) {
    RecvQLM* pRecvQLM = new RecvQLM(QHostAddress(itt.strIP), itt.nPort,
                                    itt.nSiteID, itt.strCallSign);

    connect(pRecvQLM, SIGNAL(signal_RecvQLMData(QByteArray, int, QString)),
            this, SLOT(slot_RecvQLMData(QByteArray, int, QString)));

    m_vRecvQLM.append(pRecvQLM);
  }
}

/**
 * @brief QLMParsser::slot_RecvQLMData
 * @param buffer
 * @param nQLMID
 * QLM 데이터 수신
 */
void QLMParsser::slot_RecvQLMData(QByteArray buffer, int nSiteID,
                                  QString strCallSign) {
  int nDataCount = 0;
  QByteArray bufferData;
  memcpy(&nDataCount, buffer.constData(), sizeof(int));

  // Data Count 4Byte 자르고 Data만
  bufferData.resize(nDataCount * sizeof(double));
  memcpy(bufferData.data(), buffer.constData() + sizeof(int),
         nDataCount * sizeof(double));

  UpdatePlatform stUpdatePlatform;
  stUpdatePlatform.nID =
      nSiteID + 1;  // id Counter 필요?, QLM으로 2개 이상 트랙 수신시
  stUpdatePlatform.nSiteID = nSiteID;

  for (int i = 0; i < nDataCount; i++) {
    double dData = 0.0f;
    memcpy(&dData, bufferData.constData() + (sizeof(double) * i),
           sizeof(double));
    if (i < stUpdatePlatform.TAG_CNT) {
      stUpdatePlatform.dTag[i] = dData;

      // TestCode
      if (i == 0 && nSiteID == 10000) {
        m_dTest = m_dTest + 0.00001;
        stUpdatePlatform.dTag[i] = stUpdatePlatform.dTag[i] + m_dTest;
      };
      if (i == 2) {
        stUpdatePlatform.dTag[i] = 1000;
      }

    } else {
      stUpdatePlatform.vUserDefine.append(dData);
    }
  }

  stUpdatePlatform.time = SetTime();

 
  auto ittFind = m_mapQLMData.find(nSiteID);
  if (ittFind == m_mapQLMData.end()) {
    stUpdatePlatform.stAddPlatform =
        SetPlatformAdd(stUpdatePlatform, strCallSign);
    m_mapQLMData.insert(nSiteID, stUpdatePlatform.stAddPlatform);
  } else {
    stUpdatePlatform.stAddPlatform = ittFind.value();
  }
  emit signal_UpdatePlatform(stUpdatePlatform);

  // File Write 설정?
}

/**
 * @brief QLMParsser::SetPlatformAdd
 * @param stUpdatePlatform
 * @return AddPlatform
 * AddPlatform(Header 부분)설정
 */
AddPlatform QLMParsser::SetPlatformAdd(UpdatePlatform stUpdatePlatform,
                                       QString strCallSign) {
  AddPlatform stAddPlatform;

  stAddPlatform.strSiteName = "";

  stAddPlatform.nID = stUpdatePlatform.nID;
  stAddPlatform.nSiteID = stUpdatePlatform.nSiteID;
  stAddPlatform.strCallSign = strCallSign;
  stAddPlatform.strIconName = EXAMPLE_MISSILE_ICON;

  // DCS와 값 맞춤
  stAddPlatform.ncoordSys =
      DCSCOORD_LLA;  // 좌표계 정보, 탄 종류에 따른 변경 필요

  stAddPlatform.dRefLat = stUpdatePlatform.dTag[stUpdatePlatform.LAT];
  stAddPlatform.dRefLon = stUpdatePlatform.dTag[stUpdatePlatform.LON];
  stAddPlatform.dRefAlt = stUpdatePlatform.dTag[stUpdatePlatform.ALT];
  // 값 확인 필요
  // stAddPlatform.dRefOffsetX = stUpdatePlatform.dTag[stUpdatePlatform.YAW];
  // stAddPlatform.dRefOffsetY = stUpdatePlatform.dTag[stUpdatePlatform.PITCH];
  // stAddPlatform.dRefAngle = stUpdatePlatform.dTag[stUpdatePlatform.ROLL];

  emit signal_AddPlatform(stAddPlatform);

  return stAddPlatform;
}
