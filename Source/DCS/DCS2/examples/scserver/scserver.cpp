/****************************************************************************
 *****                                                                  *****
 *****                   Classification: UNCLASSIFIED                   *****
 *****                    Classified By:                                *****
 *****                    Declassify On:                                *****
 *****                                                                  *****
 ****************************************************************************
 *
 *
 * Developed by: Naval Research Laboratory, Tactical Electronic Warfare Div.
 *               EW Modeling & Simulation, Code 5770
 *               4555 Overlook Ave.
 *               Washington, D.C. 20375-5339
 *
 * For more information please send email to simdis@enews.nrl.navy.mil
 *
 * 2002 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 *
 */

// // // // // // // // // // // // // // // // // // // // // // //
// scserver - a simple server program implemented from the DCSServer extension 
//            class API.  The extension class makes using the DCS API much 
//            simpler, but also introduces implementation limitations that
//            require the DCS server to work in a single-threaded/serial 
//            processing mode.  This type of server may not handle multiple 
//            connections very well.  A concurrent/multi-threaded server can 
//            be implemented from the raw DCS API for handling multiple 
//            simultaneous connections.  

#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <sys/timeb.h>
#include "DCSServer.h"
#include "Constants/ConstantsUnits.h"

#ifdef WIN32
#include <conio.h>
#endif


#define MAXPLAT 10   // Maximum number of platforms to create
#define PLATTIME 1   // New platform every 1 second
#define ECITRACKNUMBER ((MAXPLAT*3)+1)


// // // // // // // // // // // // // // // // // // // // // // //
// Convienent math defines

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#ifndef DEG2RAD
#define DEG2RAD 0.017453292519943295769236907684886
#endif


// // // // // // // // // // // // // // // // // // // // // // //
// Reference origin for scenario

#define REFLAT (22.0*DEG2RAD)
#define REFLON (-159.0*DEG2RAD)


// // // // // // // // // // // // // // // // // // // // // // //
// Arrays of colors, used for beams and gates
uint32_t bcolors[10] = {0xFF0000FF,0xFFFFFFFF,0x0000FFFF,0x000000FF,0xFF0000FF,
			0x00FF00FF,0x0000FFFF,0xFFFF00FF,0xFF00FFFF,0x00FFFFFF};
uint32_t gcolors[10] = {0x00FFFFFF,0xFF00FFFF,0xFFFF00FF,0x0000FFFF,0x00FF00FF,
			0xFF0000FF,0x000000FF,0xFF0000FF,0xFFFFFFFF,0x0000FFFF};


// // // // // // // // // // // // // // // // // // // // // // //
// flag to denote the server's status
int running(1);

bool deletedPlatform(false);
bool deletedBeam(false);
bool deletedGate(false);
double deletedTime(0);


// In order to properly stop the server, we set a signal that allows
// the user to stop the process by pressing Ctl C
void dcsserverInterrupt()
{
  signal(SIGINT,SIG_IGN);
  cout << "SimpleDCSServer interrupt called" << endl;
  if (running)
  {
    running = false;
  }
  else
  {
    exit(0);
  }
}


// fixes angles, in radians, between 0 and two PI
inline double ANGFIX(double x)
{
  return ((x = fmod(x,(M_PI*2.0)))<0.0)?(x+(M_PI*2.0)):x;
}


// // // // // // // // // // // // // // // // // // // // // // //
// Time in seconds since the beginning of the year
double yeartime()
{
  struct timeval tp;

  // get the current system time, using timezone value of 0
  // returns UTC time
  gettimeofday(&tp, 0);

  // put system time into a tm struct
  time_t t(tp.tv_sec);
  struct tm* p_time = gmtime(&t);

  // assemble a UTC "system time"
  unsigned int p_secs = (unsigned int)(p_time->tm_sec )  +
    (((unsigned int)(p_time->tm_min )) * 60) +
    (((unsigned int)(p_time->tm_hour)) * 60*60) +
    (((unsigned int)(p_time->tm_yday)) * 60*60*24);
  
  return (p_secs + (tp.tv_usec * 1e-06));
}

int sendPlotData (DCSServer* dcsServer, uint64_t id, double dtime, double pctVal)
{
#define PLOTSIZE 100
  static const double STARTX = 500;
  static const double INCREMENTX = 20;
  // Create an array of data to send
  float dataArray[PLOTSIZE];
  int k;
  for (k = 0; k < PLOTSIZE; ++k)
  {
    double randNum = (rand() % 100) * 0.1; // Random number between 0 and 10
    dataArray[k] = (float)randNum;
    // Add a spike around a regular area
    double curPct = ((double)k) / PLOTSIZE;
    if ((curPct > pctVal - 0.1) && (curPct < pctVal + 0.1))
      dataArray[k] += 150;
  }
  // Set up a gate to focus on the "high" value
  DCSScopeGateData gate;
  gate.id_ = 1;
  gate.center_ = STARTX + pctVal * PLOTSIZE * INCREMENTX;
  gate.start_ = gate.center_ - 35;
  gate.end_ = gate.center_ + 45;
  // Send off the array
  dcsServer->sendScopeData (id,
			    UTILS::CU_METERS,	/* units, meters */
			    UTILS::CU_VOLT,	/* units, volts */
			    dtime,		/* time of plot */
			    STARTX,		/* start of X (500 meters out) */
			    INCREMENTX,		/* Increment for X (20 meters each point) */
			    0,			/* Y Bias (none, start at 0) */
			    1,			/* Y Scale (scale at 1x data) */
			    PLOTSIZE,		/* Number of data points in array */
			    UTILS::vtFLOAT32,	/* Data is stored as 4-byte floats */
			    1,			/* Number of gates */
			    (void*)dataArray,	/* Array of data to send */
			    &gate);		/* Gate to send */
  return 0;
}


// // // // // // // // // // // // // // // // // // // // // // //
// main program loop
int main(int argc, char** argv)
{
  // The current platform id.  It will be incremented each time we create
  // a new platform in order to assign a unique id to each data set.
  int32_t id = 0;
  int32_t i = 0;
  int8_t datamode = DCSDATA_MULTICAST;
  std::stringstream sstr;

  // Values for updating beam and gate states
  double lasttime = 0.0;
  bool beamon = true;
  int32_t coloroffset = 0;
  int32_t targetID = 1;

  // First check command line args
  if (argc > 1)
  {
    if (argc > 2 || strcmp(argv[1], "-h") == 0)
    {
      printf("Usage: %s [-h | -unicast | -broadcast | -multicast]\n", argv[0]);
      return 0;
    }
    else if (strcmp(argv[1], "-unicast") == 0)
      datamode = DCSDATA_UNICAST;
    else if (strcmp(argv[1], "-broadcast") == 0)
      datamode = DCSDATA_BROADCAST;
    else if (strcmp(argv[1], "-multicast") == 0)
      datamode = DCSDATA_MULTICAST;
    else
    {
      printf("Usage: %s [-h | -unicast | -broadcast | -multicast]\n", argv[0]);
      return 0;
    }
  }

  // In order to properly stop the server, we set a signal that
  // allows the user to stop the process by pressing Ctl C.
#ifdef WIN32
  signal(SIGINT,(void(*)(int32_t))dcsserverInterrupt);
  cout << "\n\t Press Q key to quit DCS Server" << endl;
  cout << "\n\t Press P key to delete platform 1" << endl;
  cout << "\n\t Press B key to delete beam 11" << endl;
  cout << "\n\t Press G key to delete gate 21" << endl;
  cout << "\n\t Press C key to clear the database file" << endl;
  cout << "\n" << endl;
#else
  cout << "\n\t Press Ctl-C key to quit DCS Server\n" << endl;
  struct sigaction act;
  act.sa_handler=(void(*)(int32_t))dcsserverInterrupt;
  sigemptyset(&act.sa_mask);
  act.sa_flags=0;
#ifdef SA_RESTART
  act.sa_flags|=SA_RESTART;
#endif
  sigaction(SIGINT,&act,NULL);
  
  // Ignore SIGPIPE which will cause an exit when
  // reading from a broken socket more than once.
  // This only affects UNIX machines, Sun & Linux in particular
  signal(SIGPIPE,SIG_IGN);
#endif

  // remove old DCS database files
  char sysCmd[128];
#ifdef WIN32
  strncpy(sysCmd,"del *_dcsSRV.dat", sizeof(sysCmd)-1);
#else
  strncpy(sysCmd,"rm -f *_dcsSRV.dat", sizeof(sysCmd)-1);
#endif
  int sysrv = system (sysCmd);

  // Get current time & year for scenario
  time_t t = time(NULL);
  struct tm* gmt = gmtime(&t);
  int currentYear = gmt->tm_year+1900;
  double dtime = yeartime();

  // DCSServer constructor, facilitates the creation, setup and sending of 
  // data via the NRL developed DCS protocol
  DCSServer dcsServer("SimpleDCSServer", datamode);

  // Responsible for starting the DCS server based on
  // values set in the constructor.
  dcsServer.start();
  
  // Sets expiration time limit (secs) for all objects,
  // value must be > 0
  dcsServer.setExpireTimeLimit(300.);
  
  // Sends scenario initialization information to the server
  // and subsequently to all connected clients.
  dcsServer.sendScenarioHeader(DCSCOORD_ENU,
			       DCSCLASS_UNCLASSIFIED,
			       currentYear,
			       REFLAT,
			       REFLON,
			       dtime,
			       1.0,
			       DCSTIME_PACKET);

  // main loop, data is created and sent via DCS until a Ctl C is
  // issued by the user
  double xpos, ypos, course;
  double azim, elev;
  int numPlats = 0;
  char genericDataValue[72];
  double toggleDuration = 4.0;
  bool platformFourCreated = false;
  elev = 15.0 * DEG2RAD;
  cout.precision(12);

  // Create an ECI platform that is updated with a stationary point, thus using earth's rotation to move
  double eciRefTime = yeartime();
  dcsServer.sendPlatformHeader(ECITRACKNUMBER,		/* platform id */
			       "ECI Platform",		/* Platform name */
			       "unit_sphere",		/* Icon name */
			       DCSCOORD_ECI,		/* ECI coordinate system */
			       REFLAT, REFLON, 0.0,	/* Reference LLA */
			       DCSVERTDATUM_WGS84, 0.0,	/* Vertical datum */
			       DCSMAGVAR_TRUE, 0.0,	/* Magnetic variance */
			       eciRefTime,		/* ECI Reference time */
			       0.0, 0.0, 0.0,		/* GTP offsets: ignored in this case since we're not using GTP */
			       eciRefTime,		/* Data time */
			       -5559677.61765208,	/* X */
			       -2023557.16494139,	/* Y */
			       2374412.89530282,	/* Z */
			       0,			/* course */
			       0,			/* pitch */
			       0.,			/* roll */
			       0., 0., 0.,		/* vx, vy, vz */
			       0., 0., 0.);		/* ax, ay, az */
  dcsServer.sendCategoryData (31, DCS::getCategoryName (DCSPLATFORM_AIRCRAFT), DCS::getCategoryValue (DCSPLATFORM_AIRCRAFT), dtime);
  dcsServer.sendCategoryData (31, DCS::getCategoryName (DCSALLEGIENCE_FRIENDLY), DCS::getCategoryValue (DCSALLEGIENCE_FRIENDLY), dtime);

  // Continuously loop until done
  while (running)
  {
    // Get our data here.  The data can come from any source such as
    // The network or a file.  In this example it is made up.
    dtime = yeartime();

    // Generate sample position data
    xpos = 100 * (sin(dtime * 0.01));
    ypos = 100 * (cos(dtime * 0.01));
 
    // Generate sample orientation data
    course = sin(ANGFIX((M_PI * .5) + (dtime * 0.1)));

    // Generate sample azimuth beam & gate pointing data
    azim = ANGFIX(dtime * DEG2RAD);

    // Update the ECI track.  It is a static point that uses the earth's rotation to rotate
    dcsServer.sendPlatformData(31,
			       dtime,
			       -5559677.61765208,	/* X */
			       -2023557.16494139,	/* Y */
			       2374412.89530282,        /* Z */
			       0,			/* course */
			       0, 			/* pitch */
			       0.,			/* roll */
			       0., 0., 0.,		/* vx, vy, vz */
			       0., 0., 0.);		/* ax, ay, az */

    // add a new data set every few seconds until the max number is defined
    time_t test = time(NULL)-t;
    if ((numPlats < (MAXPLAT*3)) && (test/PLATTIME) > (id-1))
    {
      // increment platform count
      numPlats += 3;
      // increment unique id
      ++id;

      // Sends platform initialization information to the server
      // and subsequently to all connected clients.
      char callsign[64];
      char iconname[32];
      sstr.str("");
      sstr << id << " p-3c_orion_nrl";
      strncpy(callsign, sstr.str().c_str(), sizeof(callsign)-1);
      strncpy(iconname, "p-3c_orion_nrl", sizeof(iconname)-1);
      dcsServer.sendPlatformHeader(id,         /* platform id */
				   callsign,
				   iconname,
				   (id == 1 || id == 10) ? DCSCOORD_GTP : DCSCOORD_ENU,
				   REFLAT, REFLON, 0.0,		/* Reference LLA */
				   DCSVERTDATUM_WGS84, 0.0,	/* Veritcal Datum */
				   DCSMAGVAR_TRUE, 0.0,		/* Magnetic variance */
				   0.0,				/* ECI Reference Time */
				   0.0, 950, 90 * DEG2RAD,	/* GTP X/Y/Angle offsets; ignored if not GTP */
				   dtime,			/* Data time */
				   xpos + ((id - 1) * 100),	/* X position in tangential system */
				   ypos + ((id - 1) * 100),	/* Y position in tangential system */
				   100.,        /* Z */
				   course,
                                   (id == 4) ? 20. * DEG2RAD : 0., /* pitch */
                                   (id == 4) ? 15. * DEG2RAD : 0., /* roll */
                                   (id == 4) ? 1. : 0.,  /* vx */
                                   (id == 4) ? 2. : 0.,  /* vy */
                                   (id == 4) ? 3. : 0.,  /* vz */
				   4., 5., 6.); /* ax, ay, az */
      dcsServer.sendCategoryData (id, DCS::getCategoryName (DCSPLATFORM_AIRCRAFT), DCS::getCategoryValue (DCSPLATFORM_AIRCRAFT), dtime);
      dcsServer.sendCategoryData (id, DCS::getCategoryName (DCSALLEGIENCE_FRIENDLY), DCS::getCategoryValue (DCSALLEGIENCE_FRIENDLY), dtime);

      DCSBeamType_t beamtype(DCSBEAM_LINEAR);
      if (id == 4)
      {
	// Set the beam on platform '4' to use a steady (track) beam type instead
	// of a linear beam type.  Set the beam's target id and beam type.
	beamtype = DCSBEAM_STEADY;
      }
      else if (id == 9)
      {
	// Set the beam on platform '9' to use a body axis relative beam type instead
	// of a linear beam type.  Set the beam's target id and beam type.
	beamtype = DCSBEAM_BODYAXIS;
      }
      
      // Sends beam initialization information to the server
      // and subsequently to all connected clients.
      char beamdesc[64];
      sstr.str("");
      sstr << id+MAXPLAT << " p-3c_orion_nrl beam";
      strncpy(beamdesc, sstr.str().c_str(), sizeof(beamdesc)-1);
      dcsServer.sendBeamHeader(id+MAXPLAT,  /* beam id */
			       id,          /* host id */
			       beamdesc,
			       3 * DEG2RAD, /* hbw */
			       3 * DEG2RAD, /* vbw */
			       targetID,
			       bcolors[id-1],
			       0.,  /* azim offset */
			       0.,  /* elev offset */
			       5.,  /* x body offset */
			       0.,  /* y body offset */
			       1.,  /* z body offset */
			       beamtype,
			       dtime,
			       azim,
			       elev,
			       (id == 4) ?   0. : 300.0); /* range */

      // Sends gate initialization information to the server
      // and subsequently to all connected clients.
      char gatedesc[64];
      sstr.str("");
      sstr << id+(MAXPLAT*2) << " p-3c_orion_nrl gate";
      strncpy(gatedesc, sstr.str().c_str(), sizeof(gatedesc)-1);
      dcsServer.sendGateHeader(id+(MAXPLAT*2),  /* gate id */
			       id+MAXPLAT,      /* host id */
			       gatedesc,
			       gcolors[id-1],
			       0.,  /* azim offset */
			       0.,  /* elev offset */
			       0.,  /* x body offset */
			       0.,  /* y body offset */
			       0.,  /* z body offset */
			       DCSGATE_RANGE,
			       dtime,
			       azim,
			       elev,
			       3.0 * DEG2RAD,      /* width, i.e. hbw */
			       3.0 * DEG2RAD,      /* height, i.e. vbw */
			       (id == 4) ? -10. : 280.0,  /* min range */
			       (id == 4) ?   0. : 290.0,  /* cen range */
			       (id == 4) ?  10. : 300.0); /* max range */
    } // end of header data


    if (deletedPlatform && deletedTime < dtime)
    {
      cout << "Re-adding Platform 1" << endl;
      int32_t deletedID = 1;
      char callsign[64];
      char iconname[32];
      sstr.str("");
      sstr << deletedID << " p-3c_orion_nrl";
      strncpy(callsign, sstr.str().c_str(), sizeof(callsign)-1);
      strncpy(iconname, "p-3c_orion_nrl", sizeof(iconname)-1);
      dcsServer.sendPlatformHeader(deletedID,         /* platform id */
				   callsign,
				   iconname,
				   DCSCOORD_GTP,
				   REFLAT, REFLON, 0.0,		/* Reference LLA */
				   DCSVERTDATUM_WGS84, 0.0,	/* Veritcal Datum */
				   DCSMAGVAR_TRUE, 0.0,		/* Magnetic variance */
				   0.0,				/* ECI Reference Time */
				   0.0, 950, 90 * DEG2RAD,	/* GTP X/Y/Angle offsets; ignored if not GTP */
				   dtime,			/* Data time */
				   xpos + ((deletedID - 1) * 100),	/* X position in tangential system */
				   ypos + ((deletedID - 1) * 100),	/* Y position in tangential system */
				   100.,        /* Z */
				   course,
				   0.,          /* pitch */
				   0.,          /* roll */
				   0., 0., 0.,  /* vx, vy, vz */
				   0., 0., 0.); /* ax, ay, az */
      dcsServer.sendCategoryData (deletedID, DCS::getCategoryName (DCSPLATFORM_AIRCRAFT), DCS::getCategoryValue (DCSPLATFORM_AIRCRAFT), dtime);
      dcsServer.sendCategoryData (deletedID, DCS::getCategoryName (DCSALLEGIENCE_FRIENDLY), DCS::getCategoryValue (DCSALLEGIENCE_FRIENDLY), dtime);

      DCSBeamType_t beamtype(DCSBEAM_LINEAR);      
      // Sends beam initialization information to the server
      // and subsequently to all connected clients.
      char beamdesc[64];
      sstr.str("");
      sstr << deletedID+MAXPLAT << " p-3c_orion_nrl beam";
      strncpy(beamdesc, sstr.str().c_str(), sizeof(beamdesc)-1);
      dcsServer.sendBeamHeader(deletedID+MAXPLAT,  /* beam id */
			       deletedID,          /* host id */
			       beamdesc,
			       3 * DEG2RAD, /* hbw */
			       3 * DEG2RAD, /* vbw */
			       targetID,
			       bcolors[deletedID-1],
			       0.,  /* azim offset */
			       0.,  /* elev offset */
			       5.,  /* x body offset */
			       0.,  /* y body offset */
			       1.,  /* z body offset */
			       beamtype,
			       dtime,
			       azim,
			       elev,
			       300.0); /* range */

      // Sends gate initialization information to the server
      // and subsequently to all connected clients.
      char gatedesc[64];
      sstr.str("");
      sstr << deletedID+(MAXPLAT*2) << " p-3c_orion_nrl gate";
      strncpy(gatedesc, sstr.str().c_str(), sizeof(gatedesc)-1);
      dcsServer.sendGateHeader(deletedID+(MAXPLAT*2),  /* gate id */
			       deletedID+MAXPLAT,      /* host id */
			       gatedesc,
			       gcolors[deletedID-1],
			       0.,  /* azim offset */
			       0.,  /* elev offset */
			       0.,  /* x body offset */
			       0.,  /* y body offset */
			       0.,  /* z body offset */
			       DCSGATE_RANGE,
			       dtime,
			       azim,
			       elev,
			       3.0 * DEG2RAD,      /* width, i.e. hbw */
			       3.0 * DEG2RAD,      /* height, i.e. vbw */
			       280.0,  /* min range */
			       290.0,  /* cen range */
			       300.0); /* max range */
      deletedPlatform = false;
      deletedTime = 0;
    } // end of re-adding deleted header data

    
    if (deletedBeam && deletedTime < dtime)
    {
      cout << "Re-adding Beam 11" << endl;
      int32_t deletedID = 1;

      DCSBeamType_t beamtype(DCSBEAM_LINEAR);      
      // Sends beam initialization information to the server
      // and subsequently to all connected clients.
      char beamdesc[64];
      sstr.str("");
      sstr << deletedID+MAXPLAT << " p-3c_orion_nrl beam";
      strncpy(beamdesc, sstr.str().c_str(), sizeof(beamdesc)-1);
      dcsServer.sendBeamHeader(deletedID+MAXPLAT,  /* beam id */
			       deletedID,          /* host id */
			       beamdesc,
			       3 * DEG2RAD, /* hbw */
			       3 * DEG2RAD, /* vbw */
			       targetID,
			       bcolors[deletedID-1],
			       0.,  /* azim offset */
			       0.,  /* elev offset */
			       5.,  /* x body offset */
			       0.,  /* y body offset */
			       1.,  /* z body offset */
			       beamtype,
			       dtime,
			       azim,
			       elev,
			       300.0); /* range */

      // Sends gate initialization information to the server
      // and subsequently to all connected clients.
      char gatedesc[64];
      sstr.str("");
      sstr << deletedID+(MAXPLAT*2) << " p-3c_orion_nrl gate";
      strncpy(gatedesc, sstr.str().c_str(), sizeof(gatedesc)-1);
      dcsServer.sendGateHeader(deletedID+(MAXPLAT*2),  /* gate id */
			       deletedID+MAXPLAT,      /* host id */
			       gatedesc,
			       gcolors[deletedID-1],
			       0.,  /* azim offset */
			       0.,  /* elev offset */
			       0.,  /* x body offset */
			       0.,  /* y body offset */
			       0.,  /* z body offset */
			       DCSGATE_RANGE,
			       dtime,
			       azim,
			       elev,
			       3.0 * DEG2RAD,      /* width, i.e. hbw */
			       3.0 * DEG2RAD,      /* height, i.e. vbw */
			       280.0,  /* min range */
			       290.0,  /* cen range */
			       300.0); /* max range */
      deletedBeam = false;
      deletedTime = 0;
    } // end of re-adding deleted header data

    
    if (deletedGate && deletedTime < dtime)
    {
      cout << "Re-adding Gate 21" << endl;
      int32_t deletedID = 1;

      // Sends gate initialization information to the server
      // and subsequently to all connected clients.
      char gatedesc[64];
      sstr.str("");
      sstr << deletedID+(MAXPLAT*2) << " p-3c_orion_nrl gate";
      strncpy(gatedesc, sstr.str().c_str(), sizeof(gatedesc)-1);
      dcsServer.sendGateHeader(deletedID+(MAXPLAT*2),  /* gate id */
			       deletedID+MAXPLAT,      /* host id */
			       gatedesc,
			       gcolors[deletedID-1],
			       0.,  /* azim offset */
			       0.,  /* elev offset */
			       0.,  /* x body offset */
			       0.,  /* y body offset */
			       0.,  /* z body offset */
			       DCSGATE_RANGE,
			       dtime,
			       azim,
			       elev,
			       3.0 * DEG2RAD,      /* width, i.e. hbw */
			       3.0 * DEG2RAD,      /* height, i.e. vbw */
			       280.0,  /* min range */
			       290.0,  /* cen range */
			       300.0); /* max range */
      deletedGate = false;
      deletedTime = 0;
    } // end of re-adding deleted header data

    
    // Update all data positions
    int32_t startID = deletedPlatform ? 2 : 1;
    for (i = startID; i <= id; i++)
    {
      // Updates platform position information to the server
      // and subsequently to all connected clients.
      dcsServer.sendPlatformData(i,
				 dtime,
				 xpos + ((i - 1) * 100),
				 ypos + ((i - 1) * 100),
				 100.,        /* Z */
				 course,
				 (i == 4) ? 20.*DEG2RAD : 0., /* pitch */
				 (i == 4) ? 15.*DEG2RAD : 0., /* roll */
                                 (i == 4) ? 1. : 0.,  /* vx */
                                 (i == 4) ? 2. : 0.,  /* vy */
                                 (i == 4) ? 3. : 0.,  /* vz */
				 4., 5., 6.); /* ax, ay, az */
      
      if (i == 4)
	platformFourCreated = true;

      // Do not update if we know beams are not displayed
      if (beamon)
      {
	if (deletedBeam && i == 1)
	  continue;
	else
	{
	  // Updates beam position information to the server
	  // and subsequently to all connected clients.
	  dcsServer.sendBeamData(i+MAXPLAT,
				 dtime,
				 azim,
				 elev,
				 (i == 4) ?   0. : 300.0); /* range */

	  if (deletedGate && i == 1)
	    continue;
	  else
	  {
	    // Updates gate position information to the server
	    // and subsequently to all connected clients.
	    dcsServer.sendGateData(i+(MAXPLAT*2),
				   dtime,
				   azim,
				   elev,
				   3.0 * DEG2RAD,     /* width, i.e. hbw */
				   3.0 * DEG2RAD,     /* height, i.e. vbw */
				   (i == 4) ? -10. : 280.0,  /* min range */
				   (i == 4) ?   0. : 290.0,  /* cen range */
				   (i == 4) ?  10. : 300.0); /* max range */
	  }
	}
      } // end of if (beamon)

      // Now update beam and gate states, and change the colors
      if (dtime-lasttime > toggleDuration)
      {
	// Sends on|off state events for all objects to the server
	// and subsequently to all connected clients.
	// send beam state change event
	dcsServer.sendStateEvent(i+MAXPLAT, beamon, dtime);

	// Sends on|off state events for all objects to the server
	// and subsequently to all connected clients.
	// send gate state event
	dcsServer.sendStateEvent(i+(MAXPLAT*2), beamon, dtime);

	// Do not update if we know beams are not displayed
	if (beamon)
	{	  
	  if (deletedBeam && i == 1)
	    continue;
	  else
	  {
	    // Sends color change events for all objects to the server
	    // and subsequently to all connected clients
	    // send beam color event
	    dcsServer.sendColorEvent(i+MAXPLAT, bcolors[(i+coloroffset)%10], dtime);

	    if (deletedBeam && i == 1)
	      continue;
	    else
	    {
	      // Sends color change events for all objects to the server
	      // and subsequently to all connected clients
	      // send gate color event
	      dcsServer.sendColorEvent(i+(MAXPLAT*2), gcolors[(i+coloroffset)%10], dtime);
	    }

	    // send beam target event.
	    // As an example, cycle through all available 'targets'
	    if (i == 4)
	    {
	      ++targetID;
	      // prevent platform 4 from tracking itself
	      if (targetID == 4)
	      {
		targetID = 5;
	      }
	      else if (targetID > 10)
	      {
		targetID = 1;
	      }
	      // Sends target events for beams to the server
	      // and subsequently to all connected clients.
	      dcsServer.sendTargetEvent(i+MAXPLAT, targetID, dtime);
	    }	  
	  }
	} // end of if (beamon)
      }
    } // end of for (i to id)

    // Events for next time
    if (dtime-lasttime > toggleDuration)
    {
      // information text generic data
      memset(genericDataValue, 0, sizeof(genericDataValue));
      strncpy(genericDataValue, beamon?"Beams on":"Beams off", sizeof(genericDataValue)-1);
      dcsServer.sendGenericData(0, "Simdis_InfoText", genericDataValue, dtime, -1);

      // verify platform four exists
      if (platformFourCreated == true)
      {
	// animated line generic data
	memset(genericDataValue, 0, sizeof(genericDataValue));
	strncpy(genericDataValue, beamon?"entityid=3":"entityid=5", sizeof(genericDataValue)-1);
	dcsServer.sendGenericData(4, "SIMDIS_AnimatedLine 1", genericDataValue, dtime, -1);

	if (beamon)
	{
	  // cylinder generic data
	  memset(genericDataValue, 0, sizeof(genericDataValue));
	  strncpy(genericDataValue, "x=-0.5,y=9,z=0,px=-1,py=0,pz=0,l=0,radiusNear=.5,radiusFar=0", sizeof(genericDataValue)-1);
	  dcsServer.sendGenericData(4, "SIMDIS_Cylinder 1", genericDataValue, dtime, -1);
	  memset(genericDataValue, 0, sizeof(genericDataValue));
	  strncpy(genericDataValue, "x=-0.5,y=9,z=0,px=-1,py=0,pz=0,l=4,radiusNear=.5,radiusFar=0", sizeof(genericDataValue)-1);
	  dcsServer.sendGenericData(4, "SIMDIS_Cylinder 1", genericDataValue, dtime + (toggleDuration * 0.5), -1);
	  memset(genericDataValue, 0, sizeof(genericDataValue));
	  strncpy(genericDataValue, "x=-0.5,y=9,z=0,px=-1,py=0,pz=0,l=0,radiusNear=.5,radiusFar=0", sizeof(genericDataValue)-1);
	  dcsServer.sendGenericData(4, "SIMDIS_Cylinder 1", genericDataValue, dtime + toggleDuration, -1);

	  // scale generic data
	  memset(genericDataValue, 0, sizeof(genericDataValue));
	  strncpy(genericDataValue, "1.0`1.0`1.0", sizeof(genericDataValue)-1);
	  dcsServer.sendGenericData(targetID, "SIMDIS_ScaleXYZ", genericDataValue, dtime, -1);
	  memset(genericDataValue, 0, sizeof(genericDataValue));
	  strncpy(genericDataValue, "1.0`4.0`1.0", sizeof(genericDataValue)-1);
	  dcsServer.sendGenericData(targetID, "SIMDIS_ScaleXYZ", genericDataValue, dtime + (toggleDuration * 0.5), -1);
	  memset(genericDataValue, 0, sizeof(genericDataValue));
	  strncpy(genericDataValue, "1.0`1.0`1.0", sizeof(genericDataValue)-1);
	  dcsServer.sendGenericData(targetID, "SIMDIS_ScaleXYZ", genericDataValue, dtime + toggleDuration, -1);
	}

	// change icon
	dcsServer.sendIconNameEvent(4, (beamon?"p-3c_orion_nrl":"lear-36"), dtime);

	// change callsign
	dcsServer.sendCallsignEvent(4, (beamon?"Plane":"Jet"), dtime);
	dcsServer.sendCallsignEvent(14, (beamon?"Plane Beam":"Jet Beam"), dtime);
	dcsServer.sendCallsignEvent(24, (beamon?"Plane Gate":"Jet Gate"), dtime);

	// change classification
	dcsServer.sendClassificationEvent((beamon?"GOTS":"SIMDIS"),(beamon?0xFF000060:0xFF00FF60), dtime);

	// change SIMDIS track color
	memset(genericDataValue, 0, sizeof(genericDataValue));
	strncpy(genericDataValue, beamon?"0xFF0000FF":"-1", sizeof(genericDataValue)-1);
	dcsServer.sendGenericData(4, "SIMDIS_TrackColor", genericDataValue, dtime, -1);
      }

      lasttime = dtime;
      beamon = !beamon;
      
      // Increment color once for every blink
      if (beamon)
      {
        if (++coloroffset >= 10)
	{
	  coloroffset = 0;
	}
      }
    }

    // Send a scope plot
    sendPlotData(&dcsServer, MAXPLAT * 3 + 10, dtime, (0.5 * (sin(dtime * 0.1))) + 0.5);

    // Updates the current scenario time to the server
    // and subsequently to all connected clients.
    dcsServer.sendTime(dtime);
    
    // Responsible for both servicing new client connection
    // requests and removing expired objects.
    dcsServer.idle(dtime);

    cout << "Server time: " << dtime << endl;

    // check for keyboard entry, more specifically the q key
#ifdef WIN32
    // NOTE:  This does not work in rxvt, use bash
    if (_kbhit())
    {
      char keyPressed = _getch();
      cout << "keyPressed: " << keyPressed << endl;
      if (keyPressed == 'q' || keyPressed == 'Q')
      {
	running=false;
      }
      if (keyPressed == 'c' || keyPressed == 'C')
      {
        cout << "Clearing and rebuilding the database" << endl;
        bool cleaned = dcsServer.cleanDatabaseFile();
        cout << "Operation successful: " << cleaned << endl;
      }
      else if (deletedPlatform == false && deletedBeam == false && deletedGate == false )
      {
	if (keyPressed == 'p' || keyPressed == 'P')
	{
	  cout << "Removing Platform 1" << endl;
	  dcsServer.sendExpireEvent(1, dtime);
	  dcsServer.removeHeader(1, dtime);
	  dcsServer.removeHeader(1+MAXPLAT, dtime);
	  dcsServer.removeHeader(1+(MAXPLAT*2), dtime);
	  deletedPlatform = true;
	  // wait 10 seconds before platform is revived.
	  deletedTime = dtime + 10;
	}
	else if (keyPressed == 'b' || keyPressed == 'B')
	{
	  cout << "Removing Beam 11" << endl;
	  dcsServer.sendExpireEvent(1+MAXPLAT, dtime);
	  dcsServer.removeHeader(1+MAXPLAT, dtime);
	  dcsServer.removeHeader(1+(MAXPLAT*2), dtime);
	  deletedBeam = true;
	  // wait 10 seconds before beam is revived.
	  deletedTime = dtime + 10;
	}
	else if (keyPressed == 'g' || keyPressed == 'G')
	{
	  cout << "Removing Gate 21" << endl;
	  dcsServer.sendExpireEvent(1+(MAXPLAT*2), dtime);
	  dcsServer.removeHeader(1+(MAXPLAT*2), dtime);
	  deletedGate = true;
	  // wait 10 seconds before gate is revived.
	  deletedTime = dtime + 10;
	}
      }
    }
#endif
    // provide ~2 Hz data rate
    Sleep(500);
    
  } // end of while (running)

  return 0;
}
