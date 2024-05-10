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
#include "DCSLogger.h"
#include "DCSFileLogger.h"
#include "DCSCategoryData.h"
#include "Constants/ConstantsVariableType.h"
#include <sstream>

DCSFileLogger::DCSFileLogger(const char* filename,bool truncate)
: closefile(true)
{
  file=fopen(filename,(truncate)?"w":"a");
}

DCSFileLogger::DCSFileLogger(FILE* fp)
: file(fp),
  closefile(false)
{
}

DCSFileLogger::DCSFileLogger(int32_t fd)
: closefile(false)
{
  file=fdopen(fd,"w");
}

DCSFileLogger::~DCSFileLogger()
{
  if(closefile)
    fclose(file);
}

bool DCSFileLogger::isValid() const
{
  return (file!=NULL);
}

bool DCSFileLogger::hasError() const
{
  bool err=ferror(file)?true:false;
  clearerr(file);
  return (err!=0);
}

bool DCSFileLogger::flush() const
{
  return (fflush(file)==0)?true:false;
}

bool DCSFileLogger::logBaseData(const DCSBaseData* item) const
{
  fprintf(file,"\tType:\t\t%d\n",item->getType());
  fprintf(file,"\tID:\t\t%s\n",DCS::uint64totext(item->id_));
  fprintf(file,"\tVersion:\t0x%04x\n",item->getVersion());
  return (ferror(file)==0);
}

bool DCSFileLogger::logCoordReferenceFrame (const DCSCoordReferenceFrame& refFrame) const
{
  fprintf(file,"\tCoord System:\t%d\n",refFrame.coordsystem_);
  fprintf(file,"\tLat Origin:\t%lf\n",refFrame.latorigin_);
  fprintf(file,"\tLon Origin:\t%lf\n",refFrame.lonorigin_);
  fprintf(file,"\tAlt Origin:\t%lf\n",refFrame.altorigin_);
  fprintf(file,"\tVert Datum:\t%d\n",refFrame.verticalDatum_);
  fprintf(file,"\tVert Offset:\t%lf\n",refFrame.verticalOffset_);
  fprintf(file,"\tMag Variance:\t%d\n",refFrame.magneticVariance_);
  fprintf(file,"\tMag Offset:\t%lf\n",refFrame.magneticOffset_);
  fprintf(file,"\tECI Reference:\t%lf\n",refFrame.referenceTimeECI_);
  fprintf(file,"\tTan Plane X:\t%lf\n",refFrame.tangentOffsetX_);
  fprintf(file,"\tTan Plane Y:\t%lf\n",refFrame.tangentOffsetY_);
  fprintf(file,"\tTan Angle:\t%lf\n",refFrame.tangentAngle_);
  return (ferror(file)==0);
}

bool DCSFileLogger::logPlatformHeader(const DCSPlatformHeader* item) const
{
  //Print a title
  fprintf(file,"-I-DCSPlatformHeader-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log platform header contents
  fprintf(file,"\tIcon Name:\t%s\n",item->iconname_);
  fprintf(file,"\tCall Sign:\t%s\n",item->callsign_);
  fprintf(file,"\tDimensions:\t%lf %lf %lf\n",item->dims_.x,item->dims_.y,item->dims_.z);
  fprintf(file,"\tOffset:\t\t%lf %lf %lf\n",item->offset_.x,item->offset_.y,item->offset_.z);
  fprintf(file,"\tOri Offset:\t%lf %lf %lf\n",item->orientationoffset_.x,item->orientationoffset_.y,item->orientationoffset_.z);
  fprintf(file,"\tColor:\t\t0x%08X\n",item->color_);
  fprintf(file,"\tDraw State:\t%d\n",item->state_);
  fprintf(file,"\tTrack Status:\t%d\n",item->status_);
  fprintf(file,"\tInterpolate:\t%d\n",item->interpolate_ ? 1 : 0);
  logCoordReferenceFrame (item->referenceframe_);

  return (ferror(file)==0);
}

bool DCSFileLogger::logPlatformData(const DCSPlatformData* item) const
{
  //Print a title
  fprintf(file,"-I-DCSPlatformData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log platform data contents
  fprintf(file,"\tPosition:\t%lf %lf %lf\n",item->position_.x,item->position_.y,item->position_.z);
  fprintf(file,"\tOrientation:\t%lf %lf %lf\n",item->orientation_.x,item->orientation_.y,item->orientation_.z);
  fprintf(file,"\tVelocity:\t%lf %lf %lf\n",item->velocity_.x,item->velocity_.y,item->velocity_.z);
  fprintf(file,"\tAcceleration:\t%lf %lf %lf\n",item->accel_.x,item->accel_.y,item->accel_.z);
  fprintf(file,"\tTime:\t\t%lf\n",item->time_);

  return (ferror(file)==0);
}

bool DCSFileLogger::logBeamHeader(const DCSBeamHeader* item) const
{
  //Print a title
  fprintf(file,"-I-DCSBeamHeader-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log beam header contents
  fprintf(file,"\tCall Sign:\t%s\n",item->callsign_);
  fprintf(file,"\tOffset:\t\t%lf %lf %lf\n",item->offset_.x,item->offset_.y,item->offset_.z);
  fprintf(file,"\tAzim Offset:\t%lf\n",item->azimoffset_);
  fprintf(file,"\tElev Offset:\t%lf\n",item->elevoffset_);
  fprintf(file,"\tMissile Offset:\t%d\n",item->missileoffset_ ? 1 : 0);
  fprintf(file,"\tHor Beam Wid:\t%lf\n",item->hbw_);
  fprintf(file,"\tVert Beam Wid:\t%lf\n",item->vbw_);
  fprintf(file,"\tHost ID:\t%s\n",DCS::uint64totext(item->hostid_));
  fprintf(file,"\tTarget ID:\t%s\n",DCS::uint64totext(item->targetid_));
  fprintf(file,"\tColor:\t\t0x%08X\n",item->color_);
  fprintf(file,"\tDraw State:\t%d\n",item->state_);
  fprintf(file,"\tBeam Type:\t%d\n",item->beamtype_);
  fprintf(file,"\tInterpolate:\t%d\n",item->interpolate_ ? 1 : 0);

  return (ferror(file)==0);
}

bool DCSFileLogger::logBeamData(const DCSBeamData* item) const
{
  //Print a title
  fprintf(file,"-I-DCSBeamData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log beam data
  fprintf(file,"\tTime:\t\t%lf\n",item->time_);
  fprintf(file,"\tAzimuth:\t%lf\n",item->azim_);
  fprintf(file,"\tElevation:\t%lf\n",item->elev_);
  fprintf(file,"\tLength:\t\t%lf\n",item->length_);

  return (ferror(file)==0);
}

bool DCSFileLogger::logGateHeader(const DCSGateHeader* item) const
{
  //Print a title
  fprintf(file,"-I-DCSGateHeader-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log gate header contents
  fprintf(file,"\tCall Sign:\t%s\n",item->callsign_);
  fprintf(file,"\tOffset:\t\t%lf %lf %lf\n",item->offset_.x,item->offset_.y,item->offset_.z);
  fprintf(file,"\tAzim Offset:\t%lf\n",item->azimoffset_);
  fprintf(file,"\tElev Offset:\t%lf\n",item->elevoffset_);
  fprintf(file,"\tHost ID:\t%s\n",DCS::uint64totext(item->hostid_));
  fprintf(file,"\tColor:\t\t0x%08X\n",item->color_);
  fprintf(file,"\tDraw State:\t%d\n",item->state_);
  fprintf(file,"\tGate Type:\t%d\n",item->gatetype_);
  fprintf(file,"\tInterpolate:\t%d\n",item->interpolate_ ? 1 : 0);

  return (ferror(file)==0);
}

bool DCSFileLogger::logGateData(const DCSGateData* item) const
{
  //Print a title
  fprintf(file,"-I-DCSGateData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log gate data contents
  fprintf(file,"\tTime:\t\t%lf\n",item->time_);
  fprintf(file,"\tAzimuth:\t%lf\n",item->azim_);
  fprintf(file,"\tElevation:\t%lf\n",item->elev_);
  fprintf(file,"\tWidth:\t\t%lf\n",item->width_);
  fprintf(file,"\tHeight:\t\t%lf\n",item->height_);
  fprintf(file,"\tMin Range:\t%lf\n",item->minrange_);
  fprintf(file,"\tMax Range:\t%lf\n",item->maxrange_);
  fprintf(file,"\tCentroid:\t%lf\n",item->centroid_);
 
  return (ferror(file)==0);
}

bool DCSFileLogger::logScopeData(const DCSScopeData* item) const
{
  //Print a title
  fprintf(file,"-I-DCSScopeData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log gate data contents
  fprintf(file,"\txUnits:\t\t%d\n",item->xUnits_);
  fprintf(file,"\tyUnits:\t\t%d\n",item->yUnits_);
  fprintf(file,"\tTime:\t\t%lf\n",item->time_);
  fprintf(file,"\txStart:\t\t%lf\n",item->xStart_);
  fprintf(file,"\txStep:\t\t%lf\n",item->xStep_);
  fprintf(file,"\tyBias:\t\t%lf\n",item->yBias_);
  fprintf(file,"\tyScale:\t\t%lf\n",item->yScale_);
  fprintf(file,"\t# DataPoints:\t\t%u\n",item->numDataPoints_);
  fprintf(file,"\tdataType:\t\t%d\n",(int)item->dataType_);
  fprintf(file,"\tnumGates:\t\t%u\n",item->numGates_);
  fprintf(file,"\tData Points:\n");

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
	fprintf(file, "\t\t%d\n", (int)*dataArray);
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
	fprintf(file, "\t\t%d\n", (int)*dataArray);
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
	fprintf(file, "\t\t%d\n", (int)*dataArray);
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
	fprintf(file, "\t\t%d\n", (int)*dataArray);
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
	fprintf(file, "\t\t%u\n", *dataArray);
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
	fprintf(file, "\t\t%d\n", *dataArray);
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
        std::stringstream ss;
        ss << "\t\t" << *dataArray << "\n";
	fprintf(file, "%s", ss.str().c_str());
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
        std::stringstream ss;
        ss << "\t\t" << *dataArray << "\n";
	fprintf(file, "%s", ss.str().c_str());
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
	fprintf(file, "\t\t%f\n", *dataArray);
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
	fprintf(file, "\t\t%lf\n", *dataArray);
	dataArray++;
      }
      break;
    }
    default:
      // dataType unrecognized, do not write any data
      break;
  }

  //Write out the Gates
  fprintf(file, "\tGates:\n");
  int i = 0;
  for (i=0; i<item->numGates_; i++)
  {
    DCSScopeGateData *gate = item->gates_;
    
    fprintf(file, "\t\tid:\t%d\n", gate->id_);
    fprintf(file, "\t\tstart:\t%lf\n", gate->start_);
    fprintf(file, "\t\tcenter:\t%lf\n", gate->center_);
    fprintf(file, "\t\tend:\t%lf\n", gate->end_);
    gate++;
  }
  
  return (ferror(file)==0);
} 

bool DCSFileLogger::logTimeHeader(const DCSTimeHeader* item) const
{
  //Print a title
  fprintf(file,"-I-DCSTimeHeader-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log time header contents
  fprintf(file,"\tRealtime Mod:\t%lf\n",item->realtimemod_);
  fprintf(file,"\tTime Type:\t%d\n",item->timetype_);

  return (ferror(file)==0);
}

bool DCSFileLogger::logTimeData(const DCSTimeData* item) const
{
  //Print a title
  fprintf(file,"-I-DCSTimeData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log time data contents
  fprintf(file,"\tTime:\t\t%lf\n",item->time_);

  return (ferror(file)==0);
}

bool DCSFileLogger::logScenarioHeader(const DCSScenarioHeader* item) const
{
  //Print a title
  fprintf(file,"-I-DCSScenarioHeader-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log scenario header data
  logCoordReferenceFrame (item->referenceframe_);
  fprintf(file,"\tRef Year:\t%d\n",item->referenceyear_);
  fprintf(file,"\tClassification:\t%s\n",item->classificationstr_);
  fprintf(file,"\tClass. Color:\t%08x\n",item->classcolor_);

  return (ferror(file)==0);
}

bool DCSFileLogger::logScenarioData(const DCSScenarioData* item) const
{
  //Print a title
  fprintf(file,"-I-DCSScenarioData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log scenario data contents
  fprintf(file,"\tWind Direction:\t%lf\n",item->winddirection_);
  fprintf(file,"\tWind Speed:\t%lf\n",item->windspeed_);

  return (ferror(file)==0);
}

bool DCSFileLogger::logEvent(const DCSEvent* item) const
{
  //Print a title
  fprintf(file,"-I-DCSEvent-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log time data contents
  fprintf(file,"\tTime:\t\t%lf\n",item->time_);
  fprintf(file,"\tData:\t\t%s\n",DCS::uint64totext(item->data_));
  fprintf(file,"\tChar Data:\t\t%s\n",item->chardata_);
  fprintf(file,"\tEvent Type:\t%d\n",item->eventtype_);

  return true;
  return (ferror(file)==0);
}

bool DCSFileLogger::logGenericData(const DCSGenericData* item) const
{
  //Print a title
  fprintf(file,"-I-DCSGenericData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log generic data contents
  fprintf(file,"\tBegin Time:\t%lf\n",item->begintime_);
  fprintf(file,"\tExpire Time:\t%lf\n",item->expiretime_);
  fprintf(file,"\tTag:\t\t%s\n",item->tag_);
  fprintf(file,"\tData:\t\t%s\n",item->data_);

  return (ferror(file)==0);
}

bool DCSFileLogger::logCategoryData(const DCSCategoryData* item) const
{
  //Print a title
  fprintf(file,"-I-DCSCategoryData-I-\n");

  //Log the base data contents
  logBaseData(item);

  //Log generic data contents
  fprintf(file,"\tTime:\t%lf\n",item->time_);
  fprintf(file,"\tCategory Name:\t\t%s\n",item->name_);
  fprintf(file,"\tValue:\t\t%s\n",item->value_);

  return (ferror(file)==0);
}

bool DCSFileLogger::log(const char* item) const
{
  fprintf(file,"%s",item);
  return (ferror(file)==0);
}
