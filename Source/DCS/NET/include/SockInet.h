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

#ifndef _SOCKINET_H_
#define _SOCKINET_H_

class SockAddrInet;

///Internet socket socket class.  
/**
 * An abstract class for a socket from the ineternet socket address family.  
 */
class SockInet : public SockIO
{
protected:
  ///Use blocking I/O.  
  bool block_;			    /**< A boolean value indicating that the socket is blocking/non-blocking.  */

  ///Socket handle.  
  SOCKET sockfd_;		    /**< Handle to the socket used by this object.  */

  ///Local socket address settings.  
  SockAddrInet* localaddr_;	    /**< A socket address object specifying the local settings for this object.  */
public:
  ///Default socket constructor.  
  /**
   * Creates an unconnected socket object with default data values.  
   */
  SockInet();

  ///Socket constructor.  
  /**
   * Creates a socket object with the specified data values.  
   * @param port an unsigned 16-bit integer specifying the local port with which to bind the socket.  
   * @param iface a character string specifying the network interface to use for network transmission.  
   */
  SockInet(uint16_t port,const char* iface=NULL);

  ///Socket constructor.  
  /**
   * Creates a socket object with the specified data values.  
   * @param localaddr a SockAddrInet object specifying the network information to be used 
   *        when creating the socket.  
   */
  SockInet(const SockAddrInet* localaddr);

  ///Socket constructor.  
  /**
   * Creates a socket object from the specified data value.  
   * @param sockfd a handle to a previously created socket, to be used by the socket object.  
   */
  SockInet(SOCKET sockfd);

  ///Destructor.  
  /**
   * Virtual destructor.  Will close an open/connected socket.  
   */
  virtual ~SockInet();

  ///Indicates the socket handle references a valid kernel socket.  
  /**
   * Indicates that the socket object's socket handle is valid.  
   * @return a boolean value indicating the socket handle is valid if true, and invalid if false.  
   */
  virtual bool isValid() const;

  ///Retrieve the socket descriptor.  
  /**
   * Returns the socket descriptor.  
   * @return a SOCKET specifying the socket descriptor if a valid socket descriptor exists, or 
   *         INVALID_SOCKET if no valid socket descriptor exists.  
   */
  SOCKET getDescriptor() const;

  ///Retrieve the socket address family.  
  /**
   * Returns the socket address family type.  
   * @return an integer specifying the socket address family.  Currently only AF_INET
   *         is supported.  
   */
  int32_t getFamily() const;
    
  ///Retrieve port value.  
  /**
   * Returns value of port.  
   * @return an unsigned 16-bit integer specifying a TCP port number for the socket object.  
   */
  uint16_t getLocalPort() const;

  ///Retrieve network interface value.  
  /**
   * Returns value of network interface.  
   * @param iface a character array to be filled with the IPv4 network address associated with the network interface.  
   * @param len an integer specifying the size, in bytes, of 'addr'.  
   * @return a pointer to an array of characters containing the IPv4 network address for the associated network interface.  
   */
  const char* getIface(char* iface,int32_t len) const;

  ///Get socket option.  
  /**
   * Retrieves the value associated with a specific socket option.
   * This function uses the same arguments as the BSD socket function 'getsockopt'.  See
   * the 'getsockopt' man pages or MSDN documentation for details.  
   * @param level an integer specifying the socket protocol level for the desired option.  
   * @param name an integer specifying the name of the desired option.  
   * @param val a void pointer referencing an object to be filled with the desired option's associated value.  
   * @param size an integer specifying the size, in bytes, of 'val'.  
   * @return a boolean value indicating success if true, failure if false.  
   */
  bool getSockOpt(int32_t level,int32_t name,void* val,socklen_t* size) const;

  ///Set socket option.  
  /**
   * Sets the value associated with a specific socket option.  
   * This function uses the same arguments as the BSD socket function 'setsockopt'.  See
   * the 'setsockopt' man pages or MSDN documentation for details.  
   * @param level an integer specifying the socket protocol level for the desired option.  
   * @param name an integer specifying the name of the desired option.  
   * @param val a void pointer referencing an object filled with the value for the desired option.  
   * @param size an integer specifying the size, in bytes, of 'val'.  
   * @return a boolean value indicating success if true, failure if false.  
   */
  bool setSockOpt(int32_t level,int32_t name,const void* val,socklen_t size);

  ///Indicates blocking I/O mode.  
  /**
   * Indicates that the socket is or is not using blocking I/O.  
   * @return a boolean value indicating that the socket is using blocking I/O if true, is
   *         not using blocking I/O if false.  
   */
  bool isBlockingIO() const;

  ///Set blocking I/O mode.  
  /**
   * Sets the socket's I/O mode to blocking or non-blocking.
   * @param block a boolean value indicating that the socket should use blocking I/O if true, non-blocking 
            I/O if false.  
   * @return a boolean value indicating that I/O mode was successfully changed if true, was
   *         not successfully changed if false.  
   */
  bool setBlockingIO(bool block);
    
  //These might need to be static, just to indicate that they are
  //not unique to any one socket, but are global to the application
  ///Get socket error code.  
  /**
   * Returns the last known network error value.  This function is not specific to any 
   * one socket.  The error value retrieved can be for any socket that is part of the 
   * overall application.  
   * @return an error code indicating the reason for socket failure.  
   */
  int32_t getLastError() const;

  ///Set the socket error code.  
  /**
   * Sets the socket error code to the specified value.  All sockets share the same 
   * instance of the error code.  
   * @param error an integer specifying the value to which the error value is to be set.
   */
  void setLastError(int32_t error);

  ///Open socket.  
  /**
   * Binds socket and, depending on socket type, connects to peer or listens for connections.  
   * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.  
   */
  virtual bool open()=0;

  ///Close socket.  
  /**
   * Close an open socket.  
   * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.  
   */
  virtual bool close();

  ///Shutdown one or both of the socket communication streams (input/output).  
  /**
   * Shutdown either or both of the input and output streams of a socket, without closing the socket.  
   * @param howto an integer indicating which stream to shutdown.  
   *        SHUT_RD for input,
   *        SHUT_WR for output,
   *        SHUTRDWR for both.  
   * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.  
   */
  virtual bool shutdown(int32_t howto);

  ///Bind socket.  
  /**
   * Bind the socket to the specified port an network interface.  
   * @param port an unsigned 16-bit integer specifying the port to bind to.  To let kernel
   *        pick the port use 'EPHEMERAL_PORT'.  
   * @param iface a character string specifying the network address of the network interface to
   *        use for data transmission.  To use any network interface (INADDR_ANY), set to NULL.  
   * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.  
   */
  bool bind(uint16_t port=EPHEMERAL_PORT,const char* iface=NULL);

  ///Bind socket.  
  /**
   * Bind the socket with the specified socket address data.  
   * @param localaddr a socket address object specifying the values to be used when binding the socket.  
   * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.  
   */
  virtual bool bind(const SockAddrInet* localaddr);

  ///Indicates data is available for reading.  
  /**
   * Indicates that data is avialable in the socket buffer for reading.  
   * @param sec an unsigned integer specifying the number of seconds to wait before returning.  
   * @param usec an unsinged integer specifying the number of microseconds to wait before returning.  
   * @return a boolean value indicating that data is available if true, no data is available if false.  
   *         Will always return false if the socket is invalid/not open.  
   */
  virtual bool readReady(uint32_t sec=0,uint32_t usec=0) const;

  ///Indicates space is available for writing.  
  /**
   * Indicates that space in the socket buffer is avialable for writing.  
   * @param sec an unsigned integer specifying the number of seconds to wait before returning.  
   * @param usec an unsinged integer specifying the number of milliseconds to wait before returning.  
   * @return a boolean value indicating that space is available if true, no space is available if false.  
   *         Will always return false if the socket is invalid/not open.  
   */
  virtual bool writeReady(uint32_t sec=0,uint32_t usec=0) const;

  ///Indicates that an error was encountered.  
  /**
   * Indicates that am error was encountered by the socket object.  
   * @param sec an unsigned integer specifying the number of seconds to wait before returning.  
   * @param usec an unsinged integer specifying the number of milliseconds to wait before returning.  
   * @return a boolean value indicating that an error was encountered if true, no error was encountered if false.  
   *         Will always return false if the socket is invalid/not open.  
   */
  virtual bool errorCheck(uint32_t sec=0,uint32_t usec=0) const;

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
};

#endif
