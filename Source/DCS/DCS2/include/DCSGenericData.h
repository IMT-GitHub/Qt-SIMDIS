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

#ifndef _DCSGENERICDATA_H_
#define _DCSGENERICDATA_H_

#include <string>
#include "DCSBaseData.h"

///Generic data class.
/**
 * Class containting information for sending generic data 
 * objects to the Simdis application.  
 */
class DCSGenericData : public DCSBaseData
{
  protected:
    ///Unpacks data from a 0x0200 DCS client
    void unpack0200_ (const void* buffer, int32_t size);

  public:
    ///Generic data activation time.  
    double begintime_;		/**< Time at which the life of the generic data begins.  */
    ///Generic data deactivation time.  
    double expiretime_;		/**< Time at which the life of the generic data expires.  */
    ///Generic data tag.  
    char tag_[64];		/**< Generic data identification tag.  */
    ///Generic data.
    char data_[256];		/**< The buffer containing the generic data.  */

  public:
    ///Default generic data constructor.  
    /**
     * Creates a generic data object with default data values.
     */
    DCSGenericData();

    ///Generic data constructor.  
    /**
     * Creates a generic data object with the specified data values.
     * This constructor currently has the requirement that both the
     * tag and data arguments be provided as null terminated strings.
     * This limitation may make the constructor inadequate for initializing
     * the tag_ and data_ members.  In such cases, it will be necessary to
     * copy generic data directly into the tag_ and data_ arrays through a
     * function such as memcpy.  
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param begintime a double specifying the activation time.  
     * @param expiretime a double specifying the deactivation time.  
     * @param tag an array of bytes specifying the generic data tag.
     * @param data an array of bytes specifying the generic data.
     */
    DCSGenericData(uint64_t id,double begintime,double expiretime,const char* tag,const char* data);
    
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
    virtual int32_t dataSize() const {return dataSize (getVersion());}

    ///Determine object's data size.
    /**
     * Returns the total size, in bytes, required by the data members of
     * the current object.  Can be used to determine the proper size of
     * buffers for the pack and unpack methods.
     * @param version Server version if applicable
     * @return an integer representing the size of the object's data.
     * @see pack()
     * @see unpack()
     */
    virtual int32_t dataSize(uint16_t version) const {
      if (version == 0x0200)
	return DCSBaseData::dataSize()+sizeof(begintime_)+
	  sizeof(expiretime_)+32+72;
      else if (version < 0x0204)
	return DCSBaseData::dataSize()+sizeof(begintime_)+
	  sizeof(expiretime_)+32+sizeof(data_);
      return DCSBaseData::dataSize()+sizeof(begintime_)+
	sizeof(expiretime_)+sizeof(tag_)+sizeof(data_);
    }

    ///Determine object's physical size.
    /**
     * @see DCSBaseData::sizeOf()
     */
    virtual int32_t sizeOf() const {return sizeof(*this);}
};

#endif
