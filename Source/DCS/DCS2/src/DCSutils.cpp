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

#include "stringc.h"
#include "stdioc.h"
#include "DCSutils.h"

namespace DCS
{
  char int64totextbuf[64];
  bool istzset = false;
}

char* DCS::uint64totext(uint64_t id,char* buffer,uint32_t len,uint32_t base)
{
  char* buf=(buffer!=NULL)?buffer:int64totextbuf;
  int32_t size=(buffer!=NULL)?len:sizeof(int64totextbuf);
  uint16_t upper1=(uint16_t)((id>>48)&0xFFFF);
  uint16_t upper2=(uint16_t)((id>>32)&0xFFFF);
  uint16_t lower1=(uint16_t)((id>>16)&0xFFFF);
  uint16_t lower2=(uint16_t)(id&0xFFFF);

  if(base==16)
    snprintf(buf,size,"0x%04x 0x%04x 0x%04x 0x%04x",(int)upper1,(int)upper2,(int)lower1,(int)lower2);
  else
    snprintf(buf,size,"%u %u %u %u",upper1,upper2,lower1,lower2);
  return buf;
}

char* DCS::uint64totext(uint64_t id,uint32_t base)
{
  return uint64totext(id,NULL,0,base);
}

uint64_t DCS::make_id(uint16_t hihi,uint16_t hilo,uint16_t lohi,uint16_t lolo)
{
  return ((((uint64_t)hihi)<<48)/*&0xFFFF000000000000*/)|((((uint64_t)hilo)<<32)/*&0x0000FFFF00000000*/)|((((uint64_t)lohi)<<16)/*&0x00000000FFFF0000*/)|((uint64_t)lolo/*&0x000000000000FFFF*/);
}

void DCS::break_id(uint64_t id,uint16_t* hihi,uint16_t* hilo,uint16_t* lohi,uint16_t* lolo)
{
  *hihi=(uint16_t)((id>>48)&0xFFFF);
  *hilo=(uint16_t)((id>>32)&0xFFFF);
  *lohi=(uint16_t)((id>>16)&0xFFFF);
  *lolo=(uint16_t)(id&0xFFFF);
}

uint16_t DCS::getyear()
{
  time_t t=time(NULL);
  struct tm* gmt=gmtime(&t);
  return (uint16_t)(gmt->tm_year+1900);
}

double DCS::getyeartime()
{
  timeb tb;
  time_t t=time(NULL);
  struct tm* gmt=gmtime(&t);
  if(!istzset)
  {
    tzset();
    istzset = true;
  }
  gmt->tm_isdst=gmt->tm_mon=gmt->tm_min=gmt->tm_hour=0;  
  gmt->tm_sec=-_timezone;
  gmt->tm_mday=1;

  ftime(&tb);
  return (((double)(tb.time-mktime(gmt)))+((double)tb.millitm/1000.0));
}

double DCS::getyeartime(int16_t year)
{
  timeb tb;
  time_t t=time(NULL);
  struct tm* gmt=gmtime(&t);
  if(!istzset)
  {
    tzset();
    istzset = true;
  }
  gmt->tm_isdst=gmt->tm_mon=gmt->tm_min=gmt->tm_hour=0;
  gmt->tm_sec=-_timezone;
  gmt->tm_mday=1;
  gmt->tm_year=year-1900;

  ftime(&tb);
  return (((double)(tb.time-mktime(gmt)))+((double)tb.millitm/1000.0));
}

int DCS::getdefaultaddr(char* addr,uint32_t addrlen)
{
  char hname[BUFSIZ];
  struct hostent* hent;
  int result=SOCKET_ERROR;
  
  if(gethostname(hname,BUFSIZ)!=SOCKET_ERROR)
  {
    hent=gethostbyname(hname);
    if(hent)
    {
      int32_t i_addr;
      char *s,*p_addr=(char*)&i_addr;
      struct in_addr in;
      p_addr[0]=hent->h_addr_list[0][0];
      p_addr[1]=hent->h_addr_list[0][1];
      p_addr[2]=hent->h_addr_list[0][2];
      p_addr[3]=hent->h_addr_list[0][3];
      in.s_addr=i_addr;
      s=inet_ntoa(in);
      if(s!=NULL&&strlen(s)<addrlen)
      {
        strncpy(addr,s,addrlen-1);
        result=0;
      }
      else
      {
        setSockErr(PLATERR(EFAULT));
      }
    }
  }

  return result;
}

uint32_t DCS::getClassificationColor (DCSClassification_t classification)
{
  switch (classification)
  {
    case DCSCLASS_OFFICIALUSEONLY:
      return 0x80ffffff;
    case DCSCLASS_CONFIDENTIAL:
      return 0x80ffff00;
    case DCSCLASS_SECRET:
      return 0x800000ff;
    case DCSCLASS_SECRETWN:
      return 0x800000ff;
    case DCSCLASS_SECRETNF:
      return 0x800000ff;
    case DCSCLASS_SECRETNC:
      return 0x800000ff;
    case DCSCLASS_SECRETOC:
      return 0x800000ff;
    case DCSCLASS_SECRETPR:
      return 0x800000ff;
    case DCSCLASS_TOPSECRET:
      return 0x800080ff;
    case DCSCLASS_UNCLASSIFIED:
      break;
  }
  return 0x8000ff00;
}

const char* DCS::getClassificationString (DCSClassification_t classification)
{
  switch (classification)
  {
    case DCSCLASS_OFFICIALUSEONLY:
      return "FOR OFFICIAL USE ONLY";
    case DCSCLASS_CONFIDENTIAL:
      return "CONFIDENTIAL";
    case DCSCLASS_SECRET:
      return "SECRET";
    case DCSCLASS_SECRETWN:
      return "SECRET - WN";
    case DCSCLASS_SECRETNF:
      return "SECRET - NF";
    case DCSCLASS_SECRETNC:
      return "SECRET - NC";
    case DCSCLASS_SECRETOC:
      return "SECRET - OC";
    case DCSCLASS_SECRETPR:
      return "SECRET - PR";
    case DCSCLASS_TOPSECRET:
      return "TOP SECRET";
    case DCSCLASS_UNCLASSIFIED:
      break;
  }
  return "UNCLASSIFIED";
}

const char* DCS::getCategoryValue (DCSAllegience_t alleg)
{
  switch (alleg)
  {
    case DCSALLEGIENCE_FRIENDLY:
      return "Friendly";
    case DCSALLEGIENCE_HOSTILE:
      return "Hostile";
    case DCSALLEGIENCE_NEUTRAL:
    case DCSALLEGIENCE_CATEGORY:
      break;
  }
  return "Neutral";
}

const char* DCS::getCategoryValue (DCSPlatformType_t platType)
{
  switch (platType)
  {
    case DCSPLATFORM_SURFACESHIP:
      return "Surface Ship";
    case DCSPLATFORM_SUBMARINE:
      return "Submarine";
    case DCSPLATFORM_AIRCRAFT:
      return "Aircraft";
    case DCSPLATFORM_SATELLITE:
      return "Satellite";
    case DCSPLATFORM_HELICOPTER:
      return "Helicopter";
    case DCSPLATFORM_MISSILE:
      return "Missile";
    case DCSPLATFORM_DECOY:
      return "Decoy";
    case DCSPLATFORM_BUOY:
      return "Buoy";
    case DCSPLATFORM_REFERENCE:
      return "Reference Site";
    case DCSPLATFORM_MOBILELANDSITE:
      return "Land Vehicle";
    case DCSPLATFORM_LANDSITE:
      return "Land Site";
    case DCSPLATFORM_TORPEDO:
      return "Torpedo";
    case DCSPLATFORM_CONTACT:
      return "Contact";
    case DCSPLATFORM_UNKNOWN:
    case DCSPLATFORM_CATEGORY:
      break;
  }
  return "Unknown";
}
