/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor
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
  \brief Defines a state data array used in a state table.
*/

#ifndef _mtsStateArray_h
#define _mtsStateArray_h

#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstMultiTask/mtsStateArrayBase.h>
#include <cisstMultiTask/mtsHistory.h>

#include <vector>
#include <typeinfo>

/*!
  \ingroup cisstMultiTask

  Individual state array classes can be created from an instance of
  the following template, where _elementType represents the type of
  data used by the particular state element. It is assumed that
  _elementType is derived from mtsGenericObject.
 */
template <class _elementType>
class mtsStateArray :public mtsStateArrayBase
{
public:
    typedef _elementType value_type;
    // PK: why not use vctDynamicVector or mtsVector instead of std::vector?
    typedef std::vector<value_type> VectorType;
    typedef typename VectorType::iterator iterator;
    typedef typename VectorType::const_iterator const_iterator;

protected:
	/*! A vector to store the data. These element of the vector
	  represents the cell of the state data table. */
	VectorType Data;

public:
	/*! Default constructor. Does nothing */
	inline mtsStateArray(const value_type & objectExample,
                         size_type size = 0):
        Data(size, objectExample)
    {}
    
    
	/*! Default destructor. */
	virtual ~mtsStateArray() {}

    /*! Access element at index. This returns the data of the derived type
      (value_type) rather than the base type (mtsGenericObject), which is
      returned by the overloaded operator []. */
    const value_type & Element(index_type index) const { return Data[index]; }
    value_type & Element(index_type index) { return Data[index]; }

	/*! Overloaded [] operator. Returns data at index (of type mtsGenericObject). */
	inline mtsGenericObject & operator[](index_type index){ return Data[index]; }
	inline const mtsGenericObject & operator[](index_type index) const { return Data[index]; }
    
	/* Create the array of data. */
    inline mtsStateArrayBase * Create(const mtsGenericObject * objectExample,
                                      size_type size) {
        const value_type * typedObjectExample = dynamic_cast<const value_type *>(objectExample);
        if (typedObjectExample) {
            this->Data.resize(size, *typedObjectExample);
            this->DataClassServices = objectExample->Services();
            CMN_ASSERT(this->DataClassServices);
        } else {
            CMN_LOG_INIT_ERROR << "mtsStateArray: Create used with an object example of the wrong type, received: "
                               << objectExample->Services()->GetName()
                               << " while expecting "
                               << value_type::ClassServices()->GetName()
                               << std::endl;
            return 0;
        }
        return this;
    }

    
	/*! Copy data from one index to another within the same array.  */
	inline void Copy(index_type indexTo, index_type indexFrom) {
        this->Data[indexTo] = this->Data[indexFrom];
    }
    

	/*! Get and Set data from array.  The Get and Set member functions
	  deserve special mention because they must overcome a limitation
	  of C++ -- namely, that it does not fully support containers of
	  heterogeneous objects. In particular, we expect the 'object'
	  parameter to be of type _elementType& (the derived class) rather
	  than mtsGenericObject& (the base class). This can be handled
	  using C++ Run Time Type Information (RTTI) features such as
	  dynamic cast.
	 */
    //@{
	bool Get(index_type index, mtsGenericObject & object) const;
	bool Set(index_type index, const mtsGenericObject & object);
    //@}

	/*! Get data vector from array. */
    virtual bool GetHistory(index_type indexStart, index_type indexEnd, mtsHistory<_elementType> & data) const;
};


#include <iostream>

template <class _elementType>
bool mtsStateArray<_elementType>::Set(index_type index,  const mtsGenericObject & object) {
	//do some typechecking?? should this be an ASSERT?
	//TODO: check if throw works
	if (typeid(object) != typeid(_elementType)) {
		CMN_LOG_RUN_ERROR << "Class mtsStateArray: Set(): The passed object is not of the same kind as array. Expected: "
                          << typeid(_elementType).name()
                          << " Got: " << typeid(object).name() << std::endl;
		return false;
	}
	const _elementType* pdata = dynamic_cast<const _elementType*>(&object);
	//const _elementType* pdata = &object;
	if (pdata) {
		Data[index] = *pdata;
		return true;
	} else {
		CMN_LOG_RUN_ERROR << "Class mtsStateArray: Set(): Found NULL element in state data array" << std::endl;
	}
	return false;
}

template <class _elementType>
bool mtsStateArray<_elementType>::Get(index_type index, mtsGenericObject & object) const {
	//do some typechecking?? should this be an ASSERT?
	if (typeid(object) != typeid(_elementType)) {
		CMN_LOG_RUN_ERROR << "Class mtsStateArray: Get(): The passed object is not of the same kind as array. Expected: "
                          << typeid(_elementType).name() 
                          << " Got: " << typeid(object).name() << std::endl;
		return false;
	}
	_elementType* pdata = dynamic_cast<_elementType*>(&object);
	//_elementType* pdata = &object;
	if (pdata) {
		*pdata = Data[index];
		return true;
	} else {
		CMN_LOG_RUN_ERROR << "Class mtsStateArray: Get(): Found NULL element in state data array" << std::endl;
	}
	return false;
}

template <class _elementType>
bool mtsStateArray<_elementType>::GetHistory(index_type indexStart, index_type indexEnd,
                                             mtsHistory<_elementType> & data) const
{
    // Make sure vector is big enough
    unsigned int numToCopy = (Data.size() + indexEnd - indexStart + 1)%Data.size();
    if (data.size() < numToCopy) {
		CMN_LOG_INIT_ERROR << "Class mtsStateArray: GetHistory(): provided array too small, size = "
                           << data.size() << ", requested copy = " << numToCopy << std::endl;
        return false;
    }
    // PK: probably should use iterators instead (or perhaps a cisstVector fastcopy?)
    unsigned int i, j;
    if (indexEnd < indexStart) {  // wrap-around case
        for (i=0, j=indexStart; j < Data.size(); i++, j++)
            data[i] = Data[j];
        for (j=0; j <= indexEnd; i++, j++)
            data[i] = Data[j];
    }
    else {
        for (i=0; i < numToCopy; i++)
            data[i] = Data[indexStart+i];
    }
	return true;
}


#endif // _mtsStateArray_h

