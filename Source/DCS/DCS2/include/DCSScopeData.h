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
 *               EW Modeling and Simulation, Code 5770
 *               4555 Overlook Ave.
 *               Washington, D.C. 20375-5339
 *
 * For more information please send email to simdis@enews.nrl.navy.mil
 *
 * February 21, 2006 - U.S. Naval Research Laboratory.
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 ****************************************************************************
 *
 *
 */
#ifndef _DCSSCOPEDATA_H_
#define _DCSSCOPEDATA_H_

#include "Constants/ConstantsVariableType.h"
#include "Constants/ConstantsUnits.h"
#include "DCSBaseData.h"

///Scope Gate Data struct
/**
 * Struct contains information for the gates in each Scope
 * display for Plot-XY 2-D Plot
 * Each DCSScopeGateData instance is owned by a single instance of DCSScopeData.
 * This instance of DCSScopeData defines the units of start_, center_, and end_
 * through the DCSScopeData::xUnits_ field.  
 */
struct DCSScopeGateData
{
    uint16_t	id_;		///<The id of this Scope Gate, unique between the Gates aggregated by an instance of ScopeData
    double	start_;		///<The starting x position of the gate (units=DCSScopeData::xUnits_)
    double	center_;	///<The "center" x position of the gate (units=DCSScopeData::xUnits_, not necesarily halfway between start and end)
    double	end_;		///<The ending x position of the gate (units=DCSScopeData::xUnits_)

    ///Constructor for scope gate data
    DCSScopeGateData(uint16_t id=0, double start=0, double center=0, double end=0):
      id_(id), start_(start), center_(center), end_(end){;}
};

///Scope Data class
/**
 * Class containing information for updating and displaying
 * Scope data within a plot-XY 2-D Plot.
 */
class DCSScopeData : public DCSBaseData
{
  public:

    ///X Units
    UTILS::UnitType_t   xUnits_;	/**< The units of the x axis.  Units from Constants/ConstantsUnits.h default: CU_METERS*/
    ///Y Units
    UTILS::UnitType_t   yUnits_;	/**< The units of the y axis.  Units from Constants/ConstantsUnits.h default: CU_VOLT*/
    ///Time
    double    time_;		/**< The time of this data point*/
    ///x Start
    double    xStart_;		/**< The starting value for the x axis*/
    ///x Step
    double    xStep_;		/**< The x-axis step size*/
    ///y Bias
    double    yBias_;		/**< The y-axis bias*/
    ///y Scale
    double    yScale_;		/**< The scale of the y axis*/
    ///Num Data Points
    uint32_t  numDataPoints_;	/**< The number of data points in the data array*/
    ///Data Type
    UTILS::VariableType_t dataType_;	/**< The data Type of the data array*/
    ///Num Gates
    uint16_t  numGates_;	/**< The number of gates in the gates array*/
    ///Data Array
    void*     data_;		/**< The data array*/
    ///Gates Array
    DCSScopeGateData* gates_;	/**< The gate(s) attached to this scope*/


  protected:

    ///Creates a copy of gate data
    void copyGates_ (DCSScopeGateData** destPtr, const DCSScopeGateData* src, int numGates) const;
    ///Creates a copy of number data
    void copyData_ (void** destPtr, const void* src, int numElements, UTILS::VariableType_t dataType) const;
    ///Deletes the data point and sets it to NULL
    void deleteData_ (void** delPtr, UTILS::VariableType_t dataType) const;


  public:
    ///Default scope data constructor.  
    /**
     * Creates a beam data object with default data values.
     */
    DCSScopeData();

    ///Beam data constructor.  
    /**
     * Creates a scope data object with the specified data values.
     * @param id an unsigned 64-bit integer specifying the object's id.  
     * @param xUnits an int16 specifying the object's x-units.
     * @param yUnits an int16 specifying the object's y-units.
     * @param time a double specifying the object's time.
     * @param xStart a double specifying the start of the object's x values.
     * @param xStep a double specifying the object's x step.
     * @param yBias a double specifying the object's y bias.
     * @param yScale a double specifying the object's y scale.
     * @param numDataPoints an int32 specifying the number of data points in the data array.
     * @param dataType an int8 specifying the data type of the object's array data.
     * @param numGates an unsigned int16 specifying the object's number of gates.
     * @param data a char array containing the object's data points.
     * @param gates a DCSScopeGateData array containing the object's gates.
     */
    DCSScopeData(uint64_t id, UTILS::UnitType_t xUnits, UTILS::UnitType_t yUnits, double time, double xStart, double xStep,
		 double yBias, double yScale, uint32_t numDataPoints, UTILS::VariableType_t dataType,
		 uint16_t numGates, const void* data, const DCSScopeGateData* gates);

    virtual ~DCSScopeData();

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
    virtual int32_t dataSize() const;

    ///Determine object's physical size.
    /**
     * @see DCSBaseData::sizeOf()
     */
    virtual int32_t sizeOf() const {return sizeof(*this);}
};

#endif /* _DCSSCOPEDATA_H_ */
