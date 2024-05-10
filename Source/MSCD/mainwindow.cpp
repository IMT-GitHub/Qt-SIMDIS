#include "mainwindow.h"

#include <QDebug>

#include "Common/define.h"
#include "Display/huddisplay.h"

MainWindow::MainWindow(simVis::ViewManager* viewMan,
                       simData::DataStore& dataStore)
    : viewManager_(viewMan),
      dataStore_(dataStore),
      glWindow_(nullptr),
      m_pHUDDisplay(nullptr),
      _statsHandler(nullptr) {
  InitSimdisView();
  setWindowTitle(MAINTITLE);
}

MainWindow::~MainWindow() {
  if (_statsHandler != nullptr) {
    _statsHandler.release();
    delete _statsHandler;
    _statsHandler = nullptr;
  }

  if (m_pHUDDisplay != nullptr) {
    delete m_pHUDDisplay;
    m_pHUDDisplay = nullptr;
  }
}

void MainWindow::setGlWidget(QGLWidget* glWidget) {
  setCentralWidget(glWidget);
  glWindow_ = glWidget->windowHandle();
}

// Simdis 지도 초기화
void MainWindow::InitSimdisView() {
  // disable the default ESC-to-quit event:
  viewManager_->getViewer()->setKeyEventSetsDone(0);
  viewManager_->getViewer()->setQuitEventSetsDone(false);

  // timer fires a paint event.
  // connect(&redrawTimer_, SIGNAL(timeout()), this, SLOT(update()));
  connect(&redrawTimer_, SIGNAL(timeout()), this, SLOT(setUpdate()));
  //     timer single shot to avoid infinite loop problems in Qt on MSVC11
  //  redrawTimer_.setSingleShot(true);
  redrawTimer_.start();

  _statsHandler = new simUtil::StatsHandler;
  simVis::fixStatsHandlerGl2BlockyText(_statsHandler.get());
  osg::observer_ptr<simVis::View> mainView = viewManager_->getView(0);
  if (mainView.valid()) {
    mainView->addEventHandler(_statsHandler.get());

    // Set an initial viewpoint near the data
    simVis::Viewpoint vp;
    vp.focalPoint()->set(osgEarth::SpatialReference::get("wgs84"),
                         osg::Vec3d(129, 37, 0.0), osgEarth::ALTMODE_ABSOLUTE);
    vp.pitch()->set(-90.0, osgEarth::Units::DEGREES);
    vp.heading()->set(0.0, osgEarth::Units::DEGREES);
    vp.range()->set(300000, osgEarth::Units::METERS);
    mainView->setViewpoint(vp);
  }
}
void MainWindow::setUpdate() {
  if (glWindow_ && glWindow_->isExposed()) viewManager_->frame();
  // redrawTimer_.start();
}
// 지도 전시 설정
void MainWindow::paintEvent(QPaintEvent* e) {
  // refresh all the views.
  if (glWindow_ && glWindow_->isExposed()) viewManager_->frame();
  redrawTimer_.start();
}

void MainWindow::SetHUDDisplay() {
  if (m_pHUDDisplay == nullptr) {
    m_pHUDDisplay = new HUDDisplay(viewManager_, dataStore_);
  }
}

HUDDisplay* MainWindow::GetHUDDisplay() { return m_pHUDDisplay; }
