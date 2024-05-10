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
#ifndef SOCKCOM_H
#define SOCKCOM_H

#ifdef WIN32
#include <windows.h>
#endif
#include <string>
#include "SockIO.h"

#ifdef WIN32
typedef HANDLE SockCOMIOHandle;
#else
// Define the invalid handle value for non-Windows systems (already provided by Windows)
#define INVALID_HANDLE_VALUE -1
typedef int SockCOMIOHandle;
#endif

enum
{
  SOCKCOM_BR110    = 110,
  SOCKCOM_BR300    = 300,
  SOCKCOM_BR600    = 600,
  SOCKCOM_BR1200   = 1200,
  SOCKCOM_BR2400   = 2400,
  SOCKCOM_BR4800   = 4800,
  SOCKCOM_BR9600   = 9600,
  SOCKCOM_BR14400  = 14400,
  SOCKCOM_BR19200  = 19200,
  SOCKCOM_BR38400  = 38400,
  SOCKCOM_BR56000  = 56000,
  SOCKCOM_BR57600  = 57600,
  SOCKCOM_BR115200 = 115200,
  SOCKCOM_BR128000 = 128000,
  SOCKCOM_BR256000 = 256000
};

// Data bits
enum
{
  SOCKCOM_BITS5 = 5,
  SOCKCOM_BITS6 = 6,
  SOCKCOM_BITS7 = 7,
  SOCKCOM_BITS8 = 8
};

// Parity
enum
{
  SOCKCOM_PARITYSPACE = 0,
  SOCKCOM_PARITYMARK  = 1,
  SOCKCOM_PARITYEVEN  = 2,
  SOCKCOM_PARITYODD   = 3,
  SOCKCOM_PARITYNONE  = 4
};

// Stop bits
enum
{
  SOCKCOM_STOPBITS1   = 1,        // 1
  SOCKCOM_STOPBITS2   = 2         // 2
};

// Flow control
enum
{
  SOCKCOM_FCNONE     = 0,
  SOCKCOM_FCXONXOFF  = 1,
  SOCKCOM_FCRTSCTS   = 2
};

/**
 * Cross-platform library for reading and writing to COM ports.
 * Can read and write to COM 1-n.
 */
class SockCOM : public SockIO
{
  protected:
    ///Current COM port
    std::string comPort_;
    ///Baud rate
    unsigned int baudRate_;
    ///Data bits
    unsigned int dataBits_;
    ///Parity
    unsigned int parity_;
    ///Stop bits
    unsigned int stopBits_;
    ///Flow control
    unsigned int flowControl_;
    ///Handle to the current port
    SockCOMIOHandle ioHandle_;
    ///Current state of the object
    int currentState_;
    ///Last error reported
    int lastError_;
  public:
    ///Create a new SockCOM object bound to the specified device.
    /**Create a new SockCOM object bound to the specified device.
     * Intended for use with RS-232 devices with names that do not match the common pattern for an operating system,
     * such as USB to serial adapter cables which are assigned names such as '/dev/ttyUSB0' on Linux systems
     */
    SockCOM (const std::string& comPort, unsigned int baudRate = SOCKCOM_BR9600, unsigned int dataBits = SOCKCOM_BITS8, unsigned int parity = SOCKCOM_PARITYNONE, unsigned int stopBits = SOCKCOM_STOPBITS1, unsigned int flowControl = SOCKCOM_FCNONE);

    ///Create a new SockCOM object bound to the specific port.
    /**Create a new SockCOM object bound to the specific port.
     * The device name is determind by appending the specified port number to the following strings:
     * 'COM' + comPort for the Microsoft Windows operating system
     * '/dev/ttyS' + comPort for Linux systems
     * '/dev/term/' + (comPort + 'a')  for SunOS systems
     * '/dev/ttyf' + comPort for other UNIX systems
     */
    SockCOM (unsigned int comPort=1, unsigned int baudRate = SOCKCOM_BR9600, unsigned int dataBits = SOCKCOM_BITS8, unsigned int parity = SOCKCOM_PARITYNONE, unsigned int stopBits = SOCKCOM_STOPBITS1, unsigned int flowControl = SOCKCOM_FCNONE);

    ///Virtual destructor
    virtual ~SockCOM();

    ///Retrieve handle to serial device
    /**
     * Returns the file handle for the serial device.
     * @return an operating system specific handle to the file descriptor associated with the serial device, or 0 if the device is closed.
     */
    SockCOMIOHandle getDescriptor() const;

    ///Retrieve name of serial device
    /**
     * Returns the name of the associated serial device.
     * @return a string containing an operating system specific name of the associated serial device.
     */
    std::string getCOMPort() const;

    ///Retrieve baud rate setting for the serial device
    /**
     * Returns the baud rate associated with the serial device.
     * @return an unsigned integer specifying the baud rate selected for the serial device.
     */
    unsigned int getBaudRate() const;

    ///Retrieve number of data bits setting for the serial device
    /**
     * Returns the number of data bits per byte associated with the serial device.
     * @return an unsigned integer specifying the number of data bits per byte specified for the serial device.
     */
    unsigned int getDataBits() const;

    ///Retrieve parity setting for the serial device
    /**
     * Returns the parity setting associated with the serial device.
     * @return an unsigned integer specifying the parity setting selected for the serial device.
     */
    unsigned int getParity() const;

    ///Retrieve number of stop bits setting for the serial device
    /**
     * Returns the number of stop bits associated with the serial device.
     * @return an unsigned integer specifying the number of stop bits specified for the serial device.
     */
    unsigned int getStopBits() const;

    ///Retrieve flow control setting for the serial device
    /**
     * Returns the clow control setting associated with the serial device.
     * @return an unsigned integer specifying the flow control setting selected for the serial device.
     */
    unsigned int getFlowControl() const;

    ///Indicates the handle references a valid (open) serial device.
    /**
     * Indicates that the serial object's handle is valid (open).
     * @return a boolean value indicating the serial device handle is valid if true, and invalid if false.
     */
    virtual bool isValid() const;

    ///Indicates blocking I/O mode.
    /**
     * Indicates that the serial device is or is not using blocking I/O.
     * @return a boolean value indicating that the serial device is using blocking I/O if true, is
     *         not using blocking I/O if false.
     */
    bool isBlockingIO() const;

    ///Set blocking I/O mode.
    /**
     * Sets the serial device's I/O mode to blocking or non-blocking.
     * @param block a boolean value indicating that the serial device should use blocking I/O if true, non-blocking I/O if false.
     * @return a boolean value indicating that I/O mode was successfully changed if true, was not successfully changed if false.
     */
    bool setBlockingIO(bool block);

    ///Get system error code.
    /**
     * Returns the last known system error value.  This function is not specific to any
     * one serial device.  The error value retrieved can be for any socket that is part of the
     * overall application.
     * @return an error code indicating the reason for serial device failure.
     */
    int32_t getLastError() const;

    ///Set the system error code.
    /**
     * Sets the serial device's error code to the specified value.  All serial devices share the same
     * instance of the error code.
     * @param error an integer specifying the value to which the error value is to be set.
     */
    void setLastError(int32_t error);

    ///Open serial device.
    /**
     * Opens the serial device for communication.
     * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.
     */
    virtual bool open();

    ///Close serial device.
    /**
     * Close an open serial device.
     * @return a boolean value indicating the operation was successful if true, was unsuccessful if false.
     */
    virtual bool close();

    ///Indicates data is available for reading.
    /**
     * Indicates that data is avialable in the seral device's buffer for reading.
     * @param sec an unsigned integer specifying the number of seconds to wait before returning.
     * @param usec an unsinged integer specifying the number of microseconds to wait before returning.
     * @return a boolean value indicating that data is available if true, no data is available if false.
     */
    virtual bool readReady(uint32_t sec=0,uint32_t usec=0) const;

    ///Indicates space is available for writing.
    /**
     * Indicates that space in the serial device's buffer is avialable for writing.
     * @param sec an unsigned integer specifying the number of seconds to wait before returning.
     * @param usec an unsinged integer specifying the number of milliseconds to wait before returning.
     * @return a boolean value indicating that space is available if true, no space is available if false.
     */
    virtual bool writeReady(uint32_t sec=0,uint32_t usec=0) const;

    ///Read data.
    /**
     * Reads data that is available in the serial device's input buffer.
     * @param buffer a pointer to an object to be filled with the data in the socket buffer.
     * @param size an integer specifying the size, in bytes, of 'buffer'.
     * @param flags an integer specifying the bit flags to be passed to network sockets when reading.
     *        Unused by serial devices.
     * @return the total number of bytes read.
     */
    virtual int32_t read(void* buffer,int32_t size,int32_t flags=0) const;

    ///Write data.
    /**
     * Writes data to the network.
     * @param buffer a pointer to an object filled with the data to be written.
     * @param size an integer specifying the size, in bytes, of 'buffer'.
     * @param flags an integer specifying the bit flags to be passed to network sockets when writing.
     *        Unused by serial devices.
     * @return the total number of bytes written.
     */
    virtual int32_t write(const void* buffer,int32_t size,int32_t flags=0) const;
};

#endif /* SOCKCOM_H */
