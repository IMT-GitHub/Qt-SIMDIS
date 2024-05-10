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

#ifndef _DCSDISCOVER_H_
#define _DCSDISCOVER_H_

#include "sockdefs.h"
#include "DCSdefs.h"
class SockMulticast;

enum
{
  DISC_MSGSIZE=127
};

namespace DCSDisc
{
  /// DCS Server discovery class
  /**
   * A class to use for locate active DCS servers on a network.  A DCS client uses the class when transmiting 
   * messages to the network soliciting for active servers that are accepting client connections.  
   * A DCS server uses the class when responding to solicitations from client, providing an IP address 
   * and port to which the client can connect.  The DCSDiscover class contains multicast sockets for server-client
   * communication and data fields for storing information retrieved from the responding DCS server.  
   */
  struct DCSDiscover
  {
    public:
      SockMulticast* request;            /**< Multicast socket for client to use when sending server solicitations.  */
      SockMulticast* respond;            /**< Multicast socket for server to use when responding to client solicitations.  */
      char addr[DCS_ADDRSTRLEN];         /**< IP address provided by server as part of a response to a solicitation.  */
      uint16_t port;                     /**< Portprovided by server as part of a response to a solicitatoin.  */
      TTL_T level;                       /**< Multicast TTL in use by a DCS server operating in multicast data distribution mode
                                              that can be used to determine that a client is able to receive multicast data from a
                                              server based on client proximity to the server.  */
      char msg[DISC_MSGSIZE];            /**< A string containing a solicitation message from a client or a response message from a server.  */

      ///Default constructor
      DCSDiscover():request(NULL),respond(NULL),port(EPHEMERAL_PORT),level(1){ memset(addr,'\0',sizeof(addr)); memset(msg,'\0',sizeof(msg)); }
  };

  bool discoverServInit(DCSDiscover& info,uint16_t port,const char* addr,const char* msg,TTL_T level=1,const char* iface=NULL);

  bool discoverClientInit(DCSDiscover& info,TTL_T level=1,const char* iface=NULL);

  void discoverFree(DCSDiscover& info);

  bool discoverAnnounce(DCSDiscover& info);

  bool discoverRespond(DCSDiscover& info);

  bool discoverRequest(DCSDiscover& info);

  bool discoverReceive(DCSDiscover& info);
}

#endif
