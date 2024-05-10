#include "scenariocontrol.h"

#include <QFileDialog>

#include "../operationmain.h"
#include "ui_scenariocontrol.h"

Scenariocontrol::Scenariocontrol(QWidget *parent, QString strTitle)
    : QDialogSkin /*QDialog*/ (parent, strTitle),
      ui(new Ui::Scenariocontrol),
      m_pParent(reinterpret_cast<OperationMain *>(parent)) {
  // setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
  ui->setupUi(this);

  ui->BTN_ScenarioReady->setEnabled(false);
  ui->BTN_Record->setEnabled(false);
  ui->BTN_Stop->setEnabled(false);
}

Scenariocontrol::~Scenariocontrol() { delete ui; }

/**
 * @brief Scenariocontrol::on_BTN_ScenarioLoad_clicked
 * 시나리오 불러오기
 */
void Scenariocontrol::on_BTN_ScenarioLoad_clicked() {
  QString strFileName = "";
  strFileName = QFileDialog::getOpenFileName(
      this, FILEDIALOG_SCENARIOLOAD, QDir::currentPath(), "Files (*.xml)");

  if (m_pParent != nullptr) m_pParent->SetScenarioLoad(strFileName, false);

  if (strFileName.compare("") != 0) {
    ui->BTN_ScenarioLoad->setEnabled(false);
    ui->BTN_ScenarioReady->setEnabled(true);
    ui->BTN_Record->setEnabled(true);
    ui->BTN_Stop->setEnabled(true);
  }
}

/**
 * @brief Scenariocontrol::on_BTN_ScenarioReady_clicked
 * 시나리오 배포 및 시작
 * 추후 기능 분리 및 변경
 */
void Scenariocontrol::on_BTN_ScenarioReady_clicked() {
  m_pParent->SetMissionStart(m_stScenarioInfo);
}

void Scenariocontrol::on_BTN_Record_clicked() {}

void Scenariocontrol::on_BTN_Stop_clicked() {
  ui->BTN_ScenarioLoad->setEnabled(true);
  ui->BTN_ScenarioReady->setEnabled(false);
  ui->BTN_Record->setEnabled(false);
  ui->BTN_Stop->setEnabled(false);

  m_pParent->SetMissionStop();
}

void Scenariocontrol::SetScenarioLoad(ScenarioInfo stScenarioInfo) {
  m_stScenarioInfo = stScenarioInfo;

  ui->EDIT_FilePath->setText(
      stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_FilePath]);
  ui->EDIT_Proj->setText(
      stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Project]);
  ui->EDIT_Test->setText(
      stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Mission]);
  ui->EDIT_Class->setText(
      stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Class]);
  ui->EDIT_Date->setText(
      stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Date]);
  ui->EDIT_Mode->setText(
      stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Category]);
}
