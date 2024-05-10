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

#include <stdio.h>
#include <stdlib.h>
#include "net.h"

int main(int argc,char** argv)
{
  if(argc!=2)
  {
    printf("Usage: echoserv port\n");
    exit(1);
  }

  initializeSocket();

  SockServer serv((uint16_t)atoi(argv[1]));
  if(!serv.isValid()||!serv.isActive())
  {
    printf("Server creation failed: %d\n",serv.getLastError());
    exit(1);
  }

  printf("Server listening on %d\n",serv.getLocalPort());
  for(;;)
  {
    SockClient cli(serv.accept());

    char buffer[1500];
    printf("Received connection to %s %d from %s %d\n",cli.getIface(buffer,sizeof(buffer)),cli.getLocalPort(),cli.getAddress(buffer,sizeof(buffer)),cli.getPort());
    
    int32_t total=cli.read(buffer,1500);
    if(total==SOCKET_ERROR)
      printf("Error reading from peer: %d\n",cli.getLastError());
    else
    {
      printf("Echoing text: %s\n",buffer);
      cli.write(buffer,total);
    }
    cli.close();
  }

  serv.close();

  finalizeSocket();
  return 0;
}
