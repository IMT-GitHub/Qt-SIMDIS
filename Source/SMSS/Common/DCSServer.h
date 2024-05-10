
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
#ifndef DCSSERVER_H
#define DCSSERVER_H

#include <vector>
#include <map>
#include <string>
#include <stdio.h>

#include "DCS.h"
#include "Constants/ConstantsUnits.h"

//***************************************************************************

  /// A top level class for implementing a DCS data server.
/**
 * The DCSServer class facilitates the creation, setup and sending of data via
 * the NRL developed DCS protocol
 */
class DCSServer
{
  protected:

    char AppName_[BUFSIZ];             /**< your application's name */
    char DatabaseFile_[BUFSIZ];        /**< filename of your DCS database */
    int8_t Protocol_;                  /**< data delivery protocol */
    int32_t ServerPort_;               /**< server port number */
    int32_t DataPort_;                 /**< data delivery port number */
    int32_t TTL_;                      /**< time to live (multicast only) */
    char DataAddr_[DCS_ADDRSTRLEN];    /**< data delivery address */
    char IfaceAddr_[DCS_ADDRSTRLEN];   /**< network interface address */
    double ExpireTimeLimit_;           /**< platform expiration limit */
    double LastExpireTime_;            /**< last time platform expiration check was performed */
    bool ServerStarted;                /**< flag to denote if server has been started */

    std::map<int64_t, double> PlatMap_;/**< map of platforms keyed on DCS ID, used for checking expired platforms */
    std::map<int64_t, std::vector<int64_t> > BeamMap_;/**< map of beams keyed on DCS Platform ID, */
    std::map<int64_t, std::vector<int64_t> > GateMap_;/**< map of gates keyed on DCS Beam ID */
    std::vector<int64_t> ScopeList_;	/**< list of Scope Data keyed on DCS ID */

    DCSDisc::DCSDiscover Discover_;    /**< provides functionality for discovering new clients looking for a server */
    DCSCom::DCSServerInfo DCS_;        /**< stores information relevant to the actual DCS server */
    DCSHeaderContainer DCSHeaders_;    /**< provides functionality for inserting, updating, retrieving, and removing header objects */
    DCSScenarioHeader ScenarioHeader_; /**< initialization parameters for scenario.  Specifies classification, reference coordinate, etc. */
    DCSTimeHeader TimeHeader_;         /**< initialization parameters for time.  Specifies time source (e.g. DCS server or client workstation's system clock) */

  public:

    //DCSHeaderContainer DCSHeaders_;    // Just for testing! Move Back to protected!!!!!
    /// DCSServer constructor.
    /**
     * Initializes the data transport socket, server port number, data port number, data server IP address and time to live
     * @param p_appName a string representing the name of the data server.
     * @param proto a char representing the type of UDP socket: broadcast, mulitcast, or unicast, to use.  See DCSdefs.h for more information.
     * @param srvPort an integer representing the client's connection port number to the server.
     * @param dataPort an integer representing the port number DCS data is sent.
     * @param p_dataAddr a string representing the IP address DCS data is sent.
     * @param p_iface a string representing the network address of the network interface to use for data transmission.  To use any network interface (INADDR_ANY), set to NULL.
     * @param ttl an integer specifying the new time to live value to be used (Multicast only).
     */
    DCSServer(const char* p_appName=0,
	      int8_t proto=DCSDATA_MULTICAST,
	      int32_t srvPort=5555,
	      int32_t dataPort=5707,
	      const char* p_dataAddr=0,
	      const char* p_iface=0,
	      int32_t ttl=4);

    /// DCSServer destructor.
    /**
     * Destroys all created objects and frees resources allocated for the discovery daemon.
     * Also removes the generated DCS database file.
     */
    ~DCSServer();

    /// DCSServer start.
    /**
     * Responsible for starting the DCS server based on values set in the constructor.
     * Also handles the creation and restoration of the DCS database.
     */
    void start();

    /// DCSServer removeHeader.
    /**
     * Responsible for removing a DCS object from the DCS database, preventing it from being sent.
     * Also handles the creation and restoration of the DCS database.
     * @param id an unsigned 64-bit integer specifying the id of an object to remove.
     * @param currentTime a double specifying the current data time, units are seconds.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool removeHeader(uint64_t id, double currentTime);

    /// DCSServer idle.
    /**
     * Responsible for both servicing new client connection requests and removing expired objects.
     * @param currentTime a double specifying the current data time.
     */
    void idle(double currentTime);

    /// DCSServer setExpireTimeLimit.
    /**
     * Sets expiration time limit (secs) for all objects, value must be > 0, otherwise default to 300 seconds.
     * Also handles the creation and restoration of the DCS database.
     * @param expireLimit a double specifying the expiration time limit, units are seconds.
     */
    void setExpireTimeLimit(double expireLimit) {ExpireTimeLimit_=(expireLimit>0)?expireLimit:300.;}

    /// DCSServer sendScenarioHeader.
    /**
     * Sends scenario initialization information to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param coordsystem a char specifying the scenario data's coordinate system.  See DCSdefs.h for more information.
     * @param classification a string specifying the scenario data's classification.
     * @param classColor 0xAABBGGRR color value for the classification
     * @param referenceyear an integer specifying the scenario data's current year, i.e. 2003.
     * @param latorigin a double specifying the scenario data's reference latitude, units are radians.
     * @param lonorigin a double specifying the scenario data's reference longitude, units are radians.
     * @param altorigin a double specifying the scenario data's reference altitude, units are meters HAE.
     * @param time a double specifying the current data time, units are seconds.
     * @param timemod a double specifying the percentage of real-time in which the current time is being specified (1. = 1x, 2. = 2x, 0.5 = .5x, etc).
     * @param timesource a char specifying the type of time source to use.  See DCSdefs.h for more information.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendScenarioHeader(DCSCoordSystem_t coordsystem,
			    const std::string &classification,
			    uint32_t classColor,
			    int32_t referenceyear=DCS::getyear(),
			    double latorigin=0.,
			    double lonorigin=0.,
			    double altorigin=0.,
			    double time=0.,
			    double timemod=1.0,
			    int8_t timesource=DCSTIME_PACKET);

    /// DCSServer sendScenarioHeader.
    /**
     * Sends scenario initialization information to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param refFrame Coordinate reference frame specifying scenario origin, default coordinate system, and other settings.
     * @param classification a string specifying the scenario data's classification.  See DCSdefs.h for more information.
     * @param classColor 0xAABBGGRR color value for the classification
     * @param referenceyear an integer specifying the scenario data's current year, i.e. 2003.
     * @param time a double specifying the current data time, units are seconds.
     * @param timemod a double specifying the percentage of real-time in which the current time is being specified (1. = 1x, 2. = 2x, 0.5 = .5x, etc).
     * @param timesource a char specifying the type of time source to use.  See DCSdefs.h for more information.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendScenarioHeader(const DCSCoordReferenceFrame& refFrame,
			    const std::string &classification,
			    uint32_t classColor,
			    int32_t referenceyear=DCS::getyear(),
			    double time=0.,
			    double timemod=1.0,
			    int8_t timesource=DCSTIME_PACKET);

    /// DCSServer sendScenarioHeader.
    /**
     * Sends scenario initialization information to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param coordsystem a char specifying the scenario data's coordinate system.  See DCSdefs.h for more information.
     * @param classification a char specifying the scenario data's classification.  See DCSdefs.h for more information.
     * @param referenceyear an integer specifying the scenario data's current year, i.e. 2003.
     * @param latorigin a double specifying the scenario data's reference latitude, units are radians.
     * @param lonorigin a double specifying the scenario data's reference longitude, units are radians.
     * @param time a double specifying the current data time, units are seconds.
     * @param timemod a double specifying the percentage of real-time in which the current time is being specified (1. = 1x, 2. = 2x, 0.5 = .5x, etc).
     * @param timesource a char specifying the type of time source to use.  See DCSdefs.h for more information.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendScenarioHeader(DCSCoordSystem_t coordsystem,
			    DCSClassification_t classification,
			    int32_t referenceyear=DCS::getyear(),
			    double latorigin=0.,
			    double lonorigin=0.,
			    double time=0.,
			    double timemod=1.0,
			    int8_t timesource=DCSTIME_PACKET);

    /// DCSServer sendPlatformHeader.

    bool sendPlatformHeaer(DCSPlatformHeader header);
    
    /**
     * Sends platform initialization information to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param platid an unsigned 64-bit integer specifying the unique id of the platform.
     * @param callsign a string specifying the platform's callsign/name, size limited to 64.
     * @param iconname a string specifying the platform's 3D icon name, size limited to 64.
     * @param coordsystem a byte specifying the coordinate system.
     * @param latorigin a double specifying the latitude topo origin in radians.
     * @param lonorigin a double specifying the longitude topo origin in radians.
     * @param altorigin a double specifying the altitude topo origin in meters.
     * @param vertDatum Vertical datum of the platform data
     * @param vertOffset Vertical datum offset, if vertDatum set to DCSVERTDATUM_USER
     * @param magVar Magnetic variance to apply to orientation data
     * @param magOffset Magnetic variance offset, if magVar set to DCSMAGVAR_USER
     * @param eciReference ECI Reference time in seconds since beginning of scenario reference year
     * @param tangentX Tangent plane offset for Generic Tangent Plane (GTP) coordinate systems
     * @param tangentY Tangent plane offset for Generic Tangent Plane (GTP) coordinate systems
     * @param tangentAngle Tangent plane rotation angle in radians for Generic Tangent Plane (GTP) coordinate systems
     * @param time a double specifying the current data time, units are seconds.
     * @param x a double specifying the platform's X/Lat location, units are meters, radians for a geodetic (LLA) coordinate system.
     * @param y a double specifying the platform's Y/Lon location, units are meters, radians for a geodetic (LLA) coordinate system.
     * @param z a double specifying the platform's Z location, units are meters.
     * @param c a double specifying the platform's course, units are radians.
     * @param p a double specifying the platform's pitch, units are radians.
     * @param r a double specifying the platform's roll, units are radians.
     * @param vx a double specifying the platform's X velocity component, units are meters/second.
     * @param vy a double specifying the platform's Y velocity component, units are meters/second.
     * @param vz a double specifying the platform's Z velocity component, units are meters/second.
     * @param ax a double specifying the platform's X acceleration component, units are meters/second squared.
     * @param ay a double specifying the platform's Y acceleration component, units are meters/second squared.
     * @param az a double specifying the platform's Z acceleration component, units are meters/second squared.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendPlatformHeader(uint64_t platid,
			    const std::string &callsign,
			    const std::string &iconname,
			    DCSCoordSystem_t coordsystem,
			    double latorigin=0,
			    double lonorigin=0,
			    double altorigin=0,
			    DCSVerticalDatum_t vertDatum=DCSVERTDATUM_WGS84,
			    double vertOffset=0,
			    DCSMagneticVariance_t magVar=DCSMAGVAR_TRUE,
			    double magOffset=0,
			    double eciReference=0,
			    double tangentX=0,
			    double tangentY=0,
			    double tangentAngle=0,
			    double time=0,
			    double x=0,
			    double y=0,
			    double z=0,
			    double c=0,
			    double p=0,
			    double r=0,
			    double vx=0,
			    double vy=0,
			    double vz=0,
			    double ax=0,
			    double ay=0,
			    double az=0);

    /// DCSServer sendPlatformHeader. (deprecated)
    /**
     * Sends platform initialization information to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param platid an unsigned 64-bit integer specifying the unique id of the platform.
     * @param callsign a string specifying the platform's callsign/name, size limited to 64.
     * @param iconname a string specifying the platform's 3D icon name, size limited to 64.
     * @param plattype a char specifying the platform's type.  See DCSdefs.h for more information.
     * @param allegience a char specifying the platform's allegience (friendly, hostile, or neutral).  See DCSdefs.h for more information.
     * @param time a double specifying the current data time, units are seconds.
     * @param x a double specifying the platform's X/Lat location, units are meters, radians for a geodetic (LLA) coordinate system.
     * @param y a double specifying the platform's Y/Lon location, units are meters, radians for a geodetic (LLA) coordinate system.
     * @param z a double specifying the platform's Z location, units are meters.
     * @param c a double specifying the platform's course, units are radians.
     * @param p a double specifying the platform's pitch, units are radians.
     * @param r a double specifying the platform's roll, units are radians.
     * @param vx a double specifying the platform's X velocity component, units are meters/second.
     * @param vy a double specifying the platform's Y velocity component, units are meters/second.
     * @param vz a double specifying the platform's Z velocity component, units are meters/second.
     * @param ax a double specifying the platform's X acceleration component, units are meters/second squared.
     * @param ay a double specifying the platform's Y acceleration component, units are meters/second squared.
     * @param az a double specifying the platform's Z acceleration component, units are meters/second squared.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendPlatformHeader(uint64_t platid,
			    const std::string &callsign,
			    const std::string &iconname,
			    DCSPlatformType_t plattype=DCSPLATFORM_UNKNOWN,
			    DCSAllegience_t allegience=DCSALLEGIENCE_FRIENDLY,
			    double time=0,
			    double x=0,
			    double y=0,
			    double z=0,
			    double c=0,
			    double p=0,
			    double r=0,
			    double vx=0,
			    double vy=0,
			    double vz=0,
			    double ax=0,
			    double ay=0,
			    double az=0);

    /// DCSServer sendBeamHeader.
    /**
     * Sends beam initialization information to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param beamid an unsigned 64-bit integer specifying the unique id of the beam.
     * @param platid an unsigned 64-bit integer specifying the unique id of the host platform.
     * @param callsign a string specifying the beam's description/name, size limited to 64.
     * @param hbw a double specifying the horizontal main beam width, units are radians.
     * @param vbw a double specifying the vertical main beam width, units are radians.
     * @param targetId a 64-bit integer specifying the unique id of the target (-1 means no target).
     * @param color an unsigned 32-bit integer specifying the beam color in RRGGBBAA hexadecimal(00-FF) notation: RR=red, GG=green, BB=blue, AA=alpha component.
     * @param azimoffset a double specifying the azimuth pointing direction offset, units are radians.
     * @param elevoffset a double specifying the elevation pointing direction offset, units are radians.
     * @param xoffset a double specifying the X body system offset (X out the front), units are meters.
     * @param yoffset a double specifying the Y body system offset (Y out the left), units are meters.
     * @param zoffset a double specifying the Z body system offset (Z out the top), units are meters.
     * @param beamtype a char specifying the beam's type.  See DCSdefs.h for more information.
     * @param time a double specifying the current data time, units are seconds.
     * @param azim a double specifying the azimuth pointing direction, units are radians.
     * @param elev a double specifying the elevation pointing direction, units are radians.
     * @param length a double specifying the beam's length(range), units are meters.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendBeamHeader(uint64_t beamid,
			uint64_t platid,
			const std::string &callsign,
			double hbw,
			double vbw,
			int64_t targetId=-1,
			uint32_t color=0xff800080,
			double azimoffset=0.,
			double elevoffset=0.,
			double xoffset=0.,
			double yoffset=0.,
			double zoffset=0.,
			DCSBeamType_t beamtype=DCSBEAM_LINEAR,
			double time=0,
			double azim=0,
			double elev=0,
			double length=0);

    /// DCSServer sendGateHeader.
    /**
     * Sends gate initialization information to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param gateid an unsigned 64-bit integer specifying the unique id of the gate.
     * @param beamid an unsigned 64-bit integer specifying the unique id of the host beam.
     * @param callsign a string specifying the gate's description/name, size limited to 64.
     * @param color an unsigned 32-bit integer specifying the gate color in RRGGBBAA hexadecimal(00-FF) notation: RR=red, GG=green, BB=blue, AA=alpha component.
     * @param azimoffset a double specifying the azimuth pointing direction offset, units are radians.
     * @param elevoffset a double specifying the elevation pointing direction offset, units are radians.
     * @param xoffset a double specifying the X body system offset (X out the front), units are meters.
     * @param yoffset a double specifying the Y body system offset (Y out the left), units are meters.
     * @param zoffset a double specifying the Z body system offset (Z out the top), units are meters.
     * @param gatetype a char specifying the gate's type.  See DCSdefs.h for more information.
     * @param time a double specifying the current data time, units are seconds.
     * @param azim a double specifying the azimuth pointing direction, units are radians.
     * @param elev a double specifying the elevation pointing direction, units are radians.
     * @param width a double specifying the gate's angular width, units are radians.
     * @param height a double specifying the gate's angular height, units are radians.
     * @param minrange a double specifying the gate's minimun downrange extent, units are meters.
     * @param centroid a double specifying the gate's centroid range, units are meters.
     * @param maxrange a double specifying the gate's maximum downrange extent, units are meters.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendGateHeader(uint64_t gateid,
			uint64_t beamid,
			const std::string &callsign,
			uint32_t color=0xff000000,
			double azimoffset=0.,
			double elevoffset=0.,
			double xoffset=0.,
			double yoffset=0.,
			double zoffset=0.,
			DCSGateType_t gatetype=DCSGATE_RANGE,
			double time=0,
			double azim=0,
			double elev=0,
			double width=0,
			double height=0,
			double minrange=0,
			double centroid=0,
			double maxrange=0);

    /// DCSServer sendGateHeader. (deprecated)
    /**
     * Sends gate initialization information to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param gateid an unsigned 64-bit integer specifying the unique id of the gate.
     * @param beamid an unsigned 64-bit integer specifying the unique id of the host beam.
     * @param callsign a string specifying the gate's description/name, size limited to 64.
     * @param color an unsigned 32-bit integer specifying the gate color in RRGGBBAA hexadecimal(00-FF) notation: RR=red, GG=green, BB=blue, AA=alpha component.
     * @param xoffset a double specifying the X body system offset (X out the front), units are meters.
     * @param yoffset a double specifying the Y body system offset (Y out the left), units are meters.
     * @param zoffset a double specifying the Z body system offset (Z out the top), units are meters.
     * @param gatetype a char specifying the gate's type.  See DCSdefs.h for more information.
     * @param time a double specifying the current data time, units are seconds.
     * @param azim a double specifying the azimuth pointing direction, units are radians.
     * @param elev a double specifying the elevation pointing direction, units are radians.
     * @param width a double specifying the gate's angular width, units are radians.
     * @param height a double specifying the gate's angular height, units are radians.
     * @param minrange a double specifying the gate's minimun downrange extent, units are meters.
     * @param centroid a double specifying the gate's centroid range, units are meters.
     * @param maxrange a double specifying the gate's maximum downrange extent, units are meters.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendGateHeader(uint64_t gateid,
			uint64_t beamid,
			const std::string &callsign,
			uint32_t color=0xff000000,
			double xoffset=0.,
			double yoffset=0.,
			double zoffset=0.,
			DCSGateType_t gatetype=DCSGATE_RANGE,
			double time=0,
			double azim=0,
			double elev=0,
			double width=0,
			double height=0,
			double minrange=0,
			double centroid=0,
			double maxrange=0);

    /// DCSServer sendPlatformData.
    /**
     * Updates platform position information to the server and subsequently to all connected clients.
     * Data is transmitted via the UDP channel.
     * @param platid an unsigned 64-bit integer specifying the unique id of the platform.
     * @param time a double specifying the current data time, units are seconds.
     * @param x a double specifying the platform's X/Lat location, units are meters, radians for a geodetic (LLA) coordinate system.
     * @param y a double specifying the platform's Y/Lon location, units are meters, radians for a geodetic (LLA) coordinate system.
     * @param z a double specifying the platform's Z location, units are meters.
     * @param c a double specifying the platform's course, units are radians.
     * @param p a double specifying the platform's pitch, units are radians.
     * @param r a double specifying the platform's roll, units are radians.
     * @param vx a double specifying the platform's X velocity component, units are meters/second.
     * @param vy a double specifying the platform's Y velocity component, units are meters/second.
     * @param vz a double specifying the platform's Z velocity component, units are meters/second.
     * @param ax a double specifying the platform's X acceleration component, units are meters/second squared.
     * @param ay a double specifying the platform's Y acceleration component, units are meters/second squared.
     * @param az a double specifying the platform's Z acceleration component, units are meters/second squared.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendPlatformData(uint64_t platid,
			  double time,
			  double x,
			  double y,
			  double z,
			  double c=0,
			  double p=0,
			  double r=0,
			  double vx=0,
			  double vy=0,
			  double vz=0,
			  double ax=0,
			  double ay=0,
			  double az=0);

    /// DCSServer sendBeamData.
    /**
     * Updates beam position information to the server and subsequently to all connected clients.
     * Data is transmitted via the UDP channel.
     * @param beamid an unsigned 64-bit integer specifying the unique id of the beam.
     * @param time a double specifying the current data time, units are seconds.
     * @param azim a double specifying the azimuth pointing direction, units are radians.
     * @param elev a double specifying the elevation pointing direction, units are radians.
     * @param length a double specifying the beam's length(range), units are meters.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendBeamData(uint64_t beamid,
		      double time,
		      double azim,
		      double elev,
		      double length=10000);

    /// DCSServer sendGateData.
    /**
     * Updates gate position information to the server and subsequently to all connected clients.
     * Data is transmitted via the UDP channel.
     * @param gateid an unsigned 64-bit integer specifying the unique id of the gate.
     * @param time a double specifying the current data time, units are seconds.
     * @param azim a double specifying the azimuth pointing direction, units are radians.
     * @param elev a double specifying the elevation pointing direction, units are radians.
     * @param width a double specifying the gate's angular width, units are radians.
     * @param height a double specifying the gate's angular height, units are radians.
     * @param minrange a double specifying the gate's minimun downrange extent, units are meters.
     * @param centroid a double specifying the gate's centroid range, units are meters.
     * @param maxrange a double specifying the gate's maximum downrange extent, units are meters.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendGateData(uint64_t gateid,
		      double time,
		      double azim,
		      double elev,
		      double width,
		      double height,
		      double minrange,
		      double centroid,
		      double maxrange);

    ///Send scope data to the client.
    /**
     * Send a DCSScopeData object, created from the specified values, from the server to the client.   Scope data is transmitted via the UDP channel.
     * @param id an unsigned integer specifying the object's id.
     * @param xUnits an int16 specifying the object's x-units.
     * @param yUnits an int16 specifying the object's y-units.
     * @param time a double specifying the object's time.
     * @param xStart a double specifying the start of the object's x values.
     * @param xStep a double specifying the object's x step.
     * @param yBias a double specifying the object's y bias.
     * @param yScale a double specifying the object's y scale.
     * @param numDataPoints an int32 specifying the number of data points in the data array.
     * @param dataType an int8 specifying the data type of the object's array data.
     * @param numGates an unsigned int16 specifying the object's number of gates.
     * @param data a char array containing the object's data points.
     * @param gates a DCSCopeGateData array containing the object's gates.
     * @return true if successful, false if failed.
     */
    bool sendScopeData(uint64_t id,
		       UTILS::UnitType_t xUnits,
		       UTILS::UnitType_t yUnits,
		       double time,
		       double xStart,
		       double xStep,
		       double yBias,
		       double yScale,
		       uint32_t numDataPoints,
		       UTILS::VariableType_t dataType,
		       uint16_t numGates,
		       const void* data,
		       const DCSScopeGateData* gates);

    /// DCSServer sendGenericData.
    /**
     * Sends a specifed object's generic data to the server and subsequently to all connected clients.
     * Data is transmitted via the UDP channel.
     * @param objId an unsigned 64-bit integer specifying the unique id of the object.
     * @param tag a string specifying the generic data's tag.
     * @param data a string specifying the generic data.
     * @param bgntime a double specifying the data's activation time, units are seconds.
     * @param expiretime a double specifying the data's expiration time, units are radians.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendGenericData(uint64_t objId,
			 const std::string &tag,
			 const std::string &data,
			 double bgntime,
			 double expiretime=-1.);

    /// DCSServer sendCategoryData.
    /**
     * Sends a specifed object's category data to the server and subsequently to all connected clients.
     * Data is transmitted via the UDP channel.
     * @param objId an unsigned 64-bit integer specifying the unique id of the object.
     * @param category a string specifying the category data's category.
     * @param data a string specifying the category data.
     * @param timeVal a double specifying the data's activation time, units are seconds.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendCategoryData(uint64_t objId,
			  const std::string &category,
			  const std::string &data,
			  double timeVal=-1);

    /// DCSServer sendTime.
    /**
     * Updates the current scenario time to the server and subsequently to all connected clients.
     * Data is transmitted via the UDP channel.
     * @param time a double specifying the current data time, units are seconds.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendTime(double time);

    /// DCSServer sendStateEvent.
    /**
     * Sends on|off state events for all objects to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param objId an unsigned 64-bit integer specifying the unique id of the object.
     * @param on a boolean specifying the object's state, true(on), false(off).
     * @param time a double specifying the current data time, units are seconds.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendStateEvent(uint64_t objId,
			bool on,
			double time);

    /// DCSServer sendExpireEvent.
    /**
     * Sends an expire state event for all objects to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param objId an unsigned 64-bit integer specifying the unique id of the object.
     * @param time a double specifying the current data time, units are seconds.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendExpireEvent(uint64_t objId,
			 double time);

    /// DCSServer sendColorEvent.
    /**
     * Sends color change events for all objects to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param objId an unsigned 64-bit integer specifying the unique id of the object.
     * @param color an unsigned 32-bit integer specifying the color in RRGGBBAA hexadecimal(00-FF) notation: RR=red, GG=green, BB=blue, AA=alpha component.
     * @param time a double specifying the current data time, units are seconds.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendColorEvent(uint64_t objId,
			uint32_t color,
			double time);

    /// DCSServer sendTrackQualityEvent.
    /**
     * Sends good|bad track quality events for platforms to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param objId an unsigned 64-bit integer specifying the unique id of the platform.
     * @param quality a boolean specifying the platforms's track quality, true(good), false(bad).
     * @param time a double specifying the current data time, units are seconds.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendTrackQualityEvent(uint64_t objId,
			       bool quality,
			       double time);

    /// DCSServer sendTargetEvent.
    /**
     * Sends target events for beams to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param objId an unsigned 64-bit integer specifying the unique id of the beam.
     * @param targetId a 64-bit integer specifying the unique id of the target (-1 means no target).
     * @param time a double specifying the current data time, units are seconds.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendTargetEvent(uint64_t objId,
			 int64_t targetId,
			 double time);

    /// DCSServer sendClassificationEvent.
    /**
     * Sends scenario level classification events for beams to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param classification an 8-bit integer specifying the classification.  See DCSdefs.h for more information.
     * @param time a double specifying the current data time, units are seconds.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendClassificationEvent(DCSClassification_t classification,
				 double time);

    /// DCSServer sendClassificationEvent.
    /**
     * Sends scenario level classification events for beams to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param classification a string specifying the classification.
     * @param color an unsigned 32-bit integer specifying the color in RRGGBBAA hexadecimal(00-FF) notation: RR=red, GG=green, BB=blue, AA=alpha component.
     * @param time a double specifying the current data time, units are seconds.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendClassificationEvent(const std::string &classification,
				 uint32_t color,
				 double time);

    /// DCSServer sendPlatformInterpolate
    /**
     * Sends command to turn a platform's interpolate orientation flag on and off.  This value defaults
     * to 'on' for new platforms.  If turned off, then platforms will not interpolate orientation between data points
     * during a data playback.
     * @param platId ID of a beam object to set the flag for
     * @param interpolate Flag, if true turns on interpolation (default) of position and orientation, else if false turns off interpolation
     * @param time a double specifying the current data time, units are seconds.
     * @return Success is true, failure is false
     */
    bool sendPlatformInterpolate(uint64_t platId,
				 bool interpolate,
				 double time);

    /// DCSServer sendBeamInterpolatePosition
    /**
     * Sends command to turn a beam's interpolate position flag on and off.  This value defaults
     * to 'on' for new beams.  If turned off, then beams will not interpolate between data points
     * during a data playback.
     * @param beamId ID of a beam object to set the flag for
     * @param interpolate Flag, if true turns on interpolation (default), else if false turns off interpolation
     * @param time a double specifying the current data time, units are seconds.
     * @return Success is true, failure is false
     */
    bool sendBeamInterpolatePosition(uint64_t beamId,
				     bool interpolate,
				     double time);

    /// DCSServer sendGateInterpolatePosition
    /**
     * Sends command to turn a gate's interpolate position flag on and off.  This value defaults
     * to 'on' for new gates.  If turned off, then gates will not interpolate between data points
     * during a data playback.
     * @param gateId ID of a gate object to set the flag for
     * @param interpolate Flag, if true turns on interpolation (default), else if false turns off interpolation
     * @param time a double specifying the current data time, units are seconds.
     * @return Success is true, failure is false
     */
    bool sendGateInterpolatePosition(uint64_t gateId,
				     bool interpolate,
				     double time);

    /// DCSServer sendIconNameEvent.
    /**
     * Sends changes to icon names for platforms to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param objId an unsigned 64-bit integer specifying the unique id of the object.
     * @param iconName a string specifying the new icon of the platform, size limited to 64.
     * @param time a double specifying the current data time, units are seconds.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendIconNameEvent(uint64_t objId,
			   const std::string &iconName,
			   double time);

    /// DCSServer sendCallsignEvent.
    /**
     * Sends changes to callsigns (names) for platforms, beams and gates to the server and subsequently to all connected clients.
     * Data is transmitted via the TCP channel.
     * @param objId an unsigned 64-bit integer specifying the unique id of the object.
     * @param callsign a string specifying the new callsign of the object, size limited to 64.
     * @param time a double specifying the current data time, units are seconds.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool sendCallsignEvent(uint64_t objId,
			   const std::string &callsign,
			   double time);

    /// DCS Server getServerInfo.
    /**
     * Retrieves the low-level DCSServerInfo struct, available to access advanced functionality that this API might
     * not supply.
     * @return Pointer to the DCSServerInfo structure this API instantiation is maintaining
     */
    DCSCom::DCSServerInfo* getServerInfo() {return &DCS_;}

    /// DCS Server getPlatformMap.
    /**
     * Retrieves the map of active platforms keyed on DCS ID
     * @return Const pointer to the STL map containter this API instantiation is maintaining
     */
    const std::map<int64_t, double>* getPlatformMap() {return &PlatMap_;}

    /// DCS Server getBeamMap.
    /**
     * Retrieves the map of active beams keyed on Platform ID
     * @return Const pointer to the STL map containter this API instantiation is maintaining
     */
    const std::map<int64_t, std::vector<int64_t> >* getBeamMap() {return &BeamMap_;}

    /// DCS Server getGateMap.
    /**
     * Retrieves the map of active gates keyed on Beam ID
     * @return Const pointer to the STL map containter this API instantiation is maintaining
     */
    const std::map<int64_t, std::vector<int64_t> >* getGateMap() {return &GateMap_;}

    /// DCSServer getDatabaseFile.
    /**
     * Returns the filename of the DCS database.
     * @return a char string representing filename of the DCS database.
     */
    const char* getDatabaseFile() const {return DatabaseFile_;}

    /// DCSServer cleanDatabaseFile
    /**
     * Deletes entries from the database file that are no longer used.
     */
    bool cleanDatabaseFile();

    /// DCSServer inDCSHeaders.
    /**
     * Returns whether (true) or not (false) the listed ID is found in the DCS Header Container.
     * @return a bool representing the success(true) or failure(false) status of the method call.
     */
    bool inDCSHeaders(int64_t id_);
};

// // // // // // // // // // // // // // // // // // // // // // //

#endif /* DCS_SERVER_H */
