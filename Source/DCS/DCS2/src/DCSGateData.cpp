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
#include "DCSGateData.h"

DCSGateData::DCSGateData()
  : DCSBaseData(DCSGATEDATA),
    time_(0.0),
    azim_(0.0),
    elev_(0.0),
    width_(0.0),
    height_(0.0),
    minrange_(0.0),
    maxrange_(0.0),
    centroid_(0.0)
{
}

DCSGateData::DCSGateData(uint64_t id,double time,double azim,double elev,double width,double height,double minrange,double maxrange,double centroid)
  : DCSBaseData(id,DCSGATEDATA),
    time_(time),
    azim_(azim),
    elev_(elev),
    width_(width),
    height_(height),
    minrange_(minrange),
    maxrange_(maxrange),
    centroid_(centroid)
{
}

void DCSGateData::clear()
{
  DCSBaseData::clear();
  time_=0.0;
  azim_=0.0;
  elev_=0.0;
  width_=0.0;
  height_=0.0;
  minrange_=0.0;
  maxrange_=0.0;
  centroid_=0.0;
}

void* DCSGateData::pack(void* buffer,int32_t size) const
{
  char* data=(char*)buffer;

  data+=packit(data);
  data+=bewrite(data,&time_)*sizeof(time_);
  data+=bewrite(data,&azim_)*sizeof(azim_);
  data+=bewrite(data,&elev_)*sizeof(elev_);
  data+=bewrite(data,&width_)*sizeof(width_);
  data+=bewrite(data,&height_)*sizeof(height_);
  data+=bewrite(data,&minrange_)*sizeof(minrange_);
  data+=bewrite(data,&maxrange_)*sizeof(maxrange_);
  bewrite(data,&centroid_);

  return buffer;
}

void DCSGateData::unpack(const void* buffer,int32_t size)
{
  char* data=(char*)buffer;

  data+=unpackit(data);
  data+=beread(data,&time_)*sizeof(time_);
  data+=beread(data,&azim_)*sizeof(azim_);
  data+=beread(data,&elev_)*sizeof(elev_);
  data+=beread(data,&width_)*sizeof(width_);
  data+=beread(data,&height_)*sizeof(height_);
  data+=beread(data,&minrange_)*sizeof(minrange_);
  data+=beread(data,&maxrange_)*sizeof(maxrange_);
  beread(data,&centroid_);
}
