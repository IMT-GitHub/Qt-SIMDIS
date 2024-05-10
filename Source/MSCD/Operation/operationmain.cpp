#include "operationmain.h"

#include <QAction>
#include <QDebug>
#include <QMenu>
#include <QPainter>
#include <QSettings>

#include "../DataMngr/datamngr.h"
#include "../Display/huddisplay.h"
#include "../ScenarioMngr/scenariomngr.h"
#include "../mainwindow.h"
#include "MissionControl/dcslist.h"
#include "MissionControl/scenariocontrol.h"
#include "MissionControl/tdpslist.h"
#include "ScenarioEdit/dcsrecvconfig.h"
#include "ScenarioEdit/scenarioedit.h"
#include "ScenarioEdit/tdpseudatalist.h"
#include "ScenarioEdit/tdpsrecvconfig.h"
#include "ui_operationmain.h"

OperationMain::OperationMain(QWidget *parent)
    : QDialog(parent),
      m_pParent(reinterpret_cast<MainWindow *>(parent)),
      ui(new Ui::OperationMain),
      m_pDataMngr(nullptr),
      m_pScenarioMngr(nullptr),
      m_pScenariocontrol(nullptr),
      m_pTDPSList(nullptr),
      m_pDCSList(nullptr),
      m_pScenarioEdit(nullptr),
      m_pTDPSRecvConfig(nullptr),
      m_pTDPSEuDataList(nullptr),
      m_pDCSRecvConfig(nullptr),
      m_nMode(MODE_MISSIONCONTROL) {
  ui->setupUi(this);

  setWindowTitle(MAINTITLE);
  CreateBtn();
  // setStyleSheet("background-color: rgba(221, 237, 247, 100%)");
}

OperationMain::~OperationMain() {
  delete ui;
  for (auto itt : m_vDlgMissionControl) {
    if (itt.first != nullptr) {
      delete itt.first;
    }
  }
  m_vDlgMissionControl.clear();

  for (auto itt : m_vDlgEditControl) {
    if (itt.first != nullptr) {
      delete itt.first;
    }
  }
  m_vDlgEditControl.clear();

  for (int i = 0; i < BTN_CNT; i++) {
    if (m_pBtn[i] != nullptr) {
      delete m_pBtn[i];
      m_pBtn[i] = nullptr;
    }
  }
}

/**
 * @brief OperationMain::CreateBtn
 * 메인화면 버튼
 * 시험통제, 시나리오 편집, 다이얼로그 위치 초기화, 다이얼로그 위치 불러오기,
 * 다이얼로그 위치 저장
 */
void OperationMain::CreateBtn() {
  QString strText[BTN_CNT] = {BTN_MAIN_MISSION,     BTN_MAIN_SCENARIO,
                              BTN_MAIN_DLG_POSINIT, BTN_MAIN_DLG_POSLOAD,
                              BTN_MAIN_DLG_POSSAVE, BTN_MAIN_DLG_CLOSE};

  for (int i = 0; i < BTN_CNT; i++) {
    m_pBtn[i] = nullptr;
    m_pBtn[i] = new QPushButton(this);

    m_pBtn[i]->setText(strText[i]);
    //  m_pBtn[i]->move();
  }

  connect(m_pBtn[BTN_MISSION], SIGNAL(clicked()), this,
          SLOT(MISSION_BTN_clicked()));
  connect(m_pBtn[BTN_EDIT], SIGNAL(clicked()), this, SLOT(EDIT_BTN_clicked()));
  connect(m_pBtn[BTN_POSINIT], SIGNAL(clicked()), this,
          SLOT(POSINIT_BTN_clicked()));
  connect(m_pBtn[BTN_POSLOAD], SIGNAL(clicked()), this,
          SLOT(POSLOAD_BTN_clicked()));
  connect(m_pBtn[BTN_POSSAVE], SIGNAL(clicked()), this,
          SLOT(POSSAVE_BTN_clicked()));
  connect(m_pBtn[BTN_CLOSE], SIGNAL(clicked()), this,
          SLOT(CLOSE_BTN_clicked()));

  m_pBtn[BTN_MISSION]->setCheckable(true);
  m_pBtn[BTN_EDIT]->setCheckable(true);
  m_pBtn[BTN_MISSION]->setChecked(true);
}

/**
 * @brief OperationMain::resizeEvent
 * @param event
 * 메인 다이얼로그 크기 변경 이벤트
 */
void OperationMain::resizeEvent(QResizeEvent *event) {
  QRect rectBtn[BTN_CNT] = {QRect(10, 15, 130, 30),
                            QRect(140, 15, 130, 30),
                            QRect(width() - 560, 15, 130, 30),
                            QRect(width() - 420, 15, 130, 30),
                            QRect(width() - 280, 15, 130, 30),
                            QRect(width() - 100, 15, 80, 30)};
  for (int i = 0; i < BTN_CNT; i++) {
    if (m_pBtn[i] != nullptr) m_pBtn[i]->setGeometry(rectBtn[i]);
  }
}

/**
 * @brief OperationMain::SetLoadDlgPos
 * 팝업 다이얼로그 위치 불러오기
 */
void OperationMain::SetLoadDlgPos() {
  QSettings settings("MSCD_POS.ini", QSettings::IniFormat);
  if (settings.status() == QSettings::NoError) {
    for (auto itt : m_vDlgMissionControl) {
      QRect rect =
          settings.value(itt.first->objectName(), itt.first->geometry())
              .toRect();

      itt.first->setGeometry(rect);
    }

    for (auto itt : m_vDlgEditControl) {
      QRect rect =
          settings.value(itt.first->objectName(), itt.first->geometry())
              .toRect();

      itt.first->setGeometry(rect);
    }
  }
}

/**
 * @brief OperationMain::MISSION_BTN_clicked
 * 시험 수행 버튼
 */
void OperationMain::MISSION_BTN_clicked() {
  if (m_pBtn[BTN_MISSION]->isChecked()) {
    // qDebug() << "Mode MISSION";
    m_nMode = MODE_MISSIONCONTROL;
    SetModeShowPopup();
  }

  m_pBtn[BTN_MISSION]->setChecked(true);
  m_pBtn[BTN_EDIT]->setChecked(false);
}

/**
 * @brief OperationMain::EDIT_BTN_clicked
 * 시나리오 편집 버튼
 */
void OperationMain::EDIT_BTN_clicked() {
  if (m_pBtn[BTN_EDIT]->isChecked()) {
    // qDebug() << "Mode EDIT";
    m_nMode = MODE_SCENARIOEDIT;
    SetModeShowPopup();
  }

  m_pBtn[BTN_EDIT]->setChecked(true);
  m_pBtn[BTN_MISSION]->setChecked(false);
}

/**
 * @brief OperationMain::POSINIT_BTN_clicked
 * 팝업 다디얼로그 위치 초기화
 */
void OperationMain::POSINIT_BTN_clicked() {}

/**
 * @brief OperationMain::POSLOAD_BTN_clicked
 * 팝업 다이얼로그 위치 불러오기
 */
void OperationMain::POSLOAD_BTN_clicked() { SetLoadDlgPos(); }

/**
 * @brief OperationMain::POSSAVE_BTN_clicked
 * 팝업 다이얼로그 위치 저장
 */
void OperationMain::POSSAVE_BTN_clicked() {
  QSettings settings("MSCD_POS.ini", QSettings::IniFormat);
  for (auto itt : m_vDlgMissionControl) {
    settings.setValue(itt.first->objectName(), itt.first->geometry());
  }

  for (auto itt : m_vDlgEditControl) {
    settings.setValue(itt.first->objectName(), itt.first->geometry());
  }
}

/**
 * @brief OperationMain::CLOSE_BTN_clicked
 * 종료
 */
void OperationMain::CLOSE_BTN_clicked() { QApplication::quit(); }

/**
 * @brief OperationMain::GetDataMngr
 * @param pDataMngr
 * DataMngr 클래스 포인터 설정
 */
void OperationMain::GetDataMngr(DataMngr *pDataMngr) {
  m_pDataMngr = pDataMngr;
}

void OperationMain::GetScenarioMngr(ScenarioMngr *pScenarioMngr) {
  m_pScenarioMngr = pScenarioMngr;
}

/**
 * @brief OperationMain::SetModeShowPopup
 * 모드별 팝업 다이얼로그 전시 설정
 */
void OperationMain::SetModeShowPopup() {
  if (m_nMode == MODE_MISSIONCONTROL) {
    for (auto itt : m_vDlgMissionControl) {
      itt.first->show();
    }
    for (auto itt : m_vDlgEditControl) {
      itt.first->hide();
    }
  } else {
    for (auto itt : m_vDlgMissionControl) {
      itt.first->hide();
    }
    for (auto itt : m_vDlgEditControl) {
      itt.first->show();
    }
  }
}

/**
 * @brief OperationMain::SetInitSubDlg
 * SUB 다이얼로그 생성
 */
void OperationMain::SetInitSubDlg() {
  SetInitSubDlg_Mission();
  SetInitSubDlg_Edit();
  SetModeShowPopup();
  SetLoadDlgPos();
}

/**
 * @brief OperationMain::SetInitSubDlg_Mission
 * 시험 수행시 팝업 다이얼로그 설정
 */
void OperationMain::SetInitSubDlg_Mission() {
  m_pScenariocontrol = new Scenariocontrol(this, SCENARIO_CONTROL);
  m_pScenariocontrol->setObjectName("SCENARIO_CONTROL");
  m_pScenariocontrol->hide();
  m_vDlgMissionControl.append(QPair<QDialog *, QString>(
      reinterpret_cast<QDialog *>(m_pScenariocontrol), SCENARIO_CONTROL));

  m_pTDPSList = new TDPSList(this, TDPS_RECEIVE);
  m_pTDPSList->setObjectName("TDPS_RECEIVE");
  m_pTDPSList->hide();
  m_vDlgMissionControl.append(QPair<QDialog *, QString>(
      reinterpret_cast<QDialog *>(m_pTDPSList), TDPS_RECEIVE));

  m_pDCSList = new DCSList(this, DCS_RECIVE);
  m_pDCSList->setObjectName("DCS_RECIVE");
  m_pDCSList->hide();
  m_vDlgMissionControl.append(QPair<QDialog *, QString>(
      reinterpret_cast<QDialog *>(m_pDCSList), DCS_RECIVE));
}

/**
 * @brief OperationMain::SetInitSubDlg_Edit
 * 시나리오 편집시 팝업 다이얼로그 설정
 */
void OperationMain::SetInitSubDlg_Edit() {
  m_pScenarioEdit = new ScenarioEdit(this, SCENARIO_EDIT);
  m_pScenarioEdit->setObjectName("SCENARIO_EDIT");
  m_pScenarioEdit->hide();
  m_vDlgEditControl.append(QPair<QDialog *, QString>(
      reinterpret_cast<QDialog *>(m_pScenarioEdit), SCENARIO_EDIT));

  m_pTDPSRecvConfig = new TDPSRecvConfig(this, TDPS_RECV_CONFIG);
  m_pTDPSRecvConfig->setObjectName("TDPS_RECV_CONFIG");
  m_pTDPSRecvConfig->hide();
  m_vDlgEditControl.append(QPair<QDialog *, QString>(
      reinterpret_cast<QDialog *>(m_pTDPSRecvConfig), TDPS_RECV_CONFIG));

  m_pTDPSEuDataList = new TDPSEuDataList(this, TDPS_EU_LIST);
  m_pTDPSEuDataList->setObjectName("TDPS_EU_LIST");
  m_pTDPSEuDataList->hide();
  m_vDlgEditControl.append(QPair<QDialog *, QString>(
      reinterpret_cast<QDialog *>(m_pTDPSEuDataList), TDPS_EU_LIST));

  m_pDCSRecvConfig = new DCSRecvConfig(this, DCS_RECV_CONFIG);
  m_pDCSRecvConfig->setObjectName("DCS_RECV_CONFIG");
  m_pDCSRecvConfig->hide();
  m_vDlgEditControl.append(QPair<QDialog *, QString>(
      reinterpret_cast<QDialog *>(m_pDCSRecvConfig), DCS_RECV_CONFIG));
}

/**
 * @brief OperationMain::contextMenuEvent
 * @param event
 * 우클릭 팝업메뉴 생성
 */
void OperationMain::contextMenuEvent(QContextMenuEvent *event) {
  QMenu popupMenu(this);
  popupMenu.setStyleSheet(
      "background-color: rgba(221, 237, 247, 100%);"
      "selection-color: rgba(0, 0, 0, 50%);"
      "border-style: outset;"
      "border-width: 1px;"
      "border-color: black;");

  QVector<QPair<QAction *, QDialog *>> vDlgAction;

  if (m_nMode == MODE_MISSIONCONTROL) {
    for (auto itt : m_vDlgMissionControl) {
      if (itt.first != nullptr) {
        QAction *dlgAction = new QAction(itt.second, this);

        dlgAction->setCheckable(true);
        if (itt.first->isHidden()) {
          dlgAction->setCheckable(false);
        }
        dlgAction->setChecked(dlgAction->isCheckable());
        popupMenu.addAction(dlgAction);
        vDlgAction.append(QPair<QAction *, QDialog *>(dlgAction, itt.first));
      }
    }
  } else {
    for (auto itt : m_vDlgEditControl) {
      if (itt.first != nullptr) {
        QAction *dlgAction = new QAction(itt.second, this);

        dlgAction->setCheckable(true);
        if (itt.first->isHidden()) {
          dlgAction->setCheckable(false);
        }
        dlgAction->setChecked(dlgAction->isCheckable());
        popupMenu.addAction(dlgAction);
        vDlgAction.append(QPair<QAction *, QDialog *>(dlgAction, itt.first));
      }
    }
  }

  // 메뉴전시
  QAction *selectedAction = popupMenu.exec(event->globalPos());

  // 메뉴 선택 처리
  for (auto itt : vDlgAction) {
    if (selectedAction == itt.first) {
      if (itt.first->isCheckable()) {
        if (itt.second != nullptr) {
          itt.second->hide();
        }
        itt.first->setCheckable(false);

      } else {
        if (itt.second != nullptr) {
          itt.second->show();
          itt.second->setFocus();
        }
        itt.first->setCheckable(true);
      }
      itt.first->setChecked(itt.first->isCheckable());
      break;
    }
  }

  // delete Action

  for (auto itt : vDlgAction) {
    if (itt.first != nullptr) {
      delete itt.first;
    }
  }

  vDlgAction.clear();
}

void OperationMain::SetScenarioSave(TitleScenario stTitleScenario) {
  ScenarioInfo stScenarioInfo;
  stScenarioInfo.stTitleScenario = stTitleScenario;

  if (m_pTDPSRecvConfig != nullptr) {
    stScenarioInfo.vTDPSConfig = m_pTDPSRecvConfig->GetTDPSConfig();
    stScenarioInfo.nTDPSDispLimit = m_pTDPSRecvConfig->GetTDPSDispLimit();
  }

  if (m_pTDPSEuDataList != nullptr) {
    stScenarioInfo.vEuDataList = m_pTDPSEuDataList->GetEuDataScenario();
  }

  if (m_pDCSRecvConfig != nullptr) {
    stScenarioInfo.vDCSConfig = m_pDCSRecvConfig->GetDCSConfig();
    stScenarioInfo.nDCSDispLimit = m_pDCSRecvConfig->GetDCSDispLimit();
  }

  if (m_pScenarioMngr != nullptr) {
    m_pScenarioMngr->SetSaveScenarioFile(stScenarioInfo);
  }
}

void OperationMain::SetScenarioLoad(QString strFile, BOOL bEdit) {
  ScenarioInfo stScenarioInfo = m_pScenarioMngr->SetLoadScenarioFile(strFile);

  m_pParent->GetHUDDisplay()->SetScenarioName(
      stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Mission]);

  simData::DataStore::Transaction txn;
  simData::ScenarioProperties *props =
      m_pParent->GetDataStore()->mutable_scenarioProperties(&txn);
  props->mutable_classification()->set_label(
      stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_Class]
          .toLocal8Bit()
          .data());
  txn.complete(&props);

  stScenarioInfo.stTitleScenario.strTitle[TitleScenario::Title_FilePath] =
      strFile;

  if (bEdit == true) {
    if (m_pScenarioEdit != nullptr)
      m_pScenarioEdit->SetScenarioLoad(stScenarioInfo);

    if (m_pTDPSRecvConfig != nullptr)
      m_pTDPSRecvConfig->SetScenarioLoad(stScenarioInfo);

    if (m_pTDPSEuDataList != nullptr)
      m_pTDPSEuDataList->SetScenarioLoad(stScenarioInfo);

    if (m_pDCSRecvConfig != nullptr)
      m_pDCSRecvConfig->SetScenarioLoad(stScenarioInfo);

  } else {
    if (m_pScenariocontrol != nullptr)
      m_pScenariocontrol->SetScenarioLoad(stScenarioInfo);

    if (m_pTDPSList != nullptr) m_pTDPSList->SetScenarioLoad(stScenarioInfo);

    if (m_pDCSList != nullptr) m_pDCSList->SetScenarioLoad(stScenarioInfo);
  }
}

void OperationMain::SetScenarioInit() {
  /*
  if (m_pScenarioEdit != nullptr)
    m_pScenarioEdit->SetScenarioInit();
  */
  if (m_pTDPSRecvConfig != nullptr) m_pTDPSRecvConfig->SetScenarioInit();
  if (m_pTDPSEuDataList != nullptr) m_pTDPSEuDataList->SetScenarioInit();
  if (m_pDCSRecvConfig != nullptr) m_pDCSRecvConfig->SetScenarioInit();
}

void OperationMain::SetMissionStart(ScenarioInfo stScenarioInfo) {
  if (m_pDataMngr != nullptr) {
    m_pDataMngr->SetMissionStart(stScenarioInfo);
  }
}

void OperationMain::SetMissionStop() {
  if (m_pDataMngr != nullptr) {
    m_pDataMngr->SetMissionStop();
  }
}

/**
 * @brief OperationMain::paintEvent
 * @param event
 * 배경 그리기
 */
void OperationMain::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  QPainter painter(this);

  // Draw a filled rectangle

  QRect rectangle_back(0, 0, width() - 1, height() - 1);

  painter.fillRect(rectangle_back,
                   QColor(221, 237, 247));  // Set the fill color

  // Draw the outline of the rectangle
  painter.drawRect(rectangle_back);

  // painter.setPen(QColor(165, 172, 179), 5, Qt::SolidLine);
  painter.setPen(QPen(QColor(165, 172, 179), 1.0, Qt::SolidLine));
  painter.drawLine(0, 60, width(), 60);

  // Draw Text

  QFont font;
  font.setFamily("HY헤드라인M");
  font.setBold(true);
  font.setPointSize(30);

  painter.setFont(font);
  painter.setPen(Qt::black);

  QString strTitle = TEXT_MAIN_STATUS_REALTIME;
  if (m_nMode == MODE_SCENARIOEDIT) strTitle = TEXT_MAIN_STATUS_EDIT;

  painter.drawText(QRectF(0, 0, width(), 60),
                   Qt::AlignHCenter | Qt::AlignVCenter, strTitle);
}
