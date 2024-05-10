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
#include "DCSBeamData.h"

DCSBeamData::DCSBeamData()
  : DCSBaseData(DCSBEAMDATA),
    time_(0.0),
    azim_(0.0),
    elev_(0.0),
    length_(0.0)
{
}

DCSBeamData::DCSBeamData(uint64_t id,double time,double azim,double elev,double length)
  : DCSBaseData(id,DCSBEAMDATA),
    time_(time),
    azim_(azim),
    elev_(elev),
    length_(length)
{
}

void DCSBeamData::clear()
{
  DCSBaseData::clear();
  time_=0.0;
  azim_=0.0;
  elev_=0.0;
  length_=0.0;
}

void* DCSBeamData::pack(void* buffer,int32_t size) const
{
  char* data=(char*)buffer;

  data+=packit(data);
  data+=bewrite(data,&time_)*sizeof(time_);
  data+=bewrite(data,&azim_)*sizeof(azim_);
  data+=bewrite(data,&elev_)*sizeof(elev_);
  bewrite(data,&length_);

  return buffer;
}

void DCSBeamData::unpack(const void* buffer,int32_t size)
{
  char* data=(char*)buffer;

  data+=unpackit(data);
  data+=beread(data,&time_)*sizeof(time_);
  data+=beread(data,&azim_)*sizeof(azim_);
  data+=beread(data,&elev_)*sizeof(elev_);
  beread(data,&length_);
}
