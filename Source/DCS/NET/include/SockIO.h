// -*- mode: c++ -*-
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
 * July 2, 2004 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 *
 *
 */
#ifndef SOCKIO_H
#define SOCKIO_H

#include "inttypesc.h"

/**
 * Pure virtual class for writing to i/o sockets and devices
 */
class SockIO
{
  public:
    ///Creates an empty SockIO object
    SockIO() {}
    ///Destroys a SockIO object
    virtual ~SockIO() {}

    ///Indicates the socket handle references a valid kernel socket.  
    /**
     * Indicates that the socket object's socket handle is valid.  
     * @return a boolean value indicating the socket handle is valid if true, and invalid if false.  
     */
    virtual bool isValid() const {return false;}

    ///Indicates blocking I/O mode.  
    /**
     * Indicates that the socket is or is not using blocking I/O.  
     * @return a boolean value indicating that the socket is using blocking I/O if true, is
     *         not using blocking I/O if false.  
     */
    virtual bool isBlockingIO() const=0;

    ///Set blocking I/O mode.  
    /**
     * Sets the sockets I/O mode to blocking or non-blocking.  
     * @param block a boolean value indicating that the socket should use blocking I/O if true, non-blocking I/O if false.  
     * @return a boolean value indicating that I/O mode was successfully changed if true, was not successfully changed if false.  
     */
    virtual bool setBlockingIO(bool block)=0;
    
    ///Open socket.  
    /**
     * Binds socket and, depending on socket type, connects to peer or listens for connections.  
     * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.  
     */
    virtual bool open()=0;

    ///Close socket.  
    /**
     * Close an open socket.  
     * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.  
     */
    virtual bool close()=0;

    ///Indicates data is available for reading.  
    /**
     * Indicates that data is avialable in the socket buffer for reading.  
     * @param sec an unsigned integer specifying the number of seconds to wait before returning.  
     * @param usec an unsinged integer specifying the number of microseconds to wait before returning.  
     * @return a boolean value indicating that data is available if true, no data is available if false.  
     */
    virtual bool readReady(uint32_t sec=0,uint32_t usec=0) const=0;

    ///Indicates space is available for writing.  
    /**
     * Indicates that space in the socket buffer is avialable for writing.  
     * @param sec an unsigned integer specifying the number of seconds to wait before returning.  
     * @param usec an unsinged integer specifying the number of milliseconds to wait before returning.  
     * @return a boolean value indicating that space is available if true, no space is available if false.  
     */
    virtual bool writeReady(uint32_t sec=0,uint32_t usec=0) const=0;

    ///Read data. 
    /**
     * Reads data that is available in the socket input buffer.  
     * @param buffer a pointer to an object to be filled with the data in the socket buffer.  
     * @param size an integer specifying the size, in bytes, of 'buffer'.  
     * @param flags an integer specifying the bit flags to be passed to the socket when reading.
     *        These are the flags used by the BSD socket function 'recv'.  See the 'recv' man pages or
     *        MSDN documentation for more details.  
     * @return the total number of bytes read.  
     */
    virtual int32_t read(void* buffer,int32_t size,int32_t flags=0) const=0;

    ///Write data.  
    /**
     * Writes data to the network.  
     * @param buffer a pointer to an object filled with the data to be written.  
     * @param size an integer specifying the size, in bytes, of 'buffer'.  
     * @param flags an integer specifying the bit flags to be passed to the socket when writing.  
     *        These are the flags used by the BSD socket function 'send'.  See the 'send' man pages or
     *        MSDN documentation for more details.  
     * @return the total number of bytes written.  
     */
    virtual int32_t write(const void* buffer,int32_t size,int32_t flags=0) const=0;

    ///Write a fixed amount of data. 
    /**
     * Writes the total number of bytes specified.  
     * @param buffer a pointer to an object filled with the data to be written.  
     * @param size an integer specifying the size, in bytes, of 'buffer'.  The function will not return until
     *        exactly this number of bytes has been written, or an error has been encountered.  
     * @param flags an integer specifying the bit flags to be passed to the socket when writing.  
     *        These are the flags used by the BSD socket function 'send'.  See the 'send' man pages or
     *        MSDN documentation for more details.  
     * @return the total number of bytes written.  
     */
    virtual int32_t writeAll(const void* buffer,int32_t size,int32_t flags=0) const;
};

#endif /* SOCKIO_H */
