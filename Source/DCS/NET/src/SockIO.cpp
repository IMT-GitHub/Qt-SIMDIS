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
 * September 15, 2005 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 *
 *
 */
#include "sockdefs.h"
#include "SockIO.h"

int32_t SockIO::writeAll(const void* buffer,int32_t size,int32_t flags) const
{
  if (!isValid())
    return 0;
  int32_t sent=0,result=0;
  const char* dataptr=(const char*)buffer;
  do
  {
    result = write (dataptr+sent, size-sent, flags);
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
