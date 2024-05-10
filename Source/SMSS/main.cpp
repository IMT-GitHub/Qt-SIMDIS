/****************************************************************************
 *****                                                                  *****
 *****                   Classification: SMSS                           *****
 *****                    Classified By:                                *****
 *****                    Declassify On:                                *****
 *****                                                                  *****
 ****************************************************************************
 *
 *
 * Developed by: ⓒIMT all rights reserved. since 2024 
 *               19, Myeonghaksandanseo-ro, Yeondong-myeon, Sejong, South Korea
 *               
 * License for source code is in accompanying LICENSE.txt file. If you did
 * not receive a LICENSE.txt with this code, email IMT@imtplus.com
 *
 *
 */

/**
 * Name : n/a
 * 
 * Filename : ADBInterface.h
 * 
 * Project :
 * 
 * Purpose :
 *  The purpose of this file is to provide ADB-B/AIS,TDL,TCS,STL,AIS,UAV,VR-Force 
 * interface information processing and DCS Converting to 3D SW Send .
 * 
 * Revision History : :
 *  $Log: ale47_daemon.adb,v $
 *  Revision 1.0  
 *      2024/01/26 ssm Initial Revision
 *      2024/2/19 ssm Comm lib add 
 */

// QT 
#include <QCoreApplication>


// Default
#include <ctype.h>  /* NOLINT */
#include <stdio.h>  /* NOLINT */
#include <stdlib.h> /* NOLINT */
#include <string.h> /* NOLINT */
#include <time.h>   /* NOLINT */

#include <thread>

// Util 

// network

#ifdef WIN32
  #include "SmssManager.h"  /* Interface : AIS, ADS-B, NMEA, VR-FORCE, MUAM, LINK , ETC */
#else
  #include "include/SmssManager.h"  /* Interface : AIS, ADS-B, NMEA, VR-FORCE, MUAM, LINK , ETC */
#endif
#include "DCS.h"            /* Interface : DCS        */


// DataMgr
//


// Environment 
// 




#if defined(_MSC_VER)
#pragma warning(disable : 4996) /* deprecation */
#endif


// Suspend program for 100 milliseconds to create a 10HZ data rate.
#define SLEEP 100

// Stores information relevant to the actual DCS server
DCSCom::DCSServerInfo server;
DCSConsoleLogger logger;
SockUDP *udpState = nullptr;

bool bExit = false;

void thread_recv_udp_function_state(SockUDP *udp);    // 마스터 슬레브 상태체크
std::atomic<bool> running(true);
std::atomic<bool> master(false);



int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  initializeSocket();
  
  CSmssManager pross(&app);// = new CSmssManager(&app);

  pross.setObjectName("SMSS Manager Thread");

  pross.start();

  pross.Initialization();

  std::cout << "Press Ctl-C key to quit SMSS " << std::endl;
  printf("[IMT] SMSS  Version %f", 0.01);
  
  app.exec();

  pross.terminate();
  //pross->clear();
  //delete pross;

  finalizeSocket();
  return 0;
}

