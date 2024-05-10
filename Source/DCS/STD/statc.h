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

#ifndef _STATC_H_
#define _STATC_H_

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32

#include <io.h>

#define stat _stati64
#define fstat _fstat

#if !defined (S_ISDIR)
#define S_ISDIR(a) (_S_IFDIR & (a))
#endif /* S_ISDIR */

#if !defined (S_ISREG)
#define S_ISREG(a) (_S_IFREG & (a))
#endif /* S_ISREG */

#if !defined (F_OK)
#define F_OK    0        /* Test for existence of file. */
#endif /* F_OK */

#if !defined (R_OK)
#define R_OK    04       /* Test for read permission. */
#endif /* R_OK */

#if !defined (W_OK)
#define W_OK    02       /* Test for write permission. */
#endif /* W_OK */

#if !defined (X_OK)
#define X_OK    01       /* Test for execute permission. */
#endif /* X_OK */

#else /* WIN32 */

#include <unistd.h>

#endif /* !WIN32 */

#endif /* _STATC_H_ */
