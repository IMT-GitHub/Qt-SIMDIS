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
 * July 6, 2004 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 *
 *
 */
#ifndef WIN32
#include <unistd.h>
#include <termios.h>
#else
#include <WinSock2.h>
#include <windows.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "timec.h"
#include "iostreamc"
#include "sstreamc"

#include "sockdefs.h"
#include "SockIO.h"
#include "SockCOM.h"

// Define the default port names
#ifndef WIN32
# ifdef Linux
#  define DEVICE_STRING  "/dev/ttyS"
# else
#  ifdef Solaris
#   define DEVICE_STRING  "/dev/term/"
#  else
#   define DEVICE_STRING  "/dev/ttyf"
#  endif
# endif
#else
#  define DEVICE_STRING  "COM"
#endif

enum
{
  STATE_CLOSED=0,
  STATE_OPEN,
  STATE_ERROR
};

SockCOM::SockCOM (const std::string& comPort, unsigned int baudRate, unsigned int dataBits, unsigned int parity, unsigned int stopBits, unsigned int flowControl)
: SockIO(),
  comPort_(comPort),
  baudRate_(baudRate),
  dataBits_(dataBits),
  parity_(parity),
  stopBits_(stopBits),
  flowControl_(flowControl),
  ioHandle_(INVALID_HANDLE_VALUE),
  currentState_(STATE_CLOSED)
{
  open();
}

SockCOM::SockCOM (unsigned int comPort, unsigned int baudRate, unsigned int dataBits, unsigned int parity, unsigned int stopBits, unsigned int flowControl)
: SockIO(),
  baudRate_(baudRate),
  dataBits_(dataBits),
  parity_(parity),
  stopBits_(stopBits),
  flowControl_(flowControl),
  ioHandle_(INVALID_HANDLE_VALUE),
  currentState_(STATE_CLOSED)
{
  std::stringstream s;

#ifdef WIN32
  s << DEVICE_STRING << (int)(comPort);
#else
#ifdef Solaris
  s << DEVICE_STRING << ((int)(comPort) + (int)'a');
#else
  s << DEVICE_STRING << (int)(comPort);
#endif
#endif

  comPort_ = s.str();

  open();
}

SockCOM::~SockCOM()
{
  if (isValid())
  {
    close();
  }
}

SockCOMIOHandle SockCOM::getDescriptor() const
{
  return ioHandle_;
}

std::string SockCOM::getCOMPort() const
{
  return comPort_;
}

unsigned int SockCOM::getBaudRate() const
{
  return baudRate_;
}

unsigned int SockCOM::getDataBits() const
{
  return dataBits_;
}

unsigned int SockCOM::getParity() const
{
  return parity_;
}

unsigned int SockCOM::getStopBits() const
{
  return stopBits_;
}

unsigned int SockCOM::getFlowControl() const
{
  return flowControl_;
}

bool SockCOM::isValid() const
{
  return (ioHandle_ != INVALID_HANDLE_VALUE);
}

bool SockCOM::isBlockingIO() const
{
#ifndef WIN32
  // Unix...
  int flags;
  if ((flags = fcntl (ioHandle_, F_GETFL)) < 0)
    return true;
  return !(flags & O_NONBLOCK);
#else
  // Windows...
#endif

  return true;
}

bool SockCOM::setBlockingIO (bool block)
{
#ifndef WIN32
  // Unix...
  int flags;
  if ((flags = fcntl (ioHandle_, F_GETFL)) < 0)
    return false;
  if (block)
    flags = flags & ~O_NONBLOCK;
  else
    flags |= O_NONBLOCK;
  return (fcntl (ioHandle_, F_SETFL) >= 0);
#else
  // Windows...
  return false;
#endif
}

int32_t SockCOM::getLastError() const
{
  return lastError_;
}

void SockCOM::setLastError (int32_t error)
{
  lastError_ = error;
}

bool SockCOM::open()
{
  if (currentState_ != STATE_CLOSED)
    return false;

#ifndef WIN32
  ioHandle_ = ::open (comPort_.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  if (ioHandle_ < 0)
  {
    ioHandle_ = INVALID_HANDLE_VALUE;
    setLastError (errno);
    if (getLastError() == EACCES)
      cerr<<"ERROR: current user needs read/write access on device "<< comPort_.c_str() <<endl;
    return false;
  }

  struct termios my_termios;
  tcgetattr(ioHandle_, &my_termios);

  // Modify settings for for raw data transmission (settings match those used by the pyserial Python module)
  my_termios.c_cflag |=  (CLOCAL|CREAD);
  my_termios.c_iflag &= ~(INLCR|IGNCR|ICRNL|IGNBRK|IUCLC|PARMRK);
  my_termios.c_oflag &= ~(OPOST);
  my_termios.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK|ECHONL|ISIG|IEXTEN);

  // Poll and return what is already present with no wait
  my_termios.c_cc[VTIME] = 0;
  my_termios.c_cc[VMIN] = 0;

  // Set data bits
  my_termios.c_cflag &= ~(CSIZE);         // Clear all size bits
  switch(dataBits_)
  {
    case SOCKCOM_BITS5:
      my_termios.c_cflag |= CS5;
      break;
    case SOCKCOM_BITS6:
      my_termios.c_cflag |= CS6;
      break;
    case SOCKCOM_BITS7:
      my_termios.c_cflag |= CS7;
      break;
    default:
    case SOCKCOM_BITS8:
      my_termios.c_cflag |= CS8;
      break;
  }

  // Set parity - use non-POSIX CMSPAR for Linux and PAREXT for Solaris to enable mark/space parity
  // If CMSPAR was not defined in termios.h we will try to define it ourselves, although this may not
  // work for all Linux systems
  #if defined(Linux) && !defined(CMSPAR)
  #define CMSPAR 010000000000
  #endif
  my_termios.c_iflag &= ~(INPCK|ISTRIP);
  switch(parity_)
  {
  case SOCKCOM_PARITYSPACE:
    my_termios.c_cflag &= ~(PARODD);
#ifdef Linux
    my_termios.c_cflag |=  (PARENB|CMSPAR);
#else
    my_termios.c_cflag |=  (PARENB|PAREXT);
#endif
    break;
  case SOCKCOM_PARITYMARK:
#ifdef Linux
    my_termios.c_cflag |=  (PARENB|PARODD|CMSPAR);
#else
    my_termios.c_cflag |=  (PARENB|PARODD|PAREXT);
#endif
    break;
  case SOCKCOM_PARITYEVEN:
    my_termios.c_cflag &= ~(PARODD);
    my_termios.c_cflag |=  (PARENB);
    break;
  case SOCKCOM_PARITYODD:
    my_termios.c_cflag |= (PARENB|PARODD);
    break;
  default:
  case SOCKCOM_PARITYNONE:
    my_termios.c_cflag &= ~(PARENB|PARODD);
    break;
  }

  // Set stop bits
  switch(stopBits_)
  {
  default:
  case SOCKCOM_STOPBITS1:
    my_termios.c_cflag &= ~CSTOPB;
    break;
  case SOCKCOM_STOPBITS2:
    my_termios.c_cflag |=  CSTOPB;
    break;
  }

  // Intialize flow control settings (enable RTS and DTR lines)
  int lineFlags = TIOCM_DTR|TIOCM_RTS;
  ioctl(ioHandle_, TIOCMBIS, &lineFlags);
  my_termios.c_iflag &= ~(IXON|IXOFF|IXANY);
  my_termios.c_cflag |=  (CRTSCTS);

  // Set flow control
  if(flowControl_ == SOCKCOM_FCXONXOFF)
  {
    // Software flow control
    my_termios.c_iflag |=  (IXON|IXOFF);
  }
  else if(flowControl_ == SOCKCOM_FCRTSCTS)
  {
    // Clear RTS line enable bit
    lineFlags = TIOCM_RTS;
    ioctl(ioHandle_, TIOCMBIC, &lineFlags);
    my_termios.c_cflag |=  (CRTSCTS);
  }

  // Set baud rate
  switch(baudRate_)
  {
  case 115200:
    cfsetospeed(&my_termios, B115200);
    cfsetispeed(&my_termios, B115200);
    break;
  case 57600:
    cfsetospeed(&my_termios, B57600);
    cfsetispeed(&my_termios, B57600);
    break;
  case 38400:
    cfsetospeed(&my_termios, B38400);
    cfsetispeed(&my_termios, B38400);
    break;
  case 19200:
    cfsetospeed(&my_termios, B19200);
    cfsetispeed(&my_termios, B19200);
    break;
  case 9600:
    cfsetospeed(&my_termios, B9600);
    cfsetispeed(&my_termios, B9600);
    break;
  case 4800:
    cfsetospeed(&my_termios, B4800);
    cfsetispeed(&my_termios, B4800);
    break;
  case 2400:
    cfsetospeed(&my_termios, B2400);
    cfsetispeed(&my_termios, B2400);
    break;
  case 1200:
    cfsetospeed(&my_termios, B1200);
    cfsetispeed(&my_termios, B1200);
    break;
  case 600:
    cfsetospeed(&my_termios, B600);
    cfsetispeed(&my_termios, B600);
    break;
  case 300:
    cfsetospeed(&my_termios, B300);
    cfsetispeed(&my_termios, B300);
    break;
  case 110:
    cfsetospeed(&my_termios, B110);
    cfsetispeed(&my_termios, B110);
    break;
  default:
    cfsetospeed(&my_termios, B9600);
    cfsetispeed(&my_termios, B9600);
  }

  tcflush(ioHandle_, TCIFLUSH);
  tcsetattr(ioHandle_, TCSANOW, &my_termios);

#else
  std::string filename = std::string("\\\\.\\") + comPort_;
  ioHandle_ = CreateFile (filename.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
  if (ioHandle_ == (HANDLE)HFILE_ERROR)
  {
    ioHandle_ = INVALID_HANDLE_VALUE;
    setLastError (errno);
    return false;
  }

  COMMTIMEOUTS commTimeout;
  if (!GetCommTimeouts (ioHandle_, &commTimeout))
  {
    setLastError (errno);
    close();
    return false;
  } // endif

  commTimeout.ReadIntervalTimeout = 100;
  commTimeout.ReadTotalTimeoutMultiplier = 0;
  commTimeout.ReadTotalTimeoutConstant = 100;
  commTimeout.WriteTotalTimeoutMultiplier = 100;
  commTimeout.WriteTotalTimeoutConstant = 50;

  if (!SetCommTimeouts (ioHandle_, &commTimeout))
  {
    setLastError (errno);
    close();
    return false;
  }

  DCB devicestate;
  memset(&devicestate, 0, sizeof(devicestate));
  devicestate.DCBlength = sizeof(devicestate);
  if (!GetCommState (ioHandle_, &devicestate))
  {
    setLastError (errno);
    close();
    return false;
  }

  devicestate.ByteSize = dataBits_;

  switch(parity_)
  {
  case SOCKCOM_PARITYSPACE:
    devicestate.fParity = TRUE;
    devicestate.Parity = SPACEPARITY;
    break;
  case SOCKCOM_PARITYMARK:
    devicestate.fParity = TRUE;
    devicestate.Parity = MARKPARITY;
    break;
  case SOCKCOM_PARITYEVEN:
    devicestate.fParity = TRUE;
    devicestate.Parity = EVENPARITY;
    break;
  case SOCKCOM_PARITYODD:
    devicestate.fParity = TRUE;
    devicestate.Parity = ODDPARITY;
    break;
  default:
  case SOCKCOM_PARITYNONE:
    devicestate.fParity = FALSE;
    devicestate.Parity = NOPARITY;
    break;
  }

  switch(stopBits_)
  {
  default:
  case SOCKCOM_STOPBITS1:
    devicestate.StopBits = ONESTOPBIT;
    break;
  case SOCKCOM_STOPBITS2:
    devicestate.StopBits = TWOSTOPBITS;
    break;
  }

  // Default to no flow control (RTS/DTR are always on)
  devicestate.fOutxDsrFlow = FALSE;
  devicestate.fDtrControl = DTR_CONTROL_ENABLE;
  devicestate.fOutxCtsFlow = FALSE;
  devicestate.fRtsControl = RTS_CONTROL_ENABLE;
  devicestate.fOutX = FALSE;
  devicestate.fInX = FALSE;

  if(flowControl_ == SOCKCOM_FCXONXOFF)
  {
    // Software flow control
    devicestate.fOutX = TRUE;
    devicestate.fInX = TRUE;
  }
  else if(flowControl_ == SOCKCOM_FCRTSCTS)
  {
    devicestate.fOutxCtsFlow = TRUE;
    devicestate.fRtsControl = RTS_CONTROL_HANDSHAKE;
  }

  switch(baudRate_)
  {
  case 256000:
    devicestate.BaudRate = CBR_256000;
    break;
  case 128000:
    devicestate.BaudRate = CBR_128000;
    break;
  case 115200:
    devicestate.BaudRate = CBR_115200;
    break;
  case 57600:
    devicestate.BaudRate = CBR_57600;
    break;
  case 56000:
    devicestate.BaudRate = CBR_56000;
    break;
  case 38400:
    devicestate.BaudRate = CBR_38400;
    break;
  case 19200:
    devicestate.BaudRate = CBR_19200;
    break;
  case 14400:
    devicestate.BaudRate = CBR_14400;
    break;
  case 9600:
    devicestate.BaudRate = CBR_9600;
    break;
  case 4800:
    devicestate.BaudRate = CBR_4800;
    break;
  case 2400:
    devicestate.BaudRate = CBR_2400;
    break;
  case 1200:
    devicestate.BaudRate = CBR_1200;
    break;
  case 600:
    devicestate.BaudRate = CBR_600;
    break;
  case 300:
    devicestate.BaudRate = CBR_300;
    break;
  case 110:
    devicestate.BaudRate = CBR_110;
    break;
  default:
    devicestate.BaudRate = CBR_9600;
  }

  if (!SetCommState (ioHandle_, &devicestate))
  {
    setLastError (errno);
    close();
    return false;
  }
#endif

  currentState_ = STATE_OPEN;
  return true;
}

bool SockCOM::close()
{
  if (isValid())
  {
#ifndef WIN32
    ::close (ioHandle_);
#else
    CloseHandle (ioHandle_);
#endif
    ioHandle_ = INVALID_HANDLE_VALUE;
  }

  currentState_ = STATE_CLOSED;
  return true;
}

bool SockCOM::readReady(uint32_t sec, uint32_t usec) const
{
  if (!isValid())
    return false;
#ifndef WIN32
  // UNIX reading
  fd_set fdSet;
  struct timeval timeout = {sec, usec};
  FD_ZERO (&fdSet);
  FD_SET (ioHandle_, &fdSet);
  int result = select (ioHandle_ + 1, &fdSet, NULL, NULL, &timeout);
  if (result == SOCKET_ERROR)
    return true;
  else if (result == 0)
    return false;

  return (FD_ISSET (ioHandle_, &fdSet)) ? true : false;

#else
  // Windows reading
  struct timeval tp;
  gettimeofday (&tp, 0);
  double endTime = tp.tv_sec + sec;
  double enduTime = tp.tv_usec + usec;
  while ((tp.tv_sec < endTime) || ((tp.tv_sec == endTime) && (tp.tv_usec <= enduTime)))
  {
    DWORD errorType;
    COMSTAT comStatus;
    if (ClearCommError (ioHandle_, &errorType, &comStatus))
    {
      if (comStatus.cbInQue > 0)
	return true;
    }
    else // Try to let the client pick up on the error
      return true;
    gettimeofday (&tp, 0);
  }
  return false;
#endif
}

bool SockCOM::writeReady(uint32_t sec, uint32_t usec) const
{
  if (!isValid())
    return false;
#ifndef WIN32
  // UNIX Writing
  fd_set writefd;
  struct timeval timeout={sec,usec};

  FD_ZERO (&writefd);
  FD_SET (ioHandle_, &writefd);
  int32_t result = select (ioHandle_ + 1, NULL, &writefd, NULL, &timeout);

  //For now return true so we try to read and get an error, rather than poll for ever on a bad socket
  if (result == SOCKET_ERROR)
    return true;
  else if (result == 0)
    return false;

  return (FD_ISSET (ioHandle_, &writefd)) ? true : false;
#else
  // Windows writing
  return true;
#endif
}

int32_t SockCOM::read(void* buffer, int32_t size, int32_t flags) const
{
  if (!isValid())
    return -1;

#ifndef WIN32
  // UNIX Writing
  return ::read (ioHandle_, buffer, size);
#else
  // Windows reading
  DWORD numRead = 0;
  if (!ReadFile (ioHandle_, buffer, size, &numRead, NULL))
  {
//     setLastError (errno);
    return -1;
  }
  return numRead;
#endif
}

int32_t SockCOM::write(const void* buffer, int32_t size, int32_t flags) const
{
  if (!isValid())
    return -1;

#ifndef WIN32
  // UNIX Writing
  return ::write (ioHandle_, buffer, size);
#else
  // Windows writing
  DWORD charsWritten;
  if (!WriteFile (ioHandle_, buffer, size, &charsWritten, NULL))
  {
//     setLastError (errno);
    return -1;
  }
  return charsWritten;
#endif

  return 0;
}
