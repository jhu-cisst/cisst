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
  \brief Defines a state data array used in a state table.
*/

#ifndef _mtsStateArray_h
#define _mtsStateArray_h

#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstMultiTask/mtsStateArrayBase.h>
#include <cisstMultiTask/mtsVector.h>

#include <vector>
#include <typeinfo>

/*!
  \ingroup cisstMultiTask

  Individual state array classes can be created from an instance of
  the following template, where _elementType represents the type of
  data used by the particular state element. It is assumed that
  _elementType is derived from cmnGenericObject.
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

    
	/*! Overloaded [] operator. Returns data at index */
	inline virtual cmnGenericObject & operator[](index_type index){
        return Data[index];
    }
    
    
	inline virtual const cmnGenericObject & operator[](index_type index) const {
        return Data[index];
    }

    
	/* Create the array of data. */
	inline virtual mtsStateArrayBase * Create(const cmnGenericObject * objectExample,
                                              size_type size) {
        const value_type * typedObjectExample = dynamic_cast<const value_type *>(objectExample);
        if (typedObjectExample) {
            this->Data.resize(size, *typedObjectExample);
        } else {
            CMN_LOG(1) << "mtsStateArray: Create used with an object example of the wrong type, received: "
                       << objectExample->Services()->GetName()
                       << " while expecting "
                       << value_type::ClassServices()->GetName()
                       << std::endl;
            return 0;
        }
        return this;
    }

    
	/*! Copy data from one index to another within the same array.  */
	inline virtual void Copy(index_type indexTo, index_type indexFrom) {
        this->Data[indexTo] = this->Data[indexFrom];
    }
    

	/*! Get and Set data from array.  The Get and Set member functions
	  deserve special mention because they must overcome a limitation
	  of C++ -- namely, that it does not fully support containers of
	  heterogeneous objects. In particular, we expect the 'object'
	  parameter to be of type _elementType& (the derived class) rather
	  than cmnGenericObject& (the base class). This can be handled
	  using C++ Run Time Type Information (RTTI) features such as
	  dynamic cast.
	 */
    //@{
	virtual bool Get(index_type index, cmnGenericObject & object) const;
	virtual bool Set(index_type index, const cmnGenericObject & object);

    /*! GetVector gets a vector of data. Here, we use RTTI to make sure that object
        is of type mtsVector<_elementType> and is large enough. */
	virtual bool GetVector(index_type indexStart, index_type indexEnd, cmnGenericObject & object) const;
    //@}
};


#include <iostream>

template <class _elementType>
bool mtsStateArray<_elementType>::Set(index_type index,  const cmnGenericObject & object) {
	//do some typechecking?? should this be an ASSERT?
	//TODO: check if throw works
	if (typeid(object) != typeid(_elementType)) {
		CMN_LOG(5) << "Class mtsStateArray: Set(): The passed object is not of the same kind as array. Expected: "
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
		CMN_LOG(5) << "Class mtsStateArray: Set(): Found NULL element in state data array" << std::endl;
	}
	return false;
}

template <class _elementType>
bool mtsStateArray<_elementType>::Get(index_type index, cmnGenericObject & object) const {
	//do some typechecking?? should this be an ASSERT?
	if (typeid(object) != typeid(_elementType)) {
		CMN_LOG(5) << "Class mtsStateArray: Get(): The passed object is not of the same kind as array. Expected: "
                   << typeid(_elementType).name() 
                   << "Got: " << typeid(object).name() << std::endl;
		return false;
	}
	_elementType* pdata = dynamic_cast<_elementType*>(&object);
	//_elementType* pdata = &object;
	if (pdata) {
		*pdata = Data[index];
		return true;
	} else {
		CMN_LOG(5) << "Class mtsStateArray: Get(): Found NULL element in state data array" << std::endl;
	}
	return false;
}

template <class _elementType>
bool mtsStateArray<_elementType>::GetVector(index_type indexStart, index_type indexEnd, cmnGenericObject &object) const {
    // PK: why is this typeid check necessary?  The dynamic_cast should fail.
    if (typeid(object) != typeid(mtsVector<_elementType>)) {
		CMN_LOG(5) << "Class mtsStateArray: GetVector(): The passed object is not an mtsVector of the appropriate type. Expected: "
                   << typeid(mtsVector<_elementType>).name() 
                   << "Got: " << typeid(object).name() << std::endl;
		return false;
	}
	mtsVector<_elementType>* pdata = dynamic_cast<mtsVector<_elementType>*>(&object);
    if (!pdata) {
		CMN_LOG(5) << "Class mtsStateArray: GetVector(): dynamic_cast failure" << std::endl;
        return false;
    }
    // Make sure vector is big enough
    unsigned int numToCopy = (Data.size() + indexEnd - indexStart + 1)%Data.size();
    if (pdata->size() < numToCopy) {
		CMN_LOG(1) << "Class mtsStateArray: GetVector(): provided array too small, size = "
                   << pdata->size() << ", requested copy = " << numToCopy << std::endl;
        return false;
    }
    // PK: probably should use iterators instead
    unsigned int i, j;
    if (indexEnd < indexStart) {  // wrap-around case
        for (i=0, j=indexStart; j < Data.size(); i++, j++)
            (*pdata)[i] = Data[j];
        for (j=0; j <= indexEnd; i++, j++)
            (*pdata)[i] = Data[j];
    }
    else {
        for (i=0; i < numToCopy; i++)
            (*pdata)[i] = Data[indexStart+i];
    }
	return true;
}


#endif // _mtsStateArray_h

