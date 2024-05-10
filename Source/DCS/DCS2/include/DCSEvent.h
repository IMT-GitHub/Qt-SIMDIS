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

#ifndef _DCSEVENT_H_
#define _DCSEVENT_H_

#include "DCSBaseData.h"

///Event class.
/**
 * Class containting information for executing an event
 * in the SIMDIS application.  
 */
class DCSEvent : public DCSBaseData
{
  public:
    ///Event's execution time.  
    double time_;		/**< Time at which the event is to be executed.  */
    ///Event data.  Can represent one of four things: target id, color, state, or status.  
    int64_t data_;		/**< A value specifying data for the execution of four separate events:
				 * <ul>
				 *   <li> Set target id for a beam.
				 *     <ul>
				 *       <li> Event data is for a beam object.
				 *       <li> Event data is an eight byte integer (int64_t) id of entity to be tracked by a beam.
				 *       <li> A value of -1 means no target.  
				 *       <li> Event chardata is ignored by client and does not need to be set.  
				 *     </ul>
				 *   <li> Set color for beam, gate, or platform.
				 *     <ul>
				 *       <li> Event data for a beam, gate, or platform object.
				 *       <li> Event data is a four byte unsigned integer (uint32_t) RGBA color of object specified in hex (0xFFFFFFFF).
				 *       <li> Event data must be type cast to int64_t for storage/transmission.
				 *       <li> Event chardata is ignored by client and does not need to be set.  
				 *     </ul>
				 *   <li> Set object state for beam, gate, or platform.
				 *     <ul>
				 *       <li> Event data for a beam, gate, or platform.
				 *       <li> Event data is a single byte (int8_t) object state indicator.
				 *       <li> Event data must be type cast to int64_t for storage/transmission.
				 *       <li> Event chardata is ignored by client and does not need to be set.  
				 *     </ul>
				 *   <li> Set track status for platform.
				 *     <ul>
				 *       <li> Event data for a platform.
				 *       <li> Event data is a single byte (int8_t) object track status indicator.  
				 *       <li> Event data must be type cast to int64_t for storage/transmission.
				 *       <li> Event chardata is ignored by client and does not need to be set.  
				 *     </ul>
				 *   <li> Set classification level for scenario.
				 *     <ul>
				 *       <li> Event data for the scenario.  Event id is ignored by client and does not need to be set.  
				 *       <li> Event data is a single byte (int8_t) classification indicator.  
				 *       <li> Event chardata is ignored by client and does not need to be set.  
				 *       <li> Event data must be type cast to int64_t for storage/transmission.
				 *     </ul>
				 *   <li> Sets the interpolate position flag for a beam (defaults to "on")
				 *     <ul>
				 *       <li> Event data for a beam
				 *       <li> Event data is a single byte (int8_t) beam interpolate position status indicator.  
				 *       <li> Event data must be type cast to int64_t for storage/transmission.
				 *       <li> Event chardata is ignored by client and does not need to be set.  
				 *     </ul>
				 *   <li> Sets the interpolate position flag for a gate (defaults to "on")
				 *     <ul>
				 *       <li> Event data for a gate
				 *       <li> Event data is a single byte (int8_t) gate interpolate position status indicator.  
				 *       <li> Event data must be type cast to int64_t for storage/transmission.
				 *       <li> Event chardata is ignored by client and does not need to be set.  
				 *     </ul>
				 * </ul>
				 * @see @ref state "Object States".
				 * @see @ref status "Track Status".  
				 */
    ///Event character data.  Represents character array based data for scenario, platform, beam or gate objects.  
    char chardata_[128];	/**< A value specifying char array data for the execution of various events:
				 * <ul>
				 *   <li> Set classification string for scenario.
				 *     <ul>
				 *       <li> Event data is ignored by client and does not need to be set.  
				 *       <li> Event chardata contains the updated classification value for storage/transmission.
				 *     </ul>
				 *   <li> Set callsign for beam, gate, or platform.
				 *     <ul>
				 *       <li> Event data is ignored by client and does not need to be set.  
				 *       <li> Event chardata contains the updated callsign value for storage/transmission.
				 *     </ul>
				 * </ul>
				 */
    ///Event type.  
    int8_t eventtype_;		/**< The type of event to execute with the specified data.  
				 * @see @ref eventtype "Event Types".
				 */
    

  public:
    ///Default event constructor.  
    /**
     * Creates an event object with default data values.
     */
    DCSEvent();

    ///Event constructor.  
    /**
     * Creates an event object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the id of the object that the event is to be applied to.  
     * @param time a double specifying the execution time of the event.  
     * @param data a 64-bit integer containing variant data specifying either a target id, color, state, or status.
     * @param eventtype a byte specifying the event type, also indicating the type of data.  
     */
    DCSEvent(uint64_t id,double time,int64_t data,int8_t eventtype);

    ///Event constructor.  
    /**
     * Creates an event object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the id of the object that the event is to be applied to.  
     * @param time a double specifying the execution time of the event.  
     * @param chardata a character array containing variant data specifying classification or callsign.
     * @param eventtype a byte specifying the event type, also indicating the type of data.  
     */
    DCSEvent(uint64_t id,double time,char* chardata,int8_t eventtype);

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
    virtual int32_t dataSize(uint16_t version) const
    {
      if (version < 0x0204)
	return DCSBaseData::dataSize()+
	  sizeof(time_)+sizeof(data_)+
	  sizeof(eventtype_);
      return DCSBaseData::dataSize()+
	sizeof(time_)+sizeof(data_)+
	sizeof(chardata_)+sizeof(eventtype_);
    }

    ///Determine object's physical size.
    /**
     * @see DCSBaseData::sizeOf()
     */
    virtual int32_t sizeOf() const {return sizeof(*this);}
};    

#endif
