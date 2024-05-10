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
* 2012 - U.S. Naval Research Laboratory.
*
* The U.S. Government retains all rights to use, duplicate, distribute,
* disclose, or release this software.
*
*/

#include <string>
#include <iostream>
#include <cassert>

#include <memory.h>

#include "swapbytes.h"
#include "net.h"

#ifdef WIN32
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif
#endif

namespace
{

  /// Time playback rates
  enum
  {
    TCS_NON_REALTIME=0,
    TCS_REALTIME=1
  };

  /// Time playback direction
  enum
  {
    TCS_DIRECTION_FORWARD=0,
    TCS_DIRECTION_REVERSE=1,
    TCS_DIRECTION_STOP=2
  };

  // --------------------------------------------------------------

  /// Time data structure for sending time packets across a network
  /// to synchronize SIMDIS time based applications
  struct TCSPacket
  {
    int16_t refYear;         /**< Reference year for time, ex: 2012 */
    int32_t seconds;         /**< Seconds since beginning of reference year */
    int32_t nanoseconds;     /**< and nanoseconds.  */
    int8_t  direction;       /**< Indicates playback direction: forward(0), reverse(1), or stopped(2) */
    int8_t  realTime;        /**< Indicates real time(1) or non-real time(0) rate */
    float   timeScale;       /**< Time step for play rate */
  };

  // size of TCS time packet
  static const int TCS_PKT_SIZE = 16;

  // --------------------------------------------------------------

  /**
  * Pack contents of a TCSPacket structure into a network transmission ready
  * character array.  Contents of TCSPacket structure are stored in network byte order.
  * @param src TCSPacket structure to be packed in network byte order.
  * @param buffer character array to be filled with contents of 'src'.
  * @param len an integer indicating the size of 'buffer'.
  * @return a pointer to 'buffer' if successful, NULL if failure.
  */
  char* pack(const TCSPacket *src, char *buffer, int len)
  {
    assert(src && buffer);
    if (!src || !buffer || len < TCS_PKT_SIZE)
      return NULL;

    bewrite(buffer, &src->refYear);     buffer += sizeof(src->refYear);
    bewrite(buffer, &src->seconds);     buffer += sizeof(src->seconds);
    bewrite(buffer, &src->nanoseconds); buffer += sizeof(src->nanoseconds);
    bewrite(buffer, &src->direction);   buffer += sizeof(src->direction);
    bewrite(buffer, &src->realTime);    buffer += sizeof(src->realTime);
    bewrite(buffer, &src->timeScale);   buffer += sizeof(src->timeScale);

    return buffer;
  }

  // --------------------------------------------------------------

  /**
  * Unpack contents of a character array, to a TCSPacket structure.  Contents of
  * TCSPacket structure will be stored in host byte order.
  * @param dst TCSPacket structure to be filled from contents of 'buffer'.
  * @param buffer character array containing elements of a TCSPacket structure, stored in network byte order.
  * @param len an integer indicating the size of 'buffer'.
  * @return a pointer to 'dst' if successful, NULL if failure.
  */
  TCSPacket* unpack(TCSPacket *dst, const char *buffer, int len)
  {
    assert(dst && buffer);
    if (!dst || !buffer || len < TCS_PKT_SIZE)
      return NULL;

    beread(buffer, &dst->refYear);      buffer += sizeof(dst->refYear);
    beread(buffer, &dst->seconds);      buffer += sizeof(dst->seconds);
    beread(buffer, &dst->nanoseconds);  buffer += sizeof(dst->nanoseconds);
    beread(buffer, &dst->direction);    buffer += sizeof(dst->direction);
    beread(buffer, &dst->realTime);     buffer += sizeof(dst->realTime);
    beread(buffer, &dst->timeScale);    buffer += sizeof(dst->timeScale);

    return dst;
  }

  // --------------------------------------------------------------

  /**
  * Prints usage information of program.
  */
  void usage(char** argv)
  {
    std::cerr << "\nUsage:  " << argv[0] << " [-a multicast group address] [-p port] [-i interface] [-t TTL] [-tc] [-ts]\n\n";
    exit(0);
  }

  // --------------------------------------------------------------

  /**
  * Sends time packets out on specified multicast socket.
  * @param mcSocket Multicast socket to send TCS packets.
  */
  void timeServerMode(SockMulticast* mcSocket)
  {
    assert(mcSocket);
    if (!mcSocket)
      return;

    char iFace[64];
    memset(iFace, 0, sizeof(iFace));

    char addr[32];
    memset(addr, 0, sizeof(addr));

    // time server mode
    std::cout << "Time server mode: " << mcSocket->getPort() << " " << mcSocket->getAddress(addr, sizeof(addr)) << " " << mcSocket->getIface(iFace, sizeof(iFace)) << " " << static_cast<int>(mcSocket->getTTL()) << "\n";

    // buffer for reading/writing tcs network data
    char tcsBuffer[256];
    TCSPacket tcsPkt;
    int byteCount = 0;
    int pktCtr = 0;

    // initialize packet values
    tcsPkt.refYear = 2012;
    tcsPkt.direction = static_cast<int8_t>(TCS_DIRECTION_FORWARD);
    tcsPkt.realTime = static_cast<int8_t>(TCS_NON_REALTIME);
    tcsPkt.nanoseconds = 0;
    tcsPkt.seconds = 0;
    tcsPkt.timeScale = 1.f;

    // sends 10 packets then stops
    while (pktCtr < 10)
    {
      // initializes buffer
      memset((void*)tcsBuffer, 0, sizeof(tcsBuffer));

      // build packet
      pack(&tcsPkt, tcsBuffer, TCS_PKT_SIZE);

      // send time packet
      byteCount = mcSocket->write(&tcsBuffer, TCS_PKT_SIZE);

      // verify size written matches TCS packet
      if (byteCount == TCS_PKT_SIZE)
      {
        std::cout << "SENT Time Pkt: " << tcsPkt.refYear << " " << tcsPkt.seconds << " " << tcsPkt.nanoseconds << "\n";
      }
      else
      {
        std::cerr << "\nERROR:  Unable to write time data to multicast socket.\n\n";
      }

      // increment time
      tcsPkt.seconds++;
      pktCtr++;

      // sleep 1 second, then repeat
#ifdef UNIX
      usleep(1000000);
#else
      Sleep(1000);
#endif
    }
  }

  // --------------------------------------------------------------

  /**
  * Receives time packets out on specified multicast socket.
  * @param mcSocket Multicast socket to receive TCS packets.
  */
  void timeClientMode(SockMulticast* mcSocket)
  {
    assert(mcSocket);
    if (!mcSocket)
      return;

    char iFace[64];
    memset(iFace, 0, sizeof(iFace));

    char addr[32];
    memset(addr, 0, sizeof(addr));

    // time client mode
    std::cout << "Time client mode: " << mcSocket->getPort() << " " << mcSocket->getAddress(addr, sizeof(addr)) << " " << mcSocket->getIface(iFace, sizeof(iFace)) << " " << static_cast<int>(mcSocket->getTTL()) << "\n";

    // buffer for reading/writing tcs network data
    char tcsBuffer[256];
    TCSPacket tcsPkt;
    int byteCount = 0;
    int pktCtr = 0;

    // NOTE:  This assumes that only TCS data will be present on the given socket
    // receives 10 packets then stops
    while (pktCtr < 10)
    {
      // initializes buffer
      memset((void*)tcsBuffer, 0, sizeof(tcsBuffer));

      // reads packets, waiting .1 sec for data in socket
      byteCount = 0;
      if (mcSocket->readReady(0, 10000))
      {
        byteCount = mcSocket->read((void*)tcsBuffer, sizeof(tcsBuffer));
      }

      // verify size read matches TCS packet
      if (byteCount >= TCS_PKT_SIZE)
      {
        // unpack the data, then show the last received packet
        if (unpack(&tcsPkt, tcsBuffer, byteCount))
        {
          std::cout << "RCVD Time Pkt: " << tcsPkt.refYear << " " << tcsPkt.seconds << " " << tcsPkt.nanoseconds << "\n";
          pktCtr++;
        }
      }
    }
  }

} // end of namespace

// --------------------------------------------------------------
// Example program that sends (-ts) or receives (-tc) SIMDIS
// time synchronization packets using a multicast socket
// This program is based on the NET API that is distributed with
// the DCS API

int main(int argc, char** argv)
{
  std::string tcsGroup = "225.0.0.0";
  TTL_T tcsTTL = 4;
  uint16_t tcsPort = 10000;
  std::string tcsInterface = "";
  bool timeServer = false;

  for (int iArg=1; iArg < argc; ++iArg)
  {
    if (strcasecmp(argv[iArg], "-h") == 0 || strcasecmp(argv[iArg], "--help") == 0)
    {
      usage(argv);
    }
    else if (strcasecmp(argv[iArg], "-a") == 0)
    {
      ++iArg;
      if (iArg >= argc)
      {
        std::cerr << "\nWARNING:  No address was given after the -a command line argument.\n\n";
        usage(argv);
      }
      tcsGroup = argv[iArg];
    }
    else if (strcasecmp(argv[iArg], "-p") == 0)
    {
      ++iArg;
      if (iArg >= argc)
      {
        std::cerr << "\nWARNING:  No port was given after the -p command line argument.\n\n";
        usage(argv);
      }
      tcsPort = atoi(argv[iArg]);
    }
    else if (strcasecmp(argv[iArg], "-i") == 0)
    {
      ++iArg;
      if (iArg >= argc)
      {
        std::cerr << "\nWARNING:  No interface was given after the -i command line argument.\n\n";
        usage(argv);
      }
      tcsInterface = argv[iArg];
    }
    else if (strcasecmp(argv[iArg], "-t") == 0)
    {
      ++iArg;
      if (iArg >= argc)
      {
        std::cerr << "\nWARNING:  No TTL was given after the -t command line argument.\n\n";
        usage(argv);
      }
      tcsTTL = static_cast<TTL_T>(atoi(argv[iArg]));
    }
    else if (strcasecmp(argv[iArg], "-tc") == 0)
    {
      timeServer = false;
    }
    else if (strcasecmp(argv[iArg], "-ts") == 0)
    {
      timeServer = true;
    }
    else
    {
      std::cerr << "\nWARNING:  Unrecognized command line argument (" << argv[iArg] << ").\n\n";
      usage(argv);
    }
  }

  // initialize OS sockets
  initializeSocket();

  // create multicast socket
  SockMulticast multiCast(tcsPort, tcsGroup.c_str(), (tcsInterface.empty() ? NULL : tcsInterface.c_str()), tcsTTL);

  // Make sure our socket was succesfully constructed
  if (!multiCast.isValid())
  {
    std::cerr << "\nERROR:  Unable to construct multicast socket.\n\n";
    // turn off OS sockets on exit
    finalizeSocket();
    return -1;
  }

  // handle the time server and time client command line options
  if (timeServer)
  {
    timeServerMode(&multiCast);
  }
  else
  {
    timeClientMode(&multiCast);
  }

  // turn off OS sockets on exit
  finalizeSocket();

  return 0;
}
