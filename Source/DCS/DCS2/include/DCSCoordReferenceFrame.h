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
 * 2006 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 *
 *
 */
#ifndef DCSCOORDREFERENCEFRAME_H
#define DCSCOORDREFERENCEFRAME_H

#include "DCSdefs.h"
#include "inttypesc.h"

///Contains data relating to a coordinate reference frame
/**
 * Contains data relating to a single coordinate reference frame,
 * including coordinate system, origin, offsets, and ECI relative
 * time reference.
 */
class DCSCoordReferenceFrame
{
  public:
    ///Object's local Coordinate System.  
    DCSCoordSystem_t coordsystem_;	/**< The coordinate system used when specifiying the object's position.
					 * @see @ref coord "Coordinate Systems".  
					 */

    ///Latitude of topograhic origin.  
    double latorigin_;		/**< Latitiutde of object's topographic origin, specified in radians.  */
    ///Longitude of topograhic origin.  
    double lonorigin_;		/**< Longitude of object's topographic origin, specified in radians.  */
    ///Altitude of topograhic origin.  
    double altorigin_;		/**< Altitude of object's topographic origin, specified in meters above ellipsoid (HAE).  */

    ///Vertical datum of the platform data; only applies to data, does not apply to altorigin_
    DCSVerticalDatum_t verticalDatum_;
    ///Vertical datum offset, if verticalDatum_ set to DCSVERTDATUM_USER, in meters
    double verticalOffset_;

    ///Magnetic variance to apply to orientation data
    DCSMagneticVariance_t magneticVariance_;
    ///Magnetic variance offset, if magneticVariance_ set to DCSMAGVAR_USER, in radians
    double magneticOffset_;

    ///Reference time for ECI coordinate systems, relative to beginning of scenario reference year
    double referenceTimeECI_;

    ///Tangent plane offset for Generic Tangent Plane (GTP) coordinate systems
    double tangentOffsetX_;
    ///Tangent plane offset for Generic Tangent Plane (GTP) coordinate systems
    double tangentOffsetY_;
    ///Tangent plane rotation angle in radians for Generic Tangent Plane (GTP) coordinate systems
    double tangentAngle_;

  public:
    ///Default coordinate reference frame constructor.  
    /**
     * Creates a coordinate reference frame object with default data values.
     */
    DCSCoordReferenceFrame();

    ///Coordinate reference frame constructor.  
    /**
     * Creates a coordinate reference frame object with the specified data values.
     * @param coordsystem a byte specifying the coordinate system.
     * @param latorigin a double specifying the latitude topo origin.
     * @param lonorigin a double specifying the longitude topo origin.
     * @param altorigin a double specifying the altitude topo origin.
     * @param vertDatum Vertical datum of the platform data
     * @param vertOffset Vertical datum offset, if vertDatum set to DCSVERTDATUM_USER
     * @param magVar Magnetic variance to apply to orientation data
     * @param magOffset Magnetic variance offset, if magVar set to DCSMAGVAR_USER
     * @param eciReference ECI Reference time in seconds since beginning of scenario reference year
     * @param tangentX Tangent plane offset for Generic Tangent Plane (GTP) coordinate systems
     * @param tangentY Tangent plane offset for Generic Tangent Plane (GTP) coordinate systems
     * @param tangentAngle Tangent plane rotation angle in radians for Generic Tangent Plane (GTP) coordinate systems
     */
    DCSCoordReferenceFrame(DCSCoordSystem_t coordsystem,
			   double latorigin,double lonorigin,double altorigin,
			   DCSVerticalDatum_t vertDatum, double vertOffset,
			   DCSMagneticVariance_t magVar, double magOffset,
			   double eciReference,
			   double tangentX, double tangentY, double tangentAngle);

    ///Set contents of data object to default values.  
    /**
     * Clears contents of data object, setting all fields to default values.  
     */
    void clear();

    ///Prepare data for network transmission.
    /**
     * @see DCSBaseData::pack()
     */
    void* pack(void* buffer) const;

    ///Extract data from network transmission.
    /**
     * @see DCSBaseData::unpack()
     */
    void unpack(const void* buffer);

    ///Determine object's data size.
    /**
     * @see DCSBaseData::dataSize()
     */
    int32_t dataSize() const {return dataSize (DCS_VERSION);}

    ///Determine object's data size.
    /**
     * @see DCSBaseData::dataSize()
     */
    int32_t dataSize(uint16_t version) const {
      if (version == 0x0200) // origin and coordinate system flag only for 0x0200
	return sizeof(latorigin_)+sizeof(lonorigin_)+sizeof(int8_t);
      return sizeof(int8_t)+sizeof(latorigin_)+sizeof(lonorigin_)+sizeof(altorigin_)+
	sizeof(int8_t)+sizeof(verticalOffset_)+
	sizeof(int8_t)+sizeof(magneticOffset_)+
	sizeof(referenceTimeECI_)+
	sizeof(tangentOffsetX_)+sizeof(tangentOffsetY_)+sizeof(tangentAngle_);
    }

    ///Determine object's physical size.
    /**
     * @see DCSBaseData::sizeOf()
     */
    int32_t sizeOf() const {return sizeof(*this);}
};

#endif /* DCSCOORDREFERENCEFRAME_H */
