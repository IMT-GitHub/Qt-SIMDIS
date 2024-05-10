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

#include "sockdefs.h"
#include "SockException.h"
#include "SockAddrInet.h"
#include "SockAddrFactory.h"

SockAddrInet* SockAddrFactory::create(int32_t family)
{
  switch(family)
  {
    case AF_INET:
      return new SockAddrInet;
      break;
    default:
      setSockErr(PLATERR(EAFNOSUPPORT));
      throw SockSupportException("unsupported address family");
  }
  return NULL;
}
