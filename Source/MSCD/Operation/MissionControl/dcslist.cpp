#include "dcslist.h"

#include "dcslist_tab.h"
#include "ui_dcslist.h"

DCSList::DCSList(QWidget *parent, QString strTitle)
    : QDialogSkin(parent, strTitle), ui(new Ui::DCSList) {
  ui->setupUi(this);
  SetInitTab();
}

DCSList::~DCSList() { delete ui; }

void DCSList::SetInitTab() {
  for (auto itt : m_mapTab) {
    // ui->TabTDPS->removeTab(0);
    delete itt;
    itt = nullptr;
  }
  // 탭 갯수 확인 후 clear?
  // ui->TabTDPS->clear();
  m_mapTab.clear();
}

void DCSList::SetScenarioLoad(ScenarioInfo stScenarioInfo) {
  SetInitTab();

  for (auto itt : stScenarioInfo.vDCSConfig) {
    DCSList_tab *pTab = new DCSList_tab(this);
    pTab->setEnabled(false);
    pTab->SetPrefInfo(itt);
    // pTab->InitLogList(stScenarioInfo.vEuDataList);
    m_mapTab.insert(itt.nSiteID, pTab);

    ui->TabDCS->addTab(pTab, itt.strSiteName);
  }
}

void DCSList::slot_UpdateInfo(UpdatePlatform stUpdatePlatform) {
  auto ittFind = m_mapTab.find(stUpdatePlatform.stAddPlatform.nSiteID);
  if (ittFind != m_mapTab.end()) {
    ittFind.value()->SetStatusList(stUpdatePlatform);
  }
}

void DCSList::SetUpdatePref(int nID, PlatfromPref stPlatfromPref) {
  emit signal_UpdatePref(nID, stPlatfromPref);
}

void DCSList::SetReqestLogData(int nID) { emit signal_ReqestLogData(nID); }

void DCSList::slot_ResponseDCSLogData(int nSiteID,
                                      QVector<UpdatePlatform> vLogData) {
  auto ittfind = m_mapTab.find(nSiteID);
  if (ittfind != m_mapTab.end()) {
    ittfind.value()->SetLogList(vLogData);
  }
}
