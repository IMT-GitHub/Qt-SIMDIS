#ifndef TDPSEUDATALIST_H
#define TDPSEUDATALIST_H

#include <QApplication>
#include <QStandardItem>

#include "../../Common/Global.h"
#include "../../Common/Struct.h"
#include "../../Common/qdialogskin.h"

namespace Ui {
class TDPSEuDataList;
}

class TDPSEuDataList : public QDialogSkin {
  Q_OBJECT

 public:
  explicit TDPSEuDataList(QWidget *parent = nullptr, QString strTitle = "");
  ~TDPSEuDataList();
  QVector<EuDataList> GetEuDataScenario();
  void SetScenarioLoad(ScenarioInfo stScenarioInfo);
  void SetScenarioInit();

 private slots:
  void on_BTN_Modify_clicked();
  void on_BTN_Save_clicked();
  void on_BTN_Insert_clicked();
  void on_BTN_Delete_clicked();

 private:
  void SetInitTable();
  void InitEuData();
  void InsertEuData(EuDataList stEuDataList);

 private:
  Ui::TDPSEuDataList *ui;
  QStandardItemModel *m_pTableTDPS;
  ComboBoxDelegate *m_pComboBoxDelegate[DelegateCombo::COMBO_TDPS_EU_CNT];
};

#endif  // TDPSEUDATALIST_H
