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
#ifndef _DCSBEAMHEADER_H_
#define _DCSBEAMHEADER_H_

#include "DCSBeamData.h"
#include "DCSBaseHeader.h"
#include "DCSPoint.h"

///Beam header class.
/**
 * Class containting information for creating and initializing a
 * beam in the Simdis application.  
 */
class DCSBeamHeader : public DCSBaseHeader
{
  public:
    ///Beam's callsign/name.
    char callsign_[64];		/**< Callsign to be displayed in beam's label.  */
    ///Beam offset.  
    DCSPoint offset_;		/**< Beam's (x,y,z) offset from platform.  */
    ///Azimuth offset.
    double azimoffset_;		/**< Azimuth offset of beam in radians.  */
    ///Elevation offset.
    double elevoffset_;		/**< Elevation offset of beam in radians.  */
    ///Use a missile-type offset
    bool missileoffset_;	/**< Applies an offset to the beam to eminate from edge of icon, useful for missiles */
    ///Horizontal beam width.  
    double hbw_; 		/**< Horizontal width of the beam in radians.  */
    ///Vertical beam width.  
    double vbw_;		/**< Vertical width of the beam in radians.  */
    ///Host platfrom id.  
    int64_t hostid_;		/**< Id of platform with which beam is attached.  */
    ///Target platform id.  
    int64_t targetid_;		/**< Id of platform that beam is targeting (pointed at).  
                                 * A value of -1 indicates no target.  
                                 */
    ///Beam color.
    uint32_t color_;		/**< RGBA color of object specified in hex (0xFFFFFFFF).  
                                 * For the header, if this value is zero, it is ignored.  
                                 */
    ///Draw state.
    DCSObjectState_t state_;	/**< Beam draw state indicator.
    				 * @see @ref state "Object States".
				 */
    ///Beam type.  
    DCSBeamType_t beamtype_;	/**< The object's beam type.
				 * @see @ref beamtypes "Beam Types".
				 */
    ///Position will be interpolated or not (defaults true)
    bool interpolate_;		/**< Interpolate position between data points on playback */
    ///Current data value
    DCSBeamData beamdata_;	/**< An initial data set for the object associated with this header.  */

  public:
    ///Default beam header constructor.  
    /**
     * Creates a beam header object with default data values.
     */
    DCSBeamHeader();

    ///Beam header constructor.
    /**
     * Creates a beam header object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param callsign a string specifying the call sign.  
     * @param offset a DCSPoint specifying the beam's offset from the center of the host platform.  
     * @param azimoffset a double specifying the azimuth offset.  
     * @param elevoffset a double specifying the elevation offset.
     * @param missileOffset Applies an offset to the beam to eminate from edge of icon, useful for missiles
     * @param hbw a double specifying the horizontal beam width.  
     * @param vbw a double specifying the vertical beam width.  
     * @param hostid a 64-bit integer specifying the id of the platform with which the beam is attached.  
     * @param targetid a 64-bit integer specifying the id of the platform that the beam is tracking.  
     * @param color an unsigned integer specifying the RGBA color of object in hex (0xFFFFFF00).
     * @param state a byte specifying the object's draw state.  
     * @param beamtype a byte specifying the beam type.  
     * @param interpolate Indicates whether position should be interpolated between data points
     * @param beamdata a DCSBeamData object providing an initial data set for the object associated with this header.  
     */
    DCSBeamHeader(uint64_t id,
		  const char* callsign,
		  const DCSPoint& offset,
		  double azimoffset,
		  double elevoffset,
		  bool missileOffset,
		  double hbw,
		  double vbw,
		  int64_t hostid,
		  int64_t targetid,
		  uint32_t color,
		  DCSObjectState_t state,
		  DCSBeamType_t beamtype,
		  bool interpolate,
		  const DCSBeamData& beamdata);

    ///Beam header constructor. (deprecated)
    /**
     * Creates a beam header object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param callsign a string specifying the call sign.  
     * @param offset a DCSPoint specifying the beam's offset from the center of the host platform.  
     * @param azimoffset a double specifying the azimuth offset.  
     * @param elevoffset a double specifying the elevation offset.  
     * @param hbw a double specifying the horizontal beam width.  
     * @param vbw a double specifying the vertical beam width.  
     * @param hostid a 64-bit integer specifying the id of the platform with which the beam is attached.  
     * @param targetid a 64-bit integer specifying the id of the platform that the beam is tracking.  
     * @param color an unsigned integer specifying the RGBA color of object in hex (0xFFFFFF00).
     * @param state a byte specifying the object's draw state.  
     * @param beamtype a byte specifying the beam type.  
     * @param beamdata a DCSBeamData object providing an initial data set for the object associated with this header.  
     */
    DCSBeamHeader(uint64_t id,
		  const char* callsign,
		  const DCSPoint& offset,
		  double azimoffset,
		  double elevoffset,
		  double hbw,
		  double vbw,
		  int64_t hostid,
		  int64_t targetid,
		  uint32_t color,
		  DCSObjectState_t state,
		  DCSBeamType_t beamtype,
		  const DCSBeamData& beamdata);

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
    virtual int32_t dataSize(uint16_t version) const {
      if (version == 0x0200)
        return DCSBaseData::dataSize()+sizeof(callsign_)+sizeof(offset_)+
	  sizeof(azimoffset_)+sizeof(elevoffset_)+sizeof(hbw_)+
	  sizeof(vbw_)+sizeof(hostid_)+sizeof(targetid_)+
	  sizeof(color_)+sizeof(int8_t)+sizeof(int8_t)+
	  beamdata_.dataSize();
      return DCSBaseData::dataSize()+sizeof(callsign_)+sizeof(offset_)+
	sizeof(azimoffset_)+sizeof(elevoffset_)+sizeof(int8_t)+sizeof(hbw_)+
	sizeof(vbw_)+sizeof(hostid_)+sizeof(targetid_)+
	sizeof(color_)+sizeof(int8_t)+sizeof(int8_t)+sizeof(int8_t)+
	beamdata_.dataSize();
    }

    ///Determine object's physical size.
    /**
     * @see DCSBaseData::sizeOf()
     */
    virtual int32_t sizeOf() const {return sizeof(*this);}
};

#endif
