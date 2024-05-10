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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "net.h"
#include "DCSDiscover.h"

//These values are fixed.  They can not be specified by a server or client.  
#define ADDRESS "227.4.4.25"
#define REQUESTPORT 25252
#define RESPONDPORT 26262
#define SOLICITATION "HELO_DCS_SERVER"
#define ANNOUNCEMENT "HELO_DCS_CLIENT"

bool DCSDisc::discoverServInit(DCSDiscover& info,uint16_t port,const char* addr,const char* msg,TTL_T level,const char* iface)
{
  if(addr==NULL||addr[0]=='\0')
    return false;
  
  info.request=new SockMulticast(REQUESTPORT,ADDRESS,iface,level);
  if(!info.request->isValid())
    return false;

  info.respond=new SockMulticast(RESPONDPORT,ADDRESS,iface,level);
  if(!info.respond->isValid())
    return false;

  //Request is read only
  info.request->shutdown(SHUT_WR);

  //Respond is write only
  info.respond->shutdown(SHUT_RD);
  
  memset(info.addr,0,sizeof(info.addr));
  memset(info.msg,0,sizeof(info.msg));

  info.port=port;
  strncpy(info.addr,addr,sizeof(info.addr)-1);

  if(msg!=NULL)
    strncpy(info.msg,msg,sizeof(info.msg)-1);

  return true;
}

bool DCSDisc::discoverClientInit(DCSDiscover& info,TTL_T level,const char* iface)
{
  info.request=new SockMulticast(REQUESTPORT,ADDRESS,iface,level);
  if(!info.request->isValid())
    return false;

  info.respond=new SockMulticast(RESPONDPORT,ADDRESS,iface,level);
  if(!info.request->isValid())
    return false;

  //Request is write only
  info.request->shutdown(SHUT_RD);

  //Respond is read only
  info.respond->shutdown(SHUT_WR);

  info.port=0;
  memset(info.addr,0,sizeof(info.addr));
  memset(info.msg,0,sizeof(info.msg));

  return true;
}

void DCSDisc::discoverFree(DCSDiscover& info)
{
  if(info.request!=NULL)
  {
    delete info.request;
    info.request=NULL;
  }

  if(info.respond!=NULL)
  {
    delete info.respond;
    info.respond=NULL;
  }
}

bool DCSDisc::discoverAnnounce(DCSDiscover& info)
{
  char buffer[168];
  if(info.respond==NULL||!info.respond->isValid())
    return false;

  sprintf(buffer,"%s %s %u %s",ANNOUNCEMENT,info.addr,info.port,info.msg);

  return (info.respond->write(buffer,int32_t(strlen(buffer)+1))==SOCKET_ERROR)?false:true;
}

bool DCSDisc::discoverRespond(DCSDiscover& info)
{
  char buffer[24];

  if(info.request==NULL||!info.request->isValid())
    return false;

  int32_t total=info.request->read(buffer,sizeof(buffer));
  if(total==SOCKET_ERROR)
    return false;
  
  if(strncmp(SOLICITATION,buffer,strlen(SOLICITATION))!=0)
    return false;
  
  return discoverAnnounce(info);
}

bool DCSDisc::discoverRequest(DCSDiscover& info)
{
  char buffer[24];
  if(info.request==NULL||!info.request->isValid())
    return false;
  
  sprintf(buffer,"%s",SOLICITATION);

  return (info.request->write(buffer,int32_t(strlen(buffer)+1))==SOCKET_ERROR)?false:true;
}

bool DCSDisc::discoverReceive(DCSDiscover& info)
{
  char buffer[148],*begin,*end;
  if(info.respond==NULL||!info.respond->isValid())
    return false;

  if(info.respond->read(buffer,sizeof(buffer))==SOCKET_ERROR)
    return false;

  if((end=strchr(buffer,' '))==NULL)
    return false;
  
  *end='\0';

  if(strcmp(buffer,ANNOUNCEMENT)!=0)
    return false;

  begin=end+1;
  if((end=strchr(begin,' '))==NULL)
    return false;

  *end='\0';
  
  strncpy(info.addr,begin,sizeof(info.addr)-1);
  
  begin=end+1;
  if((end=strchr(begin,' '))==NULL)
    return false;
  
  if(!isdigit(*begin))
    return false;

  *end='\0';

  info.port=atoi(begin);

  begin=end+1;
  strncpy(info.msg,begin,sizeof(info.msg)-1);  

  return true;
}
