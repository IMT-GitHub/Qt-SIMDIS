#ifndef DEFINE_H
#define DEFINE_H

#define _LANG_KOR
// #define _LANG_ENG

// Data ID Define

#define QLM_Site_ID 10000
#define QLM_Site_ID_Sub 1000


#define DCS_Site_ID 20000
#define DCS_Site_ID_Sub 1000

#define TRACK_TYPE_POINTS "Points"
#define TRACK_TYPE_LINE "Line"
#define TRACK_TYPE_RIBON "Ribbon"
#define TRACK_TYPE_BRIDGE "Bridge"

#define TRACK_COLOR_CYAN "Cyan"
#define TRACK_COLOR_GREEN "Green"
#define TRACK_COLOR_MAGENTA "Magenta"
#define TRACK_COLOR_YELLOW "Yellow"
#define TRACK_COLOR_RED "Red"
#define TRACK_COLOR_WHITE "White"
#define TRACK_COLOR_BLACK "Black"

// Dialog Title string define
#ifdef _LANG_KOR
// DLG
#define MAINTITLE "시스템 SW"
// MISSION CONTROL POPUP
#define SCENARIO_CONTROL "시나리오 제어"
#define TDPS_RECEIVE "자료처리시스템 수신현황"
#define DCS_RECIVE "DCS 서버 수신 현황"
// SCENARIO EDIT POPUP
#define SCENARIO_EDIT "시나리오 편집"
#define TDPS_RECV_CONFIG "자료처리시스템 데이터 수신 설정"
#define TDPS_EU_LIST "자료처리시스템 데이터 목록"
#define DCS_RECV_CONFIG "서버(DCS) 데이터 수신 설정"
// BTN
#define BTN_MAIN_MISSION "시험 통제"
#define BTN_MAIN_SCENARIO "시나리오 편집"
#define BTN_MAIN_DLG_POSINIT "팝업창 위치 초기화"
#define BTN_MAIN_DLG_POSLOAD "팝업창 위치 불러오기"
#define BTN_MAIN_DLG_POSSAVE "팝업창 위치 저장하기"
#define BTN_MAIN_DLG_CLOSE "종료"
#define TEXT_MAIN_STATUS_REALTIME "Real Time"
#define TEXT_MAIN_STATUS_EDIT "Scenario Edit"
#define TEXT_MAIN_STATUS_POSTANALYSIS "Post-Mission Analysys"
// ETC
#define FILEDIALOG_SCENARIOLOAD "시나리오 파일 불러오기"
#else
#define MAINTITLE "System SW"
// MISSION CONTROL POPUP
#define SCENARIO_CONTROL "Scenario Control"
#define TDPS_RECEIVE "Data Prosses Recive List
#define DCS_RECIVE "DCS Recive List"
// SCENARIO EDIT POPUP
#define SCENARIO_EDIT "Scenario Edit"
#define TDPS_RECV_CONFIG \
  "Data Processing Recv Config"
#define TDPS_EU_LIST "Data Processing Data List"
#define DCS_RECV_CONFIG "Server(DCS) Data Recv Config"
// BTN
#define BTN_MAIN_MISSION "Test Control"
#define BTN_MAIN_SCENARIO "Scenario Edit"
#define BTN_MAIN_DLG_POSINIT "PopUp DLG Init"
#define BTN_MAIN_DLG_POSLOAD "PopUp DLG Load"
#define BTN_MAIN_DLG_POSSAVE "PopUp DLG Save"
#define TEXT_MAIN_STATUS_REALTIME "Real Time"
#define TEXT_MAIN_STATUS_EDIT "Scenario Edit"
#define TEXT_MAIN_STATUS_POSTANALYSIS "Post-Mission Analysys"
// ETC
#define FILEDIALOG_SCENARIOLOAD "Scenario File Load"
#endif

#endif  // DEFINE_H
