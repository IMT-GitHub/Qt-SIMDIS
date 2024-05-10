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

#ifndef _DCSCONSOLELOGGER_H_
#define _DCSCONSOLELOGGER_H_

class DCSBaseData;
class DCSPlatformHeader;
class DCSPlatformData;
class DCSBeamHeader;
class DCSBeamData;
class DCSGateHeader;
class DCSGateData;
class DCSScopeData;
class DCSTimeHeader;
class DCSTimeData;
class DCSScenarioHeader;
class DCSScenarioData;
class DCSEvent;
class DCSGenericData;
class DCSCategoryData;
class DCSCoordReferenceFrame;

///A class for logging DCS data structures to the console.
/**
 * DCSConsoleLogger writes the contents of DCS data structures
 * and character strings to stderr for user evaluation.
 */
class DCSConsoleLogger : public DCSLogger
{
  private:
    ///Log contents of DCSBaseData to console.
    /**
     * Log the contents of DCSBaseData to console.  Each of the
     * protected log functions will call this.  
     * @internal
     * @param item a pointer to the DCSBaseData object to be logged.
     * @return true if successful, false if failed.
     */
    bool logBaseData(const DCSBaseData* item) const;

    ///Log coordinate reference frame data to console
    bool logCoordReferenceFrame (const DCSCoordReferenceFrame& refFrame) const;

  protected:
    ///Log a platform header to console.
    /**
     * Log the contents of a DCSPlatformHeader structure to the console.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSPlatformHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logPlatformHeader()
     */
    virtual bool logPlatformHeader(const DCSPlatformHeader* item) const;

    ///Log platform data to console.
    /**
     * Log the contents of a DCSPlatformData structure to the console.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSPlatformData object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logPlatformData()
     */
    virtual bool logPlatformData(const DCSPlatformData* item) const;

    ///Log a beam header to console.
    /**
     * Log the contents of a DCSBeamHeader structure to the console.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSBeamHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logBeamHeader()
     */
    virtual bool logBeamHeader(const DCSBeamHeader* item) const;

    ///Log beam data to console.
    /**
     * Log the contents of a DCSBeamData structure to the console.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSBeamData object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logBeamData()
     */
    virtual bool logBeamData(const DCSBeamData* item) const;

    ///Log a gate header to console.
    /**
     * Log the contents of a DCSGateHeader structure to the console.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSGateHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logGateHeader()
     */
    virtual bool logGateHeader(const DCSGateHeader* item) const;

    ///Log gate data to console.
    /**
     * Log the contents of a DCSGateData structure to the console.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSGateData object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logGateData()
     */
    virtual bool logGateData(const DCSGateData* item) const;

    ///Log gate data to file.
    /**
     * Log the contents of a DCSScopeData structure to the file.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSScopeData object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logScopeData()
     */
    virtual bool logScopeData(const DCSScopeData* item) const;
    
    ///Log time header to console.
    /**
     * Log the contents of a DCSTimeHeader structure to the console.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSTimeHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logTimeHeader()
     */
    virtual bool logTimeHeader(const DCSTimeHeader* item) const;

    /**
     * Log the contents of a DCSTimeData structure to the console.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSTimeData object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logTimeData()
     */
    virtual bool logTimeData(const DCSTimeData* item) const;

    ///Log scenario header to console.
    /**
     * Log the contents of a DCSScenarioHeader structure to the console.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSScenarioHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logScenarioHeader()
     */
    virtual bool logScenarioHeader(const DCSScenarioHeader* item) const;

    ///Log scenario data to console.
    /**
     * Log the contents of a DCSScenarioData structure to the console.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSScenarioData object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logScenarioData()
     */
    virtual bool logScenarioData(const DCSScenarioData* item) const;

    ///Log an event to console.
    /**
     * Log the contents of a DCSEvent structure to the console.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSEvent object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logEvent()
     */
    virtual bool logEvent(const DCSEvent* item) const;

    ///Log generic data to console.
    /**
     * Log the contents of a DCSGenericData structure to the console.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item address DCSGenericData to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logGenericData()
     */
    virtual bool logGenericData(const DCSGenericData* item) const;

    ///Log category data to console.
    /**
     * Log the contents of a DCSCategoryData structure to the console.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item address DCSCategoryData to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logGenericData()
     */
    virtual bool logCategoryData(const DCSCategoryData* item) const;

  public:
    ///Log DCS data.
    /**
     * Logs a DCS data structure to console.  Determines the type of data and logs the proper data
     * fields to the proper location.
     * @param item a pointer to any DCS object derived from DCSBaseData to be logged.
     * @return true if successful, false if failed.
     */
    virtual bool log(const DCSBaseData* item) const {return DCSLogger::log(item);}

    ///Log text to console.
    /**
     * Log a character string to the console.  
     * @param item character string to be logged.
     * @return true if successful, false if failed.
     * @see DCSLogger::log()
     */
    virtual bool log(const char* item) const;
};

#endif
