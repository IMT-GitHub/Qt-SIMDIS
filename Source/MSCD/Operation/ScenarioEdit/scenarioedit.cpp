#include "scenarioedit.h"

#include <QFileDialog>

#include "../../Common/define.h"
#include "../operationmain.h"
#include "ui_scenarioedit.h"

ScenarioEdit::ScenarioEdit(QWidget *parent, QString strTitle)
    : QDialogSkin /*QDialog*/ (parent, strTitle),
      ui(new Ui::ScenarioEdit),
      m_pParent(reinterpret_cast<OperationMain *>(parent)) {
  ui->setupUi(this);

  SetComboBox();
}

ScenarioEdit::~ScenarioEdit() { delete ui; }

void ScenarioEdit::SetComboBox() {
  ui->COMBO_CLASSIFICATION->addItem(CLASS_UNCLASSIFIED);
  ui->COMBO_CLASSIFICATION->addItem(CLASS_USEONLY);
  ui->COMBO_CLASSIFICATION->addItem(CLASS_CONFIDENTIAL);
  ui->COMBO_CLASSIFICATION->addItem(CLASS_SECRET);
  ui->COMBO_CLASSIFICATION->setCurrentIndex(0);

  ui->COMBO_MODE->addItem("RealTime");
  ui->COMBO_MODE->addItem("Replay");
  ui->COMBO_MODE->addItem("Simulation");
  ui->COMBO_MODE->setCurrentIndex(0);
}

void ScenarioEdit::on_BTN_INIT_clicked() {
  ui->COMBO_CLASSIFICATION->setCurrentIndex(0);
  ui->COMBO_MODE->setCurrentIndex(0);
  ui->EDIT_MISSION->setText("");
  ui->EDIT_DATE->setText("");
  ui->EDIT_PROJECT->setText("");

  // init_Scenario()
  m_pParent->SetScenarioInit();
}

void ScenarioEdit::on_BTN_LOAD_clicked() {
  QString strFileName = QFileDialog::getOpenFileName(
      this, FILEDIALOG_SCENARIOLOAD, QDir::currentPath(), "Files (*.xml)");
  // qDebug() << "Scenario File Name" << strFileName;

  if (m_pParent != nullptr) m_pParent->SetScenarioLoad(strFileName);
}

void ScenarioEdit::on_BTN_SET_clicked() {
  //?
}

void ScenarioEdit::on_BTN_SAVE_clicked() {
  TitleScenario stTitleScenario;

  stTitleScenario.strTitle[TitleScenario::Title_FilePath] = "";
  stTitleScenario.strTitle[TitleScenario::Title_Project] =
      ui->EDIT_PROJECT->text();
  stTitleScenario.strTitle[TitleScenario::Title_Mission] =
      ui->EDIT_MISSION->text();
  stTitleScenario.strTitle[TitleScenario::Title_Date] = ui->EDIT_DATE->text();
  stTitleScenario.strTitle[TitleScenario::Title_Category] =
      ui->COMBO_MODE->currentText();
  stTitleScenario.strTitle[TitleScenario::Title_Class] =
      ui->COMBO_CLASSIFICATION->currentText();
  m_pParent->SetScenarioSave(stTitleScenario);
}

void ScenarioEdit::SetScenarioLoad(ScenarioInfo stScenarioInfo) {
  ui->EDIT_MISSION->setText(
      stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Mission]);
  ui->EDIT_DATE->setText(
      stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Date]);
  ui->EDIT_PROJECT->setText(
      stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Project]);

  if (stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Class]
          .compare(CLASS_UNCLASSIFIED) == 0)
    ui->COMBO_CLASSIFICATION->setCurrentIndex(0);
  else if (stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Class]
               .compare(CLASS_USEONLY) == 0)
    ui->COMBO_CLASSIFICATION->setCurrentIndex(1);
  else if (stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Class]
               .compare(CLASS_CONFIDENTIAL) == 0)
    ui->COMBO_CLASSIFICATION->setCurrentIndex(2);
  else if (stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Class]
               .compare(CLASS_SECRET) == 0)
    ui->COMBO_CLASSIFICATION->setCurrentIndex(3);
  else
    ui->COMBO_CLASSIFICATION->setCurrentIndex(0);

  if (stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Category]
          .compare(E_MODE_REALTIME) == 0)
    ui->COMBO_MODE->setCurrentIndex(0);
  else if (stScenarioInfo.stTitleScenario
               .strTitle[TitleScenario::Title_Category]
               .compare(E_MODE_REPLAY) == 0)
    ui->COMBO_MODE->setCurrentIndex(1);
  else if (stScenarioInfo.stTitleScenario
               .strTitle[TitleScenario::Title_Category]
               .compare(E_MODE_SIMULATION) == 0)
    ui->COMBO_MODE->setCurrentIndex(2);
  else
    ui->COMBO_MODE->setCurrentIndex(0);
}
