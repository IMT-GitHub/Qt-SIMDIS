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

#include <fcntl.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <map>
#include "stdlibc.h"
#include "inttypesc.h"
#include "DCSswapbytes.h"
#include "DCSdefs.h"
#include "DCSPoint.h"
#include "DCSBaseData.h"
#include "DCSBaseHeader.h"
#include "DCSPlatformData.h"
#include "DCSBeamData.h"
#include "DCSGateData.h"
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

#define DCSDB_CODE "DCSD"
#define DCSDB_CODE_SIZE 4
///DCS Database File header block
/*
 * A data block written to the start of the DCS database file to identify the file 
 * as a valid DCS database file containing DCS header data.  
 * @param code_ a 4 byte character array containing the 4 byte "DCSD" string to identify the file as a 
 *        DCS header database file.  
 * @param version_ an unsigned short containing the DCS database version number.  
 */
struct DCSDBHeader
{
public:
    ///4 byte character array containing the 4 byte "DCSD" string to identify the file as a DCS header database file.  
    char code_[DCSDB_CODE_SIZE];
    ///an unsigned short containing the DCS database version number.  
    uint16_t version_;
};


DCSHeaderContainer::DCSHeaderContainer()
  : fd_(-1),
    save_(false)
{
  memset(filename_,0,sizeof(filename_));
}

DCSHeaderContainer::~DCSHeaderContainer()
{
  clearItems();
  if(fd_!=-1)
  {
    close(fd_);
    fd_=-1;
    if(!save_)
      unlink(filename_);
  }
}

void DCSHeaderContainer::clearItems()
{
  HeaderTable::const_iterator i;
  for(i=table_.begin();i!=table_.end();i++)
    delete i->second;
  table_.clear();
/*
  while(!table_.empty())
  {
    HeaderTable::iterator i=table_.begin();
    table_.erase(i);
    delete i->second;
  }
*/
}

bool DCSHeaderContainer::initialize(const char* filename)
{
  DCSDBHeader idblock;
  int32_t flags=O_CREAT|O_TRUNC|O_RDWR;
  
  //Add additional flags if they are available
#ifdef O_BINARY
  flags|=O_BINARY;
#endif

#ifdef O_RANDOM
  flags|=O_RANDOM;
#endif

  if(filename==NULL)
    return false;
  
  fd_=open(filename,flags,0644);

  if(fd_==-1)
    return false;

  //Write the file type followed by the version.  
  memcpy(idblock.code_,DCSDB_CODE,DCSDB_CODE_SIZE);
  idblock.version_=DCS_VERSION;
  if(write(fd_,&idblock,sizeof(idblock))==-1)
  {
    close(fd_);
    fd_=-1;
    return false;
  }

  strncpy(filename_,filename,strlen(filename)-1);
  return true;
}

bool DCSHeaderContainer::restore(const char* filename,uint16_t* version)
{
  DCSDBHeader idblock;
  int32_t flags=O_CREAT|O_RDWR;
  
  //Add additional flags if they are available
#ifdef O_BINARY
  flags|=O_BINARY;
#endif

#ifdef O_RANDOM
  flags|=O_RANDOM;
#endif
  
  if(filename==NULL)
    return false;
  
  fd_=open(filename,flags,0644);

  if(fd_!=-1)
  {
    //Restore it
    DCSHeaderRecord* record=NULL;
    DCSBaseHeader* header=NULL;
    DCSHeaderGroup headergroup;
    DCSFileEntryHeader tag;
    int32_t location=0;
    int32_t result=0;

    //Read id and version, and make sure they match.  
    result=read(fd_,&idblock,sizeof(idblock));
    if(result!=sizeof(idblock))
    {
      close(fd_);
      fd_=-1;
      return false;
    }

    if(version!=NULL) *version=idblock.version_;

    if((memcmp(idblock.code_,DCSDB_CODE,DCSDB_CODE_SIZE)!=0)||(idblock.version_!=DCS_VERSION))
    {
      close(fd_);
      fd_=-1;
      return false;
    }

    do
    {
//Linux does not implement tell.  
//      location=tell(fd_);
      
      result=read(fd_,(void*)&tag,sizeof(tag));

      //Zero indicates eof
      if(result==0)
	break;
      else if(result!=sizeof(tag))
      {
        close(fd_);
	fd_=-1;
        return false;
      }

      if(tag.active_)
      {
	header=NULL;
	switch(tag.type_)
	{
          case DCSPLATFORMHEADER:
	    if(tag.size_==sizeof(headergroup.platheader_))
              header=&headergroup.platheader_;
	    break;
          case DCSBEAMHEADER:
	    if(tag.size_==sizeof(headergroup.beamheader_))
              header=&headergroup.beamheader_;
	    break;
          case DCSGATEHEADER:
	    if(tag.size_==sizeof(headergroup.gateheader_))
              header=&headergroup.gateheader_;
	    break;
	  default:
	    break;
	}

	//Either the type was unknown or the size was wrong.  Don't read this file.  
	if(header==NULL)
	{
          clearItems();
          close(fd_);
          fd_=-1;
          return false;
	}

        result=read(fd_,(void*)header,tag.size_);

	//We don't check for endof file this time because an
	//end of file here indicates a corrupted/invalid file.  
	if(result!=tag.size_)
        {
	  delete header;
          clearItems();
          close(fd_);
	  fd_=-1;
          return false;
        }

	printf("Header type: %d size is %d\n",header->getType(),header->sizeOf());

	record=new DCSHeaderRecord;
	record->size_=header->sizeOf();
	record->location_=location;

//         std::pair<uint32_t,DCSHeaderRecord*> entry(header->id_,record);
        std::pair<uint64_t,DCSHeaderRecord*> entry(header->id_,record);
        std::pair<HeaderTable::iterator,bool> p=table_.insert(entry);

	if(!p.second)
	{
	  //Could not insert?
	  clearItems();
	  close(fd_);
	  fd_=-1;
	  return false;
	}
      }
      else
      {
        lseek(fd_, tag.size_, SEEK_CUR);
      }

      //Mark location of the next record
      location+=sizeof(tag)+tag.size_;
    } while(result!=0);

    strncpy(filename_,filename,sizeof(filename_)-1);
    return true;
  }

  return false;
}

bool DCSHeaderContainer::insert(const DCSBaseHeader* header)
{
  DCSHeaderRecord* record=new DCSHeaderRecord;
  record->size_=header->sizeOf();

  //Put it in the list
  std::pair<uint64_t,DCSHeaderRecord*> entry(header->id_,record);
  std::pair<HeaderTable::iterator,bool> p=table_.insert(entry);

  //See if it was inserted
  if(p.second)
  {
    //Go to end of file for new entry
    record->location_=lseek(fd_,0,SEEK_END);

    if(record->location_!=-1)
    {
      //Write the file tag
      DCSFileEntryHeader tag;
      tag.active_=1;
      tag.type_=header->getType();
      tag.size_=header->sizeOf();

      if(write(fd_,(const void*)&tag,sizeof(tag))!=-1)
      {
        //Put it in the file
        if(write(fd_,(const void*)header,header->sizeOf())!=-1)
	{
          return true;  //Success
	}
	else
	{
	  //We might need to do something about the tag if
	  //it becomes a problem.  Currently the only real solution
	  //I can think of is to recreate the file, copying everything
	  //up to this point into it.
	  clean();
	}
      }
    }
    else
    {
      //Error with the file.  No new entry.  Remove it.
      table_.erase(header->id_);
    }
  }

  //An entry for this key already exists, or could not be written to the file.
  delete record;
  return false;
}

bool DCSHeaderContainer::update(const DCSBaseHeader* header,bool& present)
{
  HeaderTable::iterator i=table_.find(header->id_);
  
  if(i==table_.end())
  {
    present=false;
    return insert(header);
  }

  present=true;

  //We overwrite the entry with this one.
  if(i->second->size_!=header->sizeOf())
    return false;  //Something is wrong.  Sizes don't match.

  //Now we can put it in the file
//  if(lseek(fd_,i->second->location_,SEEK_SET)==-1)
  int posi=lseek(fd_,i->second->location_,SEEK_SET);
  if(posi==-1)
    return false;

  //Update the file tag
  DCSFileEntryHeader tag;
  tag.active_=1;
  tag.type_=header->getType();
  tag.size_=header->sizeOf();
  if(write(fd_,(const void*)&tag,sizeof(tag))==-1)
    return false;

  if(write(fd_,(const void*)header,header->sizeOf())==-1)
    return false;

  return true;
}

bool DCSHeaderContainer::remove(uint64_t id)
{
  HeaderTable::iterator i=table_.find(id);
  
  if(i==table_.end())
    return false;

  //Now we can update the file
  if(lseek(fd_,i->second->location_,SEEK_SET)==-1)
    return false;

  //Update the file tag - mark for deletion
  DCSFileEntryHeader tag;
  tag.active_=0;
  tag.type_=DCSUNKNOWN;
  tag.size_=i->second->size_;
  if(write(fd_,(const void*)&tag,sizeof(tag))==-1)
    return false;

  //Now Remove from table
  delete i->second;
  table_.erase(i);

  return true;
}

DCSBaseHeader* DCSHeaderContainer::retrieve(uint64_t id,DCSHeaderGroup* hg) const
{
  DCSBaseHeader* header=NULL;
  HeaderTable::const_iterator i=table_.find(id);
  
  //First clear the HeaderGroup so it has a type of 0 and baseheader_==NULL on failure.  
  if(hg)
  {
    hg->type_=0;
    hg->baseheader_=NULL;
  }

  if(i==table_.end())
    return NULL;

  //Now we can look in the file
  if(lseek(fd_,i->second->location_,SEEK_SET)==-1)
    return NULL;

  //Get the file tag
  DCSFileEntryHeader tag;
  if(read(fd_,(void*)&tag,sizeof(tag))!=sizeof(tag))
    return NULL;

  switch(tag.type_)
  {
    case DCSPLATFORMHEADER:
      header=(hg)?&hg->platheader_:new DCSPlatformHeader;
      break;
    case DCSBEAMHEADER:
      header=(hg)?&hg->beamheader_:new DCSBeamHeader;
      break;
    case DCSGATEHEADER:
      header=(hg)?&hg->gateheader_:new DCSGateHeader;
      break;
    default:
      return NULL;
  }

  if(hg)
  {
    hg->type_=tag.type_;
    hg->baseheader_=header;
  }
     
  if(read(fd_,(void*)header,header->sizeOf())!=header->sizeOf())
  {
    if(hg==NULL) delete header;
    return NULL;
  }

  return header;
}

void DCSHeaderContainer::traverse(void (*function)(const DCSBaseHeader*,void*),void* data) const
{
  HeaderTable::const_iterator i;
  DCSHeaderGroup header;
  for(i=table_.begin();i!=table_.end();i++)
  {
    retrieve(i->first,&header);
    function(header.baseheader_,data);
  }
}

bool DCSHeaderContainer::clean()
{
  DCSDBHeader idblock;
  DCSFileEntryHeader tag;
  DCSHeaderGroup headergroup;
  DCSBaseHeader *header = NULL;
  int32_t result = 0;
  long freedSpace = 0;
  long totalSize = 0; // simplier to just calculate than to querie the system

  lseek(fd_,0,SEEK_SET); // Go to beginning of file

  // File has a header block
  result = read(fd_, &idblock, sizeof(idblock));
  totalSize += sizeof(idblock);
  if (result != sizeof(idblock)) {
    // Failed to read
    close(fd_);
    fd_ = -1;
    return false;
  }

  // Process rest of file
  while (true) {
    // Read tag
    result = read(fd_, (void*) &tag, sizeof(tag));
    if (result == 0) {
      // EOF, finished
      break;
    } else if (result != sizeof(tag)) {
      close(fd_);
      fd_ = -1;
      return false;
    }

    if (!tag.active_) {
      // Not used, Delete by overwriting.
      freedSpace += sizeof(tag) + tag.size_;
      lseek(fd_, tag.size_, SEEK_CUR); // Skip reading data
    } else {
      totalSize += sizeof(tag) + tag.size_;

      // Keep, but need to shift forward in file
      header=NULL;
      switch(tag.type_)
      {
      case DCSPLATFORMHEADER:
        if(tag.size_==sizeof(headergroup.platheader_))
          header=&headergroup.platheader_;
        break;
      case DCSBEAMHEADER:
        if(tag.size_==sizeof(headergroup.beamheader_))
          header=&headergroup.beamheader_;
        break;
      case DCSGATEHEADER:
        if(tag.size_==sizeof(headergroup.gateheader_))
          header=&headergroup.gateheader_;
        break;
      default:
        cout << "UNSUPPORTED!!!" << endl;
        break;
      }

      // Read data, needed to retrive ID
      result = read(fd_, (void*) header, tag.size_);
      if (result != tag.size_)  {
        close(fd_);
        fd_ = -1;
        return false;
      }

      HeaderTable::iterator i = table_.find(header->id_);
      if ( i == table_.end()) {
        // Failed to find data in the table
        return false;
      }

      // Go to the start of the empty space in file
      lseek(fd_, -1 * (freedSpace + sizeof(tag) + tag.size_), SEEK_CUR);
      
      size_t wrv = write(fd_, (const void*) &tag, sizeof(tag));
      wrv = write(fd_, (const void*) header, tag.size_);
      
      // Update disk location stored in memory
      i->second->location_ = i->second->location_ - freedSpace;
      
      // Return to start of readable data in file
      lseek(fd_, freedSpace, SEEK_CUR);
    }
  }

  if (freedSpace > 0) {
    // Resize the file
    #ifdef WIN32
    _chsize_s(fd_, totalSize);
    #else
    int ftrv = ftruncate(fd_, totalSize);
    #endif
  }

  return true;
}

bool DCSHeaderContainer::getSave() const
{
  return save_;
}

void DCSHeaderContainer::setSave(bool save)
{
  save_=save;
}
