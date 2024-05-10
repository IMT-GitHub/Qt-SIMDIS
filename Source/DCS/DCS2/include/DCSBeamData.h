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

#ifndef _DCSBEAMDATA_H_
#define _DCSBEAMDATA_H_

#include "DCSBaseData.h"

///Beam data class.
/**
 * Class containting information for updating and displaying a
 * beam in the Simdis application.  
 */
class DCSBeamData : public DCSBaseData
{
  public:
    ///Current time.  
    double time_;		/**< Time at which data is valid in seconds (with an optional fraction of a second).  */
    ///Azimuth.  
    double azim_;		/**< Beam bearing in radians.  */
    ///Elevation.  
    double elev_;		/**< Beam elevation in radians.  */
    ///Length.  
    double length_;		/**< Beam length in meters.  */

  public:
    ///Default beam data constructor.  
    /**
     * Creates a beam data object with default data values.
     */
    DCSBeamData();

    ///Beam data constructor.  
    /**
     * Creates a beam data object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param time a double specifying the time at which the data is valid.
     * @param azim a double specifying the beam's azimuth.  
     * @param elev a double specifying the beam's elevation.  
     * @param length a double specifying the beam's length.  
     */
    DCSBeamData(uint64_t id,double time,double azim,double elev,double length);

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
					sizeof(azim_)+sizeof(elev_)+sizeof(length_);}

    ///Determine object's physical size.
    /**
     * @see DCSBaseData::sizeOf()
     */
    virtual int32_t sizeOf() const {return sizeof(*this);}
};

#endif
