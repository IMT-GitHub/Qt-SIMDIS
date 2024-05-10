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
#include "DCSBaseData.h"
#include "DCSScenarioData.h"
#include "DCSScenarioHeader.h"
#include "DCSutils.h"

namespace
{
int currentYear()
{
  // get the current system time, using timezone value of 0
  // returns UTC time
  struct timeval tp;
  gettimeofday(&tp, 0);

  // put system time into a tm struct
  time_t t(tp.tv_sec);

  struct tm *pTime = gmtime(&t);

  // years are stored as values since 1900
  return static_cast<int>(pTime->tm_year + 1900);
}
}

DCSScenarioHeader::DCSScenarioHeader()
  : DCSBaseData(DCSSCENARIOHEADER),
    referenceyear_(currentYear())
{
  memset(classificationstr_, 0, sizeof(classificationstr_));
  strncpy (classificationstr_, "FOR OFFICIAL USE ONLY", sizeof(classificationstr_)-1);
  classcolor_ = 0x80FFFFFF;
}

DCSScenarioHeader::DCSScenarioHeader(double latorigin,double lonorigin,int16_t referenceyear,DCSCoordSystem_t coordsystem,DCSClassification_t classification,const DCSScenarioData& scendata)
  : DCSBaseData(DCSSCENARIOHEADER),
    referenceyear_(referenceyear)
{
  referenceframe_.latorigin_ = latorigin;
  referenceframe_.lonorigin_ = lonorigin;
  referenceframe_.coordsystem_ = coordsystem;

  memset(classificationstr_, 0, sizeof(classificationstr_));
  strncpy (classificationstr_, DCS::getClassificationString (classification), sizeof(classificationstr_)-1);
  classcolor_ = DCS::getClassificationColor(classification);
  memcpy(&scendata_,&scendata,sizeof(scendata_));
}

DCSScenarioHeader::DCSScenarioHeader(const DCSCoordReferenceFrame& refFrame,int16_t referenceyear,const char* classification,uint32_t classColor,const DCSScenarioData& scendata)
  : DCSBaseData(DCSSCENARIOHEADER),
    referenceyear_(referenceyear),
    classcolor_(classColor),
    referenceframe_(refFrame)
{
  memset(classificationstr_, 0, sizeof(classificationstr_));
  strncpy (classificationstr_, classification, sizeof(classificationstr_)-1);
  memcpy(&scendata_,&scendata,sizeof(scendata_));
}

void DCSScenarioHeader::clear()
{
  DCSBaseData::clear();
  referenceframe_.clear();
  referenceyear_=0;
  strncpy (classificationstr_, "FOR OFFICIAL USE ONLY", sizeof(classificationstr_)-1);
  classcolor_ = 0x80FFFFFF;
  scendata_.clear();
}

void* DCSScenarioHeader::pack(void* buffer,int32_t size) const
{
  char* data=(char*)buffer;

  size_t offset=packit(data);
  if (getVersion() == 0x0200)
  {
    offset+=bewrite((data+offset),&referenceframe_.latorigin_)*sizeof(referenceframe_.latorigin_);
    offset+=bewrite((data+offset),&referenceframe_.lonorigin_)*sizeof(referenceframe_.lonorigin_);
  }
  else
  {
    referenceframe_.pack (data+offset);
    offset+=referenceframe_.dataSize (getVersion());
  }
  offset+=bewrite((data+offset),&referenceyear_)*sizeof(referenceyear_);
  int8_t tmpInt;
  if (getVersion() == 0x0200)
  {
    tmpInt = referenceframe_.coordsystem_;
    offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
    tmpInt = 0;
    offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
  }
  else
  {
    offset+=bewrite ((data+offset), classificationstr_, sizeof(classificationstr_));
    offset+=bewrite ((data+offset), &classcolor_)*sizeof(classcolor_);
  }
  scendata_.pack((data+offset),int32_t(size-offset));

  return buffer;
}

void DCSScenarioHeader::unpack(const void* buffer,int32_t size)
{
  char* data=(char*)buffer;

  size_t offset=unpackit(data);
  if (getVersion() == 0x0200)
  {
    offset+=beread((data+offset),&referenceframe_.latorigin_)*sizeof(referenceframe_.latorigin_);
    offset+=beread((data+offset),&referenceframe_.lonorigin_)*sizeof(referenceframe_.lonorigin_);
  }
  else
  {
    referenceframe_.unpack (data+offset);
    offset += referenceframe_.dataSize (getVersion());
  }
  offset+=beread((data+offset),&referenceyear_)*sizeof(referenceyear_);
  int8_t tmpInt;
  if (getVersion() == 0x0200)
  {
    offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
    referenceframe_.coordsystem_ = (DCSCoordSystem_t)tmpInt;
    offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
    strncpy (classificationstr_, DCS::getClassificationString ((DCSClassification_t)tmpInt), sizeof(classificationstr_)-1);
    classcolor_ = DCS::getClassificationColor((DCSClassification_t)tmpInt);
  }
  else
  {
    offset+=beread((data+offset),classificationstr_,sizeof(classificationstr_));
    offset+=beread((data+offset),&classcolor_)*sizeof(classcolor_);
  }
  scendata_.unpack((data+offset),int32_t(size-offset));
}
