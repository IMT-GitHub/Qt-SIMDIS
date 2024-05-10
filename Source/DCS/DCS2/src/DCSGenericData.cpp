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
#include "DCSGenericData.h"

DCSGenericData::DCSGenericData()
  : DCSBaseData(DCSGENERICDATA),
    begintime_(0.0),
    expiretime_(0.0)
{
  memset(tag_,0,sizeof(tag_));
  memset(data_,0,sizeof(data_));
}

DCSGenericData::DCSGenericData(uint64_t id,double begintime,double expiretime,const char* tag,const char* data)
  : DCSBaseData(id,DCSGENERICDATA),
    begintime_(begintime),
    expiretime_(expiretime)
{
  memset(tag_,0,sizeof(tag_));
  memset(data_,0,sizeof(data_));
  if(tag!=NULL) strncpy(tag_,tag,sizeof(tag_)-1);
  if(data!=NULL) strncpy(data_,data,sizeof(data_)-1);
}

void DCSGenericData::clear()
{
  DCSBaseData::clear();
  begintime_=0.0;
  expiretime_=0.0;
  memset(tag_,0,sizeof(tag_));
  memset(data_,0,sizeof(data_));
}

void* DCSGenericData::pack(void* buffer,int32_t size) const
{
  char* data=(char*)buffer;

  data+=packit(data);
  data+=bewrite(data,&begintime_)*sizeof(begintime_);
  data+=bewrite(data,&expiretime_)*sizeof(expiretime_);
  if (getVersion() < 0x0204)
  {
    data+=bewrite(data,tag_,32);
  }
  else
  {
    data+=bewrite(data,tag_,sizeof(tag_));
  }
  if (getVersion() == 0x0200)
    data+=bewrite(data,data_,72);
  else
    data+=bewrite(data,data_,sizeof(data_));

  return buffer;
}
#include "iostreamc"
void DCSGenericData::unpack0200_(const void* buffer,int32_t size)
{
  char* data=(char*)buffer;
  data+=beread(data,&begintime_)*sizeof(begintime_);
  data+=beread(data,&expiretime_)*sizeof(expiretime_);
  data+=beread(data,tag_,32);
  tag_[32] = '\0';
  beread(data,data_,72);
  data_[72] = '\0';
}

void DCSGenericData::unpack(const void* buffer,int32_t size)
{
  char* data=(char*)buffer;
  data+=unpackit(data);
  if (getVersion() == 0x0200)
  {
    unpack0200_ (data, size);
  }
  else
  {
    data+=beread(data,&begintime_)*sizeof(begintime_);
    data+=beread(data,&expiretime_)*sizeof(expiretime_);
    if (getVersion() < 0x0204)
    {
      data+=beread(data,tag_,32);
      tag_[32] = '\0';
    }
    else
    {
      data+=beread(data,tag_,sizeof(tag_));
    }
    beread(data,data_,sizeof(data_));
  }
}
