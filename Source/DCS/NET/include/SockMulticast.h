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

#ifndef _SOCKMULTICAST_H_
#define _SOCKMULTICAST_H_

///UDP multicast socket class.  
/**
 * A socket that utilizes the network multicast protocol for 
 * data transmission.  
 */
class SockMulticast : public SockUDP
{
private:
  bool create();
protected:
  ///Time to live.  
  TTL_T ttl_;			/**< The time to live value for data transmissions.  */
public:
  ///Default multicast socket constructor.  
  /**
   * Creates a multicast socket object with default data values.  
   */
  SockMulticast();

  ///Multicast socket constructor.  
  /**
   * Creates a multicast socket object with the specified data values.  
   * @param port an unsigned 16-bit integer specifying the port to bind 
   *        the multicast socket to.  
   * @param ttl an integer value specifying the time to live value to use 
   *        for network transmission.  
   */
  SockMulticast(uint16_t port,TTL_T ttl=1);

  ///Multicast socket constructor.  
  /**
   * Creates a multicast socket object with the specified data values, and connects it to a
   * specified multicast address.  When a multicast socket object is connected to a multicast address, 
   * data can be sent to that address with calls to 'write' rather than calls to 'writeTo'.  When a multicast 
   * socket is connected it can only communicate on that multicast address.  
   * @param port an unsigned 16-bit integer specifying the port to which 
   *        the multicast socket should bind.  
   * @param group a character string specifying the multicast address to which 
   *        the multicast socket should bind.  
   * @param mciface a character string specifying the network interface to use
   *        for network transmission.  
   * @param ttl an integer value specifying the time to live value to use 
   *        for network transmission.  
   * @param attach a boolean value specifying whether the broadcast socket is to be attached
   *        to the peer or connected to the peer.  When the socket is attached, the broadcast socket
   *        object stores the peerport and peeraddr values and uses them to call 'sendto'.  If the 
   *        socket is connected, the broadcast socket object links the socket to the peer on the
   *        kernel level with a call to the 'connect' function and sends data witn 'send'.  
   * @see SockUDP::attached_
   * @see SockUDP::write
   * @see SockUDP::writeTo
   */
  SockMulticast(uint16_t port,const char* group,const char* mciface=NULL,TTL_T ttl=1,bool attach=true);

  ///Open socket connection.  
  /**
   * @see SockInet::open
   */
  bool open();

  ///Retrieve TTL.  
  /**
   * Returns the current multicast socket object's time to live value.  
   * @return an integer specifying the socket's TTL.  
   */
  TTL_T getTTL() const;

  ///Set TTL.  
  /**
   * Sets the current multicast socket object's time to live value.  
   * @param ttl an integer specifying the new TTL value to be used.  
   * @return a boolean value indicating that the operation succeeded if true, failed if false.  
   */
  bool setTTL(TTL_T ttl);

  ///Retrieve network interface value.  
  /**
   * Returns value of network interface.  
   * @param iface a character array to be filled with the IPv4 network address associated with the network interface.  
   * @param len an integer specifying the size, in bytes, of 'addr'.  
   * @return a pointer to an array of characters containing the IPv4 network address for the associated network interface.  
   */
  char* getMcastIface(char* iface,int32_t len) const;

  ///Set network interface value.  
  /**
   * Sets the value of the network interface.  
   * @param iface a character string specifying the network interface to use
   *        for network transmission.  
   * @return a boolean value indicating that the operation succeeded if true, failed if false.  
   */
  bool setMcastIface(const char* iface);

  ///Join multicast group.  
  /**
   * Joins a specified multicast group.  
   * @param group a character string specifying the multicast address for the group to join.  
   * @return a boolean value indicating that the operation succeeded if true, failed if false.  
   */
  bool joinGroup(const char* group);

  ///Leave multicast group.  
  /**
   * Leaves a specified multicast group.  
   * @param group a character string specifying the multicast address for the group to leave.  
   * @return a boolean value indicating that the operation succeeded if true, failed if false.  
   */
  bool leaveGroup(const char* group);
};

#endif
