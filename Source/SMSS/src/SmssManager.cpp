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


#include "../include/SmssManager.h"

#include "Constants/InterfaceType.h"
#include <QProcess>
#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <qcommandlineparser.h>

#include <sstream>

#include "simCore/Common/SDKAssert.h"
#include "simCore/String/CsvReader.h"

#include "../Common/PCAPComm.h"


#include "ConsoleDebug.h"

#include "DCS.h"



#ifndef DEG2RAD
#define DEG2RAD 0.017453292519943295769236907684886
#endif

// Reference origin for scenario

#define REFLAT (36.0 * DEG2RAD)
#define REFLON (127.0 * DEG2RAD)

using namespace std;
//using namespace PCAPComm;

#ifndef WIN32
    #include <pcap.h>
    #define CAPTURE_DEVICE "enp0"
#endif

CSmssManager::CSmssManager(QObject* parent )
{
    parent_ = parent;
    //. 
    if_ = new CInterfaceManager(this);\
    dcsServer_ = new DCSServer("SMSS DCS Server", DCSDATA_MULTICAST, 5555, 5808, 0,"192.168.1.20");
    dataMngr.InitData();
    nConsoleDebug  = CONSOLE_DEBUG::DCSAUX_NONE;
}

CSmssManager::~CSmssManager(void)
{

}


/** RIPS initialization setting function */
void CSmssManager::Initialization()
{


    InterfaceInfo info;
    info.id  = 1;
    info.name = " SBS-3 AIR Platfrom (ADS-B Data) ";
    info.type = (Interface_Type)TCP_CLIENT ;
    info.ip[0] = QHostAddress ("192.168.103.61"); // Local 
    info.port[0] = 11000;

    if_->AddInterface(info);

    info.id  = 2;
    info.name = " NMEA 0183 SHIP Platfrom  (AIS Data)";
    info.type = (Interface_Type)TCP_CLIENT ;
    info.ip[0] = QHostAddress ("127.0.0.1");
    info.port[0] = 3100;

    if_->AddInterface(info);
    
    info.id  = 3;
    info.name = " DIS";
    info.type = (Interface_Type)UDP ;
    info.ip[0] = QHostAddress().Any;
    info.port[0] = 3000;

    if_->AddInterface(info);


    // info.id  = 3;
    // info.name = " test TCP Client";
    // info.type = (Interface_Type)TCP_CLIENT ;
    // info.ip[0] = QHostAddress ("127.0.0.1");
    // info.port[0] = 7000;

    // if_->AddInterface(info);

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

    dcsServer_->sendScenarioHeader(DCSCOORD_ENU, DCSCLASS_UNCLASSIFIED, currentYear,
                                36.0 * DEG2RAD, 127.0 * DEG2RAD, dtime, 1.0, DCSTIME_PACKET);

    
}

/** RIPS processing functions */
void CSmssManager::Process()
{

}
/** RIPS internal variable initialization function */
void CSmssManager::clear()
{

}

/** QThread Virtual Function  */
void CSmssManager::run()
{
    //dcsAuxKeyState keystate;                     // Key state for dcsAuxGetKeyState

    iSelectSite_ = 1;
    // csv file read to qlm tag check 
    QFile f("./RIPS/QLMTabular.csv");

    f.open(QIODevice::ReadOnly);
    QString str = f.readAll();
    std::istringstream stream(str.toStdString());

    simCore::CsvReader reder(stream);
    std::vector<std::string> tokens;        

    int nLineNumber = 0;
    while(!reder.readLine(tokens))
    {
        nLineNumber++;
        int ntokensCnt (tokens.size());
        QString strtokens(tokens[0].c_str());
        qDebug() << "as " << strtokens;    
        LTSS_Parsser::QLMTag tag;
        tag.index = atoi(tokens[0].c_str()) - 1 ;
        tag.name = tokens[1].c_str();
        tag.identity = tokens[2].c_str();
        tag.unit = "";

        QLMTag_.insert(tag.identity, tag);
    }
    CONSOLE_DEBUG::KeyState keystate;                     // Key state for dcsAuxGetKeyState
    static CONSOLE_DEBUG::KeyState oldkeystate;                     // Key state for dcsAuxGetKeyState
    static CONSOLE_DEBUG::KeyState lastkeystate;                     // Key state for dcsAuxGetKeyState


    //dataMngr.SetSADSBSampleData();
    // QCommandLineParser parser;
    // // 
    // parser.setApplicationDescription("Test helper");
    // parser.addHelpOption();
    // parser.addVersionOption();
    // parser.addPositionalArgument("source", QCoreApplication::translate("main", "Source file to copy."));
    // parser.addPositionalArgument("destination", QCoreApplication::translate("main", "Destination directory."));

    // parser.process((QCoreApplication &)(this->parent_));

    while(true)
    {

// #ifdef WIN32        
//         QProcess::execute("cmd /c cls");
// #else
//         QProcess::execute("clear");
// #endif

        //qDebug() << " SmssManager Thread ";

        if(CONSOLE_DEBUG::GetKeyState(keystate, 100) == CONSOLE_DEBUG::DCSAUX_QUIT)
        {

            QCoreApplication *app = (QCoreApplication *)parent_;
            app->quit();
            // Quit application
            break;
        }
        
        if(oldkeystate != keystate )
        {
            
            oldkeystate = keystate;

            if(keystate != CONSOLE_DEBUG::DCSAUX_NONE)
            {
                lastkeystate = keystate;
                nConsoleDebug = lastkeystate;
            }            
        }
    
        // data Send
        // this->SendtoMTIS();
        // this->SendtoSIMUL();
        // this->SendtoPLOTXY();

        double dtime = yeartime();

        double eciRefTime = yeartime();


        
    //                            // Updates the current scenario time to the server
    // // and subsequently to all connected clients.
        dcsServer_->sendTime(dtime);

    // // Responsible for both servicing new client connection
    // // requests and removing expired objects.
        dcsServer_->idle(dtime);



        QMap<int, std::pair<DCSPlatformHeader, DCSPlatformData>> adsbdata = dataMngr.GetADSBData();

        auto itt = adsbdata.begin();

        DCSCom::DCSServerInfo* info = dcsServer_->getServerInfo();


        // DCS Client check 
        if(info->clientlist.size() != 0)
        {
            // for ADS-B data 
            for (itt; itt != adsbdata.end(); itt++) 
            {
                if(itt.value().first.interpolate_ )
                {
                
                    
                    dataMngr.SetInterpolate(itt.key(), false);
                
                }
                DCSCom::sendPlatformHeader(*dcsServer_->getServerInfo(), &itt.value().first);
                DCSCom::sendPlatformData(*dcsServer_->getServerInfo(), &itt.value().second);
                

                DCSCategoryData catData(itt.value().first.id_, dtime, DCS::getCategoryName (DCSPLATFORM_AIRCRAFT), 
                DCS::getCategoryValue (DCSPLATFORM_AIRCRAFT));

                DCSCom::sendCategoryData(*dcsServer_->getServerInfo(),&catData);

                DCSCategoryData catData1(itt.value().first.id_, dtime, DCS::getCategoryName (DCSALLEGIENCE_FRIENDLY), 
                DCS::getCategoryValue (DCSALLEGIENCE_FRIENDLY));

                 DCSCom::sendCategoryData(*dcsServer_->getServerInfo(),&catData1);
                // dcsServer_->sendCategoryData(itt.value().first.id_,
                //     DCS::getCategoryName (DCSALLEGIENCE_FRIENDLY), DCS::getCategoryValue (DCSALLEGIENCE_FRIENDLY), dtime);

                // sample position update
                // DCSPoint pos = itt.value().second.position_;
                // pos.x = KDIS::UTILS::DegToRad((KDIS::UTILS::RadToDeg(pos.x) + 0.0001) );
                // pos.y = KDIS::UTILS::DegToRad((KDIS::UTILS::RadToDeg(pos.y) + 0.0001) );
                // pos.z = pos.z;

                // dataMngr.SetSADSBPlatformPosition(itt.key(), pos);
            }

            QMap<int, std::pair<DCSPlatformHeader, DCSPlatformData>> aisdata = dataMngr.GetAISData();

            auto ittais = aisdata.begin();    
        
            // AIS Data loop
            for (ittais; ittais != aisdata.end(); ittais++) 
            {

                //if(ittais.value().first.interpolate_ )
                //{
                    DCSCom::sendPlatformHeader(*dcsServer_->getServerInfo(), &ittais.value().first);
                    dataMngr.SetInterpolateAIS(ittais.key(), false);
                //}

                DCSCom::sendPlatformData(*dcsServer_->getServerInfo(), &ittais.value().second);  
                
                dcsServer_->sendCategoryData(ittais.value().first.id_,
                    DCS::getCategoryName (DCSPLATFORM_SURFACESHIP), DCS::getCategoryValue (DCSPLATFORM_SURFACESHIP), dtime);
                dcsServer_->sendCategoryData(ittais.value().first.id_,
                    DCS::getCategoryName (DCSALLEGIENCE_FRIENDLY), DCS::getCategoryValue (DCSALLEGIENCE_FRIENDLY), dtime);
   
            }
        }
        
        


        if (false) {
            
            PCAPcomm::CPCAPThrad *pMyPCAP  = new PCAPcomm::CPCAPThrad();

            if (pMyPCAP != nullptr) {
                std::vector<pcap_if_t> vtDeviceInfo;
                pMyPCAP->GetFindAllDevice(vtDeviceInfo);

                /*
                for (size_t i = 0; i < vtDeviceInfo.size(); i++) {
                    printf("%d. %s", i + 1, vtDeviceInfo[i].name);
                    if (strcmp(vtDeviceInfo[i].description, "") > 0) {
                    printf(" (%s)\n", vtDeviceInfo[i].description);
                    } else {
                    printf(" (No description available)\n");
                    }
                }
                */
                // printf("Enter the interface number (1-%d) : ",
                // INT32(vtDeviceInfo.size())); INT32 inum = 0; STRING_TYPE strTemp =
                // _T(""); STRING_GETLINE(strTemp);

                 if (!vtDeviceInfo.empty()) {
                    // linux 1
                     int inum = 1;
                     pMyPCAP->SelectDevice(vtDeviceInfo[inum - 1]);
                    // TCHAR packet_filter[] = /*_T("ip and udp")*/_T("ip and udp and host
                    // 192.168.1.125 and port 7001 or port 7002 or port 7003 or port 7004");
                    // port Filter
                    /*
                    pMyPCAP->SetFilter(
                        gf_GetText(gf_GetModulePath() + PATH_CONFIG, _T("PCAP"),
                    _T("filter")) .GetBuffer());
                    */

                    // std::cout << "Enter the name of the dump file to create : ";

                    /// STRING_TYPE fileDirectory = _T("");

                    // STRING_GETLINE(fileDirectory);

                    pMyPCAP->CreateDumpFile("TEST.pcap");

                    pMyPCAP->SetCallbackFunction(std::bind(&CSmssManager::callbackTest, this,std::placeholders::_1, std::placeholders::_2 ));
                    pMyPCAP->StartCapture();

                    pMyPCAP->FinishCapture();
                }
            }
        }

        //DCSCom::sendPlatformHeader()
        //dcsServer_->SendADSBPlatform(dataMngr.GetADSBData());
        
        
   
        QThread::msleep(100);
        

        
        
    }
}

void CSmssManager::slot_Recv(int nSite,int nType, QByteArray buffer)
{
    // site check 

    // TDPS
    //LTSS_Parsser::TDPSPlatformData data;

    if(nSite == 1)  // ADS-B Site 
    {
        ADSBParser::mode_s_msg aisData;
        if(LTSS_Parsser::ADSBParsser(buffer, 0, aisData ))
        {
            dataMngr.SetADSBData(aisData);
            /*
            auto iterData = adsbData_.find(aisData.ICAO);

            // add ADS-B Data 
            if(iterData == adsbData_.end())
            {
                mode_s_Decode data;
                data.ICAO = aisData.ICAO;
                data.strHexID = QString::number(aisData.ICAO, 16);
                data.nDCSId = aisData.ICAO;

                auto iter = adsbData_.insert(aisData.ICAO, data);

                // TC Check Update
                LTSS_Parsser::ADSBDataUpdate(aisData, (mode_s_Decode &)*iter);
            }
            else    // Update  ADS-B Data 
            {
                LTSS_Parsser::ADSBDataUpdate(aisData, (mode_s_Decode &)*iterData);       
                // qDebug() << "ICAO " << iterData->strHexID << " CallSign " << iterData->strCallSign << 
                //     "Lat :" <<  iterData->Pos.dLat << "Long" << iterData->Pos.dLon  << 
                //     "Alt" << iterData->nAlt << "Heading" << iterData->nHeading;
            }
            */
            
        }
    }
    else if(nSite == 2) // AIS Sim Site Data 
    {
        // qDebug() << "Site : NMEA 0183 Sim ";
        // LTSS_Parsser::NMEAParser(buffer, 0 );
        LTSS_Parsser::AISPlatformData data;
        if(LTSS_Parsser::AISParser(buffer, data, nConsoleDebug == CONSOLE_DEBUG::AUX_A ? true: false  ))
        {
            //data
            dataMngr.SetAISData(data);
        }
    }
    else if(nSite == 3)
    {
        //LTSS_Parsser::DISParser(buffer,0 );
        KDIS::UTILS::PDU_Factory factory;

        KDIS::KOCTET *Buffer = buffer.data();
        std::unique_ptr<KDIS::PDU::Header> pHeader = factory.Decode(Buffer, (KDIS::KUINT16)buffer.size());

        if (pHeader.get()) 
        {
            if (pHeader->GetPDUType() == KDIS::DATA_TYPE::ENUMS::Entity_State_PDU_Type) 
            {
                KDIS::PDU::Entity_State_PDU entity;
                KDataStream ds(Buffer, (KDIS::KUINT16)buffer.size());
                entity.Decode(ds);

                entity_.insert((int)entity.GetEntityIdentifier().GetEntityID(), entity);

                qDebug() << " DIS Protocol Recv ! ID:" << (int)entity.GetEntityIdentifier().GetEntityID();

                dataMngr.SetDISData(&entity);
                // Q_EMIT RecvDISData(&m_Entity);b  c
                
            }
        }

        
        
    }

}

void CSmssManager::SendtoMTIS()
{
    QByteArray data;

    auto taginfo =  QLMTag_.value("FLIGHT_TIME");
    if(taginfo.index == 0 )
        return;
    
    taginfo.index;

    auto siteData = hashTDPSData.value(iSelectSite_);

    if(siteData.id == 0 )
        return;

    double dFlightTime  = siteData.data[taginfo.index];
    
    // BSS Flight Time Send
    emit signal_Send(eSite_MTIS, data);
}

void CSmssManager::SendtoSIMUL()
{
    // BSS QLM Data Send
    QByteArray data;

    auto siteData = hashTDPSData.value(iSelectSite_);

    if(siteData.id == 0 )
        return;

    // BSS QLM Data
    //emit signal_Send(eSite_SIMUL, data);

}

void CSmssManager::SendtoPLOTXY()
{
    // BSS QLM Data Send
    QByteArray data;


    auto siteData = hashTDPSData.value(iSelectSite_);

    if(siteData.id == 0 )
        return;

        
    // BSS QLM Data
    //emit signal_Send(eSite_PLOTXY, data);
}

void CSmssManager::SendtoDCS()
{

}

// Time in seconds since the beginning of the year
double CSmssManager::yeartime() {
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

void CSmssManager::callbackTest(const struct pcap_pkthdr *header,const unsigned char *pData) 
{


    //qDebug() << " NPCAP Protocol Recv " ;

}