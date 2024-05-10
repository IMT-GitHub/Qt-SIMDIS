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

#ifndef _STDLIBC_H_
#define _STDLIBC_H_

#include <stdlib.h>
#include <limits.h>

#ifdef Linux
#include <float.h>
#endif

#define HOME "SIMDIS_HOME"

#ifdef WIN32

#ifndef PATH_MAX
#define PATH_MAX _MAX_PATH
#endif

#define VARSEP ';'
#define VARSEP_STR ";"
#define PATHSEP '\\'
#define PATHSEP_STR "\\"
#define TMPDIR "TMP"
#define TMPDIR_DEFAULT "c:\\temp"
#define CURRENT_DIR ".\\"
#define PARENT_DIR "..\\"

#define realpath(filename, resolvedname) _fullpath(resolvedname, filename, PATH_MAX)

#else

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#define VARSEP ':'
#define VARSEP_STR ":"
#define PATHSEP '/'
#define PATHSEP_STR "/"
#define TMPDIR "TMPDIR"
#define TMPDIR_DEFAULT "/tmp"
#define CURRENT_DIR "./"
#define PARENT_DIR "../"

#endif

#endif
