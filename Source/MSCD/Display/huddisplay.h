#ifndef HUDDISPLAY_H
#define HUDDISPLAY_H

#include <simCore/Time/ClockImpl.h>
#include <simData/DataStore.h>
#include <simUtil/HudPositionEditor.h>
#include <simUtil/HudPositionManager.h>
#include <simUtil/MouseDispatcher.h>
#include <simUtil/Replaceables.h>
#include <simVis/ViewManager.h>

#include <QObject>
#include <osgText/Text>
class HUDDisplay {
 public:
  HUDDisplay(simVis::ViewManager* viewMan, simData::DataStore& dataStore);
  ~HUDDisplay();
  void SetScenarioName(QString strScenarioName);

 private:
  void InitHUDView();
  void SetStatusHUD();
  void SetCompasHUD();
  void SetTopTextHUD();
  void SetBottomTextHUD();
  void SetLegendHUD();

 private:
  osg::ref_ptr<simVis::ViewManager> m_pViewManager;  // 지도 View 관리
  simData::DataStore& m_dataStore;                   // 지도 데이터 관리
  osg::ref_ptr<simUtil::HudPositionManager> m_pHud;

  osg::ref_ptr<osgText::Text> m_pScenrioName;
  simUtil::HudPositionEditor m_hudEditor;
  simUtil::MouseDispatcher m_mouseDispatcher;

  // HUD Timer
  simCore::ClockImpl* clock_;
  simUtil::TimeVariable* timeVariable_;
};

#endif  // HUDDISPLAY_H
