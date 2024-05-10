/* -*- mode: c++ -*- */
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
 * U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 *
 *
 */

#ifndef _BYTESWAP_H_
#define _BYTESWAP_H_

#include <stdio.h>
#include <string.h>
#include <algorithm>
#include "iostreamc"
#include "inttypesc.h"

#if !defined(SIM_LITTLE_ENDIAN) && !defined(SIM_BIG_ENDIAN)
#if defined(X86) || defined(ALPHA) || defined(__x86_64__) || defined(_WIN64)
#define SIM_LITTLE_ENDIAN
#endif
#endif

#ifndef BYTESWAPSHIFT

template <class T>
inline
void swap_bytes(T *const value, const size_t nitems = 1)
{
//   using namespace std;
  for (size_t i = 0; i < nitems; ++i)
  {
    char *ptr = (char*)&value[i]; // treat value as an array of bytes
    size_t size = sizeof(T);
    size_t half_size = size>>1;
    size_t size_1 = size - 1;
    for (size_t i = 0; i < half_size; ++i)
      std::swap(ptr[size_1 - i], ptr[i]);
  }
}

template <class T>
inline
T swap_bytes_return(const T &value, const size_t nitems = 1)
{
  T temp = value;
  swap_bytes(&temp);
  return temp;
}

#ifdef SIM_LITTLE_ENDIAN

template <class T>
inline
void make_little_endian_(T *const value)
{
}

template <class T>
inline
void make_big_endian_(T *const value)
{
  swap_bytes(value);
}

#else

template <class T>
inline
void make_little_endian_(T *const value)
{
  swap_bytes(value);
}

template <class T>
inline
void make_big_endian_(T *const value)
{
}

#endif

#if defined IRIX || defined Linux || defined WIN32
inline
void make_big_endian(char *const val) { make_big_endian_(val); }
#endif
inline
void make_big_endian(int8_t *const val) { make_big_endian_(val); }
inline
void make_big_endian(uint8_t *const val) { make_big_endian_(val); }
inline
void make_big_endian(int16_t *const val) { make_big_endian_(val); }
inline
void make_big_endian(uint16_t *const val) { make_big_endian_(val); }
inline
void make_big_endian(int32_t *const val) { make_big_endian_(val); }
inline
void make_big_endian(uint32_t *const val) { make_big_endian_(val); }
inline
void make_big_endian(int64_t *const val) { make_big_endian_(val); }
inline
void make_big_endian(uint64_t *const val) { make_big_endian_(val); }
inline
void make_big_endian(float *const val) { make_big_endian_(val); }
inline
void make_big_endian(double *const val) { make_big_endian_(val); }

#if defined IRIX || defined Linux || defined WIN32
inline
void make_little_endian(char *const val) { make_little_endian_(val); }
#endif
inline
void make_little_endian(int8_t *const val) { make_little_endian_(val); }
inline
void make_little_endian(uint8_t *const val) { make_little_endian_(val); }
inline
void make_little_endian(int16_t *const val) { make_little_endian_(val); }
inline
void make_little_endian(uint16_t *const val) { make_little_endian_(val); }
inline
void make_little_endian(int32_t *const val) { make_little_endian_(val); }
inline
void make_little_endian(uint32_t *const val) { make_little_endian_(val); }
inline
void make_little_endian(int64_t *const val) { make_little_endian_(val); }
inline
void make_little_endian(uint64_t *const val) { make_little_endian_(val); }
inline
void make_little_endian(float *const val) { make_little_endian_(val); }
inline
void make_little_endian(double *const val) { make_little_endian_(val); }

template <class T>
inline
void make_big_endian(T *const value, const size_t nitems)
{
  for (size_t i = 0; i < nitems; ++i)
  {
    make_big_endian(&value[i]);
  }
}

template <class T>
inline
void make_little_endian(T *const value, const size_t nitems)
{
  for (size_t i = 0; i < nitems; ++i)
  {
    make_little_endian(&value[i]);
  }
}

template <class T>
inline
size_t beread(FILE *stream, T *const val, const size_t nitems = 1)
{
  size_t nitemsread = fread(val, sizeof(T), nitems, stream);
  make_big_endian(val, nitemsread);
  return nitemsread;
}

template <class T>
inline
size_t bewrite(FILE *stream, const T *const val, const size_t nitems = 1)
{
  size_t nitemswrote = 0;
  for (size_t i = 0; i < nitems; ++i)
  {
    T temp = val[i];
    make_big_endian(&temp);
    if (fwrite(&temp, sizeof(T), 1, stream) != 1)
      break;
    ++nitemswrote;
  }
  return nitemswrote;
}

template <class T>
inline
size_t leread(FILE *stream, T *const val, const size_t nitems = 1)
{
  size_t nitemsread = fread(val, sizeof(T), nitems, stream);
  make_little_endian(val, nitemsread);
  return nitemsread;
}

template <class T>
inline
size_t lewrite(FILE *stream, const T *const val, const size_t nitems = 1)
{
  size_t nitemswrote = 0;
  for (size_t i = 0; i < nitems; ++i)
  {
    T temp = val[i];
    make_little_endian(&temp);
    if (fwrite(&temp, sizeof(T), 1, stream) != 1)
      break;
    ++nitemswrote;
  }
  return nitemswrote;
}

template <class T>
inline
size_t beread(istream &stream, T *const val, const std::streamsize nitems = 1)
{
  stream.read((char *)val, nitems * sizeof(T));
  size_t nitemsread = static_cast<size_t>(stream.gcount() / sizeof(T));
  make_big_endian(val, nitemsread);
  return nitemsread;
}

template <class T>
inline
size_t bewrite(ostream &stream, const T *const val, const size_t nitems = 1)
{
  size_t nitemswrote = 0;
  for (size_t i = 0; i < nitems; ++i)
  {
    T temp = val[i];
    make_big_endian(&temp);
    stream.write((const char *)&temp, sizeof(T));
    if (stream.bad())
      break;
    ++nitemswrote;
  }
  return nitemswrote;
}

template <class T>
inline
size_t leread(istream &stream, T *const val, const size_t nitems = 1)
{
  stream.read((char *)val, nitems * sizeof(T));
  size_t nitemsread = static_cast<size_t>(stream.gcount() / sizeof(T));
  make_little_endian(val, nitemsread);
  return nitemsread;
}

template <class T>
inline
size_t lewrite(ostream &stream, const T *const val, const size_t nitems = 1)
{
  size_t nitemswrote = 0;
  for (size_t i = 0; i < nitems; ++i)
  {
    T temp = val[i];
    make_little_endian(&temp);
    stream.write((const char *)&temp, sizeof(T));
    if (stream.bad())
      break;
    ++nitemswrote;
  }
  return nitemswrote;
}

template <class T>
inline
size_t beread(const void *const stream, T *const val, const size_t nitems = 1)
{
  memcpy(val, stream, nitems * sizeof(T));
  make_big_endian(val, nitems);
  return nitems;
}

template <class T>
inline
size_t bewrite(void *const stream, const T *const val, const size_t nitems = 1)
{
  memcpy(stream, val, nitems * sizeof(T));
  make_big_endian((T *)stream, nitems);
  return nitems;
}

template <class T>
inline
size_t leread(const void *const stream, T *const val, const size_t nitems = 1)
{
  memcpy(val, stream, nitems * sizeof(T));
  make_little_endian(val, nitems);
  return nitems;
}

template <class T>
inline
size_t lewrite(void *const stream, const T *const val, const size_t nitems = 1)
{
  memcpy(stream, val, nitems * sizeof(T));
  make_little_endian((T *)stream, nitems);
  return nitems;
}

#else

//Swap macros.  
#define SWAP16(val) ( (((val)>>8)&0xff)|(((val)<<8)&0xff00) )
#define SWAP32(val) ( (((val)>>24)&0xff)|(((val)>>8)&0xff00)|(((val)<<8)&0xff0000)|(((val)<<24)&0xff000000) )
#ifdef __GNUC__
#define SWAP64(val) ( (((val)>>56)&0xffLL)|(((val)>>40)&0xff00LL)|(((val)>>24)&0xff0000LL)|(((val)>>8)&0xff000000LL)| \
                      (((val)<<8)&0xff00000000LL)|(((val)<<24)&0xff0000000000LL)|(((val)<<40)&0xff000000000000LL)|(((val)<<56)&0xff00000000000000LL) )
#else
#define SWAP64(val) ( (((val)>>56)&0xffL)|(((val)>>40)&0xff00L)|(((val)>>24)&0xff0000L)|(((val)>>8)&0xff000000L)| \
                      (((val)<<8)&0xff00000000L)|(((val)<<24)&0xff0000000000L)|(((val)<<40)&0xff000000000000L)|(((val)<<56)&0xff00000000000000L) )
#endif

//Generic byte swapping routines.  
template<class T> inline void swap_bytes(T *const value)
{
  char *ptr=(char*)(void*)value; // treat value as an array of bytes
  size_t size=sizeof(T);
  register size_t half_size=size>>1;
  register size_t size_1=size-1;
  for(register size_t i=0;i<half_size;++i)
    std::swap(ptr[size_1-i],ptr[i]);
}

template<class T> inline void swap_bytes(T *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i)
  {
    char *ptr=(char*)(void*)&value[i]; // treat value as an array of bytes
    size_t size=sizeof(T);
    register size_t half_size=size>>1;
    register size_t size_1=size-1;
    for(register size_t i=0;i<half_size;++i)
      std::swap(ptr[size_1-i],ptr[i]);
  }
}


//Specialized routines.  

//NULL routines.  
#if defined IRIX || defined Linux || defined WIN32
template<> inline void swap_bytes(char *const value) {}
template<> inline void swap_bytes(char *const value,register const size_t nitems) {}
#endif
template<> inline void swap_bytes(int8_t *const value) {}
template<> inline void swap_bytes(uint8_t *const value) {}
template<> inline void swap_bytes(int8_t *const value,register const size_t nitems) {}
template<> inline void swap_bytes(uint8_t *const value,register const size_t nitems) {}

//Single item routines.  
template<> inline void swap_bytes(int16_t *const value) { *value=SWAP16(*value); }
template<> inline void swap_bytes(uint16_t *const value) { *value=SWAP16(*value); }
template<> inline void swap_bytes(int32_t *const value) { *value=SWAP32(*value); }
template<> inline void swap_bytes(uint32_t *const value) { *value=SWAP32(*value); }
template<> inline void swap_bytes(int64_t *const value) { *value=SWAP64(*value); }
template<> inline void swap_bytes(uint64_t *const value) { *value=SWAP64(*value); }

template<> inline void swap_bytes(float *const value)
{
  int32_t *const pseudo=(int32_t*)(void*)value;
  *pseudo=SWAP32(*pseudo);
}

template<> inline void swap_bytes(double *const value)
{
  int64_t *const pseudo=(int64_t*)(void*)value;
  *pseudo=SWAP64(*pseudo);
}

#ifdef SIM_LITTLE_ENDIAN
template<class T> inline void make_little_endian(T *const value) {}

//Generic swap.  Has a '_' prefix to prevent it from accidently being used with structs, etc.  
template<class T> inline void make_little_endian_(T *const value) {}
template<class T> inline void make_big_endian_(T *const value) { swap_bytes(value); }

#if defined IRIX || defined Linux || defined WIN32
inline void make_big_endian(char *const value) {}
#endif
inline void make_big_endian(int8_t *const value) {}
inline void make_big_endian(uint8_t *const value) {}
inline void make_big_endian(int16_t *const value) { *value=SWAP16(*value); }
inline void make_big_endian(uint16_t *const value) { *value=SWAP16(*value); }
inline void make_big_endian(int32_t *const value) { *value=SWAP32(*value); }
inline void make_big_endian(uint32_t *const value) { *value=SWAP32(*value); }
inline void make_big_endian(int64_t *const value) { *value=SWAP64(*value); }
inline void make_big_endian(uint64_t *const value) { *value=SWAP64(*value); }

inline void make_big_endian(float *const value)
{
  int32_t *const pseudo=(int32_t*)(void*)value;
  *pseudo=SWAP32(*pseudo);
}

inline void make_big_endian(double *const value)
{
  int64_t *const pseudo=(int64_t*)(void*)value;
  *pseudo=SWAP64(*pseudo);
}
#else
template<class T> inline void make_big_endian(T *const value) {}
template<class T> inline void make_big_endian_(T *const value) {}
template<class T> inline void make_little_endian_(T *const value) { swap_bytes(value); }

#if defined IRIX || defined Linux || defined WIN32
inline void make_little_endian(char *const value) {}
#endif
inline void make_little_endian(int8_t *const value) {}
inline void make_little_endian(uint8_t *const value) {}
inline void make_little_endian(int16_t *const value) { *value=SWAP16(*value); }
inline void make_little_endian(uint16_t *const value) { *value=SWAP16(*value); }
inline void make_little_endian(int32_t *const value) { *value=SWAP32(*value); }
inline void make_little_endian(uint32_t *const value) { *value=SWAP32(*value); }
inline void make_little_endian(int64_t *const value) { *value=SWAP64(*value); }
inline void make_little_endian(uint64_t *const value) { *value=SWAP64(*value); }

inline void make_little_endian(float *const value)
{
  int32_t *const pseudo=(int32_t*)(void*)value;
  *pseudo=SWAP32(*pseudo);
}

inline void make_little_endian(double *const value)
{
  int64_t *const pseudo=(int64_t*)(void*)value;
  *pseudo=SWAP64(*pseudo);
}
#endif

//Multi item routines.  
template<> inline void swap_bytes(int16_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP16(value[i]);
}

template<> inline void swap_bytes(uint16_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP16(value[i]);
}

template<> inline void swap_bytes(int32_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP32(value[i]);
}

template<> inline void swap_bytes(uint32_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP32(value[i]);
}

template<> inline void swap_bytes(int64_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP64(value[i]);
}

template<> inline void swap_bytes(uint64_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP64(value[i]);
}

template<> inline void swap_bytes(float *const value,register const size_t nitems)
{
  int32_t *const pseudo=(int32_t*)(void*)value;
  for(register size_t i=0;i<nitems;++i) pseudo[i]=SWAP32(pseudo[i]);
}

template<> inline void swap_bytes(double *const value,register const size_t nitems)
{
  int64_t *const pseudo=(int64_t*)(void*)value;
  for(register size_t i=0;i<nitems;++i) pseudo[i]=SWAP64(pseudo[i]);
}

#ifdef SIM_LITTLE_ENDIAN
template<class T> inline void make_little_endian(T *const value,register const size_t nitems) {}
template<class T> inline void make_little_endian_(T *const value,register const size_t nitems) {}
template<class T> inline void make_big_endian_(T *const value,register const size_t nitems) { swap_bytes(value,nitems); }

#if defined IRIX || defined Linux || defined WIN32
inline void make_big_endian(char *const value,register const size_t nitems) {}
#endif
inline void make_big_endian(int8_t *const value,register const size_t nitems) {}
inline void make_big_endian(uint8_t *const value,register const size_t nitems) {}

inline void make_big_endian(int16_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP16(value[i]);
}

inline void make_big_endian(uint16_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP16(value[i]);
}

inline void make_big_endian(int32_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP32(value[i]);
}

inline void make_big_endian(uint32_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP32(value[i]);
}

inline void make_big_endian(int64_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP64(value[i]);
}

inline void make_big_endian(uint64_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP64(value[i]);
}

inline void make_big_endian(float *const value,register const size_t nitems)
{
  int32_t *const pseudo=(int32_t*)(void*)value;
  for(register size_t i=0;i<nitems;++i) pseudo[i]=SWAP32(pseudo[i]);
}

inline void make_big_endian(double *const value,register const size_t nitems)
{
  int64_t *const pseudo=(int64_t*)(void*)value;
  for(register size_t i=0;i<nitems;++i) pseudo[i]=SWAP64(pseudo[i]);
}
#else
template<class T> inline void make_big_endian(T *const value,register const size_t nitems) {}
template<class T> inline void make_big_endian_(T *const value,register const size_t nitems) {}
template<class T> inline void make_little_endian_(T *const value,register const size_t nitems) { swap_bytes(value,nitems); }

#if defined IRIX || defined Linux || defined WIN32
inline void make_little_endian(char *const value,register const size_t nitems) {}
#endif
inline void make_little_endian(int8_t *const value,register const size_t nitems) {}
inline void make_little_endian(uint8_t *const value,register const size_t nitems) {}

inline void make_little_endian(int16_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP16(value[i]);
}

inline void make_little_endian(uint16_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP16(value[i]);
}

inline void make_little_endian(int32_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP32(value[i]);
}

inline void make_little_endian(uint32_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP32(value[i]);
}

inline void make_little_endian(int64_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP64(value[i]);
}

inline void make_little_endian(uint64_t *const value,register const size_t nitems)
{
  for(register size_t i=0;i<nitems;++i) value[i]=SWAP64(value[i]);
}

inline void make_little_endian(float *const value,register const size_t nitems)
{
  int32_t *const pseudo=(int32_t*)(void*)value;
  for(register size_t i=0;i<nitems;++i) pseudo[i]=SWAP32(pseudo[i]);
}

inline void make_little_endian(double *const value,register const size_t nitems)
{
  int64_t *const pseudo=(int64_t*)(void*)value;
  for(register size_t i=0;i<nitems;++i) pseudo[i]=SWAP64(pseudo[i]);
}
#endif


//Swapped I\O
template <class T> inline size_t beread(FILE *stream,T *const val)
{
  size_t nitemsread=fread(val,sizeof(T),1,stream);
  make_big_endian(val);
  return nitemsread;
}

template <class T> inline size_t beread(FILE *stream,T *const val,register const size_t nitems)
{
  size_t nitemsread=fread(val,sizeof(T),nitems,stream);
  make_big_endian(val,nitemsread);
  return nitemsread;
}

template <class T> inline size_t bewrite(FILE *stream,const T *const val)
{
  T temp=*val;
  make_big_endian(&temp);
  return fwrite(&temp,sizeof(T),1,stream);
}

template <class T> inline size_t bewrite(FILE *stream,const T *const val,register const size_t nitems)
{
  register size_t nitemswrit=0;
  while(nitemswrit<nitems)
  {
    T temp=val[nitemswrit++];
    make_big_endian(&temp);
    if(fwrite(&temp,sizeof(T),1,stream)!=1)
      break;
  }
  return nitemswrit;
}

template <class T> inline size_t leread(FILE *stream,T *const val)
{
  size_t nitemsread=fread(val,sizeof(T),1,stream);
  make_little_endian(val);
  return nitemsread;
}

template <class T> inline size_t leread(FILE *stream,T *const val,register const size_t nitems)
{
  size_t nitemsread=fread(val,sizeof(T),nitems,stream);
  make_little_endian(val,nitemsread);
  return nitemsread;
}

template <class T> inline size_t lewrite(FILE *stream,const T *const val)
{
  T temp=*val;
  make_little_endian(&temp);
  return fwrite(&temp,sizeof(T),1,stream);
}

template <class T> inline size_t lewrite(FILE *stream,const T *const val,register const size_t nitems)
{
  register size_t nitemswrit=0;
  while(nitemswrit<nitems)
  {
    T temp=val[nitemswrit++];
    make_little_endian(&temp);
    if(fwrite(&temp,sizeof(T),1,stream)!=1)
      break;
  }
  return nitemswrit;
}

template <class T> inline size_t beread(istream &stream,T *const val)
{
  stream.read((char *)val,sizeof(T));
  size_t nitemsread=stream.gcount()/sizeof(T);
  make_big_endian(val);
  return nitemsread;
}

template <class T> inline size_t beread(istream &stream,T *const val,register const size_t nitems)
{
  stream.read((char *)val,nitems*sizeof(T));
  size_t nitemsread=stream.gcount()/sizeof(T);
  make_big_endian(val,nitemsread);
  return nitemsread;
}

template <class T> inline size_t bewrite(ostream &stream,const T *const val)
{
  T temp=*val;
  make_big_endian(&temp);
  stream.write((const char *)&temp,sizeof(T));
  return (stream.bad())?0:1;
}

template <class T> inline size_t bewrite(ostream &stream,const T *const val,register const size_t nitems)
{
  register size_t nitemswrit=0;
  while(nitemswrit<nitems)
  {
    T temp = val[nitemswrit++];
    make_big_endian(&temp);
    stream.write((const char *)&temp,sizeof(T));
    if (stream.bad())
      break;
  }
  return nitemswrit;
}

template <class T> inline size_t leread(istream &stream,T *const val)
{
  stream.read((char *)val,sizeof(T));
  size_t nitemsread=stream.gcount()/sizeof(T);
  make_little_endian(val);
  return nitemsread;
}

template <class T> inline size_t leread(istream &stream,T *const val,register const size_t nitems)
{
  stream.read((char *)val,nitems*sizeof(T));
  size_t nitemsread=stream.gcount()/sizeof(T);
  make_little_endian(val,nitemsread);
  return nitemsread;
}

template <class T> inline size_t lewrite(ostream &stream,const T *const val)
{
  T temp=*val;
  make_little_endian(&temp);
  stream.write((const char *)&temp,sizeof(T));
  return (stream.bad())?0:1;
}

template <class T> inline size_t lewrite(ostream &stream,const T *const val,register const size_t nitems)
{
  size_t nitemswrit=0;
  while(nitemswrit<nitems)
  {
    T temp=val[nitemswrit++];
    make_little_endian(&temp);
    stream.write((const char *)&temp,sizeof(T));
    if (stream.bad())
      break;
  }
  return nitemswrit;
}

template <class T> inline size_t beread(const void *const stream,T *const val)
{
  memcpy(val,stream,sizeof(T));
  make_big_endian(val);
  return 1;
}

template <class T> inline size_t beread(const void *const stream,T *const val,register const size_t nitems)
{
  memcpy(val,stream,nitems*sizeof(T));
  make_big_endian(val,nitems);
  return nitems;
}

template <class T> inline size_t bewrite(void *const stream,const T *const val)
{
  memcpy(stream,val,sizeof(T));
  make_big_endian((T *)stream);
  return 1;
}

template <class T> inline size_t bewrite(void *const stream,const T *const val,register const size_t nitems)
{
  memcpy(stream,val,nitems*sizeof(T));
  make_big_endian((T *)stream,nitems);
  return nitems;
}

template <class T> inline size_t leread(const void *const stream, T *const val)
{
  memcpy(val,stream,sizeof(T));
  make_little_endian(val);
  return 1;
}

template <class T> inline size_t leread(const void *const stream, T *const val,register const size_t nitems)
{
  memcpy(val,stream,nitems*sizeof(T));
  make_little_endian(val,nitems);
  return nitems;
}

template <class T> inline size_t lewrite(void *const stream,const T *const val)
{
  memcpy(stream,val,sizeof(T));
  make_little_endian((T *)stream);
  return 1;
}

template <class T> inline size_t lewrite(void *const stream,const T *const val,register const size_t nitems)
{
  memcpy(stream,val,nitems*sizeof(T));
  make_little_endian((T *)stream,nitems);
  return nitems;
}

#endif

// backwards-compatability
// template <class T> inline void FIX_BIG_ENDIAN(T &foo) { make_big_endian(&foo, 1); }
// template <class T> inline void FIX_BIG_ENDIAN(T *foo, const size_t nelems) { make_big_endian(foo, nelems); }
// #define FIX_BIG_ENDIAN_ARRAY FIX_BIG_ENDIAN

#endif
