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

#include "inttypesc.h"
#include "DCSswapbytes.h"
#include "DCSdefs.h"
#include "DCSBaseData.h"
#include "DCSEvent.h"

DCSEvent::DCSEvent()
  : DCSBaseData(DCSEVENT),
    time_(0.0),
    data_(0),
    eventtype_(DCSEVENT_UNKNOWN)
{
  memset(chardata_,0,sizeof(chardata_));
}

DCSEvent::DCSEvent(uint64_t id,double time,int64_t data,int8_t eventtype)
  : DCSBaseData(id,DCSEVENT),
    time_(time),
    data_(data),
    eventtype_(eventtype)
{
  memset(chardata_,0,sizeof(chardata_));
}

DCSEvent::DCSEvent(uint64_t id,double time,char *chardata,int8_t eventtype)
  : DCSBaseData(id,DCSEVENT),
    time_(time),
    data_(0),
    eventtype_(eventtype)
{
  memset(chardata_,0,sizeof(chardata_));
  if(chardata!=NULL) strncpy(chardata_,chardata,sizeof(chardata_)-1);
}

void DCSEvent::clear()
{
  DCSBaseData::clear();
  time_=0.0;
  data_=0;
  eventtype_=DCSEVENT_UNKNOWN;
  memset(chardata_,0,sizeof(chardata_));
}

void* DCSEvent::pack(void* buffer,int32_t size) const
{
  char* data=(char*)buffer;

  data+=packit(data);
  data+=bewrite(data,&time_)*sizeof(time_);
  data+=bewrite(data,&data_)*sizeof(data_);
  if (getVersion() >= 0x0204)
  {
    data+=bewrite(data,chardata_,sizeof(chardata_));
  }
  bewrite(data,&eventtype_);

  return buffer;
}

void DCSEvent::unpack(const void* buffer,int32_t size)
{
  char* data=(char*)buffer;

  data+=unpackit(data);
  data+=beread(data,&time_)*sizeof(time_);
  data+=beread(data,&data_)*sizeof(data_);
  if (getVersion() >= 0x0204)
  {
    data+=beread(data,chardata_,sizeof(chardata_));
  }
  beread(data,&eventtype_);
}
