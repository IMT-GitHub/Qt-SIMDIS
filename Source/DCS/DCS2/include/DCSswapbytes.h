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

#ifndef _DCSSWAPBYTES_H_
#define _DCSSWAPBYTES_H_

// Internal header file to define byte swapping routines for DCS types.
// Should only be included by DCS .cpp files, never by .h files

#include "swapbytes.h"
#include "DCSPoint.h"

inline void make_big_endian(DCSPoint *const point)
{
  make_big_endian(&point->x);
  make_big_endian(&point->y);
  make_big_endian(&point->z);
}

inline void make_big_endian(DCSPoint *const point,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i)
  {
    make_big_endian(&point[i].x);
    make_big_endian(&point[i].y);
    make_big_endian(&point[i].z);
  }
}

#endif

