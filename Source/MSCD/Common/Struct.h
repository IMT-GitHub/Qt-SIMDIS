#ifndef STRUCT_H
#define STRUCT_H

#include <DCS.h>

#include <QDateTime>
#include <QString>
#include <QUdpSocket>
#include <QVector>

#include "define.h"
#include "simCore/Calc/CoordinateConverter.h"

// InfoList용 구조체 정보
typedef struct _TestListInfo {
  int nInternalID;
  QString strType;
  QString strCallsign;
  bool bDraw;
  bool bTrack;
  bool bLabel;
  // count는 실시간 업데이트
  _TestListInfo() {
    nInternalID = 0;
    strType = "";
    strCallsign = "";
    bDraw = true;
    bTrack = true;
    bLabel = true;
  }
} TestListInfo;

// Platform 속성정보 설정 구조체 정보
typedef struct _PlatfromPref {
  QString strTrackColor;
  QString strTrackType;
  int nTrackLength;
  int nTrackWidth;

  bool bDynamic;

  bool bDraw;
  bool bTrack;
  bool bLabel;

  _PlatfromPref() {
    strTrackColor = "";
    strTrackType = "";
    nTrackLength = 0;
    nTrackWidth = 0;
    bDynamic = false;

    bDraw = true;
    bTrack = true;
    bLabel = true;
  }

} PlatfromPref;

typedef struct _AddPlatform {
  QString strSiteName;
  QString strIconName;
  QString strCallSign;
  int nID;
  int nSiteID;
  int ncoordSys;
  double dRefLat;
  double dRefLon;
  double dRefAlt;

  double dRefOffsetX;
  double dRefOffsetY;
  double dRefAngle;

  _AddPlatform() {
    strSiteName = "";
    strIconName = "";
    strCallSign = "";
    nID = 0;
    nSiteID = 0;
    ncoordSys = 0;

    dRefLat = 0.0f;
    dRefLon = 0.0f;
    dRefAlt = 0.0f;

    dRefOffsetX = 0.0f;
    dRefOffsetY = 0.0f;
    dRefAngle = 0.0f;
  }

} AddPlatform;

typedef struct _UpdatePlatform {
  // QLM 기준 //DCS 는 재확인
  enum Enum_TAG {
    LAT = 0,  // posX
    LON,      // posY
    ALT,      // posZ
    YAW,      // oriX
    PITCH,    // oriY
    ROLL,     // oriZ
    X,        // velX
    Y,        // velY
    Z,        // velZ
    F_TIME,
    TAG_CNT,
  };

  AddPlatform stAddPlatform;
  int nID;
  int nSiteID;
  double time;
  double dTag[TAG_CNT];

  QVector<double> vUserDefine;

  _UpdatePlatform() {
    nID = 0;
    nSiteID = 0;
    for (int i = LAT; i < TAG_CNT; i++) {
      dTag[i] = 0.0f;
    }

    vUserDefine.clear();
  }

  /**
   * @brief gf_GetLLAPos
   * @return  simCore::Coordinate
   * LLA로 좌표 변환
   */
  simCore::Coordinate gf_GetLLAPos(bool bD2R = true) {
    simCore::CoordinateConverter conv;
    // 절대좌표 기준값?

    simCore::Vec3 vec3Pos;
    simCore::Vec3 vec3Ori;
    simCore::Vec3 vec3Vel;
    if (bD2R == true) {
      conv.setReferenceOrigin(stAddPlatform.dRefLat * UTILS::CU_DEG2RAD,
                              stAddPlatform.dRefLon * UTILS::CU_DEG2RAD,
                              stAddPlatform.dRefAlt * UTILS::CU_DEG2RAD);

      vec3Pos = simCore::Vec3(dTag[LAT] * UTILS::CU_DEG2RAD,
                              dTag[LON] * UTILS::CU_DEG2RAD,
                              dTag[ALT] * UTILS::CU_DEG2RAD);

      /*
      vec3Ori = simCore::Vec3(dTag[YAW] * UTILS::CU_DEG2RAD,
                              dTag[PITCH] * UTILS::CU_DEG2RAD,
                              dTag[ROLL] * UTILS::CU_DEG2RAD);

      vec3Vel = simCore::Vec3(dTag[X] * UTILS::CU_DEG2RAD,
                              dTag[Y] * UTILS::CU_DEG2RAD,
                              dTag[Z] * UTILS::CU_DEG2RAD);
      */
    } else {
      conv.setReferenceOrigin(stAddPlatform.dRefLat, stAddPlatform.dRefLon,
                              stAddPlatform.dRefAlt);

      vec3Pos = simCore::Vec3(dTag[LAT], dTag[LON], dTag[ALT]);
    }

    vec3Ori = simCore::Vec3(dTag[YAW], dTag[PITCH], dTag[ROLL]);
    vec3Vel = simCore::Vec3(dTag[X], dTag[Y], dTag[Z]);

    // 값 확인 필요
    conv.setTangentPlaneOffsets(stAddPlatform.dRefOffsetX,
                                stAddPlatform.dRefOffsetY,
                                stAddPlatform.dRefAngle);

    // 좌표계 변환
    auto coordSys = simCore::COORD_SYS_NONE;
    switch (stAddPlatform.ncoordSys) {
      case DCSCOORD_WGS84:
        coordSys = simCore::COORD_SYS_NONE;
        break;
      case DCSCOORD_LLA:
        coordSys = simCore::COORD_SYS_LLA;
        break;
      case DCSCOORD_NED:
        coordSys = simCore::COORD_SYS_NED;
        break;
      case DCSCOORD_NWU:
        coordSys = simCore::COORD_SYS_NWU;
        break;
      case DCSCOORD_ENU:
        coordSys = simCore::COORD_SYS_ENU;
        break;
      case DCSCOORD_XEAST:
        coordSys = simCore::COORD_SYS_XEAST;
        break;
      case DCSCOORD_GTP:
        coordSys = simCore::COORD_SYS_GTP;
        break;
      case DCSCOORD_ECI:
        coordSys = simCore::COORD_SYS_ECI;
        break;
      default:
        break;
    }

    simCore::Coordinate recdData(coordSys, vec3Pos);

    recdData.setOrientation(vec3Ori);

    recdData.setVelocity(vec3Vel);

    // LLA 로 좌표 변환
    simCore::Coordinate lla;
    conv.convert(recdData, lla, simCore::COORD_SYS_LLA);

    return lla;
  }

} UpdatePlatform;

// 시나리오 관련 구조체
/**
 * @brief TitleScenario
 * 시나리오 타이틀 정보
 */

typedef struct _TitleScenario {
  enum TitleScenarioEnum {
    Title_FilePath = 0,
    Title_Project,
    Title_Mission,
    Title_Date,
    Title_Category,
    Title_Class,
    Title_CNT,
  };
  QString strTitle[Title_CNT];

  _TitleScenario() {
    for (int i = 0; i < Title_CNT; i++) {
      strTitle[i] = "";
    }
  }
} TitleScenario;

/**
 * @brief TDPSConfig
 */
typedef struct _TDPSConfig {
  int nSiteID;  // Site ID
  QString strSiteName;
  QString strIP;
  int nPort;
  
  QString strDataMode;
  QString strCallSign;    // 전시 명칭
  int nTrackLength;       // 이동경로 길이
  QString strTrackType;   // 이동경로 모양(선, 점..)
  QString strTrackColor;  // 이동경로 색상(RGB로 변경?)
  int nTrackWidth;        // 이동경로 두께?
  bool bDynamic;          // Dynamic Scale

  _TDPSConfig() {
    nSiteID = 0;
    strSiteName = "siteName";
    strIP = "127.0.0.1";
    nPort = 0;
    
    strDataMode = "multicast";
    strCallSign = "T0";                 // 전시 명칭
    nTrackLength = 180;                 // 이동경로 길이
    strTrackType = TRACK_TYPE_POINTS;   // 이동경로 모양(선, 점..)
    strTrackColor = TRACK_COLOR_GREEN;  // 이동경로 색상(RGB로 변경?)
    nTrackWidth = 1;                    // 이동경로 두께?
    bDynamic = false;                   // Dynamic Scale
  }
} TDPSConfig;

typedef struct _EuDataList {
  QString strTagName;
  int nTagIndex;
  QString strUnit;
  bool bHUD;
  QString strUserTagName;
  QString strText;

  _EuDataList() {
    strTagName = "user";
    nTagIndex = 0;
    strUnit = "";
    bHUD = false;
    strUserTagName = "";
    strText = "";
  }
} EuDataList;

typedef struct _DCSConfig {
  int nSiteID;  // DCS Server별 ID
  QString strSiteName;
  QString strIP;     // vrf
  int nPort;         // vrf
  QString strTCPIP;  // DCS
  int nTCPPort;      // DCS
  QString strDataMode;
  QString strModelType;
  QString strServerName;
  int nTrackLength;
  QString strTrackType;
  QString strTrackColor;
  int nTrackWidth;
  bool bDynamic;

  _DCSConfig() {
    nSiteID = 0;
    strSiteName = "lship";
    strIP = "127.0.0.1";
    nPort = 0;
    strTCPIP = "127.0.0.1";
    nTCPPort = 0;
    strDataMode = "multicast";
    strModelType = "KDCOM";
    strServerName = "ServerName";
    nTrackLength = 180;
    strTrackType = TRACK_TYPE_POINTS;
    strTrackColor = TRACK_COLOR_GREEN;
    nTrackWidth = 1;
    bDynamic = false;
  }
} DCSConfig;

typedef struct _ScenarioInfo {
  TitleScenario stTitleScenario;
  int nTDPSDispLimit;
  int nDCSDispLimit;
  QVector<TDPSConfig> vTDPSConfig;
  QVector<EuDataList> vEuDataList;
  QVector<DCSConfig> vDCSConfig;
  _ScenarioInfo() {
    nTDPSDispLimit = 0;
    nDCSDispLimit = 0;
  }
} ScenarioInfo;

#endif  // STRUCT_H
