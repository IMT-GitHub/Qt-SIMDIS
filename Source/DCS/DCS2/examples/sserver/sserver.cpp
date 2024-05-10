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
// sserver - a simple server program implemented from the raw DCS API

#include <sstream>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <memory.h>
#include <signal.h>
#include <sys/timeb.h>
#include "DCS.h"

#ifdef WIN32
#include <conio.h>
#endif

// // // // // // // // // // // // // // // // // // // // // // //
// Character string specifying the network address of the network
// interface to use for data transmission. To use any network
// interface (INADDR_ANY), set to NULL.
static const char* IFACE = NULL;


// Default UDP data mode.  The UDP mode can be set with a command line
// argument, but also has a default value defined here as a macro to make
// the application easier to test from Microsoft Visual Studio, where
// specifying command line arguments is not as simple as typing an argument
// at the command line.  Alternatively, command line arguments can be
// specified for Microsoft Visual Studio from the "Debug" tab of the
// project settings dialog.
#define DEFAULTMODE DCSDATA_MULTICAST


#define DELAY 1000000 // Microseconds in between data updates
#define MAXPLAT 10   // Maximum number of platforms to create
#define PLATTIME 1   // New platform every 1 second


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


// In order to properly stop the server, we set a signal that allows
// the user to stop the process by pressing Ctl C
void serverInterrupt()
{
  signal(SIGINT,SIG_IGN);
  cout << "SimpleServer interrupt called" << endl;
  if (running)
  {
    running = false;
  }
  else
  {
    exit(0);
  }
}


// Our object for error reporting
DCSConsoleLogger logger;


// fixes angles, in radians, between 0 and two PI
inline double ANGFIX(double x)
{
  return ((x = fmod(x,(M_PI*2.0)))<0.0)?(x+(M_PI*2.0)):x;
}


// // // // // // // // // // // // // // // // // // // // // // //
bool acceptConnection(DCSCom::DCSServerInfo& server,
		      const DCSTimeHeader& time,
		      const DCSScenarioHeader& scenario,
		      const DCSHeaderContainer& headers)
{
  // In its simpleset form this function could merely be:
  // 'return DCSCom::serverNegotiate(DCSCom::serverAccept(server));'
  char addr[DCS_ADDRSTRLEN];

  // Accept the connection
  SockClient* client = DCSCom::serverAccept(server);

  if (client != NULL)
  {
    // Report connection
    std::stringstream sstr;
    sstr << "Accepted connection from " << client->getAddress(addr, sizeof(addr)) << " " << client->getPort() << "\n";
    logger.log(sstr.str().c_str());

    if (!DCSCom::serverNegotiate(server, client, time, scenario, headers))
    {
      logger.log("Client failed connection negotiation.\n");
      DCSCom::serverTerminateClient(server, client);
      logger.log("Client was removed.\n");
      return false;
    }

    return true;
  }

  logger.log("Connection accept failed.\n");
  return false;
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


// // // // // // // // // // // // // // // // // // // // // // //
// simple dead-reckoning algorithm to generate points in an ENU
// topographic coordinate frame
void makeENUPosition(int32_t id, double time, DCSPoint& enu, DCSPoint& ori, DCSPoint& vel)
{
  // Generate sample position data
  enu.x = ((100 * (sin(time * 0.01)) + ((id - 1) * 100)));
  enu.y = ((100 * (cos(time * 0.01)) + ((id - 1) * 100)));
  enu.z = 100;

  // Generate sample orientation data
  ori.x = sin(ANGFIX((M_PI * .5) + (time * 0.1)));
  ori.y = (id == 4) ? 20.*DEG2RAD : 0.0;
  ori.z = (id == 4) ? 15.*DEG2RAD : 0.0;

  // Generate sample velocity data
  vel.x = (id == 4) ? 1 : 0.0;
  vel.y = (id == 4) ? 2 : 0.0;
  vel.z = (id == 4) ? 3 : 0.0;
}


// // // // // // // // // // // // // // // // // // // // // // //
// Update DCS platform data
void updateData(int32_t id, double time, DCSPlatformData& data)
{
  data.id_ = id;
  makeENUPosition(id, time, data.position_, data.orientation_, data.velocity_);
  data.accel_.set(4.0, 5.0, 6.0);
  data.time_ = time;
}


// // // // // // // // // // // // // // // // // // // // // // //
// Update DCS beam data
void updateData(int32_t id, double time, DCSBeamData& data)
{
  data.id_ = id;
  data.time_ = time;
  data.azim_ = ANGFIX(time * DEG2RAD);
  data.elev_ = 15.0 * DEG2RAD;
  data.length_ = 300.0;
}


// // // // // // // // // // // // // // // // // // // // // // //
// Update DCS gate data
void updateData(int32_t id, double time, DCSGateData& data)
{
  data.id_ = id;
  data.time_ = time;
  data.azim_ = ANGFIX(time * DEG2RAD);
  data.elev_ = 15.0 * DEG2RAD;
  data.width_ = 3.0 * DEG2RAD;
  data.height_ = 3.0 * DEG2RAD;
  data.minrange_ =  (id == 24) ? -10. : 280.0;
  data.maxrange_ =  (id == 24) ?  10. : 300.0;
  data.centroid_ =  (id == 24) ?   0. : 290.0;
}


// // // // // // // // // // // // // // // // // // // // // // //
// Creates the DCS headers for platforms
void makeDataSet(int32_t id, double time, DCSPlatformData& data, DCSPlatformHeader& header)
{
  updateData(id, time, data);

  header.id_ = id;
  strncpy(header.iconname_, "p-3c_orion_nrl", sizeof(header.iconname_)-1);
  std::stringstream sstr;
  sstr << id << " p-3c_orion_nrl";
  strncpy(header.callsign_, sstr.str().c_str(), sizeof(header.callsign_)-1);
  header.dims_.set(0.0, 0.0, 0.0);
  if (id == 6)
  {
    header.orientationoffset_.set(0.0, 30.0, 0.0);
    header.offset_.set(10.0, 0.0, 10.0);
  }
  else
  {
    header.offset_.set(0.0, 0.0, 0.0);
    header.orientationoffset_.set(0.0, 0.0, 0.0);
  }
  header.referenceframe_.latorigin_ = REFLAT;
  header.referenceframe_.lonorigin_ = REFLON;
  header.referenceframe_.coordsystem_ = DCSCOORD_ENU;
  header.color_ = 0;
  header.state_ = DCSSTATE_ON;
  header.status_ = DCSSTATUS_LIVE;
  memcpy(&header.platdata_, &data, sizeof(header.platdata_));
}


// // // // // // // // // // // // // // // // // // // // // // //
// Creates the DCS headers for beams
void makeDataSet(int32_t id, int32_t hostid, double time, DCSBeamData& data, DCSBeamHeader& header)
{
  updateData(id, time, data);

  header.id_ = id;
  std::stringstream sstr;
  sstr << id << " p-3c_orion_nrl beam";
  strncpy(header.callsign_, sstr.str().c_str(), sizeof(header.callsign_)-1);
  header.missileoffset_ = true;
  header.interpolate_ = true;
  header.azimoffset_ = 0.0;
  header.elevoffset_ = 0.0;
  header.hbw_ = 3 * DEG2RAD;
  header.vbw_ = 3 * DEG2RAD;
  header.hostid_ = hostid;
  header.color_ = 0;
  header.state_ = DCSSTATE_ON;
  switch (id)
  {
    case 14:
      header.beamtype_ = DCSBEAM_STEADY;
      break;
    case 19:
      header.beamtype_ = DCSBEAM_BODYAXIS;
      break;
    default:
      header.beamtype_ = DCSBEAM_LINEAR;
      break;
  }
  memcpy(&header.beamdata_, &data, sizeof(header.beamdata_));
}


// // // // // // // // // // // // // // // // // // // // // // //
// Creates the DCS headers for gates
void makeDataSet(int32_t id, int32_t hostid, double time, DCSGateData& data, DCSGateHeader& header)
{
  updateData(id, time, data);

  header.id_ = id;
  std::stringstream sstr;
  sstr << id << " p-3c_orion_nrl gate";
  strncpy(header.callsign_, sstr.str().c_str(), sizeof(header.callsign_)-1);
  header.offset_.set(0.0, 0.0, 0.0);
  header.azimoffset_ = 0.0;
  header.elevoffset_ = 0.0;
  header.hostid_ = hostid;
  header.color_ = 0;
  header.state_ = DCSSTATE_ON;
  switch (id)
  {
    case 24:
      header.gatetype_ = DCSGATE_TARGET;
      break;
    case 29:
      header.gatetype_ = DCSGATE_BODY;
      break;
    default:
      header.gatetype_ = DCSGATE_RANGE;
      break;
  }
  memcpy(&header.gatedata_, &data, sizeof(header.gatedata_));
}


// // // // // // // // // // // // // // // // // // // // // // //
// main program loop
int main(int argc, char** argv)
{
  // The current platform id.  It will be incremented each time we create
  // a new platform in order to assign a unique id to each data set.
  int32_t id = 0;
  int32_t i = 0;
  int8_t datamode = DEFAULTMODE;
  std::stringstream sstr;
  char addr[DCS_ADDRSTRLEN];
  bool present=false, success=false;
  char genericDataValue[256];
  double toggleDuration = 4.0;
  bool platformFourCreated = false;
  int drawModeCount = 0;

  // Values for updating beam and gate states
  double lasttime = 0.0;
  bool beamon = true;
  int32_t coloroffset = 0;
  int32_t targetID = 1;

  // Get current time & year for scenario
  time_t t = time(NULL);
  struct tm* gmt = gmtime(&t);
  int currentYear = gmt->tm_year+1900;
  double dtime = yeartime();

  // remove old DCS database files
  char sysCmd[128];
#ifdef WIN32
  strncpy(sysCmd,"del dcsdb.dat", sizeof(sysCmd)-1);
#else
  strncpy(sysCmd,"rm -f dcsdb.dat", sizeof(sysCmd)-1);
#endif
  int sysrv = system (sysCmd);
  

  // Provides functionality for discovering new clients looking for a server
  DCSDisc::DCSDiscover discover;
  // Stores information relevant to the actual DCS server
  DCSCom::DCSServerInfo server;
  // Provides functionality for inserting, updating, retrieving, and removing header objects
  DCSHeaderContainer headers;
  // Provides updates for scenario wind speed and wind direction parameters.
  DCSScenarioData scendata(0.0, 0.0);
  // Initialization parameters for scenario.  Specifies classification, reference coordinate, etc.
  DCSScenarioHeader scenheader(REFLAT, REFLON, currentYear, DCSCOORD_ENU, DCSCLASS_UNCLASSIFIED, scendata);
  // Provides updates for time, if the server is acting as the time source.
  DCSTimeData timedata(dtime);
  // Initialization parameters for time.  Specifies time source (e.g. DCS server or client workstation's system clock)
  DCSTimeHeader timeheader(1.0, DCSTIME_PACKET, timedata);

  // Initialization headers for all objects
  // Provides platform updates for position, orientation, velocity, and acceleration.
  DCSPlatformData platdata;
  // DCS container for a platform object.  Specifies call sign, 3D model to display, initial position, etc.
  DCSPlatformHeader platheader;
  // DCS container for a beam object.  Specifies associated platform, target, field of view, etc.
  DCSBeamHeader beamheader;
  // DCS container for a gate object.  Specifies associated beam, width, length, etc.
  DCSGateHeader gateheader;
  // Modifies the state of a platform, beam, or gate object.  Changes color, beam target, etc.
  DCSEvent event;

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

  // In order to properly stop the server, we set a signal handler that
  // allows the user to stop the process by pressing Ctl C.
#ifdef WIN32
  // PC windows only
  signal(SIGINT, (void(*)(int32_t))serverInterrupt);
  cout << "\n\t   Press Q key to quit DCS Server\n" << endl;
#else
  // all others
  cout << "\n\t Press Ctl-C key to quit DCS Server\n" << endl;
  struct sigaction act;
  act.sa_handler = (void(*)(int32_t))serverInterrupt;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
#ifdef SA_RESTART
  act.sa_flags |= SA_RESTART;
#endif
  sigaction(SIGINT, &act, NULL);
  
  // Ignore SIGPIPE which will cause an exit when
  // reading from a broken socket more than once.
  // This only affects UNIX machines, Sun & Linux in particular
  signal(SIGPIPE, SIG_IGN);
#endif

  // reload DCS database file (if file exists)
  if (headers.restore("dcsdb.dat"))
  {
    id = static_cast<int32_t>(headers.size()/3);
    sstr.str("");
    sstr << "Loaded " << headers.size() << "platforms from file\n";
    logger.log(sstr.str().c_str());
  }
  else
  {
    logger.log("Failed to restore file.  Starting a new one.\n");
    headers.initialize("dcsdb.dat");
  }

  // Delete 'dcsdb.dat' on successful exit.
  headers.setSave(false);

  // Socket initialization status, For the PC, this function calls the
  // WSAStartup function, which must be the first Windows Sockets function
  // called by an application or DLL. It allows an application or DLL
  // to specify the version of Windows Sockets required and to retrieve
  // details of the specific Windows Sockets implementation. The application
  // or DLL can only issue further Windows Sockets functions after
  // successfully calling WSAStartup.
  if (!initializeSocket())
  {
    char errormsg[256];
    getSockErrString(errormsg, 256);
    logger.log("Error initializing socket\n");
    logger.log("System error message: ");
    logger.log(errormsg);
    logger.log("\n");
    return 0;
  }

  // Initialize our dynamic client discovery socket
  // The discovery deamon needs to know the IP address and port that
  // this server can be reached through.  For this example we are using
  // port 2525.  The IP address that is specified for this server can be set
  // by picking an outgoing interface.  It only makes sense to report that our server
  // can be accessed through the only interface it is listening on.  For this example, if the interface is
  // NULL (no interface is specified so the kernel will listen on all available
  // network interfaces) we will use the address of a network interface obtained through gethostbyname.
  // In most cases you would probably specify the host address directly, rather
  // than querying for it via gethostbyname.
  if (IFACE == NULL)
  {
    char hostname[512];

    strncpy(addr, "127.0.0.1", sizeof(addr)-1);
    if (gethostname(hostname, sizeof(hostname))!= SOCKET_ERROR)
    {
      struct hostent *hptr = gethostbyname(hostname);
      if (hptr != NULL&&*hptr->h_addr_list != NULL)
      {
	struct in_addr in;
	memcpy(&in.s_addr, *hptr->h_addr_list, 4);
	strncpy(addr, inet_ntoa(in), sizeof(addr)-1);
      }
    }
    printf("Will tell clients to connect to %s 2525\n", addr);
    if(!DCSDisc::discoverServInit(discover, 2525, addr, "Test server", 1, IFACE))
    {
      char errormsg[256];
      sstr.str("");
      sstr << "Error initializing multicast socket for client management: " << getSockErr() << "\n";
      logger.log(sstr.str().c_str());

      getSockErrString(errormsg, 256);
      logger.log("System error message: ");
      logger.log(errormsg);
      logger.log("\n");
      return 0;
    }
  }
  else
  {
    if(!DCSDisc::discoverServInit(discover, 2525, IFACE, "Test server", 1, IFACE))
    {
      char errormsg[256];
      sstr.str("");
      sstr << "Error initializing multicast socket for client management: " << getSockErr() << "\n";
      logger.log(sstr.str().c_str());

      getSockErrString(errormsg, 256);
      logger.log("System error message: ");
      logger.log(errormsg);
      logger.log("\n");
      return 0;
    }
  }

  // Initialize the DCS server - defaults to multicast
  if (datamode == DCSDATA_UNICAST)
  {
    success = DCSCom::serverStart(server, 2525, DCSDATA_UNICAST, 2020, NULL, IFACE);
    if (success)
      cout << "DCS Server Protocol: UNICAST" << endl;
  }
  else if (datamode == DCSDATA_BROADCAST)
  {
    success = DCSCom::serverStart(server, 2525, DCSDATA_BROADCAST, 2020, "255.255.255.255", IFACE);
    if (success)
      cout << "DCS Server Protocol: BROADCAST" << endl;
  }
  else
  {
    success = DCSCom::serverStart(server, 2525, DCSDATA_MULTICAST, 2020, "227.0.20.20", IFACE);
    if (success)
      cout << "DCS Server Protocol: MULTICAST" << endl;
  }

  // if not successful, then clean up allocated resources
  if (!success)
  {
    char errormsg[256];
    sstr.str("");
    sstr << "Error starting server: " << server.servsock->getLastError() << "\n";
    logger.log(sstr.str().c_str());

    getSockErrString(errormsg, 256);
    logger.log("System error message: ");
    logger.log(errormsg);
    logger.log("\n");

    DCSDisc::discoverFree(discover);
    finalizeSocket();
    return 0;
  }

#ifdef WIN32
  logger.log("Starting server.  Use Q to kill.\n");
#else
  logger.log("Starting server.  Use Ctl C to kill.\n");
#endif

  // main loop, data is created and sent via DCS until a Ctl C is
  // issued by the user
  cout.precision(12);
  while (running)
  {
    // See if any clients are looking for a server.
    // If they are, announce our IP address and port
    if (discover.request->readReady())
    {
      DCSDisc::discoverRespond(discover);
    }

    // Will wait at least 100 milliseconds for new connection test
    // This should give us a default 10Hz data update rate
    if (server.servsock->readReady(0, DELAY))
    {
      acceptConnection(server, timeheader, scenheader, headers);
    }

    // Get our data here.  The data can come from any source such as
    // The network or a file.  In this example it is made up.
    dtime = yeartime();
    timedata.time_ = dtime;
    timeheader.timedata_.time_ = dtime;

    // Send time
    DCSCom::sendTimeData(server, &timedata);

    // add a new data set every few seconds until the max number is defined
    time_t test = time(NULL)-t;
    if ((headers.size() < (MAXPLAT*3)) && (test/PLATTIME) > (id-1))
    {
      makeDataSet(++id, dtime, platdata, platheader);
      makeDataSet(id+MAXPLAT, id, dtime, beamheader.beamdata_, beamheader);
      makeDataSet(id+(MAXPLAT*2), id+MAXPLAT, dtime, gateheader.gatedata_, gateheader);

      if (id == 4)
      {
	// Set the beam's target id for the steady (target) beam type.
        beamheader.targetid_ = targetID;
      }
      else
      {
	// Otherwise, for non-steady type beams, set the targetid to the no-target state
	beamheader.targetid_ = -1;
      }

      // Update our database and clients
      // We don't really need to test 'present' but this illustrates how it's to be used.
      if (headers.update(&platheader, present))
      {
        sstr.str("");
        sstr << "Added platform " << DCS::uint64totext(platheader.id_) << "\n";
      }
      else
      {
        sstr.str("");
        sstr << "Failed to add platform " << DCS::uint64totext(platheader.id_) << "\n";
      }
      logger.log(sstr.str().c_str());
      
      if (!present)
      {
	DCSCom::sendPlatformHeader(server, &platheader);
	DCSCom::sendCategoryData (server, platheader.id_, platheader.platdata_.time_, DCS::getCategoryName (DCSALLEGIENCE_FRIENDLY), DCS::getCategoryValue (DCSALLEGIENCE_FRIENDLY));
	DCSCom::sendCategoryData (server, platheader.id_, platheader.platdata_.time_, DCS::getCategoryName (DCSPLATFORM_AIRCRAFT), DCS::getCategoryValue (DCSPLATFORM_AIRCRAFT));
      }

      if (headers.update(&beamheader, present))
      {
        sstr.str("");
        sstr << "Added beam " << DCS::uint64totext(beamheader.id_) << "\n";
      }
      else
      {
        sstr.str("");
        sstr << "Failed to add beam " << DCS::uint64totext(beamheader.id_) << "\n";
      }
      logger.log(sstr.str().c_str());
      
      if (!present)
      {
	DCSCom::sendBeamHeader(server, &beamheader);
      }

      if (headers.update(&gateheader, present))
      {
        sstr.str("");
        sstr << "Added gate " << DCS::uint64totext(gateheader.id_) << "\n";
      }
      else
      {
        sstr.str("");
        sstr << "Failed to add gate " << DCS::uint64totext(gateheader.id_) << "\n";
      }
      logger.log(sstr.str().c_str());
      
      if (!present)
      {
	DCSCom::sendGateHeader(server, &gateheader);
      }
    }

    // Update our data positions
    for (i = 1; i <= id; i++)
    {
      makeDataSet(i, dtime, platdata, platheader);
      makeDataSet(i+MAXPLAT, i, dtime, beamheader.beamdata_, beamheader);
      makeDataSet(i+(MAXPLAT*2), i+MAXPLAT, dtime, gateheader.gatedata_, gateheader);

      if (i == 4)
      {
	// Set the beam's target id for the steady (target) beam type.
        beamheader.targetid_ = targetID;
	platformFourCreated = true;
      }
      else
      {
	// Otherwise, for non-steady type beams, set the targetid to the no-target state
        beamheader.targetid_ = -1;
      }

      // Send data to clients
      DCSCom::sendPlatformData(server, &platdata);
      DCSCom::sendBeamData(server, &beamheader.beamdata_);
      DCSCom::sendGateData(server, &gateheader.gatedata_);

      // Send generic data to clients
      memset(genericDataValue, 0, sizeof(genericDataValue));
      strncpy(genericDataValue, "Value", sizeof(genericDataValue)-1);
      DCSCom::sendGenericData(server, platheader.id_, dtime, -1, "Tag", genericDataValue);
      DCSCom::sendGenericData(server, beamheader.id_, dtime, -1, "Tag", genericDataValue);
      DCSCom::sendGenericData(server, gateheader.id_, dtime, -1, "Tag", genericDataValue);

      // Send category data to clients
      memset(genericDataValue, 0, sizeof(genericDataValue));
      strncpy(genericDataValue, "Value", sizeof(genericDataValue)-1);
      DCSCategoryData catData(platheader.id_, dtime, "Name", genericDataValue);
      DCSCom::sendTCPData(server, DCSREQUEST_HEADER, &catData);
      catData.id_ = beamheader.id_;
      DCSCom::sendTCPData(server, DCSREQUEST_HEADER, &catData);
      catData.id_ = gateheader.id_;
      DCSCom::sendTCPData(server, DCSREQUEST_HEADER, &catData);

      // Now update beam and gate states, and change the colors
      if (dtime-lasttime > toggleDuration)
      {
	event.time_ = dtime;

	// We'll need to update this change in the database.
	beamheader.state_ = (beamon) ? DCSSTATE_ON : DCSSTATE_OFF;
	gateheader.state_ = (beamon) ? DCSSTATE_ON : DCSSTATE_OFF;

	beamheader.color_ = bcolors[(i+coloroffset)%10];
	gateheader.color_ = gcolors[(i+coloroffset)%10];

	// Now send the new event.
	event.data_ = (beamon) ? DCSSTATE_ON : DCSSTATE_OFF;
	event.eventtype_ = DCSEVENT_STATE;

	event.id_ = beamheader.id_;
	DCSCom::sendEvent(server, &event);

	event.id_ = gateheader.id_;
	DCSCom::sendEvent(server, &event);

	// Do not update if we know beams are not displayed
	if (beamon)
	{
  	  // Color events
	  event.eventtype_ = DCSEVENT_COLOR;

	  event.id_ = beamheader.id_;
	  event.data_ = (int32_t)beamheader.color_;
	  DCSCom::sendEvent(server, &event);

	  event.id_ = gateheader.id_;
	  event.data_ = (int32_t)gateheader.color_;
	  DCSCom::sendEvent(server, &event);

	  // Send beam target event.  Since this is an event, the data transport is
	  // TCP, thus all clients will reliably receive the beam target event.
	  // In this example, we are 'targeting' the beam (i.e. radar) to a specific platform
	  // instead of sending range, azimuth and elevation, the platform's ID value is used.
	  // The range, azimuth and elevation values for the beam are automatically calculated
	  // within SIMDIS
	  // As an example, cycle through all available 'targets'
	  if (i == 4)
	  {
	    event.eventtype_ = DCSEVENT_TARGETID;
  	    event.id_ = beamheader.id_;
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
	    event.data_ = targetID;

	    DCSCom::sendEvent(server, &event);
	  } // if i == 4
	} // if beamon
      } // if dtime-lasttime > toggleDuration

      // Update the last known position in our database.
      // No need to test 'present' because we know they are there.
      memcpy(&platheader.platdata_, &platdata, sizeof(platdata));
      if (!headers.update(&platheader, present))
      {
        sstr.str("");
        sstr << "Failed to update platform " << DCS::uint64totext(platheader.id_) << "\n";
        logger.log(sstr.str().c_str());
      }

      // For these two we decided not to declare separate structures,
      // but to update the contents of the headers directly.
      if (!headers.update(&beamheader, present))
      {
        sstr.str("");
        sstr << "Failed to update beam " << DCS::uint64totext(beamheader.id_) << "\n";
        logger.log(sstr.str().c_str());
      }

      if (!headers.update(&gateheader, present))
      {
        sstr.str("");
        sstr << "Failed to update gate " << DCS::uint64totext(gateheader.id_) << "\n";
        logger.log(sstr.str().c_str());
      }
    }

    // Events for next time
    if (dtime-lasttime > toggleDuration)
    {
      // information text generic data
      memset(genericDataValue, 0, sizeof(genericDataValue));
      strncpy(genericDataValue, beamon?"Beams on":"Beams off", sizeof(genericDataValue)-1);
      DCSCom::sendGenericData(server, 0, dtime, -1, "Simdis_InfoText", genericDataValue);

      // verify platform four exists
      if (platformFourCreated == true)
      {
	// animated line generic data
	memset(genericDataValue, 0, sizeof(genericDataValue));
	strncpy(genericDataValue, beamon?"entityid=3":"entityid=5", sizeof(genericDataValue)-1);
	DCSCom::sendGenericData(server, 4, dtime, -1, "SIMDIS_AnimatedLine 1", genericDataValue);

	if (beamon)
	{
	  // cylinder generic data
	  memset(genericDataValue, 0, sizeof(genericDataValue));
	  strncpy(genericDataValue, "x=-0.5,y=9,z=0,px=-1,py=0,pz=0,l=0,radiusNear=.5,radiusFar=0", sizeof(genericDataValue)-1);
	  DCSCom::sendGenericData(server, 4, dtime, -1, "SIMDIS_Cylinder 1", genericDataValue);
	  memset(genericDataValue, 0, sizeof(genericDataValue));
	  strncpy(genericDataValue, "x=-0.5,y=9,z=0,px=-1,py=0,pz=0,l=4,radiusNear=.5,radiusFar=0", sizeof(genericDataValue)-1);
	  DCSCom::sendGenericData(server, 4, dtime + (toggleDuration * 0.5), -1, "SIMDIS_Cylinder 1", genericDataValue);
	  memset(genericDataValue, 0, sizeof(genericDataValue));
	  strncpy(genericDataValue, "x=-0.5,y=9,z=0,px=-1,py=0,pz=0,l=0,radiusNear=.5,radiusFar=0", sizeof(genericDataValue)-1);
	  DCSCom::sendGenericData(server, 4, dtime + toggleDuration, -1, "SIMDIS_Cylinder 1", genericDataValue);

	  // scale example using generic data sent via UDP
	  memset(genericDataValue, 0, sizeof(genericDataValue));
	  strncpy(genericDataValue, "1.0`1.0`1.0", sizeof(genericDataValue)-1);
	  DCSCom::sendGenericData(server, targetID, dtime, -1, "Simdis_ScaleXYZ", genericDataValue);
	  memset(genericDataValue, 0, sizeof(genericDataValue));
	  strncpy(genericDataValue, "1.0`4.0`1.0", sizeof(genericDataValue)-1);
	  DCSCom::sendGenericData(server, targetID, dtime + (toggleDuration * 0.5), -1, "SIMDIS_ScaleXYZ", genericDataValue);

	  // scale example using generic data sent via TCP
	  memset(genericDataValue, 0, sizeof(genericDataValue));
	  strncpy(genericDataValue, "1.0`1.0`1.0", sizeof(genericDataValue)-1);
          // Note:  Generic data tags are not case sensitive
	  DCSGenericData gendata(targetID, dtime + toggleDuration, -1, "simdis_scalexyz", genericDataValue);
	  DCSCom::sendTCPData(server, DCSREQUEST_HEADER, &gendata);
	  
	  // rocket burn example using generic data sent via TCP
	  memset(genericDataValue, 0, sizeof(genericDataValue));
          sstr.str("");
          sstr << "st=" << toggleDuration << ",c=0xFF042EF9,p=-.5`-9`0,d=-15`0`0,rn=.7,rf=2.2,sc=1";
          strncpy(genericDataValue, sstr.str().c_str(), sizeof(genericDataValue)-1);
	  DCSGenericData gendata3(4, dtime, -1, "SIMDIS_RocketBurn 1", genericDataValue);
	  DCSCom::sendTCPData(server, DCSREQUEST_HEADER, &gendata3);
	  memset(genericDataValue, 0, sizeof(genericDataValue));
          sstr.str("");
          sstr << "st=" << toggleDuration << ",c=0xFF7DFFFB,p=-.5`-9`0,d=-15`0`0,rn=.5,rf=2,sc=1";
          strncpy(genericDataValue, sstr.str().c_str(), sizeof(genericDataValue)-1);
	  DCSGenericData gendata4(4, dtime, -1, "SIMDIS_RocketBurn 2", genericDataValue);
	  DCSCom::sendTCPData(server, DCSREQUEST_HEADER, &gendata4);
          // The following command will also work, but data is sent via UDP
	  // DCSCom::sendGenericData(server, 4, dtime, -1, "SIMDIS_RocketBurn 2", genericDataValue);
	  
	  static int count = 0;
	  ++count;

	  // vapor trail example using generic data sent via TCP
	  if(count == 1)
	  {
	    memset(genericDataValue, 0, sizeof(genericDataValue));
	    strncpy(genericDataValue, "wake=1,initialRadius=.5,radiusExpansionRate=.5,fadeTime=40,numRadiFromPreviousSmoke=1.5,metersBehindCurrentPosition=3,vaporTextureNames=vapor1.rgba:vapor2.rgba:vapor3.rgba:vapor4.rgba", sizeof(genericDataValue)-1);
	    DCSGenericData gendata5(4, dtime, -1, "SIMDIS_VaporTrail 1", genericDataValue);
	    DCSCom::sendTCPData(server, DCSREQUEST_HEADER, &gendata5);
            // The following command will also work, but data is sent via UDP
	    // DCSCom::sendGenericData(server, 4, dtime, -1, "SIMDIS_VaporTrail 1", genericDataValue);
	  }

	  // confidence category data via TCP
	  memset(genericDataValue, 0, sizeof(genericDataValue));
	  if(count == 1)
	  {
	    strncpy(genericDataValue, "Low", sizeof(genericDataValue)-1);
	  }
	  else if(count == 2)
	  {
	    strncpy(genericDataValue, "Medium", sizeof(genericDataValue)-1);
	  }
	  else
	  {
	    count = 0;
	    strncpy(genericDataValue, "High", sizeof(genericDataValue)-1);
	  }
	  DCSCategoryData catData (4, dtime, "Confidence", genericDataValue);
	  DCSCom::sendTCPData(server, DCSREQUEST_HEADER, &catData);
	}

	// change icon
	event.id_ = 4;
        event.eventtype_ = DCSEVENT_ICONNAME;
	strncpy(event.chardata_, beamon?"p-3c_orion_nrl":"lear-36", sizeof(event.chardata_)-1);
	event.time_ = dtime;
	DCSCom::sendEvent(server, &event);

	// change callsign
	event.id_ = 4;
        event.eventtype_ = DCSEVENT_CALLSIGN;
	strncpy(event.chardata_, beamon?"Plane":"Jet", sizeof(event.chardata_)-1);
	event.time_ = dtime;
	DCSCom::sendEvent(server, &event);
	strncpy(event.chardata_, beamon?"Plane Beam":"Jet Beam", sizeof(event.chardata_)-1);
	event.id_ = 14;
	DCSCom::sendEvent(server, &event);
	strncpy(event.chardata_, beamon?"Plane Gate":"Jet Gate", sizeof(event.chardata_)-1);
	event.id_ = 24;
	DCSCom::sendEvent(server, &event);

	// change classification
	event.id_ = 4;
        event.eventtype_ = DCSEVENT_CLASSIFICATIONSTR;
	strncpy(event.chardata_, beamon?"GOTS":"SIMDIS", sizeof(event.chardata_)-1);
	event.data_ = beamon?0x600000FF:0x60FF00FF;
	event.time_ = dtime;
	DCSCom::sendEvent(server, &event);

	// change draw mode
	++drawModeCount;
	memset(genericDataValue, 0, sizeof(genericDataValue));
        sstr.str("");
        sstr << (drawModeCount % 3);
        strncpy(genericDataValue, sstr.str().c_str(), sizeof(genericDataValue)-1);
	DCSCom::sendGenericData(server, 4, dtime, -1, "SIMDIS_DrawMode", genericDataValue);
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

    // See if our clients want anything
    DCSCom::serverProcessRequests(server, timeheader, scenheader, headers);

    // Now see if any connections died
    if (!server.vagrants.empty())
    {
      DCSCom::ClientList::iterator s = server.vagrants.begin();
      for (;s != server.vagrants.end(); s++)
      {
        sstr.str("");
        sstr << "Connection from " << (*s)->getAddress(addr, sizeof(addr)) << " " << (*s)->getPort() << " lost\n";
        logger.log(sstr.str().c_str());
      }

      DCSCom::serverExpelVagrants(server);
    }
    
    cout << "Server time: " << dtime << endl;
    
    // check for keyboard entry, more specifically the q key
#ifdef WIN32
    if (_kbhit())
    {
      char keyPressed = _getch();
      if (keyPressed == 'q')
	running=false;
    }
#endif
  } // end of while (running)

  logger.log("Shutting down\n");
  DCSCom::serverStop(server);

  // Free resources allocated for our discovery daemon.
  DCSDisc::discoverFree(discover);

  // Socket finalization status, For the PC, this function calls the WSACleanup.
  // An application or DLL is required to perform a successful WSAStartup call
  // before it can use Windows Sockets services. When it has completed the use
  // of Windows Sockets, the application or DLL must call WSACleanup to
  // deregister itself from a Windows Sockets implementation and allow the
  // implementation to free any resources allocated on behalf of the
  // application or DLL. Any pending blocking or asynchronous calls issued by
  // any thread in this process are canceled without posting any notification
  // messages or without signaling any event objects. Any pending overlapped
  // send and receive operations (WSASend/WSASendTo/WSARecv/WSARecvFrom with
  // an overlapped socket) issued by any thread in this process are also
  // canceled without setting the event object or invoking the completion
  // routine, if specified. In this case, the pending overlapped operations
  // fail with the error status WSA_OPERATION_ABORTED.
  // Sockets that were open when WSACleanup was called are reset and
  // automatically deallocated .
  // There must be a call to WSACleanup for every successful call to WSAStartup
  // made by a task. Only the final WSACleanup for that task does the actual
  // cleanup; the preceding calls simply decrement an internal reference count
  // in the Ws2_32.dll.
  finalizeSocket();

  return 0;
}
