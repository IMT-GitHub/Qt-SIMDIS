#include "tdpseudatalist.h"

#include "ui_tdpseudatalist.h"

TDPSEuDataList::TDPSEuDataList(QWidget *parent, QString strTitle)
    : QDialogSkin(parent, strTitle),
      ui(new Ui::TDPSEuDataList),
      m_pTableTDPS(nullptr) {
  ui->setupUi(this);
  for (int i = 0; i < DelegateCombo::COMBO_TDPS_EU_CNT; i++) {
    m_pComboBoxDelegate[i] = nullptr;
  }
  SetInitTable();
}

TDPSEuDataList::~TDPSEuDataList() {
  delete ui;
  if (m_pTableTDPS != nullptr) {
    delete m_pTableTDPS;
    m_pTableTDPS = nullptr;
  }

  for (int i = 0; i < DelegateCombo::COMBO_TDPS_EU_CNT; i++) {
    if (m_pComboBoxDelegate[i] != nullptr) {
      delete m_pComboBoxDelegate[i];
      m_pComboBoxDelegate[i] = nullptr;
    }
  }
}

void TDPSEuDataList::SetInitTable() {
  QStringList strListHeader = {tr("TagName"), tr("TagIndex"),    tr("Unit"),
                               tr("HUD"),     tr("UserTagName"), tr("Text")};
  if (m_pTableTDPS == nullptr)
    m_pTableTDPS = new QStandardItemModel();
  else
    m_pTableTDPS->clear();

  m_pTableTDPS->setHorizontalHeaderLabels(strListHeader);

  ui->QTable_TDPS->setModel(m_pTableTDPS);
  ui->QTable_TDPS->verticalHeader()->setFixedWidth(30);
  ui->QTable_TDPS->horizontalHeader()->setSectionsClickable(false);
  ui->QTable_TDPS->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  m_pComboBoxDelegate[0] =
      new ComboBoxDelegate(this, DelegateCombo::COMBO_TAGNAME);
  ui->QTable_TDPS->setItemDelegateForColumn(0, m_pComboBoxDelegate[0]);

  m_pComboBoxDelegate[1] =
      new ComboBoxDelegate(this, DelegateCombo::COMBO_BOOL);
  ui->QTable_TDPS->setItemDelegateForColumn(3, m_pComboBoxDelegate[1]);

  // InitEuData();
  on_BTN_Save_clicked();
}

/**
 * @brief TDPSEuDataList::InitEuData
 * 초기 10개 default 설정
 * 사용 판단 확인 필요
 */
void TDPSEuDataList::InitEuData() {
  if (m_pTableTDPS != nullptr) {
    QString strTagName[10] = {"posX", "posY", "posZ", "oriX", "oriY",
                              "oriZ", "velX", "velY", "velZ", "time"};
    QString strUnit[10] = {"deg", "deg", "m",   "deg", "deg",
                           "deg", "m/s", "m/s", "m/s", "sec"};
    QString strText[10] = {"LAT",  "LON", "ALT", "Yaw", "Pitch",
                           "Roll", "vX",  "vY",  "vZ",  "F.Time"};

    for (int i = 0; i < 10; i++) {
      QList<QStandardItem *> items;
      items.append(new QStandardItem(strTagName[i]));
      items.append(new QStandardItem(QString("%1").arg(i + 1)));
      items.append(new QStandardItem(strUnit[i]));
      items.append(new QStandardItem("TRUE"));
      items.append(new QStandardItem());
      items.append(new QStandardItem(strText[i]));

      for (int j = 0; j < items.count(); j++) {
        items.at(j)->setTextAlignment(Qt::AlignCenter);
      }

      m_pTableTDPS->appendRow(items);

      ui->QTable_TDPS->openPersistentEditor(
          m_pTableTDPS->index(m_pTableTDPS->rowCount() - 1, 0));
      ui->QTable_TDPS->openPersistentEditor(
          m_pTableTDPS->index(m_pTableTDPS->rowCount() - 1, 3));

      // m_pTableTDPS->itemFromIndex()->setTextAlignment();
    }
  }
}

void TDPSEuDataList::InsertEuData(EuDataList stEuDataList) {
  if (m_pTableTDPS != nullptr) {
    QList<QStandardItem *> items;
    // tagName
    items.append(new QStandardItem(stEuDataList.strTagName));
    // tagIndex
    items.append(new QStandardItem(QString("%1").arg(stEuDataList.nTagIndex)));
    // Unit
    items.append(new QStandardItem(stEuDataList.strUnit));
    // HUD
    QString strText = "FALSE";
    if (stEuDataList.bHUD) strText = "TRUE";
    items.append(new QStandardItem(strText));
    // UserTagName
    items.append(new QStandardItem(stEuDataList.strUserTagName));
    // Text
    items.append(new QStandardItem(stEuDataList.strText));

    for (int j = 0; j < items.count(); j++) {
      items.at(j)->setTextAlignment(Qt::AlignCenter);
    }

    m_pTableTDPS->appendRow(items);

    ui->QTable_TDPS->openPersistentEditor(
        m_pTableTDPS->index(m_pTableTDPS->rowCount() - 1, 0));
    ui->QTable_TDPS->openPersistentEditor(
        m_pTableTDPS->index(m_pTableTDPS->rowCount() - 1, 3));
  }
}

void TDPSEuDataList::on_BTN_Modify_clicked() {
  ui->BTN_Modify->setEnabled(false);
  ui->BTN_Insert->setEnabled(true);
  ui->BTN_Save->setEnabled(true);
  ui->BTN_Delete->setEnabled(true);

  ui->QTable_TDPS->setSelectionBehavior(QAbstractItemView::SelectItems);
  ui->QTable_TDPS->setEditTriggers(QAbstractItemView::DoubleClicked);

  if (m_pTableTDPS != nullptr) {
    for (int i = 0; i < m_pTableTDPS->rowCount(); i++) {
      ui->QTable_TDPS->openPersistentEditor(m_pTableTDPS->index(i, 0));
      ui->QTable_TDPS->openPersistentEditor(m_pTableTDPS->index(i, 3));
    }
  }
}

void TDPSEuDataList::on_BTN_Save_clicked() {
  ui->BTN_Modify->setEnabled(true);
  ui->BTN_Insert->setEnabled(false);
  ui->BTN_Save->setEnabled(false);
  ui->BTN_Delete->setEnabled(false);

  ui->QTable_TDPS->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->QTable_TDPS->setEditTriggers(QAbstractItemView::NoEditTriggers);

  if (m_pTableTDPS != nullptr) {
    for (int i = 0; i < m_pTableTDPS->rowCount(); i++) {
      ui->QTable_TDPS->closePersistentEditor(m_pTableTDPS->index(i, 0));
      ui->QTable_TDPS->closePersistentEditor(m_pTableTDPS->index(i, 3));
    }
  }
}

void TDPSEuDataList::on_BTN_Insert_clicked() {
  EuDataList stEuDataList;
  InsertEuData(stEuDataList);
}

void TDPSEuDataList::on_BTN_Delete_clicked() {
  QModelIndexList selectedIndexes =
      ui->QTable_TDPS->selectionModel()->selectedRows();

  if (m_pTableTDPS != nullptr) {
    for (const QModelIndex &index : selectedIndexes) {
      m_pTableTDPS->removeRow(index.row());
    }
  }
}

QVector<EuDataList> TDPSEuDataList::GetEuDataScenario() {
  QVector<EuDataList> vEuDataScenario;
  vEuDataScenario.clear();

  EuDataList stEuDataList;
  if (m_pTableTDPS != nullptr) {
    for (int row = 0; row < m_pTableTDPS->rowCount(); row++) {
      for (int col = 0; col < m_pTableTDPS->columnCount(); col++) {
        switch (col) {
          case 0:
            stEuDataList.strTagName =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 1:
            stEuDataList.nTagIndex =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toInt();
            break;
          case 2:
            stEuDataList.strUnit =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 3: {
            if (m_pTableTDPS->data(m_pTableTDPS->index(row, col))
                    .toString()
                    .compare("TRUE") == 0)
              stEuDataList.bHUD = true;
            else
              stEuDataList.bHUD = false;
          } break;
          case 4:
            stEuDataList.strUserTagName =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 5:
            stEuDataList.strText =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          default:
            break;
        }
      }
      vEuDataScenario.append(stEuDataList);
    }
  }
  return vEuDataScenario;
}

void TDPSEuDataList::SetScenarioLoad(ScenarioInfo stScenarioInfo) {
  if (m_pTableTDPS != nullptr) {
    SetInitTable();
    for (auto itt : stScenarioInfo.vEuDataList) {
      InsertEuData(itt);
    }
    on_BTN_Save_clicked();
  }
}

void TDPSEuDataList::SetScenarioInit() { SetInitTable(); }
