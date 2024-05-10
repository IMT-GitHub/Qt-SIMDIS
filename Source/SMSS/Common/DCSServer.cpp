
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
#include <cmath>
#include <sstream>
#include <signal.h>

#include "iomanipc"
#include "iostreamc"

#include "DCSServer.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

// Socket delay in microseconds for new client connection test
// 5000 usec delay should give us a rate of 200 Hz
#define DELAY 5000

// ***************************************************************************

// fixes angles, in radians, between 0 and two pi
inline double angfix2PI(double x)
{
  return ((x=fmod(x,(M_PI*2.0)))<0.0)?(x+(M_PI*2.0)):x;
}

// fixes angles, in radians, between -pi and pi
inline double angfixPI(double x)
{
  x = angfix2PI(x);
  return (x > M_PI) ? (x - 2 * M_PI) : x;
}

// ***************************************************************************
// ***************************************************************************
// ***************************************************************************

/**
 * The DCSServer class facilitates the creation, setup and sending of data via
 * the NRL developed DCS protocol
 */
DCSServer::DCSServer(const char* p_appName,
		     int8_t proto,
		     int32_t srvPort,
		     int32_t dataPort,
		     const char* p_dataAddr,
		     const char* p_iface,
		     int32_t ttl)
  : Protocol_(proto),
    ServerPort_(srvPort),
    DataPort_(dataPort),
    TTL_(ttl),
    ExpireTimeLimit_(300.),
    LastExpireTime_(0),
    ServerStarted(false)
{
  if (p_dataAddr == 0)
  {
    strncpy(DataAddr_, "225.0.45.55", sizeof(DataAddr_)-1);
  }
  else
  {
    strncpy(DataAddr_, p_dataAddr, sizeof(DataAddr_)-1);
  }

  if (p_iface == 0)
  {
    memset(IfaceAddr_, 0, sizeof(IfaceAddr_));
  }
  else
  {
    strncpy(IfaceAddr_, p_iface, sizeof(IfaceAddr_)-1);
  }

  if (p_appName == 0)
  {
    strncpy(AppName_, "DCS Server", sizeof(AppName_)-1);
  }
  else
  {
    strncpy(AppName_, p_appName, sizeof(AppName_)-1);
  }

  // clear contents of database filename
  memset(DatabaseFile_, 0, sizeof(DatabaseFile_));

  // Delete dcs database file on exit.
  DCSHeaders_.setSave(false);
}

// ***************************************************************************
//
// Destroys all created objects and frees resources allocated for the discovery
// daemon
//

DCSServer::~DCSServer()
{
  if (ServerStarted)
  {
    cout << "Quitting DCS Server: " << AppName_ << endl;

    // Free resources allocated for our discovery daemon.
    DCSCom::serverStop(DCS_);
    DCSDisc::discoverFree(Discover_);

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
  }
}

// ***************************************************************************
//
// method for starting the DCS server based on values set in the constructor
//

void DCSServer::start()
{
  // Socket initialization status, For the PC, this function calls the
  // WSAStartup function, which must be the first Windows Sockets function
  // called by an application or DLL. It allows an application or DLL
  // to specify the version of Windows Sockets required and to retrieve
  // details of the specific Windows Sockets implementation. The application
  // or DLL can only issue further Windows Sockets functions after a
  // successfully calling WSAStartup.
  if (!initializeSocket())
  {
    cerr << "Error initializing socket" << endl;
    exit(1);
  }

#ifndef WIN32
  // Ignore SIGPIPE which will cause an exit when reading from a broken socket
  // more than once.  Seems to affect only UNIX platforms
  signal(SIGPIPE,SIG_IGN);
#endif

  // Initialize our dynamic client discovery socket.
  // The discovery deameon needs to know the IP address and port that
  // this server can be reached through.  For this example we are using
  // port 5555.  The IP address that is specified for this server can be
  // set by picking an outgoing interface.  It only makes sense to report
  // that our server can be accessed through the only interface it is listening
  // on.  For this example, if the interface is NULL
  // (no interface is specified so the kernel will listen on all available
  // network interfaces) we will use the address of a network interface obtained
  // through gethostbyname.  In most cases you would probably specify the host
  // address directly, rather than querying for it via gethostbyname.
  char msg[BUFSIZ];
  char addr[DCS_ADDRSTRLEN];
  char hname[BUFSIZ];
  char hostname[BUFSIZ];
  memset (hostname, 0, sizeof(hostname));
  memset (hname, 0, sizeof(hname));
  memset (addr, 0, sizeof(addr));
  memset (msg, 0, sizeof(msg));

  if (strlen(IfaceAddr_) == 0)
  {
    strncpy(msg, AppName_, sizeof(msg)-1);
    strncpy(addr, "192.168.1.20", sizeof(addr)-1);

    if (gethostname(hname, sizeof(hname)) != SOCKET_ERROR)
    {
      strncat(msg," on ",sizeof(msg)-1);
      strncat(msg,hname,sizeof(msg)-1);

      struct hostent *hptr = gethostbyname(hname);
      if (hptr != NULL && *hptr->h_addr_list != NULL)
      {
        struct in_addr in;
        memcpy(&in.s_addr, *hptr->h_addr_list, 4);
        strncpy(addr, inet_ntoa(in), sizeof(addr)-1);
      }
    }
    else
    {
      strncpy (hname, "nohost", sizeof(hname)-1);
    }

    printf("Will tell clients to connect to %s on port %d\n", addr, ServerPort_);
    DCSDisc::discoverServInit(Discover_, ServerPort_, addr, msg, TTL_, IfaceAddr_);
  }
  else
  {
    if (gethostname(hname, sizeof(hname)) == SOCKET_ERROR)
    {
      strncpy (hname, "nohost", sizeof(hname)-1);
    }
    printf("Will tell clients to connect to %s on port %d\n", IfaceAddr_, ServerPort_);
    DCSDisc::discoverServInit(Discover_, ServerPort_, IfaceAddr_, msg, TTL_, IfaceAddr_);
  }

  // create hostname based on current local time
  char tmpTime[32];
  time_t timet = time (NULL);
  struct tm *time_TM = localtime (&timet);
  strncpy (tmpTime, asctime(time_TM), sizeof(tmpTime)-1);
  size_t ii;
  for (ii=0; ii<strlen(tmpTime); ++ii)
  {
    // replace all whitespace, colons & line feeds
    if (tmpTime[ii] == ' ')
      tmpTime[ii] = '_';
    else if (tmpTime[ii] == ':')
      tmpTime[ii] = '_';
    else if (tmpTime[ii] == '\n')
      tmpTime[ii] = '\0';
  }
  std::stringstream sstr;
  sstr << hname << "_" << tmpTime;
  strncpy(hostname, sstr.str().c_str(), sizeof(hostname)-1);

  // Initialize the DCS server - defaults to multicast
  bool success = false;
  switch (Protocol_)
  {
    case DCSDATA_UNICAST:
    {
      success=DCSCom::serverStart(DCS_, ServerPort_, DCSDATA_UNICAST,
				  DataPort_, NULL, IfaceAddr_);
      if (success)
	cout << "DCS Server Protocol: UNICAST" << endl;
      break;
    }

    case DCSDATA_BROADCAST:
    {
      if (!strcmp(&DataAddr_[strlen(DataAddr_) - 4], ".255") ||
	  !strcmp(&DataAddr_[strlen(DataAddr_) - 2], ".0"))
      {
	success=DCSCom::serverStart(DCS_, ServerPort_, DCSDATA_BROADCAST,
				    DataPort_, DataAddr_, IfaceAddr_);
      }
      else
      {
	cerr << "\n\nWARNING!  Incorrect broadcast address: " << DataAddr_ << endl;
	cerr << "WARNING!  Using limited broadcast address: 255.255.255.255\n\n" << endl;
	success=DCSCom::serverStart(DCS_, ServerPort_, DCSDATA_BROADCAST,
				    DataPort_, "255.255.255.255", IfaceAddr_);
      }
      if (success)
	cout << "DCS Server Protocol: BROADCAST" << endl;
      break;
    }

    case DCSDATA_MULTICAST:
    default:
    {
      success=DCSCom::serverStart(DCS_, ServerPort_, DCSDATA_MULTICAST,
				  DataPort_, DataAddr_, IfaceAddr_,
				  TTL_);
      if (success)
	cout << "DCS Server Protocol: MULTICAST" << endl;
      break;
    }
  }

  // test for successful creation of the DCS server
  if (!success)
  {
    printf("\nError starting server: %d\n", DCS_.servsock->getLastError());
    printf("Could not start DCS server.  Verify the server port is not in use by another application.\n\n");
    DCSDisc::discoverFree(Discover_);
    finalizeSocket();
    exit(1);
  }

  // Make the database filename for this app to use as inet data file.
  sstr.str("");
  sstr << "./" << hostname << "_" << ServerPort_ << "_dcsSRV.dat";
  strncpy(DatabaseFile_, sstr.str().c_str(), sizeof(DatabaseFile_)-1);

  // assign initial time value for expiration map
  LastExpireTime_ = (double) time(NULL);

  // reload DCS database file (if file exists)
  if (DCSHeaders_.restore(DatabaseFile_))
  {
    cout << "Loaded " << DCSHeaders_.size() << " from file.  " << endl;
  }
  else
  {
    cout << "Failed to restore file \"" << DatabaseFile_ << "\".  Starting a new one.  " << endl;
    if (!DCSHeaders_.initialize(DatabaseFile_))
    {
      printf("\nFailed to initialize track id map and header database\n");
      printf("\tCheck permissions on or remove file named \"%s\"\n\n", DatabaseFile_);
      DCSDisc::discoverFree(Discover_);
      finalizeSocket();
      exit(1);
    }
  }

  // set server started flag
  ServerStarted = true;
}

// ***************************************************************************
//
// method for removing a DCS object from the DCS database, thus preventing
// it from being sent
//

bool DCSServer::removeHeader(uint64_t objId, double currentTime)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // Have we seen this header before?
  DCSHeaderGroup headergrp;
  if (!DCSHeaders_.retrieve (objId, &headergrp))
  {
    cout << "DCSServer::removeHeader:  Did not find header for id: "
	 << DCS::uint64totext(objId) << endl;
    return false;
  }

  // check to see if there are any beams attached to platform
  std::map<int64_t, std::vector<int64_t> >::iterator bIter = BeamMap_.find(objId);
  if (bIter != BeamMap_.end())
  {
    std::vector<int64_t>::iterator vbIter;
    for (vbIter=bIter->second.begin(); vbIter != bIter->second.end(); ++vbIter)
    {
      // check to see if there are any gates attached to beam
      std::map<int64_t, std::vector<int64_t> >::iterator gIter = GateMap_.find(*vbIter);
      if (gIter != GateMap_.end())
      {
	std::vector<int64_t>::iterator vgIter;
	for (vgIter=gIter->second.begin(); vgIter != gIter->second.end(); ++vgIter)
	{
	  // remove gates from DCS headers database
          sendExpireEvent(*vgIter, currentTime);
	  DCSHeaders_.remove(*vgIter);
	}
	// remove from expiration map
	GateMap_.erase(*vbIter);
      }
      // remove beams from DCS headers database
      sendExpireEvent(*vbIter, currentTime);
      DCSHeaders_.remove(*vbIter);
    }
    // remove from expiration map
    BeamMap_.erase(objId);
  }

  // remove from expiration map
  PlatMap_.erase(objId);

  // send expire event for platform
  sendExpireEvent(objId, currentTime);

  // remove platform from DCS headers database
  DCSHeaders_.remove(objId);

#ifdef DEBUG
  cout << "DCSServer::removeHeader:  Removed header for id: "
       << DCS::uint64totext(objId) << endl;
#endif

  return true;
}

// ***************************************************************************
//
// method for servicing new client connection requests and removing expired objects
//

void DCSServer::idle(double currentTime)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return;
  }

  // check for expired tracks every 15 sec
  if (time(NULL) > LastExpireTime_ + 15.)
  {
    // Go through map and mark all the plats to remove
    // list of expired platforms to remove
    std::vector<int64_t> expiredPlatforms;
    std::map<int64_t, double>::const_iterator platIter;
    for (platIter = PlatMap_.begin();
	 platIter != PlatMap_.end();
	 ++platIter)
    {
      if (platIter->second + ExpireTimeLimit_ < currentTime)
	expiredPlatforms.push_back (platIter->first);
    }

    // Go through removal vector and remove them from map one at a time
    std::vector<int64_t>::const_iterator expIter;
    bool objRemoved = false;
    for (expIter = expiredPlatforms.begin();
	 expIter != expiredPlatforms.end(); ++expIter)
    {
      // dereference iterator in order to access platform id
      if (removeHeader(*expIter, currentTime))
	objRemoved = true;
    }

    // clean out database file
    if (objRemoved)
      DCSHeaders_.clean();

    // update expire time check
    LastExpireTime_ = (double) time(NULL);
  }

  // See if any clients are looking for a server.
  // If they are, announce our IP address and port
  if (Discover_.request->readReady())
    DCSDisc::discoverRespond(Discover_);

  // Wait for new connection test
  // Socket delay in microseconds for new client connection test
  char addr[DCS_ADDRSTRLEN];
  if (DCS_.servsock->readReady(0,DELAY))
  {
    // Accept the connection
    SockClient* client=DCSCom::serverAccept(DCS_);

    if (client!=NULL)
    {
      // Report connection
      cout << "Accepted connection from "
	   << client->getAddress(addr, sizeof(addr)) << " "
	   << client->getPort() << " @ "
	   << currentTime << endl;

      // Terminate any connections to clients that have closed
      if (!DCSCom::serverNegotiate(DCS_, client, TimeHeader_, ScenarioHeader_, DCSHeaders_))
      {
        cout << "Client failed connection negotiation." << endl;
        DCSCom::serverTerminateClient(DCS_, client);
        cout << "Client was removed: " << currentTime << endl;
      }
    }
  }

  // Now process any client requests
  DCSCom::serverProcessRequests(DCS_, TimeHeader_, ScenarioHeader_, DCSHeaders_);

  // Now check for any dead connections
  if (!DCS_.vagrants.empty())
  {
    DCSCom::ClientList::iterator s = DCS_.vagrants.begin();
    for(; s != DCS_.vagrants.end(); s++)
    {
      cout << "Connection from "
	   << (*s)->getAddress(addr,sizeof(addr)) << " "
	   << (*s)->getPort() << " lost @ " << currentTime << "\n";
    }

    DCSCom::serverExpelVagrants(DCS_);
  }
}

// ***************************************************************************
//
// method for sending scenario header data
//

bool DCSServer::sendScenarioHeader(DCSCoordSystem_t coordsystem,
				   const std::string &classification,
				   uint32_t classcolor,
				   int32_t referenceyear,
				   double latorigin,
				   double lonorigin,
				   double altorigin,
				   double time,
				   double timemod,
				   int8_t timetype)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // initialize local copy of scenario data
  ScenarioHeader_.referenceframe_.coordsystem_    = coordsystem;
  if (!classification.empty())
    strncpy (ScenarioHeader_.classificationstr_, classification.c_str(), sizeof(ScenarioHeader_.classificationstr_)-1);
  else
    strncpy (ScenarioHeader_.classificationstr_, "SIMDIS", sizeof(ScenarioHeader_.classificationstr_)-1);
  ScenarioHeader_.classcolor_     = classcolor;
  ScenarioHeader_.referenceyear_  = referenceyear;
  ScenarioHeader_.referenceframe_.latorigin_      = latorigin;
  ScenarioHeader_.referenceframe_.lonorigin_      = lonorigin;
  ScenarioHeader_.referenceframe_.altorigin_      = altorigin;

  // initialize local copy of time data
  TimeHeader_.realtimemod_    = timemod;
  TimeHeader_.timetype_       = timetype;
  TimeHeader_.timedata_.time_ = time;

  // The above headers are sent via the DCSServer::idle() method during the
  // DCSCom::serverNegotiate() call for any new client(s)

  return true;
}

bool DCSServer::sendScenarioHeader(const DCSCoordReferenceFrame& refFrame,
				   const std::string &classification,
				   uint32_t classcolor,
				   int32_t referenceyear,
				   double time,
				   double timemod,
				   int8_t timetype)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // initialize local copy of scenario data
  ScenarioHeader_.referenceframe_ = refFrame;
  if (!classification.empty())
    strncpy (ScenarioHeader_.classificationstr_, classification.c_str(), sizeof(ScenarioHeader_.classificationstr_)-1);
  else
    strncpy (ScenarioHeader_.classificationstr_, "SIMDIS", sizeof(ScenarioHeader_.classificationstr_)-1);
  ScenarioHeader_.classcolor_     = classcolor;
  ScenarioHeader_.referenceyear_  = referenceyear;

  // initialize local copy of time data
  TimeHeader_.realtimemod_    = timemod;
  TimeHeader_.timetype_       = timetype;
  TimeHeader_.timedata_.time_ = time;

  // The above headers are sent via the DCSServer::idle() method during the
  // DCSCom::serverNegotiate() call for any new client(s)
  return true;
}

// ***************************************************************************
//
// method for sending scenario header data
//

bool DCSServer::sendScenarioHeader(DCSCoordSystem_t coordsystem,
				   DCSClassification_t classification,
				   int32_t referenceyear,
				   double latorigin,
				   double lonorigin,
				   double time,
				   double timemod,
				   int8_t timetype)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // initialize local copy of scenario data
  ScenarioHeader_.referenceframe_.coordsystem_    = coordsystem;
  strncpy (ScenarioHeader_.classificationstr_, DCS::getClassificationString (classification), sizeof (ScenarioHeader_.classificationstr_)-1);
  ScenarioHeader_.classcolor_ = DCS::getClassificationColor (classification);
  ScenarioHeader_.referenceyear_  = referenceyear;
  ScenarioHeader_.referenceframe_.latorigin_      = latorigin;
  ScenarioHeader_.referenceframe_.lonorigin_      = lonorigin;

  // initialize local copy of time data
  TimeHeader_.realtimemod_    = timemod;
  TimeHeader_.timetype_       = timetype;
  TimeHeader_.timedata_.time_ = time;

  // The above headers are sent via the DCSServer::idle() method during the
  // DCSCom::serverNegotiate() call for any new client(s)
  return true;
}

// ***************************************************************************
//
// method for sending platform header & data
//

bool DCSServer::sendPlatformHeader(uint64_t platid,
				   const std::string &callsign,
				   const std::string &iconname,
				   DCSCoordSystem_t coordsystem,
				   double latorigin,
				   double lonorigin,
				   double altorigin,
				   DCSVerticalDatum_t vertDatum,
				   double vertOffset,
				   DCSMagneticVariance_t magVar,
				   double magOffset,
				   double eciReference,
				   double tangentX,
				   double tangentY,
				   double tangentAngle,
				   double time,
				   double x,
				   double y,
				   double z,
				   double c,
				   double p,
				   double r,
				   double vx,
				   double vy,
				   double vz,
				   double ax,
				   double ay,
				   double az)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // Have we seen this header before?
  DCSHeaderGroup headergrp;
  DCSPlatformHeader *platheader = (DCSPlatformHeader*)DCSHeaders_.retrieve(platid, &headergrp);

  if (!platheader)
  {
    platheader = &headergrp.platheader_;
    platheader->id_= platid;
    platheader->platdata_.id_ = platid;
    platheader->state_ = DCSSTATE_ON;
    platheader->status_ = DCSSTATUS_LIVE;
    if (!callsign.empty())
      strncpy (platheader->callsign_, callsign.c_str(), sizeof(platheader->callsign_)-1);
    if (!iconname.empty())
      strncpy (platheader->iconname_, iconname.c_str(), sizeof(platheader->iconname_)-1);
    platheader->referenceframe_.verticalDatum_ = vertDatum;
    platheader->referenceframe_.verticalOffset_ = vertOffset;
    platheader->referenceframe_.magneticVariance_ = magVar;
    platheader->referenceframe_.magneticOffset_ = magOffset;
    platheader->referenceframe_.coordsystem_ = coordsystem;
    platheader->referenceframe_.latorigin_ = latorigin;
    platheader->referenceframe_.lonorigin_ = lonorigin;
    platheader->referenceframe_.altorigin_ = altorigin;
    platheader->referenceframe_.referenceTimeECI_ = eciReference;
    platheader->referenceframe_.tangentOffsetX_ = tangentX;
    platheader->referenceframe_.tangentOffsetY_ = tangentY;
    platheader->referenceframe_.tangentAngle_ = tangentAngle;

    // add new platform to expiration map
    PlatMap_[platid] = time;

    // assign updated time
    platheader->platdata_.time_ = time;

    // assign position
    platheader->platdata_.position_.set(x, y, z);

    // assign orientation
    platheader->platdata_.orientation_.set(c, p, r);

    // assign velocities
    platheader->platdata_.velocity_.set(vx, vy, vz);

    // assign accelerations
    platheader->platdata_.accel_.set(ax, ay, az);

    // send all clients our new header
    DCSCom::sendPlatformHeader(DCS_, platheader);

    // update header in container
    bool present;
    DCSHeaders_.update(platheader, present);

#ifdef DEBUG
    cout << "Created " << platheader->callsign_ << " Header for id: "
	 << DCS::uint64totext(platid) << endl;
#endif

    return true;
  }
  return false;
}

// ***************************************************************************
//
// method for sending platform header & data
//

bool sendPlatformHeaer(DCSPlatformHeader header)
{

  return true;
}

bool DCSServer::sendPlatformHeader(uint64_t platid,
				   const std::string &callsign,
				   const std::string &iconname,
				   DCSPlatformType_t plattype,
				   DCSAllegience_t allegience,
				   double time,
				   double x,
				   double y,
				   double z,
				   double c,
				   double p,
				   double r,
				   double vx,
				   double vy,
				   double vz,
				   double ax,
				   double ay,
				   double az)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // Have we seen this header before?
  DCSHeaderGroup headergrp;
  DCSPlatformHeader *platheader = (DCSPlatformHeader*)DCSHeaders_.retrieve(platid, &headergrp);

  if (!platheader)
  {
    platheader = &headergrp.platheader_;
    platheader->id_= platid;
    platheader->platdata_.id_ = platid;
    platheader->state_ = DCSSTATE_ON;
    platheader->status_ = DCSSTATUS_LIVE;
    if (!callsign.empty())
      strncpy (platheader->callsign_, callsign.c_str(), sizeof(platheader->callsign_)-1);
    if (!iconname.empty())
      strncpy (platheader->iconname_, iconname.c_str(), sizeof(platheader->iconname_)-1);
    platheader->referenceframe_ = ScenarioHeader_.referenceframe_;

    // add new platform to expiration map
    PlatMap_[platid] = time;

    // assign updated time
    platheader->platdata_.time_ = time;

    // assign position
    platheader->platdata_.position_.set(x, y, z);

    // assign orientation
    platheader->platdata_.orientation_.set(c, p, r);

    // assign velocities
    platheader->platdata_.velocity_.set(vx, vy, vz);

    // assign accelerations
    platheader->platdata_.accel_.set(ax, ay, az);

    // send all clients our new header
    DCSCom::sendPlatformHeader(DCS_, platheader);
    
    // Send out the platform type and category data
    DCSCom::sendCategoryData(DCS_, platid, time, DCS::getCategoryName (allegience), DCS::getCategoryValue (allegience));
    DCSCom::sendCategoryData(DCS_, platid, time, DCS::getCategoryName (plattype), DCS::getCategoryValue (plattype));

    // update header in container
    bool present;
    DCSHeaders_.update(platheader, present);

#ifdef DEBUG
    cout << "Created " << platheader->callsign_ << " Header for id: "
	 << DCS::uint64totext(platid) << endl;
#endif

    return true;
  }
  return false;
}

// ***************************************************************************
//
// method for sending beam header & data
//

bool DCSServer::sendBeamHeader(uint64_t beamid,
			       uint64_t platid,
			       const std::string &callsign,
			       double hbw,
			       double vbw,
			       int64_t targetId,
			       uint32_t color,
			       double azimoffset,
			       double elevoffset,
			       double xoffset,
			       double yoffset,
			       double zoffset,
			       DCSBeamType_t beamtype,
			       double time,
			       double azim,
			       double elev,
			       double length)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // Have we seen this header before?
  DCSHeaderGroup headergrp;
  DCSBeamHeader* beamheader=(DCSBeamHeader*)DCSHeaders_.retrieve(beamid, &headergrp);

  if (!beamheader)
  {
    beamheader = &headergrp.beamheader_;
    beamheader->id_ = beamid;
    beamheader->hostid_ = platid;
    std::stringstream sstr;
    if (!callsign.empty())
    {
      sstr << callsign;
    }
    else
    {
      sstr << "beam " << DCS::uint64totext(beamid);
    }
    strncpy (beamheader->callsign_, sstr.str().c_str(), sizeof(beamheader->callsign_)-1);
    beamheader->offset_.set(xoffset, yoffset, zoffset);
    beamheader->azimoffset_ = angfix2PI(azimoffset);
    beamheader->elevoffset_ = angfixPI(elevoffset);
    beamheader->hbw_ = angfixPI(hbw);
    beamheader->vbw_ = angfixPI(vbw);
    beamheader->color_ = color;
    beamheader->targetid_ = targetId;
    beamheader->state_ = DCSSTATE_OFF;
    beamheader->beamtype_ = beamtype;

    // locate & store beam id for expiration checks
    std::map<int64_t, std::vector<int64_t> >::iterator bIter = BeamMap_.find(platid);
    if (bIter != BeamMap_.end())
    {
      bIter->second.push_back(beamid);
    }
    else
    {
      std::vector<int64_t> bVec;
      bVec.push_back(beamid);
      BeamMap_[platid] = bVec;
    }

    // assign id & time
    beamheader->beamdata_.id_ = beamid;
    beamheader->beamdata_.time_ = time;

    // assign azimuth
    beamheader->beamdata_.azim_ = azim;

    // assign elevation
    beamheader->beamdata_.elev_ = elev;

    // assign length
    beamheader->beamdata_.length_ = length;

    // send all clients our new header
    DCSCom::sendBeamHeader(DCS_, beamheader);

    // update header in container
    bool present;
    DCSHeaders_.update(beamheader, present);

#ifdef DEBUG
    cout << "Created " << sstr.str() << " Header for id: "
	 << DCS::uint64totext(beamid) << endl;
#endif

    return true;
  }
  return false;
}

// ***************************************************************************
//
// method for sending gate header & data
//

bool DCSServer::sendGateHeader(uint64_t gateid,
			       uint64_t beamid,
			       const std::string &callsign,
			       uint32_t color,
			       double azimoffset,
			       double elevoffset,
			       double xoffset,
			       double yoffset,
			       double zoffset,
			       DCSGateType_t gatetype,
			       double time,
			       double azim,
			       double elev,
			       double width,
			       double height,
			       double minrange,
			       double centroid,
			       double maxrange)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // Have we seen this header before?
  DCSHeaderGroup headergrp;
  DCSGateHeader *gateheader = (DCSGateHeader*)DCSHeaders_.retrieve(gateid, &headergrp);

  if (!gateheader)
  {
    gateheader = &headergrp.gateheader_;
    gateheader->id_ = gateid;
    std::stringstream sstr;
    if (!callsign.empty())
    {
      sstr << callsign;
    }
    else
    {
      sstr << "gate " << DCS::uint64totext(gateid);
    }
    strncpy (gateheader->callsign_, sstr.str().c_str(), sizeof(gateheader->callsign_)-1);
    gateheader->azimoffset_ = angfix2PI(azimoffset);
    gateheader->elevoffset_ = angfixPI(elevoffset);
    gateheader->offset_.set(xoffset, yoffset, zoffset);
    gateheader->hostid_ = beamid;
    gateheader->color_ = color;
    gateheader->state_ = DCSSTATE_OFF;
    gateheader->gatetype_ = gatetype;

    // locate & store gate id for expiration checks
    std::map<int64_t, std::vector<int64_t> >::iterator gIter = GateMap_.find(beamid);
    if (gIter != GateMap_.end())
    {
      gIter->second.push_back(gateid);
    }
    else
    {
      std::vector<int64_t> gVec;
      gVec.push_back(gateid);
      GateMap_[beamid] = gVec;
    }

    // assign id & time
    gateheader->gatedata_.id_ = gateid;
    gateheader->gatedata_.time_ = time;

    // assign azimuth
    gateheader->gatedata_.azim_ = azim;

    // assign elevation
    gateheader->gatedata_.elev_ = elev;

    // assign width
    gateheader->gatedata_.width_ = width;

    // assign height
    gateheader->gatedata_.height_ = height;

    // assign min range
    gateheader->gatedata_.minrange_ = minrange;

    // assign max range
    gateheader->gatedata_.maxrange_ = maxrange;

    // assign centroid
    gateheader->gatedata_.centroid_ = centroid;

    // send all clients our new header
    DCSCom::sendGateHeader(DCS_, gateheader);

    // update header in container
    bool present;
    DCSHeaders_.update(gateheader, present);

#ifdef DEBUG
    cout << "Created " << sstr.str() << " Header for id: "
	 << DCS::uint64totext(gateid) << endl;
#endif

    return true;
  }
  return false;
}


// ***************************************************************************
//
// method for sending gate header & data
//

bool DCSServer::sendGateHeader(uint64_t gateid,
			       uint64_t beamid,
			       const std::string &callsign,
			       uint32_t color,
			       double xoffset,
			       double yoffset,
			       double zoffset,
			       DCSGateType_t gatetype,
			       double time,
			       double azim,
			       double elev,
			       double width,
			       double height,
			       double minrange,
			       double centroid,
			       double maxrange)
{
  return sendGateHeader(gateid,
			beamid,
			callsign,
			color,
			0,
			0,
			xoffset,
			yoffset,
			zoffset,
			gatetype,
			time,
			azim,
			elev,
			width,
			height,
			minrange,
			centroid,
			maxrange);
}

// ***************************************************************************
//
// method for sending platform data
//

bool DCSServer::sendPlatformData(uint64_t platid,
				 double time,
				 double x,
				 double y,
				 double z,
				 double c,
				 double p,
				 double r,
				 double vx,
				 double vy,
				 double vz,
				 double ax,
				 double ay,
				 double az)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // Have we seen this header before?
  DCSHeaderGroup headergrp;
  DCSPlatformHeader *platheader = (DCSPlatformHeader*)DCSHeaders_.retrieve(platid, &headergrp);

  if (!platheader)
  {
    cout << "DCSServer::sendPlatformData:  Did not find platform header for id: "
	 << DCS::uint64totext(platid) << endl;
    return false;
  }

  // find platform in expiration map, then update time
  std::map<int64_t, double>::iterator platIter = PlatMap_.find(platid);
  if (platIter != PlatMap_.end())
  {
    platIter->second = time;
  }

  // assign updated time
  platheader->platdata_.time_ = time;

  // assign position
  platheader->platdata_.position_.set(x,y,z);

  // assign orientation
  platheader->platdata_.orientation_.set(angfix2PI(c), angfixPI(p), angfixPI(r));

  // assign velocities
  platheader->platdata_.velocity_.set(vx,vy,vz);

  // assign accelerations
  platheader->platdata_.accel_.set(ax,ay,az);

  // send all clients our new header
  DCSCom::sendPlatformHeader(DCS_, platheader);

  // update header in container
  bool present;
  DCSHeaders_.update(platheader, present);

  // send platform data
  DCSCom::sendPlatformData(DCS_, &platheader->platdata_);

  return true;
}

// ***************************************************************************
//
// method for sending beam data
//

bool DCSServer::sendBeamData(uint64_t beamid,
			     double time,
			     double azim,
			     double elev,
			     double length)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // Have we seen this header before?
  DCSHeaderGroup headergrp;
  DCSBeamHeader *beamheader = (DCSBeamHeader*)DCSHeaders_.retrieve(beamid, &headergrp);

  if (!beamheader)
  {
    cout << "DCSServer::sendBeamData:  Did not find beam header for id: "
	 << DCS::uint64totext(beamid) << endl;
    return false;
  }

  // assign id & time
  beamheader->beamdata_.id_ = beamid;
  beamheader->beamdata_.time_ = time;

  // assign azimuth
  beamheader->beamdata_.azim_ = angfix2PI(azim);

  // assign elevation
  beamheader->beamdata_.elev_ = angfixPI(elev);

  // assign length
  beamheader->beamdata_.length_ = length;

  // update header in container
  bool present;
  DCSHeaders_.update(beamheader, present);

  // send beam data
  DCSCom::sendBeamData(DCS_, &beamheader->beamdata_);

  return true;
}

// ***************************************************************************
//
// method for sending gate data
//

bool DCSServer::sendGateData(uint64_t gateid,
			     double time,
			     double azim,
			     double elev,
			     double width,
			     double height,
			     double minrange,
			     double centroid,
			     double maxrange)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // Have we seen this header before?
  DCSHeaderGroup headergrp;
  DCSGateHeader *gateheader = (DCSGateHeader*)DCSHeaders_.retrieve(gateid, &headergrp);

  if (!gateheader)
  {
    cout << "DCSServer::sendGateData:  Did not find gate header for id: "
	 << DCS::uint64totext(gateid) << endl;
    return false;
  }

  // assign id & time
  gateheader->gatedata_.id_ = gateid;
  gateheader->gatedata_.time_ = time;

  // assign azimuth
  gateheader->gatedata_.azim_ = angfix2PI(azim);

  // assign elevation
  gateheader->gatedata_.elev_ = angfixPI(elev);

  // assign width
  gateheader->gatedata_.width_ = angfix2PI(width);

  // assign height
  gateheader->gatedata_.height_ = angfix2PI(height);

  // assign min range
  gateheader->gatedata_.minrange_ = minrange;

  // assign centroid
  gateheader->gatedata_.centroid_ = centroid;

  // assign max range
  gateheader->gatedata_.maxrange_ = maxrange;

  // update header in container
  bool present;
  DCSHeaders_.update(gateheader, present);

  // send gate data
  DCSCom::sendGateData(DCS_, &gateheader->gatedata_);

  return true;
}

// ***************************************************************************
//
// method for sending a-scope data
//

bool DCSServer::sendScopeData(uint64_t id,
			      UTILS::UnitType_t xUnits,
			      UTILS::UnitType_t yUnits,
			      double time,
			      double xStart,
			      double xStep,
			      double yBias,
			      double yScale,
			      uint32_t numDataPoints,
			      UTILS::VariableType_t dataType,
			      uint16_t numGates,
			      const void* data,
			      const DCSScopeGateData* gates)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return false;
  }

  // search the Scope List for this scope, if it's not in there, add it
  unsigned int i = 0; // loop counter
  for(i; i < ScopeList_.size(); i++)
  {
    // if we find the id break out of the loop and go on
    if(ScopeList_[i] == static_cast<int64_t>(id))
      break;
  }
  //if we got all the way through the list without finding it, add it to the list
  if(i==ScopeList_.size())
    ScopeList_.push_back(id);


  DCSCom::sendScopeData(DCS_, id,xUnits,yUnits, time, xStart, xStep, yBias, yScale, numDataPoints, dataType, numGates, data, gates);

  return true;
}


// ***************************************************************************
//
// method for sending generic data
//

bool DCSServer::sendGenericData(uint64_t objId,
				const std::string &tag,
				const std::string &data,
				double bgntime,
				double expiretime)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  DCSHeaderGroup headergrp;
  // allow scenario level generic data (objId == 0)
  if (!DCSHeaders_.retrieve(objId, &headergrp) && (objId != 0))
  {
    cout << "DCSServer::sendGenericData:  Did not find header for id: " << DCS::uint64totext(objId) << endl;
    return false;
  }
  DCSGenericData genData(objId, bgntime, expiretime, tag.c_str(), data.c_str());
  DCSCom::sendGenericData(DCS_, &genData);

  return true;
}


// ***************************************************************************
//
// method for sending category data
//

bool DCSServer::sendCategoryData(uint64_t objId,
				 const std::string &category,
				 const std::string &data,
				 double timeVal)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  DCSHeaderGroup headergrp;
  // allow scenario level generic data (objId == 0)
  if (!DCSHeaders_.retrieve(objId, &headergrp) && (objId != 0))
  {
    cout << "DCSServer::sendCategoryData:  Did not find header for id: " << DCS::uint64totext(objId) << endl;
    return false;
  }
  DCSCategoryData catData(objId, timeVal, category.c_str(), data.c_str());
  DCSCom::sendCategoryData(DCS_, &catData);

  return true;
}

// ***************************************************************************
//
// method for sending time data
//

bool DCSServer::sendTime(double time)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // update and send time
  TimeHeader_.timedata_.time_ = time;
  DCSCom::sendTimeData(DCS_, &TimeHeader_.timedata_);

  return true;
}

// ***************************************************************************
//
// method for sending state event data
//

bool DCSServer::sendStateEvent(uint64_t objId,
			       bool on,
			       double time)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // Have we seen this header before?
  DCSHeaderGroup headergrp;
  DCSBaseHeader *hdr = (DCSBaseHeader*)DCSHeaders_.retrieve(objId, &headergrp);

  if (!hdr)
  {
    cout << "DCSServer::sendStateEvent:  Did not find header for id: " << DCS::uint64totext(objId) << endl;
    return false;
  }

  // create and send event
  DCSEvent event;
  event.data_ = (on)?DCSSTATE_ON:DCSSTATE_OFF;
  event.eventtype_ = DCSEVENT_STATE;
  event.time_ = time;
  event.id_ = objId;
  DCSCom::sendEvent (DCS_, &event);

  // update header in container
  bool present;
  switch (hdr->getType())
  {
    case DCSPLATFORMHEADER:
    {
      DCSPlatformHeader *platheader = (DCSPlatformHeader *)hdr;
      platheader->state_ = (DCSObjectState_t)event.data_;
      DCSHeaders_.update(platheader, present);
    }
    break;

    case DCSBEAMHEADER:
    {
      DCSBeamHeader *beamheader = (DCSBeamHeader *)hdr;
      beamheader->state_ = (DCSObjectState_t)event.data_;
      DCSHeaders_.update(beamheader, present);
    }
    break;

    case DCSGATEHEADER:
    {
      DCSGateHeader *gateheader = (DCSGateHeader *)hdr;
      gateheader->state_ = (DCSObjectState_t)event.data_;
      DCSHeaders_.update(gateheader, present);
    }
    break;
  }

  return true;
}


// ***************************************************************************
//
// method for sending expire event data
//

bool DCSServer::sendExpireEvent(uint64_t objId,
				double time)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // Have we seen this header before?
  DCSHeaderGroup headergrp;
  DCSBaseHeader *hdr = (DCSBaseHeader*)DCSHeaders_.retrieve(objId, &headergrp);

  if (!hdr)
  {
    cout << "DCSServer::sendExpireEvent:  Did not find header for id: " << DCS::uint64totext(objId) << endl;
    return false;
  }

  // create and send event
  DCSEvent event;
  event.data_ = DCSSTATE_EXPIRE;
  event.eventtype_ = DCSEVENT_STATE;
  event.time_ = time;
  event.id_ = objId;
  DCSCom::sendEvent (DCS_, &event);

  // update header in container
  bool present;
  switch (hdr->getType())
  {
    case DCSPLATFORMHEADER:
    {
      DCSPlatformHeader *platheader = (DCSPlatformHeader *)hdr;
      platheader->state_ = (DCSObjectState_t)event.data_;
      DCSHeaders_.update(platheader, present);
    }
    break;

    case DCSBEAMHEADER:
    {
      DCSBeamHeader *beamheader = (DCSBeamHeader *)hdr;
      beamheader->state_ = (DCSObjectState_t)event.data_;
      DCSHeaders_.update(beamheader, present);
    }
    break;

    case DCSGATEHEADER:
    {
      DCSGateHeader *gateheader = (DCSGateHeader *)hdr;
      gateheader->state_ = (DCSObjectState_t)event.data_;
      DCSHeaders_.update(gateheader, present);
    }
    break;
  }

  return true;
}

// ***************************************************************************
//
// method for sending color event data
//

bool DCSServer::sendColorEvent(uint64_t objId,
			       uint32_t color,
			       double time)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // Have we seen this header before?
  DCSHeaderGroup headergrp;
  DCSBaseHeader *hdr = (DCSBaseHeader*)DCSHeaders_.retrieve(objId, &headergrp);

  bool objIsScope = false;
  if (!hdr)
  {
    //check to see if this is Scope Data, if so then it's kosher that there isn't a header, if not then boot out
    unsigned int i = 0;
    for (i; i<ScopeList_.size(); i++)
    {
      if( ScopeList_[i] == static_cast<int64_t>(objId) )
      {
	objIsScope = true;
	break;
      }
    }
    if(!objIsScope)
    {
      cout << "DCSServer::sendColorEvent:  Did not find header for id: " << DCS::uint64totext(objId) << endl;
      return false;
    }
  }

  // create and send event
  DCSEvent event;
  event.data_ = color;
  event.eventtype_ = DCSEVENT_COLOR;
  event.time_ = time;
  event.id_ = objId;
  DCSCom::sendEvent (DCS_, &event);

  // if the object was ScopeData, then there is no need to update a header
  if(objIsScope)
    return true;

  // otherwise, update header in container
  bool present;
  switch (hdr->getType())
  {
    case DCSPLATFORMHEADER:
    {
      DCSPlatformHeader *platheader = (DCSPlatformHeader *)hdr;
      platheader->color_ = (uint32_t)event.data_;
      DCSHeaders_.update(platheader, present);
    }
    break;

    case DCSBEAMHEADER:
    {
      DCSBeamHeader *beamheader = (DCSBeamHeader *)hdr;
      beamheader->color_ = (uint32_t)event.data_;
      DCSHeaders_.update(beamheader, present);
    }
    break;

    case DCSGATEHEADER:
    {
      DCSGateHeader *gateheader = (DCSGateHeader *)hdr;
      gateheader->color_ = (uint32_t)event.data_;
      DCSHeaders_.update(gateheader, present);
    }
    break;
  }

  return true;
}

// ***************************************************************************
//
// method for sending track quality event data
//

bool DCSServer::sendTrackQualityEvent(uint64_t objId,
				      bool quality,
				      double time)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // Have we seen this header before?
  DCSHeaderGroup headergrp;
  DCSBaseHeader *hdr = (DCSBaseHeader*)DCSHeaders_.retrieve(objId, &headergrp);

  if (!hdr)
  {
    cout << "DCSServer::sendTrackQualityEvent:  Did not find header for id: " << DCS::uint64totext(objId) << endl;
    return false;
  }

  // create and send event
  DCSEvent event;
  event.data_ = (quality)?DCSSTATUS_LIVE:DCSSTATUS_DROPPED;
  event.eventtype_ = DCSEVENT_STATUS;
  event.time_ = time;
  event.id_ = objId;
  DCSCom::sendEvent (DCS_, &event);

  // update header in container
  bool present;
  switch (hdr->getType())
  {
    case DCSPLATFORMHEADER:
    {
      DCSPlatformHeader *platheader = (DCSPlatformHeader *)hdr;
      platheader->status_ = (DCSTrackStatus_t)event.data_;
      DCSHeaders_.update(platheader, present);
    }
    break;

    case DCSBEAMHEADER:
    case DCSGATEHEADER:
    {
      // has no effect in SIMDIS
      cout << "DCSServer::sendTrackQualityEvent:  Event has no effect, sent to beam/gate with id: " << DCS::uint64totext(objId) << endl;
    }
    break;
  }

  return true;
}


// ***************************************************************************
//
// method for sending target id event data
//

bool DCSServer::sendTargetEvent(uint64_t objId,
				int64_t targetId,
				double time)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // Have we seen this header before?
  DCSHeaderGroup headergrp;
  DCSBaseHeader *hdr = (DCSBaseHeader*)DCSHeaders_.retrieve(objId, &headergrp);
  if (!hdr)
  {
    cout << "DCSServer::sendTargetEvent:  Did not find header for id: " << DCS::uint64totext(objId) << endl;
    return false;
  }
  DCSBaseHeader *targethdr = (DCSBaseHeader*)DCSHeaders_.retrieve(targetId, &headergrp);
  if (!targethdr)
  {
    cout << "DCSServer::sendTargetEvent:  Did not find header for target id: " << DCS::uint64totext(targetId) << endl;
    return false;
  }

  // create and send event
  DCSEvent event;
  event.data_ = targetId;
  event.eventtype_ = DCSEVENT_TARGETID;
  event.time_ = time;
  event.id_ = objId;
  DCSCom::sendEvent (DCS_, &event);

  // update header in container
  bool present;
  switch (hdr->getType())
  {
    case DCSPLATFORMHEADER:
    case DCSGATEHEADER:
    {
      // has no effect in SIMDIS
      cout << "DCSServer::sendTargetEvent:  Event has no effect, sent to platform/gate with id: " << DCS::uint64totext(objId) << endl;
    }
    break;

    case DCSBEAMHEADER:
    {
      DCSBeamHeader *beamheader = (DCSBeamHeader *)hdr;
      beamheader->beamtype_ = DCSBEAM_STEADY;
      beamheader->targetid_ = event.data_;
      DCSHeaders_.update(beamheader, present);
    }
    break;
  }

  return true;
}


// ***************************************************************************
//
// method for sending the scenario classification event data
//

bool DCSServer::sendClassificationEvent(DCSClassification_t classification,
					double time)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // create and send event
  uint32_t classcolor = DCS::getClassificationColor (classification);
  const char* classStr = DCS::getClassificationString (classification);
  if ((classcolor != ScenarioHeader_.classcolor_) ||
      (strcmp (classStr, ScenarioHeader_.classificationstr_) != 0))
  {
    DCSEvent event;
    event.data_ = classification;
    event.eventtype_ = DCSEVENT_CLASSIFICATION;
    event.time_ = time;
    event.id_ = 1;
    DCSCom::sendEvent (DCS_, &event);
    strncpy (ScenarioHeader_.classificationstr_, DCS::getClassificationString (classification), sizeof (ScenarioHeader_.classificationstr_)-1);
    ScenarioHeader_.classcolor_ = DCS::getClassificationColor (classification);
  }

  return true;
}

// ***************************************************************************
//
// method for sending the scenario classification event data
//

bool DCSServer::sendClassificationEvent(const std::string &classification,
					uint32_t color,
					double time)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // create and send event
  if (!classification.empty() &&
      (strcmp (classification.c_str(), ScenarioHeader_.classificationstr_) != 0))
  {
    DCSEvent event;
    strncpy (event.chardata_, classification.c_str(), sizeof (event.chardata_)-1);
    event.data_ = color;
    event.eventtype_ = DCSEVENT_CLASSIFICATIONSTR;
    event.time_ = time;
    event.id_ = 1;
    DCSCom::sendEvent (DCS_, &event);
    strncpy (ScenarioHeader_.classificationstr_, classification.c_str(), sizeof (ScenarioHeader_.classificationstr_)-1);
    return true;
  }

  return false;
}

// ***************************************************************************
//
// method for sending event data
//

bool DCSServer::sendPlatformInterpolate(uint64_t platId,
					bool interpolate,
					double time)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // create and send event
  DCSEvent event;
  event.data_ = (interpolate)?1:0;
  event.eventtype_ = DCSEVENT_PLATFORMINTERPOLATE;
  event.time_ = time;
  event.id_ = platId;
  DCSCom::sendEvent (DCS_, &event);
  // Modify the platform header
  DCSHeaderGroup headergrp;
  DCSBaseHeader *hdr = (DCSBaseHeader*)DCSHeaders_.retrieve(platId, &headergrp);
  if (hdr != NULL && hdr->getType() == DCSPLATFORMHEADER)
    ((DCSPlatformHeader*)hdr)->interpolate_ = interpolate;
  return true;
}

// ***************************************************************************
//
// method for sending event data
//

bool DCSServer::sendBeamInterpolatePosition(uint64_t beamId,
					    bool interpolate,
					    double time)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // create and send event
  DCSEvent event;
  event.data_ = (interpolate)?1:0;
  event.eventtype_ = DCSEVENT_BEAMINTERPPOS;
  event.time_ = time;
  event.id_ = beamId;
  DCSCom::sendEvent (DCS_, &event);
  // Modify the beam header
  DCSHeaderGroup headergrp;
  DCSBaseHeader *hdr = (DCSBaseHeader*)DCSHeaders_.retrieve(beamId, &headergrp);
  if (hdr != NULL && hdr->getType() == DCSBEAMHEADER)
    ((DCSBeamHeader*)hdr)->interpolate_ = interpolate;
  return true;
}

// ***************************************************************************
//
// method for sending event data
//

bool DCSServer::sendGateInterpolatePosition(uint64_t gateId,
					    bool interpolate,
					    double time)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  // create and send event
  DCSEvent event;
  event.data_ = (interpolate)?1:0;
  event.eventtype_ = DCSEVENT_GATEINTERPPOS;
  event.time_ = time;
  event.id_ = gateId;
  DCSCom::sendEvent (DCS_, &event);
  // Modify the gate header
  DCSHeaderGroup headergrp;
  DCSBaseHeader *hdr = (DCSBaseHeader*)DCSHeaders_.retrieve(gateId, &headergrp);
  if (hdr != NULL && hdr->getType() == DCSGATEHEADER)
    ((DCSGateHeader*)hdr)->interpolate_ = interpolate;
  return true;
}

// ***************************************************************************
//
// method for sending event data
//

bool DCSServer::sendIconNameEvent(uint64_t objId,
				  const std::string &iconName,
				  double time)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  if (!iconName.empty())
  {
    DCSEvent event;
    strncpy(event.chardata_, iconName.c_str(), sizeof (event.chardata_)-1);
    event.eventtype_ = DCSEVENT_ICONNAME;
    event.time_ = time;
    event.id_ = objId;
    DCSCom::sendEvent(DCS_, &event);

    DCSHeaderGroup headergrp;
    DCSBaseHeader *hdr = (DCSBaseHeader*)DCSHeaders_.retrieve(objId, &headergrp);
    if (hdr->getType() == DCSPLATFORMHEADER)
    {
      bool present;
      DCSPlatformHeader *platheader = (DCSPlatformHeader *)hdr;
      strncpy (platheader->iconname_, iconName.c_str(), sizeof (platheader->iconname_)-1);
      DCSHeaders_.update(platheader, present);
    }
    return true;
  }

  return false;
}

// ***************************************************************************
//
// method for sending event data
//

bool DCSServer::sendCallsignEvent(uint64_t objId,
				  const std::string &callsign,
				  double time)
{
  if (!ServerStarted)
  {
    cout << "\nDCSServer::start() has not been called!\n"<<endl;
    return ServerStarted;
  }

  if (!callsign.empty())
  {
    DCSEvent event;
    strncpy(event.chardata_, callsign.c_str(), sizeof (event.chardata_)-1);
    event.eventtype_ = DCSEVENT_CALLSIGN;
    event.time_ = time;
    event.id_ = objId;
    DCSCom::sendEvent(DCS_, &event);
  
    // update header in container
    DCSHeaderGroup headergrp;
    DCSBaseHeader *hdr = (DCSBaseHeader*)DCSHeaders_.retrieve(objId, &headergrp);
    bool present;
    switch (hdr->getType())
    {
      case DCSPLATFORMHEADER:
      {
	DCSPlatformHeader *platheader = (DCSPlatformHeader *)hdr;
	strncpy (platheader->callsign_, callsign.c_str(), sizeof (platheader->callsign_)-1);
	DCSHeaders_.update(platheader, present);
      }
      break;

      case DCSBEAMHEADER:
      {
	DCSBeamHeader *beamheader = (DCSBeamHeader *)hdr;
	strncpy (beamheader->callsign_, callsign.c_str(), sizeof (beamheader->callsign_)-1);
	DCSHeaders_.update(beamheader, present);
      }
      break;

      case DCSGATEHEADER:
      {
	DCSGateHeader *gateheader = (DCSGateHeader *)hdr;
	strncpy (gateheader->callsign_, callsign.c_str(), sizeof (gateheader->callsign_)-1);
	DCSHeaders_.update(gateheader, present);
      }
      break;
    }
    return true;
  }

  return false;
}

bool DCSServer::cleanDatabaseFile()
{
  return DCSHeaders_.clean();
}

// ***************************************************************************
//
// method for determining the listed ID is found in the DCS Header Container
//

bool DCSServer::inDCSHeaders(int64_t id_)
{
  // Have we seen this header before?
  DCSHeaderGroup headergrp;
  DCSPlatformHeader *platheader = (DCSPlatformHeader*)DCSHeaders_.retrieve(id_, &headergrp);
  return (platheader == 0) ? false : true;
}
