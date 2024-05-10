#ifndef TDPSLIST_H
#define TDPSLIST_H

#include <QVector>

#include "../../Common/Struct.h"
#include "../../Common/qdialogskin.h"

class TDPSList_tab;
namespace Ui {
class TDPSList;
}

class TDPSList : public QDialogSkin {
  Q_OBJECT

 public:
  explicit TDPSList(QWidget *parent = nullptr, QString strTitle = "");
  ~TDPSList();
  void SetScenarioLoad(ScenarioInfo stScenarioInfo);
  void SetUpdatePref(int nID, PlatfromPref stPlatfromPref);
  void SetReqestLogData(int nID);

 private:
  void SetInitTab();

 public Q_SLOTS:
  void slot_UpdateInfo(UpdatePlatform stUpdatePlatform);
  void slot_ResponseQLMLogData(int nSiteID, QVector<UpdatePlatform> vLogData);

 signals:
  void signal_UpdatePref(int nID, PlatfromPref stPlatfromPref);
  void signal_ReqestLogData(int nID);

 private:
  Ui::TDPSList *ui;
  // QVector<TDPSList_tab *> m_vTab;
  QMap<int, TDPSList_tab *>
      m_mapTab;  // Key : RecvID, Value : Tap Class Pointer
};

#endif  // TDPSLIST_H
