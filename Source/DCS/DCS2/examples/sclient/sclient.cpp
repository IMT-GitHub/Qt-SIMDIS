
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
// sclient - a simple client program implemented from the DCS API

#include <sstream>
#include <stdio.h>
#include <signal.h>
#include "DCS.h"

#ifdef WIN32
#include <conio.h>
#endif

// // // // // // // // // // // // // // // // // // // // // // //
// Character string specifying the network address of the network
// interface to use for data transmission. To use any network
// interface (INADDR_ANY), set to NULL.
#define IFACE NULL

// flag to denote the client's status
int running=1;


// // // // // // // // // // // // // // // // // // // // // // //
// In order to properly stop the client, we set a signal that allows
// the user to stop the process by pressing Ctl C 
void sighandler(int)
{
  running=0;
  printf("Signaled\n");
  return;
}


// // // // // // // // // // // // // // // // // // // // // // //
// main program loop
int main()
{
  std::stringstream sstr;
  //char buffer[256];
  DCSDisc::DCSDiscover discover;
  DCSConsoleLogger logger;
  DCSCom::DCSClientInfo info;
  DCSTimeHeader time;
  DCSScenarioHeader scenario;
  DCSCom::HeaderList headers;
  DCSBaseData* data;

  // Set the signal so we can properly stop the client. 
#ifdef WIN32
  signal(SIGINT,sighandler);
#else
  struct sigaction act;
  act.sa_handler=sighandler;
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

  // Socket initialization status, For the PC, this function calls the
  // WSAStartup function, which must be the first Windows Sockets function
  // called by an application or DLL. It allows an application or DLL
  // to specify the version of Windows Sockets required and to retrieve
  // details of the specific Windows Sockets implementation. The application
  // or DLL can only issue further Windows Sockets functions after a
  // successfully calling WSAStartup.
  if (!initializeSocket())
  {
    char errormsg[256];
    sstr.str("");
    sstr << "Error initializing socket: " << getSockErr() << "\n";
    logger.log(sstr.str().c_str());

    getSockErrString(errormsg, 256);
    logger.log("System error message: ");
    logger.log(errormsg);
    logger.log("\n");
    return 0;
  }

  // First locate a server on our local subnet (1 for TTL is subnet).  
  if(!DCSDisc::discoverClientInit(discover,1,IFACE))
  {
    char errormsg[256];
    sstr.str("");
    sstr << "Error initializing multicast socket for server discovery: " << getSockErr() << "\n";
    logger.log(sstr.str().c_str());

    getSockErrString(errormsg, 256);
    logger.log("System error message: ");
    logger.log(errormsg);
    logger.log("\n");
    return 0;
  }

  DCSDisc::discoverRequest(discover);
  logger.log("Looking for a server\n");
  
  // Wait 2 minutes for the response
  if(!discover.respond->readReady(120,0))
  {
    logger.log("No server found.  Shutting down.\n");
    finalizeSocket();
    return 0;
  }

  DCSDisc::discoverReceive(discover);
  sstr.str("");
  sstr << "Located server at " << discover.addr << " " << discover.port << "\n";
  logger.log(sstr.str().c_str());

  // Establishs a connection to a listening server.
  //
  // Parameters:
  //  info:  a DCSClientInfo structure to be filled with data specific too the
  //         client being created. Resources required by DCSClientInfo will be
  //         allocated by this function. A matching call to clientDisconnect
  //         is required to properly free these resources, when finished.
  //  port:  an unsinged integer specifying the port with which to connect.
  //  addr:  network address of server with which to connect (eg. "192.168.44.5").
  //  iface: interface to connect and receive data through (eg. "127.0.0.1"
  //         for loopback or the ip address of an installed network card).
  //
  // Returns:
  //  true if successful, false if failed. On failure, no resources will have
  //  been allocated, so a call to clientDisconnect is unnecessary.
  if(!DCSCom::clientConnect(info,discover.port,discover.addr,IFACE))
  {
    sstr.str("");
    sstr << "Failed to connect to server: " << getSockErr() << "\n";
    logger.log(sstr.str().c_str());
    DCSDisc::discoverFree(discover);
    finalizeSocket();
    return 1;
  }
  else
  {
    logger.log("Connected to server\n");
  }

  // Negotiates a connection with a server. This function is used to negotiate
  // a client connection that was made by clientConnect(). The negotiation is
  // handled separately from clientConnect for use by clients that are designed
  // to be multithreaded.
  //
  // Parameters:
  //   info:      a DCSClientInfo structure to receive the UDP data mode for 
  //              the connection.
  //   time:      a DCSTimeHeader structure to be received by the client as 
  //              part of the connection negotiation.
  //   scenario:  a DCSScenarioHeader structure to be received by the client
  //              as part of the connection negotiation.
  //   headers:   a HeaderList object to be filled with DCSBaseData derived
  //              objects received by the client as part of the connection
  //              negotiation.
  //   version:   an unsigned short to be filled the DCS protocol version
  //              number supported by the DCS server. If a client negotiaion
  //              fails, check this value to make sure it matches the version
  //              of the DCS protocol employed by the client. (optional)
  //
  // Returns:
  //  true if successful, false if failed. On failure, client connection is
  //  not closed by this function. The application should close the socket
  //  explicitly with a call to clientDisconnect if this function fails. The
  //  contents of the HeaderList, if any, will also have ot be destroyed
  //  explicitly through use of the 'delete' operator.
  if(!DCSCom::clientNegotiate(info,time,scenario,headers))
  {
    sstr.str("");
    sstr << "Failed to negotiate connection: " << getSockErr() << "\n";
    logger.log(sstr.str().c_str());
    // Closes an active connection and frees resources allocated for
    // DCSClientInfo by clientConnect.
    //
    // Parameters:
    //   info:  a DCSClientInfo structure containing data sepcific to the client
    //          to be disconnected. Resources allocated for this structure by
    //          clientConnect will be freed.
    //
    // Returns:
    //  true if successful, false if failed. Failure indicates that one or both
    //  of the data and client sockets did not exist.
    DCSCom::clientDisconnect(info);
    DCSDisc::discoverFree(discover);
    finalizeSocket();
    return 1;
  }
  else
  {
#ifdef WIN32
    cout << "\n\t   Press Q key to quit DCS Client\n" << endl;
#else
    cout << "\n\t Press Ctl-C key to quit DCS Client\n" << endl;
#endif
    
    char addr[DCS_ADDRSTRLEN];
    sstr.str("");
    sstr << "Connection settings: " << info.datatype << " " << info.datasock->getPort() << " " << info.datasock->getAddress(addr,sizeof(addr)) << "\n";
    logger.log(sstr.str().c_str());
    logger.log(&time);
    logger.log(&scenario);

    sstr.str("");
    sstr << "Received " << headers.size() << " platforms\n";
    logger.log(sstr.str().c_str());
    DCSCom::HeaderList::iterator iter=headers.begin();
    for(;iter!=headers.end();iter++)
      logger.log(*iter);
  }

  
  // Make a data requests to server. Initiaites a date request to server.
  // Data sent by server as a result of this request is to be read with
  // the readData function. Prior to using this function the client must
  // be connected to the server and the DCSClientInfo structure initialized
  // through a call to clientConnect.
  //
  // Parameters:
  //   info:     a DCSClientInfo structure specifying the socket with which 
  //             data requests are to be sent.
  //   request:  an unsinged byte specifying the type of data request being
  //             made.
  //
  // Returns:
  //   true if successful, false if failed.
  if (!DCSCom::clientRequest(info,DCSREQUEST_DATAMODE))
  {
    logger.log("DCSREQUEST_DATAMODE failed\n");
  }
  if (!DCSCom::clientRequest(info,DCSREQUEST_TIME))
  {
    logger.log("DCSREQUEST_TIME failed\n");
  }
  if (!DCSCom::clientRequest(info,DCSREQUEST_SCENARIO))
  {
    logger.log("DCSREQUEST_SCENARIO failed\n");
  }
  if (!DCSCom::clientRequest(info,DCSREQUEST_HEADERS))
  {
    logger.log("DCSREQUEST_HEADERS failed\n");
  }
  if (!DCSCom::clientRequest(info,DCSREQUEST_KEEPALIVE))
  {
    logger.log("DCSREQUEST_KEEPALIVE failed\n");
  }

  
  // Get UDP data updates
  while(running)
  {
    // Test server socket
    // All of these socket operations should have error tests.  Make sure
    // your application does.  
    if(info.servsock->readReady(0,5000))
    {
      // See what type of data was received
      int8_t byte;
      int32_t result=info.servsock->read(&byte,sizeof(byte));
      if(result==SOCKET_ERROR||result==0)
      {
	logger.log("Lost connection to server\n");
	running=0;
	continue;
      }

      switch(byte)
      {
	case DCSREQUEST_DATAMODE:
	{
	  // We requested the data mode from the server.
	  // Server sends 1 byte for mode type (Unicast,Broadcast,Multicast).
	  // Followed 2 bytes for port.
	  // Followed by DCS_ADDRSTRLEN bytes as a string containing address.
	  // If the type is Unicast, the server sends it's own address incase
	  // the client would like to 'connect' to that address.  
	  int8_t datatype;
	  uint16_t dataport;
          char dataaddr[DCS_ADDRSTRLEN];
	  info.servsock->readAll(&datatype,sizeof(datatype));
          info.servsock->readAll(&dataport,sizeof(dataport));
          dataport=ntohs(dataport);
          info.servsock->readAll(dataaddr,sizeof(dataaddr));
          sstr.str("");
          sstr << "\nData settings: " << datatype << " " << dataport << " " << dataaddr << "\n";
          logger.log(sstr.str().c_str());
	  break;
	}
	case DCSREQUEST_TIME:
	  // We requested the time header.
	  data=DCSCom::readTCPData(info);
	  logger.log("\nNew Time Header: \n");
	  logger.log(data);
	  delete data;
	  break;
	case DCSREQUEST_SCENARIO:
	  // We requested the scenario header.
	  data=DCSCom::readTCPData(info);
	  logger.log("\nNew Scenario Header: \n");
	  logger.log(data);
	  delete data;
	  break;
	case DCSREQUEST_EVENT:
	  // The server sent us an event.
	  data=DCSCom::readTCPData(info);
	  logger.log("\nServer sent Event: \n");
	  logger.log(data);
	  delete data;
	  break;
	case DCSREQUEST_HEADER:
	  // The server sent us a new header.
	  data=DCSCom::readTCPData(info);
	  logger.log("\nServer sent new Header: \n");
	  logger.log(data);
	  delete data;
	  break;
	case DCSREQUEST_HEADERS:
	{
	  // Server is responding to our request for the headers.
	  // It first sends the total number of headers.
	  // Then sends each header.
	  int32_t total;
	  
          info.servsock->readAll(&total,sizeof(total));
          total=ntohl(total);

	  logger.log("\nServer sent new Headers: \n");
          while(total-->0)
          {
	    data=DCSCom::readTCPData(info);
	    logger.log(data);
	    delete data;
	  }
	  break;
        }
	case DCSREQUEST_DISCONNECT:
	  logger.log("\nServer sent disconnect\n");
	  running=0;
	  break;
	case DCSREQUEST_KEEPALIVE:
	  // A response to our probe.  Server is still there.
	  logger.log("\nServer is alive\n");
	  break;
      }
    }
    
    // Test Data socket - a real application would probably want to read
    // more than one at a time.
    int limit=100;
    while(info.datasock->readReady(0,5000)&&--limit>0)
    {
      // We have something
      data=DCSCom::readUDPData(info);

      // Make sure it wasn't junk on the network that we want to ignore.  
      if(data)
      {
        logger.log(data);
        delete data;
      }
    }
    
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

  printf("Disconnecting\n");
  // Closes an active connection and frees resources allocated for
  // DCSClientInfo by clientConnect.
  //
  // Parameters:
  //   info:  a DCSClientInfo structure containing data sepcific to the client
  //          to be disconnected. Resources allocated for this structure by
  //          clientConnect will be freed.
  //
  // Returns:
  //  true if successful, false if failed. Failure indicates that one or both
  //  of the data and client sockets did not exist.
  DCSCom::clientDisconnect(info);
  
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
