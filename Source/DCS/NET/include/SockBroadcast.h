// -*- mode: c++ -*-
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

#ifndef _SOCKBROADCAST_H_
#define _SOCKBROADCAST_H_

///UDP boroadcast socket class.  
/**
 * A socket that utilizes the network broadcast protocol for 
 * data transmission.  
 */
class SockBroadcast : public SockUDP
{
private:
  bool create();
public:
  ///Default broadcast socket constructor.  
  /**
   * Creates a broadcast socket object with default data values.  
   */
  SockBroadcast();

  ///Broadcast socket constructor.  
  /**
   * Creates a broadcast socket object with the specified data values.  
   * @param port an unsigned 16-bit integer specifying the port to bind 
   *        the broadcast socket to.  
   * @param iface a character string specifying the network interface to use
   *        for network transmission.  When reading data with a broadcast socket,
   *        it is best not to set the interface unless you really know what you are doing.  
   */
  SockBroadcast(uint16_t port,const char* iface=NULL);

  ///Broadcast socket constructor.  
  /**
   * Creates a broadcast socket object with the specified data values, and connects it to a
   * specified broadcast address.  When a broadcast socket object is connected to an address, data can be sent
   * to that address with calls to 'write' rather than calls to 'writeTo'.  When a broadcast socket is connected
   * it can only communicate with that address.  Connecting a broadcast socket to a specific broadcast 
   * address can cause problems when attempting to read from that socket, because a number of systems
   * read all broadcast data on "0.0.0.0" or "255.255.255.255", so if the broadcast socket is connected
   * to a specific address it may never receive any information.  
   * @param peerport a 16-bit integer specifying the port of the peer that the broadcast socket
   *        is to be connected/attached to.  
   * @param peeraddr a character string specifying the network broadcast address 
   *        that the broadcast socket is connected/attached to.  
   * @param port an unsigned 16-bit integer specifying the port to bind 
   *        the broadcast socket to.  
   * @param iface a character string specifying the network interface to use
   *        for network transmission.  When reading data with a broadcast socket,
   *        it is best not to set the interface unless you really know what you are doing.  
   * @param attach a boolean value specifying whether the broadcast socket is to be attached
   *        to the peer or connected to the peer.  When the socket is attached, the broadcast socket
   *        object stores the peerport and peeraddr values and uses them to call 'sendto'.  If the 
   *        socket is connected, the broadcast socket object links the socket to the peer on the
   *        kernel level with a call to the 'connect' function and sends data with 'send'.  
   * @see SockUDP::attached_
   * @see SockUDP::write
   * @see SockUDP::writeTo
   */
  SockBroadcast(uint16_t peerport,const char* peeraddr,uint16_t port,const char* iface=NULL,bool attach=true);

  ///Broadcast socket constructor.  
  /**
   * Creates a broadcast socket object with the specified data values.  
   * @param localaddr a SockAddrInet object specifying the network information to be used 
   *        when creating the broadcast socket.  
   * @see SockAddrInet
   */
  SockBroadcast(const SockAddrInet* localaddr);

  ///Broadcast socket constructor.  
  /**
   * Creates a broadcast socket object with the specified data values, and connects it to a
   * specified broadcast address.  When a broadcast socket object is connected to an address, data can be sent
   * to that address with calls to 'write' rather than calls to 'writeTo'.  When a broadcast socket is connected
   * it can only communicate with that address.  Connecting a broadcast socket to a specific broadcast 
   * address can cause problems when attempting to read from that socket, because a number of systems
   * read all broadcast data on "0.0.0.0" or "255.255.255.255", so if the broadcast socket is connected
   * to a specific address it may never receive any information.  
   * @param peeraddr a SockAddrInet object specifying the network information for the broadcast 
   *        address to connect with when creating the broadcast socket.  
   * @param localaddr a SockAddrInet object specifying the network information to be used 
   *        when creating the broadcast socket.  
   * @param attach a boolean value specifying whether the broadcast socket is to be attached
   *        to the peer or connected to the peer.  When the socket is attached, the broadcast socket
   *        object stores the peerport and peeraddr values and uses them to call 'sendto'.  If the 
   *        socket is connected, the broadcast socket object links the socket to the peer on the
   *        kernel level with a call to the 'connect' function and sends data with 'send'.  
   * @see SockUDP::attached_
   * @see SockUDP::write
   * @see SockUDP::writeTo
   */
  SockBroadcast(const SockAddrInet* peeraddr,const SockAddrInet* localaddr,bool attach=true);

  ///Broadcast socket constructor.  
  /**
   * Creates a broadcast socket object from the specified data value.  
   * @param sockfd a handle to a previously created socket, to be used by the broadcast socket object.  
   */
  SockBroadcast(SOCKET sockfd);

  ///Open socket connection.  
  /**
   * @see SockInet::open
   */
  virtual bool open();
};

#endif
