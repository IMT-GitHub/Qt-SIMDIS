#ifndef TDPSLIST_TAB_H
#define TDPSLIST_TAB_H

#include <QPushButton>
#include <QWidget>

#include "../../Common/Global.h"
#include "../../Common/Struct.h"

class TDPSList;
namespace Ui {
class TDPSList_tab;
}

class TDPSList_tab : public QWidget {
  Q_OBJECT

  enum {
    CHECK_DRAW = 0,
    CHECK_TRACK,
    CHECK_LABEL,
    CHECK_CNT,
  };

 public:
  explicit TDPSList_tab(QWidget *parent = nullptr);
  ~TDPSList_tab();
  void SetInfoList(TestListInfo stTestListInfo);
  void SetStatusList(UpdatePlatform stUpdatePlatform);

  void InitLogList(QVector<EuDataList> vEuDataList);
  void SetLogList(QVector<UpdatePlatform> vUpdatePlatform);

  void SetPrefInfo(TDPSConfig stTDPSConfig);

 private slots:
  void on_EDIT_Length_textEdited(const QString &arg1);
  void on_EDIT_Width_textEdited(const QString &arg1);
  void on_COMBO_Type_currentIndexChanged(int index);
  void on_COMBO_Color_currentIndexChanged(int index);
  void on_CHECK_Scale_clicked();
  /*
  void slot_DrawChecked(bool bChecked);
  void slot_TrackChecked(bool bChecked);
  void slot_LabelChecked(bool bChecked);
  */

  void slot_DrawChecked(QModelIndex index);
  void slot_TrackChecked(QModelIndex index);
  void slot_LabelChecked(QModelIndex index);
  void slot_RefreshBTN_Clicked();

  void on_pushButton_clicked();

 private:
  void CreateBtn();
  void InitInfoList();
  void InitStatusList();
  void SetUpdatePref();

 private:
  Ui::TDPSList_tab *ui;
  QStandardItemModel *m_pTableInfo;
  QStandardItemModel *m_pTableStatus;
  QStandardItemModel *m_pTableLog;

  CheckBoxDelegate *m_pCheckBoxDelegate[CHECK_CNT];
  int m_nDelegateCol[CHECK_CNT];

  TDPSList *m_pParent;
  int m_nInternalID;
  int m_nMsgCount;

  QPushButton *m_pBtnRefresh;

  PlatfromPref m_stDefultPref;
  QMap<int, PlatfromPref> m_mapInfoList;  // Info 정보 및 속성정보
};

#endif  // TDPSLIST_TAB_H
