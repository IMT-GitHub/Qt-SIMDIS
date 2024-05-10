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

#ifndef _SOCKUNICAST_H_
#define _SOCKUNICAST_H_

///UDP unicast socket class.  
/**
 * A UDP socket that utilizes network unicast for data transmission.  
 */
class SockUnicast : public SockUDP
{
private:
  bool create();
public:
  ///Default unicast socket constructor.  
  /**
   * Creates a unicast socket object with default data values.  
   */
  SockUnicast();

  ///Unicast socket constructor.  
  /**
   * Creates a unicast socket object with the specified data values.  
   * @param port an unsigned 16-bit integer specifying the port to bind 
   *        the socket to.  
   * @param iface a character string specifying the network interface to use
   *        for network transmission.  
   */
  SockUnicast(uint16_t port,const char* iface=NULL);

  ///Unicast socket constructor.  
  /**
   * Creates a unicast socket object with the specified data values, and connects it to a
   * specified peer.  When a unicast socket object is connected to a peer, data can be sent
   * to that peer with calls to 'write' rather than calls to 'writeTo'.  When a UDP socket is connected
   * it can only communicate with the peer.  
   * @param peerport a 16-bit integer specifying the port of the peer that the socket
   *        is to be connected/attached to.  
   * @param peeraddr a character string specifying the network address of the peer 
   *        that the socket is connected/attached to.  
   * @param port an unsigned 16-bit integer specifying the port to bind 
   *        the socket to.  
   * @param iface a character string specifying the network interface to use
   *        for network transmission.  
   * @param attach a boolean value specifying whether the socket is to be attached
   *        to the peer or connected to the peer.  When the socket is attached, the socket
   *        object stores the peerport and peeraddr values and uses them to call 'sendto'.  If the 
   *        socket is connected, the socket object links the socket to the peer on the
   *        kernel level with a call to the 'connect' function and sends data with 'send'.  
   * @see SockUDP::attached_
   * @see SockUDP::write
   * @see SockUDP::writeTo
   */
  SockUnicast(uint16_t peerport,const char* peeraddr,uint16_t port,const char* iface=NULL,bool attach=true);

  ///Unicast socket constructor.  
  /**
   * Creates a unicast socket object with the specified data values.  
   * @param localaddr a SockAddrInet object specifying the network information to be used 
   *        when creating the socket.  
   * @see SockAddrInet
   */
  SockUnicast(const SockAddrInet* localaddr);

  ///Unicast socket constructor.  
  /**
   * Creates a unicast socket object with the specified data values, and connects it to a
   * specified peer.  When a unicast socket object is connected to a peer, data can be sent
   * to that peer with calls to 'write' rather than calls to 'writeTo'.  When a UDP socket is connected
   * it can only communicate with the peer.  
   * @param peeraddr a SockAddrInet object specifying the network information for the peer 
   *        to connect with when creating the socket.  
   * @param localaddr a SockAddrInet object specifying the network information to be used 
   *        when creating the socket.  
   * @param attach a boolean value specifying whether the socket is to be attached
   *        to the peer or connected to the peer.  When the socket is attached, the socket
   *        object stores the peerport and peeraddr values and uses them to call 'sendto'.  If the 
   *        socket is connected, the socket object links the socket to the peer on the
   *        kernel level with a call to the 'connect' function and sends data with 'send'.  
   * @see SockUDP::attached_
   * @see SockUDP::write
   * @see SockUDP::writeTo   */
  SockUnicast(const SockAddrInet* peeraddr,const SockAddrInet* localaddr,bool attach=true);

  ///Unicast socket constructor.  
  /**
   * Creates a socket object from the specified data value.  
   * @param sockfd a handle to a previously created socket, to be used by the socket object.  
   */
  SockUnicast(SOCKET sockfd);

  ///Open socket connection.  
  /**
   * @see SockInet::open
   */
  virtual bool open();
};

#endif
