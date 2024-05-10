#ifndef TDPSRECVCONFIG_H
#define TDPSRECVCONFIG_H

#include <QStandardItem>

#include "../../Common/Global.h"
#include "../../Common/Struct.h"
#include "../../Common/qdialogskin.h"

namespace Ui {
class TDPSRecvConfig;
}  // namespace Ui

class TDPSRecvConfig : public QDialogSkin {
  Q_OBJECT

 public:
  explicit TDPSRecvConfig(QWidget *parent = nullptr, QString strTitle = "");
  ~TDPSRecvConfig();
  QVector<TDPSConfig> GetTDPSConfig();
  int GetTDPSDispLimit();
  void SetScenarioLoad(ScenarioInfo stScenarioInfo);
  void SetScenarioInit();

 private slots:
  void on_BTN_Modify_clicked();
  void on_BTN_Save_clicked();
  void on_BTN_Insert_clicked();
  void on_BTN_Delete_clicked();

 private:
  void SetInitTable();
  void InsertConfig(TDPSConfig stTDPSConfig);

 private:
  Ui::TDPSRecvConfig *ui;
  QStandardItemModel *m_pTableTDPS;
  ComboBoxDelegate *m_pComboBoxDelegate[DelegateCombo::COMBO_TDPS_RECV_CNT];
  int m_nCol[DelegateCombo::COMBO_TDPS_RECV_CNT];
};

#endif  // TDPSRECVCONFIG_H
