#ifndef DCSLIST_H
#define DCSLIST_H

#include "../../Common/Struct.h"
#include "../../Common/qdialogskin.h"

class DCSList_tab;
namespace Ui {
class DCSList;
}

class DCSList : public QDialogSkin {
  Q_OBJECT

 public:
  explicit DCSList(QWidget *parent = nullptr, QString strTitle = "");
  ~DCSList();
  void SetScenarioLoad(ScenarioInfo stScenarioInfo);
  void SetUpdatePref(int nID, PlatfromPref stPlatfromPref);
  void SetReqestLogData(int nID);

 public Q_SLOTS:
  void slot_UpdateInfo(UpdatePlatform stUpdatePlatform);
  void slot_ResponseDCSLogData(int nSiteID, QVector<UpdatePlatform> vLogData);

 signals:
  void signal_UpdatePref(int nID, PlatfromPref stPlatfromPref);
  void signal_ReqestLogData(int nID);

 private:
  void SetInitTab();

 private:
  Ui::DCSList *ui;
  QMap<int, DCSList_tab *> m_mapTab;  // Key : RecvID, Value : Tap Class Pointer
};

#endif  // DCSLIST_H
