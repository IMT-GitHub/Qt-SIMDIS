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

#include <memory.h>
#include "inttypesc.h"
#include "DCSswapbytes.h"
#include "DCSdefs.h"
#include "DCSBaseData.h"
#include "DCSTimeData.h"
#include "DCSTimeHeader.h"

DCSTimeHeader::DCSTimeHeader()
  : DCSBaseData(DCSTIMEHEADER),
    realtimemod_(1.0),
    timetype_(0)
{
}

DCSTimeHeader::DCSTimeHeader(double realtimemod,int8_t timetype,const DCSTimeData& timedata)
  : DCSBaseData(DCSTIMEHEADER),
    realtimemod_(realtimemod),
    timetype_(timetype)
{
  memcpy(&timedata_,&timedata,sizeof(timedata_));
}

void DCSTimeHeader::clear()
{
  DCSBaseData::clear();
  realtimemod_=1.0;
  timetype_=0;
  timedata_.clear();
}

void* DCSTimeHeader::pack(void* buffer,int32_t size) const
{
  char* data=(char*)buffer;

  int32_t offset=packit(data);
  offset+= (int32_t) bewrite((data+offset),&realtimemod_)*sizeof(realtimemod_);
  offset+= (int32_t) bewrite((data+offset),&timetype_)*sizeof(timetype_);
  timedata_.pack((data+offset),(size-offset));

  return buffer;
}

void DCSTimeHeader::unpack(const void* buffer,int32_t size)
{
  char* data=(char*)buffer;

  int32_t offset=unpackit(data);
  offset+= (int32_t) beread((data+offset),&realtimemod_)*sizeof(realtimemod_);
  offset+= (int32_t) beread((data+offset),&timetype_)*sizeof(timetype_);
  timedata_.unpack((data+offset),(size-offset));
}
