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
#include "SockUDP.h"

SockUDP::SockUDP()
: SockInet(),
  attached_(false),
  connected_(false)
{
  peeraddr_=(SockAddrInet*)SockAddrFactory::create(AF_INET);
}

SockUDP::SockUDP(uint16_t port,const char* iface)
: SockInet(port,iface),
  attached_(false),
  connected_(false)
{
  peeraddr_=(SockAddrInet*)SockAddrFactory::create(AF_INET);
}

SockUDP::SockUDP(uint16_t peerport,const char* peeraddr,uint16_t port,const char* iface,bool attach)
: SockInet(port,iface),
  attached_(attach),
  connected_(!attach)
{
  peeraddr_=(SockAddrInet*)SockAddrFactory::create(AF_INET);
  peeraddr_->setPort(peerport);
  peeraddr_->setAddress(peeraddr);
}

SockUDP::SockUDP(const SockAddrInet* localaddr)
: SockInet(localaddr),
  attached_(false),
  connected_(false)
{
  peeraddr_=(SockAddrInet*)SockAddrFactory::create(AF_INET);
}

SockUDP::SockUDP(const SockAddrInet* peeraddr,const SockAddrInet* localaddr,bool attach)
: SockInet(localaddr),
  attached_(attach),
  connected_(!attach)
{
  peeraddr_=(SockAddrInet*)SockAddrFactory::create(AF_INET);
  peeraddr_->copy(peeraddr);
}

SockUDP::SockUDP(SOCKET sockfd)
: SockInet(sockfd),
  attached_(false),
  connected_(false)
{
  if(sockfd!=INVALID_SOCKET)
  {
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

SockUDP::~SockUDP()
{
  if(isValid()) close();
  if (peeraddr_)
    delete peeraddr_;
}

bool SockUDP::isConnected() const
{
  return connected_;
}

bool SockUDP::isAttached() const
{
  return attached_;
}

uint16_t SockUDP::getPort() const
{
  return peeraddr_->getPort();
}

char* SockUDP::getAddress(char* peeraddr,int32_t len) const
{
  return peeraddr_->getAddress(peeraddr,len);
}

int32_t SockUDP::read(void* buffer,int32_t size,int32_t flags) const
{
  if(attached_)
    return readFrom(buffer,size,flags,NULL,NULL);
  else
    return SockInet::read(buffer,size,flags);
}

int32_t SockUDP::write(const void* buffer,int32_t size,int32_t flags) const
{
  if(attached_)
    return writeTo(buffer,size,flags,peeraddr_->getSockAddr(),peeraddr_->getSockAddrLen());
  else
    return SockInet::write(buffer,size,flags);
}

int32_t SockUDP::readFrom(void* buffer,int32_t size,int32_t flags,SOCKADDR* from,socklen_t* addrlen) const
{
  return recvfrom(sockfd_,PLATDATACAST(buffer),size,flags,from,addrlen);
}

int32_t SockUDP::writeTo(const void* buffer,int32_t size,int32_t flags,const SOCKADDR* to,socklen_t addrlen) const
{
  return sendto(sockfd_,PLATDATACONSTCAST(buffer),size,flags,to,addrlen);
}

bool SockUDP::close()
{
  peeraddr_->setPort(EPHEMERAL_PORT);
  peeraddr_->setAddress(NULL);
  connected_=false;
  attached_=false;
  return SockInet::close();
}

bool SockUDP::connect(uint16_t peerport,const char* peeraddr)
{
  SOCKADDR_IN sin;
  socklen_t slen=sizeof(sin);

  peeraddr_->setPort(peerport);
  peeraddr_->setAddress(peeraddr);
  if(::connect(sockfd_,peeraddr_->getSockAddr(),peeraddr_->getSockAddrLen())==SOCKET_ERROR)
    return false;
  connected_=true;

  // Get local port and interface for newly connected socket
  getsockname(sockfd_, (SOCKADDR*)&sin, &slen);
  localaddr_->copy((const SOCKADDR*)&sin,slen);

  return true;
}

bool SockUDP::attach(uint16_t peerport,const char* peeraddr)
{
  peeraddr_->setPort(peerport);
  peeraddr_->setAddress(peeraddr);
  attached_=true;
  return true;
}

bool SockUDP::disconnect()
{
  SOCKADDR sa;
  sa.sa_family=AF_UNSPEC;
  int32_t result=::connect(sockfd_,(const SOCKADDR*)&sa,sizeof(sa));

  //It is okay if the error is unsopported address family
  if(result==SOCKET_ERROR&&getSockErr()!=PLATERR(EAFNOSUPPORT))
    return false;
  peeraddr_->setPort(EPHEMERAL_PORT);
  peeraddr_->setAddress(NULL);
  connected_=false;
  return true;
}

bool SockUDP::detatch()
{
  peeraddr_->setPort(EPHEMERAL_PORT);
  peeraddr_->setAddress(NULL);
  attached_=false;
  return true;
}
