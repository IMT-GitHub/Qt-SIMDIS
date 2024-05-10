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

#include <string.h>
#include "sockdefs.h"
#include "SockAddrFactory.h"
#include "SockAddrInet.h"
#include "SockIO.h"
#include "SockInet.h"
#include "SockUDP.h"
#include "SockMulticast.h"

SockMulticast::SockMulticast()
: ttl_(8)
{
  create();
}

// Note: On Linux, This constructor may not be able to properly distinguish
// between multicast messages send to the same port on different groups
// because the ::bind() call does not specify a group address.
SockMulticast::SockMulticast(uint16_t port,TTL_T ttl)
: SockUDP(port,NULL),
  ttl_(ttl)
{
  if(create())
  {
    if(::bind(sockfd_,localaddr_->getSockAddr(),localaddr_->getSockAddrLen())==SOCKET_ERROR)
    {
      closeSocketErr(sockfd_);
      sockfd_=INVALID_SOCKET;
    }
    else
    {
      // Get local port and interface for bound socket
      SOCKADDR_IN sin;
      socklen_t slen=sizeof(sin);
      getsockname(sockfd_, (SOCKADDR*)&sin, &slen);
      localaddr_->copy((const SOCKADDR*)&sin,slen);
    }
  }
}

SockMulticast::SockMulticast(uint16_t port,const char* group,const char* mciface,TTL_T ttl,bool attach)
: SockUDP(port,group,port,NULL,attach),
  ttl_(ttl)
{
  if(create())
  {
#ifndef WIN32
    // On Linux: Must bind() to the group address in order to
    // separate group-based multicast messages.  Source:
    // http://stackoverflow.com/questions/2741936/c-linux-receiving-data-from-multiple-multicast-address-on-same-port-how-to-d
    localaddr_->setAddress(group);
#endif
    if(::bind(sockfd_,localaddr_->getSockAddr(), localaddr_->getSockAddrLen())==SOCKET_ERROR)
    {
      closeSocketErr(sockfd_);
      sockfd_=INVALID_SOCKET;
    }
    else
    {
      // Get local port and interface for bound socket
      SOCKADDR_IN sin;
      socklen_t slen=sizeof(sin);
      getsockname(sockfd_, (SOCKADDR*)&sin, &slen);
      localaddr_->copy((const SOCKADDR*)&sin,slen);

      if(!joinGroup(group))
      {
        closeSocketErr(sockfd_);
        sockfd_=INVALID_SOCKET;
      }
      else
      {
        if(mciface!=NULL&&mciface[0]!='\0')
        {
          if(!setMcastIface(mciface))
          {
	    closeSocketErr(sockfd_);
	    sockfd_=INVALID_SOCKET;
          }
        }
      }
    }
  }
}

bool SockMulticast::create()
{
  sockfd_=socket(AF_INET,SOCK_DGRAM,0);
  if(sockfd_==INVALID_SOCKET)
    return false;

  int32_t reuse=1;
#ifdef SO_REUSEPORT
  setsockopt(sockfd_,SOL_SOCKET,SO_REUSEPORT,PLATOPTCONSTCAST(&reuse),sizeof(reuse));
#else
  setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,PLATOPTCONSTCAST(&reuse),sizeof(reuse));
#endif
  setsockopt(sockfd_,IPPROTO_IP,IP_MULTICAST_TTL,PLATOPTCONSTCAST(&ttl_),sizeof(ttl_));

  return true;
}

bool SockMulticast::open()
{
  return create();
}

TTL_T SockMulticast::getTTL() const
{
  return ttl_;
}

bool SockMulticast::setTTL(TTL_T ttl)
{
  if(setsockopt(sockfd_,IPPROTO_IP,IP_MULTICAST_TTL,PLATOPTCONSTCAST(&ttl),sizeof(ttl))==SOCKET_ERROR)
    return false;
  ttl_=ttl;
  return true;
}

char* SockMulticast::getMcastIface(char* iface,int32_t len) const
{
  in_addr_t iaddr=0;
  socklen_t iaddrlen=sizeof(iaddr);
  if(getsockopt(sockfd_,IPPROTO_IP,IP_MULTICAST_IF,PLATOPTCAST(&iaddr),&iaddrlen)==SOCKET_ERROR)
    return NULL;
  struct in_addr in;
  in.s_addr=iaddr;
  char* s=inet_ntoa(in);
  if(s==NULL)
    return NULL;
  strncpy(iface,s,len);
  return iface;
}

bool SockMulticast::setMcastIface(const char* iface)
{
  in_addr_t iaddr=inet_addr(iface);
  if(setsockopt(sockfd_,IPPROTO_IP,IP_MULTICAST_IF,PLATOPTCONSTCAST(&iaddr),sizeof(iaddr))==SOCKET_ERROR)
    return false;
  return true;
}

bool SockMulticast::joinGroup(const char* group)
{
  struct ip_mreq mreq;
  memset(&mreq,0,sizeof(mreq));

  mreq.imr_multiaddr.s_addr=inet_addr(group);
  mreq.imr_interface.s_addr=IN4ADDR_WILDCARD;

  if(setsockopt(sockfd_,IPPROTO_IP,IP_ADD_MEMBERSHIP,PLATOPTCONSTCAST(&mreq),sizeof(mreq))==SOCKET_ERROR)
    return false;
  return true;
}

bool SockMulticast::leaveGroup(const char* group)
{
  struct ip_mreq mreq;
  memset(&mreq,0,sizeof(mreq));

  mreq.imr_multiaddr.s_addr=inet_addr(group);
  mreq.imr_interface.s_addr=IN4ADDR_WILDCARD;

  if(setsockopt(sockfd_,IPPROTO_IP,IP_DROP_MEMBERSHIP,PLATOPTCONSTCAST(&mreq),sizeof(mreq))==SOCKET_ERROR)
    return false;
  return true;
}
