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

#ifndef _DCSBASEHEADER_H_
#define _DCSBASEHEADER_H_

#include "DCSBaseData.h"

///An abstract base class for DCS network header data types.
/**
 * All DCS header data types to be sent to the network are derived from this class.
 * Headers are sepearted from general data because they require special processing for
 * storage and client server connection negotiations.  The main purpose of using
 * a base header class is to limit the types of data that can be stored in the DCSHeaderContainer.  
 */
class DCSBaseHeader : public DCSBaseData
{
  public:
    ///Base header constructor.  
    /**
     * Initializes the contents of DCSBaseHeader and sets the data type value.
     * @param type a short representing the type of data structure being created.  
     */
    DCSBaseHeader(int16_t type):DCSBaseData(type){}

    ///Base header constructor.  
    /**
     * Initializes the contents of DCSBaseHeader and sets the id and data type values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param type a short representing the type of data structure being created.  
     */
    DCSBaseHeader(uint64_t id,int16_t type):DCSBaseData(id,type){}

    ///Virtual destructor.  
    /**
     * Important for properly destroying derived objects.
     */
    virtual ~DCSBaseHeader(){}

    ///Prepare data for network transmission.
    /**
     * @see DCSBaseData::pack()
     */
    virtual  void* pack(void* buffer,int32_t size) const=0;

    ///Extract data from network transmission.
    /**
     * @see DCSBaseData::unpack()
     */
    virtual void unpack(const void* buffer,int32_t size)=0;

    ///Determine object's data size.
    /**
     * @see DCSBaseData::dataSize()
     */
    virtual int32_t dataSize() const {return DCSBaseData::dataSize();}

    ///Determine object's data size.
    /**
     * @see DCSBaseData::dataSize()
     */
    virtual int32_t dataSize(uint16_t version) const {return dataSize();}

    ///Determine object's physical size.
    /**
     * @see DCSBaseData::sizeOf()
     */
    virtual int32_t sizeOf() const {return sizeof(*this);}
};

#endif
