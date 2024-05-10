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

#ifndef _DCSSCENARIOHEADER_H_
#define _DCSSCENARIOHEADER_H_

#include "DCSScenarioData.h"
#include "DCSBaseData.h"
#include "DCSdefs.h"
#include "DCSCoordReferenceFrame.h"

///Scenario header class.
/**
 * Class containting data for specifying the initial scenario 
 * information for the Simdis application.  
 */
class DCSScenarioHeader : public DCSBaseData
{
  public:
    ///Scenario reference year.  
    int16_t referenceyear_;		/**< Calendar year for scenario.  */

    ///Data classification string
    char classificationstr_[64];
    ///Classification color in 0xAABBGGRR
    uint32_t classcolor_;

    ///Coordinate system reference frame
    DCSCoordReferenceFrame referenceframe_;	/**< Coordinate reference frame information for scenario */

    ///Current data value
    DCSScenarioData scendata_;		/**< An initial data set for the object associated with this header.  */

  public:
    ///Default scenario header constructor.  
    /**
     * Creates a scenario header object with default data values.
     */
    DCSScenarioHeader();

    ///Scenario header constructor.  
    /**
     * Creates a scenario header object with the specified data values.  
     * @param refFrame Coordinate reference frame specifying scenario origin, default coordinate system, and other settings.
     * @param referenceyear a short specifying the calendar year.
     * @param classification a string specifying the data classification.
     * @param classColor a 32 bit integer specificying the classification color in 0xAABBGGRR
     * @param scendata a DCSScenarioData object providing an initial data set for the object associated with this header.  
     */
    DCSScenarioHeader(const DCSCoordReferenceFrame& refFrame, int16_t referenceyear,const char* classification,uint32_t classColor,const DCSScenarioData& scendata);

    ///Scenario header constructor. (deprecated)
    /**
     * Creates a scenario header object with the specified data values.  
     * @param latorigin a double specifying the latitude topo origin.
     * @param lonorigin a double specifying the longitude topo origin.
     * @param referenceyear a short specifying the calendar year.
     * @param coordsystem a byte specifying the overall coordinate system.  
     * @param classification a byte specifying the data classification.
     * @param scendata a DCSScenarioData object providing an initial data set for the object associated with this header.  
     */
    DCSScenarioHeader(double latorigin,double lonorigin,int16_t referenceyear,DCSCoordSystem_t coordsystem,DCSClassification_t classification,const DCSScenarioData& scendata);

    ///Set contents of data object to default values.  
    /**
     * Clears contents of data object, setting all fields to default values.  
     */
    virtual void clear();

    ///Prepare data for network transmission.
    /**
     * @see DCSBaseData::pack()
     */
    virtual void* pack(void* buffer,int32_t size) const;

    ///Extract data from network transmission.
    /**
     * @see DCSBaseData::unpack()
     */
    virtual void unpack(const void* buffer,int32_t size);

    ///Determine object's data size.
    /**
     * @see DCSBaseData::dataSize()
     */
    virtual int32_t dataSize() const {return dataSize (DCS_VERSION);}

    ///Determine object's data size.
    /**
     * @see DCSBaseData::dataSize()
     */
    virtual int32_t dataSize(uint16_t version) const {
      if (version == 0x0200)
	return DCSBaseData::dataSize()+
	  sizeof(referenceyear_)+sizeof(int8_t)+
	  referenceframe_.dataSize(version)+
	  scendata_.dataSize();
      return DCSBaseData::dataSize()+
	sizeof(referenceyear_)+
	sizeof(classificationstr_)+sizeof(classcolor_)+
	referenceframe_.dataSize(version)+
	scendata_.dataSize();
    }

    ///Determine object's physical size.
    /**
     * @see DCSBaseData::sizeOf()
     */
    virtual int32_t sizeOf() const {return sizeof(*this);}
};

#endif
