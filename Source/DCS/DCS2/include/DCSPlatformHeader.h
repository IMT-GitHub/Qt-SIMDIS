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

#ifndef _DCSPLATFORMHEADER_H_
#define _DCSPLATFORMHEADER_H_

#include "DCSPlatformData.h"
#include "DCSPoint.h"
#include "DCSBaseHeader.h"
#include "DCSdefs.h"
#include "DCSCoordReferenceFrame.h"

///Platform header class.
/**
 * Class containting information for creating and initializing a
 * platform in the Simdis application.  
 */
class DCSPlatformHeader : public DCSBaseHeader
{
  public:
    ///Object's associated icon name.
    char iconname_[64];		/**< Name of file containing iconic representation of this object.  */
    ///Object's callsign/name.  
    char callsign_[64];		/**< Callsign to be displayed in the object's label.  */
    ///Icon dimensions.
    DCSPoint dims_;		/**< Bounding box for iconic representation of this object.  */
    ///Offset from position.
    DCSPoint offset_;		/**< Platform's (x,y,z) offset from it's specified position.  */
    ///Orientation offset.
    DCSPoint orientationoffset_;/**< Platform's (y,p,r) orientation offset.  */
    ///Platform color.  
    uint32_t color_;		/**< RGBA color of object specified in hex (0xFFFFFFFF).  
                                 * For the header, if this value is zero, it is ignored.  
                                 */
    ///Draw state.
    DCSObjectState_t state_;	/**< Platform draw state indicator.
    				 * @see @ref state "Object States".
				 */
    ///Track quality.
    DCSTrackStatus_t status_;	/**< Platform track status.
    				 * @see @ref status "Track Status".
				 */
    ///Orientation and position will be interpolated or not (defaults true)
    bool interpolate_;		/**< Interpolate orientation and position between data points on playback */
    ///Coordinate system reference frame
    DCSCoordReferenceFrame referenceframe_;	/**< Coordinate reference frame information for data */
    ///Current data value
    DCSPlatformData platdata_;	/**< An initial data set for the object associated with this header.  */


  public:
    ///Default platform header constructor.  
    /**
     * Creates a platform header object with default data values.
     */
    DCSPlatformHeader();

    ///Platform header constructor.
    /**
     * Creates a platform header object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param iconname a string specifying the icon name.  
     * @param callsign a string specifying the call sign.
     * @param dims a DCSPoint structure specifying the icon dimensions.
     * @param offset a DCSPoint structure specifying the platform's offset from it's specified position.  
     * @param oriOffset a DCSPoint structure specifying the platform's orientation offset.  
     * @param color an unsigned integer specifying the RGBA color of object in hex (0xFFFFFF00).
     * @param state a byte specifying the object's draw state.  
     * @param status a byte specifying the object's track status.
     * @param interpolate Indicates whether position and orientation should be interpolated between data points
     * @param refFrame Coordinate reference frame of the platform's data.
     * @param platdata a DCSPlatformData object providing an initial data set for the object associated with this header.  
     */
    DCSPlatformHeader(uint64_t id,
		      const char* iconname,
		      const char* callsign,
		      const DCSPoint& dims,
		      const DCSPoint& offset,
		      const DCSPoint& oriOffset,
		      uint32_t color,
		      DCSObjectState_t state,
		      DCSTrackStatus_t status,
		      bool interpolate,
		      const DCSCoordReferenceFrame& refFrame,
		      const DCSPlatformData& platdata);

    ///Platform header constructor. (deprecated)
    /**
     * Creates a platform header object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param iconname a string specifying the icon name.  
     * @param callsign a string specifying the call sign.
     * @param dims a DCSPoint structure specifying the icon dimensions.
     * @param offset a DCSPoint structure specifying the platform's offset from it's specified position.  
     * @param color an unsigned integer specifying the RGBA color of object in hex (0xFFFFFF00).
     * @param state a byte specifying the object's draw state.  
     * @param status a byte specifying the object's track status.
     * @param interpolate Indicates whether position and orientation should be interpolated between data points
     * @param refFrame Coordinate reference frame of the platform's data.
     * @param platdata a DCSPlatformData object providing an initial data set for the object associated with this header.  
     */
    DCSPlatformHeader(uint64_t id,
		      const char* iconname,
		      const char* callsign,
		      const DCSPoint& dims,
		      const DCSPoint& offset,
		      uint32_t color,
		      DCSObjectState_t state,
		      DCSTrackStatus_t status,
		      bool interpolate,
		      const DCSCoordReferenceFrame& refFrame,
		      const DCSPlatformData& platdata);

    ///Platform header constructor. (deprecated)
    /**
     * Creates a platform header object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param iconname a string specifying the icon name.  
     * @param callsign a string specifying the call sign.
     * @param dims a DCSPoint structure specifying the icon dimensions.
     * @param offset a DCSPoint structure specifying the platform's offset from it's specified position.  
     * @param latorigin a double specifying the latitude topo origin.
     * @param lonorigin a double specifying the longitude topo origin.
     * @param color an unsigned integer specifying the RGBA color of object in hex (0xFFFFFF00).
     * @param state a byte specifying the object's draw state.  
     * @param status a byte specifying the object's track status.  
     * @param coordsystem a byte specifying the coordinate system.
     * @param allegience a byte specifying the allegience of the platform.  
     * @param plattype a byte specifying the platform type.  
     * @param platdata a DCSPlatformData object providing an initial data set for the object associated with this header.  
     */
    DCSPlatformHeader(uint64_t id,
		      const char* iconname,
		      const char* callsign,
		      const DCSPoint& dims,
		      const DCSPoint& offset,
		      double latorigin,
		      double lonorigin,
		      uint32_t color,
		      DCSObjectState_t state,
		      DCSTrackStatus_t status,
		      DCSCoordSystem_t coordsystem,
		      DCSAllegience_t allegience,
		      DCSPlatformType_t plattype,
		      const DCSPlatformData& platdata);

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
    virtual int32_t dataSize() const {return dataSize (DCS_VERSION);}

    ///Determine object's data size.
    /**
     * @see DCSBaseData::dataSize()
     */
    virtual int32_t dataSize(uint16_t version) const;

    ///Determine object's physical size.
    /**
     * @see DCSBaseData::sizeOf()
     */
    virtual int32_t sizeOf() const {return sizeof(*this);}
};

#endif
