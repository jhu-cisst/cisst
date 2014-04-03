/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Ankur Kapoor
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
  \brief Defines a base class the state data array.
*/

#ifndef _mtsStateArrayBase_h
#define _mtsStateArrayBase_h

#include <cisstMultiTask/mtsGenericObject.h>

/*!
  \ingroup cisstMultiTask

  Abstract base class for state data arrays.  This class is mostly
  pure virtual and is used as a base class for mtsStateArray which is
  templated.  Using a base class allows to keep all the state arrays
  in an homogenous container of pointers on different types of state
  arrays.

  \sa mtsStateArray */
class mtsStateArrayBase {
protected:
    /*! Protected constructor. Does nothing. */
    inline mtsStateArrayBase(void){};

    /*! Class services associated to the element contained */
    const cmnClassServicesBase * DataClassServices;

public:
    typedef size_t index_type;
    typedef size_t size_type;

    /*! Default destructor. Does nothing. */
    inline virtual ~mtsStateArrayBase(void) {};

    /*! Overloaded subscript operator. */
    virtual mtsGenericObject & operator[](index_type index) = 0;

	/*! Overloaded subscript operator. */
	virtual const mtsGenericObject & operator[](index_type index) const = 0;

	/*! Create the array of data.  This is currently unused. */
	virtual mtsStateArrayBase * Create(const mtsGenericObject * objectExample, size_type size) = 0;

	/*! Copy data from one index to another. */
	virtual void Copy(index_type indexTo, index_type indexFrom) = 0;

	/*! Get data from array. */
	virtual bool Get(index_type index, mtsGenericObject & data) const = 0;

	/*! Set data in array. */
	virtual bool Set(index_type index, const mtsGenericObject & data) = 0;

    virtual bool SetDataSize(const size_t size) = 0;

    bool SetSize(const size_t size){
        return SetDataSize(size);
    }

};


#endif // _mtsStateArrayBase_h

