#include "huddisplay.h"

#include <simCore/String/TextReplacer.h>
#include <simCore/Time/Utils.h>
#include <simUtil/StatusText.h>
#include <simVis/ClassificationBanner.h>
#include <simVis/Compass.h>
#include <simVis/Registry.h>
#include <simVis/Utils.h>
#include <simVis/View.h>

#include <osgEarth/CullingUtils>

#include "HUDDisplay_Legend.h"
#include "simUtil/ExampleResources.h"

static const std::string KEY_MAP_SCALE = "MapScale";
static const std::string KEY_STATUS_TEXT = "StatusText";
static const std::string KEY_DEMO_TEXT = "DemoText";
static const std::string KEY_CLASSIFICATION_TOP = "ClassificationTop";
static const std::string KEY_CLASSIFICATION_BOTTOM = "ClassificationBottom";
static const std::string KEY_COMPASS = "Compass";
static const std::string KEY_LEGEND = "Legend";

HUDDisplay::HUDDisplay(simVis::ViewManager* viewMan,
                       simData::DataStore& dataStore)
    : m_pViewManager(viewMan), m_dataStore(dataStore), m_pScenrioName(nullptr) {
  InitHUDView();
  SetStatusHUD();
  SetCompasHUD();
  SetTopTextHUD();
  SetBottomTextHUD();
  // SetLegendHUD();

  // hudEditor_.setVisible(!hudEditor_.isVisible());
}

HUDDisplay::~HUDDisplay() {
  /*if (m_pScenrioName != nullptr) {
    delete m_pScenrioName;
    m_pScenrioName = nullptr;
  }*/
}

/**
 * @brief HUDDisplay::InitHUDView
 * HUD View 설정
 */

void HUDDisplay::InitHUDView() {
  // auto mainView = viewManager_->getView(0);
  auto superHUD = m_pViewManager->getView(1);
  // simUtil::MouseDispatcher mouseDispatcher;
  m_mouseDispatcher.setViewManager(m_pViewManager);

  // For osgEarth::LineDrawable to work on SuperHUD, need an
  // InstallCameraUniform
  if (superHUD != nullptr) {
    superHUD->getOrCreateHUD()->addCullCallback(
        new osgEarth::InstallCameraUniform());
    // Configure the HUD Editor properly
    // simUtil::HudPositionEditor hudEditor;
    m_pHud = m_hudEditor.hud();
    m_hudEditor.bindAll(*superHUD->getOrCreateHUD(), m_mouseDispatcher, -100);
  }
}

/**
 * @brief HUDDisplay::SetStatusHUD
 * Map Status HUD 전시
 */
void HUDDisplay::SetStatusHUD() {
  // Configure text replacement variables that will be used for status text
  auto mainView = m_pViewManager->getView(0);
  auto superHUD = m_pViewManager->getView(1);

  // 시간정보 추가 필요(시험시 - 현재시간, 리허설시 - 시나리오 시간)
  clock_ = new simCore::ClockImpl;
  simVis::Registry::instance()->setClock(clock_);
  clock_->setMode(
      simCore::Clock::MODE_FREEWHEEL,
      simCore::TimeStamp(1970, simCore::getSystemTime() - _timezone));

  m_dataStore.bindToClock(clock_);
  m_dataStore.setDataLimiting(true);

  // Update the scene manager with clock time
  // 태양 위치설정
  /*
  clock_->registerTimeCallback(simCore::Clock::TimeObserverPtr(
      new simExamples::SkyNodeTimeUpdater(mainView->getSceneManager())));
  */
  // Update the clock on an event callback

  mainView->getSceneManager()->addUpdateCallback(
      new simExamples::IdleClockCallback(*clock_, m_dataStore));

  timeVariable_ = new simUtil::TimeVariable(*clock_);
  timeVariable_ = new simUtil::TimeVariable(*clock_);
  timeVariable_->setFormat(simCore::TIMEFORMAT_MONTHDAY);

  simCore::TextReplacerPtr textReplacer(new simCore::TextReplacer());
  textReplacer->addReplaceable(timeVariable_);
  textReplacer->addReplaceable(new simUtil::AzimuthVariable(mainView));
  textReplacer->addReplaceable(new simUtil::ElevationVariable(mainView));
  textReplacer->addReplaceable(new simUtil::LatitudeVariable(mainView));
  textReplacer->addReplaceable(new simUtil::LongitudeVariable(mainView));
  textReplacer->addReplaceable(new simUtil::AltitudeVariable(mainView));

  // Add status text
  osg::ref_ptr<simUtil::StatusTextNode> statusText =
      new simUtil::StatusTextNode(textReplacer);
  statusText->setStatusSpec(
      "Time:\t%TIME% \n"
      "Azimuth:\t%AZ% \n"
      "Elevation:\t%EL% \n"
      "Latitude:\t%LAT% \n"
      "Longitude:\t%LON% \n"
      "Altitude:\t%ALT% \n");

  superHUD->getOrCreateHUD()->addChild(statusText.get());
  m_pHud->addWindow(KEY_STATUS_TEXT, osg::Vec2d(0.005, 0.005),
                    new simUtil::RepositionMatrixPxCallback(statusText.get()));
  // Estimate the size.  No need to be exact at this time.
  m_pHud->setSize(KEY_STATUS_TEXT, osg::Vec2d(0.0, 0.0), osg::Vec2d(300, 150));
}

/**
 * @brief HUDDisplay::SetCompasHUD
 * 콤파스 HUD 전시
 */
void HUDDisplay::SetCompasHUD() {  // Add Compass

  auto mainView = m_pViewManager->getView(0);
  auto superHUD = m_pViewManager->getView(1);

  osg::ref_ptr<simVis::CompassNode> compass =
      new simVis::CompassNode("compass.png");
  compass->setActiveView(mainView);
  // compass->setWindVaneVisible(!compass->isWindVaneVisible());

  std::shared_ptr<simVis::UpdateWindVaneListener> dsUpdate(
      new simVis::UpdateWindVaneListener(compass.get()));
  m_dataStore.addScenarioListener(dsUpdate);

  // Adjust the anchor position so the compass lower-right is at (1.0,0.0)
  static const float WIDTH_PX = 128.f;
  static const float BUFFER_PX = 25.f;
  const float OFFSET = WIDTH_PX * 0.5f + BUFFER_PX;
  compass->setMatrix(osg::Matrix::translate(osg::Vec3f(-OFFSET, OFFSET, 0.f)));

  // Wrap it in a transform to allow it to tie into the unified HUD easily
  osg::MatrixTransform* xform = new osg::MatrixTransform();
  xform->addChild(compass.get());
  superHUD->getOrCreateHUD()->addChild(xform);
  m_pHud->addWindow(KEY_COMPASS, osg::Vec2d(1.0, 0.0),
                    new simUtil::RepositionMatrixPxCallback(xform));
  const float TOTAL_PX = WIDTH_PX + BUFFER_PX;
  m_pHud->setSize(KEY_COMPASS, osg::Vec2d(-TOTAL_PX, BUFFER_PX),
                  osg::Vec2d(-BUFFER_PX, TOTAL_PX));
}

/**
 * @brief HUDDisplay::SetTopTextHUD
 * 상단 Text(시험명) HUD 전시
 */

void HUDDisplay::SetScenarioName(QString strScenarioName) {
  if (m_pScenrioName != nullptr)
    m_pScenrioName->setText(strScenarioName.toStdString());
}
void HUDDisplay::SetTopTextHUD() {
  if (m_pScenrioName == nullptr) {
    auto superHUD = m_pViewManager->getView(1);

    m_pScenrioName = new osgText::Text();
    m_pScenrioName->setText("-");

    m_pScenrioName->setFont("arialbd.ttf");
    m_pScenrioName->setAlignment(osgText::TextBase::CENTER_TOP);
    m_pScenrioName->setCharacterSize(simVis::osgFontSize(24.f));
    m_pScenrioName->setCharacterSizeMode(osgText::TextBase::SCREEN_COORDS);
    m_pScenrioName->setAxisAlignment(osgText::TextBase::SCREEN);
    m_pScenrioName->setColor(
        simVis::ColorUtils::RgbaToVec4(0xffff0080));  // 0xffffff80
    m_pScenrioName->setBackdropType(osgText::Text::OUTLINE);
    m_pScenrioName->setBackdropColor(osg::Vec4f(0.f, 0.f, 0.f, 1.f));
    m_pScenrioName->setBackdropOffset(0.03f);  // OUTLINE_THICKNESS
    m_pScenrioName->setDataVariance(osg::Object::DYNAMIC);

    osg::MatrixTransform* xform = new osg::MatrixTransform();
    xform->addChild(m_pScenrioName);
    superHUD->getOrCreateHUD()->addChild(xform);
    m_pHud->addWindow(KEY_CLASSIFICATION_TOP, osg::Vec2d(0.5, 0.995),
                      new simUtil::RepositionMatrixPxCallback(xform));
    const osg::BoundingBox& bbox = m_pScenrioName->getBoundingBox();
    m_pHud->setSize(KEY_CLASSIFICATION_TOP,
                    osg::Vec2d(bbox.xMin(), bbox.yMin()),
                    osg::Vec2d(bbox.xMax(), bbox.yMax()));
  }
}

/**
 * @brief HUDDisplay::SetBottomTextHUD
 * 하단 Text(등급명) HUD 전시
 */
void HUDDisplay::SetBottomTextHUD() {
  auto superHUD = m_pViewManager->getView(1);

  osg::ref_ptr<simVis::ClassificationLabelNode> bottom =
      new simVis::ClassificationLabelNode();
  bottom->bindTo(&m_dataStore);
  bottom->setAlignment(osgText::TextBase::CENTER_BOTTOM);
  osg::MatrixTransform* xform = new osg::MatrixTransform();
  xform->addChild(bottom.get());
  superHUD->getOrCreateHUD()->addChild(xform);
  m_pHud->addWindow(KEY_CLASSIFICATION_BOTTOM, osg::Vec2d(0.5, 0.005),
                    new simUtil::RepositionMatrixPxCallback(xform));
  const osg::BoundingBox& bbox = bottom->getBoundingBox();
  m_pHud->setSize(KEY_CLASSIFICATION_BOTTOM,
                  osg::Vec2d(bbox.xMin(), bbox.yMin()),
                  osg::Vec2d(bbox.xMax(), bbox.yMax()));
}

/**
 * @brief HUDDisplay::SetLegendHUD
 * 범례 HUD 전시
 */
void HUDDisplay::SetLegendHUD() {
  auto superHUD = m_pViewManager->getView(1);

  // Create a legend that is 4x2
  simUtil::GridTransform* legendGrid = new simUtil::GridTransform(2, true);
  legendGrid->setSpacing(6.f);
  superHUD->getOrCreateHUD()->addChild(legendGrid);

  // Add 4 entries
  legendGrid->addChild(new LegendIconCell);
  legendGrid->addChild(new LegendTextCell("Entity 1", "Content for entity 1"));
  // legendGrid->addChild(new LegendIconCell);
  // legendGrid->addChild(new LegendTextCell("Entity 2", "[none]"));
  // legendGrid->addChild(new LegendIconCell);
  /*
  legendGrid->addChild(new LegendTextCell(
      "Entity 3",
      "Multi-line content\nEntity 3\nhas multiple lines of text.\nThere are 4 "
      "lines of text in this legend entry."));
  legendGrid->addChild(new LegendIconCell);
  legendGrid->addChild(new LegendTextCell("Entity 4", "Content"));
  */

  // Adjust size
  legendGrid->setSize(legendGrid->getDefaultWidth(),
                      legendGrid->getDefaultHeight());

  // Add the text to the HUD at 80% / 50%
  m_pHud->addWindow(KEY_LEGEND, osg::Vec2d(0.8, 0.5),
                    new simUtil::RepositionMatrixPxCallback(legendGrid));
  m_pHud->setSize(KEY_LEGEND, osg::Vec2d(0.0, 0.0),
                  osg::Vec2d(legendGrid->width(), legendGrid->height()));
}
