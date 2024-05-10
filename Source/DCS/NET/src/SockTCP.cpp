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

SockTCP::SockTCP()
{
}

SockTCP::SockTCP(uint16_t port,const char* iface)
: SockInet(port,iface)
{
}

SockTCP::SockTCP(const SockAddrInet* localaddr)
: SockInet(localaddr)
{
}

SockTCP::SockTCP(SOCKET sockfd)
: SockInet(sockfd)
{
}

//Attemp to read all bytes.  In the case that the peer terminates the connection 
//gracefully, without error, we return whatever was read.  
int32_t SockTCP::readAll(void* buffer,int32_t size,int32_t flags) const
{
  int32_t read=0,result=0;

#if defined(MSG_WAITALL) && !defined(WIN32)
  if(!(flags&MSG_WAITALL))
    flags|=MSG_WAITALL;

  for(;;)
  {
    result=recv(sockfd_,PLATOPTCAST(buffer),size,flags);
    if(result==SOCKET_ERROR&&getSockErr()==PLATERR(EINTR))
      continue;
    else
      break;
  }
  read=result;
#else //Do it manually
  char* dataptr=(char*)buffer;

  do
  {
    //Use PLATOPTCAST because UNIX needs the cast to void*
    result=recv(sockfd_,PLATOPTCAST(dataptr+read),size-read,flags);
    if(result==SOCKET_ERROR)
    {
      if(getSockErr()==PLATERR(EINTR))
	result=0;
      else
        return result;
    }
    else if (result==0)
      break;

    read+=result;
  }
  while(read!=size);
#endif
  return read;
}

int32_t SockTCP::writeAll(const void* buffer,int32_t size,int32_t flags) const
{
  int32_t sent=0,result=0;
  const char* dataptr=(const char*)buffer;
  do
  {
    //Use PLATOPTCONSTCAST because UNIX needs the cast to const void*
    result=send(sockfd_,PLATOPTCONSTCAST(dataptr+sent),size-sent,flags);
    if(result==SOCKET_ERROR||result==0)
    {
      if(getSockErr()==PLATERR(EINTR))
	result=0;
      else
        return result;
    }
    sent+=result;
  }
  while(sent!=size);
  return sent;
}
