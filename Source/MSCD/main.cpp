#define _WINSOCKAPI_
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QMainWindow>
#include <QMenuBar>
#include <QSignalMapper>
#include <QStatusBar>
#include <QTranslator>

#include "DataMngr/datamngr.h"
#include "Operation/operationmain.h"
#include "ScenarioMngr/scenariomngr.h"
#include "mainwindow.h"
#include "simCore/Common/HighPerformanceGraphics.h"
#include "simCore/Common/Version.h"
#include "simCore/String/Utils.h"
#include "simData/MemoryDataStore.h"
#include "simQt/ViewWidget.h"
#include "simUtil/DbConfigurationFile.h"
#include "simUtil/DefaultDataStoreValues.h"
#include "simUtil/ExampleResources.h"
#include "simUtil/MouseDispatcher.h"
#include "simVis/Gl3Utils.h"
#include "simVis/Registry.h"
#include "simVis/Scenario.h"
#include "simVis/SceneManager.h"
#include "simVis/View.h"
#include "simVis/ViewManagerLogDbAdapter.h"

#ifdef WIN32
const char PATH_SEP = '\\';
#else
const char PATH_SEP = '/';
#endif

struct STApplication {
  osg::ref_ptr<simVis::View> mainView;
  // osg::ref_ptr<simVis::View> mainRttView;
  osg::ref_ptr<simVis::View> insetView;
  // osg::ref_ptr<simVis::View> insetRttView;
};

struct FrameRateAction : public QAction {
  FrameRateAction(MainWindow* win, QSignalMapper& signalMapper, int frameRateHz)
      : QAction(win) {
    int interval = 0;
    if (frameRateHz != 0) {
      setText(tr("%1 Hertz").arg(frameRateHz));
      interval = 1000 / frameRateHz;
    } else {
      setText(tr("Unlimited"));
    }

    signalMapper.setMapping(this, interval);
    connect(this, SIGNAL(triggered()), &signalMapper, SLOT(map()));
    setCheckable(true);
  }
};

void CoustomConfigureSearchPaths() {
  const std::string folders[] = {
      "aqm-37c",
      "as-17_krypton",
      "decoys",
      "dragon_eye",
      "EKV",
      "mm-38_exocet",
      "mm-40_exocet",
      "nulka",
      "OBV",
      "OSP",
      "SRALT",
      "STARS",
      "UGV",
      "USSV",
      "imageIcons",
      "ships",
      std::string("ships") + PATH_SEP + "textures",
      ""  // Placeholder last item
  };

  const std::string& basePath = simExamples::getSampleDataPath();
  const std::string modelPath = basePath + PATH_SEP + "models";

#ifndef WIN32
  {
    // On Linux, add a search path for libraries relative to executable path
    // (installDir/bin)
    osgDB::FilePathList libPaths = osgDB::getLibraryFilePathList();
    // ../lib/amd64-linux is used by SIMDIS applications distributed by NRL
    libPaths.push_back("../lib/amd64-linux");
    // SDK examples from an SDK build need ../lib in the libpath
    libPaths.push_back("../lib");
    osgDB::setLibraryFilePathList(libPaths);
  }
#endif

  simVis::Registry* simVisRegistry = simVis::Registry::instance();
  simVis::FilePathList pathList;
  simVisRegistry->getModelSearchPaths(pathList);

  // Add variables from SIMDIS_SDK_FILE_PATH
  pathList.push_back(basePath);
  pathList.push_back(basePath + PATH_SEP + "textures");
  pathList.push_back(modelPath);

  // Add all of the directories for SIMDIS_SDK_FILE_PATH models
  for (size_t k = 0; !folders[k].empty(); ++k)
    pathList.push_back(modelPath + PATH_SEP + folders[k]);

  // Add SIMDIS_DIR variables
  // SIMDIS 배포판 data 참조부분
  // "SIMDIS_DIR" -> 배포판 위치정보
  const std::string SIMDIS_DIR = simCore::getEnvVar("SIMDIS_DIR");
  if (!SIMDIS_DIR.empty()) {
    const std::string dataDir = SIMDIS_DIR + PATH_SEP + "data";
    const std::string simdisDirModelFolders[] = {
        "aircraft",
        "decoy",
        "equipment",
        "imageIcons",
        std::string("imageIcons") + PATH_SEP + "NTDS",
        std::string("imageIcons") + PATH_SEP + "NTDS" + PATH_SEP + "jreap",
        std::string("imageIcons") + PATH_SEP + "NTDS" + PATH_SEP + "large",
        std::string("imageIcons") + PATH_SEP + "NTDS" + PATH_SEP + "small",
        std::string("imageIcons") + PATH_SEP + "SCORE",
        "missiles",
        "other",
        "satellite",
        "ships",
        "sites",
        "vehicles",
        ""  // Placeholder last item
    };
    // Iterate over the folders, adding them all to the search path
    for (size_t k = 0; !simdisDirModelFolders[k].empty(); ++k)
      pathList.push_back(dataDir + PATH_SEP + "models" + PATH_SEP +
                         simdisDirModelFolders[k]);
    // Add textures directory to the search path
    pathList.push_back(dataDir + PATH_SEP + "textures" + PATH_SEP + "models");
  }
  // Save model path
  simVis::FilePathList modelPathList(pathList.begin(), pathList.end());

  // Next, configure the data file path list, which is separate from the model
  // path list
  pathList = osgDB::getDataFilePathList();
  if (!SIMDIS_DIR.empty()) {
    // SIMDIS SDK shaders are placed here; SIMDIS shaders override data/osgEarth
    // shaders
    pathList.push_back(SIMDIS_DIR + PATH_SEP + "data" + PATH_SEP + "shaders");
    // osgEarth textures, including the moon
    pathList.push_back(SIMDIS_DIR + PATH_SEP + "data" + PATH_SEP + "osgEarth");
    // SIMDIS model textures
    pathList.push_back(SIMDIS_DIR + PATH_SEP + "data" + PATH_SEP + "textures" +
                       PATH_SEP + "modelsFull");
    pathList.push_back(SIMDIS_DIR + PATH_SEP + "data" + PATH_SEP + "textures" +
                       PATH_SEP + "models");
    // SIMDIS textures
    pathList.push_back(SIMDIS_DIR + PATH_SEP + "data" + PATH_SEP + "textures" +
                       PATH_SEP + "app");
    // osgText looks under data directory for fonts/fontname.ttf -- add data for
    // data/fonts folder
    pathList.push_back(SIMDIS_DIR + PATH_SEP + "data");

    // GOG files under data/GOG
    pathList.push_back(SIMDIS_DIR + PATH_SEP + "data" + PATH_SEP + "GOG");
  }
  pathList.push_back(basePath + PATH_SEP + "textures");

  // Add the user dir to the end of the path lists to be searched last
  const std::string simdisUserDir = simCore::getEnvVar("SIMDIS_USER_DIR");
  if (!simdisUserDir.empty()) {
    modelPathList.push_back(simdisUserDir);
    pathList.push_back(simdisUserDir);
  }

  // Set the model and data path lists
  simVisRegistry->setModelSearchPaths(modelPathList);
  osgDB::setDataFilePathList(pathList);

  // Set the environment variable for proj4 if it's not set already
  const std::string PROJ_LIB = simCore::getEnvVar("PROJ_LIB");
  if (PROJ_LIB.empty() || !osgDB::fileExists(PROJ_LIB + "/proj/proj.db")) {
    // First try to use the SIMDIS_DIR, then fall back to SIMDIS_SDK_DATA_PATH
    if (!SIMDIS_DIR.empty() &&
        osgDB::fileExists(SIMDIS_DIR + "/data/proj/proj.db"))
      simCore::setEnvVar("PROJ_LIB", SIMDIS_DIR + "/data/proj", true);
    else if (!basePath.empty() && osgDB::fileExists(basePath + "/proj/proj.db"))
      simCore::setEnvVar("PROJ_LIB", basePath + "/proj", true);
  }

  // Fix the GL3 version
  simVis::applyMesaGlVersionOverride();
}

int SetSimdisView(int argc, char** argv) {
  simCore::checkVersionThrow();
  // simExamples::configureSearchPaths();
  CoustomConfigureSearchPaths();

  // a Map and a Scene Manager:
  // osg::ref_ptr<osgEarth::Map> map = simExamples::createDefaultExampleMap();
  osg::ref_ptr<simVis::SceneManager> sceneMan = new simVis::SceneManager();
  sceneMan->setMap(simExamples::createDefaultExampleMap());

  // add sky node
  simExamples::addDefaultSkyNode(sceneMan.get());

  // A view to embed in our widget:
  osg::ref_ptr<simVis::View> mainView = new simVis::View();
  mainView->setSceneManager(sceneMan.get());
  mainView->setNavigationMode(simVis::NAVMODE_ROTATEPAN);
  // Note that no debug handlers are installed, because we cycle through frame
  // rate in menu

  //  The ViewManager coordinates the rendering of all our views.
  osg::ref_ptr<simVis::ViewManager> viewMan = new simVis::ViewManager();

  // Create a mouse dispatcher for the HUD Editor
  simUtil::MouseDispatcher mouseDispatcher;
  mouseDispatcher.setViewManager(viewMan);

  // Add a new "top-level" view. A top-level view can have inset views, and
  // also has a HUD stack for overlay text and graphics.
  viewMan->addView(mainView.get());

  // Set up the logarithmic depth buffer for all views
  osg::ref_ptr<simVis::ViewManagerLogDbAdapter> logDb =
      new simVis::ViewManagerLogDbAdapter;
  logDb->install(viewMan.get());

  // Add a dataStore for the platform
  simData::MemoryDataStore dataStore;
  sceneMan->getScenario()->bind(&dataStore);

  simData::DataStore::Transaction txn;
  simData::ScenarioProperties* props =
      dataStore.mutable_scenarioProperties(&txn);
  props->mutable_classification()->set_fontcolor(
      0x00ff0080);  // Transparent green, RRGGBBAA
  props->mutable_classification()->set_label("Unknown");
  txn.complete(&props);

#ifdef Q_WS_X11
  // required for multi-threaded viewer on Linux:
  XInitThreads();
#endif

  QApplication app(argc, argv);

  QFile file(":/file/MSCD.qss");
  file.open(QFile::ReadOnly);
  QString styleSheet{file.readAll()};
  app.setStyleSheet(styleSheet);

  MainWindow win(viewMan.get(), dataStore);
  simQt::ViewWidget* viewWidget = new simQt::ViewWidget(mainView.get());

  QStatusBar statusbar;
  win.setStatusBar(&statusbar);
  // win.statusBar();
  statusbar.hide();
  QDesktopWidget* desktop = QApplication::desktop();
  QRect screenGeometry = desktop->screenGeometry(0);
  win.setGlWidget(viewWidget);
  win.setGeometry(screenGeometry.x(), screenGeometry.y(), 100, 100);
  // win.showMaximized();
  win.showFullScreen();

  win.toggleFrameRate(true);
  win.setTimerInterval(1);

  // HUD, Inset View는 QT widget 전시 이후 추가 필요

  // Create the SuperHUD
  osg::ref_ptr<simVis::View> superHud = new simVis::View();
  bool bResult = superHud->setUpViewAsHUD(mainView);  // 확인
  viewMan->addView(superHud.get());

  win.SetHUDDisplay();

  // win.showMaximized();
  // win.setWindowFlags(Qt::FramelessWindowHint);

  // top menubar, bottom statusbar
  /*
  QSignalMapper mapper(&app);
  QObject::connect(&mapper, SIGNAL(mapped(int)), &win,
                   SLOT(setTimerInterval(int)));

  win.statusBar()->showMessage(QString(
      "Congratulations! You've embedded the SDK Viewer in a Qt Widget."));

  QMenu* bar = win.menuBar()->addMenu(QObject::tr("Frame Rate"));

  QAction* toggleFrameRateAction =
      new QAction(QObject::tr("Show Frame Rate"), &win);
  toggleFrameRateAction->setShortcut(QKeySequence("Alt+F"));
  bar->addAction(toggleFrameRateAction);
  QObject::connect(toggleFrameRateAction, SIGNAL(toggled(bool)), &win,
                   SLOT(toggleFrameRate(bool)));
  bar->addSeparator()->setText(QObject::tr("Rates"));

  QActionGroup* actionGroup = new QActionGroup(&win);
  actionGroup->addAction(new FrameRateAction(&win, mapper, 1));
  actionGroup->addAction(new FrameRateAction(&win, mapper, 10));
  actionGroup->addAction(new FrameRateAction(&win, mapper, 15));
  actionGroup->addAction(new FrameRateAction(&win, mapper, 30));
  actionGroup->addAction(new FrameRateAction(&win, mapper, 60));
  actionGroup->addAction(new FrameRateAction(&win, mapper, 120));
  actionGroup->addAction(new FrameRateAction(&win, mapper, 0));
  actionGroup->setExclusive(true);
  bar->addActions(actionGroup->actions());

  // Activate the Unlimited Hz
  actionGroup->actions().at(6)->trigger();
 */
  // 운용화면 전시
  // QDesktopWidget* desktop = QApplication::desktop();
  screenGeometry = desktop->screenGeometry(1);

  OperationMain operationWidget(&win);
  // operationWidget.resize(100, 100);
  operationWidget.setGeometry(screenGeometry.x(), screenGeometry.y() + 30, 1920,
                              1680);

  operationWidget.show();
  operationWidget.SetInitSubDlg();
  //

  DataMngr dataMngr(dataStore, operationWidget);
  ScenarioMngr scenaroMngr;
  operationWidget.GetDataMngr(&dataMngr);
  operationWidget.GetScenarioMngr(&scenaroMngr);

  win.show();
  app.exec();
  // delete action;
  delete viewWidget;
  return 0;
}

int main(int argc, char* argv[]) {
  return SetSimdisView(argc, argv);
  /*
QApplication a(argc, argv);
MainWindow w;
w.show();
return a.exec();
*/
}
