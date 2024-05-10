#ifndef SCENARIOCONTROL_H
#define SCENARIOCONTROL_H

// #include <QDialog>
#include <QSizeGrip>
#include <QVBoxLayout>

#include "../../Common/Struct.h"
#include "../../Common/qdialogskin.h"

class OperationMain;
namespace Ui {
class Scenariocontrol;
}

class Scenariocontrol : public QDialogSkin /*QDialog*/ {
  Q_OBJECT

 public:
  explicit Scenariocontrol(QWidget *parent = nullptr, QString strTitle = "");
  ~Scenariocontrol();
  void SetScenarioLoad(ScenarioInfo stScenarioInfo);

 private slots:
  void on_BTN_ScenarioLoad_clicked();
  void on_BTN_ScenarioReady_clicked();

  void on_BTN_Record_clicked();

  void on_BTN_Stop_clicked();

 private:
  OperationMain *m_pParent;
  Ui::Scenariocontrol *ui;
  QSizeGrip *sizeGrip;
  QVBoxLayout *layout_Size;
  ScenarioInfo m_stScenarioInfo;
};

#endif  // SCENARIOCONTROL_H
