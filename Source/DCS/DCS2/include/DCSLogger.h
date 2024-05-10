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

#ifndef _DCSLOGGER_H_
#define _DCSLOGGER_H_

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

///An abstract class for logging DCS data structures.
/**
 * DCS data structures are logged with derivations of this class, created to
 * write data to the console, files, etc.
 */
class DCSLogger
{
  protected:
    ///Log a platform header.
    /**
     * Pure virtual function to be overriden for logging a DCSPlatformHeader
     * structure to file, console, etc.  Will be used by the public log function.  
     * @internal
     * @param item a pointer to the DCSPlatformHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see log()
     */
    virtual bool logPlatformHeader(const DCSPlatformHeader* item) const=0;

    ///Log platform data.
    /**
     * Pure virtual function to be overriden for logging a DCSPlatformData
     * structure to file, console, etc.  Will be used by the public log function.  
     * @internal
     * @param item a pointer to the DCSPlatformData object to be logged.  
     * @return true if successful, false if failed.  
     * @see log()
     */
    virtual bool logPlatformData(const DCSPlatformData* item) const=0;

    ///Log a beam header.
    /**
     * Pure virtual function to be overriden for logging a DCSBeamHeader
     * structure to file, console, etc.  Will be used by the public log function.  
     * @internal
     * @param item a pointer to the DCSBeamHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see log()
     */
    virtual bool logBeamHeader(const DCSBeamHeader* item) const=0;

    ///Log beam data.
    /**
     * Pure virtual function to be overriden for logging a DCSBeamData
     * structure to file, console, etc.  Will be used by the public log function.  
     * @internal
     * @param item a pointer to the DCSBeamData object to be logged.  
     * @return true if successful, false if failed.  
     * @see log()
     */
    virtual bool logBeamData(const DCSBeamData* item) const=0;

    ///Log a gate header.
    /**
     * Pure virtual function to be overriden for logging a DCSGateHeader
     * structure to file, console, etc.  Will be used by the public log function.  
     * @internal
     * @param item a pointer to the DCSGateHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see log()
     */
    virtual bool logGateHeader(const DCSGateHeader* item) const=0;

    ///Log gate data.
    /**
     * Pure virtual function to be overriden for logging a DCSGateData
     * structure to file, console, etc.  Will be used by the public log function.  
     * @internal
     * @param item a pointer to the DCSGateData object to be logged.  
     * @return true if successful, false if failed.  
     * @see log()
     */
    virtual bool logGateData(const DCSGateData* item) const=0;


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
    virtual bool logScopeData(const DCSScopeData* item) const=0;
    
    ///Log time header.
    /**
     * Pure virtual function to be overriden for logging a DCSTimeHeader
     * structure to file, console, etc.  Will be used by the public log function.  
     * @internal
     * @param item a pointer to the DCSTimeHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see log()
     */
    virtual bool logTimeHeader(const DCSTimeHeader* item) const=0;

    ///Log time data.
    /**
     * Pure virtual function to be overriden for logging a DCSTimeData
     * structure to file, console, etc.  Will be used by the public log function.  
     * @internal
     * @param item a pointer to the DCSTimeData object to be logged.  
     * @return true if successful, false if failed.  
     * @see log()
     */
    virtual bool logTimeData(const DCSTimeData* item) const=0;

    ///Log scenario header.
    /**
     * Pure virtual function to be overriden for logging a DCSScenarioHeader
     * structure to file, console, etc.  Will be used by the public log function.  
     * @internal
     * @param item a pointer to the DCSScenarioHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see log()
     */
    virtual bool logScenarioHeader(const DCSScenarioHeader* item) const=0;

    ///Log scenario data.
    /**
     * Pure virtual function to be overriden for logging a DCSScenarioData
     * structure to file, console, etc.  Will be used by the public log function.  
     * @internal
     * @param item a pointer to the DCSScenarioData object to be logged.  
     * @return true if successful, false if failed.  
     * @see log()
     */
    virtual bool logScenarioData(const DCSScenarioData* item) const=0;

    ///Log an event.
    /**
     * Pure virtual function to be overriden for logging a DCSEvent
     * structure to file, console, etc.  Will be used by the public log function.  
     * @internal
     * @param item a pointer to the DCSEvent object to be logged.  
     * @return true if successful, false if failed.  
     * @see log()
     */
    virtual bool logEvent(const DCSEvent* item) const=0;

    ///Log generic data.
    /**
     * Pure virtual function to be overriden for logging a DCSGenericData
     * structure to file, console, etc.  Will be used by the public log function.  
     * @internal
     * @param item a pointer to the DCSGenericData object to be logged.  
     * @return true if successful, false if failed.  
     * @see log()
     */
    virtual bool logGenericData(const DCSGenericData* item) const=0;

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
    virtual bool logCategoryData(const DCSCategoryData* item) const=0;

  public:
    ///Virtual destructor.
    /**
     * Important for properly destroying derived objects.
     */
    virtual ~DCSLogger(){}

    ///Log DCS data.
    /**
     * Logs a DCS data structure to file, console, etc depending on the
     * implementation.  Determines the type of data and logs the proper data
     * fields to the proper location.
     * @param item a pointer to any DCS object derived from DCSBaseData to be logged.
     * @return true if successful, false if failed.
     */
    virtual bool log(const DCSBaseData* item) const;

    ///Log text.
    /**
     * Pure virtual function to be overridden for logging character strings
     * to file, console, etc.
     * @param item character string to be logged.
     * @return true if successful, false if failed.
     */
    virtual bool log(const char* item) const=0;
};

#endif
