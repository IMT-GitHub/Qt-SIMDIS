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
#include "SockTCP.h"
#include "SockServer.h"

SockServer::SockServer()
: listening_(false),
  backlog_(32)
{
  create();
}

SockServer::SockServer(uint16_t port,const char* iface,int32_t backlog)
: SockTCP(port,iface),
  listening_(false),
  backlog_(backlog)
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

      if(::listen(sockfd_,backlog_)==SOCKET_ERROR)
      {
	closeSocketErr(sockfd_);
	sockfd_=INVALID_SOCKET;
      }
      else
        listening_=true;
    }
  }
}

SockServer::SockServer(const SockAddrInet* localaddr,int32_t backlog)
: SockTCP(localaddr),
  listening_(false),
  backlog_(backlog)
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

      if(::listen(sockfd_,backlog_)==SOCKET_ERROR)
      {
	closeSocketErr(sockfd_);
	sockfd_=INVALID_SOCKET;
      }
      else
        listening_=true;
    }
  }
}

SockServer::SockServer(SOCKET sockfd)
: SockTCP(sockfd),
  listening_(false),
  backlog_(32)
{
  //SockInet takse care of setting fd, addr, port, and family
  //We do not know if this is already a listening socket
  //And we have no way of knowing the backlog
  //We could either disallow this constructor, or call listen again
  //To either make it a listening socket if it is not listening
  //Or to update it's backlog to a known value if it is listening
}

bool SockServer::isActive() const
{
  return listening_;
}

bool SockServer::isListening() const
{
  return listening_;
}

int32_t SockServer::getBacklog() const
{
  return backlog_;
}

bool SockServer::create()
{
  sockfd_=socket(AF_INET,SOCK_STREAM,0);
  if(sockfd_==INVALID_SOCKET)
    return false;

#ifndef WIN32
  //Defy TIME_WAIT on UNIX.  Allows socket to bind to port immediately after another has closed, without wating.
  //On windows time wait does not seem to matter and setting this just allows multiple binds to the port.
  int32_t reuse=1;
  setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,(const void*)&reuse,sizeof(reuse));
#endif

  return true;
}

bool SockServer::open()
{
  return create();
}

bool SockServer::close()
{
  listening_=false;
  return SockTCP::close();
}

bool SockServer::listen(int32_t backlog)
{
  backlog_=backlog;
  if(::listen(sockfd_,backlog)==SOCKET_ERROR)
    listening_=false;
  else
    listening_=true;
  return listening_;
}

SOCKET SockServer::accept()
{
  return ::accept(sockfd_,NULL,NULL);
}
