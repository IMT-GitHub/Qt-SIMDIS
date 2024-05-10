#include "tdpsrecvconfig.h"

#include "ui_tdpsrecvconfig.h"

TDPSRecvConfig::TDPSRecvConfig(QWidget *parent, QString strTitle)
    : QDialogSkin(parent, strTitle),
      ui(new Ui::TDPSRecvConfig),
      m_pTableTDPS(nullptr) {
  ui->setupUi(this);

  m_nCol[0] = 3;
  m_nCol[1] = 6;
  m_nCol[2] = 7;
  m_nCol[3] = 9;

  for (int i = 0; i < DelegateCombo::COMBO_TDPS_RECV_CNT; i++) {
    m_pComboBoxDelegate[i] = nullptr;
  }
  SetInitTable();
}

TDPSRecvConfig::~TDPSRecvConfig() {
  delete ui;
  if (m_pTableTDPS != nullptr) {
    delete m_pTableTDPS;
    m_pTableTDPS = nullptr;
  }

  for (int i = 0; i < DelegateCombo::COMBO_TDPS_RECV_CNT; i++) {
    if (m_pComboBoxDelegate[i] != nullptr) {
      delete m_pComboBoxDelegate[i];
      m_pComboBoxDelegate[i] = nullptr;
    }
  }
}

/**
 * @brief TDPSRecvConfig::SetInitTable
 * TDPS 테이블 초기화
 */
void TDPSRecvConfig::SetInitTable() {
  QStringList strListHeader = {
      tr("Site"),       tr("IP"),          tr("Port"),      tr("DataMode"),
      tr("CallSign"),   tr("TrackLength"), tr("TrackType"), tr("TrackColor"),
      tr("TrackWidth"), tr("Dynamic"),
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

  int nType[DelegateCombo::COMBO_TDPS_RECV_CNT] = {
      DelegateCombo::COMBO_DATAMODE, DelegateCombo::COMBO_TRACKTYPE,
      DelegateCombo::COMBO_TRACKCOLOR, DelegateCombo::COMBO_BOOL};
  for (int i = 0; i < DelegateCombo::COMBO_TDPS_RECV_CNT; i++) {
    m_pComboBoxDelegate[i] = new ComboBoxDelegate(this, nType[i]);
    ui->QTable_TDPS->setItemDelegateForColumn(m_nCol[i],
                                              m_pComboBoxDelegate[i]);
  }

  on_BTN_Save_clicked();
}

/**
 * @brief TDPSRecvConfig::InsertConfig
 * * @param stTDPSConfig
 * TDPS 리스트 추가
 */
void TDPSRecvConfig::InsertConfig(TDPSConfig stTDPSConfig) {
  if (m_pTableTDPS != nullptr) {
    QList<QStandardItem *> items;

    // site
    items.append(new QStandardItem(stTDPSConfig.strSiteName));
    // ip
    items.append(new QStandardItem(stTDPSConfig.strIP));
    // port
    items.append(new QStandardItem(QString("%1").arg(stTDPSConfig.nPort)));
    // datamode
    items.append(new QStandardItem(stTDPSConfig.strDataMode));
    // callsign
    items.append(new QStandardItem(stTDPSConfig.strCallSign));
    // tracklength
    items.append(
        new QStandardItem(QString("%1").arg(stTDPSConfig.nTrackLength)));
    // tracktype
    items.append(new QStandardItem(stTDPSConfig.strTrackType));
    // trackcolor
    items.append(new QStandardItem(stTDPSConfig.strTrackColor));
    // trackwidth
    items.append(
        new QStandardItem(QString("%1").arg(stTDPSConfig.nTrackWidth)));
    // dynamic
    QString strText = "FALSE";
    if (stTDPSConfig.bDynamic) strText = "TRUE";
    items.append(new QStandardItem(strText));

    for (int j = 0; j < items.count(); j++) {
      items.at(j)->setTextAlignment(Qt::AlignCenter);
    }

    m_pTableTDPS->appendRow(items);

    for (int i = 0; i < DelegateCombo::COMBO_TDPS_RECV_CNT; i++) {
      ui->QTable_TDPS->openPersistentEditor(
          m_pTableTDPS->index(m_pTableTDPS->rowCount() - 1, m_nCol[i]));
    }
  }
}

/**
 * @brief TDPSRecvConfig::on_BTN_Modify_clicked
 * 수정 버튼 클릭
 */
void TDPSRecvConfig::on_BTN_Modify_clicked() {
  ui->BTN_Modify->setEnabled(false);
  ui->BTN_Insert->setEnabled(true);
  ui->BTN_Save->setEnabled(true);
  ui->BTN_Delete->setEnabled(true);

  ui->QTable_TDPS->setSelectionBehavior(QAbstractItemView::SelectItems);
  ui->QTable_TDPS->setEditTriggers(QAbstractItemView::DoubleClicked);

  if (m_pTableTDPS != nullptr) {
    for (int i = 0; i < m_pTableTDPS->rowCount(); i++) {
      for (int j = 0; j < DelegateCombo::COMBO_TDPS_RECV_CNT; j++) {
        ui->QTable_TDPS->openPersistentEditor(
            m_pTableTDPS->index(i, m_nCol[j]));
      }
    }
  }
}

/**
 * @brief TDPSRecvConfig::on_BTN_Save_clicked
 * 저장 버튼 클릭
 */
void TDPSRecvConfig::on_BTN_Save_clicked() {
  ui->BTN_Modify->setEnabled(true);
  ui->BTN_Insert->setEnabled(false);
  ui->BTN_Save->setEnabled(false);
  ui->BTN_Delete->setEnabled(false);

  ui->QTable_TDPS->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->QTable_TDPS->setEditTriggers(QAbstractItemView::NoEditTriggers);

  if (m_pTableTDPS != nullptr) {
    for (int i = 0; i < m_pTableTDPS->rowCount(); i++) {
      for (int j = 0; j < DelegateCombo::COMBO_TDPS_RECV_CNT; j++) {
        ui->QTable_TDPS->closePersistentEditor(
            m_pTableTDPS->index(i, m_nCol[j]));
      }
    }
  }
}

/**
 * @brief TDPSRecvConfig::on_BTN_Insert_clicked
 * 리스트 추가 버튼 클릭
 */
void TDPSRecvConfig::on_BTN_Insert_clicked() {
  TDPSConfig stTDPSConfig;  // 선언시 초기화됨.
  InsertConfig(stTDPSConfig);
}

/**
 * @brief TDPSRecvConfig::on_BTN_Delete_clicked
 * 리스트 삭제 버튼 클릭
 */
void TDPSRecvConfig::on_BTN_Delete_clicked() {
  QModelIndexList selectedIndexes =
      ui->QTable_TDPS->selectionModel()->selectedRows();

  for (const QModelIndex &index : selectedIndexes) {
    if (m_pTableTDPS != nullptr) {
      m_pTableTDPS->removeRow(index.row());
    }
  }
}

/**
 * @brief TDPSRecvConfig::GetTDPSConfig
 * @return QVector<TDPSConfig>
 * 시나리오 저장
 */
QVector<TDPSConfig> TDPSRecvConfig::GetTDPSConfig() {
  QVector<TDPSConfig> vTDPSConfig;
  vTDPSConfig.clear();

  TDPSConfig stTDPSConfig;
  if (m_pTableTDPS != nullptr) {
    for (int row = 0; row < m_pTableTDPS->rowCount(); row++) {
      for (int col = 0; col < m_pTableTDPS->columnCount(); col++) {
        switch (col) {
          case 0:
            stTDPSConfig.strSiteName =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 1:
            stTDPSConfig.strIP =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 2:
            stTDPSConfig.nPort =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toInt();
            break;
          case 3:
            stTDPSConfig.strDataMode =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 4:
            stTDPSConfig.strCallSign =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 5:
            stTDPSConfig.nTrackLength =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toInt();
            break;
          case 6:
            stTDPSConfig.strTrackType =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 7:
            stTDPSConfig.strTrackColor =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toString();
            break;
          case 8:
            stTDPSConfig.nTrackWidth =
                m_pTableTDPS->data(m_pTableTDPS->index(row, col)).toInt();
            break;
          case 9: {
            if (m_pTableTDPS->data(m_pTableTDPS->index(row, col))
                    .toString()
                    .compare("TRUE") == 0)
              stTDPSConfig.bDynamic = true;
            else
              stTDPSConfig.bDynamic = false;
          } break;
          default:
            break;
        }
      }
      vTDPSConfig.append(stTDPSConfig);
    }
  }
  return vTDPSConfig;
}

int TDPSRecvConfig::GetTDPSDispLimit() {
  return ui->EDIT_DispLimit->text().toInt();
}

void TDPSRecvConfig::SetScenarioLoad(ScenarioInfo stScenarioInfo) {
  if (m_pTableTDPS != nullptr) {
    SetInitTable();
    for (auto itt : stScenarioInfo.vTDPSConfig) {
      InsertConfig(itt);
    }
    on_BTN_Save_clicked();
  }
}

void TDPSRecvConfig::SetScenarioInit() { SetInitTable(); }
