#ifndef HUDDISPLAY_LEGEND_H
#define HUDDISPLAY_LEGEND_H

#include "simVis/SceneManager.h"
#endif  // HUDDISPLAY_LEGEND_H

#include <simCore/Calc/Math.h>>
#include <simUtil/GridTransform.h>
#include <simVis/Scenario.h>
#include <simVis/Utils.h>
#include <simVis/View.h>
/** size of square for legend icons */
static const float SQUARE_SIZE = 48.f;

/** Helper method that creates a colored square from (0,0) to (width,width) */
osg::Geometry* newSquare(const osg::Vec4f& color, float width = 1.f) {
  osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
  osg::Vec3Array* verts = new osg::Vec3Array;
  verts->push_back(osg::Vec3f(0.f, 0.f, 0.f));
  verts->push_back(osg::Vec3f(width, 0.f, 0.f));
  verts->push_back(osg::Vec3f(width, width, 0.f));
  verts->push_back(osg::Vec3f(0.f, width, 0.f));
  geom->setVertexArray(verts);
  osg::Vec4Array* colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);
  colors->push_back(color);
  geom->setColorArray(colors);
  geom->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_FAN, 0, 4));

  /*
  osg::observer_ptr<simVis::View> view;
  osg::ref_ptr<simVis::SceneManager> scene = view->getSceneManager();
  osg::ref_ptr<simVis::PlatformNode> platformNode =
      scene->getScenario()->find<simVis::PlatformNode>(2);
  osg::RenderInfo render;
  view->tetherCamera(platformNode.get());
  render.setView(view);
  geom->draw(render);
 */

  return geom.release();
}

/** Draws a green square of fixed size at upper-left of cell. */
class LegendIconCell : public simUtil::GridCell {
 public:
  LegendIconCell() {
    // Placeholder, just use a green-ish square
    addChild(newSquare(osg::Vec4f(0.f, 0.6f, 0.f, 1.f), SQUARE_SIZE));
    // Configure the default size
    setDefaultSize(SQUARE_SIZE, SQUARE_SIZE);
    // Icon cells have a fixed width
    setFixedWidth(SQUARE_SIZE);
  }

 protected:
  /** Similar to GridCell::setPositionImpl_(), but do not scale contents, and
   * positions a little different. */
  virtual void setPositionImpl_() {
    osg::Matrix m;
    // Do not scale contents; we're already at right pixel size.
    // Translate so upper-left corner is at (height) pixels.
    m.postMult(osg::Matrix::translate(
        osg::Vec3f(x(), y() + height() - SQUARE_SIZE, 0.f)));
    setMatrix(m);
  }
};

/** Encapsulates the content and label text for a single cell in the legend */
class LegendTextCell : public simUtil::GridCell {
 public:
  LegendTextCell(const std::string& title, const std::string& content)
      : title_(new osgText::Text) {
    // Create the title label
    title_->setText(title);
    title_->setAlignment(osgText::TextBase::LEFT_TOP);
    title_->setAxisAlignment(osgText::TextBase::SCREEN);
    title_->setAutoRotateToScreen(true);
    title_->setCharacterSize(simVis::osgFontSize(16.f));
    title_->setColor(osg::Vec4f(1.f, 1.f, 1.f, 1.f));
    title_->setFont("arialbd.ttf");
    title_->setBackdropColor(osg::Vec4f(0.f, 0.f, 0.f, 1.f));
    title_->setBackdropType(osgText::Text::DROP_SHADOW_BOTTOM_RIGHT);

    // Create the content label
    content_ = new osgText::Text(*title_);
    content_->setText(content);
    content_->setCharacterSize(simVis::osgFontSize(10.f));

    // Add the children
    addChild(title_.get());
    addChild(content_.get());

    // Assign the width and height based on title/outline size
    const auto& titleBb = title_->getBoundingBox();
    const auto& contentBb = content_->getBoundingBox();
    const osg::Vec2f titleSize(titleBb.xMax() - titleBb.xMin(),
                               titleBb.yMax() - titleBb.yMin());
    const osg::Vec2f contentSize(contentBb.xMax() - contentBb.xMin(),
                                 contentBb.yMax() - contentBb.yMin());
    setDefaultSize(simCore::sdkMax(titleSize.x(), contentSize.x()),
                   2.f + titleSize.y() + contentSize.y());
    titleHeight_ = titleSize.y();
  }

 protected:
  /** Override to reposition the text based on the X, Y, and Height values
   * configured. */
  virtual void setPositionImpl_() {
    // Adjust the position of the text manually
    title_->setPosition(osg::Vec3f(x(), y() + height(), 0.f));
    content_->setPosition(
        osg::Vec3f(x(), y() + height() - titleHeight_ - 2.f, 0.f));
  }

 private:
  osg::ref_ptr<osgText::Text> title_;
  osg::ref_ptr<osgText::Text> content_;
  float titleHeight_;
};
