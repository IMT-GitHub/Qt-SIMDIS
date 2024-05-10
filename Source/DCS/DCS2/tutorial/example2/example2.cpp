/*****************************************************************************
* EXAMPLE2
*
* Example 2 adds the DCS locator capability to the server.  This requires the 
* server to listen for and respond to client 'solicitaions' for active DCS 
* servers.  
*
******************************************************************************/

#include <stdio.h>
#include <signal.h>
#include "DCS.h"
#include "DCSaux.h"

#define REFLAT 0 //(22.0*(M_PI/180.0))
#define REFLON 0 //(-159.0*(M_PI/180.0))

// Suspend program for 100 milliseconds to create a 10HZ data rate.  
#define SLEEP 100

int main(int argc, char** argv)
{
  bool success;
  double dtime = DCS::getyeartime();           // Data time
  char addr[INET_ADDRSTRLEN] = "127.0.0.1";    // String for an IP address
  dcsAuxKeyState keystate;                     // Key state for dcsAuxGetKeyState

  // Network info
  short server_port = 4567;
  short data_port   = 5678;
  char  data_addr[INET_ADDRSTRLEN] = "226.100.100.5";
  char  data_type   = DCSDATA_MULTICAST;
  TTL_T mcast_ttl   = 1;

  // DCS info
  DCSCom::DCSServerInfo server;      // DCS server info structure
  DCSHeaderContainer headers;        // Data structure to store Platform, Beam, and Gate data

  DCSScenarioData scendata(0.0, 0.0);  // Scenario wind data
  DCSScenarioHeader scenheader(REFLAT, REFLON, DCS::getyear(), DCSCOORD_ENU, DCSCLASS_UNCLASSIFIED, scendata);  // Initialization parameters for scenario

  DCSTimeData timedata(dtime);                              // Provides updates for time
  DCSTimeHeader timeheader(1.0, DCSTIME_PACKET, timedata);  // Initialization parameters for time

  // DCS Discovery info
  DCSDisc::DCSDiscover discover;

#ifndef WIN32
  // Ignore UNIX SIGPIPE
  signal(SIGPIPE, SIG_IGN);
#endif

  // Windows requires initialization of Winsock DLL
  if (!initializeSocket())
  {
    printf("Error initializing socket\n");
    return 0;
  }

  // Initialize dcs header database here
  headers.initialize("dcsdb.dat");
  headers.setSave(false);           // Delete 'dcsdb.dat' on successful exit.

  //
  // Start discovery service here
  //
  DCS::getdefaultaddr(addr,sizeof(addr));
  DCSDisc::discoverServInit(discover, server_port, addr, "Demonstration server", mcast_ttl, NULL);

  // Start server here
  success = DCSCom::serverStart(server, server_port, data_type, data_port, data_addr, NULL, mcast_ttl);
  if (!success)
  {
    printf("Error starting server: %d\n", server.servsock->getLastError());
    DCSDisc::discoverFree(discover);
    finalizeSocket();
    return 0;
  }

  printf("Starting server.  Press 'h' for help, 'q' to quit.\n");

  // main loop
  for (;;)
  {
    if(dcsAuxGetKeyState(keystate, SLEEP) == DCSAUX_QUIT)
    {
      // Quit application
      break;
    }

    //
    // Provide DCS server IP address and port to any requesting clients
    //
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
          printf("Accepted connection from %s %d\n", client->getAddress(addr, sizeof(addr)), client->getPort());
        }
      }
    }


    // Send time to clients here
    dtime = DCS::getyeartime();               // Current time in seconds since beginning of year
    timeheader.timedata_.time_ = dtime;       // Update header for distribution to clients at connection negotiations
    timedata.time_ = dtime;
    DCSCom::sendTimeData(server, &timedata);


    // Check for client requests here
    DCSCom::serverProcessRequests(server, timeheader, scenheader, headers);

    // Check for stale connections here
    if (!server.vagrants.empty())
    {
      // Print information for each closed socket
      DCSCom::ClientList::iterator s;
      for (s = server.vagrants.begin(); s != server.vagrants.end(); s++)
      {
        printf("Connection from %s %d closed\n", (*s)->getAddress(addr, sizeof(addr)), (*s)->getPort());
      }

      // Remove all closed sockets
      DCSCom::serverExpelVagrants(server);
    }
  }

  // Stop server here
  DCSCom::serverStop(server);
  printf("Shutting down\n");

  //
  // Stop discovery service and clean up resources here
  //
  DCSDisc::discoverFree(discover);

  // Free Winsock DLL resources
  finalizeSocket();

  return 0;
}
