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
 * filename main.cpp
 *
 * Project : 
 *
 * Purpose : 
 * The purpose of this 
 *
 * Revesion History :
 *
 */

#include <QCoreApplication>
//#include <QApplication>
//#include <QMainWindow>
//#include <QMenuBar>
//#include <QStatusBar>
#include <QSignalMapper>
#include <QSettings>
#include <QThread>
#include <QDebug>

#include "include/RipsManager.h"
#include <ctype.h>  /* NOLINT */
#include <stdio.h>  /* NOLINT */
#include <stdlib.h> /* NOLINT */
#include <string.h> /* NOLINT */
#include <time.h>   /* NOLINT */
#include <signal.h>
#include <time.h>

#ifndef WIN32
#include <iostream>
#endif

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif

using namespace std;

int main(int argc, char *argv[])
{
 QCoreApplication app(argc, argv);

#ifdef Q_WS_X11
  // required for multi-threaded viewer on Linux:
  XInitThreads();
#endif

  CRipsManager *pross = new CRipsManager(&app);

  pross->setObjectName("RIPS Manager Thread");

  qDebug() << "qDebug RIPS  Version %f : " << 3.14;
  
  // QThread Start
  pross->start();
  

  // 초기 ini 파일 로드 

  // 통신 설정 
  pross->Initialization();
  
  // Main Thread QCoreApplication execute
  app.exec();  

  // QThread terminate
  pross->terminate();
  pross->clear();
  delete pross;
  
  return 1;
}
