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

SockInet::SockInet()
: block_(true),
  sockfd_(INVALID_SOCKET)
{
  localaddr_=SockAddrFactory::create(AF_INET);
}

SockInet::SockInet(uint16_t port,const char* iface)
: block_(true),
  sockfd_(INVALID_SOCKET)
{
  localaddr_=SockAddrFactory::create(AF_INET);
  localaddr_->setPort(port);
  localaddr_->setAddress(iface);
}

SockInet::SockInet(const SockAddrInet* localaddr)
: block_(true),
  sockfd_(INVALID_SOCKET)
{
  localaddr_=SockAddrFactory::create(AF_INET);
  localaddr_->copy(localaddr);
}

SockInet::SockInet(SOCKET sockfd)
: block_(false),
  sockfd_(INVALID_SOCKET)
{
  if(sockfd!=INVALID_SOCKET)
  {
    SOCKADDR_IN sin;
    socklen_t slen=sizeof(sin);

    localaddr_=SockAddrFactory::create(AF_INET);
    getsockname(sockfd,(SOCKADDR*)&sin,&slen);
    localaddr_->copy((const SOCKADDR*)&sin,slen);

    sockfd_=sockfd;

    //We don't know if it's blocking or not, so make it blocking
    setBlockingIO(true);
  }
  else
  {
    localaddr_=SockAddrFactory::create(AF_INET);
    block_=true;
  }
}

SockInet::~SockInet()
{
  if(isValid()) close();
  delete localaddr_;
  localaddr_=(SockAddrInet*)-1;
}

bool SockInet::isValid() const
{
  return (sockfd_!=INVALID_SOCKET);
}

SOCKET SockInet::getDescriptor() const
{
  return sockfd_;
}

int32_t SockInet::getFamily() const
{
  return AF_INET;
}

uint16_t SockInet::getLocalPort() const
{
  return localaddr_->getPort();
}

const char* SockInet::getIface(char* iface,int32_t len) const
{
  return localaddr_->getAddress(iface,len);
}

bool SockInet::getSockOpt(int32_t level,int32_t name,void* val,socklen_t* size) const
{
  if(getsockopt(sockfd_,level,name,PLATOPTCAST(val),size)==SOCKET_ERROR)
    return false;
  return true;
}

bool SockInet::setSockOpt(int32_t level,int32_t name,const void* val,socklen_t size)
{
  if(setsockopt(sockfd_,level,name,PLATOPTCONSTCAST(val),size)==SOCKET_ERROR)
    return false;
  return true;
}

bool SockInet::isBlockingIO() const
{
  return block_;
}

bool SockInet::setBlockingIO(bool block)
{
  if(block!=block_)
  {
    if(isValid())
    {
      uint32_t mode=(block)?0:1;
      if(ioctlsocket(sockfd_,FIONBIO,(u_long *)&mode)==SOCKET_ERROR)
	return false;
    }
    block_=block;
  }
  return true;
}

int32_t SockInet::getLastError() const
{
  return getSockErr();
}

void SockInet::setLastError(int32_t error)
{
  setSockErr(error);
}

bool SockInet::close()
{
  if(closeSocketErr(sockfd_)==SOCKET_ERROR)
    return false;
  sockfd_=INVALID_SOCKET;
  block_=true;
  return true;
}

bool SockInet::shutdown(int32_t howto)
{
  if(::shutdown(sockfd_,howto)==SOCKET_ERROR)
    return false;
  return true;
}

bool SockInet::bind(const SockAddrInet* localaddr)
{
  SOCKADDR_IN sin;
  socklen_t slen=sizeof(sin);

  localaddr_->copy(localaddr);
  if(::bind(sockfd_,localaddr_->getSockAddr(),localaddr_->getSockAddrLen())==SOCKET_ERROR)
    return false;

  // Get local port and interface for bound socket
  getsockname(sockfd_, (SOCKADDR*)&sin, &slen);
  localaddr_->copy((const SOCKADDR*)&sin,slen);

  return true;
}


bool SockInet::bind(uint16_t port,const char* iface)
{
  SOCKADDR_IN sin;
  socklen_t slen=sizeof(sin);

  localaddr_->setPort(port);
  localaddr_->setAddress(iface);
  if(::bind(sockfd_,localaddr_->getSockAddr(),localaddr_->getSockAddrLen())==SOCKET_ERROR)
    return false;

  // Get local port and interface for bound socket
  getsockname(sockfd_, (SOCKADDR*)&sin, &slen);
  localaddr_->copy((const SOCKADDR*)&sin,slen);

  return true;
}

bool SockInet::readReady(uint32_t sec,uint32_t usec) const
{
  fd_set readfd;
  struct timeval timeout={sec,usec};

  if(isValid())
  {
    FD_ZERO(&readfd);
    FD_SET(sockfd_,&readfd);
#ifdef WIN32
    int32_t result=select(0,&readfd,NULL,NULL,&timeout);
#else
    int32_t result=select(sockfd_+1,&readfd,NULL,NULL,&timeout);
#endif /* WIN32 */

    //For now return true so we try to read and get an error, rather than poll for ever on a bad socket
    if(result==SOCKET_ERROR)
      return true;
    else if(result==0)
      return false;
    else
      return (FD_ISSET(sockfd_,&readfd))?true:false;
  }

  // Socket is not valid and should not be processed
  return false;
}

bool SockInet::writeReady(uint32_t sec,uint32_t usec) const
{
  fd_set writefd;
  struct timeval timeout={sec,usec};

  if(isValid())
  {
    FD_ZERO(&writefd);
    FD_SET(sockfd_,&writefd);
#ifdef WIN32
    int32_t result=select(0,NULL,&writefd,NULL,&timeout);
#else
    int32_t result=select(sockfd_+1,NULL,&writefd,NULL,&timeout);
#endif /* WIN32 */
    

    //For now return true so we try to read and get an error, rather than poll for ever on a bad socket
    if(result==SOCKET_ERROR)
      return true;
    else if(result==0)
      return false;
    else
      return (FD_ISSET(sockfd_,&writefd))?true:false;
  }

  // Socket is not valid and should not be processed
  return false;
}

bool SockInet::errorCheck(uint32_t sec,uint32_t usec) const
{
  fd_set errfd;
  struct timeval timeout={sec,usec};

  if(isValid())
  {
    FD_ZERO(&errfd);
    FD_SET(sockfd_,&errfd);
#ifdef WIN32
    int32_t result=select(0,NULL,NULL,&errfd,&timeout);
#else
    int32_t result=select(sockfd_+1,NULL,NULL,&errfd,&timeout);
#endif /* WIN32 */
    

    //For now return true so we try to read and get an error, rather than poll for ever on a bad socket
    if(result==SOCKET_ERROR)
      return true;
    else if(result==0)
      return false;
    else
      return (FD_ISSET(sockfd_,&errfd))?true:false;
  }

  // Socket is not valid and was not processed
  return false;
}

int32_t SockInet::read(void* buffer,int32_t size,int32_t flags) const
{
  return recv(sockfd_,PLATDATACAST(buffer),size,flags);
}

int32_t SockInet::write(const void* buffer,int32_t size,int32_t flags) const
{
  return send(sockfd_,PLATDATACONSTCAST(buffer),size,flags);
}
