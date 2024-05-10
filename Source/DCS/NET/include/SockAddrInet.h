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

#ifndef _SOCKADDRINET_H_
#define _SOCKADDRINET_H_

///Socket address class.  
/**
 * An internet socket address class containing a network address and
 * a TCP/UDP port number.  
 */
class SockAddrInet
{
  friend class SockAddrFactory;
protected:
  ///Port number.  
  uint16_t port_;			    /**< A 16-bit TCP od UDP port number.  */

  ///Network address.  
  char addr_[INET_ADDRSTRLEN];		    /**< IPv4 network address, represented as a string (eg 132.250.132.147).  */

  ///Socket address structure.  
  SOCKADDR_IN sa_;			    /**< IPv4 socket address structure used by BSD socket routines.  */

  ///Default socket address constructor.  
  /**
   * Creates an event object with default data values.  Can not be accessed directly, 
   * but is used by SockAddrFactory.  
   * @see SockAddrFactory::create
   */
  SockAddrInet();
public:

  ///Socket address constructor.  
  /**
   * Creates a socket address object with the specified values.  
   * @param port an unsigned 16-bit integer specifying a TCP or UDP port number.  
   * @param addr a character string specifying an IPv4 network address.  
   */
  SockAddrInet(uint16_t port,const char* addr);
 
  ///Copy socket address object.  
  /**
   * Copies the contents of a socket address object into this socket address object.  
   * @param source a socket address object whose contents are to be copied.  
   */
  void copy(const SockAddrInet* source);

  ///Copy socket address object.  
  /**
   * Copies the contents of a socket address object into this socket address object.  
   * @param source a standard socket address structure whose contents are to be copied.  
   * @param len an integer specifying the size of 'source' in bytes.  
   */
  void copy(const SOCKADDR* source,socklen_t len);

  ///Retrieve port value.  
  /**
   * Returns value of port.  
   * @return an unsigned 16-bit integer specifying a TCP or UDP port number.  
   * @see port_
   */
  uint16_t getPort() const;

  ///Set port value.  
  /**
   * Sets value of port.  
   * @param port an unsigned 16-bit integer specifying the TCP or UDP port number with which 
   *        the port value is to be set.  
   * @see port_
   */
  void setPort(uint16_t port);

  ///Retrieve address value.  
  /**
   * Returns value of network address.  
   * @param addr a character array to be filled with the IPv4 network address.  
   * @param len an integer specifying the size, in bytes, of 'addr'.  
   * @return a pointer to an array of characters containing the IPv4 network address.  
   * @see addr_
   */
  char* getAddress(char* addr,int32_t len) const;

  ///Set address value.  
  /**
   * Sets value of network address.  
   * @param addr a character string specifying the IPv4 network address with which 
   *        the network address value is to be set.  
   * @see addr_
   */
  void setAddress(const char* addr);

  ///Retrieve the socket address family.  
  /**
   * Returns the socket address family type.  
   * @return an integer specifying the socket address family.  Currently only AF_INET
   *         is supported.  
   */
  int32_t getFamily() const;

  ///Retrieve the socket address structure.  
  /**
   * Returns the socket address structure.  
   * @return the SOCKADDR structure containing socket address information.  
   * @see sa_
   */
  const SOCKADDR* getSockAddr() const;

  ///Retrieve the socket address structure size.  
  /**
   * Returns the size of the socket address structure.  
   * @return an integer specifying the size of the socket address structure.  
   * @see sa_
   */
  int32_t getSockAddrLen() const;
};

#endif
