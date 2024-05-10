#ifndef DCSLIST_TAB_H
#define DCSLIST_TAB_H

#include <QDialog>
#include <QPushButton>

#include "../../Common/Global.h"
#include "../../Common/Struct.h"
namespace Ui {
class DCSList_tab;
}

class DCSList;
class DCSList_tab : public QDialog {
  Q_OBJECT

  enum {
    CHECK_DRAW = 0,
    CHECK_TRACK,
    CHECK_LABEL,
    CHECK_CNT,
  };

 public:
  explicit DCSList_tab(QWidget *parent = nullptr);
  ~DCSList_tab();
  void SetInfoList(TestListInfo stTestListInfo);
  void SetStatusList(UpdatePlatform stUpdatePlatform);
  void SetPrefInfo(DCSConfig stDCSConfig);
  void SetLogList(QVector<UpdatePlatform> vUpdatePlatform);

 private slots:
  void on_QTable_Info_clicked(const QModelIndex &index);

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

 private:
  void InitInfoList();
  void SetUpdatePref();
  void SetSelectPrefInfo();
  void SetSelectStatus();
  void CreateBtn();
  void InitLogList();
  void InitStatusList();

 private:
  Ui::DCSList_tab *ui;
  QStandardItemModel *m_pTableInfo;
  QStandardItemModel *m_pTableStatus;
  QStandardItemModel *m_pTableLog;

  CheckBoxDelegate *m_pCheckBoxDelegate[CHECK_CNT];
  int m_nDelegateCol[CHECK_CNT];

  // QMap<int, TestListInfo> m_mapInfoList;
  PlatfromPref m_stDefultPref;
  QMap<int, PlatfromPref> m_mapInfoList;  // Info 정보 및 속성정보
  QMap<int, UpdatePlatform> m_mapStatusInfo;

  int m_nSelectInfoID;  

  DCSList *m_pParent;
  QPushButton *m_pBtnRefresh;
};

#endif  // DCSLIST_TAB_H
