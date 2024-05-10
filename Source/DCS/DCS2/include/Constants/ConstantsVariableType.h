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
 *               EW Modeling and Simulation, Code 5770
 *               4555 Overlook Ave.
 *               Washington, D.C. 20375-5339
 *
 * For more information please send email to simdis@enews.nrl.navy.mil
 *
 * March 16, 2006 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 *
 *
 */
#ifndef CONSTANTSVARIABLETYPE_H
#define CONSTANTSVARIABLETYPE_H

#include <string>
#include <vector>
#include "inttypesc.h"
#include <typeinfo>
#include <assert.h>

/** \file ConstantsVariableType.h
 * Defines an enumeration of variable declaration types, used to identify
 * types such as int, float, or char
 */

namespace UTILS
{
  ///Enumeration of base variable types
  enum VariableType_t
  {
    vtUNKNOWN = 0,
    vtUINT8,       ///< unsigned char
    vtINT8,        ///< signed char
    vtUINT16,      ///< unsigned short
    vtINT16,       ///< signed short
    vtUINT32,      ///< unsigned int
    vtINT32,       ///< signed int
    vtUINT64,      ///< unsigned long long
    vtINT64,       ///< signed long long
    vtFLOAT32,     ///< float
    vtFLOAT64,     ///< double
    vtSTRING,      ///< string
    vtBLOB,        ///< array of bytes
  };

  ///Returns number of bytes of a given variable type, returning 0 on error
  inline short getSizeOf (VariableType_t varType)
  {
    switch (varType)
    {
      case vtUINT8:
	return 1;
      case vtINT8:
	return 1;
      case vtUINT16:
	return 2;
      case vtINT16:
	return 2;
      case vtUINT32:
	return 4;
      case vtINT32:
	return 4;
      case vtUINT64:
	return 8;
      case vtINT64:
	return 8;
      case vtFLOAT32:
	return 4;
      case vtFLOAT64:
	return 8;
      case vtSTRING:
	// Strings are variable width
	assert(0);
	return 4;
      case vtBLOB:
	return 4;
      case vtUNKNOWN:
	// Unknown values have unknown width
	assert(0);
	return 0;
    } // Switch on which variable type
    return 0;
  } // getNumBytes()

  typedef std::vector<VariableType_t> vVariableType_t;

  inline bool isFixedStorageType(VariableType_t storageType)
  {
    if((storageType == vtSTRING) || (storageType == vtBLOB))
      return false;
    return true;
  }

  inline bool allFixedStorageTypes(const vVariableType_t& storageTypes, uint64_t* numBytes)
  {
    if(numBytes != NULL)
      *numBytes = 0;
    vVariableType_t::const_iterator typeIter;
    for(typeIter = storageTypes.begin();
	typeIter != storageTypes.end();
	++typeIter)
    {
      if(isFixedStorageType(*typeIter) == false)
	return false;
    if(numBytes != NULL)
      *numBytes = (*numBytes) + getSizeOf(*typeIter);
    }
    return true;
  }

  inline bool storageTypesTheSame(const vVariableType_t& a,
				  const vVariableType_t& b)
  {
    if(a.size() != b.size())
      return false;
    vVariableType_t::const_iterator aIter;
    vVariableType_t::const_iterator bIter;
    for(aIter = a.begin(), bIter = b.begin();
	aIter != a.end();
	++aIter, ++bIter)
    {
      if((*aIter) != (*bIter))
	return false;
    }
    return true;
  }

  template <class StorageType>
  VariableType_t getStorageType(const StorageType& value)
  {
    if(typeid(value) == typeid(uint8_t))
      return vtUINT8;
    if(typeid(value) == typeid(int8_t))
      return vtINT8;
    if(typeid(value) == typeid(uint16_t))
      return vtUINT16;
    if(typeid(value) == typeid(int16_t))
      return vtINT16;
    if(typeid(value) == typeid(uint32_t))
      return vtUINT32;
    if(typeid(value) == typeid(int32_t))
      return vtINT32;
    if(typeid(value) == typeid(uint64_t))
      return vtUINT64;
    if(typeid(value) == typeid(int64_t))
      return vtINT64;
    if(typeid(value) == typeid(float))
      return vtFLOAT32;
    if(typeid(value) == typeid(double))
      return vtFLOAT64;
    if(typeid(value) == typeid(std::string))
      return vtSTRING;
    return vtUNKNOWN;
  }
}

#endif /* CONSTANTSVARIABLETYPE_H */
