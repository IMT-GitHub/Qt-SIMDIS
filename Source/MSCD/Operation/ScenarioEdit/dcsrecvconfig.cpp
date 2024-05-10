#include "dcsrecvconfig.h"

#include "ui_dcsrecvconfig.h"

DCSRecvConfig::DCSRecvConfig(QWidget *parent, QString strTitle)
    : QDialogSkin(parent, strTitle),
      ui(new Ui::DCSRecvConfig),
      m_pTableTDPS(nullptr) {
  ui->setupUi(this);
  m_nDelegateCol[0] = 0;
  m_nDelegateCol[1] = 5;
  m_nDelegateCol[2] = 6;
  m_nDelegateCol[3] = 9;
  m_nDelegateCol[4] = 12;

  for (int i = 0; i < DelegateCombo::COMBO_DCS_RECV_CNT; i++) {
    m_pComboBoxDelegate[i] = nullptr;
  }
  SetInitTable();
}

DCSRecvConfig::~DCSRecvConfig() {
  delete ui;
  for (int i = 0; i < DelegateCombo::COMBO_DCS_RECV_CNT; i++) {
    if (m_pComboBoxDelegate[i] != nullptr) {
      delete m_pComboBoxDelegate[i];
      m_pComboBoxDelegate[i] = nullptr;
    }
  }
}

void DCSRecvConfig::SetInitTable() {
  QStringList strListHeader = {
      tr("Site"),        tr("IP"),        tr("Port"),       tr("TCP IP"),
      tr("TCP Port"),    tr("DataMode"),  tr("ModelType"),  tr("ServerName"),
      tr("TrackLength"), tr("TrackType"), tr("TrackColor"), tr("TrackWidth"),
      tr("Dynamic"),
  };
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

  int nType[DelegateCombo::COMBO_DCS_RECV_CNT] = {
      DelegateCombo::COMBO_DCS_SITE, DelegateCombo::COMBO_DCS_DATAMODE,
      DelegateCombo::COMBO_DCS_MODEL, DelegateCombo::COMBO_TRACKTYPE,
      DelegateCombo::COMBO_BOOL};

  for (int i = 0; i < DelegateCombo::COMBO_DCS_RECV_CNT; i++) {
    m_pComboBoxDelegate[i] = new ComboBoxDelegate(this, nType[i]);
    ui->QTable_TDPS->setItemDelegateForColumn(m_nDelegateCol[i],
                                              m_pComboBoxDelegate[i]);
  }

  on_BTN_Save_clicked();
}

void DCSRecvConfig::on_BTN_Modify_clicked() {
  ui->BTN_Modify->setEnabled(false);
  ui->BTN_Insert->setEnabled(true);
  ui->BTN_Save->setEnabled(true);
  ui->BTN_Delete->setEnabled(true);

  ui->QTable_TDPS->setSelectionBehavior(QAbstractItemView::SelectItems);
  ui->QTable_TDPS->setEditTriggers(QAbstractItemView::DoubleClicked);

  if (m_pTableTDPS != nullptr) {
    for (int i = 0; i < m_pTableTDPS->rowCount(); i++) {
      for (int j = 0; j < DelegateCombo::COMBO_DCS_RECV_CNT; j++) {
        ui->QTable_TDPS->openPersistentEditor(
            m_pTableTDPS->index(i, m_nDelegateCol[j]));
      }
    }
  }
}

void DCSRecvConfig::on_BTN_Save_clicked() {
  ui->BTN_Modify->setEnabled(true);
  ui->BTN_Insert->setEnabled(false);
  ui->BTN_Save->setEnabled(false);
  ui->BTN_Delete->setEnabled(false);

  ui->QTable_TDPS->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->QTable_TDPS->setEditTriggers(QAbstractItemView::NoEditTriggers);

  if (m_pTableTDPS != nullptr) {
    for (int i = 0; i < m_pTableTDPS->rowCount(); i++) {
      for (int j = 0; j < DelegateCombo::COMBO_DCS_RECV_CNT; j++) {
        ui->QTable_TDPS->closePersistentEditor(
            m_pTableTDPS->index(i, m_nDelegateCol[j]));
      }
    }
  }
}

void DCSRecvConfig::on_BTN_Insert_clicked() {
  DCSConfig stDCSConfig;
  InsertConfig(stDCSConfig);
}

void DCSRecvConfig::on_BTN_Delete_clicked() {
  QModelIndexList selectedIndexes =
      ui->QTable_TDPS->selectionModel()->selectedRows();
  if (m_pTableTDPS != nullptr) {
    for (const QModelIndex &index : selectedIndexes) {
      m_pTableTDPS->removeRow(index.row());
    }
  }
}

void DCSRecvConfig::InsertConfig(DCSConfig stDCSConfig) {
  QList<QStandardItem *> items;
  items.append(new QStandardItem(stDCSConfig.strSiteName));
  items.append(new QStandardItem(stDCSConfig.strIP));
  items.append(new QStandardItem(QString("%1").arg(stDCSConfig.nPort)));
  items.append(new QStandardItem(stDCSConfig.strTCPIP));
  items.append(new QStandardItem(QString("%1").arg(stDCSConfig.nTCPPort)));
  items.append(new QStandardItem(stDCSConfig.strDataMode));
  items.append(new QStandardItem(stDCSConfig.strModelType));
  items.append(new QStandardItem(stDCSConfig.strServerName));
  items.append(new QStandardItem(QString("%1").arg(stDCSConfig.nTrackLength)));
  items.append(new QStandardItem(stDCSConfig.strTrackType));
  items.append(new QStandardItem(stDCSConfig.strTrackColor));
  items.append(new QStandardItem(QString("%1").arg(stDCSConfig.nTrackWidth)));

  QString strText = "FALSE";
  if (stDCSConfig.bDynamic) strText = "TRUE";
  items.append(new QStandardItem(strText));

  for (int j = 0; j < items.count(); j++) {
    items.at(j)->setTextAlignment(Qt::AlignCenter);
  }

  if (m_pTableTDPS != nullptr) {
    m_pTableTDPS->appendRow(items);

    for (int i = 0; i < DelegateCombo::COMBO_DCS_RECV_CNT; i++) {
      ui->QTable_TDPS->openPersistentEditor(
          m_pTableTDPS->index(m_pTableTDPS->rowCount() - 1, m_nDelegateCol[i]));
    }
  }
}

QVector<DCSConfig> DCSRecvConfig::GetDCSConfig() {
  QVector<DCSConfig> vDCSConfig;
  vDCSConfig.clear();

  DCSConfig stDCSConfig;
  if (m_pTableTDPS != nullptr) {
    for (int row = 0; row < m_pTableTDPS->rowCount(); row++) {
      for (int col = 0; col < m_pTableTDPS->columnCount(); col++) {
        switch (col) {
          case 0:
            stDCSConfig.strSiteName =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 1:
            stDCSConfig.strIP =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 2:
            stDCSConfig.nPort =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toInt();
            break;
          case 3:
            stDCSConfig.strTCPIP =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 4:
            stDCSConfig.nTCPPort =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toInt();
            break;
          case 5:
            stDCSConfig.strDataMode =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 6:
            stDCSConfig.strModelType =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 7:
            stDCSConfig.strServerName =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 8:
            stDCSConfig.nTrackLength =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toInt();
            break;
          case 9:
            stDCSConfig.strTrackType =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 10:
            stDCSConfig.strTrackColor =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 11:
            stDCSConfig.nTrackWidth =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toInt();
            break;
          case 12: {
            if (m_pTableTDPS->data(m_pTableTDPS->index(row, col))
                    .toString()
                    .compare("TRUE") == 0)
              stDCSConfig.bDynamic = true;
            else
              stDCSConfig.bDynamic = false;
          } break;
          default:
            break;
        }
      }
      vDCSConfig.append(stDCSConfig);
    }
  }
  return vDCSConfig;
}

int DCSRecvConfig::GetDCSDispLimit() {
  return ui->EDIT_DispLimit->text().toInt();
}

void DCSRecvConfig::SetScenarioLoad(ScenarioInfo stScenarioInfo) {
  if (m_pTableTDPS != nullptr) {
    SetInitTable();
    for (auto itt : stScenarioInfo.vDCSConfig) {
      InsertConfig(itt);
    }
    on_BTN_Save_clicked();
  }
}

void DCSRecvConfig::SetScenarioInit() { SetInitTable(); }
