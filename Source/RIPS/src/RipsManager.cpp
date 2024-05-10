/*
 * Created on Tue Mar 05 2024
 *
 *  -*- mode: c++ -*- 
 ****************************************************************************
 *****                                                                  *****
 *****                   Classification: Manager                         *****
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
 * filename RipsManager.cpp
 *
 * Project :
 *
 * Purpose : 
 * The purpose of this 
 *
 * Revesion History :
 *
 */

// Default
#include "../include/RipsManager.h"

// QT 
#include <QProcess>
#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QSettings>
#include <QApplication>

// 
#include <sstream>

// sim SDK
#include "simCore/Common/SDKAssert.h"   
#include "simCore/String/CsvReader.h"

#include "ConsoleDebug.h"   // Comm include
#include "SupportType.h"    // Comm include


#ifndef DEG2RAD
#define DEG2RAD 0.017453292519943295769236907684886
#endif

// Reference origin for scenario

#define REFLAT (36.0 * DEG2RAD)
#define REFLON (127.0 * DEG2RAD)




CRipsManager::CRipsManager(QObject* parent )
{
    parent_ = parent;

    this->LoadConfigfile();

}

CRipsManager::~CRipsManager(void)
{

}


/** RIPS initialization setting function */
void CRipsManager::Initialization()
{
    if_ = new CInterfaceManager(this);

    /** DCS Server init  */
    dcsServer_ = new DCSServer(ifinfo[eSite_DCS_SERVER].name.c_str(), 
        DCSDATA_MULTICAST, 
        ifinfo[eSite_DCS_SERVER].port[0], 5808, 0, 
        ifinfo[eSite_DCS_SERVER].ip[0].toString().toStdString().c_str());


    /** RIPS - MCC Interface Protocol Add  */
    ifinfo[eSite_MCC].type = (Interface_Type)UDP;
    if_->AddInterface(ifinfo[eSite_MCC]);

    /** RIPS - MTIS Interface Protocol Add  */
    ifinfo[eSite_MTIS].type = (Interface_Type)UDP;
    if_->AddInterface(ifinfo[eSite_MTIS]);

    /** RIPS - SIMUL Interface Protocol Add  */
    ifinfo[eSite_SIMUL].type = (Interface_Type)UDP;
    if_->AddInterface(ifinfo[eSite_SIMUL]);

    int nSiteCount = 0;
    /** RIPS - TSPI Interface Protocol Add  */
    for(int i = 0; i < 4; i++)
    {   
        ifinfo[eSite_TSPI1 + i ].type = (Interface_Type)UDP;
        ifinfo[eSite_TSPI1 + i ].parser = (Parser_Type)QLM;
        if_->AddInterface(ifinfo[eSite_TSPI1 + i]);
    }

    connect(if_, SIGNAL(signal_InterfaceRecv(int, int, QByteArray)), this, SLOT(slot_Recv(int, int, QByteArray)));
    connect(this, SIGNAL(signal_Send(int,  QByteArray)), if_, SLOT(slot_Send(int,  QByteArray)));


    //*  DCS Initialization */
    // Responsible for starting the DCS server based on
    // values set in the constructor.
    dcsServer_->start();

    // Sets expiration time limit (secs) for all objects,
    // value must be > 0
    dcsServer_->setExpireTimeLimit(300.);

    // Sends scenario initialization information to the server
    // and subsequently to all connected clients.
    // Get current time & year for scenario
    time_t t = time(NULL);
    struct tm* gmt = gmtime(&t);
    int currentYear = gmt->tm_year + 1900;
    double dtime = yeartime();

    //!after : DCS Scenario info RIPSConfig.ini file scenario info apply     */
    dcsServer_->sendScenarioHeader(DCSCOORD_ENU, DCSCLASS_UNCLASSIFIED, 
        currentYear, 36.0 * DEG2RAD, 127.0 * DEG2RAD, 
        dtime, 1.0, DCSTIME_PACKET);
}

/** RIPS processing functions */
void CRipsManager::Process()
{

}
/** RIPS internal variable initialization function */
void CRipsManager::clear()
{

}

/** QThread Virtual Function  */
void CRipsManager::run()
{
    //CONSOLE_DEBUG::KeyState keystate;                     // Key state for dcsAuxGetKeyState

    iSelectSite_ = 1;
    
    
    // 
    while(true)
    {
        // #ifdef WIN32        
        //         QProcess::execute("cmd /c cls");
        // #else
        //         QProcess::execute("clear");
        // #endif

        //qDebug() << " RipsManager Thread ";

        // if(CONSOLE_DEBUG::GetKeyState(keystate, KEY_SLEEP) == CONSOLE_DEBUG::DCSAUX_QUIT)
        // {
        //     QCoreApplication *app = (QCoreApplication *)parent_;
        //     app->quit();
        //     // Quit application
        //     break;
        // }

        // data Send
        this->SendtoMTIS();
        this->SendtoSIMUL();
        this->SendtoPLOTXY();

        QThread::msleep(100);
    }
}

void CRipsManager::slot_Recv(int nSite,int nType, QByteArray buffer)
{
    // site check 

    // TSPI 
    LTSS_Parsser::TDPSPlatformData data;

    /** Processes RAW data received via UDP from the TSPI site.
        QLM parser role */
    if(nSite >= eSite_TSPI1 && nSite <= eSite_TSPI5)
    {
        // byte array to tdps data 
        data =  LTSS_Parsser::QLMParser(buffer, 0);

        auto tdpsdata = hashTDPSData.find(nSite);

        // update data
        if(tdpsdata != hashTDPSData.end() )
        {
            data.id = nSite;
            hashTDPSData[nSite] = data;
        }
        else    // add data
        {
            hashTDPSData.insert(nSite, data);
        }        
    }
    else if(nSite == (int)eSite_MCC)   /** Processes scenario information and BSS selection information from mcc.  */                            
    {
        // ! Senario info Processes 

        // ! BSS Selection info Processes 


    }

}

void CRipsManager::LoadConfigfile() /** ini file and csv file Load  */
{

    /** Configure  ini file load  */
    memset(&ifinfo, 0, sizeof(ifinfo));

    QString path = QApplication::applicationDirPath() + "/Config/RIPSConfig.ini";
    QSettings setting(path, QSettings::IniFormat);

    setting.beginGroup("DCS_SERVER");
    QString sIp = setting.value("IP", "").toString();
    QString sPort = setting.value("PORT", "").toString();
    QString sName = setting.value("NAME", "").toString();
    setting.endGroup();

    std::cout << "Init Path =  " << path.toStdString() << std::endl;
    std::cout << "DCS Name =  " << sName.toStdString() << std::endl;
    std::cout << "DCS IP =  " << sIp.toStdString() << std::endl;
    std::cout << "DCS Port =  " << sPort.toStdString() << std::endl;
    

    ifinfo[eSite_DCS_SERVER].ip[0] = QHostAddress(sIp);
    ifinfo[eSite_DCS_SERVER].port[0] = sPort.toInt();
    ifinfo[eSite_DCS_SERVER].name = sName.toStdString();
    ifinfo[eSite_DCS_SERVER].id = eSite_DCS_SERVER;

    /** RIPS to MCC Configure */
    setting.beginGroup("RIPS_MCC");
    ifinfo[eSite_MCC].ip[0] = QHostAddress(setting.value("LocalIP", "").toString());
    ifinfo[eSite_MCC].ip[1] = QHostAddress(setting.value("RemoteIP", "").toString());
    ifinfo[eSite_MCC].port[0] = setting.value("LocalPORT", "").toInt();
    ifinfo[eSite_MCC].port[1] = setting.value("RemotePORT", "").toInt();
    ifinfo[eSite_MCC].name = setting.value("NAME", "").toString().toStdString();
    ifinfo[eSite_MCC].id = eSite_MCC;
    setting.endGroup();

    /** RIPS to MTIS  Configure */
    setting.beginGroup("RIPS_MTIS");
    ifinfo[eSite_MTIS].ip[0] = QHostAddress(setting.value("LocalIP", "").toString());
    ifinfo[eSite_MTIS].ip[1] = QHostAddress(setting.value("RemoteIP", "").toString());
    ifinfo[eSite_MTIS].port[0] = setting.value("LocalPORT", "").toInt();
    ifinfo[eSite_MTIS].port[1] = setting.value("RemotePORT", "").toInt();
    ifinfo[eSite_MTIS].name = setting.value("NAME", "").toString().toStdString();
    ifinfo[eSite_MTIS].id = eSite_MTIS;
    setting.endGroup();

    /** RIPS to SIMUL Configure */
    setting.beginGroup("RIPS_SIMUL");
    ifinfo[eSite_SIMUL].ip[0] = QHostAddress(setting.value("LocalIP", "").toString());
    ifinfo[eSite_SIMUL].ip[1] = QHostAddress(setting.value("RemoteIP", "").toString());
    ifinfo[eSite_SIMUL].port[0] = setting.value("LocalPORT", "").toInt();
    ifinfo[eSite_SIMUL].port[1] = setting.value("RemotePORT", "").toInt();
    ifinfo[eSite_SIMUL].name = setting.value("NAME", "").toString().toStdString();
    ifinfo[eSite_SIMUL].id = eSite_SIMUL;
    setting.endGroup();

    /** RIPS to TSPI Configure 5EA site */
    setting.beginGroup("TSPI");

    for(int i = 0 ; i < 5;i++)
    {
        QString strTemp;
        strTemp = QString("TSPI%1IP").arg(i+1);
        ifinfo[eSite_TSPI1 + i].ip[0] = QHostAddress(setting.value(strTemp, "").toString());
        
        strTemp = QString("TSPI%1Port").arg(i+1);
        ifinfo[eSite_TSPI1 + i].port[0] = setting.value(strTemp, "").toInt();
        //ifinfo[3].port[1] = setting.value("RemotePORT", "").toInt();
        
        strTemp = QString("TSPI%1NAME").arg(i+1);
        ifinfo[eSite_TSPI1 + i].name = setting.value(strTemp, "").toString().toStdString();
        ifinfo[eSite_TSPI1 + i ].id = eSite_TSPI1;
    }

    setting.endGroup();



    //CSV file read to qlm tag check KeyState
    QFile f(QApplication::applicationDirPath() + "RIPS/QLMTabular.csv");

    f.open(QIODevice::ReadOnly);
    QString str = f.readAll();
    std::istringstream stream(str.toStdString());

    simCore::CsvReader reder(stream);
    std::vector<std::string> tokens;        

    int nLineNumber = 0;
    // 
    while(!reder.readLine(tokens))
    {
        nLineNumber++;
        int ntokensCnt (tokens.size());
        QString strtokens(tokens[0].c_str());
        
        LTSS_Parsser::QLMTag tag;
        tag.index = atoi(tokens[0].c_str()) - 1 ;
        tag.name = tokens[1].c_str();
        tag.identity = tokens[2].c_str();
        tag.unit = "";

        QLMTag_.insert(tag.identity, tag);
    }
}

void CRipsManager::SendtoMTIS()
{
    QByteArray data;
    static uint64_t tempCount = 0;

    auto taginfo =  QLMTag_.value("FLIGHT_TIME");
    // if(taginfo.index == 0 )
    //     return;
    
    taginfo.index;

    auto siteData = hashTDPSData.value(iSelectSite_);

    // if(siteData.id == 0 )
    //     return;

    double dFlightTime  = siteData.data[taginfo.index];
    

    RIPS_MTIS_TYPE sendData;
    memset(&sendData, 0, sizeof(sendData));

    sendData.datasize = 14;
    sendData.opcode = 64;
    sendData.reset = 1;
    
    sendData.time = tempCount++;// (uint64_t)dFlightTime;

    char buffTemp[sizeof(sendData)];
    memcpy(buffTemp, &sendData, sizeof(sendData));

    data = QByteArray().fromRawData(buffTemp, sizeof(sendData));

    emit signal_Send(eSite_MTIS, data);
        

    data.clear();
    MCC_MTIS_TYPE sendDataTitle;
    memset(&sendDataTitle, 0, sizeof(sendDataTitle));

    sendDataTitle.datasize = 14;
    sendDataTitle.opcode = 60;

    QString str = QString("날짜(22xx.xx.xx(월))");
    sprintf(sendDataTitle.cDate,"%s",(const char *)str.toLocal8Bit().toStdString().c_str());
    QString str2 = QString("타이틀(00년00실사격 시험)");
    sprintf(sendDataTitle.cTitle,"%s",(const char *)str2.toLocal8Bit().toStdString().c_str());

    //char charStr[50];
    //memset(&charStr,0,sizeof(charStr));

    //std::string strUtf8 = str.toLocal8Bit().toStdString().c_str();
    //QString str1 = QString().fromLocal8Bit(strUtf8.c_str());
    //sprintf(charStr, (const char*)str.toLocal8Bit().toStdString().c_str());

    //QString str1 = QString().fromLocal8Bit(charStr);

    
    //qDebug() << "한글 테스트 " << str1;
    char buffTemp1[sizeof(sendDataTitle)];
    memcpy(buffTemp1, &sendDataTitle, sizeof(sendDataTitle));

    data = QByteArray().fromRawData(buffTemp1, sizeof(sendDataTitle));

    RIPS_MTIS_TYPE *recvdata = (RIPS_MTIS_TYPE *)data.data(); 

    
    

    // BSS Flight Time Send
    emit signal_Send(eSite_MTIS, data);
}

void CRipsManager::SendtoSIMUL()
{
    // BSS QLM Data Send
    QByteArray data;

    auto siteData = hashTDPSData.value(iSelectSite_);

    if(siteData.id == 0 )
        return;

    // BSS QLM Data
    //emit signal_Send(eSite_SIMUL, data);

}

void CRipsManager::SendtoPLOTXY()
{
    // BSS QLM Data Send
    QByteArray data;

    auto siteData = hashTDPSData.value(iSelectSite_);

    if(siteData.id == 0 )
    {
        // 선택된 TDPS(QLM) 데이터를 DCS Data (Platform, Beam, Gate, Event )변환하여 
        // 로 전송한다.
        
        return;
    }
}

// Time in seconds since the beginning of the year
double CRipsManager::yeartime() {
  struct timeval tp;

  // get the current system time, using timezone value of 0
  // returns UTC time
  gettimeofday(&tp, 0);

  // put system time into a tm struct
  time_t t(tp.tv_sec);
  struct tm* p_time = gmtime(&t);

  // assemble a UTC "system time"
  unsigned int p_secs = (unsigned int)(p_time->tm_sec) +
                        (((unsigned int)(p_time->tm_min)) * 60) +
                        (((unsigned int)(p_time->tm_hour)) * 60 * 60) +
                        (((unsigned int)(p_time->tm_yday)) * 60 * 60 * 24);

  return (p_secs + (tp.tv_usec * 1e-06));
}
