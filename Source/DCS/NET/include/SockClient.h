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

#ifndef _SOCKCLIENT_H_
#define _SOCKCLIENT_H_

class SockAddrInet;

///TCP client socket class.  
/**
 * A TCP client socket that utilizes the TCP network protocol for 
 * reliable two-way network communication.  
 */
class SockClient : public SockTCP
{
private:
  bool create();
protected:
  ///Connection indicator.  
  bool connected_;		      /**< Indicates that the client socket is connected to a peer.  */

  ///Peer network address information.  
  SockAddrInet* peeraddr_;	      /**< Socket address object containig the information of the socket's peer.  */
public:
  ///Default client socket constructor.  
  /**
   * Creates an unconnected client socket object with default data values.  
   */
  SockClient();

  ///Client socket constructor.  
  /**
   * Creates a connected client socket object with the specified data values.  
   * @param peerport an unsigned 16-bit integer specifying the port of the TCP server socket with which to connect.  
   * @param peeraddr a character string specifying the network address of the TCP server socket with which to connect.  
   * @param port an unsigned 16-bit integer specifying the local port with which to bind the client socket.  
   * @param iface a character string specifying the network interface to use for network transmission.  
   */
  SockClient(uint16_t peerport,const char* peeraddr,uint16_t port=EPHEMERAL_PORT,const char* iface=NULL);

  ///Client socket constructor.  
  /**
   * Creates a connected client socket object with the specified data values.  
   * @param peeraddr a SockAddrInet object specifying the network information for the peer 
   *        (TCP server socket) to connect with when creating the client socket.  
   * @param localaddr a SockAddrInet object specifying the network information to be used 
   *        when creating the client socket.  
   */
  SockClient(SockAddrInet* peeraddr,SockAddrInet* localaddr);

  ///Client socket constructor.  
  /**
   * Creates a client socket object from the specified data value.  
   * @param sockfd a handle to a previously created socket, to be used by the client socket object.  
   */
  SockClient(SOCKET sockfd);

  ///Destructor.  
  /**
   * Virtual destructor.  Will close an open/connected client socket.  
   */
  virtual ~SockClient();
    
  ///Indicates an active socket.  
  /**
   * @see SockTCP::isActive
   */
  virtual bool isActive() const;

  ///Indicates a connected socket.  
  /**
   * @return a boolean value indicating that the socket is connected if true, and is not connected if false.  
   */
  virtual bool isConnected() const;

  ///Retrieve peer's port value.  
  /**
   * Returns value of the port for the server socket that the client is connected to.  
   * @return an unsigned 16-bit integer specifying the TCP server socket's port number.  
   */
  uint16_t getPort() const;

  ///Retrieve peer's network address value.  
  /**
   * Returns value of the network address for the server socket that the client is connected to.  
   * @param peeraddr a character array to be filled with the server socket's IPv4 network address.  
   * @param len an integer specifying the size, in bytes, of 'addr'.  
   * @return a pointer to an array of characters containing the TCP server socket's IPv4 network address.  
   */
  const char* getAddress(char* peeraddr,int32_t len) const;

  ///Open socket connection.  
  /**
   * @see SockInet::open
   */
  virtual bool open();

  ///Close socket connection.  
  /**
   * @see SockInet::close
   */
  virtual bool close();

  ///Connect to peer.  
  /**
   * Connects socket to the specified TCP server socket.  
   * @param peerport an unsigned 16-bit integer specifying the port of the TCP server socket with which to connect.  
   * @param peeraddr a character string specifying the network address of the TCP server socket with which to connect.  
   * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.  If the operation 
   *         was performed with non-blocking I/O and the function returned before the connection was established, the return 
   *         value will be false but the isConnected() and isActive() functions will return true, indicating that the connection 
   *         is pending.  The readReady() or writeReady() functions can be used to determine the completion of a pending 
   *         non-blocking connect.  When the non-blocking connection is completed, the getSockErr() function can be used to 
   *         determine that the connection was successful.  If the non-blocking connect failed, the close() function should
   *         be used to reset the socket state.  
   * @see SockInet::readReady
   * @see SockInet::writeReady
   * @see SockInet::isBlockingIO
   * @see SockInet::setBlockingIO
   * @see SockInet::close
   * @see getSockErr
   */
  bool connect(uint16_t peerport,const char* peeraddr);
};

#endif
