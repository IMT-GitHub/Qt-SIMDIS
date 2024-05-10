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
#include "DCSPoint.h"
#include "DCSBaseData.h"
#include "DCSPlatformData.h"

DCSPlatformData::DCSPlatformData()
  : DCSBaseData(DCSPLATFORMDATA),
    position_(0,0,0),
    orientation_(0,0,0),
    velocity_(0,0,0),
    accel_(0,0,0),
    time_(0.0)
{
}

DCSPlatformData::DCSPlatformData(uint64_t id,const DCSPoint& position,const DCSPoint& orientation,const DCSPoint& velocity,const DCSPoint& accel,double time)
  : DCSBaseData(id,DCSPLATFORMDATA),
    position_(position.x,position.y,position.z),
    orientation_(orientation.x,orientation.y,orientation.z),
    velocity_(velocity.x,velocity.y,velocity.z),
    accel_(accel.x,accel.y,accel.z),
    time_(time)
{
}

void DCSPlatformData::clear()
{
  DCSBaseData::clear();
  position_.set(0,0,0);
  orientation_.set(0,0,0);
  velocity_.set(0,0,0);
  accel_.set(0,0,0);
  time_=0.0;
}

void* DCSPlatformData::pack(void* buffer,int32_t size) const
{
  char* data=(char*)buffer;

  data+=packit(data);
  data+=bewrite(data,&position_)*sizeof(position_);
  data+=bewrite(data,&orientation_)*sizeof(orientation_);
  data+=bewrite(data,&velocity_)*sizeof(velocity_);
  data+=bewrite(data,&accel_)*sizeof(accel_);
  bewrite(data,&time_);

  return buffer;
};

void DCSPlatformData::unpack(const void* buffer,int32_t size)
{
  char* data=(char*)buffer;

  data+=unpackit(data);
  data+=beread(data,&position_)*sizeof(position_);
  data+=beread(data,&orientation_)*sizeof(orientation_);
  data+=beread(data,&velocity_)*sizeof(velocity_);
  data+=beread(data,&accel_)*sizeof(accel_);
  beread(data,&time_);
};
