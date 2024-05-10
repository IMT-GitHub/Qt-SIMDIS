#ifndef _DCSBASEDATA_H_
#define _DCSBASEDATA_H_
/****************************************************************************
 *****                   Classification: UNCLASSIFIED                   *****
 *****                    Classified By:                                *****
 *****                    Declassify On:                                *****
 ****************************************************************************
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
#include "DCSdefs.h"
#include "inttypesc.h"

/// abstract base class for DCS network data types.
/**
 * All DCS data types to be sent to the network are derived from this class.
 */
class DCSBaseData
{
private:
    int16_t type_;	//The type of derived class
    int16_t version_;	//The version number for the DCS application that generated this data object

public:
    ///Unique id.  
    uint64_t id_;	/**< Each object must have a unique integer id.  Using an unsigned integer allows for more than 4 million unique object ids.  */

public:
    ///Base data constructor.  
    /**
     * Initializes the contents of DCSBaseData and sets the data type value.
     * @param type a short representing the type of data structure being created.  
     */
    DCSBaseData(int16_t type = DCSUNKNOWN)
    : type_   (type)
    , version_(DCS_VERSION)
    , id_     (0xFFFFFFFF)
    {
    }

    ///Base data constructor.  
    /**
     * Initializes the contents of DCSBaseData and sets the id and data type values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param type a short representing the type of data structure being created.  
     */
    DCSBaseData(uint64_t id, int16_t type)
    : type_   (type)
    , version_(DCS_VERSION)
    , id_     (id)
    {
    }

    ///Virtual destructor.  
    /**
     * Important for properly destroying derived objects.
     */
    virtual ~DCSBaseData(){}

    ///Set contents of data object to default values.  
    /**
     * Clears contents of data object, setting all fields to default values.  
     * Type and version values are never modified.  
     */
    virtual void clear() {id_=0xFFFFFFFF;}

    ///Get object type.  
    /**
     * Returns a word indicating the type of data object.
     * @return a short representing the type of data object.  
     */
    int16_t getType() const {return type_;}

    ///Get DCS version.  
    /**
     * The version number for the DCS application that generated this data object.
     * @return a short representing the version number.  High order byte is major
     * number and low order byte is minor number.  
     */
    int16_t getVersion() const {return version_;}

    ///Prepare data for network transmission.
    /**
     * Packs the contents of this structure into a network byte ordered
     * buffer.  This buffer is then sent out on the network.  The buffer
     * is expected to be equal to or larger than the size of the structure.
     * If the buffer is too small to accomodate the contents of the structure
     * an exception will be thrown.  
     * @param buffer a memory buffer large enough to hold the contents of the data structure.
     * @param size the size, in bytes, of buffer.
     * @return a pointer to the argument 'buffer' for convenience.
     * @see dataSize()
     * @see unpack()
     */
    virtual  void* pack(void* buffer,int32_t size) const=0;

    ///Extract data from network transmission.
    /**
     * Extracts the contents of a buffer, received from the network
     * to this structure.  The buffer is expected to be equal to or
     * larger than the size of the structure.  If the buffer is too small
     * to contain all of the data required by the structure an exception
     * will be thrown.  
     * @param buffer a memory buffer containing data for the data structure.
     * @param size the size, in bytes, of buffer.
     * @see dataSize()
     * @see pack()
     */
    virtual void unpack(const void* buffer,int32_t size)=0;

    ///Determine object's data size.
    /**
     * Returns the total size, in bytes, required by the data members of
     * the current object.  Can be used to determine the proper size of
     * buffers for the pack and unpack methods.
     * @return an integer representing the size of the object's data.
     * @see pack()
     * @see unpack()
     */
    virtual int32_t dataSize() const {return sizeof(type_)+sizeof(id_)+sizeof(version_);}

    ///Determine object's data size.
    /**
     * Returns the total size, in bytes, required by the data members of
     * the current object.  Can be used to determine the proper size of
     * buffers for the pack and unpack methods.
     * @param version Server version if applicable
     * @return an integer representing the size of the object's data.
     * @see pack()
     * @see unpack()
     */
    virtual int32_t dataSize(uint16_t version) const {return dataSize();}

    ///Determine object's physical size.
    /**
     * Returns actual size of object in memory.  May differ from data size
     * for padding, virtual function pointer table, etc.
     * @return an integer representing the size of the object as returned from sizeof.  
     */
    virtual int32_t sizeOf() const {return sizeof(*this);}

  protected:
    ///Internal packing function
    int32_t packit(void* buffer) const; /**< @internal */
    ///Internal unpacking function
    int32_t unpackit(const void* buffer); /**< @internal */
};

#endif

