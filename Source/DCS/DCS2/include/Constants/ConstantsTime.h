/* -*- mode: c++ -*- */
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
 * March 13, 2003 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 */
#ifndef CONSTANTS_TIME_H
#define CONSTANTS_TIME_H

#include <inttypesc.h>

namespace UTILS
{
  static const double CT_INV_60(1./60.);
  static const double CT_INV_3600(1./3600.);

  // Constants need for GPS time
  static const int CT_GPS_EPOCH_SIZE(1024);    /**< Number of weeks in a GPS epoch */
  static const int CT_GPS_START_YEAR(1980);    /**< GPS Time start January 6th, 1980 */
  static const int CT_GPS_START_DAY(5);        /**< 5 since counting starts at zero */
  static const double CT_SECONDS_IN_WEEK(7.0 * 24.0 * 60.0 * 60);
}

#endif /* CONSTANTS_TIME_H */
