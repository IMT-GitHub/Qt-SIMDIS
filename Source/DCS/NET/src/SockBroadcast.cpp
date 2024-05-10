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
#include "SockAddrInet.h"
#include "SockIO.h"
#include "SockInet.h"
#include "SockUDP.h"
#include "SockBroadcast.h"

SockBroadcast::SockBroadcast()
{
  create();
}

SockBroadcast::SockBroadcast(uint16_t port,const char* iface)
: SockUDP(port,iface)
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

SockBroadcast::SockBroadcast(uint16_t peerport,const char* peeraddr,uint16_t port,const char* iface,bool attach)
: SockUDP(peerport,peeraddr,port,iface,attach)
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

    if(attach)
      attached_=true;
    else
    {
      if(::connect(sockfd_,peeraddr_->getSockAddr(),peeraddr_->getSockAddrLen())==SOCKET_ERROR)
      {
        closeSocketErr(sockfd_);
        sockfd_=INVALID_SOCKET;
      }
      else
      {
	// Get local port and interface for connected socket
	SOCKADDR_IN sin;
	socklen_t slen=sizeof(sin);
	getsockname(sockfd_, (SOCKADDR*)&sin, &slen);
	localaddr_->copy((const SOCKADDR*)&sin,slen);

        connected_=true;
      }
    }
  }
}

SockBroadcast::SockBroadcast(const SockAddrInet* localaddr)
: SockUDP(localaddr)
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

SockBroadcast::SockBroadcast(const SockAddrInet* peeraddr,const SockAddrInet* localaddr,bool attach)
: SockUDP(peeraddr,localaddr,attach)
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
      // Get local port and interface for connected socket
      SOCKADDR_IN sin;
      socklen_t slen=sizeof(sin);
      getsockname(sockfd_, (SOCKADDR*)&sin, &slen);
      localaddr_->copy((const SOCKADDR*)&sin,slen);
    }

    if(attach)
      attached_=true;
    else
    {
      if(::connect(sockfd_,peeraddr_->getSockAddr(),peeraddr_->getSockAddrLen())==SOCKET_ERROR)
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

        connected_=true;
      }
    }
  }
}

SockBroadcast::SockBroadcast(SOCKET sockfd)
: SockUDP(sockfd)
{
  //SockUDP handles everything
}

bool SockBroadcast::create()
{
  sockfd_=socket(AF_INET,SOCK_DGRAM,0);
  if(sockfd_==INVALID_SOCKET)
    return false;

  int32_t enable=1;
  setsockopt(sockfd_,SOL_SOCKET,SO_BROADCAST,PLATOPTCONSTCAST(&enable),sizeof(enable));
#ifdef SO_REUSEPORT
  setsockopt(sockfd_,SOL_SOCKET,SO_REUSEPORT,PLATOPTCONSTCAST(&enable),sizeof(enable));
#else
  setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,PLATOPTCONSTCAST(&enable),sizeof(enable));
#endif
  return true;
}

bool SockBroadcast::open()
{
  return create();
}
