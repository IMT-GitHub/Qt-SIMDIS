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
#include "DCSScenarioData.h"

DCSScenarioData::DCSScenarioData()
  : DCSBaseData(DCSSCENARIODATA),
    winddirection_(0.0),
    windspeed_(0.0)
{
}

DCSScenarioData::DCSScenarioData(double winddirection,double windspeed)
  : DCSBaseData(DCSSCENARIODATA),
    winddirection_(winddirection),
    windspeed_(windspeed)
{
}

void DCSScenarioData::clear()
{
  DCSBaseData::clear();
  winddirection_=0.0;
  windspeed_=0.0;
}

void* DCSScenarioData::pack(void* buffer,int32_t size) const
{
  char* data=(char*)buffer;

  data+=packit(data);
  data+=bewrite(data,&winddirection_)*sizeof(winddirection_);
  bewrite(data,&windspeed_);

  return buffer;
}

void DCSScenarioData::unpack(const void* buffer,int32_t size)
{
  char* data=(char*)buffer;

  data+=unpackit(data);
  data+=beread(data,&winddirection_)*sizeof(winddirection_);
  beread(data,&windspeed_);
}
