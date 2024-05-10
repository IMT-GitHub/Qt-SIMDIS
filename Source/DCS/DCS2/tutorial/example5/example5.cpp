/*****************************************************************************
* EXAMPLE5
*
* Example 5 replaces the DCS function calls with the DCS middleware server 
* class.  
*
******************************************************************************/

#include <stdio.h>
#include <signal.h>
#include <memory.h>
#include "DCS.h"
#include "DCSServer.h"
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

int main(int argc, char** argv)
{
  bool beamon = true;
  bool gateon = true;
  double dtime = DCS::getyeartime();           // Data time
  char addr[INET_ADDRSTRLEN] = "127.0.0.1";    // String for an IP address
  dcsAuxKeyState keystate;                     // Key state for dcsAuxGetKeyState

  // Network info
  short server_port = 4567;
  short data_port   = 5678;
  char  data_addr[INET_ADDRSTRLEN] = "226.100.100.5";
  char  data_type   = DCSDATA_MULTICAST;
  TTL_T mcast_ttl   = 1;

  // Data points for all objects
  DCSPlatformData platdata;
  DCSBeamData beamdata;
  DCSGateData gatedata;

  // DCS logger
  DCSConsoleLogger logger;

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

  //
  // Start server here
  //
  DCSServer* server = new DCSServer("Demonstration server", data_type, server_port, data_port, data_addr, NULL, mcast_ttl);
  server->start();

  //
  // Set expiration time for non-updating tracks here
  //
  server->setExpireTimeLimit(300.0);

  //
  // Set scenario information here
  //
  server->sendScenarioHeader(DCSCOORD_NED, DCSCLASS_UNCLASSIFIED, 
                             DCS::getyear(), REFLAT, REFLON, dtime, DCSTIME_PACKET);

  //
  // Setup the platform, beam, and gate headers/data here
  //
  platdata.id_ = DCS::make_id(0, 0, PLATFORM, 1);
  platdata.time_ = dtime;
  platdata.position_.set(0.0, 0.0, -100.0);
  platdata.orientation_.set();              // Default to 0
  platdata.velocity_.set();                 // Default to 0
  platdata.accel_.set();                 // Default to 0

  beamdata.id_ = DCS::make_id(0, 0, BEAM, 1);
  beamdata.time_ = dtime;
  beamdata.azim_ = 0.0;
  beamdata.elev_ = 15.0 * (PI / 180.0);
  beamdata.length_ = 200.0;

  gatedata.id_ = DCS::make_id(0, 0, GATE, 1);
  gatedata.time_ = dtime;
  gatedata.azim_ = 0.0;
  gatedata.elev_ = 15.0 * (PI / 180.0);
  gatedata.width_ = 3.0 * (PI / 180.0);
  gatedata.height_ = 3.0 * (PI / 180.0);
  gatedata.minrange_ = 180.0;
  gatedata.maxrange_ = 200.0;
  gatedata.centroid_ = 190.0;

  server->sendPlatformHeader(platdata.id_,
                             "UAV", 
                             "usaf_uav", 
                             DCSPLATFORM_AIRCRAFT, 
                             DCSALLEGIENCE_FRIENDLY,
                             platdata.time_,
                             platdata.position_.x,
                             platdata.position_.y,
                             platdata.position_.z,
                             platdata.orientation_.x,
                             platdata.orientation_.y,
                             platdata.orientation_.z,
                             platdata.velocity_.x,
                             platdata.velocity_.y,
                             platdata.velocity_.z,
                             platdata.accel_.x,
                             platdata.accel_.y,
                             platdata.accel_.z);

  server->sendBeamHeader(beamdata.id_, 
                         platdata.id_, 
                         "UAV Beam", 
                         3 * (PI / 180.0), 
                         3 * (PI / 180.0), 
                         -1, 
                         0x0000FFFF, 
                         0.0, 
                         0.0, 
                         5.0, 
                         0.0, 
                         1.0, 
                         DCSBEAM_LINEAR, 
                         beamdata.time_, 
                         beamdata.azim_, 
                         beamdata.elev_, 
                         beamdata.length_);

  server->sendGateHeader(gatedata.id_, 
                         beamdata.id_, 
                         "UAV Gate", 
                         0x00FFFFFF, 
                         0.0, 
                         0.0, 
                         0.0, 
                         DCSGATE_RANGE, 
                         gatedata.time_, 
                         gatedata.azim_, 
                         gatedata.elev_, 
                         gatedata.width_, 
                         gatedata.height_, 
                         gatedata.minrange_, 
                         gatedata.centroid_, 
                         gatedata.maxrange_);

  logger.log("Starting server.  Press 'h' for help, 'q' to quit.\n");

  // main loop
  for (;;)
  {
    if(dcsAuxGetKeyState(keystate,SLEEP) == DCSAUX_QUIT)
    {
      // Quit application
      break;
    }

    //
    // Get platform data here
    //
    if(dcsAuxGetPlatformUpdate(platdata, keystate))
    {
      server->sendPlatformData(platdata.id_,
 	 		       platdata.time_, 
                               platdata.position_.x,
                               platdata.position_.y,
                               platdata.position_.z,
                               platdata.orientation_.x,
                               platdata.orientation_.y,
                               platdata.orientation_.z,
                               platdata.velocity_.x,
                               platdata.velocity_.y,
                               platdata.velocity_.z,
                               platdata.accel_.x,
                               platdata.accel_.y,
                               platdata.accel_.z);
    }

    //
    // Get beam data here
    //
    if(dcsAuxGetBeamUpdate(beamdata, keystate))
    {
      server->sendBeamData(beamdata.id_,
                           beamdata.time_, 
                           beamdata.azim_, 
                           beamdata.elev_, 
                           beamdata.length_);
    }

    //
    // Get gate data here
    //
    if(dcsAuxGetGateUpdate(gatedata, keystate))
    {
      server->sendGateData(gatedata.id_, 
                           gatedata.time_, 
                           gatedata.azim_, 
                           gatedata.elev_, 
                           gatedata.width_, 
                           gatedata.height_, 
                           gatedata.minrange_, 
                           gatedata.centroid_, 
                           gatedata.maxrange_);
    }

    //
    // Get event to change beam state here
    //
    if(dcsAuxGetCommand(keystate) == DCSAUX_BEAMTOGGLE)
    {
      beamon = !beamon;
      server->sendStateEvent(beamdata.id_, beamon, DCS::getyeartime());
    }

    //
    // Get event to change gate state here
    //
    if(dcsAuxGetCommand(keystate) == DCSAUX_GATETOGGLE)
    {
      gateon = !gateon;
      server->sendStateEvent(gatedata.id_, gateon, DCS::getyeartime());
    }

    //
    // Send time to clients here
    //
    dtime = DCS::getyeartime();               // Current time in seconds since beginning of year
    server->sendTime(dtime);

    //
    // Process server events (locator response, client connection, etc) here
    //
    server->idle(dtime);
  }

  //
  // Stop server here
  //
  delete server;
  logger.log("Shutting down\n");

  // Free Winsock DLL resources
  finalizeSocket();

  return 0;
}
