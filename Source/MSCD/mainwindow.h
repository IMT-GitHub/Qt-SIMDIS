#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGLWidget>
#include <QMainWindow>
#include <QTimer>
#include <QWindow>

#include "simData/DataStore.h"
#include "simUtil/MouseDispatcher.h"
#include "simUtil/StatsHandler.h"
#include "simVis/Utils.h"
#include "simVis/View.h"
#include "simVis/ViewManager.h"

class HUDDisplay;
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(simVis::ViewManager *viewMan, simData::DataStore &dataStore);
  virtual ~MainWindow();

  //////////////3차원 지도 전시 설정//////////////
  void setGlWidget(QGLWidget *glWidget);
  void paintEvent(QPaintEvent *e);

  HUDDisplay *GetHUDDisplay();
  void SetHUDDisplay();
  simData::DataStore *GetDataStore() {
    if (&dataStore_ != nullptr)
      return &dataStore_;
    else
      return nullptr;
  }

 private:
  void InitSimdisView();

 protected:
  QTimer redrawTimer_;  // 프레임 설정용 타이머
  osg::ref_ptr<simVis::ViewManager> viewManager_;  // 지도 View 관리
  simData::DataStore &dataStore_;                  // 지도 데이터 관리
  osg::ref_ptr<simUtil::StatsHandler>
      _statsHandler;   // status 핸들러(무슨 Status?)
  QWindow *glWindow_;  // Window Handle

  HUDDisplay *m_pHUDDisplay;

 public Q_SLOTS:
  void setTimerInterval(int value) { redrawTimer_.setInterval(value); }
  void toggleFrameRate(bool turnOn) {
    // Pick the StatsType based on turnOn flag
    simUtil::StatsHandler::StatsType type =
        (turnOn ? simUtil::StatsHandler::FRAME_RATE
                : simUtil::StatsHandler::NO_STATS);
    // Update the stats type in the handler
    _statsHandler->setStatsType(type, viewManager_->getView(0));
  }
  void setUpdate();
  ///////////////////------////////////////////
};
#endif  // MAINWINDOW_H
