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
#include "SockMonitor.h"

SockMonitor::SockMonitor()
{
}

SockMonitor::~SockMonitor()
{
}

bool SockMonitor::addReadMonitor(SockInet* sock)
{
  std::pair<SocketMap::iterator, bool> result = readsocks_.insert(std::make_pair(sock->getDescriptor(), sock));
  return result.second;
}

bool SockMonitor::addWriteMonitor(SockInet* sock)
{
  std::pair<SocketMap::iterator, bool> result = writesocks_.insert(std::make_pair(sock->getDescriptor(), sock));
  return result.second;
}

bool SockMonitor::addErrorMonitor(SockInet* sock)
{
  std::pair<SocketMap::iterator, bool> result = errorsocks_.insert(std::make_pair(sock->getDescriptor(), sock));
  return result.second;
}

bool SockMonitor::removeReadMonitor(SockInet* sock)
{
  return readsocks_.erase(sock->getDescriptor()) > 0 ? true : false;
}

bool SockMonitor::removeWriteMonitor(SockInet* sock)
{
  return writesocks_.erase(sock->getDescriptor()) > 0 ? true : false;
}

bool SockMonitor::removeErrorMonitor(SockInet* sock)
{
  return errorsocks_.erase(sock->getDescriptor()) > 0 ? true : false;
}

void SockMonitor::getReadSockets(SocketList& socks) const
{
  SocketMap::const_iterator iter;
  for(iter = readsocks_.begin(); iter != readsocks_.end(); ++iter)
    socks.push_back(iter->second);
}

void SockMonitor::getWriteSockets(SocketList& socks) const
{
  SocketMap::const_iterator iter;
  for(iter = writesocks_.begin(); iter != writesocks_.end(); ++iter)
    socks.push_back(iter->second);
}

void SockMonitor::getErrorSockets(SocketList& socks) const
{
  SocketMap::const_iterator iter;
  for(iter = errorsocks_.begin(); iter != errorsocks_.end(); ++iter)
    socks.push_back(iter->second);
}

int32_t SockMonitor::watchRead(SocketList& readsocks, uint32_t sec,uint32_t usec)
{
  int32_t result = 0;
  if (!readsocks_.empty())
  {
    SocketMap::iterator iter;
    SOCKET maxfd = 0;
    struct timeval timeout={sec,usec};
    fd_set readset;

    FD_ZERO(&readset);

    for(iter = readsocks_.begin(); iter != readsocks_.end(); ++iter)
    {
      FD_SET(iter->first,&readset);
      if(maxfd < iter->first)
        maxfd = iter->first;
    }

  #ifdef WIN32
    result = select(0, &readset, NULL, NULL, &timeout);
  #else
    result = select(maxfd+1, &readset, NULL, NULL, &timeout);
  #endif /* WIN32 */

    if(result > 0)
    {
      for(iter = readsocks_.begin(); iter != readsocks_.end(); ++iter)
      {
        if(FD_ISSET(iter->first,&readset))
          readsocks.push_back(iter->second);
      }
    }
  }

  return result;
}

int32_t SockMonitor::watchWrite(SocketList& writesocks, uint32_t sec,uint32_t usec)
{
  int32_t result = 0;
  if (!writesocks_.empty())
  {
    SocketMap::iterator iter;
    SOCKET maxfd = 0;
    struct timeval timeout={sec,usec};
    fd_set writeset;

    FD_ZERO(&writeset);

    for(iter = writesocks_.begin(); iter != writesocks_.end(); ++iter)
    {
      FD_SET(iter->first,&writeset);
      if(maxfd < iter->first)
        maxfd = iter->first;
    }

  #ifdef WIN32
    result = select(0, NULL, &writeset, NULL, &timeout);
  #else
    result = select(maxfd+1, NULL, &writeset, NULL, &timeout);
  #endif /* WIN32 */

    if(result > 0)
    {
      for(iter = writesocks_.begin(); iter != writesocks_.end(); ++iter)
      {
        if(FD_ISSET(iter->first,&writeset))
          writesocks.push_back(iter->second);
      }
    }
  }

  return result;
}

int32_t SockMonitor::watchError(SocketList& errorsocks, uint32_t sec,uint32_t usec)
{
  int32_t result = 0;
  if (!errorsocks_.empty())
  {
    SocketMap::iterator iter;
    SOCKET maxfd = 0;
    struct timeval timeout={sec,usec};
    fd_set errorset;

    FD_ZERO(&errorset);

    for(iter = errorsocks_.begin(); iter != errorsocks_.end(); ++iter)
    {
      FD_SET(iter->first,&errorset);
      if(maxfd < iter->first)
        maxfd = iter->first;
    }

  #ifdef WIN32
    result = select(0, NULL, NULL, &errorset, &timeout);
  #else
    result = select(maxfd+1, NULL, NULL, &errorset, &timeout);
  #endif /* WIN32 */

    if(result > 0)
    {
      for(iter = errorsocks_.begin(); iter != errorsocks_.end(); ++iter)
      {
        if(FD_ISSET(iter->first,&errorset))
          errorsocks.push_back(iter->second);
      }
    }
  }

  return result;
}

int32_t SockMonitor::watchAll(SocketList& readsocks, SocketList& writesocks, SocketList& errorsocks, uint32_t sec,uint32_t usec)
{
  SocketMap::iterator iter;
  SOCKET maxfd = 0;
  struct timeval timeout={sec,usec};
  fd_set readset;
  fd_set writeset;
  fd_set errorset;

  FD_ZERO(&readset);
  FD_ZERO(&writeset);
  FD_ZERO(&errorset);

  for(iter = readsocks_.begin(); iter != readsocks_.end(); ++iter)
  {
    FD_SET(iter->first,&readset);
    if(maxfd < iter->first)
      maxfd = iter->first;
  }

  for(iter = writesocks_.begin(); iter != writesocks_.end(); ++iter)
  {
    FD_SET(iter->first,&writeset);
    if(maxfd < iter->first)
      maxfd = iter->first;
  }

  for(iter = errorsocks_.begin(); iter != errorsocks_.end(); ++iter)
  {
    FD_SET(iter->first,&errorset);
    if(maxfd < iter->first)
      maxfd = iter->first;
  }

#ifdef WIN32
  int32_t result=select(0,&readset,&writeset,&errorset,&timeout);
#else
  int32_t result=select(maxfd+1,&readset,&writeset,&errorset,&timeout);
#endif /* WIN32 */

  if(result > 0)
  {
    for(iter = readsocks_.begin(); iter != readsocks_.end(); ++iter)
    {
      if(FD_ISSET(iter->first,&readset))
        readsocks.push_back(iter->second);
    }

    for(iter = writesocks_.begin(); iter != writesocks_.end(); ++iter)
    {
      if(FD_ISSET(iter->first,&writeset))
        writesocks.push_back(iter->second);
    }

    for(iter = errorsocks_.begin(); iter != errorsocks_.end(); ++iter)
    {
      if(FD_ISSET(iter->first,&errorset))
        errorsocks.push_back(iter->second);
    }
  }

  return result;
}
