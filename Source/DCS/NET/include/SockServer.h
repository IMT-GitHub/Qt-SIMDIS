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

#ifndef _SOCKSERVER_H_
#define _SOCKSERVER_H_

///TCP server socket class.  
/**
 * A TCP server socket that utilizes the TCP network protocol for 
 * reliable two-way network communication.  
 */
class SockServer : public SockTCP
{
private:
  bool create();
protected:
  ///Listening for connections.  
  bool listening_;		      /**< Indicates that the server socket is listening for client socket connections.  */

  ///Connection backlog.  
  int32_t backlog_;		      /**< The connection backlog specifying the number of unaccepted connections may be present at a time.  */
public:
  ///Default server socket constructor.  
  /**
   * Creates an inactive server socket object with default data values.  
   */
  SockServer();

  ///Server socket constructor.  
  /**
   * Creates an actively listening server socket object with the specified data values.  
   * @param port an unsigned 16-bit integer specifying the local port with which to bind the server socket.  
   *        This is the port on which client connections shall be accepted.  
   * @param iface a character string specifying the network interface to use for network communication.  
   * @param backlog an integer specifying the server backlog.  
   * @see backlog_
   */
  SockServer(uint16_t port,const char* iface=NULL,int32_t backlog=32);

  ///Server socket constructor.  
  /**
   * Creates an actively listening server socket object with the specified data values.  
   * @param localaddr a SockAddrInet object specifying the network information to be used 
   *        when creating the server socket.  
   * @param backlog an integer specifying the server backlog.  
   * @see backlog_
   */
  SockServer(const SockAddrInet* localaddr,int32_t backlog=32);

  ///Server socket constructor.  
  /**
   * Creates a server socket object from the specified data value.  
   * @param sockfd a handle to a previously created socket, to be used by the server socket object.  
   */
  SockServer(SOCKET sockfd);

  ///Indicates an active socket.  
  /**
   * @see SockTCP::isActive
   */
  bool isActive() const;

  ///Indicates a listening socket.  
  /**
   * Indicates that the server socket object is listening for connections.  
   * @return a boolean value indicating that the socket is listening if true, not listening if false.  
   */
  bool isListening() const;

  ///Retrieve server's backlog value.  
  /**
   * Returns the server's curretn backlog value.  
   * @return an integer specifying the server socket's backlog.  
   */
  int32_t getBacklog() const;

  ///Start server socket.  
  /**
   * @see SockInet::open
   */
  virtual bool open();

  ///Close server socket.  
  /**
   * @see SockInet::close
   */
  virtual bool close();

  ///Listen for connections.  
  /**
   * Starts server socket object listening for client connections.  
   * @param backlog an integer specifying the server socket object's backlog.  
   * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.  
   */
  bool listen(int32_t backlog=32);

  ///Accept client connection.  
  /**
   * Accepts a connection from a client socket.  
   * @return a handle to the connected clietn socket.  
   */
  SOCKET accept();
};

#endif
