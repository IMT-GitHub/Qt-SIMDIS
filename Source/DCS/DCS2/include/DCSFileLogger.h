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

#ifndef _DCSFILELOGGER_H_
#define _DCSFILELOGGER_H_

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

///A class for logging DCS data structures to a file.
/**
 * DCSFileLogger writes the contents of DCS data structures
 * and character strings to a user specified log file.
 */
class DCSFileLogger : public DCSLogger
{
  private:
    ///File pointer.
    /**
     * A file pointer specifying the file to be used for logging.
     * @internal
     */
    FILE* file;

    ///File close indicator.
    /**
     * A flag indicating whether a file should be closed when the file logger
     * is destoryed (true if the file was opened by the logger, false if the
     * file was opened by a third party).
     * @internal
     */
    const bool closefile;
    
    ///Log contents of DCSBaseData to file.
    /**
     * Log the contents of DCSBaseData to file.  Each of the
     * protected log functions will call this.  
     * @internal
     * @param item a pointer to the DCSBaseData object to be logged.
     * @return true if successful, false if failed.
     */
    bool logBaseData(const DCSBaseData* item) const;

    ///Log coordinate reference frame data to file
    bool logCoordReferenceFrame (const DCSCoordReferenceFrame& refFrame) const;

  protected:
    ///Log a platform header to file.
    /**
     * Log the contents of a DCSPlatformHeader structure to the file.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSPlatformHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logPlatformHeader()
     */
    virtual bool logPlatformHeader(const DCSPlatformHeader* item) const;

    ///Log platform data to file.
    /**
     * Log the contents of a DCSPlatformData structure to the file.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSPlatformData object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logPlatformData()
     */
    virtual bool logPlatformData(const DCSPlatformData* item) const;

    ///Log a beam header to file.
    /**
     * Log the contents of a DCSBeamHeader structure to the file.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSBeamHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logBeamHeader()
     */
    virtual bool logBeamHeader(const DCSBeamHeader* item) const;

    ///Log beam data to file.
    /**
     * Log the contents of a DCSBeamData structure to the file.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSBeamData object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logBeamData()
     */
    virtual bool logBeamData(const DCSBeamData* item) const;

    ///Log a gate header to file.
    /**
     * Log the contents of a DCSGateHeader structure to the file.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSGateHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logGateHeader()
     */
    virtual bool logGateHeader(const DCSGateHeader* item) const;

    ///Log gate data to file.
    /**
     * Log the contents of a DCSGateData structure to the file.
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

    ///Log time header to file.
    /**
     * Log the contents of a DCSTimeHeader structure to the file.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSTimeHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logTimeHeader()
     */
    virtual bool logTimeHeader(const DCSTimeHeader* item) const;

    ///Log time data to file.
    /**
     * Log the contents of a DCSTimeData structure to the file.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSTimeData object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logTimeData()
     */
    virtual bool logTimeData(const DCSTimeData* item) const;

    ///Log scenario header to file.
    /**
     * Log the contents of a DCSScenarioHeader structure to the file.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSScenarioHeader object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logScenarioHeader()
     */
    virtual bool logScenarioHeader(const DCSScenarioHeader* item) const;

    ///Log scenario data to file.
    /**
     * Log the contents of a DCSScenarioData structure to the file.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSScenarioData object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logScenarioData()
     */
    virtual bool logScenarioData(const DCSScenarioData* item) const;

    ///Log an event to file.
    /**
     * Log the contents of a DCSEvent structure to the file.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSEvent object to be logged.  
     * @return true if successful, false if failed.  
     * @see DCSLogger::log()
     * @see DCSLogger::logEvent()
     */
    virtual bool logEvent(const DCSEvent* item) const;

    ///Log generic data to file.
    /**
     * Log the contents of a DCSGenericData structure to the file.
     * Used by the public DCSLogger::log function.  
     * @internal
     * @param item a pointer to the DCSGenericData object to be logged.  
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
    ///File logger constructor
    /**
     * Create a file logger to log to a file of a specified name.  If the
     * file does not exist it will be created.  if the file exists it can be
     * optionally truncated or appended to.  The file will be closed when the
     * logger is destroyed.  
     * @param filename string specifying the name of file to open/create for logging.
     * @param truncate option specifying whether an existing file should be appended to or truncated.  If option is set to true the file woll be truncated.  If set to false the file will be appended to.
     */
    DCSFileLogger(const char* filename,bool truncate);

    ///File logger constructor
    /**
     * Create a file logger to log to a file that was previously opened with a call to fopen.
     * Because the file logger did not open the file, the file will not be closed when the
     * logger is destroyed.  This constructor makes it possible to pass stdout or stderr to the file logger
     * in place of a file name or pointer to a file on disk (making DCSConsoleLogger almost entirely unnecessary).
     * @param fp file pointer specifying a file previously opened with a call to fopen.  
     */
    DCSFileLogger(FILE* fp);
    
    ///File logger constructor
    /**
     * Create a file logger to log to a file that was previosuly opened with a call to open.
     * DCSFileLogger will use the specified file descriptor to create a file
     * stream (FILE*) with a call to fdopen.  Converting the file descriptor to a
     * file stream is done to enable I/O buffering, which is not done with file descriptors.  
     * Because the file logger did not open the file, the file will not be closed when the
     * logger is destroyed.  
     * @param fd file descriptor specifying a file previously opened with a call to open.
     * This descriptor will be converted to a FILE* with a call to fdopen.  The file descriptor must
     * have been created with write access.  
     */
    DCSFileLogger(int32_t fd);

    ///File logger destructor
    /**
     * File logger destructor to close a file if it was opened by the file loger.
     */
    ~DCSFileLogger();
    
    ///Check for valid file.
    /**
     * Checks to see if the file logger has a valid file pointer.  A file
     * pointer would only be invalid if the file logger failed to open a file
     * for any reason, was passed a NULL file pointer, or was past an invalid
     * file descriptor that could not be converted to a stream (the file descriptor
     * is not for a valid open file).
     * @return true if file is valid, false if file is not valid.
     */
    bool isValid() const;

    ///Check for file error.
    /**
     * Checks to see if the file logger has encountered an error with the file.
     * Error check is done with a call to ferror.  After the check is done, the
     * error indicator is reset with a call to clearerr.  
     * @return true if error was encountered, false if error was not encountered.
     */
    bool hasError() const;

    ///Flush file buffer.
    /**
     * Flushes data from the file buffer with a call to fflush.
     * @return true if successful, false if an error was encountered.  
     */
    bool flush() const;

    ///Log DCS data.
    /**
     * Logs a DCS data structure to file.  Determines the type of data and logs the proper data
     * fields to the proper location.
     * @param item a pointer to any DCS object derived from DCSBaseData to be logged.
     * @return true if successful, false if failed.
     */
    virtual bool log(const DCSBaseData* item) const {return DCSLogger::log(item);}

    ///Log text to file.
    /**
     * Log a character string to the file.  
     * @param item character string to be logged.
     * @return true if successful, false if failed.
     * @see DCSLogger::log()
     */
    virtual bool log(const char* item) const;
};

#endif
