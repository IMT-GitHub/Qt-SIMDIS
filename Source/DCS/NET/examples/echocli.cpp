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
  if(argc!=3)
  {
    printf("Usage: echocli <address> <port>\n");
    exit(1);
  }

  initializeSocket();

  SockClient cli(atoi(argv[2]),argv[1]);
  if(!cli.isValid()||!cli.isActive())  //See if it is created and connected
  {
    printf("Client creation failed: %d\n",cli.getLastError());
    exit(1);
  }

  char buffer[1024];
  printf("Client connected to %s %d\n",cli.getAddress(buffer,sizeof(buffer)),cli.getPort());

  printf("Enter text: ");
  fflush(stdin);
  if (scanf("%s", buffer) != 1)
     return 1;

  int32_t total=cli.write(buffer,sizeof(buffer));
  if(total==SOCKET_ERROR)
    printf("Error sending data: %d\n",cli.getLastError());
  else if(total==0)
    printf("Server closed connection\n");
  else
  {
    cli.read(buffer,sizeof(buffer));
    printf("Server replied: %s\n",buffer);
  }
  cli.close();

  finalizeSocket();

  return 0;
}
