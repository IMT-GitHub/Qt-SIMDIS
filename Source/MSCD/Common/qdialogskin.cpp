#include "qdialogskin.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QIcon>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QSettings>
#include <QTextCodec>
#include <QTextEdit>

#define TITLE_HEIGHT 20
QDialogSkin::QDialogSkin(QWidget *parent, QString strTitle)
    : QDialog(parent),
      m_bIsTitle(false),
      m_bIsBig(false),
      m_strTitle(strTitle) {
  setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
  setStyleSheet(styleSheet());
  setFocus();

  // sizegrip 초기화
  m_pSizeGrip = nullptr;

  // btn 초기화
  for (int i = 0; i < BTN_CNT; i++) {
    m_pBtn[i] = nullptr;
  }

  CreateSizeGrip();
  CreateBtn();
}

QDialogSkin::~QDialogSkin() {
  if (m_pSizeGrip != nullptr) {
    delete m_pSizeGrip;
  }

  for (int i = 0; i < BTN_CNT; i++) {
    if (m_pBtn[i] != nullptr) {
      delete m_pBtn[i];
    }
  }
}

void QDialogSkin::CreateSizeGrip() {
  if (m_pSizeGrip == nullptr) {
    m_pSizeGrip = new QSizeGrip(this);
    m_pSizeGrip->move(width() - 18, height() - 20);
    m_pSizeGrip->setFixedSize(20, 20);
  }
}

void QDialogSkin::CreateBtn() {
  QPoint btnPos[BTN_CNT] = {QPoint(width() - 45, 1), QPoint(width() - 23, 1)};
  QIcon btnIcon[BTN_CNT] = {QIcon(":/image/res/BTNSIZE_N.png"),
                            QIcon(":/image/res/BTNEXIT_N.png")};
  for (int i = 0; i < BTN_CNT; i++) {
    if (m_pBtn[i] == nullptr) {
      m_pBtn[i] = new QPushButton(btnIcon[i], nullptr, this);
      m_pBtn[i]->setFixedSize(20, 20);
      m_pBtn[i]->setStyleSheet(
          "background-color: rgba(0, 0, 0, 0%);"
          "border:transparent;");
      m_pBtn[i]->move(btnPos[i]);
    }
  }

  connect(m_pBtn[BTN_SIZE], SIGNAL(pressed()), SLOT(BtnSize_pressed()));
  connect(m_pBtn[BTN_SIZE], SIGNAL(released()), SLOT(BtnSize_released()));

  connect(m_pBtn[BTN_EXIT], SIGNAL(pressed()), SLOT(BtnExit_pressed()));
  connect(m_pBtn[BTN_EXIT], SIGNAL(released()), SLOT(BtnExit_released()));
}

void QDialogSkin::BtnSize_pressed() {
  if (m_bIsBig == false) {
    m_pBtn[BTN_SIZE]->setIcon(QIcon(":/image/res/BTNSIZE_P.png"));
  } else {
    m_pBtn[BTN_SIZE]->setIcon(QIcon(":/image/res/BTNSIZE_SP.png"));
  }
}

void QDialogSkin::BtnSize_released() {
  if (m_bIsBig == false) {
    // 다이얼로그 크기 최대 변경
    m_pBtn[BTN_SIZE]->setIcon(QIcon(":/image/res/BTNSIZE_SN.png"));
    m_bIsBig = true;

    m_beforRect = geometry();
    QDesktopWidget *desktop = QApplication::desktop();
    QRect screenGeometry = desktop->screenGeometry(desktop->screenNumber(this));
    setGeometry(screenGeometry);

  } else {
    // 다이얼로그 크기 이전 사이즈 변경
    m_pBtn[BTN_SIZE]->setIcon(QIcon(":/image/res/BTNSIZE_N.png"));
    m_bIsBig = false;

    setGeometry(m_beforRect);
  }
}

void QDialogSkin::BtnExit_pressed() {
  m_pBtn[BTN_EXIT]->setIcon(QIcon(":/image/res/BTNEXIT_P.png"));
}

void QDialogSkin::BtnExit_released() {
  m_pBtn[BTN_EXIT]->setIcon(QIcon(":/image/res/BTNEXIT_N.png"));
  hide();
}

void QDialogSkin::resizeEvent(QResizeEvent *event) {
  QPoint btnPos[BTN_CNT] = {QPoint(width() - 45, 1), QPoint(width() - 23, 1)};
  for (int i = 0; i < BTN_CNT; i++) {
    if (m_pBtn[i] != nullptr) {
      m_pBtn[i]->move(btnPos[i]);
    }
  }

  if (m_pSizeGrip != nullptr) {
    m_pSizeGrip->move(width() - 18, height() - 20);
  }
}

void QDialogSkin::mouseMoveEvent(QMouseEvent *event) {
  if (event->buttons() & Qt::LeftButton && m_bIsTitle) {
    // Dialog Move
    QPoint diff = event->globalPos() - m_MousePos;
    QWidget *parent = __super::window();
    QPoint pos = parent->pos();
    pos += diff;
    parent->move(pos);
    m_MousePos = event->globalPos();
  }
}

void QDialogSkin::mousePressEvent(QMouseEvent *event) {
  if (event->buttons() & Qt::LeftButton) {
    // Dialog Move
    auto pos = event->localPos();
    if (pos.y() < TITLE_HEIGHT) {
      m_bIsTitle = true;
      m_MousePos = event->globalPos();
    }
  }
}

void QDialogSkin::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  QPainter painter(this);

  // Draw a filled rectangle

  QRect rectangle(0, 0, width() - 1, TITLE_HEIGHT);
  QRect rectangle_back(0, 0, width() - 1, height() - 1);

  QColor titleColor;
  titleColor = QColor(0, 120, 215);
  /*
  if (!hasFocus()) {
    titleColor = QColor(240, 240, 240);
  }
  */
  painter.fillRect(rectangle_back,
                   QColor(240, 240, 240));  // Set the fill color
  painter.fillRect(rectangle, titleColor);  // Set the fill color

  // QPen pen = QPen(QColor(0, 120, 215), 1);
  QPen pen = QPen(Qt::black, 1);
  painter.setPen(pen);

  // Draw the outline of the rectangle
  painter.drawRect(rectangle_back);
  painter.drawRect(rectangle);

  // Draw Text
  QFont font;
  font.setBold(true);
  font.setPointSize(9);

  painter.setFont(font);
  painter.setPen(Qt::white);

  painter.drawText(QRectF(7, 1, width() - 10, TITLE_HEIGHT), Qt::AlignVCenter,
                   m_strTitle);
}

void QDialogSkin::mouseReleaseEvent(QMouseEvent *event) { m_bIsTitle = false; }
