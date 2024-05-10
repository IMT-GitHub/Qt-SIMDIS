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
#include "SockAddrFactory.h"
#include "SockAddrInet.h"
#include "SockIO.h"
#include "SockInet.h"
#include "SockTCP.h"
#include "SockClient.h"

SockClient::SockClient()
: connected_(false)
{
  peeraddr_=(SockAddrInet*)SockAddrFactory::create(AF_INET);
  create();
}

SockClient::SockClient(uint16_t peerport,const char* peeraddr,uint16_t port,const char* iface)
: SockTCP(port,iface),
  connected_(false)
{
  peeraddr_=(SockAddrInet*)SockAddrFactory::create(AF_INET);
  peeraddr_->setPort(peerport);
  peeraddr_->setAddress(peeraddr);

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
      connected_ = true;
    }
  }
}

SockClient::SockClient(SockAddrInet* peeraddr,SockAddrInet* localaddr)
  : SockTCP(localaddr),
  connected_(false)
{
  peeraddr_=(SockAddrInet*)SockAddrFactory::create(AF_INET);
  peeraddr_->copy(peeraddr);

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
      connected_ = true;
    }
  }
}

SockClient::SockClient(SOCKET sockfd)
: SockTCP(sockfd),
  connected_(false)
{
  if(sockfd!=INVALID_SOCKET)
  {
    //check for peer
    SOCKADDR_IN sin;
    socklen_t slen=sizeof(sin);

    peeraddr_=(SockAddrInet*)SockAddrFactory::create(AF_INET);
    if(getpeername(sockfd,(SOCKADDR*)&sin,&slen)!=SOCKET_ERROR)
    {
      peeraddr_->copy((const SOCKADDR*)&sin,slen);
      connected_=true;
    }
  }
  else
    peeraddr_=(SockAddrInet*)SockAddrFactory::create(AF_INET);
}

SockClient::~SockClient()
{
  if(isValid()) close();
  if (peeraddr_)
    delete peeraddr_;
}

bool SockClient::isActive() const
{
  return connected_;
}

bool SockClient::isConnected() const
{
  return connected_;
}

uint16_t SockClient::getPort() const
{
  return peeraddr_->getPort();
}

const char* SockClient::getAddress(char* peeraddr,int32_t len) const
{
  return peeraddr_->getAddress(peeraddr,len);
}

bool SockClient::create()
{
  sockfd_=socket(AF_INET,SOCK_STREAM,0);
  return (sockfd_==INVALID_SOCKET)?false:true;
}

bool SockClient::open()
{
  return create();
}

bool SockClient::close()
{
  peeraddr_->setPort(EPHEMERAL_PORT);
  peeraddr_->setAddress(NULL);
  connected_=false;
  return SockTCP::close();
}

// Returning an enumeration type to indicate failure, immediate connect, or non-blocking operation in progress
// could be useful here to inidcate actual connect state, but may break consistency of API and would deviate from 
// effort to mirror functionality of BSD socket library
bool SockClient::connect(uint16_t peerport,const char* peeraddr)
{
  SOCKADDR_IN sin;
  socklen_t slen=sizeof(sin);

  peeraddr_->setPort(peerport);
  peeraddr_->setAddress(peeraddr);
  if(::connect(sockfd_,peeraddr_->getSockAddr(),peeraddr_->getSockAddrLen())==SOCKET_ERROR)
  {
    // If mode is non-blocking and connection is pending, set connected to true
#ifdef WIN32
    if(!block_ && (getSockErr()==PLATERR(EWOULDBLOCK)))
#else
    if(!block_ && (getSockErr()==PLATERR(EINPROGRESS)))
#endif
    {
      connected_ = true;
    }

    return false;
  }

  // Get local port and interface for bound socket
  getsockname(sockfd_, (SOCKADDR*)&sin, &slen);
  localaddr_->copy((const SOCKADDR*)&sin,slen);

  connected_ = true;

  return true;
}
