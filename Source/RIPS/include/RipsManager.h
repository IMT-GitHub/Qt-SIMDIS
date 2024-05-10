/*
 * Created on Tue Mar 05 2024
 *
 *  -*- mode: c++ -*- 
 ****************************************************************************
 *****                                                                  *****
 *****                   Classification: Intface Module                 *****
 *****                    Classified By:                                *****
 *****                    Declassify On:                                *****
 *****                                                                  *****
 ****************************************************************************
 * *
 * *
 * Developed by: ⓒIMT all rights reserved. since 1997  
 *               19, Myeonghaksandanseo-ro, Yeondong-myeon, Sejong, South Korea
 * *
 * License for source code is in accompanying LICENSE.txt file. If you did
 * not receive a LICENSE.txt with this code, email IMT@imtplus.com 
 * *
 * ⓒIMT Research Sungmin Shin 
 * *
 * ⓒIMT retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 *
 * filename RipsManager.h
 *
 * Project : 
 *
 * Purpose : 
 * The purpose of this 
 *
 * Revesion History :
 *
 */


#ifndef RIPS_MANAGER_H
#define RIPS_MANAGER_H

#include <qmutex.h>
#include <QObject>
#include <QThread>
#include "InterfaceManager.h"
#include "ConsoleDebug.h"
#include "parsser.h"

#include "../Common/DCSServer.h"
#include "DCS.h"

using namespace std;

typedef enum  eRIPS_SITE
{
    eSite_DCS_SERVER = 0 ,
    eSite_MCC ,
    eSite_MTIS ,
    eSite_SIMUL ,
    eSite_TSPI1 ,
    eSite_TSPI2 ,
    eSite_TSPI3 ,
    eSite_TSPI4 ,
    eSite_TSPI5 
};

typedef enum  eRIPS_REMOTE
{
    REMOTE = 0 ,
    LOCAL 
};

class CRipsManager : public QThread
{
    Q_OBJECT;

public:
    CRipsManager(QObject* parent = 0);
    virtual ~CRipsManager();

    /** RIPS initialization setting function */
    void Initialization();
    /** RIPS processing functions */
    void Process();
    /** RIPS internal variable initialization function */
    void clear();

private:
    QObject *parent_;
    QHash<int , LTSS_Parsser::TDPSPlatformData> hashTDPSData;
    QHash<QString , LTSS_Parsser::QLMTag> QLMTag_;

    DCSServer *dcsServer_; /** DCS Interface Manager */

    InterfaceInfo ifinfo[10];
    
    void LoadConfigfile(); /** ini file and csv file Load  */

    void SendtoMTIS();
    void SendtoSIMUL();
    void SendtoPLOTXY();

    double yeartime();
protected:
    /** QThread Virtual Function  */
    virtual void run() override;

    
   

    
    bool bSelectSite_;      /** Presence or absence of selected site   */ 
    
    int32_t iSelectSite_;   /** Selected site index value */

    
    CInterfaceManager *if_; /** RIPS Interface Manager  */


signals:
    bool signal_Send(int nSite, QByteArray buffer);

public Q_SLOTS:
    void slot_Recv(int nSite, int nType, QByteArray buffer);


public:
    
};



#endif // RIPS_MANAGER_H