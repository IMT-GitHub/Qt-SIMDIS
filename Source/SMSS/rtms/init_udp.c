//---------------------------------------------------------------------------
//                                C ROUTINE
//---------------------------------------------------------------------------
//
// Name : 
//
// Filename : init_udp.c
//
// Project : UH-60P
//
// Purpose :
// The purpose of this package is to 
//
//
// Revision History :
//    $Log: init_udp.c,v $
//    Revision 1.1  2006/06/02 12:19:57  ufoman
//    SCN:   DR:   add a function for open udp socket
//
//    Revision 1.0  2006/05/17 14:22:29  root
//    Initial Revision
//
//
//---------------------------------------------------------------------------
//                                C ROUTINE
//---------------------------------------------------------------------------

#include <sys/types.h>
#include <sys/socket.h>
#include <linux/sockios.h>	/* sioc ioctls */
#include <linux/if_ether.h>	/* ETH_P_IP */
#include <netinet/in.h>		/* sockaddr_in, IPPROTO_UDP, hton__ macros */
#include <net/if.h>		/* ifreq */
#include <netdb.h>		/* gethostbyname */
#include <string.h>		/* strcpy */
#include <errno.h>		/* errno */
#include <stdio.h>		/* fprintf */
#include <unistd.h>		/* fcntl, gethostname */
#include <fcntl.h>		/* fcntl */

// by baek-rev1
// --------------------------------------------------------------------------------------
int init_udp_socket(char * saRX_name,
                    char * saTX_name, 
                    int    saRX_port, 
                    int    saTX_port, 
                    int  * skt, 
                    struct sockaddr_in *saRX, 
                    struct sockaddr_in *saTX) 
// rx : local, tx : remote
{

  struct hostent *hostent_p;

  /* PF_xxx == AF_xxx (Protocol Families (PF) are same as Address Families (AF)) */
  /* Create socket */
  if ((*skt = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) 
  {
    perror("socket creation failed");
    return(-1);
  }

  hostent_p=gethostbyname(saRX_name);
  if (hostent_p==NULL) 
  {
    herror("gethostbyname failed");
    return -1;
  }
  memset(saRX,0,sizeof(struct sockaddr_in));
  saRX->sin_family = AF_INET;
  saRX->sin_port = htons(saRX_port);
//  saRX->sin_addr.s_addr = inet_addr("10.10.20.131");
  memcpy(&(saRX->sin_addr),hostent_p->h_addr, hostent_p->h_length);

  hostent_p=gethostbyname(saTX_name);
  if (hostent_p==NULL) 
  {
    herror("gethostbyname failed");
    return -1;
  }
  memset(saTX,0,sizeof(struct sockaddr_in));
  saTX->sin_family = AF_INET;
  saTX->sin_port = htons(saTX_port);
//  saTX->sin_addr.s_addr = inet_addr("10.10.20.135");
  memcpy(&(saTX->sin_addr),hostent_p->h_addr, hostent_p->h_length);
 
  if (bind(*skt, (struct sockaddr *) saRX, sizeof(struct sockaddr_in)) < 0)
  {
    perror("bind failed");
    return -1;
  }

  if (fcntl(*skt, F_SETFL, O_NONBLOCK) < 0)
  {
    perror("fcntl O_NONBLOCK failed");
  }

  return 0;

}
// --------------------------------------------------------------------------------------

int init_send(char * hostname, 
              int port, 
              int * skt, 
              struct sockaddr_in *local, 
              struct sockaddr_in *remote) 
{

  struct hostent *hostent_p;
  extern int h_errno;

  /* PF_xxx == AF_xxx (Protocol Families (PF) are same as Address Families (AF)) */
  /* Create socket */
  if (-1== (*skt = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))) 
  {
    perror("socket creation failed");
    return(-1);
  }

  memset(local,0,sizeof(struct sockaddr_in));
  local->sin_family = AF_INET;
  local->sin_port = htons(port);
  // local->sin_addr.s_addr = INADDR_ANY;
  local->sin_addr.s_addr = inet_addr("10.10.20.131");

  hostent_p=gethostbyname(hostname);
  if (NULL==hostent_p) 
  {
    herror("gethostbyname failed");
    return(-1);
  }

  memset(remote,0,sizeof(struct sockaddr_in));
  remote->sin_family = AF_INET;
  remote->sin_port = htons(port);
  memcpy(&(remote->sin_addr),hostent_p->h_addr, hostent_p->h_length);
 
  if (bind(*skt, (struct sockaddr *) local, sizeof(struct sockaddr_in)) < 0)
  {
    perror("bind failed");
    return(-1);
  }

  if (fcntl(*skt, F_SETFL, O_NONBLOCK) < 0)
  {
    perror("fcntl O_NONBLOCK failed");
  }

  return (0);
}

int init_recv(char * hostname, 
              int port, 
              int * skt, 
              struct sockaddr_in *local, 
              struct sockaddr_in *remote) 
{

  struct hostent *hostent_p;
  extern int h_errno;
  int status;

  /* PF_xxx == AF_xxx (Protocol Families (PF) are same as Address Families (AF)) */
  /* Create socket */
  if (-1== (*skt = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))) 
  {
    perror("socket creation failed");
    return(-1);
  }

  memset(local,0,sizeof(struct sockaddr_in));
  local->sin_family = AF_INET;
  local->sin_port = htons(port);
  local->sin_addr.s_addr = INADDR_ANY;

  hostent_p=gethostbyname(hostname);
  if (NULL==hostent_p) 
  {
    herror("gethostbyname failed");
    return(-1);
  }
  memset(remote,0,sizeof(struct sockaddr_in));
  remote->sin_family = AF_INET;
  remote->sin_port = htons(port);
  memcpy(&(remote->sin_addr),hostent_p->h_addr, hostent_p->h_length);
 
  status=bind(*skt, (struct sockaddr *) local, sizeof(struct sockaddr_in));
  if (status) 
  {
    perror("bind failed");
    return(-1);
  }

  if ((status=fcntl(*skt, F_SETFL, O_NONBLOCK)))
  {
    perror("fcntl O_NONBLOCK failed");
  }

  return (0);

}

