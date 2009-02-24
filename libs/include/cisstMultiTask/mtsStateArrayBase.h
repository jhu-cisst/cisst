/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#include <cisstCommon/cmnGenericObject.h>

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

public:
    typedef unsigned int index_type;
    typedef unsigned int size_type;

	/*! Default destructor. Does nothing. */
	inline virtual ~mtsStateArrayBase(void) {};

	/*! Overloaded subscript operator. */
	virtual cmnGenericObject & operator[](index_type index) = 0;

	/*! Overloaded subscript operator. */
	virtual const cmnGenericObject & operator[](index_type index) const = 0;
    
	/*! Create the array of data. */
	virtual mtsStateArrayBase * Create(const cmnGenericObject * objectExample, size_type size) = 0;

	/*! Copy data from one index to another. */
	virtual void Copy(index_type indexTo, index_type indexFrom) = 0;

	/*! Get data from array. */
	virtual bool Get(index_type index, cmnGenericObject & data) const = 0;

	/*! Set data in array. */
	virtual bool Set(index_type index, const cmnGenericObject & data) = 0;

	/*! Get data vector from array. */
	virtual bool GetVector(index_type indexStart, index_type indexEnd, cmnGenericObject & data) const = 0;
};


#endif // _mtsStateArrayBase_h

