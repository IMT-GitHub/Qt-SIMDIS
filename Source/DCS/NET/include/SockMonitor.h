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
 * The U.S. Government retains all rights to use, duplicate, distribute,     
 * disclose, or release this software.
 *
 */

#ifndef _SOCKMONITOR_H_
#define _SOCKMONITOR_H_

#include <list>
#include <map>

class SockInet;

typedef std::list<SockInet*> SocketList;

///Socket pending read, write, and error event monitoring class.  
/**
 * A class for monitoring multiple sockets for pending read, write, and error events.
 * This serves as a cross-platform method for select() calls on multiple sockets.
 */
class SockMonitor
{
public:
  ///Default socket monitor constructor.  
  /** Creates an empty socket monitor object. */
  SockMonitor();

  ///Destructor.  
  /** Virtual destructor. */
  virtual ~SockMonitor();

  /** Adds a socket to the list of sockets to watch for read ready */
  bool addReadMonitor(SockInet* sock);

  /** Adds a socket to the list of sockets to watch for write ready */
  bool addWriteMonitor(SockInet* sock);

  /** Add a socket to the list of sockets to monitor for errors */
  bool addErrorMonitor(SockInet* sock);

  /** Removes a socket from the read list */
  bool removeReadMonitor(SockInet* sock);

  /** Removes a socket from the write list */
  bool removeWriteMonitor(SockInet* sock);

  /** Removes a socket from the error observation list */
  bool removeErrorMonitor(SockInet* sock);

  /** Retrieves sockets in the read list */
  void getReadSockets(SocketList& socks) const;

  /** Retrieves sockets in the write list */
  void getWriteSockets(SocketList& socks) const;

  /** Retrieves sockets in the error monitoring list */
  void getErrorSockets(SocketList& socks) const;

  /** Watches for a read on all read sockets for up to sec.usec time.  Ready sockets get returned in readsocks.
   * @param readsocks User supplied list of sockets, that gets filled with sockets ready to read from
   * @param sec Number of seconds to wait for readReady
   * @param usec Number of microseconds to wait for readReady
   * @return Result of select() -- total number of bits set in the bit mask, or -1 for errors with errno set
   */
  int32_t watchRead(SocketList& readsocks, uint32_t sec=0,uint32_t usec=0);

  /** Watches for a write ready on all read sockets for up to sec.usec time.  Ready sockets get returned in writesocks.
   * @param writesocks User supplied list of sockets, that gets filled with sockets ready to write to
   * @param sec Number of seconds to wait for writeReady
   * @param usec Number of microseconds to wait for writeReady
   * @return Result of select() -- total number of bits set in the bit mask, or -1 for errors with errno set
   */
  int32_t watchWrite(SocketList& writesocks, uint32_t sec=0,uint32_t usec=0);

  /** Watches for error conditions pending on error sockets for up to sec.usec time.  Error-ready sockets get returned in errorsocks.
   * @param errorsocks User supplied list of sockets, that gets filled with sockets with pending error conditions
   * @param sec Number of seconds to wait
   * @param usec Number of microseconds to wait
   * @return Result of select() -- total number of bits set in the bit mask, or -1 for errors with errno set
   */
  int32_t watchError(SocketList& errorsocks, uint32_t sec=0,uint32_t usec=0);

  /** Watches read, write, and error sockets for appropriate conditions.  Selected sockets get returned in appropriate list.
   * @param readsocks User supplied list of sockets, that gets filled with sockets ready to read from
   * @param writesocks User supplied list of sockets, that gets filled with sockets ready to write to
   * @param errorsocks User supplied list of sockets, that gets filled with sockets with pending error conditions
   * @param sec Number of seconds to wait
   * @param usec Number of microseconds to wait
   * @return Result of select() -- total number of bits set in the bit mask, or -1 for errors with errno set
   */
  int32_t watchAll(SocketList& readsocks, SocketList& writesocks, SocketList& errorsocks, uint32_t sec=0,uint32_t usec=0);

protected:
  ///Maps a SOCKET structure to a SockInet for internal usage
  typedef std::map<SOCKET,SockInet*> SocketMap;
  ///Sockets to observe for read ready on select()
  SocketMap readsocks_;
  ///Sockets to observe for write ready on select()
  SocketMap writesocks_;
  ///Sockets to observe for errors on select()
  SocketMap errorsocks_;
};

#endif
