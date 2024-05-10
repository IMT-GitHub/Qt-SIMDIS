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
#include "SockException.h"
#include "SockAddrInet.h"

SockAddrInet::SockAddrInet()
: port_(EPHEMERAL_PORT)
{
  memset(addr_,0,INET_ADDRSTRLEN);
  memset(&sa_,0,sizeof(SOCKADDR_IN));
  sa_.sin_family=AF_INET;
  sa_.sin_port=EPHEMERAL_PORT;
  sa_.sin_addr.s_addr=IN4ADDR_WILDCARD;
}

SockAddrInet::SockAddrInet(uint16_t port,const char* addr)
{
  memset(addr_,0,INET_ADDRSTRLEN);
  memset(&sa_,0,sizeof(SOCKADDR_IN));
  sa_.sin_family=AF_INET;
  setPort(port);
  setAddress(addr);
}

void SockAddrInet::copy(const SockAddrInet* source)
{
  if(source==NULL)
  {
    setSockErr(PLATERR(EFAULT));
    throw SockValueException("socket address pointer is NULL");
  }

  if(source->getFamily()!=AF_INET)
  {
    setSockErr(PLATERR(EINVAL));
    throw SockValueException("address families do not match");
  }

  SockAddrInet* isource=(SockAddrInet*)source;
  port_=isource->port_;
  memcpy(addr_,isource->addr_,INET_ADDRSTRLEN);
  memcpy(&sa_,&isource->sa_,sizeof(SOCKADDR_IN));
}

void SockAddrInet::copy(const SOCKADDR* source,socklen_t len)
{
  if(source==NULL)
  {
    setSockErr(PLATERR(EFAULT));
    throw SockValueException("socket address pointer is NULL");
  }

  if(len!=sizeof(SOCKADDR_IN))
  {
    setSockErr(PLATERR(EFAULT));
    throw SockValueException("socket address structure is wrong size");
  }

  if(source->sa_family!=AF_INET)
  {
    setSockErr(PLATERR(EINVAL));
    throw SockValueException("address families do not match");
  }

  SOCKADDR_IN* isource=(SOCKADDR_IN*)source;
  port_=ntohs(isource->sin_port);
  if(ntohl(isource->sin_addr.s_addr) == INADDR_ANY)
  {
    memset(addr_,0,INET_ADDRSTRLEN);
  }
  else
  {
    char* ntoa=inet_ntoa(isource->sin_addr);
    if(ntoa==NULL)  // Silently ignore inet_ntoa failure
      memset(addr_,0,INET_ADDRSTRLEN);
    else
      strncpy(addr_,ntoa,INET_ADDRSTRLEN);
  }
  memcpy(&sa_,isource,sizeof(SOCKADDR_IN));
}

uint16_t SockAddrInet::getPort() const
{
  return port_;
}

void SockAddrInet::setPort(uint16_t port)
{
  port_=port;
  sa_.sin_port=htons(port);
}

char* SockAddrInet::getAddress(char* addr,int32_t len) const
{
  if(addr==NULL)
  {
    setSockErr(PLATERR(EFAULT));
    throw SockValueException("address pointer is NULL");
  }
  else
  {
    strncpy(addr,addr_,len-1);
    addr[len-1]='\0';
  }
  return addr;
}

void SockAddrInet::setAddress(const char* addr)
{
  if(addr==NULL||addr[0]=='\0')
  {
    memset(addr_,0,INET_ADDRSTRLEN);
    sa_.sin_addr.s_addr=IN4ADDR_WILDCARD;
  }
  else
  {
    strncpy(addr_,addr,INET_ADDRSTRLEN-1);
    addr_[INET_ADDRSTRLEN-1]='\0';
    sa_.sin_addr.s_addr=inet_addr(addr);
  }
}

int32_t SockAddrInet::getFamily() const
{
  return AF_INET;
}

const SOCKADDR* SockAddrInet::getSockAddr() const
{
  return (const SOCKADDR*)&sa_;
}

int32_t SockAddrInet::getSockAddrLen() const
{
  return sizeof(SOCKADDR_IN);
}
