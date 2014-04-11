/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
#include <cisstMultiTask/mtsGenericObjectProxy.h>
#include <cisstMultiTask/mtsStateArrayBase.h>

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


    bool SetDataSize(const size_t size){
        value_type objectExample = Data[0];
        //\todo add try catch for alloc exception
        Data.resize(size,objectExample);
        return true;
    }


    /*! Access element at index. This returns the data of the derived type
      (value_type) rather than the base type (mtsGenericObject), which is
      returned by the overloaded operator []. */
    const value_type & Element(index_type index) const { return Data[index]; }
    value_type & Element(index_type index) { return Data[index]; }

	/*! Overloaded [] operator. Returns data at index (of type mtsGenericObject).
        Currently used for data collection (mtsCollectorState). */
	inline mtsGenericObject & operator[](index_type index){ return Data[index]; }
	inline const mtsGenericObject & operator[](index_type index) const { return Data[index]; }

	/* Create the array of data. This is currently unused. */
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
};


#include <iostream>

template <class _elementType>
bool mtsStateArray<_elementType>::Set(index_type index,  const mtsGenericObject & object)
{
    // Case 1: The state table entry was derived from mtsGenericObject
    const _elementType *pdata = dynamic_cast<const _elementType *>(&object);
    if (pdata) {
		Data[index] = *pdata;
		return true;
    }
    // Case 2: The state table entry was not derived from mtsGenericObject, so it was wrapped
    typedef typename mtsGenericTypesUnwrap<_elementType>::RefType RefType;
    const RefType* pref = dynamic_cast<const RefType*>(&object);
	if (pref) {
		Data[index] = *pref;
		return true;
	}
    CMN_LOG_RUN_ERROR << "mtsStateArray::Set -- type mismatch, expected " << typeid(_elementType).name() << std::endl;
	return false;
}

// PK: obsolete: use accessor class instead
template <class _elementType>
bool mtsStateArray<_elementType>::Get(index_type index, mtsGenericObject & object) const {
	_elementType* pdata = dynamic_cast<_elementType*>(&object);
	if (pdata) {
		*pdata = Data[index];
		return true;
    }
    CMN_LOG_RUN_ERROR << "mtsStateArray::Get -- type mismatch, expected " << typeid(_elementType).name() << std::endl;
	return false;
}

#endif // _mtsStateArray_h

