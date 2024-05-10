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
#ifndef _DCSPLATFORMDATA_H_
#define _DCSPLATFORMDATA_H_

#include "DCSPoint.h"
#include "DCSBaseData.h"

///Platform data class.
/**
 * Class containting information for updating and displaying a
 * platform in the Simdis application.  
 */
class DCSPlatformData : public DCSBaseData
{
  public:
    ///Object's position.  
    DCSPoint position_;		/**< Object's (lat(rad), lon(rad), alt(m))|(x,y,z) position in meters.  */
    ///Object's orientation.  
    DCSPoint orientation_;	/**< Object's (C,P,R)|(psi,theta,phi) orientation in radians.  */
    ///Object's velocity.  
    DCSPoint velocity_;		/**< Object's (vx,vy,vz) velocity vector in meters/second.  */
    ///Object's acceleration.  
    DCSPoint accel_;		/**< Object's (ax,ay,az) acceleration vector in meters/second**2.  */
    ///Current time.  
    double time_;		/**< Time at which data is valid in seconds (with an optional fraction of a second).  */

  public:
    ///Default platform data constructor.  
    /**
     * Creates a platform data object with default data values.
     */
    DCSPlatformData();

    ///Platform data constructor.  
    /**
     * Creates a platform data object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param position a DCSPoint specifying the object's position.  
     * @param orientation a DCSPoint specifying the object's orientation.  
     * @param velocity a DCSPoint specifying the object's velocity.  
     * @param accel a DCSPoint specifying the object's acceleration.  
     * @param time a double specifying the time at which the data is valid.  
     */
    DCSPlatformData(uint64_t id,const DCSPoint& position,const DCSPoint& orientation,
		    const DCSPoint& velocity,const DCSPoint& accel,
		    double time);
    
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
    virtual int32_t dataSize() const {return DCSBaseData::dataSize()+sizeof(position_)+sizeof(orientation_)+
					sizeof(velocity_)+sizeof(accel_)+sizeof(time_);}

    ///Determine object's physical size.
    /**
     * @see DCSBaseData::sizeOf()
     */
    virtual int32_t sizeOf() const {return sizeof(*this);}
};

#endif
