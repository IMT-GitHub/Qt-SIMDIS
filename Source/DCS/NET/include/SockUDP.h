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

#ifndef _SOCKUDP_H_
#define _SOCKUDP_H_

class SockAddrInet;

///UDP socket class.  
/**
 * An abstract class specifying a UDP internet socket.  
 */
class SockUDP : public SockInet
{
protected:
  ///Socket is attached.  
  /**
   * Attaching a socket to another address simply means that you no longer have to call
   * 'readFrom' because all reads will use the address from the attach specified address.
   * After attaching, 'write' will call 'writeTo' with the specified info.  This
   * does not actually connect the socket, it merely stores the peer info at the application level.  
   */
  bool attached_;

  ///Socket is connected.  
  /**
   * Connectiong a socket to a specific address will produce the same behavior as attaching a socket, 
   * but will differ internally.  Connecting a socket will actually connect the socket to the specified 
   * address on the kernel level.  This can improve performance of UDP write operations a great deal, 
   * but means that the connected socket can only communicate with the address to which it is connected.  
   */
  bool connected_;

  ///Peer network address information.  
  /** Remote port and address for a connected UDP socket
   * When sending UDP data to a single host it will be more
   * efficient to "connect" to that host.  You are not "connecting"
   * as you would with TCP, rather, the kernel is storing the info for
   * the peer.  This way the kernel does not have to copy the peer's info
   * and issue a temporary connect/disconnect for every datagram sent.
   * Connected UDP sockets can only send AND receive data to/from the
   * connected peer.
   * Also with a connected UDP socket, the application will receive
   * Asyncronous errors such as EHOSTUNREACH that would otherwise not
   * be delivered to an unconnected UDP socket.
   */
  SockAddrInet* peeraddr_;
public:
  ///Default UDP socket constructor.  
  /**
   * Creates a UDP socket object with default data values.  
   */
  SockUDP();

  ///UDP socket constructor.  
  /**
   * Creates a UDP socket object with the specified data values.  
   * @param port an unsigned 16-bit integer specifying the port to bind 
   *        the socket to.  
   * @param iface a character string specifying the network interface to use
   *        for network transmission.  
   */
  SockUDP(uint16_t port,const char* iface=NULL);

  ///UDP socket constructor.  
  /**
   * Creates a UDP socket object with the specified data values, and connects it to a
   * specified peer.  When a UDP socket object is connected to a peer, data can be sent
   * to that peer with calls to 'write' rather than calls to 'writeTo'.  When a UDP socket is connected
   * it can only communicate with the peer.  
   * @param peerport a 16-bit integer specifying the port of the peer to which the socket
   *        is to be connected/attached.  
   * @param peeraddr a character string specifying the network address of the peer to which 
   *        the socket is to be connected/attached.  
   * @param port an unsigned 16-bit integer specifying the port to which the socket is to bind.  
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
  SockUDP(uint16_t peerport,const char* peeraddr,uint16_t port,const char* iface=NULL,bool attach=true);

  ///UDP socket constructor.  
  /**
   * Creates a UDP socket object with the specified data values.  
   * @param localaddr a SockAddrInet object specifying the network information to be used 
   *        when creating the socket.  
   * @see SockAddrInet
   */
  SockUDP(const SockAddrInet* localaddr);

  ///UDP socket constructor.  
  /**
   * Creates a UDP socket object with the specified data values, and connects it to a
   * specified peer.  When a UDP socket object is connected to a peer, data can be sent
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
  SockUDP(const SockAddrInet* peeraddr,const SockAddrInet* localaddr,bool attach=true);

  ///UDP socket constructor.  
  /**
   * Creates a socket object from the specified data value.  
   * @param sockfd a handle to a previously created socket, to be used by the socket object.  
   */
  SockUDP(SOCKET sockfd);

  ///Destructor.  
  /**
   * Virtual destructor.  Will close an open/connected socket.  
   */
  virtual ~SockUDP();

  ///Indicates a connected socket.  
  /**
   * @return a boolean value indicating that the socket is connected if true, and is not connected if false.  
   */
  bool isConnected() const;

  ///Indicates an attached socket.  
  /**
   * @return a boolean value indicating that the socket is attached if true, and is not attached if false.  
   */
  bool isAttached() const;

  ///Retrieve attached/connected port value.  
  /**
   * Returns value of the port that the socket is attached or connected to.  
   * @return an unsigned 16-bit integer specifying the UDP port number.  
   */
  uint16_t getPort() const;

  ///Retrieve attached/connected network address value.  
  /**
   * Returns value of the network address that the socket is attached or connected to.  
   * @param peeraddr a character array to be filled with the IPv4 network address.  
   * @param len an integer specifying the size, in bytes, of 'addr'.  
   * @return a pointer to an array of characters containing the TCP server socket's IPv4 network address.  
   */
  char* getAddress(char* peeraddr,int32_t len) const;

  ///Read data. 
  /**
   * Reads data that is available in the socket input buffer.  
   * @param buffer a pointer to an object to be filled with the data in the socket buffer.  
   * @param size an integer specifying the size, in bytes, of 'buffer'.  
   * @param flags an integer specifying the bit flags to be passed to the socket when reading.  
   *        These are the flags used by the BSD socket function 'recv'.  See the 'recv' man pages or
   *        MSDN documentation for more details.  
   * @return the total number of bytes read.  
   */
  virtual int32_t read(void* buffer,int32_t size,int32_t flags=0) const;

  ///Write data.  
  /**
   * Writes data to the network.  
   * @param buffer a pointer to an object filled with the data to be written.  
   * @param size an integer specifying the size, in bytes, of 'buffer'.  
   * @param flags an integer specifying the bit flags to be passed to the socket when writing.  
   *        These are the flags used by the BSD socket function 'send'.  See the 'send' man pages or
   *        MSDN documentation for more details.  
   * @return the total number of bytes written.  
   */
  virtual int32_t write(const void* buffer,int32_t size,int32_t flags=0) const;

  ///Read data. 
  /**
   * Reads data that is available in the socket input buffer, and reports the address and port of the sender.  
   * @param buffer a pointer to an object to be filled with the data in the socket buffer.  
   * @param size an integer specifying the size, in bytes, of 'buffer'.  
   * @param flags an integer specifying the bit flags to be passed to the socket when reading.  
   *        These are the flags used by the BSD socket function 'recvfrom'.  See the 'recvfrom' man pages or
   *        MSDN documentation for more details.  
   * @param from a socket address structure specifying the address and port of the peer that sent the data.  
   * @param addrlen an integer specifying the size of 'to' in bytes.  
   * @return the total number of bytes read.  
   */
  virtual int32_t readFrom(void* buffer,int32_t size,int32_t flags=0,SOCKADDR* from=NULL,socklen_t* addrlen=NULL) const;

  ///Write to specified destination.  
  /**
   * Writes data to the specified network address.  
   * @param buffer a pointer to an object filled with the data to be written.  
   * @param size an integer specifying the size, in bytes, of 'buffer'.  
   * @param flags an integer specifying the bit flags to be passed to the socket when writing.  
   *        These are the flags used by the BSD socket function 'sendto'.  See the 'sendto' man pages or
   *        MSDN documentation for more details.  
   * @param to a socket address structure specifying the destination address and port of the data to be written.  
   * @param addrlen an integer specifying the size of 'to' in bytes.  
   * @return the total number of bytes written.  
   */
  virtual int32_t writeTo(const void* buffer,int32_t size,int32_t flags=0,const SOCKADDR* to=NULL,socklen_t addrlen=0) const;

  ///Open connected socket.  
  /**
   * @see SockInet::open
   */
  virtual bool open()=0;

  ///Close connected socket.  
  /**
   * @see SockInet::close
   */
  virtual bool close();

  ///Connect socket.  
  /**
   * Connect the socket to a specified port and address.  
   * @param peerport an unsigned 16-bit integer specifying the port with which to connect.  
   * @param peeraddr a character string specifying the UDP network address with which to connect.  
   * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.  
   */
  bool connect(uint16_t peerport,const char* peeraddr);

  ///Attach socket.  
  /**
   * Attach the socket to a specified port and address.  
   * @param peerport an unsigned 16-bit integer specifying the port with which to attach.  
   * @param peeraddr a character string specifying the UDP network address with which to attach.  
   * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.  
   */
  bool attach(uint16_t peerport,const char* peeraddr);

  ///Disconnect socket.  
  /**
   * Disconnect a connected socket.  
   * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.  
   */
  bool disconnect();

  ///Detach socket.  
  /**
   * Detach an attached socket.  
   * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.  
   */
  bool detatch();
};

#endif
