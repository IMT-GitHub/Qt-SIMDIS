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

#include <stdio.h>
#include "inttypesc.h"
#include "iostreamc"
#include "DCSswapbytes.h"
#include "DCSdefs.h"
#include "DCSutils.h"
#include "DCSPoint.h"
#include "DCSBaseData.h"
#include "DCSBaseHeader.h"
#include "DCSPlatformData.h"
#include "DCSBeamData.h"
#include "DCSGateData.h"
#include "DCSScopeData.h"
#include "DCSTimeData.h"
#include "DCSScenarioData.h"
#include "DCSPlatformHeader.h"
#include "DCSBeamHeader.h"
#include "DCSGateHeader.h"
#include "DCSTimeHeader.h"
#include "DCSScenarioHeader.h"
#include "DCSEvent.h"
#include "DCSGenericData.h"
#include "DCSCategoryData.h"
#include "DCSLogger.h"
#include "DCSConsoleLogger.h"
#include "Constants/ConstantsVariableType.h"

bool DCSConsoleLogger::logBaseData(const DCSBaseData* item) const
{
  fprintf(stderr,"\tType:\t\t%d\n",item->getType());
  fprintf(stderr,"\tID:\t\t%s\n",DCS::uint64totext(item->id_));
  fprintf(stderr,"\tVersion:\t0x%04x\n",item->getVersion());
  return true;
}

bool DCSConsoleLogger::logCoordReferenceFrame (const DCSCoordReferenceFrame& refFrame) const
{
  fprintf(stderr,"\tCoord System:\t%d\n",refFrame.coordsystem_);
  fprintf(stderr,"\tLat Origin:\t%lf\n",refFrame.latorigin_);
  fprintf(stderr,"\tLon Origin:\t%lf\n",refFrame.lonorigin_);
  fprintf(stderr,"\tAlt Origin:\t%lf\n",refFrame.altorigin_);
  fprintf(stderr,"\tVert Datum:\t%d\n",refFrame.verticalDatum_);
  fprintf(stderr,"\tVert Offset:\t%lf\n",refFrame.verticalOffset_);
  fprintf(stderr,"\tMag Variance:\t%d\n",refFrame.magneticVariance_);
  fprintf(stderr,"\tMag Offset:\t%lf\n",refFrame.magneticOffset_);
  fprintf(stderr,"\tECI Reference:\t%lf\n",refFrame.referenceTimeECI_);
  fprintf(stderr,"\tTan Plane X:\t%lf\n",refFrame.tangentOffsetX_);
  fprintf(stderr,"\tTan Plane Y:\t%lf\n",refFrame.tangentOffsetY_);
  fprintf(stderr,"\tTan Angle:\t%lf\n",refFrame.tangentAngle_);
  return true;
}

bool DCSConsoleLogger::logPlatformHeader(const DCSPlatformHeader* item) const
{
  //Print a title
  fprintf(stderr,"-I-DCSPlatformHeader-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log platform header contents
  fprintf(stderr,"\tIcon Name:\t%s\n",item->iconname_);
  fprintf(stderr,"\tCall Sign:\t%s\n",item->callsign_);
  fprintf(stderr,"\tDimensions:\t%lf %lf %lf\n",item->dims_.x,item->dims_.y,item->dims_.z);
  fprintf(stderr,"\tOffset:\t\t%lf %lf %lf\n",item->offset_.x,item->offset_.y,item->offset_.z);
  fprintf(stderr,"\tOri Offset:\t%lf %lf %lf\n",item->orientationoffset_.x,item->orientationoffset_.y,item->orientationoffset_.z);
  fprintf(stderr,"\tColor:\t\t0x%08X\n",item->color_);
  fprintf(stderr,"\tDraw State:\t%d\n",item->state_);
  fprintf(stderr,"\tTrack Status:\t%d\n",item->status_);
  fprintf(stderr,"\tInterpolate:\t%d\n",item->interpolate_ ? 1 : 0);
  logCoordReferenceFrame (item->referenceframe_);

  return true;
}

bool DCSConsoleLogger::logPlatformData(const DCSPlatformData* item) const
{
  //Print a title
  fprintf(stderr,"-I-DCSPlatformData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log platform data contents
  fprintf(stderr,"\tPosition:\t%lf %lf %lf\n",item->position_.x,item->position_.y,item->position_.z);
  fprintf(stderr,"\tOrientation:\t%lf %lf %lf\n",item->orientation_.x,item->orientation_.y,item->orientation_.z);
  fprintf(stderr,"\tVelocity:\t%lf %lf %lf\n",item->velocity_.x,item->velocity_.y,item->velocity_.z);
  fprintf(stderr,"\tAcceleration:\t%lf %lf %lf\n",item->accel_.x,item->accel_.y,item->accel_.z);
  fprintf(stderr,"\tTime:\t\t%lf\n",item->time_);

  return true;
}

bool DCSConsoleLogger::logBeamHeader(const DCSBeamHeader* item) const
{
  //Print a title
  fprintf(stderr,"-I-DCSBeamHeader-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log beam header contents
  fprintf(stderr,"\tCall Sign:\t%s\n",item->callsign_);
  fprintf(stderr,"\tOffset:\t\t%lf %lf %lf\n",item->offset_.x,item->offset_.y,item->offset_.z);
  fprintf(stderr,"\tAzim Offset:\t%lf\n",item->azimoffset_);
  fprintf(stderr,"\tElev Offset:\t%lf\n",item->elevoffset_);
  fprintf(stderr,"\tMissile Offset:\t%d\n",item->missileoffset_ ? 1 : 0);
  fprintf(stderr,"\tHor Beam Wid:\t%lf\n",item->hbw_);
  fprintf(stderr,"\tVert Beam Wid:\t%lf\n",item->vbw_);
  fprintf(stderr,"\tHost ID:\t%s\n",DCS::uint64totext(item->hostid_));
  fprintf(stderr,"\tTarget ID:\t%s\n",DCS::uint64totext(item->targetid_));
  fprintf(stderr,"\tColor:\t\t0x%08X\n",item->color_);
  fprintf(stderr,"\tDraw State:\t%d\n",item->state_);
  fprintf(stderr,"\tBeam Type:\t%d\n",item->beamtype_);
  fprintf(stderr,"\tInterpolate:\t%d\n",item->interpolate_ ? 1 : 0);

  return true;
}

bool DCSConsoleLogger::logBeamData(const DCSBeamData* item) const
{
  //Print a title
  fprintf(stderr,"-I-DCSBeamData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log beam data
  fprintf(stderr,"\tTime:\t\t%lf\n",item->time_);
  fprintf(stderr,"\tAzimuth:\t%lf\n",item->azim_);
  fprintf(stderr,"\tElevation:\t%lf\n",item->elev_);
  fprintf(stderr,"\tLength:\t\t%lf\n",item->length_);

  return true;
}

bool DCSConsoleLogger::logGateHeader(const DCSGateHeader* item) const
{
  //Print a title
  fprintf(stderr,"-I-DCSGateHeader-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log gate header contents
  fprintf(stderr,"\tCall Sign:\t%s\n",item->callsign_);
  fprintf(stderr,"\tOffset:\t\t%lf %lf %lf\n",item->offset_.x,item->offset_.y,item->offset_.z);
  fprintf(stderr,"\tHost ID:\t%s\n",DCS::uint64totext(item->hostid_));
  fprintf(stderr,"\tColor:\t\t0x%08X\n",item->color_);
  fprintf(stderr,"\tDraw State:\t%d\n",item->state_);
  fprintf(stderr,"\tGate Type:\t%d\n",item->gatetype_);
  fprintf(stderr,"\tInterpolate:\t%d\n",item->interpolate_ ? 1 : 0);

  return true;
}

bool DCSConsoleLogger::logGateData(const DCSGateData* item) const
{
  //Print a title
  fprintf(stderr,"-I-DCSGateData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log gate data contents
  fprintf(stderr,"\tTime:\t\t%lf\n",item->time_);
  fprintf(stderr,"\tAzimuth:\t%lf\n",item->azim_);
  fprintf(stderr,"\tElevation:\t%lf\n",item->elev_);
  fprintf(stderr,"\tWidth:\t\t%lf\n",item->width_);
  fprintf(stderr,"\tHeight:\t\t%lf\n",item->height_);
  fprintf(stderr,"\tMin Range:\t%lf\n",item->minrange_);
  fprintf(stderr,"\tMax Range:\t%lf\n",item->maxrange_);
  fprintf(stderr,"\tCentroid:\t%lf\n",item->centroid_);
  
  return true;
}

bool DCSConsoleLogger::logScopeData(const DCSScopeData* item) const
{
  //Print a title
  fprintf(stderr,"-I-DCSScopeData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log gate data contents
  fprintf(stderr,"\txUnits:\t\t%d\n",(int)item->xUnits_);
  fprintf(stderr,"\tyUnits:\t\t%d\n",(int)item->yUnits_);
  fprintf(stderr,"\tTime:\t\t%lf\n",item->time_);
  fprintf(stderr,"\txStart:\t\t%lf\n",item->xStart_);
  fprintf(stderr,"\txStep:\t\t%lf\n",item->xStep_);
  fprintf(stderr,"\tyBias:\t\t%lf\n",item->yBias_);
  fprintf(stderr,"\tyScale:\t\t%lf\n",item->yScale_);
  fprintf(stderr,"\t# DataPoints:\t\t%u\n",item->numDataPoints_);
  fprintf(stderr,"\tdataType:\t\t%d\n",(int)item->dataType_);
  fprintf(stderr,"\tnumGates:\t\t%u\n",item->numGates_);
  fprintf(stderr,"\tData Points:\n");

  // write out the data points
  if (item->numDataPoints_ == 0)
    return true;

  switch (item->dataType_)
  {
    case UTILS::vtUINT8:
    {
      uint32_t i = 0;
      uint8_t *dataArray = (uint8_t*) item->data_;
      for (i=0; i < item->numDataPoints_*sizeof(uint8_t); i+=sizeof(uint8_t))
      {
	fprintf(stderr, "\t\t%d\n", (int)*dataArray);
	dataArray++;
      }
      break;
    }
    case UTILS::vtINT8:
    {
      uint32_t i = 0;
      int8_t *dataArray = (int8_t*) item->data_;
      for (i=0; i < item->numDataPoints_*sizeof(int8_t); i+=sizeof(int8_t))
      {
	fprintf(stderr, "\t\t%d\n", (int)*dataArray);
	dataArray++;
      }
      break;
    }
    case UTILS::vtUINT16:
    {
      uint32_t i = 0;
      uint16_t *dataArray = (uint16_t*) item->data_;
      for (i=0; i < item->numDataPoints_*sizeof(uint16_t); i+=sizeof(uint16_t))
      {
	fprintf(stderr, "\t\t%d\n", (int)*dataArray);
	dataArray++;
      }
      break;
    }
    case UTILS::vtINT16:
    {
      uint32_t i = 0;
      int16_t *dataArray = (int16_t*) item->data_;
      for (i=0; i < item->numDataPoints_*sizeof(int16_t); i+=sizeof(int16_t))
      {
	fprintf(stderr, "\t\t%d\n", (int)*dataArray);
	dataArray++;
      }
      break;
    }
    case UTILS::vtUINT32:
    {
      uint32_t i = 0;
      uint32_t *dataArray = (uint32_t*) item->data_;
      for (i=0; i < item->numDataPoints_*sizeof(uint32_t); i+=sizeof(uint32_t))
      {
	fprintf(stderr, "\t\t%u\n", *dataArray);
	dataArray++;
      }
      break;
    }
    case UTILS::vtINT32:
    {
      uint32_t i = 0;
      int32_t *dataArray = (int32_t*) item->data_;
      for (i=0; i < item->numDataPoints_*sizeof(int32_t); i+=sizeof(int32_t))
      {
	fprintf(stderr, "\t\t%d\n", *dataArray);
	dataArray++;
      }
      break;
    }
    case UTILS::vtUINT64:
    {
      uint32_t i = 0;
      uint64_t *dataArray = (uint64_t*) item->data_;
      for (i=0; i < item->numDataPoints_*sizeof(uint64_t); i+=sizeof(uint64_t))
      {
        cerr << "\t\t" << *dataArray << "\n";
	dataArray++;
      }
      break;
    }
    case UTILS::vtINT64:
    {
      uint32_t i = 0;
      int64_t *dataArray = (int64_t*) item->data_;
      for (i=0; i < item->numDataPoints_*sizeof(int64_t); i+=sizeof(int64_t))
      {
        cerr << "\t\t" << *dataArray << "\n";
	dataArray++;
      }
      break;
    }
    case UTILS::vtFLOAT32:
    {
      uint32_t i = 0;
      float *dataArray = (float*) item->data_;
      for (i=0; i < item->numDataPoints_*sizeof(float); i+=sizeof(float))
      {
	fprintf(stderr, "\t\t%f\n", *dataArray);
	dataArray++;
      }
      break;
    }
    case UTILS::vtFLOAT64:
    {
      uint32_t i = 0;
      double *dataArray = (double*) item->data_;
      for (i=0; i < item->numDataPoints_*sizeof(double); i+=sizeof(double))
      {
	fprintf(stderr, "\t\t%lf\n", *dataArray);
	dataArray++;
      }
      break;
    }
    default:
      // dataType unrecognized, do not write any data
      break;
  }

  //Write out the Gates
  fprintf(stderr, "\tGates:\n");
  int i = 0;
  for (i=0; i<item->numGates_; i++)
  {
    DCSScopeGateData *gate = item->gates_;
    
    fprintf(stderr, "\t\tid:\t%d\n", gate->id_);
    fprintf(stderr, "\t\tstart:\t%lf\n", gate->start_);
    fprintf(stderr, "\t\tcenter:\t%lf\n", gate->center_);
    fprintf(stderr, "\t\tend:\t%lf\n", gate->end_);
    gate++;
  }
      
  return true;
}


bool DCSConsoleLogger::logTimeHeader(const DCSTimeHeader* item) const
{
  //Print a title
  fprintf(stderr,"-I-DCSTimeHeader-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log time header contents
  fprintf(stderr,"\tRealtime Mod:\t%lf\n",item->realtimemod_);
  fprintf(stderr,"\tTime Type:\t%d\n",item->timetype_);

  return true;
}

bool DCSConsoleLogger::logTimeData(const DCSTimeData* item) const
{
  //Print a title
  fprintf(stderr,"-I-DCSTimeData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log time data contents
  fprintf(stderr,"\tTime:\t\t%lf\n",item->time_);

  return true;
}

bool DCSConsoleLogger::logScenarioHeader(const DCSScenarioHeader* item) const
{
  //Print a title
  fprintf(stderr,"-I-DCSScenarioHeader-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log scenario header data
  logCoordReferenceFrame (item->referenceframe_);
  fprintf(stderr,"\tRef Year:\t%d\n",item->referenceyear_);
  fprintf(stderr,"\tClassification:\t%s\n",item->classificationstr_);
  fprintf(stderr,"\tClass. Color:\t%08x\n",item->classcolor_);
  
  return true;
}

bool DCSConsoleLogger::logScenarioData(const DCSScenarioData* item) const
{
  //Print a title
  fprintf(stderr,"-I-DCSScenarioData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log scenario data contents
  fprintf(stderr,"\tWind Direction:\t%lf\n",item->winddirection_);
  fprintf(stderr,"\tWind Speed:\t%lf\n",item->windspeed_);

  return true;
}

bool DCSConsoleLogger::logEvent(const DCSEvent* item) const
{
  //Print a title
  fprintf(stderr,"-I-DCSEvent-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log time data contents
  fprintf(stderr,"\tTime:\t\t%lf\n",item->time_);
  fprintf(stderr,"\tData:\t\t%s\n",DCS::uint64totext(item->data_));
  fprintf(stderr,"\tEvent Type:\t%d\n",item->eventtype_);

  return true;
}

bool DCSConsoleLogger::logGenericData(const DCSGenericData* item) const
{
  //Print a title
  fprintf(stderr,"-I-DCSGenericData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log generic data contents
  fprintf(stderr,"\tBegin Time:\t%lf\n",item->begintime_);
  fprintf(stderr,"\tExpire Time:\t%lf\n",item->expiretime_);
  fprintf(stderr,"\tTag:\t\t%s\n",item->tag_);
  fprintf(stderr,"\tData:\t\t%s\n",item->data_);

  return true;
}

bool DCSConsoleLogger::logCategoryData(const DCSCategoryData* item) const
{
  //Print a title
  fprintf(stderr,"-I-DCSCategoryData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log generic data contents
  fprintf(stderr,"\tTime:\t%lf\n",item->time_);
  fprintf(stderr,"\tCategory Name:\t\t%s\n",item->name_);
  fprintf(stderr,"\tValue:\t\t%s\n",item->value_);

  return true;
}

bool DCSConsoleLogger::log(const char* item) const
{
  fprintf(stderr,"%s",item);
  return true;
}
