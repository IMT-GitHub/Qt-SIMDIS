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
#include "SockUnicast.h"

SockUnicast::SockUnicast()
{
  create();
}

SockUnicast::SockUnicast(uint16_t port,const char* iface)
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

SockUnicast::SockUnicast(uint16_t peerport,const char* peeraddr,uint16_t port,const char* iface,bool attach)
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

SockUnicast::SockUnicast(const SockAddrInet* localaddr)
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

SockUnicast::SockUnicast(const SockAddrInet* peeraddr,const SockAddrInet* localaddr,bool attach)
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

SockUnicast::SockUnicast(SOCKET sockfd)
: SockUDP(sockfd)
{
  //SockUDP handles everything
}

bool SockUnicast::create()
{
  sockfd_=socket(AF_INET,SOCK_DGRAM,0);
  return (sockfd_==INVALID_SOCKET)?false:true;
}

bool SockUnicast::open()
{
  return create();
}
