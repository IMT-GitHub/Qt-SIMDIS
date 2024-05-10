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

#ifndef _DCSDEFS_H_
#define _DCSDEFS_H_

/** @file DCSdefs.h
 * @brief DCS Constants.
 *
 * Constant values representing different settings specific to
 * certain DCS data types.
 */

///DCS version.
/**
 * The version number for this DCS release.  The high order byte represents
 * the major version number and the low order byte represents the minor version
 * number.
 */
static const int DCS_VERSION = 0x0204;


///Maximum packet size for UDP data.
/**
 * The maximum amount of data that will be sent by DCS in a
 * single data packet.  The value is the ethernet MTU(1500 bytes) minus
 * the IP header(20 bytes) minus the udp header(8 bytes).  However,
 * a message can be larger than the MTU, up to 16KB on a Microsoft system.
 */
static const int MAX_DATAGRAM_SIZE = 16384;

///DCS server ID size.
/**
 * Number of bytes used for the unique server instance ID.  A unique ID is
 * generated by each server at the time the server is started, to be used
 * for separating server data from other data that may be present in the same
 * data stream.  This id is currently created from a combination of the server
 * host address, server data port, and server start time.  
 */
static const int DCS_SERVERIDSZ = 10;

///DCS address string length.
/**
 * Number of bytes used to represent an IP address string.  There are four 3 digit numbers, 
 * three periods, and one null terminator reulting in a string length of 16.  
 */
static const int DCS_ADDRSTRLEN = 16;

/** @name Data Types
 * DCS data types.
 */
//@{
///DCS data types.  
/**
 * A list of data types handled by DCS.  Used to identify the contents of
 * a DCS data packet.
 * @anchor type
 */
enum DCSDataType_t
{
  DCSUNKNOWN=0,		/**< Unknown data type.  The contents of this packet are undefined. */
  DCSPLATFORMHEADER=1,	/**< Platform header.  This packet contains data pertaining to a SIMDIS platform header structure. */
  DCSPLATFORMDATA=2,	/**< Platform data.  This packet contains data pertaining to a SIMDIS platform data structure. */
  DCSBEAMHEADER=3,	/**< Beam header.  This packet contains data pertaining to a SIMDIS beam header structure. */
  DCSBEAMDATA=4,	/**< Beam data.  This packet contains data pertaining to a SIMDIS beam data structure. */
  DCSGATEHEADER=5,	/**< Gate header.  This packet contains data pertaining to a SIMDIS gate header structure. */
  DCSGATEDATA=6,	/**< Gate data.  This packet contains data pertaining to a SIMDIS gate data structure. */
  DCSTIMEHEADER=7,	/**< Time header.  This packet contains data pertaining to a SIMDIS time header structure. */
  DCSTIMEDATA=8,	/**< Time data.  This packet contains data pertaining to a SIMDIS time data structure. */
  DCSSCENARIOHEADER=9,	/**< Scenario header.  This packet contains data pertaining to a SIMDIS scenario header structure. */
  DCSSCENARIODATA=10,	/**< Scenario data.  This packet contains data pertaining to a SIMDIS scenario data structure. */
  DCSEVENT=11,		/**< Event.  This packet contains data pertaining to a SIMDIS event structure. */
  DCSGENERICDATA=12,	/**< Generic data.  This packet contains data pertaining to a SIMDIS generic data structure. */
  DCSSCOPEDATA=13,	/**< Scope data.  This packet contains data pertaining to a Plot-XY Scope display. */
  DCSCATEGORYDATA=14	/**< Category data.  This packet contains data pertaining to a SIMDIS category data entry. */
};
//@}

/** @name Beam Types
 * Supported beam types.
 */
//@{
///Supported beam types.
/**
 * Identifiers for beam types recognized by DCS and SIMDIS.
 * @anchor beamtypes
 */
enum DCSBeamType_t
{
  DCSBEAM_UNKNOWN=0,		/**< Unknown beam type. */
  DCSBEAM_STEADY=1,		/**< Steady beam. */
  DCSBEAM_CONICALSPIRAL=2,	/**< Conical spiral beam. (not supported) */
  DCSBEAM_HORIZBIDIR=3,		/**< Horizontal bi-directional beam. (not supported) */
  DCSBEAM_VERTBIDIR=4,		/**< Vertical bi-directional beam. (not supported) */
  DCSBEAM_HORIZUNIDIR=5,	/**< Horizontal uni-directional beam. (not supported) */
  DCSBEAM_VERTUNIDIR=6,		/**< Vertical uni-directional beam. (not supported) */
  DCSBEAM_HORIZRASTER=7,	/**< Horizontal raster beam. (not supported) */
  DCSBEAM_VERTRASTER=8,		/**< Vertical raster beam. (not supported) */
  DCSBEAM_LINEAR=9,		/**< Linear beam. */
  DCSBEAM_BODYAXIS=10		/**< Platform body axis beam. */
};
//@}

/** @name Gate Types
 * Supported gate types.
 */
//@{
///Supported gate types.
/**
 * Identifiers for gate types recognized by DCS and SIMDIS.
 * @anchor gatetypes
 */
enum DCSGateType_t
{
  DCSGATE_UNKNOWN=0,		/**< Unknown gate type. */
  DCSGATE_RANGE=1,		/**< Range gate. */
  DCSGATE_GUARD=2,		/**< Guard gate. */
  DCSGATE_TARGET=3,		/**< Target gate. */
  DCSGATE_ANGLE=4,		/**< Angle gate. */
  DCSGATE_RAIN=5,		/**< Rain gate. */
  DCSGATE_CLUTTER=6,		/**< Clutter gate. */
  DCSGATE_FOOTPRINT=7,		/**< Footprint gate. */
  DCSGATE_SECTOR=8,		/**< Sector gate. */
  DCSGATE_PUSH=9,		/**< Push gate. */
  DCSGATE_COVERAGE=10,		/**< Coverage (volume) gate. */
  DCSGATE_BODY=11		/**< Body relative gate. */
};
//@}

/** @name Platform Types
 * Supported platform types.
 */
//@{
///Supported platform types.
/**
 * Identifiers for platform types recognized by DCS and SIMDIS.
 * @anchor plattypes
 */
enum DCSPlatformType_t
{
  DCSPLATFORM_UNKNOWN=0,		/**< Unknown platform type. */
  DCSPLATFORM_SURFACESHIP=1,		/**< Surface ship. */
  DCSPLATFORM_SUBMARINE=2,		/**< Submarine. */
  DCSPLATFORM_AIRCRAFT=3,		/**< Aircraft. */
  DCSPLATFORM_SATELLITE=4,		/**< Satellite. */
  DCSPLATFORM_HELICOPTER=5,		/**< Helicopter. */
  DCSPLATFORM_MISSILE=6,		/**< Missile. */
  DCSPLATFORM_DECOY=7,			/**< Decoy. */
  DCSPLATFORM_BUOY=8,			/**< Buoy. */
  DCSPLATFORM_REFERENCE=9,		/**< Reference. */
  DCSPLATFORM_MOBILELANDSITE=10,	/**< Mobile land site. */
  DCSPLATFORM_LANDSITE=11,		/**< Land site. */
  DCSPLATFORM_TORPEDO=12,		/**< Torpedo. */
  DCSPLATFORM_CONTACT=13,		/**< Contact. */
  DCSPLATFORM_CATEGORY=14		/**< Platform type set via category data value. */
};
//@}

/** @name Allegience Types
 * Supported allegience types.
 */
//@{
///Supported platform types.
/**
 * Identifiers for platform allegience types recognized by DCS and SIMDIS.
 * @anchor allegience
 */
enum DCSAllegience_t
{
  DCSALLEGIENCE_FRIENDLY=0,		/**< Platform is friendly. */
  DCSALLEGIENCE_HOSTILE=1,		/**< Platform is hostile. */
  DCSALLEGIENCE_NEUTRAL=2,		/**< Platform is neutral. */
  DCSALLEGIENCE_CATEGORY=3		/**< Allegience set via category data value. */
};
//@}

/** @name Coordinate Systems
 * Supported coordinate system types.
 */
//@{
///Supported coordinate system types.  
/**
 * Identifiers for coordinate systems recognized by DCS and SIMDIS.  The
 * position informationed contained in a DCS data packet are expected to be represented by the
 * specified coordinate system type.
 * @anchor coord
 */
enum DCSCoordSystem_t
{
  DCSCOORD_WGS84=0,	/**< Earth Centered, Earth Fixed Geocentric coordinate system: based on WGS-84. */
  DCSCOORD_LLA=1,	/**< Geodetic coordinate system with position information specified as x=lat,y=lon,z=alt. */
  DCSCOORD_NED=2,	/**< Topographic coordinate system with position information specified as x=north,y=east,z=down. */
  DCSCOORD_NWU=3,	/**< Topographic coordinate system with position information specified as x=north,y=west,z=up. */
  DCSCOORD_ENU=4,	/**< Topographic coordinate system with position information specified as x=east,y=north,z=up. */
  DCSCOORD_XEAST=5,	/**< Cartesian tangent plane system with position information specified as x=east,y=north,z=up. */
  DCSCOORD_GTP=6,       /**< Flat Earth: Generic Tangent Plane, User defined X-Y rotation and tangential offset. */
  DCSCOORD_ECI=7        /**< Earth Centered Inertial Geocentric coordinate system: based on WGS-84. */
};
//@}

/** @name Event Types
 * Supported event types.
 */
//@{
///Supported event types.
/**
 * Identifiers for event types recognized by DCS and SIMDIS.
 * @anchor eventtype
 */
enum DCSEventType_t
{
  DCSEVENT_UNKNOWN=0,		/**< Event type is unknown. */
  DCSEVENT_TARGETID=1,		/**< Event to set target id. */
  DCSEVENT_COLOR=2,		/**< Event to set object color. */
  DCSEVENT_STATE=3,		/**< Event to set object state.
				 * @see @ref state "Object States".
				 */
  DCSEVENT_STATUS=4,		/**< Event to set track status.
				 * @see @ref status "Track Status".
				 */
  DCSEVENT_CLASSIFICATION=5,	/**< Event to set classification level for scenario. */
  DCSEVENT_BEAMINTERPPOS=6,	/**< Event to set beam interpolate position flag */
  DCSEVENT_GATEINTERPPOS=7,	/**< Event to set gate interpolate position flag */
  DCSEVENT_PLATFORMINTERPOLATE=8,	/**< Event to set platform interpolate orientation flag */
  DCSEVENT_CLASSIFICATIONSTR=9,	/**< Event to set classification string for scenario. */
  DCSEVENT_CALLSIGN=10,		/**< Event to set the callsign for platform, beam or gate. */
  DCSEVENT_ICONNAME=11		/**< Event to set the iconname for a platform. */
};
//@}

/** @name Object States
 * Object state values.
 */
//@{
///Object state values.  
/**
 * Values that represent the state of a given object.
 * @anchor state
 */
enum DCSObjectState_t
{
  DCSSTATE_SAME=0,	/**< State has not changed. */
  DCSSTATE_ON=1,	/**< Toggle object state to on. */
  DCSSTATE_OFF=2,	/**< Toggle object state to off (end time is set). */
  DCSSTATE_EXPIRE=3	/**< Expired object, remove from both server & client. */
};
//@}

/** @name Track Status
 * Track status values.
 */
//@{
///Track status values.  
/**
 * Represent the current status of an object being tracked.  Indicate
 * whether a track is still valid or has been lost.
 * @anchor status
 */
enum DCSTrackStatus_t
{
  DCSSTATUS_DROPPED=0,	/**< Track has been dropped (no good). */
  DCSSTATUS_LIVE=1	/**< Track is live (good). */
};
//@}

/** @name Time Sources
 * Types of Time Sources
 */
//@{
///Types of time sources.  
/**
 * Specify the type time source for SIMDIS.
 * @anchor time
 */
enum DCSTimeType_t
{
  DCSTIME_SYSTEM=0,	/**< Real-time, client uses own system clock to set time. */
  DCSTIME_PACKET=1,	/**< Nonreal-time, client updates only when a time packet is received. */
  DCSTIME_REALTIME=2	/**< Real-time, client start time is synchronized to first time packet. */
};
//@}

/** @name Classification Types
 * Data classification types.
 */
//@{
///Data classification types.  
/**
 * Indicates classification level of data being sent by DCS.
 * @anchor class
 */
enum DCSClassification_t
{
  DCSCLASS_UNCLASSIFIED=0,	/**< Data is unclassified. */
  DCSCLASS_OFFICIALUSEONLY=1,	/**< Data is for official use only. */
  DCSCLASS_CONFIDENTIAL=2,	/**< Data is confidential. */
  DCSCLASS_SECRET=3,		/**< Data is secret. */
  DCSCLASS_SECRETWN=4,		/**< Data is secret, warning notice-intelligence sources or methods involved. */
  DCSCLASS_SECRETNF=5,		/**< Data is secret, not releasable to foreign nationals. */
  DCSCLASS_SECRETNC=6,		/**< Data is secret, not releasable to contractors/consultants. */
  DCSCLASS_SECRETOC=7,		/**< Data is secret, dissemination and extraction of information controlled by originator. */
  DCSCLASS_SECRETPR=8,		/**< Data is secret, caution-proprietary information involved. */
  DCSCLASS_TOPSECRET=9		/**< Data is top secret. */
};
//@}

/** @name UDP Data Modes
 * UDP data transmission modes.  
 */
//@{
///UDP data transmission modes.  
/**
 * Indicates the type of UDP socket used to transmit data from DCS to SIMDIS.
 * @anchor modes
 */
enum DCSUDPMode_t
{
  DCSDATA_UNCONNECTED=0,	/**< No UDP socket, socket is invalid. */
  DCSDATA_UNICAST=1,		/**< Standard Unicast socket. */
  DCSDATA_BROADCAST=2,		/**< Broadcast socket. */
  DCSDATA_MULTICAST=3		/**< Multicast socket. */
};
//@}

/** @name Client Requests
 * DCS client data requests
 */
//@{
///DCS client data requests
/**
 * Valid data requests that a client may make from the server.
 * @anchor requests
 */
enum DCSRequestType_t
{
  DCSREQUEST_CONNECT=0,         /**< Client requests negotiation of a new connection. */
  DCSREQUEST_VERSION=1,         /**< Client requests DCS API version. */
  DCSREQUEST_DATAMODE=2,	/**< Client requests the type of UDP socket used for data transmission.
				 * @see @ref modes "Data Modes".
				 */
  DCSREQUEST_TIME=3,		/**< Client requests time data in the form of DCSTimeHeader. */
  DCSREQUEST_SCENARIO=4,	/**< Client requests scenario data in the form of DCSScenarioHeader. */
  DCSREQUEST_EVENT=5,		/**< Server sends new event to all connected clients.  Not to be used by client for making requests. */
  DCSREQUEST_HEADER=6,		/**< Server sends new header to all connected clients.  Client requests a header for a specific id.  Also used for sending DCSGenericData and DCSCategoryData over TCP */
  DCSREQUEST_HEADERS=7,		/**< Client requests headers for all known objects.  Header objects are DCSPlatformHeader, DCSBeamHeader, DCSGateHeader. */
  DCSREQUEST_DISCONNECT=8,	/**< Client requests to disconnect from server or is ordered to disconnect by server. */
  DCSREQUEST_KEEPALIVE=9	/**< Client/server check for active connection. */
};
//@}

/** @name Database Generation
 * DCS header container database file generation types.  
 */
//@{
///DCS header container database file generation types.  
/**
 * Means by which DCS header container database file was generated.
 * @anchor database
 */
enum DCSContainerState_t
{
  DCSFILE_CREATED=0,            /**< DCS header container created a new data file. */
  DCSFILE_RESTORED=1            /**< DCS header container restored data from a file. */
};
//@}


///Enumeration of Vertical datum type constants
enum DCSVerticalDatum_t
{
  DCSVERTDATUM_WGS84=1,  /**< WGS-84 Ellipsoidal Vertical Datum */
  DCSVERTDATUM_MSL,      /**< Mean Sea Level based on EGM-96 Datum */
  DCSVERTDATUM_NAVD88,   /**< North American Vertical Datum of 1988 (not supported) */
  DCSVERTDATUM_NGVD29,   /**< National Geodetic Vertical Datum of 1929 (Sea Level Datum) (not supported) */
  DCSVERTDATUM_NAD83,    /**< North American Datum of 1983 (not supported) */
  DCSVERTDATUM_MHHW,     /**< Mean Higher High Water Tidal Datum (not supported) */
  DCSVERTDATUM_MHW,      /**< Mean High Water Tidal Datum (not supported) */
  DCSVERTDATUM_DTL,      /**< Diurnal Tide Level Diurnal (not supported) */
  DCSVERTDATUM_MTL,      /**< Mean Tide Level Datum (not supported) */
  DCSVERTDATUM_LMSL,     /**< Local Mean Sea Level Datum (not supported) */
  DCSVERTDATUM_MLW,      /**< Mean Low Water Tidal Datum (not supported) */
  DCSVERTDATUM_MLLW,     /**< Mean Lower Low Water Tidal Datum (not supported)*/
  DCSVERTDATUM_USER      /**< User Defined Vertical Datum Offset */
};

///Enumeration of magnetic variance datum type constants
enum DCSMagneticVariance_t
{
  DCSMAGVAR_TRUE=1,  /**< No variance, True Heading */
  DCSMAGVAR_WMM,     /**< World Magnetic Model (WMM) */
  DCSMAGVAR_USER     /**< User Defined Magnetic Variance Offset */
};

#endif
