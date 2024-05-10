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

#include "stdioc.h"
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
#include "DCSCategoryData.h"
#include "DCSLogger.h"
#include "DCSStreamLogger.h"
#include "Constants/ConstantsVariableType.h"

DCSStreamLogger::DCSStreamLogger(ostream* os)
: stream(os)
{
}

bool DCSStreamLogger::isValid() const
{
  return (stream!=NULL);
}

bool DCSStreamLogger::hasError() const
{
  bool err=stream->bad();
  stream->clear();
  return err;
}

bool DCSStreamLogger::logBaseData(const DCSBaseData* item) const
{
  *stream << "\tType:\t\t" << item->getType() << endl;
  *stream << "\tID:\t\t" << DCS::uint64totext(item->id_) << endl;

  ios::fmtflags opts=stream->flags();
  std::streamsize w=stream->width(4);
  char f=stream->fill('0');
  stream->setf(ostream::hex,ostream::basefield);
  stream->setf(ostream::showbase);
  *stream << "\tVersion:\t" << item->getVersion() << endl;
  stream->flags(opts);
  stream->width(w);
  stream->fill(f);

  return stream->good();
}

bool DCSStreamLogger::logCoordReferenceFrame (const DCSCoordReferenceFrame& refFrame) const
{
  *stream << "\tCoord System:\t" << refFrame.coordsystem_ << endl;
  *stream << "\tLat Origin:\t" << refFrame.latorigin_ << endl;
  *stream << "\tLon Origin:\t" << refFrame.lonorigin_ << endl;
  *stream << "\tAlt Origin:\t" << refFrame.altorigin_ << endl;
  *stream << "\tVert Datum:\t" << refFrame.verticalDatum_ << endl;
  *stream << "\tVert Offset:\t" << refFrame.verticalOffset_ << endl;
  *stream << "\tMag Variance:\t" << refFrame.magneticVariance_ << endl;
  *stream << "\tMag Offset:\t" << refFrame.magneticOffset_ << endl;
  *stream << "\tECI Reference:\t" << refFrame.referenceTimeECI_ << endl;
  *stream << "\tTan Plane X:\t" << refFrame.tangentOffsetX_ << endl;
  *stream << "\tTan Plane Y:\t" << refFrame.tangentOffsetY_ << endl;
  *stream << "\tTan Angle:\t" << refFrame.tangentAngle_ << endl;
  return stream->good();
}

bool DCSStreamLogger::logPlatformHeader(const DCSPlatformHeader* item) const
{
  //Print a title
  *stream << "-I-DCSPlatformHeader-I-" << endl;

  //Log the base data contents
  logBaseData(item);

  //Log platform header contents
  *stream << "\tIcon Name:\t" << item->iconname_ << endl;
  *stream << "\tCall Sign:\t" << item->callsign_ << endl;
  *stream << "\tDimensions:\t" << item->dims_.x << " " << item->dims_.y << " " << item->dims_.z << endl;
  *stream << "\tOffset:\t\t" << item->offset_.x << " " << item->offset_.y << " " << item->offset_.z << endl;
  *stream << "\tOri Offset:\t" << item->orientationoffset_.x << " " << item->orientationoffset_.y << " " << item->orientationoffset_.z << endl;

  char hexval[11];
  sprintf(hexval,"0x%08X",item->color_);
  *stream << "\tColor:\t\t" << hexval << endl;
  
  *stream << "\tDraw State:\t" << item->state_ << endl;
  *stream << "\tTrack Status:\t" << item->status_ << endl;
  *stream << "\tInterpolate:\t" << (item->interpolate_ ? 1 : 0)  << endl;
  logCoordReferenceFrame (item->referenceframe_);

  return stream->good();
}

bool DCSStreamLogger::logPlatformData(const DCSPlatformData* item) const
{
  //Print a title
  *stream << "-I-DCSPlatformData-I-" << endl;

  //Log the base data contents
  logBaseData(item);

  //Log platform data contents
  *stream << "\tPosition:\t" << item->position_.x << " " << item->position_.y << " " << item->position_.z << endl;
  *stream << "\tOrientation:\t" << item->orientation_.x << " " << item->orientation_.y << " " << item->orientation_.z << endl;
  *stream << "\tVelocity:\t" << item->velocity_.x << " " << item->velocity_.y << " " << item->velocity_.z << endl;
  *stream << "\tAcceleration:\t" << item->accel_.x << " " << item->accel_.y << " " << item->accel_.z << endl;
  *stream << "\tTime:\t\t" << item->time_ << endl;

  return stream->good();
}

bool DCSStreamLogger::logBeamHeader(const DCSBeamHeader* item) const
{
  //Print a title
  *stream << "-I-DCSBeamHeader-I-" << endl;

  //Log the base data contents
  logBaseData(item);

  //Log beam header contents
  *stream << "\tCall Sign:\t" << item->callsign_ << endl;
  *stream << "\tOffset:\t\t" << item->offset_.x << " " << item->offset_.y << " " << item->offset_.z << endl;
  *stream << "\tAzim Offset:\t" << item->azimoffset_ << endl;
  *stream << "\tElev Offset:\t" << item->elevoffset_ << endl;
  *stream << "\tMissile Offset:\t" << (item->missileoffset_ ? 1 : 0)  << endl;
  *stream << "\tHor Beam Wid:\t" << item->hbw_ << endl;
  *stream << "\tVert Beam Wid:\t" << item->vbw_ << endl;
  *stream << "\tHost ID:\t" << DCS::uint64totext(item->hostid_) << endl;
  *stream << "\tTarget ID:\t" << DCS::uint64totext(item->targetid_) << endl;

  //It's easier this way
  char hexval[11];
  sprintf(hexval,"0x%08X",item->color_);
  *stream << "\tColor:\t\t" << hexval << endl;
  
  *stream << "\tDraw State:\t" << item->state_ << endl;
  *stream << "\tBeam Type:\t" << item->beamtype_ << endl;
  *stream << "\tInterpolate:\t" << (item->interpolate_ ? 1 : 0)  << endl;

  return stream->good();
}

bool DCSStreamLogger::logBeamData(const DCSBeamData* item) const
{
  //Print a title
  *stream << "-I-DCSBeamData-I-" << endl;

  //Log the base data contents
  logBaseData(item);

  //Log beam data
  *stream << "\tTime:\t\t" << item->time_ << endl;
  *stream << "\tAzimuth:\t" << item->azim_ << endl;
  *stream << "\tElevation:\t" << item->elev_ << endl;
  *stream << "\tLength:\t\t" << item->length_ << endl;

  return stream->good();
}

bool DCSStreamLogger::logGateHeader(const DCSGateHeader* item) const
{
  //Print a title
  *stream << "-I-DCSGateHeader-I-" << endl;

  //Log the base data contents
  logBaseData(item);

  //Log gate header contents
  *stream << "\tCall Sign:\t" << item->callsign_ << endl;
  *stream << "\tOffset:\t\t" << item->offset_.x << " " << item->offset_.y << " " << item->offset_.z << endl;
  *stream << "\tAzim Offset:\t" << item->azimoffset_ << endl;
  *stream << "\tElev Offset:\t" << item->elevoffset_ << endl;
  *stream << "\tHost ID:\t" << DCS::uint64totext(item->hostid_) << endl;

  //It's easier this way
  char hexval[11];
  sprintf(hexval,"0x%08X",item->color_);
  *stream << "\tColor:\t\t" << hexval << endl;
  
  *stream << "\tDraw State:\t" << item->state_ << endl;
  *stream << "\tGate Type:\t" << item->gatetype_ << endl;
  *stream << "\tInterpolate:\t" << (item->interpolate_ ? 1 : 0)  << endl;

  return stream->good();
}

bool DCSStreamLogger::logGateData(const DCSGateData* item) const
{
  //Print a title
  *stream << "-I-DCSGateData-I-" << endl;

  //Log the base data contents
  logBaseData(item);

  //Log gate data contents
  *stream << "\tTime:\t\t" << item->time_ << endl;
  *stream << "\tAzimuth:\t" << item->azim_ << endl;
  *stream << "\tElevation:\t" << item->elev_ << endl;
  *stream << "\tWidth:\t\t" << item->width_ << endl;
  *stream << "\tHeight:\t\t" << item->height_ << endl;
  *stream << "\tMin Range:\t" << item->minrange_ << endl;
  *stream << "\tMax Range:\t" << item->maxrange_ << endl;
  *stream << "\tCentroid:\t" << item->centroid_ << endl;
  
  return stream->good();
}

bool DCSStreamLogger::logScopeData(const DCSScopeData* item) const
{
  // Print a title
  *stream << "-I-DCSScopeData-I-" << endl;

  //Log the base data contents
  logBaseData(item);

  //Log scope data contents
  *stream << "\txUnits:\t\t" << item->xUnits_ << endl;
  *stream << "\tyUnits:\t\t" << item->yUnits_ << endl;
  *stream << "\tTime:\t\t" << item->time_ << endl;
  *stream << "\txStart:\t\t" << item->xStart_ << endl;
  *stream << "\txStep:\t\t" << item->xStep_ << endl;
  *stream << "\tyBias:\t\t" << item->yBias_ << endl;
  *stream << "\tyScale:\t\t" << item->yScale_ << endl;
  *stream << "\t# DataPoints:\t\t" << item->numDataPoints_ << endl;
  *stream << "\tdataType:\t\t" << item->dataType_ << endl;
  *stream << "\tnumGates:\t\t" << item->numGates_ << endl;

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
	*stream << "\t\t" << *dataArray << endl;
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
	*stream << "\t\t" << *dataArray << endl;
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
	*stream << "\t\t" << *dataArray << endl;
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
	*stream << "\t\t" << *dataArray << endl;
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
	*stream << "\t\t" << *dataArray << endl;
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
	*stream << "\t\t" << *dataArray << endl;
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
	*stream << "\t\t" << *dataArray << endl;
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
	*stream << "\t\t" << *dataArray << endl;
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
	*stream << "\t\t" << *dataArray << endl;
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
	*stream << "\t\t" << *dataArray << endl;
	dataArray++;
      }
      break;
    }
    default:
      // dataType unrecognized, do not write any data
      break;
  }

  //Write out the Gates
  *stream << "\tGates: " << endl;
  int i = 0;
  for (i=0; i<item->numGates_; i++)
  {
    DCSScopeGateData *gate = item->gates_;
    
    *stream << "\t\tid:\t" << gate->id_ << endl;
    *stream << "\t\tstart:\t" << gate->start_ << endl;
    *stream << "\t\tcenter:\t" << gate->center_ << endl;
    *stream << "\t\tend:\t" << gate->end_ << endl;

    gate++;
  }
  
  return stream->good();

}


bool DCSStreamLogger::logTimeHeader(const DCSTimeHeader* item) const
{
  //Print a title
  *stream << "-I-DCSTimeHeader-I-" << endl;

  //Log the base data contents
  logBaseData(item);

  //Log time header contents
  *stream << "\tRealtime Mod:\t" << item->realtimemod_ << endl;
  *stream << "\tTime Type:\t" << item->timetype_ << endl;

  return stream->good();
}

bool DCSStreamLogger::logTimeData(const DCSTimeData* item) const
{
  //Print a title
  *stream << "-I-DCSTimeData-I-" << endl;

  //Log the base data contents
  logBaseData(item);

  //Log time data contents
  *stream << "\tTime:\t\t" << item->time_ << endl;

  return stream->good();
}

bool DCSStreamLogger::logScenarioHeader(const DCSScenarioHeader* item) const
{
  //Print a title
  *stream << "-I-DCSScenarioHeader-I-" << endl;

  //Log the base data contents
  logBaseData(item);

  //Log scenario header data
  logCoordReferenceFrame (item->referenceframe_);
  *stream << "\tRef Year:\t" << item->referenceyear_ << endl;
  *stream << "\tClassification:\t" << item->classificationstr_ << endl;
  char hexval[11];
  sprintf(hexval,"0x%08X",item->classcolor_);
  *stream << "\tClass. Color:\t" << hexval << endl;
  
  return stream->good();
}

bool DCSStreamLogger::logScenarioData(const DCSScenarioData* item) const
{
  //Print a title
  *stream << "-I-DCSScenarioData-I-" << endl;

  //Log the base data contents
  logBaseData(item);

  //Log scenario data contents
  *stream << "\tWind Direction:\t" << item->winddirection_ << endl;
  *stream << "\tWind Speed:\t" << item->windspeed_ << endl;

  return stream->good();
}

bool DCSStreamLogger::logEvent(const DCSEvent* item) const
{
  //Print a title
  *stream << "-I-DCSEvent-I-" << endl;

  //Log the base data contents
  logBaseData(item);

  //Log time data contents
  *stream << "\tTime:\t\t" << item->time_ << endl;
  *stream << "\tData:\t\t" << DCS::uint64totext(item->data_) << endl;
  *stream << "\tChar Data:\t\t" << item->chardata_ << endl;
  *stream << "\tEvent Type:\t" << item->eventtype_ << endl;

  return stream->good();
}

bool DCSStreamLogger::logGenericData(const DCSGenericData* item) const
{
  //Print a title
  *stream << "-I-DCSGenericData-I-" << endl;

  //Log the base data contents
  logBaseData(item);

  //Log generic data contents
  *stream << "\tBegin Time:\t" << item->begintime_ << endl;
  *stream << "\tExpire Time:\t" << item->expiretime_ << endl;
  *stream << "\tTag:\t\t" << item->tag_ << endl;
  *stream << "\tData:\t\t" << item->data_ << endl;

  return stream->good();
}

bool DCSStreamLogger::logCategoryData(const DCSCategoryData* item) const
{
  //Print a title
  *stream << "-I-DCSCategoryData-I-" << endl;

  //Log the base data contents
  logBaseData(item);

  //Log generic data contents
  *stream << "\tTime:\t" << item->time_ << endl;
  *stream << "\tCategory Name:\t\t" << item->name_ << endl;
  *stream << "\tValue:\t\t" << item->value_ << endl;

  return stream->good();
}

bool DCSStreamLogger::log(const char* item) const
{
  *stream << item;
  return stream->good();
}
