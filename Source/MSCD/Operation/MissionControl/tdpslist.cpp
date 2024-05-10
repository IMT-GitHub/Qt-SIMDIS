#include "tdpslist.h"

#include "tdpslist_tab.h"
#include "ui_tdpslist.h"

TDPSList::TDPSList(QWidget *parent, QString strTitle)
    : QDialogSkin(parent, strTitle), ui(new Ui::TDPSList) {
  ui->setupUi(this);
  SetInitTab();
}

TDPSList::~TDPSList() {
  delete ui;
  SetInitTab();
}

/**
 * @brief TDPSList::SetInitTab
 * 탭 정보 초기화
 */
void TDPSList::SetInitTab() {
  for (auto itt : m_mapTab) {
    // ui->TabTDPS->removeTab(0);
    delete itt;
    itt = nullptr;
  }
  // 탭 갯수 확인 후 clear?
  // ui->TabTDPS->clear();
  m_mapTab.clear();
}

/**
 * @brief TDPSList::SetScenarioLoad
 * @param vTDPSConfig
 * 시나리오 수신 및 탭 설정
 */
void TDPSList::SetScenarioLoad(ScenarioInfo stScenarioInfo) {
  SetInitTab();

  for (auto itt : stScenarioInfo.vTDPSConfig) {
    TDPSList_tab *pTab = new TDPSList_tab(this);
    pTab->setEnabled(false);
    pTab->SetPrefInfo(itt);
    pTab->InitLogList(stScenarioInfo.vEuDataList);
    m_mapTab.insert(itt.nSiteID, pTab);
    // pTab->SetScenarioLoad();
    ui->TabTDPS->addTab(pTab, itt.strSiteName);
  }
}

/**
 * @brief TDPSList::slot_UpdateInfo
 * @param stUpdatePlatform
 * 플랫폼 정보 업데이트
 */
void TDPSList::slot_UpdateInfo(UpdatePlatform stUpdatePlatform) {
  auto ittFind = m_mapTab.find(stUpdatePlatform.stAddPlatform.nSiteID);
  if (ittFind != m_mapTab.end()) {
    ittFind.value()->SetStatusList(stUpdatePlatform);
  }
}

void TDPSList::SetUpdatePref(int nID, PlatfromPref stPlatfromPref) {
  emit signal_UpdatePref(nID, stPlatfromPref);
}

void TDPSList::SetReqestLogData(int nID) { emit signal_ReqestLogData(nID); }

void TDPSList::slot_ResponseQLMLogData(int nSiteID,
                                       QVector<UpdatePlatform> vLogData) {
  auto ittfind = m_mapTab.find(nSiteID);
  if (ittfind != m_mapTab.end()) {
    ittfind.value()->SetLogList(vLogData);
  }
}
