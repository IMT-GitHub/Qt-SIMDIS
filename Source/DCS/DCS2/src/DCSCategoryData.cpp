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
 *               EW Modeling and Simulation, Code 5770
 *               4555 Overlook Ave.
 *               Washington, D.C. 20375-5339
 *
 * For more information please send email to simdis@enews.nrl.navy.mil
 *
 * July 18, 2006 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 *
 *
 */
#include "DCSCategoryData.h"
#include "inttypesc.h"
#include "DCSswapbytes.h"
#include "DCSdefs.h"
#include "DCSBaseData.h"

DCSCategoryData::DCSCategoryData()
  : DCSBaseData(DCSCATEGORYDATA),
    time_(0.0)
{
  memset(name_,0,sizeof(name_));
  memset(value_,0,sizeof(value_));
}

DCSCategoryData::DCSCategoryData(uint64_t id,double timeVal,const char* name,const char* value)
  : DCSBaseData(id,DCSCATEGORYDATA),
    time_(timeVal)
{
  memset(name_,0,sizeof(name_));
  memset(value_,0,sizeof(value_));
  if(name!=NULL) strncpy(name_,name,sizeof(name_)-1);
  if(value!=NULL) strncpy(value_,value,sizeof(value_)-1);
}

void DCSCategoryData::clear()
{
  DCSBaseData::clear();
  time_=0.0;
  memset(name_,0,sizeof(name_));
  memset(value_,0,sizeof(value_));
}

void* DCSCategoryData::pack(void* buffer,int32_t size) const
{
  char* data=(char*)buffer;

  data+=packit(data);
  data+=bewrite(data,&time_)*sizeof(time_);
  data+=bewrite(data,name_,sizeof(name_));
  bewrite(data,value_,sizeof(value_));

  return buffer;
}

void DCSCategoryData::unpack(const void* buffer,int32_t size)
{
  char* data=(char*)buffer;

  data+=unpackit(data);
  data+=beread(data,&time_)*sizeof(time_);
  data+=beread(data,name_,sizeof(name_));
  beread(data,value_,sizeof(value_));
}

