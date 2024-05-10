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
#include <string.h>
#include "inttypesc.h"
#include "DCSswapbytes.h"
#include "DCSdefs.h"
#include "DCSPoint.h"
#include "DCSBaseData.h"
#include "DCSBaseHeader.h"
#include "DCSPlatformData.h"
#include "DCSPlatformHeader.h"

DCSPlatformHeader::DCSPlatformHeader()
  : DCSBaseHeader(DCSPLATFORMHEADER),
    dims_(0.0,0.0,0.0),
    offset_(0.0,0.0,0.0),
    orientationoffset_(0.0,0.0,0.0),
    color_(0xFFFFFFFF),
    state_(DCSSTATE_ON),
    status_(DCSSTATUS_LIVE),
    interpolate_(true)
{
  memset(iconname_,0,sizeof(iconname_));
  memset(callsign_,0,sizeof(callsign_));
}

DCSPlatformHeader::DCSPlatformHeader(uint64_t id,
				     const char* iconname,
				     const char* callsign,
				     const DCSPoint& dims,
				     const DCSPoint& offset,
				     const DCSPoint& oriOffset,
				     uint32_t color,
				     DCSObjectState_t state,
				     DCSTrackStatus_t status,
				     bool interpolate,
				     const DCSCoordReferenceFrame& refFrame,
				     const DCSPlatformData& platdata)
  : DCSBaseHeader(id,DCSPLATFORMHEADER),
    dims_(dims.x,dims.y,dims.z),
    offset_(offset.x,offset.y,offset.z),
    orientationoffset_(oriOffset.x,oriOffset.y,oriOffset.z),
    color_(color),
    state_(state),
    status_(status),
    interpolate_(interpolate),
    referenceframe_(refFrame)
{
  memset(iconname_,0,sizeof(iconname_));
  memset(callsign_,0,sizeof(callsign_));
  if(iconname!=NULL) strncpy(iconname_,iconname,sizeof(iconname_)-1);
  if(callsign!=NULL) strncpy(callsign_,callsign,sizeof(callsign_)-1);

  memcpy(&platdata_,&platdata,sizeof(platdata_));
}

DCSPlatformHeader::DCSPlatformHeader(uint64_t id,
				     const char* iconname,
				     const char* callsign,
				     const DCSPoint& dims,
				     const DCSPoint& offset,
				     uint32_t color,
				     DCSObjectState_t state,
				     DCSTrackStatus_t status,
				     bool interpolate,
				     const DCSCoordReferenceFrame& refFrame,
				     const DCSPlatformData& platdata)
  : DCSBaseHeader(id,DCSPLATFORMHEADER),
    dims_(dims.x,dims.y,dims.z),
    offset_(offset.x,offset.y,offset.z),
    orientationoffset_(0.0,0.0,0.0),
    color_(color),
    state_(state),
    status_(status),
    interpolate_(interpolate),
    referenceframe_(refFrame)
{
  memset(iconname_,0,sizeof(iconname_));
  memset(callsign_,0,sizeof(callsign_));
  if(iconname!=NULL) strncpy(iconname_,iconname,sizeof(iconname_)-1);
  if(callsign!=NULL) strncpy(callsign_,callsign,sizeof(callsign_)-1);

  memcpy(&platdata_,&platdata,sizeof(platdata_));
}

DCSPlatformHeader::DCSPlatformHeader(uint64_t id,
				     const char* iconname,
				     const char* callsign,
				     const DCSPoint& dims,
				     const DCSPoint& offset,
				     double latorigin,
				     double lonorigin,
				     uint32_t color,
				     DCSObjectState_t state,
				     DCSTrackStatus_t status,
				     DCSCoordSystem_t coordsystem,
				     DCSAllegience_t allegience,
				     DCSPlatformType_t plattype,
				     const DCSPlatformData& platdata)
  : DCSBaseHeader(id,DCSPLATFORMHEADER),
    dims_(dims.x,dims.y,dims.z),
    offset_(offset.x,offset.y,offset.z),
    orientationoffset_(0.0,0.0,0.0),
    color_(color),
    state_(state),
    status_(status),
    interpolate_(true)
{
  memset(iconname_,0,sizeof(iconname_));
  memset(callsign_,0,sizeof(callsign_));
  if(iconname!=NULL) strncpy(iconname_,iconname,sizeof(iconname_)-1);
  if(callsign!=NULL) strncpy(callsign_,callsign,sizeof(callsign_)-1);

  memcpy(&platdata_,&platdata,sizeof(platdata_));
  referenceframe_.latorigin_=latorigin;
  referenceframe_.lonorigin_=lonorigin;
  referenceframe_.coordsystem_=coordsystem;
}

void DCSPlatformHeader::clear()
{
  DCSBaseData::clear();
  memset(iconname_,0,sizeof(iconname_));
  memset(callsign_,0,sizeof(callsign_));
  dims_.set(0.0,0.0,0.0);
  offset_.set(0.0,0.0,0.0);
  orientationoffset_.set(0.0,0.0,0.0);
  color_=0xFFFFFFFF;
  state_=DCSSTATE_ON;
  status_=DCSSTATUS_LIVE;
  interpolate_ = true;
  referenceframe_.clear();
  platdata_.clear();
}

void* DCSPlatformHeader::pack(void* buffer,
			      int32_t size) const
{
  //Test size - throw exception if invalid
  char* data=(char*)buffer;

  size_t offset = (size_t) packit(data);
  if (getVersion() < 0x0204)
  {
    offset+=bewrite((data+offset),iconname_,32);
  }
  else
  {
    offset+=bewrite((data+offset),iconname_,sizeof(iconname_));
  }
  offset+=bewrite((data+offset),callsign_,sizeof(callsign_));
  offset+=bewrite((data+offset),&dims_)*sizeof(dims_);
  offset+=bewrite((data+offset),&offset_)*sizeof(offset_);
  if (getVersion() == 0x0200)
  {
    offset+=bewrite((data+offset),&referenceframe_.latorigin_)*sizeof(referenceframe_.latorigin_);
    offset+=bewrite((data+offset),&referenceframe_.lonorigin_)*sizeof(referenceframe_.lonorigin_);
  }
  offset+=bewrite((data+offset),&color_)*sizeof(color_);
  int8_t tmpInt = state_;
  offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
  tmpInt = status_;
  offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
  if (getVersion() != 0x0200)
  {
    tmpInt = interpolate_ ? 1 : 0;
    offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
    referenceframe_.pack (data+offset);
    offset += referenceframe_.dataSize (DCS_VERSION);
  }
  else
  {
    tmpInt = referenceframe_.coordsystem_;
    offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
    tmpInt = 0; // allegience_;
    offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
    tmpInt = 0; // plattype_;
    offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
  }

  if (getVersion() >= 0x0202)
  {
    offset+=bewrite((data+offset),&orientationoffset_)*sizeof(orientationoffset_);
  }

  platdata_.pack((data+offset),int32_t(size-offset));

  return buffer;
}

void DCSPlatformHeader::unpack(const void* buffer,
			       int32_t size)
{
  //Test size - throw exception if invalid
  char* data=(char*)buffer;

  size_t offset=unpackit(data);
  if (getVersion() < 0x0204)
  {
    offset+=beread((data+offset),iconname_,32);
    iconname_[32] = '\0';
  }
  else
  {
    offset+=beread((data+offset),iconname_,sizeof(iconname_));
  }
  offset+=beread((data+offset),callsign_,sizeof(callsign_));
  offset+=beread((data+offset),&dims_)*sizeof(dims_);
  offset+=beread((data+offset),&offset_)*sizeof(offset_);
  if (getVersion() == 0x0200)
  {
    offset+=beread((data+offset),&referenceframe_.latorigin_)*sizeof(referenceframe_.latorigin_);
    offset+=beread((data+offset),&referenceframe_.lonorigin_)*sizeof(referenceframe_.lonorigin_);
  }
  offset+=beread((data+offset),&color_)*sizeof(color_);
  int8_t tmpInt;
  offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
  state_ = (DCSObjectState_t)tmpInt;
  offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
  status_ = (DCSTrackStatus_t)tmpInt;
  if (getVersion() != 0x0200)
  {
    offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
    interpolate_ = (tmpInt != 0);
    referenceframe_.unpack (data+offset);
    offset += referenceframe_.dataSize (DCS_VERSION);
  }
  else
  {
    offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
    referenceframe_.coordsystem_ = (DCSCoordSystem_t)tmpInt;
    // allegience_ has been replaced with category data
    offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
    // plattype_ has been replaced with category data
    offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
  }

  if (getVersion() >= 0x0202)
  {
    offset+=beread((data+offset),&orientationoffset_)*sizeof(orientationoffset_);
  }

  platdata_.unpack((data+offset),int32_t(size-offset));
}

int32_t DCSPlatformHeader::dataSize(uint16_t version) const
{
  if (version == 0x0200)
  {
    return DCSBaseData::dataSize()+
      32+sizeof(callsign_)+
      sizeof(dims_)+
      sizeof(offset_)+
      sizeof(referenceframe_.latorigin_)+
      sizeof(referenceframe_.lonorigin_)+
      sizeof(color_)+
      sizeof(int8_t)+
      sizeof(int8_t)+
      sizeof(int8_t)+
      sizeof(int8_t)+
      sizeof(int8_t)+
      platdata_.dataSize();
  }
  else if (version == 0x0201)
  {
    return DCSBaseData::dataSize()+
      32+sizeof(callsign_)+
      sizeof(dims_)+
      sizeof(offset_)+
      sizeof(color_)+
      sizeof(int8_t)+
      sizeof(int8_t)+
      sizeof(int8_t)+
      referenceframe_.dataSize (DCS_VERSION)+
      platdata_.dataSize();
  }
  else if (version < 0x0204)
  {
    return DCSBaseData::dataSize()+
      32+sizeof(callsign_)+
      sizeof(dims_)+
      sizeof(offset_)+
      sizeof(orientationoffset_)+
      sizeof(color_)+
      sizeof(int8_t)+
      sizeof(int8_t)+
      sizeof(int8_t)+
      referenceframe_.dataSize (DCS_VERSION)+
      platdata_.dataSize();
  }
  // version 0x0204 and above
  return DCSBaseData::dataSize()+
    sizeof(iconname_)+
    sizeof(callsign_)+
    sizeof(dims_)+
    sizeof(offset_)+
    sizeof(orientationoffset_)+
    sizeof(color_)+
    sizeof(int8_t)+
    sizeof(int8_t)+
    sizeof(int8_t)+
    referenceframe_.dataSize (DCS_VERSION)+
    platdata_.dataSize();
}
