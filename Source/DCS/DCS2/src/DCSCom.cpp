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

#include <time.h>
#include <vector>
#include <map>
#include "net.h"
#include "stdlibc.h"
#include "stdioc.h"
#include "inttypesc.h"
#include "DCSswapbytes.h"
#include "DCSdefs.h"
#include "DCSPoint.h"
#include "DCSBaseData.h"
#include "DCSBaseHeader.h"
#include "DCSPlatformData.h"
#include "DCSBeamData.h"
#include "DCSGateData.h"
#include "DCSScopeData.h"
#include "DCSTimeData.h"
#include "DCSScenarioData.h"
#include "DCSPlatformHeader.h"
#include "DCSBeamHeader.h"
#include "DCSGateHeader.h"
#include "DCSTimeHeader.h"
#include "DCSScenarioHeader.h"
#include "DCSEvent.h"
#include "DCSGenericData.h"
#include "DCSHeaderContainer.h"
#include "DCSCategoryData.h"
#include "DCSCom.h"
#include "DCSutils.h"
#include "Constants/ConstantsVariableType.h"

char* DCSCom::getServerID(const DCSServerInfo& info,char* id,int32_t len)
{
  memcpy(id,info.server_id,DCS_SERVERIDSZ);
  return id;
}

char* DCSCom::getServerID(const DCSClientInfo& info,char* id,int32_t len)
{
  memcpy(id,info.server_id,DCS_SERVERIDSZ);
  return id;
}

char* DCSCom::getServerIDString(const DCSServerInfo& info,char* str,int32_t len)
{
  int32_t i=0;
  char hex[3];
  snprintf(str,len,"%02X",(uint8_t)info.server_id[i++]);
  len-=3;  //2 characters plus '\0'

  while(i<DCS_SERVERIDSZ&&len>0)
  {
    snprintf(hex,3,"%02X",(uint8_t)info.server_id[i++]);
    strncat(str,hex,len);
    len-=2;
  }
  return str;
}

char* DCSCom::getServerIDString(const DCSClientInfo& info,char* str,int32_t len)
{
  int32_t i=0;
  char hex[3];
  snprintf(str,len,"%02X",(uint8_t)info.server_id[i++]);
  len-=3;  //2 characters plus '\0'

  while(i<DCS_SERVERIDSZ&&len>0)
  {
    snprintf(hex,3,"%02X",(uint8_t)info.server_id[i++]);
    strncat(str,hex,len);
    len-=2;
  }
  return str;
}

//Error handling - test to make sure valid sockets were created
bool DCSCom::serverStart(DCSServerInfo& info,uint16_t servport,int8_t datamode,uint16_t dataport,const char* dataaddr,const char* iface,TTL_T ttl)
{
  switch(datamode)
  {
    case DCSDATA_UNCONNECTED:
    default:
      info.datatype=DCSDATA_UNCONNECTED;
      info.datasock=NULL;
      info.servsock=NULL;
      return false;
    case DCSDATA_UNICAST:
      if(info.datasock!=NULL)
	delete info.datasock;
      info.datasock=new SockUnicast(EPHEMERAL_PORT,iface);
      break;
    case DCSDATA_BROADCAST:
      if(info.datasock!=NULL)
	delete info.datasock;
      info.datasock=new SockBroadcast();//EPHEMERAL_PORT,iface); You don't usually specify the interface.  It can be determined through the address.
      info.datasock->attach(dataport,dataaddr);
      break;
    case DCSDATA_MULTICAST:
      if(info.datasock!=NULL)
	delete info.datasock;
      info.datasock=new SockMulticast(dataport,dataaddr,iface,ttl);
      break;
  }
  info.serverVersion = DCS_VERSION;

  //Make sure our data socket was succesfully constructed.
  if(!info.datasock->isValid())
  {
    delete info.datasock;
    info.datasock=NULL;
    info.datatype=DCSDATA_UNCONNECTED;
    return false;
  }

  //We don't read on this socket
  info.datasock->shutdown(SHUT_RD);
  info.dataport=dataport;
  info.datatype=datamode;

  if(info.servsock!=NULL)
    delete info.servsock;
  info.servsock=new SockServer(servport,iface);
  if(!info.servsock->isValid())
  {
    delete info.servsock;
    delete info.datasock;
    info.servsock=NULL;
    info.datasock=NULL;
    info.datatype=DCSDATA_UNCONNECTED;
    info.dataport=EPHEMERAL_PORT;
    return false;
  }

  //Make our server_id
  int32_t addr=(int32_t)time(NULL);
  if(iface==NULL||iface[0]=='\0'||strcmp(iface,"127.0.0.1")==0)
  {
    //Get primary IP
    char hostname[BUFSIZ];
    if(gethostname(hostname,BUFSIZ)!=SOCKET_ERROR)
    {
      struct hostent *he=gethostbyname(hostname);
      if(he!=NULL)
        addr=*((int32_t*)he->h_addr_list[0]);
    }
  }

  int32_t tm=(int32_t)time(NULL);

  //Make sure all data is in network byte order
  make_big_endian(&servport);
  make_big_endian(&tm);

  memcpy(info.server_id,&addr,4);
  memcpy((info.server_id+4),&servport,2);
  memcpy((info.server_id+6),&tm,4);

  return true;
}

void DCSCom::serverStop(DCSServerInfo& info)
{
  if(info.datasock!=NULL)
  {
    delete info.datasock;
    info.datasock=NULL;
    info.datatype=DCSDATA_UNCONNECTED;
    info.dataport=EPHEMERAL_PORT;
  }

  if(info.servsock!=NULL)
  {
    delete info.servsock;
    info.servsock=NULL;
  }

  //Get rid of all of the clients
  serverTerminateAllClients(info);
//  while(!info.clientlist.empty())
//  {
//    SockClient* target=info.clientlist.back();
//    info.clientlist.pop_back();
//    delete target;
//  }
}

//Error handling - Possibly check to make certain client sockets are not already created
SockClient* DCSCom::serverAccept(const DCSServerInfo& info)
{
  SockClient* client=NULL;
  SOCKET sockfd=INVALID_SOCKET;
  for(;;)
  {
    sockfd=info.servsock->accept();
    if(sockfd==INVALID_SOCKET&&info.servsock->getLastError()==PLATERR(EINTR))
      continue;
    break;
  }

  if(sockfd!=INVALID_SOCKET)
  {
    int32_t flag=1;
    client=new SockClient(sockfd);
    if(!client->isValid())
    {
      delete client;
      client=NULL;
    }
    else
      client->setSockOpt(SOL_SOCKET,SO_KEEPALIVE,&flag,sizeof(flag));
  }

  return client;
}

//If there is a problem should client connection be terminated inside this function?
bool DCSCom::serverNegotiate(DCSServerInfo& info,SockClient* client,const DCSTimeHeader& time,const DCSScenarioHeader& scenario,const DCSHeaderContainer& headers)
{
  int8_t byte;

  //First test to make sure this is valid.
  if(client==NULL||!client->isValid())
    return false;

  //First get connection request
  if(serverReadRequest(client)!=DCSREQUEST_CONNECT)
    return false;

  //Acknowledge
  byte=DCSREQUEST_CONNECT;
  if(client->write(&byte,sizeof(byte))==SOCKET_ERROR)
    return false;

  //Get version request
  if(serverReadRequest(client)!=DCSREQUEST_VERSION)
    return false;

  //Send version
  byte=((0xFF00&DCS_VERSION)>>8);
  if(client->write(&byte,sizeof(byte))==SOCKET_ERROR)
    return false;
  byte=0xFF&DCS_VERSION;
  if(client->write(&byte,sizeof(byte))==SOCKET_ERROR)
    return false;

  //Get data type request.
  if(serverReadRequest(client)!=DCSREQUEST_DATAMODE)
    return false;
  if(!serverSendDataParams(info,client))
    return false;

  //Send the server id to be used as a data filter
  if(client->write(info.server_id,DCS_SERVERIDSZ)==SOCKET_ERROR)
    return false;

  //Send time
  if(serverReadRequest(client)!=DCSREQUEST_TIME)
    return false;
  if(!serverSendTime(client,time))
    return false;

  //Send scenario
  if(serverReadRequest(client)!=DCSREQUEST_SCENARIO)
    return false;
  if(!serverSendScenario(client,scenario))
    return false;

  //Send headers
  if(serverReadRequest(client)!=DCSREQUEST_HEADERS)
    return false;
  if(!serverSendHeaders(client,headers))
    return false;

  info.clientlist.push_back(client);
  return true;
}

bool DCSCom::serverSendDataParams(const DCSServerInfo& info,const SockClient* client)
{
  int8_t byte,mode;
  uint16_t port;
  char addr[DCS_ADDRSTRLEN];

  //First test to make sure this is valid.
  if(client==NULL||!client->isValid())
    return false;

  byte=DCSREQUEST_DATAMODE;
  if(client->write(&byte,sizeof(byte))!=sizeof(byte))
    return false;

  //Send mode, followed by address and port.
  //In the case of UDP we will send our IP address for the address.
  mode=info.datatype;
  if(client->write(&mode,sizeof(mode))!=sizeof(mode))
    return false;

  port=htons(info.dataport);
  if(client->write(&port,sizeof(port))!=sizeof(port))
    return false;

  info.datasock->getAddress(addr,sizeof(addr));
  if(client->write(addr,sizeof(addr))!=sizeof(addr))
    return false;

  return true;
}

bool DCSCom::serverSendTime(const SockClient* client,const DCSTimeHeader& time)
{
  char data[MAX_DATAGRAM_SIZE];

  //First test to make sure this is valid.
  if(client==NULL||!client->isValid())
    return false;

  int8_t byte=DCSREQUEST_TIME;
  if(client->write(&byte,sizeof(byte))!=sizeof(byte))
    return false;

  if(client->write(time.pack(data,sizeof(data)),time.dataSize())!=time.dataSize())
    return false;

  return true;
}

bool DCSCom::serverSendScenario(const SockClient* client,const DCSScenarioHeader& scenario)
{
  char data[MAX_DATAGRAM_SIZE];

  //First test to make sure this is valid.
  if(client==NULL||!client->isValid())
    return true;

  int8_t byte=DCSREQUEST_SCENARIO;
  if(client->write(&byte,sizeof(byte))!=sizeof(byte))
    return false;

  if(client->write(scenario.pack(data,sizeof(data)),scenario.dataSize())==SOCKET_ERROR)
    return false;

  return true;
}

bool DCSCom::serverSendHeaders(const SockClient* client,const DCSHeaderContainer& headers)
{
  //First test to make sure this is valid.
  if(client==NULL||!client->isValid())
    return true;

  uint8_t byte=DCSREQUEST_HEADERS;
  if(client->write(&byte,sizeof(byte))!=sizeof(byte))
    return false;

  int32_t total=htonl(uint32_t(headers.size()));
  if(client->write(&total,sizeof(total))==SOCKET_ERROR)
    return false;

  headers.traverse(serverHCFunc,(void*)client);

  return true;
}

void DCSCom::serverHCFunc(const DCSBaseHeader* header,void* data)
{
  char buffer[MAX_DATAGRAM_SIZE];
  const SockClient* client=(const SockClient*)data;
  client->write(header->pack(buffer,sizeof(buffer)),header->dataSize());
}

bool DCSCom::serverTerminateClient(DCSServerInfo& info,SockClient* client)
{
  bool success=false;

  if(client==NULL)
    return false;

  //Try to find it in the list
  ClientList::iterator s;
  for(s=info.clientlist.begin();s!=info.clientlist.end();++s)
  {
    if(*s==client)
    {
      success=true;
      break;
    }
  }

  if(success)
    info.clientlist.erase(s);

  //Try sending it a hangup message
  if(client->writeReady())
  {
    int8_t byte=DCSREQUEST_DISCONNECT;
    client->write(&byte,sizeof(byte));
  }

  delete client;

  return success;
}

bool DCSCom::serverTerminateAllClients(DCSServerInfo& info)
{
  while(!info.clientlist.empty())
  {
    SockClient* client=info.clientlist.back();
    info.clientlist.pop_back();

    if(client->writeReady())
    {
      int8_t byte=DCSREQUEST_DISCONNECT;
      client->write(&byte,sizeof(byte));
    }

    delete client;
  }

  //Clear out any of these guys too.
  info.vagrants.clear();

  return true;
}

bool DCSCom::serverExpelVagrants(DCSServerInfo& info)
{
  while(!info.vagrants.empty())
  {
    SockClient* client=info.vagrants.back();
    info.vagrants.pop_back();
    serverTerminateClient(info,client);
  }

  return true;
}

//Maybe throw an exception on an error
int8_t DCSCom::serverReadRequest(const SockClient* client)
{
  int8_t byte;
  int32_t result=client->readAll(&byte,sizeof(byte));
  return (result==SOCKET_ERROR||result==0)?-1:byte;
}

void DCSCom::serverProcessRequests(DCSServerInfo& info,DCSTimeHeader& time,DCSScenarioHeader& scenario,DCSHeaderContainer& headers)
{
  //See if they want anything
  ClientList::iterator s;
  for(s=info.clientlist.begin();s!=info.clientlist.end();++s)
  {
    if((*s)->errorCheck())
    {
      info.vagrants.push_back(*s);
    }
    else if((*s)->readReady())
    {
      int8_t req=serverReadRequest(*s);
      switch(req)
      {
        case DCSREQUEST_DATAMODE:
	  if(!serverSendDataParams(info,(*s)))
            info.vagrants.push_back(*s);
	  break;
        case DCSREQUEST_TIME:
	  if(!serverSendTime((*s),time))
            info.vagrants.push_back(*s);
	  break;
        case DCSREQUEST_SCENARIO:
	  if(!serverSendScenario((*s),scenario))
            info.vagrants.push_back(*s);
	  break;
	case DCSREQUEST_EVENT:
	  //This is not a valid request.
	  //It is for the server to send only not the client.
	  break;
	case DCSREQUEST_HEADER:
	{
	  char buffer[8];
	  uint64_t id;
	  DCSHeaderGroup header;

	  // Read id value
	  if((*s)->readAll(buffer,sizeof(buffer))!=sizeof(buffer))
	  {
	    info.vagrants.push_back(*s);
	    break;
	  }

	  // Find header
	  beread(buffer,&id);
	  if(headers.retrieve(id,&header)==NULL)
	  {
	    // A header for the rspecified ID does not exist - send an expire event to invalidate it
	    int8_t byte=DCSREQUEST_EVENT;
	    char data[MAX_DATAGRAM_SIZE];
	    DCSEvent event(id,time.timedata_.time_,DCSSTATE_EXPIRE,DCSEVENT_STATE);

	    if((*s)->write(&byte,sizeof(byte))==SOCKET_ERROR)
	    {
	      info.vagrants.push_back(*s);
	      break;
	    }

	    if((*s)->write(event.pack(data,sizeof(data)),event.dataSize (info.serverVersion))==SOCKET_ERROR)
	    {
	      info.vagrants.push_back(*s);
	      break;
	    }
	  }
	  else
	  {
	    // Send header to client
	    int8_t byte=DCSREQUEST_HEADER;
	    char data[MAX_DATAGRAM_SIZE];

	    if((*s)->write(&byte,sizeof(byte))==SOCKET_ERROR)
	    {
	      info.vagrants.push_back(*s);
	      break;
	    }

	    if((*s)->write(header.baseheader_->pack(data,sizeof(data)),header.baseheader_->dataSize (info.serverVersion))==SOCKET_ERROR)
	    {
	      info.vagrants.push_back(*s);
	      break;
	    }
	  }
	  break;
        }
	case DCSREQUEST_HEADERS:
	  if(!serverSendHeaders((*s),headers))
            info.vagrants.push_back(*s);
	  break;
	case DCSREQUEST_KEEPALIVE:
	{
	  //Checking to see if we're still here.  Echo it back.
          int8_t byte=DCSREQUEST_KEEPALIVE;
          if((*s)->write(&byte,sizeof(byte))==SOCKET_ERROR)
            info.vagrants.push_back(*s);
	  break;
	}
        case DCSREQUEST_DISCONNECT:
	case SOCKET_ERROR:
	  //Either the client is hanging up or it is already gone.
	  //serverExpelVagrants will attempt to echo DCSREQUEST_DISCONNECT so we don't need to do it here.
          info.vagrants.push_back(*s);
	  break;
      }
    }
  }
}

//The other functions are just for show.  This is all you really need to send a data object.
bool DCSCom::sendUDPData(const DCSServerInfo& info,const DCSBaseData* data)
{
  //Possibly check result for errors.
  int32_t result=0;
  char address[DCS_ADDRSTRLEN];
  char buffer[MAX_DATAGRAM_SIZE];

  if(info.datatype==DCSDATA_UNCONNECTED||info.datasock==NULL||!info.datasock->isValid())
    return false;

  //Prepend the server id
  memcpy(buffer,info.server_id,DCS_SERVERIDSZ);

  //For unicast the data must be sent to each client individually
  if(info.datatype==DCSDATA_UNICAST)
  {
    SockAddrInet* addr=SockAddrFactory::create(AF_INET);
    addr->setPort(info.dataport);

    ClientList::const_iterator c;
    for(c=info.clientlist.begin();c!=info.clientlist.end();c++)
    {
      addr->setAddress((*c)->getAddress(address,sizeof(address)));
      data->pack(buffer+DCS_SERVERIDSZ,MAX_DATAGRAM_SIZE-DCS_SERVERIDSZ);
      result=info.datasock->writeTo(buffer,data->dataSize(info.serverVersion)+DCS_SERVERIDSZ,0,addr->getSockAddr(),addr->getSockAddrLen());
    }

    delete addr;
  }
  else
  {
    data->pack(buffer+DCS_SERVERIDSZ,MAX_DATAGRAM_SIZE-DCS_SERVERIDSZ);
    result=info.datasock->write(buffer,data->dataSize(info.serverVersion)+DCS_SERVERIDSZ);
  }

  return (result==SOCKET_ERROR)?false:true;
}

bool DCSCom::sendTCPData(const DCSServerInfo& info,int8_t type,const DCSBaseData* data)
{
  int32_t result=0;
  char buffer[MAX_DATAGRAM_SIZE];
  ClientList::const_iterator c;

  for(c=info.clientlist.begin();c!=info.clientlist.end();c++)
  {
    result=(*c)->write(&type,sizeof(type));

    //If there is an error we should probably remove the socket.  But from where?
    //We'll shut it down so we get read errors
    if(result==SOCKET_ERROR||result==0)
    {
      (*c)->shutdown(SHUT_RDWR);
      continue;
    }

    result=(*c)->write(data->pack(buffer,sizeof(buffer)),data->dataSize(info.serverVersion));

    //If there is an error we should probably remove the socket.  But from where?
    //We'll shut it down so we get read errors
    if(result==SOCKET_ERROR||result==0)
      (*c)->shutdown(SHUT_RDWR);
  }

  return true;
}

bool DCSCom::sendPlatformHeader(const DCSServerInfo& info,uint64_t id,const char* iconname,const char* callsign,
				const DCSPoint& dims,const DCSPoint& offset,double latorigin,double lonorigin,
				uint32_t color,DCSObjectState_t state,DCSTrackStatus_t status,DCSCoordSystem_t coordsystem,
				DCSAllegience_t allegience,DCSPlatformType_t plattype,const DCSPlatformData& platdata)
{
  DCSPlatformHeader data(id,iconname,callsign,dims,offset,latorigin,lonorigin,color,state,status,coordsystem,allegience,plattype,platdata);
  bool rv = sendTCPData(info,DCSREQUEST_HEADER,&data);
  if (rv)
  {
    rv = (sendCategoryData (info, id, platdata.time_, DCS::getCategoryName (allegience), DCS::getCategoryValue (allegience)) ? true : false);
    if (rv)
      rv = (sendCategoryData (info, id, platdata.time_, DCS::getCategoryName (plattype), DCS::getCategoryValue (plattype)) ? true : false);
  }
  return rv;
}

bool DCSCom::sendPlatformHeader(const DCSServerInfo& info,uint64_t id,const char* iconname,const char* callsign,
				const DCSPoint& dims,const DCSPoint& offset,
				uint32_t color,DCSObjectState_t state,DCSTrackStatus_t status,bool interpolate,
				const DCSCoordReferenceFrame& refFrame,
				const DCSPlatformData& platdata)
{
  DCSPlatformHeader data(id,iconname,callsign,dims,offset,color,state,status,interpolate,
			 refFrame, platdata);
  return sendTCPData(info,DCSREQUEST_HEADER,&data);
}

bool DCSCom::sendPlatformData(const DCSServerInfo& info,uint64_t id,const DCSPoint& position,const DCSPoint& orientation,const DCSPoint& velocity,const DCSPoint& accel,double time)
{
  DCSPlatformData data(id,position,orientation,velocity,accel,time);
  return sendUDPData(info,&data);
}

bool DCSCom::sendBeamHeader(const DCSServerInfo& info,uint64_t id,const char* callsign,const DCSPoint& offset,double azimoffset,double elevoffset,bool missileOffset,double hbw,double vbw,int32_t hostid,int32_t targetid,uint32_t color,DCSObjectState_t state,DCSBeamType_t beamtype,bool interpolate,const DCSBeamData& beamdata)
{
  DCSBeamHeader data(id,callsign,offset,azimoffset,elevoffset,missileOffset,hbw,vbw,hostid,targetid,color,state,beamtype,interpolate,beamdata);
  return sendTCPData(info,DCSREQUEST_HEADER,&data);
}

bool DCSCom::sendBeamHeader(const DCSServerInfo& info,uint64_t id,const char* callsign,const DCSPoint& offset,double azimoffset,double elevoffset,double hbw,double vbw,int32_t hostid,int32_t targetid,uint32_t color,DCSObjectState_t state,DCSBeamType_t beamtype,const DCSBeamData& beamdata)
{
  DCSBeamHeader data(id,callsign,offset,azimoffset,elevoffset,hbw,vbw,hostid,targetid,color,state,beamtype,beamdata);
  return sendTCPData(info,DCSREQUEST_HEADER,&data);
}

bool DCSCom::sendBeamData(const DCSServerInfo& info,uint64_t id,double time,double azim,double elev,double length)
{
  DCSBeamData data(id,time,azim,elev,length);
  return sendUDPData(info,&data);
}

bool DCSCom::sendGateHeader(const DCSServerInfo& info,uint64_t id,const char* callsign,const DCSPoint& offset,int32_t hostid,uint32_t color,DCSObjectState_t state,DCSGateType_t gatetype,bool interpolate,const DCSGateData& gatedata)
{
  DCSGateHeader data(id,callsign,offset,hostid,color,state,gatetype,interpolate,gatedata);
  return sendTCPData(info,DCSREQUEST_HEADER,&data);
}

bool DCSCom::sendGateHeader(const DCSServerInfo& info,uint64_t id,const char* callsign,const DCSPoint& offset,int32_t hostid,uint32_t color,DCSObjectState_t state,DCSGateType_t gatetype,const DCSGateData& gatedata)
{
  DCSGateHeader data(id,callsign,offset,hostid,color,state,gatetype,gatedata);
  return sendTCPData(info,DCSREQUEST_HEADER,&data);
}

bool DCSCom::sendGateData(const DCSServerInfo& info,uint64_t id,double time,double azim,double elev,double width,double height,double minrange,double maxrange,double centroid)
{
  DCSGateData data(id,time,azim,elev,width,height,minrange,maxrange,centroid);
  return sendUDPData(info,&data);
}

bool DCSCom::sendScopeData(const DCSServerInfo& info, uint64_t id, UTILS::UnitType_t xUnits, UTILS::UnitType_t yUnits, double time, double xStart,
			   double xStep, double yBias, double yScale, uint32_t numDataPoints, UTILS::VariableType_t dataType, uint16_t numGates,
			   const void* data, const DCSScopeGateData* gates)
{
  DCSScopeData dataPacket(id,xUnits,yUnits, time, xStart, xStep, yBias, yScale, numDataPoints, dataType, numGates, data, gates);
  return sendUDPData(info,&dataPacket);
}

bool DCSCom::sendTimeHeader(const DCSServerInfo& info,double realtimemod,int8_t timetype,const DCSTimeData& timedata)
{
  DCSTimeHeader data(realtimemod,timetype,timedata);
  return sendTCPData(info,DCSREQUEST_TIME,&data);
}

bool DCSCom::sendTimeData(const DCSServerInfo& info,double time)
{
  DCSTimeData data(time);
  return sendUDPData(info,&data);
}

bool DCSCom::sendScenarioHeader(const DCSServerInfo& info,const DCSCoordReferenceFrame& refFrame,int16_t referenceyear,const char* classificationStr,uint32_t classificationColor,const DCSScenarioData& scendata)
{
  DCSScenarioHeader data(refFrame,referenceyear,classificationStr,classificationColor,scendata);
  return sendTCPData(info,DCSREQUEST_SCENARIO,&data);
}

bool DCSCom::sendScenarioHeader(const DCSServerInfo& info,double latorigin,double lonorigin,int16_t referenceyear,DCSCoordSystem_t coordsystem,DCSClassification_t classification,const DCSScenarioData& scendata)
{
  DCSScenarioHeader data(latorigin,lonorigin,referenceyear,coordsystem,classification,scendata);
  return sendTCPData(info,DCSREQUEST_SCENARIO,&data);
}

bool DCSCom::sendScenarioData(const DCSServerInfo& info,double winddirection,double windspeed)
{
  DCSScenarioData data(winddirection,windspeed);
  return sendUDPData(info,&data);
}

bool DCSCom::sendEvent(const DCSServerInfo& info,uint64_t id,double time,int32_t data,int8_t eventtype)
{
  DCSEvent event(id,time,data,eventtype);
  return sendTCPData(info,DCSREQUEST_EVENT,&event);
}

bool DCSCom::sendGenericData(const DCSServerInfo& info,uint64_t id,double begintime,double expiretime,const char* tag,const char* data)
{
  DCSGenericData gendata(id,begintime,expiretime,tag,data);
  return sendUDPData(info,&gendata);
}

bool DCSCom::sendCategoryData(const DCSServerInfo& info,uint64_t id,double timeVal,const char* name,const char* value)
{
  DCSCategoryData catdata(id,timeVal, name, value);
  return sendCategoryData(info,&catdata);
}

bool DCSCom::sendPlatformHeader(const DCSServerInfo& info,const DCSPlatformHeader* data)
{
  return sendTCPData(info,DCSREQUEST_HEADER,data);
}

bool DCSCom::sendPlatformData(const DCSServerInfo& info,const DCSPlatformData* data)
{
  return sendUDPData(info,data);
}

bool DCSCom::sendBeamHeader(const DCSServerInfo& info,const DCSBeamHeader* data)
{
  return sendTCPData(info,DCSREQUEST_HEADER,data);
}

bool DCSCom::sendBeamData(const DCSServerInfo& info,const DCSBeamData* data)
{
  return sendUDPData(info,data);
}

bool DCSCom::sendGateHeader(const DCSServerInfo& info,const DCSGateHeader* data)
{
  return sendTCPData(info,DCSREQUEST_HEADER,data);
}

bool DCSCom::sendGateData(const DCSServerInfo& info,const DCSGateData* data)
{
  return sendUDPData(info,data);
}

bool DCSCom::sendScopeData(const DCSServerInfo& info,const DCSScopeData* data)
{
  return sendUDPData(info,data);
}

bool DCSCom::sendTimeHeader(const DCSServerInfo& info,const DCSTimeHeader* data)
{
  return sendTCPData(info,DCSREQUEST_TIME,data);
}

bool DCSCom::sendTimeData(const DCSServerInfo& info,const DCSTimeData* data)
{
  return sendUDPData(info,data);
}

bool DCSCom::sendScenarioHeader(const DCSServerInfo& info,const DCSScenarioHeader* data)
{
  return sendTCPData(info,DCSREQUEST_SCENARIO,data);
}

bool DCSCom::sendScenarioData(const DCSServerInfo& info,const DCSScenarioData* data)
{
  return sendUDPData(info,data);
}

bool DCSCom::sendEvent(const DCSServerInfo& info,const DCSEvent* data)
{
  return sendTCPData(info,DCSREQUEST_EVENT,data);
}

bool DCSCom::sendGenericData(const DCSServerInfo& info,const DCSGenericData* data)
{
  return sendUDPData(info,data);
}

bool DCSCom::sendCategoryData(const DCSServerInfo& info,const DCSCategoryData* data)
{
  return sendTCPData(info,DCSREQUEST_HEADER,data);
}

bool DCSCom::clientConnect(DCSClientInfo& info,uint16_t port,const char* addr,const char* iface, bool block)
{
  if(info.servsock!=NULL)
    delete info.servsock;

  info.servsock=new SockClient;

  if(!info.servsock->isValid())
  {
    delete info.servsock;
    info.servsock=NULL;
    return false;
  }

  // Set blocking I/O mode
  if(!info.servsock->setBlockingIO(block))
  {
    delete info.servsock;
    info.servsock=NULL;
    return false;
  }

  // Bind to interface
  if(!info.servsock->bind(EPHEMERAL_PORT,iface))
  {
    delete info.servsock;
    info.servsock=NULL;
    return false;
  }

  // Connect to server
  if(!info.servsock->connect(port,addr))
  {
    delete info.servsock;
    info.servsock=NULL;
    return false;
  }

  int32_t flag=1;
  info.servsock->setSockOpt(SOL_SOCKET,SO_KEEPALIVE,&flag,sizeof(flag));

  return true;
}

bool DCSCom::clientNegotiate(DCSClientInfo& info,DCSTimeHeader& time,DCSScenarioHeader& scenario,uint32_t* headers,uint16_t* version)
{
  int8_t byte;
  uint16_t port;
  uint16_t ver;
  uint32_t total;
  char addr[DCS_ADDRSTRLEN];
  char iface[DCS_ADDRSTRLEN];
  char data[MAX_DATAGRAM_SIZE];

  if(info.servsock==NULL||!info.servsock->isValid())
    return false;

  // Switch to blocking IO, if a non-blocking connect was performed
  if(!info.servsock->isBlockingIO())
  {
    if(!info.servsock->setBlockingIO(true))
      return false;
  }

  if(!clientRequest(info,DCSREQUEST_CONNECT))
    return false;

  //Start it off
  if(info.servsock->readAll(&byte,sizeof(byte))!=sizeof(byte))
    return false;
  //Server should echo it back
  if(byte!=DCSREQUEST_CONNECT)
    return false;

  //Make version request
  if(!clientRequest(info,DCSREQUEST_VERSION))
    return false;

  //Get version
  if(info.servsock->readAll(&byte,sizeof(byte))!=sizeof(byte))
    return false;
  ver=byte;

  if(info.servsock->readAll(&byte,sizeof(byte))!=sizeof(byte))
    return false;
  ver=((0xFF&ver)<<8)|byte;

  if(version!=NULL) *version=ver;
  info.serverVersion = ver;

  //Check version
  if(ver>DCS_VERSION)
    return false;

  //Ask for data channel
  if(!clientRequest(info,DCSREQUEST_DATAMODE))
    return false;

  //Data mode
  if(info.servsock->readAll(&byte,sizeof(byte))!=sizeof(byte))
    return false;
  if(byte!=DCSREQUEST_DATAMODE)
    return false;
  if(info.servsock->readAll(&info.datatype,sizeof(info.datatype))!=sizeof(info.datatype))
    return false;

  //Data port
  if(info.servsock->readAll(&port,sizeof(port))!=sizeof(port))
    return false;
  port=ntohs(port);

  //Data address
  if(info.servsock->readAll(addr,sizeof(addr))!=sizeof(addr))
    return false;

  //Data server_id
  if(info.servsock->readAll(info.server_id,DCS_SERVERIDSZ)!=DCS_SERVERIDSZ)
    return false;

  //Ask for time
  if(!clientRequest(info,DCSREQUEST_TIME))
    return false;

  if(info.servsock->readAll(&byte,sizeof(byte))!=sizeof(byte))
    return false;
  if(byte!=DCSREQUEST_TIME)
    return false;
  if(info.servsock->readAll(data,time.dataSize (info.serverVersion))!=time.dataSize (info.serverVersion))
    return false;
  time.unpack(data,time.dataSize (info.serverVersion));

  //Ask for scenario
  if(!clientRequest(info,DCSREQUEST_SCENARIO))
    return false;

  if(info.servsock->readAll(&byte,sizeof(byte))!=sizeof(byte))
    return false;
  if(byte!=DCSREQUEST_SCENARIO)
    return false;
  if(info.servsock->readAll(data,scenario.dataSize (info.serverVersion))!=scenario.dataSize (info.serverVersion))
    return false;
  scenario.unpack(data,scenario.dataSize (info.serverVersion));

  //Ask for headers
  if(!clientRequest(info,DCSREQUEST_HEADERS))
    return false;
  if(info.servsock->readAll(&byte,sizeof(byte))!=sizeof(byte))
    return false;
  if(byte!=DCSREQUEST_HEADERS)
    return false;

  if(info.servsock->readAll(&total,sizeof(total))!=sizeof(total))
    return false;

  if(headers!=NULL) *headers=ntohl(total);

  //Make our data socket
  //Use the same interface as the TCP socket when applicable
  info.servsock->getIface(iface,sizeof(iface));
  switch(info.datatype)
  {
    case DCSDATA_UNCONNECTED:
    default:
      info.datatype=DCSDATA_UNCONNECTED;
      return false;
    case DCSDATA_UNICAST:
      if(info.datasock!=NULL)
	delete info.datasock;
      info.datasock=new SockUnicast(port,iface);
//       info.datasock->connect(port,addr);  //I don't think this actually helps our receive (only send which we don't do here), but that's allright
      break;
    case DCSDATA_BROADCAST:
      if(info.datasock!=NULL)
	delete info.datasock;
      info.datasock=new SockBroadcast(port);  //Broadcast datagrams come in on their own interface "0.0.0.0", so setting one will make us never read them.
//       info.datasock->connect(port,addr);
      break;
    case DCSDATA_MULTICAST:
      if(info.datasock!=NULL)
	delete info.datasock;
      info.datasock=new SockMulticast(port,addr,iface);
      break;
  }

  //Make sure our data socket was succesfully constructed.
  if(!info.datasock->isValid())
    return false;

  //We don't write to this socket
  info.datasock->shutdown(SHUT_WR);

  return true;
}

bool DCSCom::clientNegotiate(DCSClientInfo& info,DCSTimeHeader& time,DCSScenarioHeader& scenario,HeaderList& headers,uint16_t* version)
{
  int8_t byte;
  uint16_t port;
  uint16_t ver;
  int32_t total;
  DCSBaseData* header=NULL;
  HeaderList platforms;
  HeaderList beams;
  HeaderList gates;
  char addr[DCS_ADDRSTRLEN];
  char iface[DCS_ADDRSTRLEN];
  char data[MAX_DATAGRAM_SIZE];

  if(info.servsock==NULL||!info.servsock->isValid())
    return false;

  // Switch to blocking IO, if a non-blocking connect was performed
  if(!info.servsock->isBlockingIO())
  {
    if(!info.servsock->setBlockingIO(true))
      return false;
  }

  if(!clientRequest(info,DCSREQUEST_CONNECT))
    return false;

  //Start it off
  if(info.servsock->readAll(&byte,sizeof(byte))!=sizeof(byte))
    return false;
  //Server should echo it back
  if(byte!=DCSREQUEST_CONNECT)
    return false;

  //Make version request
  if(!clientRequest(info,DCSREQUEST_VERSION))
    return false;

  //Get version
  if(info.servsock->readAll(&byte,sizeof(byte))!=sizeof(byte))
    return false;
  ver=byte;

  if(info.servsock->readAll(&byte,sizeof(byte))!=sizeof(byte))
    return false;
  ver=((0xFF&ver)<<8)|byte;

  if(version!=NULL) *version=ver;
  info.serverVersion = ver;

  //Check version
  if(ver>DCS_VERSION)
    return false;

  //Ask for data channel
  if(!clientRequest(info,DCSREQUEST_DATAMODE))
    return false;

  //Data mode
  if(info.servsock->readAll(&byte,sizeof(byte))!=sizeof(byte))
    return false;
  if(byte!=DCSREQUEST_DATAMODE)
    return false;
  if(info.servsock->readAll(&info.datatype,sizeof(info.datatype))!=sizeof(info.datatype))
    return false;

  //Data port
  if(info.servsock->readAll(&port,sizeof(port))!=sizeof(port))
    return false;
  port=ntohs(port);

  //Data address
  if(info.servsock->readAll(addr,sizeof(addr))!=sizeof(addr))
    return false;

  //Data server_id
  if(info.servsock->readAll(info.server_id,DCS_SERVERIDSZ)!=DCS_SERVERIDSZ)
    return false;

  //Ask for time
  if(!clientRequest(info,DCSREQUEST_TIME))
    return false;

  if(info.servsock->readAll(&byte,sizeof(byte))!=sizeof(byte))
    return false;
  if(byte!=DCSREQUEST_TIME)
    return false;
  if(info.servsock->readAll(data,time.dataSize (info.serverVersion))!=time.dataSize (info.serverVersion))
    return false;

  time.unpack(data,time.dataSize (info.serverVersion));

  //Ask for scenario
  if(!clientRequest(info,DCSREQUEST_SCENARIO))
    return false;

  if(info.servsock->readAll(&byte,sizeof(byte))!=sizeof(byte))
    return false;
  if(byte!=DCSREQUEST_SCENARIO)
    return false;
  if(info.servsock->readAll(data,scenario.dataSize (info.serverVersion))!=scenario.dataSize (info.serverVersion))
    return false;
  scenario.unpack(data,scenario.dataSize (info.serverVersion));

  //Ask for headers
  if(!clientRequest(info,DCSREQUEST_HEADERS))
    return false;
  if(info.servsock->readAll(&byte,sizeof(byte))!=sizeof(byte))
    return false;
  if(byte!=DCSREQUEST_HEADERS)
    return false;

  if(info.servsock->readAll(&total,sizeof(total))!=sizeof(total))
    return false;
  total=ntohl(total);

  while(total-->0)
  {
    header=readTCPData(info);
    if(header==NULL)
      return false;

    //We store the headers in list by types and then merge them into a single list sorted by type
    switch(header->getType())
    {
      case DCSBEAMHEADER:
	beams.push_back(header);
	break;
      case DCSGATEHEADER:
	gates.push_back(header);
	break;
      default:
	platforms.push_back(header);
    }
  }

  //Now we merge the headers into the list: platforms, then beams, then gates
  headers.insert(headers.end(), platforms.begin(), platforms.end());
  headers.insert(headers.end(), beams.begin(), beams.end());
  headers.insert(headers.end(), gates.begin(), gates.end());

  //Make our data socket
  //Use the same interface as the TCP socket when applicable
  info.servsock->getIface(iface,sizeof(iface));
  switch(info.datatype)
  {
    case DCSDATA_UNCONNECTED:
    default:
      info.datatype=DCSDATA_UNCONNECTED;
      return false;
    case DCSDATA_UNICAST:
      if(info.datasock!=NULL)
	delete info.datasock;
      info.datasock=new SockUnicast(port,iface);
//       info.datasock->connect(port,addr);  //I don't think this actually helps our receive (only send which we don't do here), but that's allright
      break;
    case DCSDATA_BROADCAST:
      if(info.datasock!=NULL)
	delete info.datasock;
      info.datasock=new SockBroadcast(port);  //Broadcast datagrams typically come in on a broadcast interface like "0.0.0.0", so setting one will make us never read them.
//       info.datasock->connect(port,addr);
      break;
    case DCSDATA_MULTICAST:
      if(info.datasock!=NULL)
	delete info.datasock;
      info.datasock=new SockMulticast(port,addr,iface);
      break;
  }

  //Make sure our data socket was succesfully constructed.
  if(!info.datasock->isValid())
    return false;

  //We don't write to this socket
  info.datasock->shutdown(SHUT_WR);

  return true;
}

void DCSCom::clientDisconnect(DCSClientInfo& info)
{
  if(info.datasock!=NULL)
  {
    delete info.datasock;
    info.datasock=NULL;
    info.datatype=DCSDATA_UNCONNECTED;
  }

  if(info.servsock!=NULL)
  {
    if(clientRequest(info,DCSREQUEST_DISCONNECT))
    {
      //Check on response
      int8_t byte;
      info.servsock->shutdown(SHUT_WR);

      //We give it 0.25 seconds to respond before giving up
      if(info.servsock->readReady(0,250000))
      {
        info.servsock->readAll(&byte,sizeof(byte));
        if(byte!=DCSREQUEST_DISCONNECT)
        {
	  //Print warning? We need to close anyway so what does it matter?
	  ;
        }
      }
    }

    delete info.servsock;
    info.servsock=NULL;
  }
}

bool DCSCom::clientRequest(const DCSClientInfo& info,int8_t request)
{
  if(info.servsock==NULL||!info.servsock->isValid())
    return false;

  // Headers and events can not be requested with this function
  if(request == DCSREQUEST_HEADER || request == DCSREQUEST_EVENT)
    return false;

  if(info.servsock->write(&request,sizeof(request))==SOCKET_ERROR)
    return false;

  return true;
}

bool DCSCom::clientHeaderRequest(const DCSClientInfo& info,uint64_t id)
{
  int8_t request = DCSREQUEST_HEADER;
  char buffer[8];

  if(info.servsock==NULL||!info.servsock->isValid())
    return false;

  if(info.servsock->write(&request,sizeof(request))==SOCKET_ERROR)
    return false;

  bewrite(buffer,&id);
  if(info.servsock->write(buffer,sizeof(buffer))==SOCKET_ERROR)
    return false;

  return true;
}

DCSBaseData* DCSCom::readTCPData(const DCSClientInfo& info)
{
  char data[MAX_DATAGRAM_SIZE];
  int16_t* type=0;
  DCSBaseData* header=NULL;

  if(info.servsock->readAll(data,sizeof(int16_t))!=sizeof(int16_t))
    return NULL;

  type = (int16_t*)data;
  switch(ntohs(*type))
  {
    case DCSPLATFORMHEADER:
      header=new DCSPlatformHeader();
      break;
    case DCSBEAMHEADER:
      header=new DCSBeamHeader();
      break;
    case DCSGATEHEADER:
      header=new DCSGateHeader();
      break;
    case DCSTIMEHEADER:
      header=new DCSTimeHeader();
      break;
    case DCSSCENARIOHEADER:
      header=new DCSScenarioHeader();
      break;
    case DCSEVENT:
      header=new DCSEvent();
      break;
    case DCSGENERICDATA:
      header=new DCSGenericData();
      break;
    case DCSCATEGORYDATA:
      header=new DCSCategoryData();
      break;
    case DCSUNKNOWN:
    default:
      return NULL;
  }

  int expectedSize = header->dataSize(info.serverVersion) - sizeof(int16_t);
  if(info.servsock->readAll((char*)(data + sizeof(int16_t)), header->dataSize(info.serverVersion) - sizeof(int16_t)) != expectedSize)
  {
    delete header;
    return NULL;
  }

  header->unpack(data,header->dataSize(info.serverVersion));

  return header;
}

DCSBaseData* DCSCom::readUDPData(const DCSClientInfo& info)
{
  char data[MAX_DATAGRAM_SIZE];
  int16_t type=0;
  DCSBaseData* header=NULL;

  int32_t result=info.datasock->read(data,sizeof(data));
  if(result==SOCKET_ERROR||result==0)
    return NULL;

  //First make sure it's one of ours (sent server we are connected to).
  if(memcmp(data,info.server_id,DCS_SERVERIDSZ)!=0)
    return NULL;

  type=ntohs(*((uint16_t*)&data[DCS_SERVERIDSZ]));
  switch(type)
  {
    case DCSPLATFORMDATA:
      header=new DCSPlatformData();
      break;
    case DCSBEAMDATA:
      header=new DCSBeamData();
      break;
    case DCSGATEDATA:
      header=new DCSGateData();
      break;
    case DCSTIMEDATA:
      header=new DCSTimeData();
      break;
    case DCSSCENARIODATA:
      header=new DCSScenarioData();
      break;
    case DCSGENERICDATA:
      header=new DCSGenericData();
      break;
    case DCSCATEGORYDATA:
      header=new DCSCategoryData();
      break;
    case DCSSCOPEDATA:
      header=new DCSScopeData();
      break;
    case DCSUNKNOWN:
    default:
      return NULL;
  }

  if ((type != DCSSCOPEDATA) && ((result-DCS_SERVERIDSZ)!=header->dataSize(info.serverVersion)))
  {
    delete header;
    return NULL;
  }

  header->unpack(&data[DCS_SERVERIDSZ], result-DCS_SERVERIDSZ);

  return header;
}
