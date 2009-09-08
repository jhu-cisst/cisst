/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides
  Created on: 2004-04-30

  (C) Copyright 2004-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines an index used for mtsStateTable.
*/

#ifndef _mtsStateIndex_h
#define _mtsStateIndex_h

#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>

#include <cisstMultiTask/mtsExport.h>


/*!  
  \ingroup cisstMultiTask

  The time indexing of state data is provided by the mtsStateIndex
  class.

  Each iteration of the Task Loop increments a tick counter, which is
  stored as an unsigned long.  (mtsTimeTicks). A mtsStateIndex object
  contains the following information: The tick value is stored in the
  mtsStateIndex class to act as a data validity check. This is used to
  ensure that the circular buffer has not wrapped around and
  overwritten the index entry between the time that the mtsStateIndex
  object was created and the time it was used.
 */
class CISST_EXPORT mtsStateIndex : public mtsGenericObject {

    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

 public:
    /*! TimeTicks are typedef'ed as unsigned long */
    typedef unsigned long long int TimeTicksType;
    
 private:
	/*! The index into the set of circular buffers corresponding to
      the time */
	int TimeIndex;

	/*! The tick value corresponding to the time. */
	TimeTicksType TimeTicks;

	/*! The size of the circular buffer. */
	int BufferLength;

public:
	/*! Default constructor. Does nothing. */
	mtsStateIndex(): TimeIndex(0), TimeTicks(0), BufferLength(0) {}
    
	/*! Default constructor. Does nothing. */
	mtsStateIndex(int index, TimeTicksType ticks, int Length):
        TimeIndex(index), TimeTicks(ticks), BufferLength(Length) {}
    
	/*! Default destructor. Does nothing. */
	~mtsStateIndex() {}

    virtual void ToStream(std::ostream &out) const {
        out << "Index = " << TimeIndex << ", Ticks = " << Ticks()
            << ", Length = " << BufferLength << std::endl;
    }

    /*! The length of the circular buffer. */
    int Length(void) const {
        return BufferLength;
    }

    //*! Return the index into the circular buffer.*/
	int Index(void) const {
       return TimeIndex;
    }

	/*! Return the ticks corresponding to the time. */
	TimeTicksType Ticks(void) const {
        return TimeTicks;
    }

	/*! 
	    Note that the increment operators are not defined for
	    this class, since we dont want future times.
	 */
	mtsStateIndex& operator--() {
		TimeTicks--;
		TimeIndex--;
		if (TimeIndex<0) {
            TimeIndex = BufferLength-1;
        }
		return *this;
	}

	/*! Overloaded operator */
	mtsStateIndex& operator-=(int number) {
		TimeTicks -= number;
		TimeIndex = TimeTicks % BufferLength;
		return *this;
	}

	/*! Overloaded operator */
	mtsStateIndex operator-(int number) {
		mtsStateIndex tmp = *this;
		return (tmp -= number);
	}

	/*! The comparison operators */
	bool operator==(const mtsStateIndex & that) const {
		return (TimeIndex == that.TimeIndex && TimeTicks == that.TimeTicks);
	}

	/*! Overloaded operator */
	bool operator!=(const mtsStateIndex & that) const {
		return !(*this == that);
	}

    /*! Serialize the content of the object without any extra
      information, i.e. no class type nor format version.  The
      "receiver" is supposed to already know what to expect. */ 
    virtual void SerializeRaw(std::ostream & outputStream) const {
        cmnSerializeRaw(outputStream, this->TimeIndex);
        cmnSerializeRaw(outputStream, this->TimeTicks);
        cmnSerializeRaw(outputStream, this->BufferLength);
    }

    /*! De-serialize the content of the object without any extra
      information, i.e. no class type nor format version. */
    virtual void DeSerializeRaw(std::istream & inputStream) {
        cmnDeSerializeRaw(inputStream, this->TimeIndex);
        cmnDeSerializeRaw(inputStream, this->TimeTicks);
        cmnDeSerializeRaw(inputStream, this->BufferLength);
    }
};


#endif // _mtsStateIndex_h

CMN_DECLARE_SERVICES_INSTANTIATION(mtsStateIndex)

