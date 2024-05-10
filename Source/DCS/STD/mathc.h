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

#ifndef _MATH_C_H_
#define _MATH_C_H_

#include <math.h>

//
// Function Macros
//
#ifndef FMOD
#define FMOD(a, b) (fmod ((double)((a) + (b)), (double)(b)))
#endif

#ifndef ABS
#define ABS(x)	((x) < 0 ? -(x) : (x))
#endif

#ifndef MIN3
#define MIN3(a,b,c) ((((a)<(b))&&((a)<(c))) ? (a) : (((b)<(c)) ? (b) : (c)))
#endif

#ifndef MAX3
#define MAX3(a,b,c) ((((a)>(b))&&((a)>(c))) ? (a) : (((b)>(c)) ? (b) : (c)))
#endif

#ifndef SQR
#define SQR(x) ((x) * (x))
#endif

#ifndef SGN
#define SGN(a) ((a>=0) ? ((a==0) ? (0):(1)):(-1)) 
#endif

#ifdef Linux
#define finite(x) (sizeof (x) == sizeof (float) ? __finitef (x) : __finite (x))
#elif !defined _WIN32
#include <ieeefp.h>  // finite found here
#endif

#ifdef _WIN32
#include <float.h>
#define copysign _copysign
#ifndef isnan
#define isnan _isnan
#endif
#define finite _finite
#define drem fmod

#ifdef __cplusplus
namespace UTILS
{
inline double acosh(double x) { return log(x+ sqrt(x*x-1.)); }
inline double asinh(double x) { return log(x+ sqrt(x*x+1.)); }
};
using UTILS::acosh;
using UTILS::asinh;
#define HAVE_ACOSH
#endif

#endif

#endif
