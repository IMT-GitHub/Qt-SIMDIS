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
#include "DCSPoint.h"
#include "DCSBaseData.h"
#include "DCSBaseHeader.h"
#include "DCSBeamData.h"
#include "DCSBeamHeader.h"

DCSBeamHeader::DCSBeamHeader()
  : DCSBaseHeader(DCSBEAMHEADER),
    offset_(0.0,0.0,0.0),
    azimoffset_(0.0),
    elevoffset_(0.0),
    missileoffset_(false),
    hbw_(0.0),
    vbw_(0.0),
    hostid_(-1),
    targetid_(-1),
    color_(0xFFFFFFFF),
    state_(DCSSTATE_ON),
    beamtype_(DCSBEAM_UNKNOWN),
    interpolate_(true)
{
  memset(callsign_,0,sizeof(callsign_));
}

DCSBeamHeader::DCSBeamHeader(uint64_t id,
			     const char* callsign,
			     const DCSPoint& offset,
			     double azimoffset,
			     double elevoffset,
			     bool missileoffset,
			     double hbw,
			     double vbw,
			     int64_t hostid,
			     int64_t targetid,
			     uint32_t color,
			     DCSObjectState_t state,
			     DCSBeamType_t beamtype,
			     bool interpolate,
			     const DCSBeamData& beamdata)
  : DCSBaseHeader(id,DCSBEAMHEADER),
    offset_(offset.x,offset.y,offset.z),
    azimoffset_(azimoffset),
    elevoffset_(elevoffset),
    missileoffset_(missileoffset),
    hbw_(hbw),
    vbw_(vbw),
    hostid_(hostid),
    targetid_(targetid),
    color_(color),
    state_(state),
    beamtype_(beamtype),
    interpolate_(interpolate)
{
  memset(callsign_,0,sizeof(callsign_));
  if(callsign!=NULL) strncpy(callsign_,callsign,sizeof(callsign_)-1);

  memcpy(&beamdata_,&beamdata,sizeof(beamdata_));
}

DCSBeamHeader::DCSBeamHeader(uint64_t id,
			     const char* callsign,
			     const DCSPoint& offset,
			     double azimoffset,
			     double elevoffset,
			     double hbw,
			     double vbw,
			     int64_t hostid,
			     int64_t targetid,
			     uint32_t color,
			     DCSObjectState_t state,
			     DCSBeamType_t beamtype,
			     const DCSBeamData& beamdata)
  : DCSBaseHeader(id,DCSBEAMHEADER),
    offset_(offset.x,offset.y,offset.z),
    azimoffset_(azimoffset),
    elevoffset_(elevoffset),
    missileoffset_(false),
    hbw_(hbw),
    vbw_(vbw),
    hostid_(hostid),
    targetid_(targetid),
    color_(color),
    state_(state),
    beamtype_(beamtype),
    interpolate_(true)
{
  memset(callsign_,0,sizeof(callsign_));
  if(callsign!=NULL) strncpy(callsign_,callsign,sizeof(callsign_)-1);

  memcpy(&beamdata_,&beamdata,sizeof(beamdata_));
}

void DCSBeamHeader::clear()
{
  DCSBaseData::clear();
  memset(callsign_,0,sizeof(callsign_));
  offset_.set(0.0,0.0,0.0);
  azimoffset_=0.0;
  elevoffset_=0.0;
  missileoffset_ = false;
  hbw_=0.0;
  vbw_=0.0;
  hostid_=-1;
  targetid_=-1;
  color_=0xFFFFFFFF;
  state_=DCSSTATE_ON;
  beamtype_=DCSBEAM_UNKNOWN;
  interpolate_ = true;
  beamdata_.clear();
}

void* DCSBeamHeader::pack(void* buffer,int32_t size) const
{
  char* data=(char*)buffer;

  size_t offset = (size_t) packit(data);
  offset+=bewrite((data+offset),callsign_,sizeof(callsign_));
  offset+=bewrite((data+offset),&offset_)*sizeof(offset_);
  offset+=bewrite((data+offset),&azimoffset_)*sizeof(azimoffset_);
  offset+=bewrite((data+offset),&elevoffset_)*sizeof(elevoffset_);
  if (getVersion() != 0x0200)
  {
    int8_t missOff = (missileoffset_ ? 1 : 0);
    offset+=bewrite((data+offset),&missOff)*sizeof(missOff);
  }
  offset+=bewrite((data+offset),&hbw_)*sizeof(hbw_);
  offset+=bewrite((data+offset),&vbw_)*sizeof(vbw_);
  offset+=bewrite((data+offset),&hostid_)*sizeof(hostid_);
  offset+=bewrite((data+offset),&targetid_)*sizeof(targetid_);
  offset+=bewrite((data+offset),&color_)*sizeof(color_);
  int8_t tmpInt = state_;
  offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
  tmpInt = beamtype_;
  offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
  if (getVersion() != 0x0200)
  {
    tmpInt = interpolate_ ? 1 : 0;
    offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
  }
  beamdata_.pack((data+offset), (int32_t) (size-offset));

  return buffer;
}

void DCSBeamHeader::unpack(const void* buffer,int32_t size)
{
  char* data=(char*)buffer;

  size_t offset = (size_t) unpackit(data);
  offset+=beread((data+offset),callsign_,sizeof(callsign_));
  offset+=beread((data+offset),&offset_)*sizeof(offset_);
  offset+=beread((data+offset),&azimoffset_)*sizeof(azimoffset_);
  offset+=beread((data+offset),&elevoffset_)*sizeof(elevoffset_);
  if (getVersion() != 0x0200)
  {
    int8_t missOff;
    offset+=beread((data+offset),&missOff)*sizeof(missOff);
    missileoffset_ = (missOff != 0);
  }
  offset+=beread((data+offset),&hbw_)*sizeof(hbw_);
  offset+=beread((data+offset),&vbw_)*sizeof(vbw_);
  offset+=beread((data+offset),&hostid_)*sizeof(hostid_);
  offset+=beread((data+offset),&targetid_)*sizeof(targetid_);
  offset+=beread((data+offset),&color_)*sizeof(color_);
  int8_t tmpInt;
  offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
  state_ = (DCSObjectState_t)tmpInt;
  offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
  beamtype_ = (DCSBeamType_t)tmpInt;
  if (getVersion() != 0x0200)
  {
    offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
    interpolate_ = (tmpInt != 0);
  }
  beamdata_.unpack((data+offset), (int32_t) (size-offset));
}
