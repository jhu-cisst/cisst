/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsStateIndex.h,v 1.6 2008/12/03 21:08:30 mbalicki Exp $

  Author(s):  Ankur Kapoor, Peter Kazanzides
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

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

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegister.h>

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
class CISST_EXPORT mtsStateIndex : public cmnGenericObject {

    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, 5);

 public:
    /*! TimeTicks are typedef'ed as unsigned long */
    typedef unsigned long TimeTicksType;
    
 private:
	/*! The index into the set of circular buffers corresponding to
      the time */
	int TimeIndex;

    //*! Return the index into the circular buffer.*/
	int Index(void) const {
       return TimeIndex;
    }

	/*! The tick value corresponding to the time. */
	TimeTicksType TimeTicks;

	/*! The size of the circular buffer. */
	int BufferLength;

    /*! State table needs to have access to the internals of this class (index specifically) */
    friend class mtsStateTable;

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
};


#endif // _mtsStateIndex_h

CMN_DECLARE_SERVICES_INSTANTIATION(mtsStateIndex)

