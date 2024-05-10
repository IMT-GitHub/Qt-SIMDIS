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
#include "DCSTimeData.h"

DCSTimeData::DCSTimeData()
  : DCSBaseData(DCSTIMEDATA),
    time_(0.0)
{
}

DCSTimeData::DCSTimeData(double time)
  : DCSBaseData(DCSTIMEDATA),
    time_(time)
{
}

void DCSTimeData::clear()
{
  DCSBaseData::clear();
  time_=0.0;
}

void* DCSTimeData::pack(void* buffer,int32_t size) const
{
  char* data=(char*)buffer;

  data+=packit(data);
  bewrite(data,&time_);

  return buffer;
}

void DCSTimeData::unpack(const void* buffer,int32_t size)
{
  char* data=(char*)buffer;

  data+=unpackit(data);
  beread(data,&time_);
}
