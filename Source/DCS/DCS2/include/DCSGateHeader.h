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
#ifndef _DCSGATEHEADER_H_
#define _DCSGATEHEADER_H_

#include "DCSGateData.h"
#include "DCSBaseHeader.h"
#include "DCSPoint.h"

///Gate header class.
/**
 * Class containting information for creating and initializing a
 * gate in the Simdis application.  
 */
class DCSGateHeader : public DCSBaseHeader
{
  public:
    ///Gate's callsign/name.
    char callsign_[64];		/**< Callsign to be displayed in gate's label.  */
    ///Gate offset.
    DCSPoint offset_;		/**< Gate's (x,y,z) offset from centroid position.  */
    ///Azimuth offset.
    double azimoffset_;		/**< Azimuth offset of gate in radians.  */
    ///Elevation offset.
    double elevoffset_;		/**< Elevation offset of gate in radians.  */
    ///Host beam id.  
    int64_t hostid_;		/**< Id of beam with which gate is attached.  */
    ///Gate color.
    uint32_t color_;		/**< RGBA color of object specified in hex (0xFFFFFFFF).  
                                 * For the header, if this value is zero, it is ignored.  
                                 */
    ///Draw state.
    DCSObjectState_t state_;	/**< Gate draw state indicator.
    				 * @see @ref state "Object States".
				 */
    ///Gate type.  
    DCSGateType_t gatetype_;	/**< The object's gate type.
				 * @see @ref gatetypes "Gate Types".
				 */
    ///Position will be interpolated or not (defaults true)
    bool interpolate_;		/**< Interpolate position between data points on playback */
    ///Current data value
    DCSGateData gatedata_;	/**< An initial data set for the object associated with this header.  */

  public:
    ///Default gate header constructor.  
    /**
     * Creates a gate header object with default data values.
     */
    DCSGateHeader();

    ///Gate header constructor.
    /**
     * Creates a gate header object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param callsign a string specifying the call sign.  
     * @param offset a DCSPoint specifying the gate's offset from the centroid's position.  
     * @param azimoffset a double specifying the azimuth offset.  
     * @param elevoffset a double specifying the elevation offset.
     * @param hostid a 64-bit integer specifying the id of the associated beam.
     * @param color an unsigned integer specifying the RGBA color of object in hex (0xFFFFFF00).
     * @param state a byte specifying the object's draw state.  
     * @param gatetype a byte specifying the gate type.  
     * @param interpolate Indicates whether position should be interpolated between data points
     * @param gatedata a DCSGateData object providing an initial data set for the object associated with this header.  
     */
    DCSGateHeader(uint64_t id,
		  const char* callsign,
		  const DCSPoint& offset,
		  double azimoffset,
		  double elevoffset,
		  int64_t hostid,
		  uint32_t color,
		  DCSObjectState_t state,
		  DCSGateType_t gatetype,
		  bool interpolate,
		  const DCSGateData& gatedata);

    ///Gate header constructor. (deprecated)
    /**
     * Creates a gate header object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param callsign a string specifying the call sign.  
     * @param offset a DCSPoint specifying the gate's offset from the centroid's position.  
     * @param hostid a 64-bit integer specifying the id of the associated beam.
     * @param color an unsigned integer specifying the RGBA color of object in hex (0xFFFFFF00).
     * @param state a byte specifying the object's draw state.  
     * @param gatetype a byte specifying the gate type.  
     * @param interpolate Indicates whether position should be interpolated between data points
     * @param gatedata a DCSGateData object providing an initial data set for the object associated with this header.  
     */
    DCSGateHeader(uint64_t id,
		  const char* callsign,
		  const DCSPoint& offset,
		  int64_t hostid,
		  uint32_t color,
		  DCSObjectState_t state,
		  DCSGateType_t gatetype,
		  bool interpolate,
		  const DCSGateData& gatedata);

    ///Gate header constructor. (deprecated)
    /**
     * Creates a gate header object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param callsign a string specifying the call sign.  
     * @param offset a DCSPoint specifying the gate's offset from the centroid's position.  
     * @param hostid a 64-bit integer specifying the id of the associated beam.
     * @param color an unsigned integer specifying the RGBA color of object in hex (0xFFFFFF00).
     * @param state a byte specifying the object's draw state.  
     * @param gatetype a byte specifying the gate type.  
     * @param gatedata a DCSGateData object providing an initial data set for the object associated with this header.  
     */
    DCSGateHeader(uint64_t id,
		  const char* callsign,
		  const DCSPoint& offset,
		  int64_t hostid,
		  uint32_t color,
		  DCSObjectState_t state,
		  DCSGateType_t gatetype,
		  const DCSGateData& gatedata);

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
