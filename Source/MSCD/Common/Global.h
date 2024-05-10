#ifndef GLOBAL_H
#define GLOBAL_H

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QItemDelegate>
#include <QLineEdit>
#include <QStandardItem>

enum DelegateCombo {
  COMBO_DATAMODE = 0,
  COMBO_TRACKTYPE,
  COMBO_TRACKCOLOR,
  COMBO_TAGNAME,
  COMBO_BOOL,
  COMBO_DCS_SITE,
  COMBO_DCS_MODEL,
  COMBO_DCS_DATAMODE,
  COMBO_CNT,

  COMBO_TDPS_RECV_CNT = 4,
  COMBO_TDPS_EU_CNT = 2,
  COMBO_DCS_RECV_CNT = 5,
};
class ComboBoxDelegate : public QItemDelegate {
 private:
  int m_nType;

 public:
  ComboBoxDelegate(QObject *parent = nullptr, int nType = 0);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;

  void setEditorData(QWidget *editor, const QModelIndex &index) const;

  void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const;

  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const;
};

/**
 * @brief The CheckBoxDelegate class
 * tableview 속 checkbox : enable 적용이 안됨.
 */
class CheckBoxDelegate : public QItemDelegate {
  Q_OBJECT
 public:
  CheckBoxDelegate(QObject *parent = nullptr);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const;

  bool editorEvent(QEvent *event, QAbstractItemModel *model,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index);

 signals:
  // void signal_Clecked(bool bChecked);
  void signal_Clecked(QModelIndex index);
};

double SetTime();

#endif  // GLOBAL_H
