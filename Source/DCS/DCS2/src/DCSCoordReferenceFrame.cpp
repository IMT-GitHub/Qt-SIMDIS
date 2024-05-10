// -*- mode: c++ -*-
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
 *               EW Modeling and Simulation, Code 5770
 *               4555 Overlook Ave.
 *               Washington, D.C. 20375-5339
 *
 * For more information please send email to simdis@enews.nrl.navy.mil
 *
 * July 24, 2006 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 *
 *
 */
#include "DCSCoordReferenceFrame.h"
#include "inttypesc.h"
#include "DCSswapbytes.h"

DCSCoordReferenceFrame::DCSCoordReferenceFrame()
  : coordsystem_(DCSCOORD_ENU),
    latorigin_(0.0),
    lonorigin_(0.0),
    altorigin_(0.0),
    verticalDatum_ (DCSVERTDATUM_WGS84),
    verticalOffset_ (0.0),
    magneticVariance_ (DCSMAGVAR_TRUE),
    magneticOffset_ (0.0),
    referenceTimeECI_ (0.0),
    tangentOffsetX_ (0.0),
    tangentOffsetY_ (0.0),
    tangentAngle_ (0.0)
{
}

DCSCoordReferenceFrame::DCSCoordReferenceFrame(DCSCoordSystem_t coordsystem,
					       double latorigin,double lonorigin,double altorigin,
					       DCSVerticalDatum_t vertDatum, double vertOffset,
					       DCSMagneticVariance_t magVar, double magOffset,
					       double eciReference, double tangentX, double tangentY, double tangentAngle)
  : coordsystem_(coordsystem),
    latorigin_(latorigin),
    lonorigin_(lonorigin),
    altorigin_(altorigin),
    verticalDatum_ (vertDatum),
    verticalOffset_ (vertOffset),
    magneticVariance_ (magVar),
    magneticOffset_ (magOffset),
    referenceTimeECI_ (eciReference),
    tangentOffsetX_ (tangentX),
    tangentOffsetY_ (tangentY),
    tangentAngle_ (tangentAngle)
{
}

void DCSCoordReferenceFrame::clear()
{
  coordsystem_=DCSCOORD_ENU;
  latorigin_=0.0;
  lonorigin_=0.0;
  altorigin_=0.0;
  verticalDatum_ = DCSVERTDATUM_WGS84;
  verticalOffset_ = 0.0;
  magneticVariance_ = DCSMAGVAR_TRUE;
  magneticOffset_ = 0.0;
  tangentOffsetX_ = 0.0;
  tangentOffsetY_ = 0.0;
  tangentAngle_ = 0.0;
}

void* DCSCoordReferenceFrame::pack(void* buffer) const
{
  //Test size - throw exception if invalid
  char* data=(char*)buffer;

  size_t offset=0;
  int8_t tmpInt = coordsystem_;
  offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
  offset+=bewrite((data+offset),&latorigin_)*sizeof(latorigin_);
  offset+=bewrite((data+offset),&lonorigin_)*sizeof(lonorigin_);
  offset+=bewrite((data+offset),&altorigin_)*sizeof(altorigin_);
  tmpInt = verticalDatum_;
  offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
  offset+=bewrite((data+offset),&verticalOffset_)*sizeof(verticalOffset_);
  tmpInt = magneticVariance_;
  offset+=bewrite((data+offset),&tmpInt)*sizeof(tmpInt);
  offset+=bewrite((data+offset),&magneticOffset_)*sizeof(magneticOffset_);
  offset+=bewrite((data+offset),&referenceTimeECI_)*sizeof(referenceTimeECI_);
  offset+=bewrite((data+offset),&tangentOffsetX_)*sizeof(tangentOffsetX_);
  offset+=bewrite((data+offset),&tangentOffsetY_)*sizeof(tangentOffsetY_);
  offset+=bewrite((data+offset),&tangentAngle_)*sizeof(tangentAngle_);

  return data+offset;
}

void DCSCoordReferenceFrame::unpack(const void* buffer)
{
  char* data=(char*)buffer;

  size_t offset=0;
  int8_t tmpInt;
  offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
  coordsystem_ = (DCSCoordSystem_t)tmpInt;
  offset+=beread((data+offset),&latorigin_)*sizeof(latorigin_);
  offset+=beread((data+offset),&lonorigin_)*sizeof(lonorigin_);
  offset+=beread((data+offset),&altorigin_)*sizeof(altorigin_);
  offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
  verticalDatum_ = (DCSVerticalDatum_t)tmpInt;
  offset+=beread((data+offset),&verticalOffset_)*sizeof(verticalOffset_);
  offset+=beread((data+offset),&tmpInt)*sizeof(tmpInt);
  magneticVariance_ = (DCSMagneticVariance_t)tmpInt;
  offset+=beread((data+offset),&magneticOffset_)*sizeof(magneticOffset_);
  offset+=beread((data+offset),&referenceTimeECI_)*sizeof(referenceTimeECI_);
  offset+=beread((data+offset),&tangentOffsetX_)*sizeof(tangentOffsetX_);
  offset+=beread((data+offset),&tangentOffsetY_)*sizeof(tangentOffsetY_);
  offset+=beread((data+offset),&tangentAngle_)*sizeof(tangentAngle_);
}

