#ifndef QDIALOGSKIN_H
#define QDIALOGSKIN_H

#include <QDebug>
#include <QDialog>
#include <QPushButton>
#include <QSizeGrip>
#include <QVBoxLayout>

class QDialogSkin : public QDialog {
  Q_OBJECT

  enum {
    BTN_SIZE = 0,
    BTN_EXIT,  // HIDE
    BTN_CNT,
  };

 public:
  explicit QDialogSkin(QWidget *parent = nullptr, QString strTitle = "");
  ~QDialogSkin();
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  // void mouseDoubleClickEvent(QMouseEvent *event) override;

 private:
  void CreateSizeGrip();
  void CreateBtn();

 private slots:
  void BtnSize_pressed();
  void BtnSize_released();
  void BtnExit_pressed();
  void BtnExit_released();

 private:
  QSizeGrip *m_pSizeGrip;
  QPushButton *m_pBtn[BTN_CNT];
  QString m_strTitle;
  QPoint m_MousePos;
  bool m_bIsTitle;
  bool m_bIsBig;
  QRect m_beforRect;
  // QPoint m_beforPos;
  // QSize m_beforSize;
};

#endif  // QDIALOGSKIN_H
