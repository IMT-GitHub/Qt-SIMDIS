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
#ifndef _DCSTIMEHEADER_H_
#define _DCSTIMEHEADER_H_

#include "DCSTimeData.h"
#include "DCSBaseData.h"

///Time header class.
/**
 * Class containting initial information for specifying the time in the
 * Simdis application.  
 */
class DCSTimeHeader : public DCSBaseData
{
  public:
    ///Real time modification.  
    double realtimemod_;	/**< Percentage of real-time in which the current time is being specified (as a double: 1.0 = 100%, 2.0 = 200%, 0.5 = 50%, etc).  */
    ///Type of time source.  
    int8_t timetype_;		/**< Source from which time is to be aquired.
				 * @see @ref time "Time Sources".  
				 */
    ///Current data value
    DCSTimeData timedata_;	/**< An initial data set for the object associated with this header.  */

  public:
    ///Default time header constructor.  
    /**
     * Creates a time header object with default data values.
     */
    DCSTimeHeader();

    ///Time header constructor.  
    /**
     * Creates a time header object with the specified data values.
     * @param realtimemod a double specifying the percentage of real-time.
     * @param timetype a byte specifying the time source to use.  
     * @param timedata a DCSTimeData object providing an initial data set for the object associated with this header.  
     */
    DCSTimeHeader(double realtimemod,int8_t timetype,const DCSTimeData& timedata);

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
    virtual int32_t dataSize() const {return DCSBaseData::dataSize()+
					sizeof(realtimemod_)+sizeof(timetype_)+
					timedata_.dataSize();}

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
