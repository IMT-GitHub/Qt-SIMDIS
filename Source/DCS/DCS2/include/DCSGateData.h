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

#ifndef _DCSGATEDATA_H_
#define _DCSGATEDATA_H_

#include "DCSBaseData.h"

///Gate data class.
/**
 * Class containting information for updating and displaying a
 * gate in the Simdis application.  
 */
class DCSGateData : public DCSBaseData
{
  public:
    ///Current time.  
    double time_;		/**< Time at which data is valid in seconds (with an optional fraction of a second).  */
    ///Azimuth.  
    double azim_;		/**< Gate bearing in radians.  */
    ///Elevation.  
    double elev_;		/**< Gate elevation in radians.  */
    ///Width.  
    double width_;		/**< Gate width in radians.  */
    ///Height.  
    double height_;		/**< Gate height in radians.  */
    ///Gate start distance.  
    double minrange_;		/**< Distance at which gate starts in meters.  */
    ///Gate end distance.  
    double maxrange_;		/**< Distance at which gate ends in meters.  */
    ///Gate centroid distance.  
    double centroid_;		/**< Distance at which gate centroid is located in meters.  */

  public:
    ///Default gate data constructor.  
    /**
     * Creates a gate data object with default data values.
     */
    DCSGateData();

    ///Gate data constructor.  
    /**
     * Creates a gate data object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param time a double specifying the time at which the data is valid.
     * @param azim a double specifying the gate's azimuth.  
     * @param elev a double specifying the gate's elevation.  
     * @param width a double specifying the gate's width.  
     * @param height a double specifying the gate's height.  
     * @param minrange a double specifying the gate's start distance.  
     * @param maxrange a double specifying the gate's end distance.  
     * @param centroid a double specifying the gate's centroid distance.  
     */
    DCSGateData(uint64_t id,double time,double azim,double elev,double width,double height,
		double minrange,double maxrange,double centroid);

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
					sizeof(azim_)+sizeof(elev_)+sizeof(width_)+
					sizeof(height_)+sizeof(minrange_)+sizeof(maxrange_)+
					sizeof(centroid_);}

    ///Determine object's physical size.
    /**
     * @see DCSBaseData::sizeOf()
     */
    virtual int32_t sizeOf() const {return sizeof(*this);}
};

#endif
