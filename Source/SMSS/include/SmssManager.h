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
//#include "ConsoleDebug.h"
#include "parsser.h"

#include "DCS.h"

#include "../KDIS/KDIS/Extras/PDU_Factory.h"
#include "../KDIS/KDIS/Network/Connection.h" // A cross platform connection class.
#include "../KDIS/KDIS/PDU/Entity_Info_Interaction/Entity_State_PDU.h"


#include "../Common/DCSServer.h"
#include "../Common/datamngr.h"

using namespace KDIS;
using namespace DATA_TYPE;
using namespace PDU;
using namespace ENUMS;
using namespace NETWORK;



using namespace std;

typedef enum  
{
    eSite_MCC = 0 ,
    eSite_PLOTXY ,
    eSite_MTIS ,
    eSite_SIMUL ,
    eSite_TDPS 
}eSite;

class CSmssManager : public QThread
{
    Q_OBJECT;

public:
    CSmssManager(QObject* parent);
    virtual ~CSmssManager();

    /** RIPS initialization setting function */
    void Initialization();
    /** RIPS processing functions */
    void Process();
    /** RIPS internal variable initialization function */
    void clear();
    void callbackTest(const struct pcap_pkthdr *header, const unsigned char *pData);
    
private:
    QObject *parent_;

    /** TDPS Missle Raw Data */
    QHash<QString, LTSS_Parsser::QLMTag> QLMTag_;

    /** TDPS Missle Info */
    QHash<int, LTSS_Parsser::TDPSPlatformData> hashTDPSData;

    /** ADS-B Nomal Aircraft Info  */
    QHash<int, mode_s_Decode> adsbData_;

    /** AIS Nomal Ship Info  */
    QHash<int, LTSS_Parsser::AISPlatformData> aisData_;

    /** VR-FORCE Target Aircraft Info  */
    QHash<int, KDIS::PDU::Entity_State_PDU> entity_;

    
    DataMngr dataMngr;

    void SendtoMTIS();
    void SendtoSIMUL();
    void SendtoPLOTXY();
    void SendtoDCS();
    double yeartime();

    int nConsoleDebug ;
    

protected:
    /** QThread Virtual Function  */
    virtual void run() override;
    
    bool bSelectSite_;      /** Presence or absence of selected site   */ 
    
    int32_t iSelectSite_;   /** Selected site index value */
    
    CInterfaceManager *if_; /** SMSS Interface Manager  */

    DCSServer *dcsServer_; /** DCS Interface Manager */


signals:
    bool signal_Send(int nSite, QByteArray buffer);

public Q_SLOTS:
    void slot_Recv(int nSite, int nType, QByteArray buffer);


public:
    
};



#endif // RIPS_MANAGER_H