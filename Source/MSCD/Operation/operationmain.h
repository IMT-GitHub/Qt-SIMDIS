#ifndef OPERATIONMAIN_H
#define OPERATIONMAIN_H

#include <QContextMenuEvent>
#include <QDialog>
#include <QPair>
#include <QPushButton>
#include <QVector>

#include "../Common/Struct.h"
#include "../Common/define.h"
class DataMngr;
class ScenarioMngr;
class MainWindow;
// 시험 통제
class Scenariocontrol;
class TDPSList;
class DCSList;
// 시나리오편집
class ScenarioEdit;
class TDPSRecvConfig;
class TDPSEuDataList;
class DCSRecvConfig;
namespace Ui {
class OperationMain;
}

class OperationMain : public QDialog {
  Q_OBJECT
  enum {
    BTN_MISSION = 0,
    BTN_EDIT,
    BTN_POSINIT,
    BTN_POSLOAD,
    BTN_POSSAVE,
    BTN_CLOSE,
    BTN_CNT,

    MODE_MISSIONCONTROL = 0,
    MODE_SCENARIOEDIT,
  };

 public:
  explicit OperationMain(QWidget *parent = nullptr);
  ~OperationMain();
  void contextMenuEvent(QContextMenuEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void GetDataMngr(DataMngr *pDataMngr);
  void GetScenarioMngr(ScenarioMngr *pScenarioMngr);
  void SetInitSubDlg();
  void SetInitSubDlg_Mission();
  void SetInitSubDlg_Edit();
  TDPSList *GetTDPSList() { return m_pTDPSList; };
  DCSList *GetDCSList() { return m_pDCSList; };
  void SetScenarioSave(TitleScenario stTitleScenario);
  void SetScenarioLoad(QString strFile, BOOL bEdit = true);
  void SetScenarioInit();
  void SetMissionStart(ScenarioInfo stScenarioInfo);
  void SetMissionStop();

 private:
  void CreateBtn();
  void SetLoadDlgPos();
  void SetModeShowPopup();

 private slots:
  void MISSION_BTN_clicked();
  void EDIT_BTN_clicked();
  void POSINIT_BTN_clicked();
  void POSLOAD_BTN_clicked();
  void POSSAVE_BTN_clicked();
  void CLOSE_BTN_clicked();

 private:
  Ui::OperationMain *ui;

  QPushButton *m_pBtn[BTN_CNT];
  // Popup Dialog
  QVector<QPair<QDialog *, QString>> m_vDlgMissionControl;  // QDialog
  QVector<QPair<QDialog *, QString>> m_vDlgEditControl;     // QDialog

  // 시험 통제 팝업 다이얼로그
  Scenariocontrol *m_pScenariocontrol;
  TDPSList *m_pTDPSList;
  DCSList *m_pDCSList;

  // 시나리오 편집 팝업 다이얼로그
  ScenarioEdit *m_pScenarioEdit;
  TDPSRecvConfig *m_pTDPSRecvConfig;
  TDPSEuDataList *m_pTDPSEuDataList;
  DCSRecvConfig *m_pDCSRecvConfig;

  // 수신 데이터 관리
  DataMngr *m_pDataMngr;

  // 시나리오 파일관리
  ScenarioMngr *m_pScenarioMngr;

  // 부모 클래스 포인터
  MainWindow *m_pParent;
  int m_nMode;
};

#endif  // OPERATIONMAIN_H
