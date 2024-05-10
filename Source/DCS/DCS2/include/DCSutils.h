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

#ifndef _DCS_UTILS_H
#define _DCS_UTILS_H 

#include "inttypesc.h"
#include "timec.h"
#include "sockdefs.h"
#include "DCSdefs.h"

namespace DCS
{
  ///uint64 to text converter.
  /**
   * Converts an unsigned 64 bit integer to text.  The text is stored in a character
   * array supplied as a function argument.  A pointer to this array is returned as a
   * convenience.  Text can be represented as base 10 (decimal) or base 16 (hexadecimal).
   * @param id an unsigned 64 bit integer to be converted to an ASCII text string.  
   * @param buffer a pointer to a character array to be filled with the converted ASCII text string.
   * @param len an unsigned 32 bit integer specifying the size in bytes of 'buffer'.
   * @param base an unsigned 32 bit integer indicating the numeric base.  Can be 10 for decimal and 16 for hex.  
   * @return a pointer to the argument 'buffer' for convenience.  
   */
  char* uint64totext(uint64_t id,char* buffer,uint32_t len,uint32_t base=10);

  ///uint64 to text converter.
  /**
   * Converts an unsigned 64 bit integer to text.  The text is stored in a local static
   * array which is overwritten in each call.  Text can be represented as base 10 (decimal)
   * or base 16 (hexadecimal).
   * @param id an unsigned 64 bit integer to be converted to an ASCII text string.  
   * @param base an unsigned 32 bit integer indicating the numeric base.  Can be 10 for decimal and 16 for hex.  
   * @return a character array containing the printable ASCII representation of the
   *         64 bit value.  This array is a local static value which is overwritten in
   *         each call.  
   */
  char* uint64totext(uint64_t id,uint32_t base=10);

  ///DCS 64 bit ID generator
  /**
   * Creates an unsigned 64 bit integer from four unsigned 16 bit integers.  Although this function
   * was created to utilize the common range numbering scheme of a combination of
   * range assigned vehicle id, sensor id, and system id (with an additional unsigned 16 bit integer
   * to specify a range id, when dealing with data from multiple locations), the unsigned 16 bit 
   * integers may contain any value, but at least one must be unique.  
   * @param hihi an unsigned 16 bit integer to be used in the creation of an unique unsigned 64 bit integer.  This
   *        value will be placed in the high order byte of the high order word in the unsigned 64 bit integer
   *        (bits 63..48).  
   * @param hilo an unsigned 16 bit integer to be used in the creation of an unique unsigned 64 bit integer.  This
   *        value will be placed in the low order byte of the high order word in the unsigned 64 bit integer 
   *        (bits 47..32).  
   * @param lohi an unsigned 16 bit integer to be used in the creation of an unique unsigned 64 bit integer.  This
   *        value wil be placed in the high order byte of the low order word in the unsigned 64 bit integer 
   *        (bits 31..16).  
   * @param lolo an unsigned 16 bit integer to be used in the creation of an unique unsigned 64 bit integer.  This
   *        value will be placed in the low order byte of the low order word in the unsigned 64 bit integer 
   *        (bits 15..0).  
   * @return an unsigned 64 bit integer constructed from the four unsigned 16 bit integer input parameters.  
   */
  uint64_t make_id(uint16_t hihi,uint16_t hilo,uint16_t lohi,uint16_t lolo);
  
  ///DCS 64 bit ID deconstructor
  /**
   * Separates an unsigned 64 bit integer into four unsigned 16 bit integers.  This function provides the inverse functionality
   * of the 'make_id' function.  It may be used to obtain the original unsigned 16 bit integers that were
   * used in the creation of a 64 bit ID.
   * @param id an unsigned 64 bit integer to be broken into four unsigned 16 bit integers.  
   * @param hihi a pointer to an unsigned 16 bit integer to be filled with the high order byte of the high
   *        order word in the unsigned 64 bit integer, 'id', (bits 63..48).  
   * @param hilo a pointer to an unsigned 16 bit integer to be filled with the low order byte of the high
   *        order word in the unsigned 64 bit integer, 'id', (bits 47..32).  
   * @param lohi a pointer to an unsigned 16 bit integer to be filled with the high order byte of the low
   *        order word in the unsigned 64 bit integer, 'id', (bits 31..16).  
   * @param lolo a pointer to an unsigned 16 bit integer to be filled with the low order byte of the low
   *        order word in the unsigned 64 bit integer, 'id', (bits 15..0).  
   */
  void break_id(uint64_t id,uint16_t* hihi,uint16_t* hilo,uint16_t* lohi,uint16_t* lolo);

  uint16_t getyear();

  double getyeartime();

  double getyeartime(int16_t year);

  int getdefaultaddr(char* addr,uint32_t len);

  uint32_t getClassificationColor (DCSClassification_t classification);
  const char* getClassificationString (DCSClassification_t classification);

  inline
  const char* getCategoryName (DCSAllegience_t alleg) {return "Affinity";}
  inline
  const char* getCategoryName (DCSPlatformType_t platType) {return "Platform Type";}
  const char* getCategoryValue (DCSAllegience_t alleg);
  const char* getCategoryValue (DCSPlatformType_t platType);
}

#endif // _DCS_UTILS_H
