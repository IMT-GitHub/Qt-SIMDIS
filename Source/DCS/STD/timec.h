/* -*- mode: c++ -*- */
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
 * U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 *
 *
 */

#ifndef SIMCORE_COMMON_TIME_H
#define SIMCORE_COMMON_TIME_H

#include <time.h>
#include <sys/timeb.h>

#ifdef UNIX

#include <sys/time.h>
#include <unistd.h>

#define timestruc_to_timespec(a, b) memcpy((a), (b), sizeof(*(a)))

#ifdef __cplusplus
extern "C" {
#endif

inline
void Sleep(unsigned int milliseconds)
{
  usleep(milliseconds * 1000);
}
    
#ifdef __cplusplus
}
#endif

#else

#include <winsock2.h>

//winsock2.h defines timeval but not timezone or timespec
struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};

#ifndef ACE_OS_INCLUDE_SYS_OS_TIME_H
# if !defined (ACE_HAS_POSIX_TIME)
#define ACE_HAS_POSIX_TIME
#endif
//typedef struct timespec_t
//{
//    time_t tv_sec;
//    long   tv_nsec;
//} timespec_t;
#endif

#ifndef timespec_to_timeval
#define timespec_to_timeval(t, ts) \
        (t)->tv_sec = (ts)->tv_sec; \
        (t)->tv_usec = (ts)->tv_nsec / 1000;

#define timeval_to_timespec(ts, t) \
        (ts)->tv_sec = (t)->tv_sec; \
        (ts)->tv_nsec = (t)->tv_usec * 1000;
#endif

// because of Exceed's XlibXtra.h
#define gettimeofday GetTimeOfDay

#ifdef __cplusplus
extern "C" {
#endif

// because of Exceed's XlibXtra.h
inline
int GetTimeOfDay(struct timeval *t, struct timezone *z)
{
  struct _timeb _gtodtmp;
  _ftime (&_gtodtmp);
  (t)->tv_sec = (long)_gtodtmp.time;
  (t)->tv_usec = _gtodtmp.millitm * 1000;

  if (z)
  {
    (z)->tz_minuteswest = _gtodtmp.timezone;
    (z)->tz_dsttime = _gtodtmp.dstflag;
  }

  return 0;
}

#ifdef __cplusplus
}
#endif

#endif

#endif /* SIMCORE_COMMON_TIME_H */
