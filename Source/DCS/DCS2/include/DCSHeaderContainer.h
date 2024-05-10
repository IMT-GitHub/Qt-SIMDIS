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

#ifndef _DCSHEADERCONTAINER_H_
#define _DCSHEADERCONTAINER_H_

class DCSBaseHeader;

///A structure for tagging each record added to the data file
/**
 * Each record added to the data file shall be prepended with this structure.
 * The strucutre serves the purpose if identifying the state of the record, active
 * or marked for deletion, and indicating the size of the record, for reloading, external processing, etc.
 * This structure is used internally by DCSHeaderContainer.  
 * @internal
 * @see DCSHeaderContainer
 */
struct DCSFileEntryHeader
{
  public:
    ///Entry status indicator.  
    int8_t active_;	/**< The status of the following file entry, indicating that it is active or is marked for deletion.  */
    ///Padding to make valgrind happy
    int8_t pad_;
    ///Entry type.  
    int16_t type_;      /**< The type of data object.  */
    ///Entry size.  
    int16_t size_;	/**< The size of the following file entry.  */
    DCSFileEntryHeader() : active_(0), pad_(0), type_(0), size_(0) {}
};

///A structure for identifying the location of a header in a DCSHeaderContainer's random access file.
/**
 * A structure to hold the id, size, and location in the random access file for each
 * header that is stored in a DCSHeaderContainer.
 * This structure is used internally by DCSHeaderContainer.  
 * @internal
 * @see DCSHeaderContainer
 */
struct DCSHeaderRecord
{
  public:
    ///Header location within random access file.  
    int32_t location_;	/**< The location of this particular header within the random access file.  */
    ///Size of the header.  
    uint16_t size_;	/**< The size of the header data present in the file.  */
    ///Padding to make valgrind happy
    int16_t pad_;
    DCSHeaderRecord() : location_(0), size_(0), pad_(0) {}
};

///A structure for generic header retrieval
/**
 * A a structure for retrieving one of the three header types from a DCSHeaderContainer.
 * This structure can be used as an argument for DCSHeaderContainer::retrieve in order
 * to avoid dynamic memory allocation.  If a DCSHeaderGroup argument is given to 
 * DCSHeaderContainer::retrieve, the approprate union field will be used to store the 
 * retrieved header and the header type flag field will be set to indicate which 
 * type of header was retrieved.  When DCSHeaderContainer::retrieve does not receive 
 * a DCSHeaderGroup item as an argument, it allocates the memory necessary to store 
 * the retreived header, leaving the responsibility of freeing that memory to the 
 * calling program.
 */
struct DCSHeaderGroup
{
  public:
    ///Entry type.  
    int16_t type_;                   /**< The type of header object.  */

    DCSBaseHeader* baseheader_;     /**< A pointer that will be set to point the header object that was filled with the retrieved data.  */

    ///DCS platform header.  
    DCSPlatformHeader platheader_;  /**< A DCSPlatformHeader object to be filled if the retrieved data is for a platform header.  */

    ///DCS beam header.  
    DCSBeamHeader beamheader_;      /**< A DCSBeamHeader object to be filled if the retrieved data is for a beam header.  */

    ///DCS gate header.  
    DCSGateHeader gateheader_;      /**< A DCSGateHeader object to be filled if the retrieved data is for a gate header.  */

    DCSHeaderGroup() : type_(DCSUNKNOWN),baseheader_(NULL) {}
};

///Header record table for header container.
/**
 * A table for header records to be maintained by the header container.
 * @internal
 */
typedef std::map<uint64_t,DCSHeaderRecord*> HeaderTable;

///A container class for storing header information.
/**
 * DCS header data types are stored in a header container.  The header container
 * stores the contents of a header object in a random access file, only keeping
 * the header id in memory.  This keeps memory usage to a minimum and allows
 * for data recovery, so in the event that a DCS server crashes it may be rstored
 * to the exact state at failure by reading the file.  The DCSHeaderContainer is used
 * by the server for distributing headers to the client during a connection negotiation.  
 */
class DCSHeaderContainer
{
  protected:
    ///Random access file descriptor
    int fd_;			/**< File descriptor for random access file containting the header data.  */
    ///Random access file preservation flag;
    bool save_;			/**< Flag indicating that the random access file should not be deleted when the container is destroyed.  */
    ///Random access file name.
    char filename_[PATH_MAX];	/**< Name identifying the random access file containing the header data.  */
    ///Record table
    HeaderTable table_; /**< Table of records containing a header's id, size, and location within the random access file.  A record is stored for each entry in the random access file.  */

    ///Destroy all of the header records in the table.  
    /**
     * Remove and delete each header record contained within the table.  
     */
    void clearItems();

  public:
    ///Header container constructor
    /**
     * Initializes the contents of DCSHeaderRecord to the default values.
     */
    DCSHeaderContainer();

    //Header container destructor
    /**
     * Destroys all resources allocated for header container and deletes the random access file if the save flag is set to false.  
     */
    ~DCSHeaderContainer();

    ///Indicates size of the DCSHeaderContainer.
    /**
     * Indicates the number of headers stored within the DCSHeaderContainer.
     * @return an integer indicating the number of headers stored in the DCSHeaderContainer.  
     */
    size_t size() const {return table_.size();}
    
    ///Intialize the DCSHeaderContainer.
    /**
     * Creates the random access file.  If the file exists, it's contents will be erased and the file will be repaced.  
     * @param filename a string speciying the name to use when creating the random access file.
     * @return true if successful, false if failure.  
     */
    bool initialize(const char* filename);

    ///Restore the DCSHeaderContainer from a file.
    /**
     * Opens an existing random access file.  If the file exists, it's contents will be used to (re)build the database.
     * If the file does not exist, function will behave as if a failure was encountered and return false.  
     * @param filename a string speciying the name for the random access file containing the header data.
     * @param version an unsigned short to be filled the DCS version number of the file's creating server.  If a restore fails, check this value to make sure
     * it matches the DCS version number of the current server.  
     * @return true if successful, false if failure.  
     */
    bool restore(const char* filename,uint16_t* version=NULL);

    ///Add a new header to the container.
    /**
     * Adds a new header to the container.  If a header with the same id is
     * already present in the table, the new header will not be added.  If it
     * is unknown whether a particular header is already present in the container,
     * it is safer to use the update function.
     * This function makes a copy of the specified header, so it is safe to free the 
     * resources used by the specified header after the insertion.  
     * @param header a pointer to the DCSBaseHeader derived object to be added to the container.
     * @return true if successful, false if failure.
     * @see insert()
     */
    bool insert(const DCSBaseHeader* header);

    ///Update an existing header.
    /**
     * Updates the contents of a header that is present in the container.  If the
     * specified header is not present in the container, a new entry for that header will be inserted.
     * For this reason, it is advisable for using update rather than insert when adding/updating headers to the container.
     * This function makes a copy of the specified header, so it is safe to free the 
     * resources used by the specified header after the update.  
     * @param header a pointer to the DCSBaseHeader derived object with which the containers contents are to be updated.
     * @param present a boolean value indicating if the specified header was present in the container.  This
     * value is set by the function for inspection by the calling process when 'update' returns.
     * If the value is true the object was already present in the container.
     * If value is false the object was not yet present and has been inserted.  
     * @return true if successful, false if failure.
     * @see restore()
     */
    bool update(const DCSBaseHeader* header,bool& present);

    ///Remove a header from the container.
    /**
     * Removes a header from the container.  The header's record will be removed from the
     * table containg the location of the header within the random access file.  The entry in the random
     * access file will be marked for deletion, but not deleted.  Lazy deletion is used with the random access file
     * to minimize the amount of processing time required for reorganizing the file with the deletion of each entry.
     * Any headers marked for deletion can be cleared from the file with a call to clean.
     * @param id an unsigned integer identifying the object to be removed from the container.  
     * @return true if successful, false if failure.
     * @see clean()
     */
    bool remove(uint64_t id);

    ///Retrieve a header from the container.
    /**
     * Retrieves the requested record from the container and returns a pointer
     * to that record.  If a DCSHeaderGroup object is specified, the record will be copied to the
     * appropriate field in the object.  If no DCSHeaderGroup object is specified, resources for the
     * returned DCSBaseHeader object are allocated by this function with the new operator.  The calling
     * application is responsible for freeing these resources through a subsequent call to the delete operator.  
     * @param id an unsigned integer identifying the header to retrieve.
     * @param hg a DCSHeaderGroup structure to be filled with the retrieved data.  This
     *        argument is optional.  If hg is NULL, retrieve will allocate the memory needed
     *        to store the retrieved data.  The calling program is responsible for freeing
     *        this memory.  
     * @return a pointer to the requested header, or NULL if the header is not found.  
     */
    DCSBaseHeader* retrieve(uint64_t id,DCSHeaderGroup* hg=NULL) const;

    ///Traverse the container, executing a user specified function.
    /**
     * Traverse the container element by element, executing a user specified function upon
     * each element.  The function may be used for any purpose except modifying a header, deleting a header, or deallocating the memory
     * segment within which the header is stored.
     * @param function a pointer to the function that is to be used for processing a header.
     * This function is shall be deisgned to accept two parameters, the first is pointer to the header
     * object to process, and the second is a pointer to a data object specified by the user in the call to traverse.
     * This function is not expected to return a value.  
     * @param data a pointer to a data object to be used as a parameter for the specified function.  
     */
    void traverse(void (*function)(const DCSBaseHeader*,void*),void* data) const;

    ///Remove any records marked for deletion from the random access file.
    /**
     * Process the random access file in order to remove any records that are marked for deletion.
     * The file will be reorganized to eliminate any gaps caused by the removal of records, and the
     * table will be rebuilt to update the values specifying the location of each record in the file.
     * This function is not thread safe!  A clean should not be executed while other execution units are
     * accessing the file.  
     * @return true if successful, false if failure.  
     */
    bool clean();

    ///Check if the random access file is to be preserved on exit.
    /**
     * Check to see if the random access file should be saved on container destruction.  The
     * default action is to destroy the file when the container is destroyed.  
     */
    bool getSave() const;

    ///Specify that the random access file should be preserved on exit.
    /**
     * Specify whether the random access file should be saved or deleted when the container is destroyed.
     * Setting this value to true makes it possible to save a random access file for (re)loading at
     * a later time.
     * @param save flag specifying the preservation status fo a random access file.
     */
    void setSave(bool save);
};

#endif
