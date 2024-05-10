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

bool DCSLogger::log(const DCSBaseData* item) const
{
  switch(item->getType())
  {
    case DCSPLATFORMHEADER:
      return logPlatformHeader((const DCSPlatformHeader*)item);
    case DCSPLATFORMDATA:
      return logPlatformData((const DCSPlatformData*)item);
    case DCSBEAMHEADER:
      return logBeamHeader((const DCSBeamHeader*)item);
    case DCSBEAMDATA:
      return logBeamData((const DCSBeamData*)item);
    case DCSGATEHEADER:
      return logGateHeader((const DCSGateHeader*)item);
    case DCSGATEDATA:
      return logGateData((const DCSGateData*)item);
    case DCSSCOPEDATA:
      return logScopeData((const DCSScopeData*)item);
    case DCSTIMEHEADER:
      return logTimeHeader((const DCSTimeHeader*)item);
    case DCSTIMEDATA:
      return logTimeData((const DCSTimeData*)item);
    case DCSSCENARIOHEADER:
      return logScenarioHeader((const DCSScenarioHeader*)item);
    case DCSSCENARIODATA:
      return logScenarioData((const DCSScenarioData*)item);
    case DCSEVENT:
      return logEvent((const DCSEvent*)item);
    case DCSGENERICDATA:
      return logGenericData((const DCSGenericData*)item);
    case DCSCATEGORYDATA:
      return logCategoryData((const DCSCategoryData*)item);
    case DCSUNKNOWN:
    default:
      return false;
  }
  return false;
}
