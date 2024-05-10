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
#include "DCSGateData.h"
#include "DCSGateHeader.h"

DCSGateHeader::DCSGateHeader()
  : DCSBaseHeader(DCSGATEHEADER),
    offset_(0.0,0.0,0.0),
    azimoffset_(0.0),
    elevoffset_(0.0),
    hostid_(-1),
    color_(0xFFFFFFFF),
    state_(DCSSTATE_ON),
    gatetype_(DCSGATE_UNKNOWN),
    interpolate_(true)
{
  memset(callsign_,0,sizeof(callsign_));
}

DCSGateHeader::DCSGateHeader(uint64_t id,
			     const char* callsign,
			     const DCSPoint& offset,
			     double azimoffset,
			     double elevoffset,
			     int64_t hostid,
			     uint32_t color,
			     DCSObjectState_t state,
			     DCSGateType_t gatetype,
			     bool interpolate,
			     const DCSGateData& gatedata)
  : DCSBaseHeader(id,DCSGATEHEADER),
    offset_(offset.x,offset.y,offset.z),
    azimoffset_(azimoffset),
    elevoffset_(elevoffset),
    hostid_(hostid),
    color_(color),
    state_(state),
    gatetype_(gatetype),
    interpolate_(interpolate)
{
  memset(callsign_,0,sizeof(callsign_));
  if(callsign!=NULL) strncpy(callsign_,callsign,sizeof(callsign_)-1);

  memcpy(&gatedata_,&gatedata,sizeof(gatedata_));
}

DCSGateHeader::DCSGateHeader(uint64_t id,
			     const char* callsign,
			     const DCSPoint& offset,
			     int64_t hostid,
			     uint32_t color,
			     DCSObjectState_t state,
			     DCSGateType_t gatetype,
			     bool interpolate,
			     const DCSGateData& gatedata)
  : DCSBaseHeader(id,DCSGATEHEADER),
    offset_(offset.x,offset.y,offset.z),
    azimoffset_(0.0),
    elevoffset_(0.0),
    hostid_(hostid),
    color_(color),
    state_(state),
    gatetype_(gatetype),
    interpolate_(interpolate)
{
  memset(callsign_,0,sizeof(callsign_));
  if(callsign!=NULL) strncpy(callsign_,callsign,sizeof(callsign_)-1);

  memcpy(&gatedata_,&gatedata,sizeof(gatedata_));
}

DCSGateHeader::DCSGateHeader(uint64_t id,
			     const char* callsign,
			     const DCSPoint& offset,
			     int64_t hostid,
			     uint32_t color,
			     DCSObjectState_t state,
			     DCSGateType_t gatetype,
			     const DCSGateData& gatedata)
  : DCSBaseHeader(id,DCSGATEHEADER),
    offset_(offset.x,offset.y,offset.z),
    azimoffset_(0.0),
    elevoffset_(0.0),
    hostid_(hostid),
    color_(color),
    state_(state),
    gatetype_(gatetype),
    interpolate_(true)
{
  memset(callsign_,0,sizeof(callsign_));
  if(callsign!=NULL) strncpy(callsign_,callsign,sizeof(callsign_)-1);

  memcpy(&gatedata_,&gatedata,sizeof(gatedata_));
}

void DCSGateHeader::clear()
{
  DCSBaseData::clear();
  memset(callsign_,0,sizeof(callsign_));
  offset_.set(0.0,0.0,0.0);
  azimoffset_=0.0;
  elevoffset_=0.0;
  hostid_=-1;
  color_=0xFFFFFFFF;
  state_=DCSSTATE_ON;
  gatetype_=DCSGATE_UNKNOWN;
  interpolate_ = true;
  gatedata_.clear();
}

void* DCSGateHeader::pack(void* buffer,int32_t size) const
{
  char* data=(char*)buffer;

  size_t offset=packit(data);
  offset+=bewrite((data+offset),callsign_,sizeof(callsign_));
  offset+=bewrite((data+offset),&offset_)*sizeof(offset_);
  offset+=bewrite((data+offset),&hostid_)*sizeof(hostid_);
  offset+=bewrite((data+offset),&color_)*sizeof(color_);
  int8_t tmpInt = (int8_t)state_;
  offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
  tmpInt = (int8_t)gatetype_;
  offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
  if (getVersion() >= 0x0201)
  {
    tmpInt = interpolate_ ? 1 : 0;
    offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
  }  
  if (getVersion() >= 0x0204)
  {
    offset+=bewrite((data+offset),&azimoffset_)*sizeof(azimoffset_);
    offset+=bewrite((data+offset),&elevoffset_)*sizeof(elevoffset_);
  }  
  gatedata_.pack((data+offset),int32_t((size-offset)));

  return buffer;
}

void DCSGateHeader::unpack(const void* buffer,int32_t size)
{
  char* data=(char*)buffer;

  size_t offset=unpackit(data);
  offset+=beread((data+offset),callsign_,sizeof(callsign_));
  offset+=beread((data+offset),&offset_)*sizeof(offset_);
  offset+=beread((data+offset),&hostid_)*sizeof(hostid_);
  offset+=beread((data+offset),&color_)*sizeof(color_);
  int8_t tmpInt;
  offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
  state_ = (DCSObjectState_t)tmpInt;
  offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
  gatetype_ = (DCSGateType_t)tmpInt;
  if (getVersion() >= 0x0201)
  {
    offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
    interpolate_ = (tmpInt != 0);
  }  
  if (getVersion() >= 0x0204)
  {
    offset+=beread((data+offset),&azimoffset_)*sizeof(azimoffset_);
    offset+=beread((data+offset),&elevoffset_)*sizeof(elevoffset_);
  }  
  gatedata_.unpack((data+offset),int32_t((size-offset)));
}

int32_t DCSGateHeader::dataSize(uint16_t version) const
{
  if (version == 0x0200)
  {
    return DCSBaseData::dataSize()+
      sizeof(callsign_)+
      sizeof(offset_)+
      sizeof(hostid_)+
      sizeof(color_)+
      sizeof(int8_t)+
      sizeof(int8_t)+
      gatedata_.dataSize();
  }
  else if (version < 0x0204)
  {
    // version 0x0201
    return DCSBaseData::dataSize()+
      sizeof(callsign_)+
      sizeof(offset_)+
      sizeof(hostid_)+
      sizeof(color_)+
      sizeof(int8_t)+
      sizeof(int8_t)+
      sizeof(int8_t)+
      gatedata_.dataSize();;
  }
  else
  {
    // version 0x0204
    return DCSBaseData::dataSize()+
      sizeof(callsign_)+
      sizeof(offset_)+
      sizeof(azimoffset_)+
      sizeof(elevoffset_)+
      sizeof(hostid_)+
      sizeof(color_)+
      sizeof(int8_t)+
      sizeof(int8_t)+
      sizeof(int8_t)+
      gatedata_.dataSize();;
  }
}
