#ifndef SCENARIOEDIT_H
#define SCENARIOEDIT_H

#include "../../Common/Struct.h"
#include "../../Common/qdialogskin.h"

#define CLASS_UNCLASSIFIED "UNCLASSIFIED"
#define CLASS_USEONLY "FOR OFFICIAL USE ONLY"
#define CLASS_CONFIDENTIAL "CONFIDENTIAL"
#define CLASS_SECRET "SECRET"

#define E_MODE_REALTIME "RealTime"
#define E_MODE_REPLAY "Replay"
#define E_MODE_SIMULATION "Simulation"

class OperationMain;
namespace Ui {
class ScenarioEdit;
}

class ScenarioEdit : public QDialogSkin {
  Q_OBJECT

 public:
  explicit ScenarioEdit(QWidget *parent = nullptr, QString strTitle = "");
  ~ScenarioEdit();
  void SetScenarioLoad(ScenarioInfo stScenarioInfo);

  // public Q_SLOTS:
  // void SetCalenderDate();

 private slots:
  void on_BTN_INIT_clicked();
  void on_BTN_LOAD_clicked();
  void on_BTN_SET_clicked();
  void on_BTN_SAVE_clicked();
  // void on_EDIT_DATE_selectionChanged();

 private:
  void SetComboBox();

 private:
  Ui::ScenarioEdit *ui;
  OperationMain *m_pParent;
  // QCalendarWidget *m_pCalenderWidget;
};

#endif  // SCENARIOEDIT_H
