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
 * 2002 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,     
 * disclose, or release this software.
 *
 */

#ifndef _SOCKEXCEPTION_H_
#define _SOCKEXCEPTION_H_

///Socket exception class.  
/**
 * A generic socket exception class.  
 */
class SockException
{
private:
  const char* message_;
  static const char exceptionname_[];
public:
  ///Default socket exception constructor.  
  /**
   * Creates a socket exception object with a default message.  
   */
  SockException():message_(SockException::exceptionname_){}

  ///Socket exception constructor.  
  /**
   * Creates a socket exception object with a user specified error message.  
   * @param msg a character string containing an error message.  
   */
  SockException(const char* msg):message_(msg){}

  ///Retrieve error message.  
  /**
   * Returns the socket exception's error message.  
   * @return a charcter string containing an error message.  
   */
  const char* message() const {return message_;}

  ///Destructor.  
  /**
   * Virtual destructor.  
   */
  virtual ~SockException() {}
};

///Socket value exception class.  
/**
 * A socket value exception class.  
 */
class SockValueException : public SockException
{
private:
  static const char exceptionname_[];
public:
  ///Default socket value exception constructor.  
  /**
   * Creates a socket value exception object with a default message.  
   */
  SockValueException():SockException(SockValueException::exceptionname_){}

  ///Socket value exception constructor.  
  /**
   * Creates a socket value exception object with a user specified error message.  
   * @param msg a character string containing an error message.  
   */
  SockValueException(const char* msg):SockException(msg){}
};

///Socket connection exception class.  
/**
 * A socket connection exception class.  
 */
class SockConnException : public SockException
{
private:
  static const char exceptionname_[];
public:
  ///Default socket connection exception constructor.  
  /**
   * Creates a socket connection exception object with a default message.  
   */
  SockConnException():SockException(SockConnException::exceptionname_){}

  ///Socket connection exception constructor.  
  /**
   * Creates a socket connection exception object with a user specified error message.  
   * @param msg a character string containing an error message.  
   */
  SockConnException(const char* msg):SockException(msg){}
};

///Socket support exception class.  
/**
 * A socket support exception class.  
 */
class SockSupportException : public SockException
{
private:
  static const char exceptionname_[];
public:
  ///Default socket support exception constructor.  
  /**
   * Creates a socket support exception object with a default message.  
   */
  SockSupportException():SockException(SockSupportException::exceptionname_){}

  ///Socket support exception constructor.  
  /**
   * Creates a socket support exception object with a user specified error message.  
   * @param msg a character string containing an error message.  
   */
  SockSupportException(const char* msg):SockException(msg){}
};

#endif
