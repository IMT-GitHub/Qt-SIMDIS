#include "Global.h"

#include "../STD/timec.h"
#include "define.h"
ComboBoxDelegate::ComboBoxDelegate(QObject *parent, int nType)
    : QItemDelegate(parent), m_nType(nType) {}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const {
  QComboBox *editor = new QComboBox(parent);

  /*
      editor->setStyleSheet(
          "QComboBox {"
          "   text-align: center;"
          "}");
  */
  switch (m_nType) {
    case COMBO_DCS_DATAMODE:
      editor->addItem("broadcast");
    case COMBO_DATAMODE:
      editor->addItem("multicast");
      editor->addItem("unicast");
      editor->setCurrentIndex(0);
      break;
    case COMBO_TRACKTYPE:
      editor->addItem(TRACK_TYPE_POINTS);
      editor->addItem(TRACK_TYPE_LINE);
      editor->addItem(TRACK_TYPE_RIBON);
      editor->addItem(TRACK_TYPE_BRIDGE);
      editor->setCurrentIndex(0);
      break;
    case COMBO_TRACKCOLOR:
      editor->addItem(TRACK_COLOR_CYAN);
      editor->addItem(TRACK_COLOR_GREEN);
      editor->addItem(TRACK_COLOR_MAGENTA);
      editor->addItem(TRACK_COLOR_YELLOW);
      editor->addItem(TRACK_COLOR_RED);
      editor->addItem(TRACK_COLOR_WHITE);
      editor->addItem(TRACK_COLOR_BLACK);
      editor->setCurrentIndex(0);
      break;
    case COMBO_TAGNAME:
      editor->addItem("posX");
      editor->addItem("posY");
      editor->addItem("posZ");
      editor->addItem("oriX");
      editor->addItem("oriY");
      editor->addItem("oriZ");
      editor->addItem("velX");
      editor->addItem("velY");
      editor->addItem("velZ");
      editor->addItem("time");
      editor->addItem("user");
      editor->setCurrentIndex(10);
      break;
    case COMBO_DCS_SITE:
      editor->addItem("ls");
      editor->addItem("as");
      editor->addItem("ns");      
      editor->setCurrentIndex(0);
      break;
    case COMBO_BOOL:
      editor->addItem("FALSE");
      editor->addItem("TRUE");
      editor->setCurrentIndex(0);
      break;
    case COMBO_DCS_MODEL:
      editor->addItem("model1");
      editor->addItem("model2");
      
      editor->setCurrentIndex(0);
      break;
    default:
      break;
  }

  editor->setEditable(true);
  editor->lineEdit()->setReadOnly(true);
  editor->lineEdit()->setAlignment(Qt::AlignCenter);

  return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const {
  QString value = index.model()->data(index, Qt::EditRole).toString();
  QComboBox *comboBox = static_cast<QComboBox *>(editor);
  comboBox->setCurrentText(value);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const {
  QComboBox *comboBox = static_cast<QComboBox *>(editor);
  QString value = comboBox->currentText();
  model->setData(index, value, Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const {
  editor->setGeometry(option.rect);
};

/**
 * @brief The CheckBoxDelegate class
 * tableview 속 checkbox : enable 적용이 안됨.
 */

CheckBoxDelegate::CheckBoxDelegate(QObject *parent) : QItemDelegate(parent) {}

void CheckBoxDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const {
  bool value = index.data().toBool();
  QStyleOptionButton cbo;

  cbo.rect = option.rect;
  cbo.rect.setLeft(option.rect.center().x() - 7);
  cbo.state = value ? QStyle::State_On : QStyle::State_Off;
  cbo.text = "";
  cbo.state = cbo.state | QStyle::State_Enabled;

  qApp->style()->drawControl(QStyle::CE_CheckBox, &cbo, painter);
}

bool CheckBoxDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) {
  Q_UNUSED(option);

  if (event->type() == QEvent::MouseButtonRelease) return false;
  model->setData(index, !index.data().toBool());
  // emit signal_Clecked(index.data().toBool());
  emit signal_Clecked(index);
  return event->type() == QEvent::MouseButtonDblClick ? true : false;
}

/**
 * @brief SetTime
 * 시간 정보 설정
 */
double SetTime() {
  struct timeval tp;

  // get the current system time, using timezone value of 0
  // returns UTC time
  gettimeofday(&tp, 0);

  // put system time into a tm struct
  time_t t(tp.tv_sec);
  struct tm *p_time = gmtime(&t);

  // assemble a UTC "system time"
  unsigned int p_secs = (unsigned int)(p_time->tm_sec) +
                        (((unsigned int)(p_time->tm_min)) * 60) +
                        (((unsigned int)(p_time->tm_hour)) * 60 * 60) +
                        (((unsigned int)(p_time->tm_yday)) * 60 * 60 * 24);

  return (p_secs + (tp.tv_usec * 1e-06));
}
