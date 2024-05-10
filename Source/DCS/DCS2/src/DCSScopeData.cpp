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
#include "inttypesc.h"
#include "DCSswapbytes.h"
#include "DCSdefs.h"
#include "DCSPoint.h"
#include "DCSBaseData.h"
#include "DCSScopeData.h"
#include "Constants/ConstantsVariableType.h"

using namespace UTILS;



DCSScopeData::DCSScopeData()
  : DCSBaseData(DCSSCOPEDATA),
    xUnits_(CU_METERS),
    yUnits_(CU_VOLT),
    time_(0.0),
    xStart_(0.0),
    xStep_(0.0),
    yBias_(0.0),
    yScale_(0.0),
    numDataPoints_(0),
    dataType_(UTILS::vtUINT8),
    numGates_(0),
    data_(NULL),
    gates_(NULL)
    
{
}

DCSScopeData::DCSScopeData(uint64_t id, UTILS::UnitType_t xUnits, UTILS::UnitType_t yUnits, double time, double xStart, double xStep,
			   double yBias, double yScale, uint32_t numDataPoints, UTILS::VariableType_t dataType,
			   uint16_t numGates, const void* data, const DCSScopeGateData* gates)
  : DCSBaseData(id,DCSSCOPEDATA),
    xUnits_(xUnits),
    yUnits_(yUnits),
    time_(time),
    xStart_(xStart),
    xStep_(xStep),
    yBias_(yBias),
    yScale_(yScale),
    numDataPoints_(numDataPoints),
    dataType_(dataType),
    numGates_(numGates),
    data_(NULL),
    gates_(NULL)
{
  if (numGates > 0)
    copyGates_ (&gates_, gates, numGates);
  if (numDataPoints > 0)
    copyData_ (&data_, data, numDataPoints, dataType);
}

DCSScopeData::~DCSScopeData()
{
  deleteData_ (&data_, dataType_);
  delete[] gates_;
  gates_ = NULL;
}

void DCSScopeData::clear()
{
  DCSBaseData::clear();
  xUnits_ = CU_METERS;
  yUnits_ = CU_VOLT;
  time_ = 0.0;
  xStart_ = 0.0;
  xStep_ = 0.0;
  yBias_ = 0.0;
  yScale_ = 0.0;
  numDataPoints_ = 0;
  deleteData_ (&data_, dataType_);
  dataType_ = UTILS::vtUINT8;
  numGates_ = 0;
  delete [] gates_;
  gates_ = NULL;
}

void* DCSScopeData::pack(void* buffer,int32_t size) const
{
  if (dataSize() > size)
    return NULL;

  char* data=(char*)buffer;

  data+=packit(data);
  uint16_t unitsTmp = (uint16_t)xUnits_;
  data+=bewrite(data,&unitsTmp)*sizeof(unitsTmp);
  unitsTmp = (uint16_t)yUnits_;
  data+=bewrite(data,&unitsTmp)*sizeof(unitsTmp);
  data+=bewrite(data,&time_)*sizeof(time_);
  data+=bewrite(data,&xStart_)*sizeof(xStart_);
  data+=bewrite(data,&xStep_)*sizeof(xStep_);
  data+=bewrite(data,&yBias_)*sizeof(yBias_);
  data+=bewrite(data,&yScale_)*sizeof(yScale_);
  data+=bewrite(data,&numDataPoints_)*sizeof(numDataPoints_);
  uint8_t dataTypePlaceholder = (uint8_t)dataType_;
  data+=bewrite(data,&dataTypePlaceholder)*sizeof(dataTypePlaceholder);
  data+=bewrite(data,&numGates_)*sizeof(numGates_);
  
  // write out the data points
  if (numDataPoints_ == 0)
    return buffer;

  switch (dataType_)
  {
    case UTILS::vtUINT8:
    {
      uint32_t i = 0;
      uint8_t *dataArray = (uint8_t*) data_;
      for (i=0; i < numDataPoints_*sizeof(uint8_t); i+=sizeof(uint8_t))
      {
	data+=bewrite(data, dataArray)*sizeof(uint8_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtINT8:
    {
      uint32_t i = 0;
      int8_t *dataArray = (int8_t*) data_;
      for (i=0; i < numDataPoints_*sizeof(int8_t); i+=sizeof(int8_t))
      {
	data+=bewrite(data, dataArray)*sizeof(int8_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtUINT16:
    {
      uint32_t i = 0;
      uint16_t *dataArray = (uint16_t*) data_;
      for (i=0; i < numDataPoints_*sizeof(uint16_t); i+=sizeof(uint16_t))
      {
	data+=bewrite(data, dataArray)*sizeof(uint16_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtINT16:
    {
      uint32_t i = 0;
      int16_t *dataArray = (int16_t*) data_;
      for (i=0; i < numDataPoints_*sizeof(int16_t); i+=sizeof(int16_t))
      {
	data+=bewrite(data, dataArray)*sizeof(int16_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtUINT32:
    {
      uint32_t i = 0;
      uint32_t *dataArray = (uint32_t*) data_;
      for (i=0; i < numDataPoints_*sizeof(uint32_t); i+=sizeof(uint32_t))
      {
	data+=bewrite(data, dataArray)*sizeof(uint32_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtINT32:
    {
      uint32_t i = 0;
      int32_t *dataArray = (int32_t*) data_;
      for (i=0; i < numDataPoints_*sizeof(int32_t); i+=sizeof(int32_t))
      {
	data+=bewrite(data, dataArray)*sizeof(int32_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtUINT64:
    {
      uint32_t i = 0;
      uint64_t *dataArray = (uint64_t*) data_;
      for (i=0; i < numDataPoints_*sizeof(uint64_t); i+=sizeof(uint64_t))
      {
	data+=bewrite(data, dataArray)*sizeof(uint64_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtINT64:
    {
      uint32_t i = 0;
      int64_t *dataArray = (int64_t*) data_;
      for (i=0; i < numDataPoints_*sizeof(int64_t); i+=sizeof(int64_t))
      {
	data+=bewrite(data, dataArray)*sizeof(int64_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtFLOAT32:
    {
      uint32_t i = 0;
      float *dataArray = (float*) data_;
      for (i=0; i < numDataPoints_*sizeof(float); i+=sizeof(float))
      {
	data+=bewrite(data, dataArray)*sizeof(float);
	dataArray++;
      }
      break;
    }
    case UTILS::vtFLOAT64:
    {
      uint32_t i = 0;
      double *dataArray = (double*) data_;
      for (i=0; i < numDataPoints_*sizeof(double); i+=sizeof(double))
      {
	data+=bewrite(data, dataArray)*sizeof(double);
	dataArray++;
      }
      break;
    }
    default:
      // dataType unrecognized, do not write any data
      break;
  }

  //Write out the Gates
  int i = 0;
  for (i=0; i<numGates_; i++)
  {
    DCSScopeGateData *gate = gates_;
    data+=bewrite(data, &(gate->id_))*sizeof(uint16_t);
    data+=bewrite(data, &(gate->start_))*sizeof(double);
    data+=bewrite(data, &(gate->center_))*sizeof(double);
    data+=bewrite(data, &(gate->end_))*sizeof(double);
    gate++;
  }
  
  //return buffer
  return buffer;
}


void DCSScopeData::unpack(const void* buffer,int32_t size)
{
  char* data=(char*)buffer;

  data+=unpackit(data);
  uint16_t unitsTmp;
  data+=beread(data,&unitsTmp)*sizeof(unitsTmp);
  xUnits_ = (UTILS::UnitType_t)unitsTmp;
  data+=beread(data,&unitsTmp)*sizeof(unitsTmp);
  yUnits_ = (UTILS::UnitType_t)unitsTmp;
  data+=beread(data,&time_)*sizeof(time_);
  data+=beread(data,&xStart_)*sizeof(xStart_);
  data+=beread(data,&xStep_)*sizeof(xStep_);
  data+=beread(data,&yBias_)*sizeof(yBias_);
  data+=beread(data,&yScale_)*sizeof(yScale_);
  data+=beread(data,&numDataPoints_)*sizeof(numDataPoints_);
  uint8_t dataTypePlaceholder;
  data+=beread(data,&dataTypePlaceholder)*sizeof(dataTypePlaceholder);
  dataType_ = (UTILS::VariableType_t)dataTypePlaceholder;
  data+=beread(data,&numGates_)*sizeof(numGates_);


  if (numDataPoints_ == 0)
    return;
    
  switch (dataType_)
  {
    case UTILS::vtUINT8:
    {
      uint32_t i = 0;
      uint8_t *dataArray = new uint8_t[numDataPoints_];
      data_ = (char*)dataArray;
      for (i=0; i < numDataPoints_; i++)
      {
	data+=beread(data, dataArray)*sizeof(uint8_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtINT8:
    {
      uint32_t i = 0;
      int8_t *dataArray = new int8_t[numDataPoints_];
      data_ = (char*)dataArray;
      for (i=0; i < numDataPoints_; i++)
      {
	data+=beread(data, dataArray)*sizeof(int8_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtUINT16:
    {
      uint32_t i = 0;
      uint16_t *dataArray = new uint16_t[numDataPoints_];
      data_ = (char*)dataArray;
      for (i=0; i < numDataPoints_; i++)
      {
	data+=beread(data, dataArray)*sizeof(uint16_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtINT16:
    {
      uint32_t i = 0;
      int16_t *dataArray = new int16_t[numDataPoints_];
      data_ = (char*)dataArray;
      for (i=0; i < numDataPoints_; i++)
      {
	data+=beread(data, dataArray)*sizeof(int16_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtUINT32:
    {
      uint32_t i = 0;
      uint32_t *dataArray = new uint32_t[numDataPoints_];
      data_ = (char*)dataArray;
      for (i=0; i < numDataPoints_; i++)
      {
	data+=beread(data, dataArray)*sizeof(uint32_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtINT32:
    {
      uint32_t i = 0;
      int32_t *dataArray = new int32_t[numDataPoints_];
      data_ = (char*)dataArray;
      for (i=0; i < numDataPoints_; i++)
      {
	data+=beread(data, dataArray)*sizeof(int32_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtUINT64:
    {
      uint32_t i = 0;
      uint64_t *dataArray = new uint64_t[numDataPoints_];
      data_ = (char*)dataArray;
      for (i=0; i < numDataPoints_; i++)
      {
	data+=beread(data, dataArray)*sizeof(uint64_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtINT64:
    {
      uint32_t i = 0;
      int64_t *dataArray = new int64_t[numDataPoints_];
      data_ = (char*)dataArray;
      for (i=0; i < numDataPoints_; i++)
      {
	data+=beread(data, dataArray)*sizeof(int64_t);
	dataArray++;
      }
      break;
    }
    case UTILS::vtFLOAT32:
    {
      uint32_t i = 0;
      float *dataArray = new float[numDataPoints_];
      data_ = (char*)dataArray;
      for (i=0; i < numDataPoints_; i++)
      {
	data+=beread(data, dataArray)*sizeof(float);
	dataArray++;
      }
      break;
    }
    case UTILS::vtFLOAT64:
    {
      uint32_t i = 0;
      double *dataArray = new double[numDataPoints_];
      data_ = (char*)dataArray;
      for (i=0; i < numDataPoints_; i++)
      {
	data+=beread(data, dataArray)*sizeof(double);
	dataArray++;
      }
      break;
    }
    
    default:
      // dataType unrecognized, do not write any data
      data_ = NULL;
      break;
  }

  
  //Read in the Gates
  if (numGates_ == 0)
  {
    gates_ = NULL;
    return;
  }
  
  gates_ = new DCSScopeGateData[numGates_];
  
  int i = 0;
  for (i=0; i<numGates_; i++)
  {
    data+=beread(data, &(gates_[i].id_))*sizeof(uint16_t);
    data+=beread(data, &(gates_[i].start_))*sizeof(double);
    data+=beread(data, &(gates_[i].center_))*sizeof(double);
    data+=beread(data, &(gates_[i].end_))*sizeof(double);
  }

}

int32_t DCSScopeData::dataSize() const
{
  return DCSBaseData::dataSize() + sizeof(uint16_t) +
    sizeof(uint16_t) + sizeof(time_) + sizeof(xStart_) +
    sizeof(xStep_) + sizeof(yBias_) + sizeof(yScale_) +
    sizeof(numDataPoints_) + sizeof(uint8_t) /* data type */ +
    sizeof(numGates_) +
    UTILS::getSizeOf(dataType_) * numDataPoints_ +
    sizeof(DCSScopeGateData) * numGates_;
}

void DCSScopeData::copyGates_ (DCSScopeGateData** destPtr, const DCSScopeGateData* src, int numGates) const
{
  if (*destPtr)
    delete[] *destPtr;
  *destPtr = NULL;
  if (numGates)
  {
    *destPtr = new DCSScopeGateData[numGates];
    int k;
    for (k = 0; k < numGates; ++k)
      (*destPtr)[k] = src[k];
  }
}

void DCSScopeData::copyData_ (void** destPtr, const void* src, int numElements, UTILS::VariableType_t dataType) const
{
  deleteData_ (destPtr, dataType);
  if (numElements)
  {
    // Switch is needed for data allocation
    switch (dataType)
    {
      case UTILS::vtUINT8:
	*destPtr = new uint8_t[numElements];
	break;
      case UTILS::vtINT8:
	*destPtr = new int8_t[numElements];
	break;
      case UTILS::vtUINT16:
	*destPtr = new uint16_t[numElements];
	break;
      case UTILS::vtINT16:
	*destPtr = new int16_t[numElements];
	break;
      case UTILS::vtUINT32:
	*destPtr = new uint32_t[numElements];
	break;
      case UTILS::vtINT32:
	*destPtr = new int32_t[numElements];
	break;
      case UTILS::vtUINT64:
	*destPtr = new uint64_t[numElements];
	break;
      case UTILS::vtINT64:
	*destPtr = new int64_t[numElements];
	break;
      case UTILS::vtFLOAT32:
	*destPtr = new float[numElements];
	break;
      case UTILS::vtFLOAT64:
	*destPtr = new double[numElements];
	break;
      default:
	*destPtr = NULL;
	break;
    } // switch on data type
    // Copy the data
    if (*destPtr)
      memcpy (*destPtr, src, numElements * UTILS::getSizeOf (dataType));
  } // If numElements
}

void DCSScopeData::deleteData_ (void** delPtr, UTILS::VariableType_t dataType) const
{
  if (*delPtr != NULL)
  {
    switch (dataType)
    {
      case UTILS::vtUINT8:
      {
	uint8_t* val = (uint8_t*)(*delPtr);
	delete[] val;
	break;
      }
      case UTILS::vtINT8:
      {
	int8_t* val = (int8_t*)(*delPtr);
	delete[] val;
	break;
      }
      case UTILS::vtUINT16:
      {
	uint16_t* val = (uint16_t*)(*delPtr);
	delete[] val;
	break;
      }
      case UTILS::vtINT16:
      {
	int16_t* val = (int16_t*)(*delPtr);
	delete[] val;
	break;
      }
      case UTILS::vtUINT32:
      {
	uint32_t* val = (uint32_t*)(*delPtr);
	delete[] val;
	break;
      }
      case UTILS::vtINT32:
      {
	int32_t* val = (int32_t*)(*delPtr);
	delete[] val;
	break;
      }
      case UTILS::vtUINT64:
      {
	uint64_t* val = (uint64_t*)(*delPtr);
	delete[] val;
	break;
      }
      case UTILS::vtINT64:
      {
	int64_t* val = (int64_t*)(*delPtr);
	delete[] val;
	break;
      }
      case UTILS::vtFLOAT32:
      {
	float* val = (float*)(*delPtr);
	delete[] val;
	break;
      }
      case UTILS::vtFLOAT64:
      {
	double* val = (double*)(*delPtr);
	delete[] val;
	break;
      }
      default:
	// Cannot delete unknown data
	return;
    } // switch on data type
  } // If delPtr != NULL
  *delPtr = NULL;
}
