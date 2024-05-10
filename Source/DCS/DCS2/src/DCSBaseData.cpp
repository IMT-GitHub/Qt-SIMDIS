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

int32_t DCSBaseData::packit(void* buffer) const
{
  char* data=(char*)buffer;
  size_t total=bewrite(data,&type_)*sizeof(type_);
  total+=bewrite(data+total,&version_)*sizeof(version_);
  total+=bewrite(data+total,&id_)*sizeof(id_);
  return (int32_t) total;
}

int32_t DCSBaseData::unpackit(const void* buffer)
{
  char* data=(char*)buffer;
  size_t total=beread(data,&type_)*sizeof(type_);
  total+=beread(data+total,&version_)*sizeof(version_);
  total+=beread(data+total,&id_)*sizeof(id_);
  return (int32_t) total;
}

