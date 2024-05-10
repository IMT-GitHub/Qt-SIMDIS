/*****************************************************************************
* EXAMPLE4
*
* Example 4 adds functionality to traverse the header container and print 
* contents with a DCSLogger.  DCSLogger also replaces 'printf'
*
******************************************************************************/

#include <sstream>
#include <stdio.h>
#include <signal.h>
#include <memory.h>
#include "DCS.h"
#include "DCSaux.h"

#define PI (3.141592654)

#define REFLAT 0 //(22.0*(PI/180.0))
#define REFLON 0 //(-159.0*(PI/180.0))

// Categories for creating the id
#define PLATFORM 1
#define BEAM     2
#define GATE     3

// Suspend program for 100 milliseconds to create a 10HZ data rate.  
#define SLEEP 100

//
// Add traversal function to print contents of a header here
//
void print_header(const DCSBaseHeader* header, void* data)
{
  DCSLogger* logger = (DCSLogger*)data;
  logger->log(header);
}

int main(int argc, char** argv)
{
  bool success, present;
  double dtime = DCS::getyeartime();           // Data time
  char addr[INET_ADDRSTRLEN] = "127.0.0.1";    // String for an IP address
  dcsAuxKeyState keystate;                     // Key state for dcsAuxGetKeyState

  // Network info
  short server_port = 4567;
  short data_port   = 5678;
  char  data_addr[INET_ADDRSTRLEN] = "226.100.100.5";
  char  data_type   = DCSDATA_MULTICAST;
  TTL_T mcast_ttl   = 1;

  // DCS logger
  DCSConsoleLogger logger;

  // DCS info
  DCSCom::DCSServerInfo server;     // DCS server info structure
  DCSHeaderContainer headers;       // Data structure to store Platform, Beam, and Gate data

  DCSScenarioData scendata(0.0, 0.0);  // Scenario wind data
  DCSScenarioHeader scenheader(REFLAT, REFLON, DCS::getyear(), DCSCOORD_NED, DCSCLASS_UNCLASSIFIED, scendata);  // Initialization parameters for scenario

  DCSTimeData timedata(dtime);                              // Provides updates for time
  DCSTimeHeader timeheader(1.0, DCSTIME_PACKET, timedata);  // Initialization parameters for time

  // Initialization headers for all objects
  DCSPlatformData platdata;
  DCSPlatformHeader platheader;
  DCSBeamHeader beamheader;
  DCSGateHeader gateheader;
  DCSEvent event;                   // Modifies the state of a platform, beam, or gate object
  
  // DCS Discovery info
  DCSDisc::DCSDiscover discover;

#ifndef WIN32
  // Ignore UNIX SIGPIPE
  signal(SIGPIPE, SIG_IGN);
#endif

  // Windows requires initialization of Winsock DLL
  if (!initializeSocket())
  {
    logger.log("Error initializing socket\n");
    return 0;
  }

  // Initialize dcs header database here
  headers.initialize("dcsdb.dat");
  headers.setSave(false);           // Delete 'dcsdb.dat' on successful exit.


  // Setup the platform, beam, and gate headers/data here
  platdata.id_ = DCS::make_id(0, 0, PLATFORM, 1);
  platdata.time_ = dtime;
  platdata.position_.set(0.0, 0.0, -100.0);
  platdata.orientation_.set();              // Default to 0
  platdata.velocity_.set();                 // Default to 0
  platdata.accel_.set();                 // Default to 0

  platheader.id_ = platdata.id_;
  strncpy(platheader.iconname_, "usaf_uav", sizeof(platheader.iconname_)-1);
  strncpy(platheader.callsign_, "UAV", sizeof(platheader.callsign_)-1);
  platheader.dims_.set();
  platheader.offset_.set();
  platheader.referenceframe_.latorigin_ = REFLAT;
  platheader.referenceframe_.lonorigin_ = REFLON;
  platheader.referenceframe_.coordsystem_ = DCSCOORD_ENU;
//  platheader.allegience_ = DCSALLEGIENCE_FRIENDLY;
//  platheader.plattype_ = DCSPLATFORM_AIRCRAFT;
  platheader.color_ = 0;
  platheader.state_ = DCSSTATE_ON;
  platheader.status_ = DCSSTATUS_LIVE;
  memcpy(&platheader.platdata_, &platdata, sizeof(platheader.platdata_));

  beamheader.id_ = DCS::make_id(0, 0, BEAM, 1);
  strncpy(beamheader.callsign_, "UAV Beam", sizeof(beamheader.callsign_)-1);
  beamheader.offset_.set(5.0, 0.0, 1.0);
  beamheader.azimoffset_ = 0.0;
  beamheader.elevoffset_ = 0.0;
  beamheader.hbw_ = 3 * (PI / 180.0);
  beamheader.vbw_ = 3 * (PI / 180.0);
  beamheader.hostid_ = platheader.id_;
  beamheader.color_ = 0x0000FFFF;
  beamheader.state_ = DCSSTATE_ON;
  beamheader.beamtype_ = DCSBEAM_LINEAR;

  beamheader.beamdata_.id_ = beamheader.id_;
  beamheader.beamdata_.time_ = dtime;
  beamheader.beamdata_.azim_ = 0.0;
  beamheader.beamdata_.elev_ = 15.0 * (PI / 180.0);
  beamheader.beamdata_.length_ = 200.0;

  gateheader.id_ = DCS::make_id(0, 0, GATE, 1);
  strncpy(gateheader.callsign_, "UAV Gate", sizeof(gateheader.callsign_)-1);
  gateheader.offset_.set(0.0, 0.0, 0.0);
  gateheader.hostid_ = beamheader.id_;
  gateheader.color_ = 0x00FFFFFF;
  gateheader.state_ = DCSSTATE_ON;
  gateheader.gatetype_ = DCSGATE_RANGE;

  gateheader.gatedata_.id_ = gateheader.id_;
  gateheader.gatedata_.time_ = dtime;
  gateheader.gatedata_.azim_ = 0.0;
  gateheader.gatedata_.elev_ = 15.0 * (PI / 180.0);
  gateheader.gatedata_.width_ = 3.0 * (PI / 180.0);
  gateheader.gatedata_.height_ = 3.0 * (PI / 180.0);
  gateheader.gatedata_.minrange_ = 180.0;
  gateheader.gatedata_.maxrange_ = 200.0;
  gateheader.gatedata_.centroid_ = 190.0;

  // Add headers to database here
  if (!headers.update(&platheader, present))
  {
    printf("Failed to add platform to database\n");
  }

  // 'present' indicates if the header was already in the database.  
  // If the header was not in the database, it is new and should be 
  // distributed to all clients with the DCSCom send header functions.  
  if (!headers.update(&beamheader, present))
  {
    printf("Failed to add beam to database\n");
  }

  if (!headers.update(&gateheader, present))
  {
    printf("Failed to add gate to database\n");
  }

  
  // Start discovery service here
  DCS::getdefaultaddr(addr,sizeof(addr));
  DCSDisc::discoverServInit(discover, server_port, addr, "Demonstration server", mcast_ttl, NULL);

  // Start server here
  success = DCSCom::serverStart(server, server_port, data_type, data_port, data_addr, NULL, mcast_ttl);
  if (!success)
  {
    std::stringstream sstr;
    sstr << "Error starting server: " << server.servsock->getLastError() << "\n";
    logger.log(sstr.str().c_str());
    DCSDisc::discoverFree(discover);
    finalizeSocket();
    return 0;
  }

  logger.log("Starting server.  Press 'h' for help, 'q' to quit.\n");

  // main loop
  for (;;)
  {
    if(dcsAuxGetKeyState(keystate,SLEEP)==DCSAUX_QUIT)
    {
      // Quit application
      break;
    }

    // Provide DCS server IP address and port to any requesting clients
    if (discover.request->readReady())
    {
      DCSDisc::discoverRespond(discover);
    }

    // Check for pending client connections here
    if (server.servsock->readReady(0, 0))
    {
      // Accept the connection here
      SockClient* client = DCSCom::serverAccept(server);
      if (client != NULL)
      {
        if (!DCSCom::serverNegotiate(server, client, timeheader, scenheader, headers))
        {
          DCSCom::serverTerminateClient(server, client);
        }
        else
        {
          // Report connection
          std::stringstream sstr;
          sstr << "Accepted connection from " << client->getAddress(addr, sizeof(addr)) << " " << client->getPort() << "\n";
          logger.log(sstr.str().c_str());
        }
      }
    }


    // Send time to clients here
    dtime = DCS::getyeartime();               // Current time in seconds since beginning of year
    timeheader.timedata_.time_ = dtime;       // Update header for distribution to clients at connection negotiations
    timedata.time_ = dtime;
    DCSCom::sendTimeData(server, &timedata);


    // Get platform data here
    if(dcsAuxGetPlatformUpdate(platdata, keystate))
    {
      // Function returns TRUE if there is new data
      // Send new data and update header container

      DCSCom::sendPlatformData(server, &platdata);

      // Update database with new position
      memcpy(&platheader.platdata_, &platdata, sizeof(platdata));
      if (!headers.update(&platheader, present))
      {
	logger.log("Failed to update platform\n");
      }
    }

    // Get beam data here
    if(dcsAuxGetBeamUpdate(beamheader.beamdata_, keystate))
    {
      // Function returns TRUE if there is new data
      // Send new data and update header container

      DCSCom::sendBeamData(server, &beamheader.beamdata_);

      if (!headers.update(&beamheader, present))
      {
	logger.log("Failed to update beam\n");
      }
    }

    // Get gate data here
    if(dcsAuxGetGateUpdate(gateheader.gatedata_, keystate))
    {
      // Function returns TRUE if there is new data
      // Send new data and update header container

      DCSCom::sendGateData(server, &gateheader.gatedata_);

      if (!headers.update(&gateheader, present))
      {
	logger.log("Failed to update gate\n");
      }
    }

    // Get event to change beam state here
    if(dcsAuxGetBeamEvent(beamheader, event, keystate))
    {
      // Function returns TRUE if keyboard input modified the beam state
      // Send event and update header container

      event.id_ = beamheader.id_;
      DCSCom::sendEvent(server, &event);

      if (!headers.update(&beamheader, present))
      {
	logger.log("Failed to update beam\n");
      }
    }

    // Get event to change gate state here
    if(dcsAuxGetGateEvent(gateheader, event, keystate))
    {
      // Function returns TRUE if keyboard input modified the gate state
      // Send event and update header container

      event.id_ = gateheader.id_;
      DCSCom::sendEvent(server, &event);

      if (!headers.update(&gateheader, present))
      {
	logger.log("Failed to update gate\n");
      }
    }

    //
    // Execute traversal to print header contents here
    //
    if(dcsAuxGetCommand(keystate)==DCSAUX_PRINTTRACKS)
    {
      headers.traverse(print_header, (void*)&logger);
    }


    // Check for client requests here
    DCSCom::serverProcessRequests(server, timeheader, scenheader, headers);

    // Check for stale connections here
    if (!server.vagrants.empty())
    {
      // Print information for each closed socket
      DCSCom::ClientList::iterator s;
      for (s = server.vagrants.begin(); s != server.vagrants.end(); s++)
      {
        std::stringstream sstr;
        sstr << "Connection from " << (*s)->getAddress(addr, sizeof(addr)) << " " << (*s)->getPort() << " closed\n";
        logger.log(sstr.str().c_str());
      }

      // Remove all closed sockets
      DCSCom::serverExpelVagrants(server);
    }
  }

  // Stop server here
  DCSCom::serverStop(server);
  logger.log("Shutting down\n");

  // Stop discovery service and clean up resources here
  DCSDisc::discoverFree(discover);

  // Free Winsock DLL resources
  finalizeSocket();

  return 0;
}
