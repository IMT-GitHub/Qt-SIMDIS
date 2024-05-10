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
#include <net.h>

int main(int argc,char** argv)
{
  int32_t i;
  
  if(argc!=2)
  {
    printf("Usage: udpcli address\n");
    exit(1);
  }

  initializeSocket();

  SockUDP* fd[3];
  fd[0]=new SockUnicast();//5555,argv[1],EPHEMERAL_PORT);
  fd[1]=new SockBroadcast();
  fd[2]=new SockMulticast(5557,"227.2.3.45");

//   fd[0]->connect(port,addr);
//   fd[1]->connect(port,addr);

  SOCKADDR_IN sin[3];
  sin[0].sin_family=AF_INET;
  sin[0].sin_port=htons(5555);
  sin[0].sin_addr.s_addr=inet_addr(argv[1]);

  sin[1].sin_family=AF_INET;
  sin[1].sin_port=htons(5556);
  sin[1].sin_addr.s_addr=inet_addr("255.255.255.255");

  sin[2].sin_family=AF_INET;
  sin[2].sin_port=htons(5557);
  sin[2].sin_addr.s_addr=inet_addr("227.2.3.45");

  for(i=0;i<3;i++)
  {
    if(!fd[i]->isValid())
    {
      printf("Failed to create socket %d: %d\n",i,fd[i]->getLastError());
      exit(1);
    }
  }

  char buffer[1024];
  printf("Enter text: ");
  fflush(stdin);
  if (scanf("%s", buffer) != 1)
     return 1;

  for(i=0;i<3;i++)
  {
    int32_t total=fd[i]->writeTo(buffer,sizeof(buffer),0,(const SOCKADDR*)&sin[i],sizeof(sin[i]));
    if(total==SOCKET_ERROR)
      printf("Socket %d error sending data: %d\n",i,fd[i]->getLastError());
    else
    {
      fd[i]->readFrom(buffer,sizeof(buffer),0,NULL,NULL);
      printf("Socket %d received reply: %s\n",i,buffer);
    }
  }

  for(i=0;i<3;i++)
    fd[i]->close();

  finalizeSocket();

  return 0;
}
