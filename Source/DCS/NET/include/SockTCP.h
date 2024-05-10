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

#ifndef _SOCKTCP_H_
#define _SOCKTCP_H_

///TCP socket class.  
/**
 * An abstract class specifying a TCP internet socket with  
 * reliable two-way network communication.  
 */
class SockTCP : public SockInet
{
public:
  ///Default socket constructor.  
  /**
   * Creates a socket object with default data values.  
   */
  SockTCP();

  ///Socket constructor.  
  /**
   * Creates a socket object with the specified data values.  
   * @param port an unsigned 16-bit integer specifying the local port with which to bind the socket.  
   * @param iface a character string specifying the network interface to use for network communication.  
   */
  SockTCP(uint16_t port,const char* iface=NULL);

  ///Socket constructor.  
  /**
   * Creates a socket object with the specified data values.  
   * @param localaddr a SockAddrInet object specifying the network information to be used 
   *        when creating the socket.  
   */
  SockTCP(const SockAddrInet* localaddr);

  ///Socket constructor.  
  /**
   * Creates a socket object from the specified data value.  
   * @param sockfd a handle to a previously created socket, to be used by the socket object.  
   */
  SockTCP(SOCKET sockfd);

  //Is the socket connected or listening?
  ///Indicates an active socket.  
  /**
   * Indicates that the socket is listening if it is a server and is connected if it is a client.  
   * @return a boolean value indicating that the socket is active if true, inactive if false.  
   */
  virtual bool isActive() const=0;

  ///Open socket.  
  /**
   * @see SockInet::open
   */
  virtual bool open()=0;

  ///Read a fixed amount of data. 
  /**
   * Reads the total number of bytes specified.  
   * @param buffer a pointer to an object to be filled with the data in the socket buffer.  
   * @param size an integer specifying the size, in bytes, of 'buffer'.  The function will not return until
   *        exactly this number of bytes has been read, or an error has been encountered.  
   * @param flags an integer specifying the bit flags to be passed to the socket when reading.  
   *        These are the flags used by the BSD socket function 'recv'.  See the 'recv' man pages or
   *        MSDN documentation for more details.  
   * @return the total number of bytes read.  
   */
  virtual int32_t readAll(void* buffer,int32_t size,int32_t flags=0) const;

  ///Write a fixed amount of data. 
  /**
   * Writes the total number of bytes specified.  
   * @param buffer a pointer to an object filled with the data to be written.  
   * @param size an integer specifying the size, in bytes, of 'buffer'.  The function will not return until
   *        exactly this number of bytes has been written, or an error has been encountered.  
   * @param flags an integer specifying the bit flags to be passed to the socket when writing.  
   *        These are the flags used by the BSD socket function 'send'.  See the 'send' man pages or
   *        MSDN documentation for more details.  
   * @return the total number of bytes written.  
   */
  virtual int32_t writeAll(const void* buffer,int32_t size,int32_t flags=0) const;
};

#endif
