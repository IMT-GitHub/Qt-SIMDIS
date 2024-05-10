#include "tdpslist_tab.h"

#include "../../Common/define.h"
#include "tdpslist.h"
#include "ui_tdpslist_tab.h"
TDPSList_tab::TDPSList_tab(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::TDPSList_tab),
      m_pParent(reinterpret_cast<TDPSList *>(parent)),
      m_nInternalID(0),
      m_nMsgCount(0),
      m_pTableInfo(nullptr),
      m_pTableStatus(nullptr),
      m_pTableLog(nullptr),
      m_pBtnRefresh(nullptr) {
  ui->setupUi(this);

  for (int i = 0; i < CHECK_CNT; i++) {
    m_pCheckBoxDelegate[i] = nullptr;
    m_nDelegateCol[i] = 2 + i;
  }

  InitInfoList();
  InitStatusList();
  CreateBtn();
  // InitLogList();
}

TDPSList_tab::~TDPSList_tab() {
  delete ui;

  m_mapInfoList.clear();

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

void TDPSList_tab::CreateBtn() {
  if (m_pBtnRefresh == nullptr) {
    m_pBtnRefresh = new QPushButton("Button", this);
    m_pBtnRefresh->setGeometry(QRect(125, 136, 22, 22));
    m_pBtnRefresh->setText("");
    // m_pBtnRefresh->setStyleSheet("background-color: rgba(100, 100,
    // 100,128);");
    m_pBtnRefresh->setIcon(QIcon(":/image/res/BTNREFRESH.png"));
    m_pBtnRefresh->setIconSize(QSize(16, 16));
    connect(m_pBtnRefresh, SIGNAL(clicked()), this,
            SLOT(slot_RefreshBTN_Clicked()));
  }
}

void TDPSList_tab::slot_RefreshBTN_Clicked() {
  if (m_pParent != nullptr) m_pParent->SetReqestLogData(m_nInternalID);
}

void TDPSList_tab::InitInfoList() {
  m_stDefultPref;  // 초기화

  QStringList strListHeader = {
      tr("Internal ID"), tr("Callsign"), tr("Draw"),
      tr("Track"),       tr("Label"),    tr("Count"),
  };

  if (m_pTableInfo != nullptr) {
    delete m_pTableInfo;
    m_pTableInfo = nullptr;
  }
  m_pTableInfo = new QStandardItemModel();

  m_pTableInfo->setHorizontalHeaderLabels(strListHeader);
  ui->QTable_Info->setModel(m_pTableInfo);
  ui->QTable_Info->verticalHeader()->setFixedWidth(30);
  ui->QTable_Info->horizontalHeader()->setSectionsClickable(false);

  ui->QTable_Info->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  ui->QTable_Info->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->QTable_Info->setSelectionMode(QAbstractItemView::NoSelection);
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

/**
 * @brief TDPSList_tab::InitList
 * 리스트 정보 초기 설정
 */
void TDPSList_tab::InitStatusList() {
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
/*
void TDPSList_tab::InitLogList() {
  // user Tag 추가 설정 필요

  QStringList strListHeader = {
      tr("TimeLocal"),   tr("Lat(deg)"),      tr("Lon(deg)"),
      tr("Alt(m)"),      tr("Yaw(deg)"),      tr("Pitch(deg)"),
      tr("Roll(deg)"),   tr("Vx(E) (m/sec)"), tr("Vy(N) (deg)"),
      tr("Vz(U) (deg)"), tr("Speed (m/sec)"),
  };
  if (m_pTableLog == nullptr)
    m_pTableLog = new QStandardItemModel();
  else
    m_pTableLog->clear();

  m_pTableLog->setHorizontalHeaderLabels(strListHeader);
  ui->QTable_Log->setModel(m_pTableLog);
  ui->QTable_Log->verticalHeader()->setFixedWidth(30);
  ui->QTable_Log->horizontalHeader()->setSectionsClickable(false);

  ui->QTable_Log->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  ui->QTable_Log->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->QTable_Log->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
*/
void TDPSList_tab::InitLogList(QVector<EuDataList> vEuDataList) {
  QStringList strListHeader;
  strListHeader.append("TimeLocal");
  for (auto itt : vEuDataList) {
    QString strHeader = "";
    if (itt.strTagName.compare("user") != 0)
      strHeader = QString("%1(%2)").arg(itt.strTagName).arg(itt.strText);
    else
      strHeader = QString("%1(%2)").arg(itt.strUserTagName).arg(itt.strText);

    strListHeader.append(strHeader);
  }

  if (m_pTableLog != nullptr) {
    delete m_pTableLog;
    m_pTableLog = nullptr;
  }

  m_pTableLog = new QStandardItemModel();

  m_pTableLog->setHorizontalHeaderLabels(strListHeader);
  ui->QTable_Log->setModel(m_pTableLog);
  ui->QTable_Log->verticalHeader()->setFixedWidth(30);

  ui->QTable_Log->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  ui->QTable_Log->horizontalHeader()->setDefaultSectionSize(5);

  ui->QTable_Log->horizontalHeader()->setSectionsClickable(false);
  ui->QTable_Log->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);

  ui->QTable_Log->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->QTable_Log->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void TDPSList_tab::SetPrefInfo(TDPSConfig stTDPSConfig) {
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

  ui->EDIT_Length->setText(QString("%1").arg(stTDPSConfig.nTrackLength));
  ui->EDIT_Width->setText(QString("%1").arg(stTDPSConfig.nTrackWidth));

  ui->COMBO_Color->setCurrentText(stTDPSConfig.strTrackColor);
  ui->COMBO_Type->setCurrentText(stTDPSConfig.strTrackType);
  ui->CHECK_Scale->setChecked(stTDPSConfig.bDynamic);

  m_stDefultPref.nTrackLength = stTDPSConfig.nTrackLength;
  m_stDefultPref.nTrackWidth = stTDPSConfig.nTrackWidth;

  m_stDefultPref.strTrackColor = stTDPSConfig.strTrackColor;
  m_stDefultPref.strTrackType = stTDPSConfig.strTrackType;

  m_stDefultPref.bDynamic = stTDPSConfig.bDynamic;
}

/**
 * @brief TDPSList_tab::SetInfoList
 * 시나리오 정보 기반 PlatfomInfo 설정
 */
void TDPSList_tab::SetInfoList(TestListInfo stTestListInfo) {
  QList<QStandardItem *> items;
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

/**
 * @brief TDPSList_tab::SetStatusList
 * 실시간 상태정보 전시
 * new QStandardItem 선언해도 QStandardItemModel 내부서 자동 삭제됨
 */
void TDPSList_tab::SetStatusList(UpdatePlatform stUpdatePlatform) {
  if (isEnabled() == false) setEnabled(true);
  auto ittfind = m_mapInfoList.find(stUpdatePlatform.nID);
  if (ittfind == m_mapInfoList.end()) {
    // 최초 생성시 PlatformInfo List 구성
    m_nInternalID = stUpdatePlatform.nID;
    TestListInfo stTestListInfo;
    stTestListInfo.nInternalID = stUpdatePlatform.nID;
    stTestListInfo.strCallsign = stUpdatePlatform.stAddPlatform.strCallSign;

    SetInfoList(stTestListInfo);

    PlatfromPref stPlatfromPref = m_stDefultPref;
    m_mapInfoList.insert(stUpdatePlatform.nID, stPlatfromPref);
  }

  if (m_nInternalID == stUpdatePlatform.nID) {
    if (m_pTableStatus != nullptr) {
      // 실시간 상태 정보
      QStringList strList;
      strList.append(QString::number(stUpdatePlatform.time, 'f', 3));

      for (int i = 0; i < UpdatePlatform::TAG_CNT; i++) {
        strList.append(QString::number(stUpdatePlatform.dTag[i], 'f', 3));
      }

      // Update Value
      int nCol = 0;
      for (auto itt : strList) {
        m_pTableStatus->setData(m_pTableStatus->index(0, nCol), itt);
        nCol++;
      }
    }
  }

  m_nMsgCount++;
  m_pTableInfo->setData(m_pTableInfo->index(0, 5), m_nMsgCount);
}

void TDPSList_tab::SetLogList(QVector<UpdatePlatform> vUpdatePlatform) {
  if (m_pTableLog != nullptr) {
    m_pTableLog->removeRows(0, m_pTableLog->rowCount());
    for (auto itt : vUpdatePlatform) {
      QStringList strList;
      strList.append(QString::number(itt.time, 'f', 3));

      for (int i = 0; i < UpdatePlatform::TAG_CNT; i++) {
        strList.append(QString::number(itt.dTag[i], 'f', 3));
      }

      QList<QStandardItem *> items;
      int nCnt = 0;
      for (auto itt : strList) {
        items.append(new QStandardItem(itt));
        items.at(nCnt)->setTextAlignment(Qt::AlignCenter);
        nCnt++;
      }

      m_pTableLog->appendRow(items);
    }
  }
}

void TDPSList_tab::SetUpdatePref() {
  if (isEnabled() == true) {
    PlatfromPref stPlatfromPref;

    stPlatfromPref.nTrackLength = ui->EDIT_Length->text().toInt();
    stPlatfromPref.nTrackWidth = ui->EDIT_Width->text().toInt();

    stPlatfromPref.strTrackColor = ui->COMBO_Color->currentText();
    stPlatfromPref.strTrackType = ui->COMBO_Type->currentText();
    stPlatfromPref.bDynamic = ui->CHECK_Scale->isChecked();

    if (m_pTableInfo != nullptr) {
      stPlatfromPref.bDraw = m_pTableInfo->index(0, 2).data().toBool();
      stPlatfromPref.bTrack = m_pTableInfo->index(0, 3).data().toBool();
      stPlatfromPref.bLabel = m_pTableInfo->index(0, 4).data().toBool();
    }

    // qDebug() << "Internal ID " << m_nInternalID;
    if (m_pParent != nullptr && m_nInternalID != 0) {
      m_pParent->SetUpdatePref(m_nInternalID, stPlatfromPref);
    }
  }
}

void TDPSList_tab::on_EDIT_Length_textEdited(const QString &arg1) {
  SetUpdatePref();
}

void TDPSList_tab::on_EDIT_Width_textEdited(const QString &arg1) {
  SetUpdatePref();
}

void TDPSList_tab::on_COMBO_Type_currentIndexChanged(int index) {
  SetUpdatePref();
}

void TDPSList_tab::on_COMBO_Color_currentIndexChanged(int index) {
  SetUpdatePref();
}

void TDPSList_tab::slot_DrawChecked(QModelIndex index) {
  // qDebug() << "Draw" << bChecked;
  if (index.data().toBool() == false) {
    m_pTableInfo->setData(m_pTableInfo->index(0, 3), "false");
    m_pTableInfo->setData(m_pTableInfo->index(0, 4), "false");
  } else {
    m_pTableInfo->setData(m_pTableInfo->index(0, 3), "true");
    m_pTableInfo->setData(m_pTableInfo->index(0, 4), "true");
  }
  SetUpdatePref();
}

void TDPSList_tab::slot_TrackChecked(QModelIndex index) {
  if (index.data().toBool() == true) {
    m_pTableInfo->setData(m_pTableInfo->index(0, 2), "true");
  }
  SetUpdatePref();
}
void TDPSList_tab::slot_LabelChecked(QModelIndex index) {
  if (index.data().toBool() == true) {
    m_pTableInfo->setData(m_pTableInfo->index(0, 2), "true");
  }
  SetUpdatePref();
}

void TDPSList_tab::on_CHECK_Scale_clicked() { SetUpdatePref(); }

void TDPSList_tab::on_pushButton_clicked() {
  if (m_pTableLog != nullptr)
    m_pTableLog->removeRows(0, m_pTableLog->rowCount());
}
