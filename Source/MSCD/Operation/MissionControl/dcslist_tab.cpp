#include "dcslist_tab.h"

#include "dcslist.h"
#include "ui_dcslist_tab.h"

DCSList_tab::DCSList_tab(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::DCSList_tab),
      m_pParent(reinterpret_cast<DCSList *>(parent)),
      m_pTableInfo(nullptr),
      m_pTableStatus(nullptr),
      m_pTableLog(nullptr),
      m_pBtnRefresh(nullptr),
      m_nSelectInfoID(0) {
  ui->setupUi(this);

  m_mapInfoList.clear();
  m_mapStatusInfo.clear();

  for (int i = 0; i < CHECK_CNT; i++) {
    m_pCheckBoxDelegate[i] = nullptr;
    m_nDelegateCol[i] = 3 + i;
  }

  InitInfoList();
  InitStatusList();
  InitLogList();
  CreateBtn();
}

DCSList_tab::~DCSList_tab() {
  delete ui;

  m_mapInfoList.clear();
  m_mapStatusInfo.clear();

  if (m_pTableInfo != nullptr) {
    delete m_pTableInfo;
    m_pTableInfo = nullptr;
  }

  if (m_pTableStatus != nullptr) {
    delete m_pTableStatus;
    m_pTableStatus = nullptr;
  }

  if (m_pTableLog != nullptr) {
    delete m_pTableLog;
    m_pTableLog = nullptr;
  }

  if (m_pBtnRefresh != nullptr) {
    disconnect(m_pBtnRefresh, SIGNAL(clicked()), this,
               SLOT(slot_RefreshBTN_Clicked()));

    delete m_pBtnRefresh;
    m_pBtnRefresh = nullptr;
  }

  disconnect(m_pCheckBoxDelegate[0], SIGNAL(signal_Clecked(QModelIndex)), this,
             SLOT(slot_DrawChecked(QModelIndex)));
  disconnect(m_pCheckBoxDelegate[1], SIGNAL(signal_Clecked(QModelIndex)), this,
             SLOT(slot_TrackChecked(QModelIndex)));
  disconnect(m_pCheckBoxDelegate[2], SIGNAL(signal_Clecked(QModelIndex)), this,
             SLOT(slot_LabelChecked(QModelIndex)));

  for (int i = 0; i < CHECK_CNT; i++) {
    if (m_pCheckBoxDelegate[i] != nullptr) {
      delete m_pCheckBoxDelegate[i];
      m_pCheckBoxDelegate[i] = nullptr;
    }
  }
}

void DCSList_tab::CreateBtn() {
  if (m_pBtnRefresh == nullptr) {
    m_pBtnRefresh = new QPushButton("Button", this);
    m_pBtnRefresh->setGeometry(QRect(125, 156, 22, 22));
    m_pBtnRefresh->setText("");
    // m_pBtnRefresh->setStyleSheet("background-color: rgba(100, 100,
    // 100,128);");
    m_pBtnRefresh->setIcon(QIcon(":/image/res/BTNREFRESH.png"));
    m_pBtnRefresh->setIconSize(QSize(16, 16));
    connect(m_pBtnRefresh, SIGNAL(clicked()), this,
            SLOT(slot_RefreshBTN_Clicked()));
  }
}

void DCSList_tab::slot_RefreshBTN_Clicked() {
  if (m_pParent != nullptr) m_pParent->SetReqestLogData(m_nSelectInfoID);
}

void DCSList_tab::InitInfoList() {
  m_stDefultPref;  // 초기화
  QStringList strListHeader = {
      tr("Type"),  tr("Internal ID"), tr("Callsign"), tr("Draw"),
      tr("Track"), tr("Label"),       tr("Count"),
  };
  if (m_pTableInfo == nullptr)
    m_pTableInfo = new QStandardItemModel();
  else
    m_pTableInfo->clear();

  m_pTableInfo->setHorizontalHeaderLabels(strListHeader);
  ui->QTable_Info->setModel(m_pTableInfo);
  ui->QTable_Info->verticalHeader()->setFixedWidth(30);
  ui->QTable_Info->horizontalHeader()->setSectionsClickable(false);

  ui->QTable_Info->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  ui->QTable_Info->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->QTable_Info->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->QTable_Info->setEditTriggers(QAbstractItemView::NoEditTriggers);

  for (int i = 0; i < CHECK_CNT; i++) {
    if (m_pCheckBoxDelegate[i] == nullptr) {
      m_pCheckBoxDelegate[i] = new CheckBoxDelegate(this);
      ui->QTable_Info->setItemDelegateForColumn(m_nDelegateCol[i],
                                                m_pCheckBoxDelegate[i]);
    }
  }
  connect(m_pCheckBoxDelegate[0], SIGNAL(signal_Clecked(QModelIndex)), this,
          SLOT(slot_DrawChecked(QModelIndex)));
  connect(m_pCheckBoxDelegate[1], SIGNAL(signal_Clecked(QModelIndex)), this,
          SLOT(slot_TrackChecked(QModelIndex)));
  connect(m_pCheckBoxDelegate[2], SIGNAL(signal_Clecked(QModelIndex)), this,
          SLOT(slot_LabelChecked(QModelIndex)));
}

void DCSList_tab::InitStatusList() {
  // ui->QList_Info
  QStringList strListHeader = {
      tr("TimeLocal"),   tr("Lat(deg)"),      tr("Lon(deg)"),
      tr("Alt(m)"),      tr("Yaw(deg)"),      tr("Pitch(deg)"),
      tr("Roll(deg)"),   tr("Vx(E) (m/sec)"), tr("Vy(N) (deg)"),
      tr("Vz(U) (deg)"), tr("Speed (m/sec)"),
  };

  if (m_pTableStatus != nullptr) {
    delete m_pTableStatus;
    m_pTableStatus = nullptr;
  }

  m_pTableStatus = new QStandardItemModel();

  m_pTableStatus->setHorizontalHeaderLabels(strListHeader);
  ui->QTable_Status->setModel(m_pTableStatus);
  ui->QTable_Status->verticalHeader()->setFixedWidth(30);
  ui->QTable_Status->horizontalHeader()->setSectionsClickable(false);

  ui->QTable_Status->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  ui->QTable_Status->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->QTable_Status->setSelectionMode(QAbstractItemView::NoSelection);
  ui->QTable_Status->setEditTriggers(QAbstractItemView::NoEditTriggers);

  // 초기 row 생성
  QStringList strList;
  for (int i = 0; i < strListHeader.size(); i++) {
    strList.append(QString(""));
  }

  QList<QStandardItem *> items;
  int nCnt = 0;
  for (auto itt : strList) {
    items.append(new QStandardItem(itt));
    items.at(nCnt)->setTextAlignment(Qt::AlignCenter);
    nCnt++;
  }

  m_pTableStatus->appendRow(items);
}

void DCSList_tab::InitLogList() {
  QStringList strListHeader = {
      tr("TimeLocal"),   tr("Lat(deg)"),      tr("Lon(deg)"),
      tr("Alt(m)"),      tr("Yaw(deg)"),      tr("Pitch(deg)"),
      tr("Roll(deg)"),   tr("Vx(E) (m/sec)"), tr("Vy(N) (deg)"),
      tr("Vz(U) (deg)"), tr("Speed (m/sec)"),
  };

  if (m_pTableLog != nullptr) {
    delete m_pTableLog;
    m_pTableLog = nullptr;
  }
  m_pTableLog = new QStandardItemModel();

  m_pTableLog->setHorizontalHeaderLabels(strListHeader);
  ui->QTable_Log->setModel(m_pTableLog);
  ui->QTable_Log->verticalHeader()->setFixedWidth(30);
  ui->QTable_Log->horizontalHeader()->setSectionsClickable(false);

  ui->QTable_Log->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  ui->QTable_Log->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->QTable_Log->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->QTable_Log->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void DCSList_tab::SetInfoList(TestListInfo stTestListInfo) {
  QList<QStandardItem *> items;
  // Type
  items.append(new QStandardItem(stTestListInfo.strType));
  // internalID
  items.append(
      new QStandardItem(QString("%1").arg(stTestListInfo.nInternalID)));
  // CallSign
  items.append(new QStandardItem(stTestListInfo.strCallsign));
  // Draw
  items.append(new QStandardItem(QString("%1").arg(stTestListInfo.bDraw)));
  // Track
  items.append(new QStandardItem(QString("%1").arg(stTestListInfo.bTrack)));
  // Label
  items.append(new QStandardItem(QString("%1").arg(stTestListInfo.bLabel)));
  // Count
  items.append(new QStandardItem("0"));

  for (int j = 0; j < items.count(); j++) {
    items.at(j)->setTextAlignment(Qt::AlignCenter);
  }

  if (m_pTableInfo != nullptr) {
    m_pTableInfo->appendRow(items);
  }
}

void DCSList_tab::SetStatusList(UpdatePlatform stUpdatePlatform) {
  if (isEnabled() == false) setEnabled(true);  // 데이터 수신시 enable
  // 최초 수신에 따른 InfoList 설정
  auto ittfind = m_mapInfoList.find(stUpdatePlatform.nID);
  if (ittfind == m_mapInfoList.end()) {
    /*
    if(m_nSelectInfoID == 0)
        m_nSelectInfoID = stUpdatePlatform.nID;
    */
    TestListInfo stTestListInfo;
    stTestListInfo.nInternalID = stUpdatePlatform.nID;
    stTestListInfo.strCallsign = stUpdatePlatform.stAddPlatform.strCallSign;
    stTestListInfo.strType = "AIR";  //
    SetInfoList(stTestListInfo);

    PlatfromPref stPlatfromPref = m_stDefultPref;
    m_mapInfoList.insert(stUpdatePlatform.nID, stPlatfromPref);
  }

  // Status 정보 바로전시 및 마지막값 전시를 위한 자료구조
  m_mapStatusInfo.insert(stUpdatePlatform.nID, stUpdatePlatform);

  if (m_nSelectInfoID == stUpdatePlatform.nID) {
    QStringList strList;
    auto posInfo = stUpdatePlatform.gf_GetLLAPos(false);
    strList.append(QString::number(stUpdatePlatform.time, 'f', 3));
    strList.append(QString::number(posInfo.lat() * UTILS::CU_RAD2DEG, 'f', 3));
    strList.append(QString::number(posInfo.lon() * UTILS::CU_RAD2DEG, 'f', 3));
    strList.append(QString::number(posInfo.alt(), 'f', 3));
    strList.append(QString::number(posInfo.yaw(), 'f', 3));
    strList.append(QString::number(posInfo.pitch(), 'f', 3));
    strList.append(QString::number(posInfo.roll(), 'f', 3));
    strList.append(QString::number(posInfo.vx(), 'f', 3));
    strList.append(QString::number(posInfo.vy(), 'f', 3));
    strList.append(QString::number(posInfo.vz(), 'f', 3));
    // strList.append(QString::number(stUpdatePlatform.nID));

    // Update Value
    int nCol = 0;
    for (auto itt : strList) {
      m_pTableStatus->setData(m_pTableStatus->index(0, nCol), itt);
      nCol++;
    }
  }

  for (int nRow = 0; nRow < m_pTableInfo->rowCount(); nRow++) {
    if (m_pTableInfo->data(m_pTableInfo->index(nRow, 1)).toInt() ==
        stUpdatePlatform.nID) {
      int nMsgCount = m_pTableInfo->data(m_pTableInfo->index(nRow, 6)).toInt();
      nMsgCount++;
      m_pTableInfo->setData(m_pTableInfo->index(nRow, 6), nMsgCount);
    }
  }
}

void DCSList_tab::SetPrefInfo(DCSConfig stDCSConfig) {
  // QIntValidator lineediter내부서 자동삭제?
  QIntValidator *intValidator = new QIntValidator(0, 9999);  // 최소수,최대수
  ui->EDIT_Length->setValidator(intValidator);
  ui->EDIT_Width->setValidator(intValidator);

  ui->COMBO_Type->addItem(TRACK_TYPE_POINTS);
  ui->COMBO_Type->addItem(TRACK_TYPE_LINE);
  ui->COMBO_Type->addItem(TRACK_TYPE_RIBON);
  ui->COMBO_Type->addItem(TRACK_TYPE_BRIDGE);

  ui->COMBO_Color->addItem(TRACK_COLOR_CYAN);
  ui->COMBO_Color->addItem(TRACK_COLOR_GREEN);
  ui->COMBO_Color->addItem(TRACK_COLOR_MAGENTA);
  ui->COMBO_Color->addItem(TRACK_COLOR_YELLOW);
  ui->COMBO_Color->addItem(TRACK_COLOR_RED);
  ui->COMBO_Color->addItem(TRACK_COLOR_WHITE);
  ui->COMBO_Color->addItem(TRACK_COLOR_BLACK);
  // ui->COMBO_Color->setCurrentIndex(0);

  ui->EDIT_Length->setText(QString("%1").arg(stDCSConfig.nTrackLength));
  ui->EDIT_Width->setText(QString("%1").arg(stDCSConfig.nTrackWidth));

  ui->COMBO_Color->setCurrentText(stDCSConfig.strTrackColor);
  ui->COMBO_Type->setCurrentText(stDCSConfig.strTrackType);
  ui->CHECK_Scale->setChecked(stDCSConfig.bDynamic);

  m_stDefultPref.nTrackLength = stDCSConfig.nTrackLength;
  m_stDefultPref.nTrackWidth = stDCSConfig.nTrackWidth;

  m_stDefultPref.strTrackColor = stDCSConfig.strTrackColor;
  m_stDefultPref.strTrackType = stDCSConfig.strTrackType;

  m_stDefultPref.bDynamic = stDCSConfig.bDynamic;
}

void DCSList_tab::SetSelectPrefInfo() {
  auto ittfind = m_mapInfoList.find(m_nSelectInfoID);
  if (ittfind != m_mapInfoList.end()) {
    ui->EDIT_Length->setText(QString("%1").arg(ittfind.value().nTrackLength));
    ui->EDIT_Width->setText(QString("%1").arg(ittfind.value().nTrackWidth));

    ui->COMBO_Color->setCurrentText(ittfind.value().strTrackColor);
    ui->COMBO_Type->setCurrentText(ittfind.value().strTrackType);
    ui->CHECK_Scale->setChecked(ittfind.value().bDynamic);
  }
}

void DCSList_tab::SetSelectStatus() {
  auto ittfind = m_mapStatusInfo.find(m_nSelectInfoID);
  if (ittfind != m_mapStatusInfo.end()) {
    QStringList strList;
    auto posInfo = ittfind->gf_GetLLAPos(false);
    strList.append(QString::number(ittfind->time, 'f', 3));
    strList.append(QString::number(posInfo.lat() * UTILS::CU_RAD2DEG, 'f', 3));
    strList.append(QString::number(posInfo.lon() * UTILS::CU_RAD2DEG, 'f', 3));
    strList.append(QString::number(posInfo.alt(), 'f', 3));
    strList.append(QString::number(posInfo.yaw(), 'f', 3));
    strList.append(QString::number(posInfo.pitch(), 'f', 3));
    strList.append(QString::number(posInfo.roll(), 'f', 3));
    strList.append(QString::number(posInfo.vx(), 'f', 3));
    strList.append(QString::number(posInfo.vy(), 'f', 3));
    strList.append(QString::number(posInfo.vz(), 'f', 3));
    // strList.append(QString::number(stUpdatePlatform.nID));

    // Update Value
    int nCol = 0;
    for (auto itt : strList) {
      m_pTableStatus->setData(m_pTableStatus->index(0, nCol), itt);
      nCol++;
    }
  }
}

void DCSList_tab::on_QTable_Info_clicked(const QModelIndex &index) {
  if (m_pTableInfo != nullptr)
    m_nSelectInfoID = m_pTableInfo->index(index.row(), 1).data().toInt();
  if (m_pParent != nullptr) m_pParent->SetReqestLogData(m_nSelectInfoID);
  SetSelectPrefInfo();
  SetSelectStatus();
}

void DCSList_tab::SetUpdatePref() {
  if (isEnabled() == true) {
    auto ittfind = m_mapInfoList.find(m_nSelectInfoID);
    if (ittfind != m_mapInfoList.end()) {
      PlatfromPref stPlatfromPref;

      stPlatfromPref.nTrackLength = ui->EDIT_Length->text().toInt();
      stPlatfromPref.nTrackWidth = ui->EDIT_Width->text().toInt();

      stPlatfromPref.strTrackColor = ui->COMBO_Color->currentText();
      stPlatfromPref.strTrackType = ui->COMBO_Type->currentText();
      stPlatfromPref.bDynamic = ui->CHECK_Scale->isChecked();

      if (m_pTableInfo != nullptr) {
        for (int nRow = 0; nRow < m_pTableInfo->rowCount(); nRow++) {
          if (m_nSelectInfoID == m_pTableInfo->index(nRow, 1).data().toInt()) {
            stPlatfromPref.bDraw = m_pTableInfo->index(nRow, 3).data().toBool();
            stPlatfromPref.bTrack =
                m_pTableInfo->index(nRow, 4).data().toBool();
            stPlatfromPref.bLabel =
                m_pTableInfo->index(nRow, 5).data().toBool();
            break;
          }
        }
      }

      ittfind.value() = stPlatfromPref;

      // qDebug() << "Internal ID " << m_nSelectInfoID;
      if (m_pParent != nullptr && m_nSelectInfoID != 0) {
        m_pParent->SetUpdatePref(m_nSelectInfoID, stPlatfromPref);
      }
    }
  }
}

void DCSList_tab::on_EDIT_Length_textEdited(const QString &arg1) {
  SetUpdatePref();
}

void DCSList_tab::on_EDIT_Width_textEdited(const QString &arg1) {
  SetUpdatePref();
}

void DCSList_tab::on_COMBO_Type_currentIndexChanged(int index) {
  SetUpdatePref();
}

void DCSList_tab::on_COMBO_Color_currentIndexChanged(int index) {
  SetUpdatePref();
}

void DCSList_tab::slot_DrawChecked(QModelIndex index) {
  // qDebug() << "Draw" << bChecked;
  if (index.data().toBool() == false) {
    m_pTableInfo->setData(m_pTableInfo->index(index.row(), 4), "false");
    m_pTableInfo->setData(m_pTableInfo->index(index.row(), 5), "false");
  } else {
    m_pTableInfo->setData(m_pTableInfo->index(index.row(), 4), "true");
    m_pTableInfo->setData(m_pTableInfo->index(index.row(), 5), "true");
  }
  m_nSelectInfoID = m_pTableInfo->index(index.row(), 1).data().toInt();
  SetUpdatePref();
}

void DCSList_tab::slot_TrackChecked(QModelIndex index) {
  if (index.data().toBool() == true) {
    m_pTableInfo->setData(m_pTableInfo->index(index.row(), 3), "true");
  }
  m_nSelectInfoID = m_pTableInfo->index(index.row(), 1).data().toInt();
  SetUpdatePref();
}
void DCSList_tab::slot_LabelChecked(QModelIndex index) {
  if (index.data().toBool() == true) {
    m_pTableInfo->setData(m_pTableInfo->index(index.row(), 3), "true");
  }
  m_nSelectInfoID = m_pTableInfo->index(index.row(), 1).data().toInt();
  SetUpdatePref();
}

void DCSList_tab::on_CHECK_Scale_clicked() { SetUpdatePref(); }

void DCSList_tab::SetLogList(QVector<UpdatePlatform> vUpdatePlatform) {
  if (m_pTableLog != nullptr) {
    m_pTableLog->removeRows(0, m_pTableLog->rowCount());
    for (auto itt : vUpdatePlatform) {
      QList<QStandardItem *> items;
      auto posInfo = itt.gf_GetLLAPos(false);
      int nItem = 0;

      QString strText = QString::number(itt.time, 'f', 3);
      items.append(new QStandardItem(strText));
      items.at(nItem)->setTextAlignment(Qt::AlignCenter);
      nItem++;

      strText = QString::number(posInfo.lat() * UTILS::CU_RAD2DEG, 'f', 3);
      items.append(new QStandardItem(strText));
      items.at(nItem)->setTextAlignment(Qt::AlignCenter);
      nItem++;

      strText = QString::number(posInfo.lon() * UTILS::CU_RAD2DEG, 'f', 3);
      items.append(new QStandardItem(strText));
      items.at(nItem)->setTextAlignment(Qt::AlignCenter);
      nItem++;

      strText = QString::number(posInfo.alt(), 'f', 3);
      items.append(new QStandardItem(strText));
      items.at(nItem)->setTextAlignment(Qt::AlignCenter);
      nItem++;

      strText = QString::number(posInfo.yaw(), 'f', 3);
      items.append(new QStandardItem(strText));
      items.at(nItem)->setTextAlignment(Qt::AlignCenter);
      nItem++;

      strText = QString::number(posInfo.pitch(), 'f', 3);
      items.append(new QStandardItem(strText));
      items.at(nItem)->setTextAlignment(Qt::AlignCenter);
      nItem++;

      strText = QString::number(posInfo.roll(), 'f', 3);
      items.append(new QStandardItem(strText));
      items.at(nItem)->setTextAlignment(Qt::AlignCenter);
      nItem++;

      strText = QString::number(posInfo.vx(), 'f', 3);
      items.append(new QStandardItem(strText));
      items.at(nItem)->setTextAlignment(Qt::AlignCenter);
      nItem++;

      strText = QString::number(posInfo.vy(), 'f', 3);
      items.append(new QStandardItem(strText));
      items.at(nItem)->setTextAlignment(Qt::AlignCenter);
      nItem++;

      strText = QString::number(posInfo.vz(), 'f', 3);
      items.append(new QStandardItem(strText));
      items.at(nItem)->setTextAlignment(Qt::AlignCenter);
      nItem++;

      m_pTableLog->appendRow(items);
    }
  }
}
