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

#ifndef _SOCKADDRFACTORY_H_
#define _SOCKADDRFACTORY_H_

class SockAddrInet;

///Socket address creator.  
/**
 * A class to create a socket address object for a desired socket address family.
 * Currently only AF_INET is supporter.  
 */
class SockAddrFactory
{
public:
  /**
   * Creates a new SocketAddrInet object.  
   * @param family an integer specifying the desired socket address family for the object to be 
   *        created.  Currently only AF_INET is supported.  
   * @return a newly created SocketAddrInet object.  
   * @see SockAddrInet
   */
  static SockAddrInet* create(int32_t family);
};

#endif
