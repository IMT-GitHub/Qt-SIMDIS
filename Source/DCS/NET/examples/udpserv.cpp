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

#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include "net.h"

int main(int argc,char** argv)
{
  int32_t i;
  SOCKADDR_IN sin;
  socklen_t slen;
  char buffer[1500];

  initializeSocket();

  SockUDP* fd[3];
  fd[0]=new SockUnicast((uint16_t)5555);
  fd[1]=new SockBroadcast((uint16_t)5556);
  fd[2]=new SockMulticast(5557,"227.2.3.45");

  for(i=0;i<3;i++)
  {
    if(!fd[i]->isValid())
    {
      printf("Failed to create socket %d: %d\n",i,fd[i]->getLastError());
      exit(1);
    }
  }

//  This would make the socket receive data from 127.0.0.1 only
//  fd[0]->connect(port,"127.0.0.1");
//  fd[1]->connect(port,"192.168.44.255");

  for(;;)
  {
    for(i=0;i<3;i++)
    {
      if(fd[i]->readReady())
      {
	slen=sizeof(sin);
	int32_t total=fd[i]->readFrom(buffer,sizeof(buffer),0,(SOCKADDR*)&sin,&slen);
	if(total==SOCKET_ERROR)
	  printf("Error reading from socket %d: %d\n",i,fd[i]->getLastError());
	else
	{
	  printf("Socket %d echoing text to %s %d: %s\n",i,inet_ntoa(sin.sin_addr),ntohs(sin.sin_port),buffer);
	  if(i<2)
	  if(fd[i]->writeTo(buffer,total,0,(const SOCKADDR*)&sin,slen)==SOCKET_ERROR)
	    printf("Error writing to socket %d: %d\n",i,fd[i]->getLastError());
	}
      }
    }

    //Sleep for a while
    struct timeval tv={0,200};
    select(0,NULL,NULL,NULL,&tv);
  }

  for(i=0;i<3;i++)
    fd[i]->close();

  finalizeSocket();

  return 0;
}
