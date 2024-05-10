#ifndef DCSRECVCONFIG_H
#define DCSRECVCONFIG_H

#include <QStandardItem>

#include "../../Common/Global.h"
#include "../../Common/Struct.h"
#include "../../Common/qdialogskin.h"

namespace Ui {
class DCSRecvConfig;
}

class DCSRecvConfig : public QDialogSkin {
  Q_OBJECT

 public:
  explicit DCSRecvConfig(QWidget *parent = nullptr, QString strTitle = "");
  ~DCSRecvConfig();
  QVector<DCSConfig> GetDCSConfig();
  int GetDCSDispLimit();
  void SetScenarioLoad(ScenarioInfo stScenarioInfo);
  void SetScenarioInit();

 private slots:
  void on_BTN_Modify_clicked();
  void on_BTN_Save_clicked();
  void on_BTN_Insert_clicked();
  void on_BTN_Delete_clicked();

 private:
  void SetInitTable();
  void InsertConfig(DCSConfig stDCSConfig);

 private:
  Ui::DCSRecvConfig *ui;
  QStandardItemModel *m_pTableTDPS;
  ComboBoxDelegate *m_pComboBoxDelegate[DelegateCombo::COMBO_DCS_RECV_CNT];
  int m_nDelegateCol[DelegateCombo::COMBO_DCS_RECV_CNT];
};

#endif  // DCSRECVCONFIG_H
