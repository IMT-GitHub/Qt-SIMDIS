/********************************************************************************
*                     S o c k e t  C o m p a t i b i l i t y                    *
*********************************************************************************
* Copyright (C) 1999,2000,2001 by Dustin Graves.   All Rights Reserved.         *
********************************************************************************/

#ifndef _SOCKDEFS_H_
#define _SOCKDEFS_H_

#include "inttypesc.h"

#if defined IRIX
#include <sys/socket.h>
#elif defined WIN32
typedef int                 socklen_t;
#endif

#ifdef WIN32
typedef unsigned long                 in_addr_t;
#endif

#ifndef WIN32
#include <errno.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>

#ifdef Solaris
#include <sys/filio.h>
#include <sys/sockio.h>
#endif


typedef uint32_t            SOCKET;
typedef struct sockaddr     SOCKADDR;
typedef struct sockaddr_in  SOCKADDR_IN;
typedef int8_t              TTL_T;
typedef unsigned long       u_long;

#define INVALID_SOCKET    ((SOCKET)(~0))
#define SOCKET_ERROR               (-1)

#define PLATERR(x) x
inline int closesocket(SOCKET sock) { return close(sock); }
inline int ioctlsocket(SOCKET sock, long cmd, u_long* argp) { return ioctl(sock, cmd, argp); }
inline void setSockErr(int32_t err) {errno=err;}
inline int32_t getSockErr()       {return errno;}

inline char* getSockErrString(char* buffer, size_t len)
{
  memset(buffer, 0, len);
  strncpy(buffer, strerror(errno), len - 1);
  return buffer;
}

inline int closeSocketErr(SOCKET sock) { return close(sock); }

inline bool initializeSocket()    {return true;}
inline bool finalizeSocket()      {return true;}

//Standard for argument types is "void*", but windows uses "char*" so,
//we need to cast the types in order to avoid warnings.
#define PLATDATACAST(x) x
#define PLATDATACONSTCAST(x) x
#define PLATOPTCAST(x) ((void*)(x))
#define PLATOPTCONSTCAST(x) ((const void*)(x))

#else

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

typedef __int32                TTL_T;

#define PLATERR(x) (WSA##x)              //Put our error value into proper WIN32 format
#define getSockErr  WSAGetLastError
#define setSockErr  WSASetLastError

inline char* getSockErrString(char* buffer, size_t len)
{
  memset(buffer, 0, len);
  FormatMessage(
      FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      WSAGetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) buffer,
      DWORD(len - 1),
      NULL
  );
  return buffer;
}

// Close a socket, preserving the current WinSock error code. When closesocket is called directly,
// it will clear the WinSock error code (unless it fails). This function preserves the value of
// the current error code when closesocket is called. It is intended to preserve the error
// code when used as described by the following pseudo-code:
// bool bindSocket(SOCKET sock)
// {
//   if (bind(sock) == SOCKET_ERROR())
//   {
//     // bind failed; don't reset the error code so the caller of bindSocket can obtain it
//     closeSocketErr(sock);
//     return false;
//   }
//   return true;
// }
inline int closeSocketErr(SOCKET sock)
{
  int error = WSAGetLastError();
  int result = closesocket(sock);
  WSASetLastError(error);
  return result;
}

inline bool finalizeSocket()
{
  return (WSACleanup()==SOCKET_ERROR)?false:true;
}

inline bool initializeSocket ()
{
  WSADATA stWSAData;
  return (WSAStartup(0x0202,&stWSAData)!=0)?false:true;
}

//Standard for argument types is "void*", but windows uses "char*" so,
//we need to cast the types in order to avoid warnings.
#define PLATDATACAST(x) ((char*)(x))
#define PLATDATACONSTCAST(x) ((const char*)(x))
#define PLATOPTCAST(x) ((char*)(x))
#define PLATOPTCONSTCAST(x) ((const char*)(x))

#endif /* WIN32 */

#ifndef SHUT_RD
#define SHUT_RD 0
#endif

#ifndef SHUT_WR
#define SHUT_WR 1
#endif

#ifndef SHUT_RDWR
#define SHUT_RDWR 2
#endif

#ifndef INADDR_NONE
#define INADDR_NONE 0xFFFFFFFF
#endif

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

#define MAXSOCKADDR 128  //Max sockaddr structure size
#define EPHEMERAL_PORT 0
#define IN4ADDR_WILDCARD htonl(INADDR_ANY)

// WIN32 is missing an inet_aton function
#ifdef WIN32
inline int inet_aton(const char* strptr,struct in_addr* addrptr)
{
  uint32_t addr=inet_addr(strptr);
  if(addr!=INADDR_NONE)
  {
    addrptr->s_addr=addr;
    return 1;
  }
  return 0;
}
#endif

#endif

