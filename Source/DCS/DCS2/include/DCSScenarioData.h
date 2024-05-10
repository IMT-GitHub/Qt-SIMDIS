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
#ifndef _DCSSCENARIODATA_H_
#define _DCSSCENARIODATA_H_

#include "DCSBaseData.h"

///Scenario data class.
/**
 * Class containting data for specifying the scenario 
 * information for the Simdis application.  
 */
class DCSScenarioData : public DCSBaseData
{
  public:
    ///Wind direction
    double winddirection_;	/**< Wind direction for Simdis to display in radians.  */
    ///Wind speed
    double windspeed_;		/**< Wind speed for Simdis to display in meters/second.  */

  public:
    ///Default scenario data constructor.  
    /**
     * Creates a scenario data object with default data values.
     */
    DCSScenarioData();

    ///Scenario data constructor.  
    /**
     * Creates a scenario data object with the specified data values.  
     * @param winddirection a double specifying the wind direction.  
     * @param windspeed a double specifying the wind speed.
     */
    DCSScenarioData(double winddirection,double windspeed);

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
    virtual int32_t dataSize() const {return DCSBaseData::dataSize()+
					sizeof(winddirection_)+sizeof(windspeed_);}

    ///Determine object's physical size.
    /**
     * @see DCSBaseData::sizeOf()
     */
    virtual int32_t sizeOf() const {return sizeof(*this);}
};

#endif
