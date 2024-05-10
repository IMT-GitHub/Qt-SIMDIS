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
 * July 18, 2006 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 *
 *
 */
#ifndef DCSCATEGORYDATA_H
#define DCSCATEGORYDATA_H

#include <string>
#include "DCSBaseData.h"

///Category data class.
/**
 * Class containting information for sending category data 
 * objects to the Simdis application.  
 */
class DCSCategoryData : public DCSBaseData
{
  public:
    ///Category data activation time.  
    double time_;		/**< Time at which the category data becomes valid.  */
    ///Category name.
    char name_[64];		/**< Category data identification category name.  */
    ///Category value.
    char value_[64];		/**< The buffer value for the category.  */

  public:
    ///Default category data constructor.  
    /**
     * Creates a category data object with default data values.
     */
    DCSCategoryData();

    ///Generic data constructor.  
    /**
     * Creates a category data object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param timeVal a double specifying the time of validity.  
     * @param name an array of bytes specifying the category data category name.
     * @param value an array of bytes specifying the category data value.
     */
    DCSCategoryData(uint64_t id,double timeVal,const char* name,const char* value);
    
    ///Set contents of data object to default values.  
    /**
     * Clears contents of data object, setting all fields to default values.  
     */
    virtual void clear();

    ///Prepare data for network transmission.
    /**
     * @see DCSBaseData::pack()
     */
    virtual void* pack(void* buffer,int32_t size) const;

    ///Extract data from network transmission.
    /**
     * @see DCSBaseData::unpack()
     */
    virtual void unpack(const void* buffer,int32_t size);

    ///Determine object's data size.
    /**
     * @see DCSBaseData::dataSize()
     */
    virtual int32_t dataSize() const {return DCSBaseData::dataSize()+sizeof(time_)+
					sizeof(name_)+sizeof(value_);}

    ///Determine object's physical size.
    /**
     * @see DCSBaseData::sizeOf()
     */
    virtual int32_t sizeOf() const {return sizeof(*this);}
};

#endif /* DCSCATEGORYDATA_H */
