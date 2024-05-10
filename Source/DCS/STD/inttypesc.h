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
 * U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 *
 *
 */

#ifndef INTTYPESC_H
#define INTTYPESC_H

typedef unsigned int uint_t;

#if defined IRIX || defined Solaris

#include <inttypes.h>

#elif defined Linux

#include <stdint.h>

#elif defined WIN32

#  ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#  endif
//#  include <windows.h>
#  include "inttypes_ms.h"
//#  undef TRUE
//#  undef FALSE
//#  undef ABSOLUTE
//#  undef RELATIVE

#else
#error "could not complete posix int type definitions"
#endif

#endif /* INTTYPESC_H */
