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

#ifndef _DCSCOM_H_
#define _DCSCOM_H_

#include "DCSdefs.h"
#include "Constants/ConstantsUnits.h"
#include "Constants/ConstantsVariableType.h"
#include "sockdefs.h"

class SockInet;
class SockUDP;
class SockClient;
class SockServer;
class DCSBaseData;
class DCSBaseHeader;
class DCSBeamHeader;
class DCSBeamData;
class DCSCategoryData;
class DCSCoordReferenceFrame;
class DCSEvent;
class DCSGateHeader;
class DCSGateData;
class DCSGenericData;
class DCSHeaderContainer;
class DCSPlatformHeader;
class DCSPlatformData;
class DCSScenarioHeader;
class DCSScenarioData;
class DCSScopeData;
class DCSTimeHeader;
class DCSTimeData;

struct DCSPoint;
struct DCSScopeGateData;

///DCS namespace.
/**
 * Namespace defining functions for basic connection and communication with DCS server and clients.
 */
namespace DCSCom
{
  ///Client list for server.
  /**
   * A list of addresses for SockClient objects representing client connections to the server.
   * @internal
   */
  typedef std::vector<SockClient*> ClientList;

  ///Header list for client.
  /**
   * A list of headers retrieved from the server during a connection negotiation.
   */
  typedef std::vector<DCSBaseData*> HeaderList;

  ///Client info record.
  /**
   * Stores information relevant to a client connection.
   */
  struct DCSClientInfo
  {
    public:
      ///Data transport type.  
      int8_t datatype;		       /**< The type of UDP socket: broadcast, mulitcast, or unicast.
				        * @see @ref modes "Data Modes".
				        */
      ///Unique server ID.  
      char server_id[DCS_SERVERIDSZ];  /**< Unique ID to be used for filtering data sent to clients.
                                        *   With this, multiple servers can send data on the same address,
                                        *   and the client can ignore any data from the unknown servers, by
                                        *   only processing data that has the proper server_id value.  
                                        */
      ///UDP connection.  
      SockUDP* datasock;	       /**< UDP socket for receiveing data updates.  */
      ///TCP connection.  
      SockClient* servsock;	       /**< TCP connection to the server.  */
      ///Server version
      uint16_t serverVersion;	       /**< Server version number */

      ///Default client info constructor.
      /**
       * DCSClientInfo constructor to initalize data members to default values.
       */
      DCSClientInfo():datatype(DCSDATA_UNCONNECTED),datasock(NULL),servsock(NULL),serverVersion(0){}
  };

  ///Server info record.
  /**
   * Stores information relevant to a server.
   */
  struct DCSServerInfo
  {
    public:
      ///Data transport type.  
      int8_t datatype;		       /**< The type of UDP socket: broadcast, mulitcast, or unicast.
				        * @see @ref modes "Data Modes".
				        */
      ///Unique server ID.  
      char server_id[DCS_SERVERIDSZ];  /**< Unique ID to be used for filtering data sent to clients.
                                        *   With this, multiple servers can send data on the same address,
                                        *   and the client can ignore any data from the unknown servers, by
                                        *   only processing data that has the proper server_id value.  
                                        */
      //UDP data port for Unicast socket.  
      int16_t dataport;		       /**< The UDP data port for sending data via Unicast.  */
      ///UDP connection.  
      SockUDP* datasock;	       /**< UDP socket for sending data updates.  */
      ///TCP server socket.  
      SockServer* servsock;	       /**< TCP server socket for receiving connections from clients and for processing client requests.  */
      ///TCP client list.
      ClientList clientlist;           /**< List of TCP client sockets connected to server.  */
      ///TCP closed connection list.
      mutable ClientList vagrants;     /**< List of TCP client sockets whose connections have been closed.  If one of the server functions detects a client connection has closed it will add a reference for that socket to this list so that the application may properly dispose of the client sockets.  */
      ///Server version
      uint16_t serverVersion;	       /**< Server version number */

      ///Default server info constructor.
      /**
       * DCSServerInfo constructor to initalize data members to default values.  
       */
      DCSServerInfo():datatype(DCSDATA_UNCONNECTED),datasock(NULL),servsock(NULL),serverVersion(0){}
  };

  ///Retrieve server id from a server record.  
  /**
   * Retrieve ID for server defined by the specified server info record.  
   * @param info a DCSServerInfo structure from which the id will be retrieved.  
   * @param id a character buffer to be filled with the server id.  
   * @param len an integer specifying the size of the 'id' parameter.  The length should be at least DCS_SERVERIDSZ.  
   * @return a pointer to the 'id' parameter.  
   */
  char* getServerID(const DCSServerInfo& info,char* id,int32_t len);

  ///Retrieve server id from a client record.  
  /**
   * Retrieve ID for the server associated with the client defined by the specified client info record.  
   * @param info a DCSClientInfo structure from which the id will be retrieved.  
   * @param id a character buffer to be filled with the server id.  
   * @param len an integer specifying the size of the 'id' parameter.  The length should be at least DCS_SERVERIDSZ.  
   * @return a pointer to the 'id' parameter.  
   */
  char* getServerID(const DCSClientInfo& info,char* id,int32_t len);

  ///Retrieve server id from a server record as a human readable string.  
  /**
   * Retrieve ID for server defined by the specified server info record as a human readable string.  
   * @param info a DCSServerInfo structure from which the id will be retrieved.  
   * @param str a character buffer to be filled with the server id string.  
   * @param len an integer specifying the size of the 'str' parameter.  
   * @return a pointer to the 'str' parameter.  
   */
  char* getServerIDString(const DCSServerInfo& info,char* str,int32_t len);

  ///Retrieve server id from a client record as a human readable string.  
  /**
   * Retrieve ID for the server associated with the client defined by the specified client info record as a human readable string.  
   * @param info a DCSClientInfo structure from which the id will be retrieved.  
   * @param str a character buffer to be filled with the server id string.  
   * @param len an integer specifying the size of the 'str' parameter.  
   * @return a pointer to the 'str' parameter.  
   */
  char* getServerIDString(const DCSClientInfo& info,char* str,int32_t len);

  //Server operations - operations for providing data to clients such as Simdis

  ///Server creation/initialization.  
  /**
   * Creates a listening server.  Allocates some resources for
   * fields of the DCSServerInfo structure.
   * @param info a DCSServerInfo structure to be filled with data specific too the server being started.  Resources required
   * by DCSServerInfo will be allocated by this function.  A matching call to serverStop is required to properly free these resources, when finished.
   * @param servport an unsinged integer specifying the port that the server should listen on.
   * @param datamode a byte specifying the UDP mode: Broadcast, Multicast, or unicast.  If this value is DCSDATA_UNICAST, dataaddr is ignored.  
   * @param dataport an unsinged integer specifying the port that the server should transmit UDP data on.
   * @param dataaddr a string specifying the UDP address for transimitting data.  If datamode is DCSDATA_UNICAST, this value is ignored.  
   * @param iface an optional argument that may be used to specify the interface on which the server should listen and transmit data.
   * @param ttl a value specifying the TTL for the Multicast UDP mode.  If datamode is not set to DCSDATA_MCAST, this value is ignored.  
   * The interface is specified as the numerical IP address of the network device to be used for transmitting data (eg. "127.0.0.1" for loopback or the ip address of an installed network card).  This
   * will cause the server to only receive and send on this device.  Not specifying an interface allows the kernel to determine the proper interface to use.
   * @return true if successful, false if failed.  On failure, no resources will have been allocated, so a call to serverStop is unnecessary.  
   * @see @ref modes "Data Modes"
   * @see serverStop()
   */
  bool serverStart(DCSServerInfo& info,uint16_t servport,int8_t datamode,uint16_t dataport,const char* dataaddr,const char* iface=NULL,TTL_T ttl=1);

  ///Server shutdown.  
  /**
   * Shuts down server and frees resources allocated for DCSServerInfo by serverStart
   * @param info a DCSServerInfo structure containing data sepcific to the server to be stopped.  Resources allocated for this structure by serverStart will be freed.  
   * @see serverStart()
   */
  void serverStop(DCSServerInfo& info);

  ///Accept connections from clients.
  /**
   * Accepts a client connection, creating a new SockClient object for the client.  Prior to using this function
   * the server must be started and the DCSServerInfo structure initialized through a call to serverStart.
   * A new client connection must processed with a call to serverNegotiate.  
   * @param info a DCSServerInfo structure specifying the server socket to accept connections with.
   * @return NULL if unsuccessful or a pointer to a SockClient object for the newly accepted connection if successful.  
   * @see serverStart()
   */
  SockClient* serverAccept(const DCSServerInfo& info);

  ///Negotiate a new client connection.
  /**
   * Negotiates a connection with a newly accepeted client.  This function is used to process a client that was accepted
   * by serverAccept().  The negotiation is handled separately from serverAccept for use by servers that are designed to be multithreaded.
   * With the accept and negotiate functions being handled separately, a new thread can be spawned everytime a client is received by serverAccept.
   * This new thread will call serverNegotiate and handle any future client requests.  This way the thread responsible for accepting connections can
   * accept a new client and go right back to listening for new connections without having to take the time to process the newly accepted connection.
   * Special care must be taken with this function when used in multithreaded servers.  It is very important that the DCSHeaderContainer not be modified
   * while the negotiation is taking place.  Because DCSHeaderContainer is not itself re-entrant, the server application will be required to add the approproate
   * mutual exclusion locks for reads and writes to the DCSHeaderContainer database.
   * The client socket will be added to the client list found in DCSServerInfo if the negotiation is successful.  
   * @param info a DCSServerInfo structure specifying the UDP data mode for the connection.  
   * @param client a newly accepted SockClient that needs to be connected to the server.
   * @param time a DCSTimeHeader structure to be sent to the new client as part of the connection negotiation.  
   * @param scenario a DCSScenarioHeader structure to be sent to the new client as part of the connection negotiation.
   * @param headers a DCSHeaderContainer providing the headers to be sent to the new client as part of the connection negotiation.  
   * @return true if successful, false if failed.  On success, the socket is added to the client list found in DCSServerInfo.
   * On failure, client connection is not closed by this function.  The application should close the connection
   * explicitly with a call to serverTerminateClient if this function fails.  serverTerminateClient will free the resources allocated by serverAccept.  
   * @see serverAccept()
   * @see serverTerminateClient()
   */
  bool serverNegotiate(DCSServerInfo& info,SockClient* client,const DCSTimeHeader& time,const DCSScenarioHeader& scenario,const DCSHeaderContainer& headers);

  ///Send information for UDP data transmission.  
  /**
   * Sends the UDP type, address, and port number to be used for UDP data communications.  This information is sent via TCP.  
   * The serverNegotiate function uses this function for transmitting data communication settings to the client.  
   * @param info a DCSServerInfo structure containing the settings for UDP type, address, and port to be used for data transmission.
   * @param client a SockClient with which the data is to be sent.  
   * @return true if successful, false if failed.  On failure, client connection is not closed by this function.  The application should close the socket
   * explicitly with a call to serverTerminateClient if this function fails.  
   * @see serverNegotiate()
   * @see serverTerminateClient()
   */
  bool serverSendDataParams(const DCSServerInfo& info,const SockClient* client);

  ///Send time header to a client.  
  /**
   * Sends the DCSTimeHeader to the client.  This information is sent via TCP.  
   * The serverNegotiate function uses this function for transmitting the time header to the client.  
   * @param client a SockClient with which the time header is to be sent.
   * @param time a DCSTimeHeader structure to be sent to the client.  
   * @return true if successful, false if failed.  On failure, client connection is not closed by this function.  The application should close the socket
   * explicitly with a call to serverTerminateClient if this function fails.  
   * @see serverNegotiate()
   * @see serverTerminateClient()
   */
  bool serverSendTime(const SockClient* client,const DCSTimeHeader& time);

  ///Send scenario header to a client.  
  /**
   * Sends the DCSScenarioHeader to the client.  This information is sent via TCP.  
   * The serverNegotiate function uses this function for transmitting the scenario header to the client.  
   * @param client a SockClient with which the scenario header is to be sent.
   * @param scenario a DCSScenarioHeader structure to be sent to the client.  
   * @return true if successful, false if failed.  On failure, client connection is not closed by this function.  The application should close the socket
   * explicitly with a call to serverTerminateClient if this function fails.  
   * @see serverNegotiate()
   * @see serverTerminateClient()
   */
  bool serverSendScenario(const SockClient* client,const DCSScenarioHeader& scenario);

  ///Send all known platform headers to a client.
  /**
   * Sends the headers stored in a DCSHeaderContainer to the client.  This information is sent via TCP.  
   * The serverNegotiate function uses this function for transmitting the headers to the client.  
   * @param client a SockClient with which the headers are to be sent.  
   * @param headers a DCSHeaderContainer structure containing headers to be sent to the client.  
   * @return true if successful, false if failed.  On failure, client connection is not closed by this function.  The application should close the socket
   * explicitly with a call to serverTerminateClient if this function fails.  
   * @see serverNegotiate()
   * @see serverTerminateClient()
   */
  bool serverSendHeaders(const SockClient* client,const DCSHeaderContainer& headers);

  ///Function for DCSHeaderContainer to send a header to a client.
  /**
   * Sends a single header to a client.  This function is supplied as an argument to the
   * traverse method provided by DCSHeaderContainer.  It is used by serverNegotiate for distributing headers.
   * @param header a DCSBaseData derived object to be sent to the client.
   * @param data a pointer to the socket to be used for transmitting the header.
   * @internal
   * @see DCSHeaderContainer::traverse()
   * @see serverNegotiate()
   */
  void serverHCFunc(const DCSBaseHeader* header,void* data);

  ///Terminate a client connection.
  /**
   * Closes a client connection, removes the client socket from the client list
   * maintained by DCSServerInfo, and frees resources allocated for the SockClient by serverAccept.
   * Should be used for closing all sockets.  A socket should never be closed and deleted directly by user code.
   * @param info a DCSServerInfo structure containing the client list that the client socket is to be removed from.
   * @param client a SockClient object specifying the client socket to be terminated.
   * @return true if successful, false if failed.  Failure indicates that the socket pointer was NULL or was not present in the client socket list.  
   */
  bool serverTerminateClient(DCSServerInfo& info,SockClient* client);
  
  ///Terminates all client connections.
  /**
   * Closes all client connections, removes the client sockets from the client list
   * maintained by DCSServerInfo, and frees resources allocated for each SockClient by serverAccept.
   * Should be used for closing all sockets.  A socket should never be closed and deleted directly by user code.
   * @param info a DCSServerInfo structure containing the client list that the client sockets are to be removed from.
   * @return true if successful, false if failed.  Failure indicates that the socket pointer was NULL or was not present in the client socket list.  
   */
  bool serverTerminateAllClients(DCSServerInfo& info);

  ///Clear out any sockets that appear in the vagrant list.
  /**
   * Removes the vagrant client sockets from the client and vagrant lists
   * maintained by DCSServerInfo, and frees resources allocated for the SockClient by serverAccept.
   * It's a good idea to use this function whenever a failed connection is detected, in order to keep the contents
   * of the client and vagrant lists maintained by DCSServerInfo in sync with each other.
   * @param info a DCSServerInfo structure containing the client list and the vagrant list that the vagrant sockets are to be removed from.
   * @return true if successful, false if failed.  
   */
  bool serverExpelVagrants(DCSServerInfo& info);

  ///Receive request from client.
  /**
   * Read a data request from a client.  Prior to calling this function, the client socket must be accepted by a call to serverAccept.  
   * @param client a SockClient object specifying the socket from which data request is being received.
   * @return client request code if successful, -1 if failed.
   * @see @ref requests "Client Requests"
   * @see serverAccept()
   */
  int8_t serverReadRequest(const SockClient* client);

  ///Process any requests from all clients in client list.
  /**
   * Test each client in the client list for a pending request, and, if a request is
   * pending, process that request.  This function is rather simple, and is provided only as a convenience
   * for processing client requests.  If a more detailed request handler is required (eg. needs to log requests, etc)
   * this function can easily be extended.  Note that this function will add vagrant clients to the vagrant list, but will not expel them.  
   * @param info a DCSServerInfo structure containing the client list that the client sockets to process.
   * @param time a DCSTimeHeader structure to be sent to a client upon request.  
   * @param scenario a DCSScenarioHeader structure to be sent to a client upon request.  
   * @param headers a DCSHeaderContainer providing the headers to be sent to a client upon request.  
   */
  void serverProcessRequests(DCSServerInfo& info,DCSTimeHeader& time,DCSScenarioHeader& scenario,DCSHeaderContainer& headers);

  ///Send a platform header to the client.
  /**
   * Send a DCSPlatformHeader object, created from the specified values, from the server to the client.   Platform headers are transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param id an unsigned integer specifying the object's id.  
   * @param iconname a string specifying the icon name.  
   * @param callsign a string specifying the call sign.
   * @param dims a DCSPoint structure specifying the icon dimensions.
   * @param offset a DCSPoint structure specifying the platform's offset from it's specified position.  
   * @param color an unsigned 32-bit integer specifying the platform's color in RRGGBBAA hexadecimal(00-FF) notation: RR=red, GG=green, BB=blue, AA=alpha component.  
   * @param state a byte specifying the platform's DCS state: no change(0), on(1), off(2), expired(3)  
   * @param status a byte specifying the platform's current status from the server: dropped(0), live(1)
   * @param interpolate Indicates whether position and orientation should be interpolated between data points
   * @param refFrame Coordinate reference frame of the platform's data.
   * @param platdata a DCSPlatformData object providing an initial data set for the object associated with this header.  
   * @return true if successful, false if failed.  
   */
  bool sendPlatformHeader(const DCSServerInfo& info,uint64_t id,const char* iconname,const char* callsign,
			  const DCSPoint& dims,const DCSPoint& offset,
			  uint32_t color,DCSObjectState_t state,DCSTrackStatus_t status,bool interpolate,
			  const DCSCoordReferenceFrame& refFrame,
			  const DCSPlatformData& platdata);

  ///Send a platform header to the client. (Deprecated)
  /**
   * Send a DCSPlatformHeader object, created from the specified values, from the server to the client.   Platform headers are transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param id an unsigned integer specifying the object's id.  
   * @param iconname a string specifying the icon name.  
   * @param callsign a string specifying the call sign.
   * @param dims a DCSPoint structure specifying the icon dimensions.
   * @param offset a DCSPoint structure specifying the platform's offset from it's specified position.  
   * @param latorigin a double specifying the latitude topo origin.
   * @param lonorigin a double specifying the longitude topo origin.
   * @param color an unsigned 32-bit integer specifying the platform's color in RRGGBBAA hexadecimal(00-FF) notation: RR=red, GG=green, BB=blue, AA=alpha component.  
   * @param state a byte specifying the platform's DCS state: no change(0), on(1), off(2), expired(3)  
   * @param status a byte specifying the platform's current status from the server: dropped(0), live(1)
   * @param coordsystem a byte specifying the coordinate system.
   * @param allegience a byte specifying the allegience of the platform.  
   * @param plattype a byte specifying the platform type.  
   * @param platdata a DCSPlatformData object providing an initial data set for the object associated with this header.  
   * @return true if successful, false if failed.  
   */
  bool sendPlatformHeader(const DCSServerInfo& info,uint64_t id,const char* iconname,const char* callsign,const DCSPoint& dims,const DCSPoint& offset,
			  double latorigin,double lonorigin,uint32_t color,DCSObjectState_t state,DCSTrackStatus_t status,DCSCoordSystem_t coordsystem,DCSAllegience_t allegience,DCSPlatformType_t plattype,const DCSPlatformData& platdata);

  ///Send platform data to the client.
  /**
   * Send a DCSPlatformData object, created from the specified values, from the server to the client.   Platform data is transmitted via the UDP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param id an unsigned integer specifying the object's id.  
   * @param position a DCSPoint specifying the object's position.  
   * @param orientation a DCSPoint specifying the object's orientation.  
   * @param velocity a DCSPoint specifying the object's velocity.  
   * @param accel a DCSPoint specifying the object's acceleration.  
   * @param time a double specifying the time at which the data is valid.  
   * @return true if successful, false if failed.  
   */
  bool sendPlatformData(const DCSServerInfo& info,uint64_t id,const DCSPoint& position,const DCSPoint& orientation,const DCSPoint& velocity,const DCSPoint& accel,double time);

  ///Send a beam header to the client.
  /**
   * Send a DCSBeamHeader object, created from the specified values, from the server to the client.   Beam headers are transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param id an unsigned integer specifying the object's id.  
   * @param callsign a string specifying the call sign.  
   * @param offset a DCSPoint specifying the beam's offset from the center of the host platform.  
   * @param azimoffset a double specifying the azimuth offset.  
   * @param elevoffset a double specifying the elevation offset.  
   * @param missileOffset Applies an offset to the beam to eminate from edge of icon, useful for missiles
   * @param hbw a double specifying the horizontal beam width.  
   * @param vbw a double specifying the vertical beam width.  
   * @param hostid an integer specifying the id of the platform with which the beam is attached.
   * @param targetid an integer specifying the id of the target to which the beam is tracking.
   * @param color an unsigned 32-bit integer specifying the beam's color in RRGGBBAA hexadecimal(00-FF) notation: RR=red, GG=green, BB=blue, AA=alpha component.  
   * @param state a byte specifying the beam's DCS state: no change(0), on(1), off(2), expired(3)  
   * @param beamtype a byte specifying the beam type.  
   * @param interpolate Indicates whether position should be interpolated between data points
   * @param beamdata a DCSBeamData object providing an initial data set for the object associated with this header.  
   * @return true if successful, false if failed.  
   */
  bool sendBeamHeader(const DCSServerInfo& info,uint64_t id,const char* callsign,const DCSPoint& offset,
		      double azimoffset,double elevoffset,bool missileOffset,double hbw,double vbw,int32_t hostid,int32_t targetid,
		      uint32_t color,DCSObjectState_t state,DCSBeamType_t beamtype,bool interpolate,const DCSBeamData& beamdata);

  ///Send a beam header to the client. (deprecated)
  /**
   * Send a DCSBeamHeader object, created from the specified values, from the server to the client.   Beam headers are transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param id an unsigned integer specifying the object's id.  
   * @param callsign a string specifying the call sign.  
   * @param offset a DCSPoint specifying the beam's offset from the center of the host platform.  
   * @param azimoffset a double specifying the azimuth offset.  
   * @param elevoffset a double specifying the elevation offset.  
   * @param hbw a double specifying the horizontal beam width.  
   * @param vbw a double specifying the vertical beam width.  
   * @param hostid an integer specifying the id of the platform with which the beam is attached.
   * @param targetid an integer specifying the id of the target to which the beam is tracking.
   * @param color an unsigned 32-bit integer specifying the beam's color in RRGGBBAA hexadecimal(00-FF) notation: RR=red, GG=green, BB=blue, AA=alpha component.  
   * @param state a byte specifying the beam's DCS state: no change(0), on(1), off(2), expired(3)  
   * @param beamtype a byte specifying the beam type.  
   * @param beamdata a DCSBeamData object providing an initial data set for the object associated with this header.  
   * @return true if successful, false if failed.  
   */
  bool sendBeamHeader(const DCSServerInfo& info,uint64_t id,const char* callsign,const DCSPoint& offset,
		      double azimoffset,double elevoffset,double hbw,double vbw,int32_t hostid,int32_t targetid,uint32_t color,DCSObjectState_t state,DCSBeamType_t beamtype,const DCSBeamData& beamdata);

  ///Send beam data to the client.
  /**
   * Send a DCSBeamData object, created from the specified values, from the server to the client.   Beam data is transmitted via the UDP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param id an unsigned integer specifying the object's id.  
   * @param time a double specifying the time at which the data is valid.
   * @param azim a double specifying the beam's azimuth.  
   * @param elev a double specifying the beam's elevation.  
   * @param length a double specifying the beam's length.  
   * @return true if successful, false if failed.  
   */
  bool sendBeamData(const DCSServerInfo& info,uint64_t id,double time,double azim,double elev,double length);

  ///Send a gate header to the client.
  /**
   * Send a DCSGateHeader object, created from the specified values, from the server to the client.   Gate headers are transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param id an unsigned integer specifying the object's id.  
   * @param callsign a string specifying the call sign.  
   * @param offset a DCSPoint specifying the gate's offset from the centroid's position.  
   * @param hostid an integer specifying the id of the associated beam.
   * @param color an unsigned 32-bit integer specifying the gate's color in RRGGBBAA hexadecimal(00-FF) notation: RR=red, GG=green, BB=blue, AA=alpha component.  
   * @param state a byte specifying the gate's DCS state: no change(0), on(1), off(2), expired(3)  
   * @param gatetype a byte specifying the gate type.  
   * @param interpolate Indicates whether position should be interpolated between data points
   * @param gatedata a DCSGateData object providing an initial data set for the object associated with this header.  
   * @return true if successful, false if failed.  
   */  
  bool sendGateHeader(const DCSServerInfo& info,uint64_t id,const char* callsign,const DCSPoint& offset,int32_t hostid,uint32_t color,DCSObjectState_t state,DCSGateType_t gatetype,bool interpolate,const DCSGateData& gatedata);

  ///Send a gate header to the client. (deprecated)
  /**
   * Send a DCSGateHeader object, created from the specified values, from the server to the client.   Gate headers are transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param id an unsigned integer specifying the object's id.  
   * @param callsign a string specifying the call sign.  
   * @param offset a DCSPoint specifying the gate's offset from the centroid's position.  
   * @param hostid an integer specifying the id of the associated beam.
   * @param color an unsigned 32-bit integer specifying the gate's color in RRGGBBAA hexadecimal(00-FF) notation: RR=red, GG=green, BB=blue, AA=alpha component.  
   * @param state a byte specifying the gate's DCS state: no change(0), on(1), off(2), expired(3)  
   * @param gatetype a byte specifying the gate type.  
   * @param gatedata a DCSGateData object providing an initial data set for the object associated with this header.  
   * @return true if successful, false if failed.  
   */  
  bool sendGateHeader(const DCSServerInfo& info,uint64_t id,const char* callsign,const DCSPoint& offset,int32_t hostid,uint32_t color,DCSObjectState_t state,DCSGateType_t gatetype,const DCSGateData& gatedata);

  ///Send gate data to the client.
  /**
   * Send a DCSGateData object, created from the specified values, from the server to the client.   Gate data is transmitted via the UDP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param id an unsigned integer specifying the object's id.  
   * @param time a double specifying the time at which the data is valid.
   * @param azim a double specifying the gate's azimuth.
   * @param elev a double specifying the gate's elevation.  
   * @param width a double specifying the gate's width.  
   * @param height a double specifying the gate's height.  
   * @param minrange a double specifying the gate's start distance.  
   * @param maxrange a double specifying the gate's end distance.  
   * @param centroid a double specifying the gate's centroid distance.  
   * @return true if successful, false if failed.  
   */
  bool sendGateData(const DCSServerInfo& info,uint64_t id,double time,double azim,double elev,double width,double height,
		    double minrange,double maxrange,double centroid);


  ///Send scope data to the client.
  /**
   * Send a DCSScopeData object, created from the specified values, from the server to the client.   Scope data is transmitted via the UDP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param id an unsigned integer specifying the object's id.
   * @param xUnits an int16 specifying the object's x-units.
   * @param yUnits an int16 specifying the object's y-units.
   * @param time a double specifying the object's time.
   * @param xStart a double specifying the start of the object's x values.
   * @param xStep a double specifying the object's x step.
   * @param yBias a double specifying the object's y bias.
   * @param yScale a double specifying the object's y scale.
   * @param numDataPoints an int32 specifying the number of data points in the data array.
   * @param dataType an int8 specifying the data type of the object's array data.
   * @param numGates an unsigned int16 specifying the object's number of gates.
   * @param data a char array containing the object's data points.
   * @param gates a DCSCopeGateData array containing the object's gates.
   * @return true if successful, false if failed.  
   */
  bool sendScopeData(const DCSServerInfo& info, uint64_t id, UTILS::UnitType_t xUnits, UTILS::UnitType_t yUnits, double time, double xStart,
		     double xStep, double yBias, double yScale, uint32_t numDataPoints, UTILS::VariableType_t dataType, uint16_t numGates,
		     const void* data, const DCSScopeGateData* gates);

  
  ///Send a time header to the client.
  /**
   * Send a DCSTimeHeader object, created from the specified values, from the server to the client.   Time headers are transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param realtimemod a double specifying the percentage of real-time.
   * @param timetype a byte specifying the time source to use.  
   * @param timedata a DCSTimeData object providing an initial data set for the object associated with this header.  
   * @return true if successful, false if failed.  
   */
  bool sendTimeHeader(const DCSServerInfo& info,double realtimemod,int8_t timetype,const DCSTimeData& timedata);

  ///Send time data to the client.
  /**
   * Send a DCSTimeData object, created from the specified values, from the server to the client.   Time data is transmitted via the UDP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param time a double specifying the current time.
   * @return true if successful, false if failed.  
   */
  bool sendTimeData(const DCSServerInfo& info,double time);

  ///Send a scenario header to the client.
  /**
   * Send a DCSScenarioHeader object, created from the specified values, from the server to the client.   Scenario headers are transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param refFrame Default coordinate reference frame of the scenario and other scenario options
   * @param referenceyear a short specifying the calendar year.
   * @param classificationStr character array for the classification text
   * @param classificationColor 0xAABBGGRR color of the classification text
   * @param scendata a DCSScenarioData object providing an initial data set for the object associated with this header.  
   * @return true if successful, false if failed.  
   */
  bool sendScenarioHeader(const DCSServerInfo& info,const DCSCoordReferenceFrame& refFrame,int16_t referenceyear,const char* classificationStr,uint32_t classificationColor,const DCSScenarioData& scendata);

  ///Send a scenario header to the client. (deprecated)
  /**
   * Send a DCSScenarioHeader object, created from the specified values, from the server to the client.   Scenario headers are transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param latorigin a double specifying the latitude topo origin.
   * @param lonorigin a double specifying the longitude topo origin.
   * @param referenceyear a short specifying the calendar year.
   * @param coordsystem a byte specifying the overall coordinate system.  
   * @param classification a byte specifying the data classification.
   * @param scendata a DCSScenarioData object providing an initial data set for the object associated with this header.  
   * @return true if successful, false if failed.  
   */
  bool sendScenarioHeader(const DCSServerInfo& info,double latorigin,double lonorigin,int16_t referenceyear,DCSCoordSystem_t coordsystem,DCSClassification_t classification,const DCSScenarioData& scendata);

  ///Send scenario data to the client.
  /**
   * Send a DCSScenarioData object, created from the specified values, from the server to the client.   Scenario data is transmitted via the UDP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param winddirection a double specifying the wind direction.  
   * @param windspeed a double specifying the wind speed.
   * @return true if successful, false if failed.  
   */
  bool sendScenarioData(const DCSServerInfo& info,double winddirection,double windspeed);

  ///Send an event to the client.
  /**
   * Send a DCSEvent object, created from the specified values, from the server to the client.   Event data is transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.  
   * @param id an unsigned integer specifying the object's id.  
   * @param time a double specifying the execution time of the event.  
   * @param data an integer containing variant data specifying either a target id, color, state, or status.
   * @param eventtype a byte specifying the event type, also indicating the type of data.  
   * @return true if successful, false if failed.  
   */
  bool sendEvent(const DCSServerInfo& info,uint64_t id,double time,int32_t data,int8_t eventtype);

  ///Send generic data to the client.
  /**
   * Send a DCSGenericData object, created from the specified values, from the server to the client.   Generic data is transmitted via the UDP channel.
   * This function currently has the requirement that both the
   * tag and data arguments be provided as null terminated strings.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param id an unsigned integer specifying the object's id.  
   * @param begintime a double specifying the activation time.  
   * @param expiretime a double specifying the deactivation time.  
   * @param tag an array of bytes specifying the generic data tag.
   * @param data an array of bytes specifying the generic data.
   * @return true if successful, false if failed.  
   */
  bool sendGenericData(const DCSServerInfo& info,uint64_t id,double begintime,double expiretime,const char* tag,const char* data);

  ///Send generic data to the client.
  /**
   * Send a DCSCategoryData object, created from the specified values, from the server to the client.   Category data is transmitted via the TCP channel.
   * This function currently has the requirement that both the
   * tag and data arguments be provided as null terminated strings.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param id an unsigned integer specifying the object's id.  
   * @param timeValue a double specifying the time of validity.  
   * @param name an array of bytes specifying the category data category name.
   * @param value an array of bytes specifying the category data value.
   * @return true if successful, false if failed.  
   */
  bool sendCategoryData(const DCSServerInfo& info,uint64_t id,double timeValue,const char* name,const char* value);

  ///Send data to the client via UDP.
  /**
   * Send a DCSBaseData derived object from the server to the client via the UDP channel.
   * @param info a DCSServerInfo structure containing the socket to be used for data transmission.
   * @param data a pointer to the DCSBaseData derived object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */
  bool sendUDPData(const DCSServerInfo& info,const DCSBaseData* data);

  ///Send data to the client via TCP.
  /**
   * Send a DCSBaseData derived object from the server to the client via the TCP channel.
   * @param info a DCSServerInfo structure containing the socket to be used for data transmission.
   * @param type a byte specifying the type of data be sent (as a DCS request type).  To be sent prior to transmission of actual data so client knows what to expect.  
   * @param data a pointer to the DCSBaseData derived object to be transmitted to the client.
   * @return true if successful, false if failed.  
   * @see @ref requests "Client Requests"
   */
  bool sendTCPData(const DCSServerInfo& info,int8_t type,const DCSBaseData* data);
  
  ///Send a platform header to the client.
  /**
   * Send a DCSPlatformHeader object from the server to the client.   Platform headers are transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param data a pointer to the DCSPlatformHeader object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */
  bool sendPlatformHeader(const DCSServerInfo& info,const DCSPlatformHeader* data);

  ///Send platform data to the client.
  /**
   * Send a DCSPlatformData object from the server to the client.   Platform data is transmitted via the UDP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param data a pointer to the DCSPlatformData object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */
  bool sendPlatformData(const DCSServerInfo& info,const DCSPlatformData* data);

  ///Send a beam header to the client.
  /**
   * Send a DCSBeamHeader object from the server to the client.   Beam headers are transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param data a pointer to the DCSBeamHeader object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */
  bool sendBeamHeader(const DCSServerInfo& info,const DCSBeamHeader* data);

  ///Send beam data to the client.
  /**
   * Send a DCSBeamData object from the server to the client.   Beam data is transmitted via the UDP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param data a pointer to the DCSBeamData object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */
  bool sendBeamData(const DCSServerInfo& info,const DCSBeamData* data);

  ///Send a gate header to the client.
  /**
   * Send a DCSGateHeader object from the server to the client.   Gate headers are transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param data a pointer to the DCSGateHeader object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */  
  bool sendGateHeader(const DCSServerInfo& info,const DCSGateHeader* data);

  ///Send gate data to the client.
  /**
   * Send a DCSGateData object from the server to the client.   Gate data is transmitted via the UDP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param data a pointer to the DCSGateData object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */
  bool sendGateData(const DCSServerInfo& info,const DCSGateData* data);

  ///Send gate data to the client.
  /**
   * Send a DCSScopeData object from the server to the client.   Gate data is transmitted via the UDP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param data a pointer to the DCSScopeData object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */
  bool sendScopeData(const DCSServerInfo& info,const DCSScopeData* data);
  
  ///Send a time header to the client.
  /**
   * Send a DCSTimeHeader object from the server to the client.   Time headers are transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param data a pointer to the DCSTimeHeader object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */
  bool sendTimeHeader(const DCSServerInfo& info,const DCSTimeHeader* data);

  ///Send time data to the client.
  /**
   * Send a DCSTimeData object from the server to the client.   Time data is transmitted via the UDP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param data a pointer to the DCSTimeData object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */
  bool sendTimeData(const DCSServerInfo& info,const DCSTimeData* data);

  ///Send a scenario header to the client.
  /**
   * Send a DCSScenarioHeader object from the server to the client.   Scenario headers are transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param data a pointer to the DCSScenarioHeader object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */
  bool sendScenarioHeader(const DCSServerInfo& info,const DCSScenarioHeader* data);

  ///Send scenario data to the client.
  /**
   * Send a DCSScenarioData object from the server to the client.   Scenario data is transmitted via the UDP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param data a pointer to the DCSScenarioData object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */
  bool sendScenarioData(const DCSServerInfo& info,const DCSScenarioData* data);

  ///Send an event to the client.
  /**
   * Send a DCSEvent object from the server to the client.   Event data is transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.  
   * @param data a pointer to the DCSEvent object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */
  bool sendEvent(const DCSServerInfo& info,const DCSEvent* data);

  ///Send generic data to the client.
  /**
   * Send a DCSGenericData object from the server to the client.   Generic data is transmitted via the UDP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param data a pointer to the DCSGenericData object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */
  bool sendGenericData(const DCSServerInfo& info,const DCSGenericData* data);

  ///Send category data to the client.
  /**
   * Send a DCSCategoryData object from the server to the client.   Category data is transmitted via the TCP channel.
   * @param info a DCSServerInfo structure specifying the socket to be used for data transmission.
   * @param data a pointer to the DCSCategoryData object to be transmitted to the client.
   * @return true if successful, false if failed.  
   */
  bool sendCategoryData(const DCSServerInfo& info,const DCSCategoryData* data);


  //Client operations

  ///Connect to server.
  /**
   * Establish a connection to a listening server.  This operation may be performed with either blocking or non-blcoking I/O.  
   * @param info a DCSClientInfo structure to be filled with data specific too the client being created.  Resources required
   * by DCSClientInfo will be allocated by this function.  A matching call to clientDisconnect is required to properly free these resources, when finished.
   * @param port an unsinged integer specifying the port with which to connect.  
   * @param addr network address of server with which to connect (eg. "192.168.44.5").
   * @param iface interface to connect and receive data through (eg. "127.0.0.1" for loopback or the ip address of an installed network card).
   * @param block a boolean value specifying that the connection operation be performed with blocking or non-blocking I/O.  A non blocking operation may 
   *        return before the connection is established, requiring the calling process to test for a completed connection before the socket may be used.  
   * @return true if successful, false if failed.  On failure, no resources will have been allocated, so a call to clientDisconnect is unnecessary.  
   * @see clientDisconnect()
   */
  bool clientConnect(DCSClientInfo& info,uint16_t port,const char* addr,const char* iface=NULL,bool block=true);

  ///Negotiate a new connection with server - without reading headers.
  /**
   * Negotiates a connection with a server.  This function is used to negotiate a client connection that was made
   * by clientConnect().  The negotiation is handled separately from clientConnect for use by clients that want to read the headers separately.  
   * This is useful for clients that want to process headers while performing other operations simultaneously.  
   * This operation may only be performed with blocking I/O.  If the function receives a non-blocking socket, it will set the socket to blocking mode before performing 
   * the operation and reset it to the non-blocking mode after the operation has completed.  
   * @param info a DCSClientInfo structure to receive the UDP data mode for the connection.  
   * @param time a DCSTimeHeader structure to be received by the client as part of the connection negotiation.  
   * @param scenario a DCSScenarioHeader structure to be received by the client as part of the connection negotiation.
   * @param headers a pointer to integer for retrieving the number of DCSBaseData derived objects to be subsequently received by the client as part of the connection negotiation.
   * @param version an unsigned short to be filled the DCS protocol version number supported by the DCS server.  If a client negotiaion fails, check this value to make sure
   * it matches the version of the DCS protocol employed by the client.  
   * @return true if successful, false if failed.  On failure, client connection is not closed by this function.  The application should close the socket
   * explicitly with a call to clientDisconnect if this function fails.  The contents of the HeaderList, if any, will also have ot be destroyed explicitly
   * through use of the 'delete' operator.  
   * @see clientConnect()
   * @see clientDisconnect()
   * @see DCSHeaderInfo
   */
  bool clientNegotiate(DCSClientInfo& info,DCSTimeHeader& time,DCSScenarioHeader& scenario,uint32_t* headers=NULL,uint16_t* version=NULL);

  ///Negotiate a new connection with server - reading all headers.
  /**
   * Negotiates a connection with a server.  This function is used to negotiate a client connection that was made
   * by clientConnect().  The negotiation is handled separately from clientConnect for use by clients that are designed to be multithreaded.  
   * The function will not return until all platform, beam, and gate headers have been retrieved from the server.  
   * This operation may only be performed with blocking I/O.  If the function receives a non-blocking socket, it will set the socket to blocking mode before performing 
   * the operation and reset it to the non-blocking mode after the operation has completed.  
   * @param info a DCSClientInfo structure to receive the UDP data mode for the connection.  
   * @param time a DCSTimeHeader structure to be received by the client as part of the connection negotiation.  
   * @param scenario a DCSScenarioHeader structure to be received by the client as part of the connection negotiation.
   * @param headers a HeaderList object to be filled with DCSBaseData derived objects received by the client as part of the connection negotiation.
   * @param version an unsigned short to be filled the DCS protocol version number supported by the DCS server.  If a client negotiaion fails, check this value to make sure
   * it matches the version of the DCS protocol employed by the client.  
   * @return true if successful, false if failed.  On failure, client connection is not closed by this function.  The application should close the socket
   * explicitly with a call to clientDisconnect if this function fails.  The contents of the HeaderList, if any, will also have ot be destroyed explicitly
   * through use of the 'delete' operator.  
   * @see clientConnect()
   * @see clientDisconnect()
   * @see DCSHeaderInfo
   */
  bool clientNegotiate(DCSClientInfo& info,DCSTimeHeader& time,DCSScenarioHeader& scenario,HeaderList& headers,uint16_t* version=NULL);

  ///Close an active connection.
  /**
   * Closes connection and frees resources allocated for DCSClientInfo by clientConnect.  
   * @param info a DCSClientInfo structure containing data sepcific to the client to be disconnected.  Resources allocated for this structure by clientConnect will be freed.  
   * @return true if successful, false if failed.  Failure indicates that one or both of the data and client sockets did not exist.  
   * @see clientConnect()
   */
  void clientDisconnect(DCSClientInfo& info);

  ///Request data from server.
  /**
   * Send a data request to server.  Data sent by server as a result of this request is to be read with the readUDPData and readTCPData functions.  Prior to using this function
   * the client must be connected to the server and the DCSClientInfo structure initialized through a call to clientConnect.  
   * Attempts to send DCSREQUEST_HEADER thorugh this function will result in error.  DCSREQUEST_HEADER must be made through clientHeaderRequest.  
   * @param info a DCSClientInfo structure specifying the socket with which data requests are to be sent.
   * @param request an unsinged byte specifying the type of data request being made.  
   * @return true if successful, false if failed.  
   * @see @ref requests "Client Requests"
   * @see clientConnect()
   * @see clientHeaderRequest()
   * @see readTCPData()
   * @see readUPPData()
   */
  bool clientRequest(const DCSClientInfo& info,int8_t request);

  ///Request a platform, beam, or gate header from server.
  /**
   * Send a header request to server.  Data sent by server as a result of this request is to be read with the readTCPData function.  Prior to using this function
   * the client must be connected to the server and the DCSClientInfo structure initialized through a call to clientConnect.  
   * @param info a DCSClientInfo structure specifying the socket with which data requests are to be sent.
   * @param id a 64-bit unsigned integer specifying the id for the header to retrieve.  
   * @return true if successful, false if failed.  
   * @see @ref requests "Client Requests"
   * @see clientConnect()
   * @see readTCPData()
   */
  bool clientHeaderRequest(const DCSClientInfo& info,uint64_t id);

  ///Receive data from TCP data channel.
  /**
   * Reads a single data object from the TCP data channel.
   * @param info a DCSClientInfo structure specifying the socket with which data is to be received.
   * @return a pointer to a DCBaseData object.  The object's type may be indentified with a call
   * to DCSBeamData::getType().  The application is responsible for freeing the memory allocated for
   * the object through use of the 'delete' operator.
   * @see DCSBeamData::getType()
   */
  DCSBaseData* readTCPData(const DCSClientInfo& info);

  ///Receive data from UDP data channel.
  /**
   * Reads a single data object from the UDP data channel.
   * @param info a DCSClientInfo structure specifying the socket with which data is to be received.
   * @return a pointer to a DCBaseData object.  The object's type may be indentified with a call
   * to DCSBeamData::getType().  The application is responsible for freeing the memory allocated for
   * the object through use of the 'delete' operator.
   * @see DCSBeamData::getType()
   */
  DCSBaseData* readUDPData(const DCSClientInfo& info);
}

#endif
