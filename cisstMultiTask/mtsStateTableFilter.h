/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Peter Kazanzides
  Created on: 2022-02-07

  (C) Copyright 2022 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines the state table filters, which are used by mtsInterfaceProvided.
*/

#ifndef _mtsStateTableFilter_h
#define _mtsStateTableFilter_h

#include <cisstMultiTask/mtsStateTable.h>

// Base class for keeping track of allocated filters
class mtsStateTableFilterBase {
public:
    mtsStateTableFilterBase() {}
    virtual ~mtsStateTableFilterBase() {}
};

// Following class contains common code to avoid duplication in mtsStateTableFilter
template <class _elementType, class _outputType>
class mtsStateTableFilterCommon : public mtsStateTableFilterBase
{
protected:
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;
    typedef typename mtsGenericTypes<_elementType>::FinalType FinalType;
    AccessorType *stateAccessor;
public:
    mtsStateTableFilterCommon(AccessorType *sa) : stateAccessor(sa) {}
    ~mtsStateTableFilterCommon() {}

    virtual bool GetValue(const _elementType &stateValue, _outputType &value) const = 0;

    bool GetFiltered(const mtsStateIndex & when, _outputType &value) const
    {
        FinalType stateValue;
        bool ret = stateAccessor->Get(when, stateValue);
        if (ret)
            ret = GetValue(_elementType(stateValue), value);
        return ret;
    }
    bool GetLatestFiltered(_outputType &value) const
    {
        FinalType stateValue;
        bool ret = stateAccessor->GetLatest(stateValue);
        if (ret)
            ret = GetValue(_elementType(stateValue), value);
        return ret;
    }
};

// Templated class, specialized for three supported filter methods (see below).
// This class defines the GetValue method that is called by the GetFiltered and
// GetLatestFiltered methods.

template <class _elementType, class _outputType, class _filterMethod>
class mtsStateTableFilter : public mtsStateTableFilterCommon<_elementType, _outputType>
{
public:
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;
    mtsStateTableFilter(AccessorType *sa, _filterMethod fm);
    ~mtsStateTableFilter();

    bool GetValue(const _elementType &stateValue, _outputType &value) const;
};

// Specialized for filter method of form:
//      bool InputType::ReadMethod(OutputType &) const
// where:
//      InputType is the class that was added to the State Table
//      OutputType is the class that will be returned by the command object

template<class _elementType, class _outputType>
class mtsStateTableFilter<_elementType, _outputType, bool (_elementType::*)(_outputType &) const> : public mtsStateTableFilterCommon<_elementType, _outputType>
{
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;
    typedef mtsStateTableFilterCommon<_elementType, _outputType> BaseType;
    typedef bool (_elementType::*ReadMethod)(_outputType &) const;
    ReadMethod readMethod;
public:
    mtsStateTableFilter(AccessorType *sa, ReadMethod rm) : BaseType(sa), readMethod(rm) {}
    ~mtsStateTableFilter() {}

    bool GetValue(const _elementType &stateValue, _outputType &value) const
    { return (stateValue.*readMethod)(value); }
};

// Specialized for filter method of form:
//      OutputType InputType::ReadMethod(void) const
// where:
//      InputType is the class that was added to the State Table
//      OutputType is the class that will be returned by the command object

template<class _elementType, class _outputType>
class mtsStateTableFilter<_elementType, _outputType, _outputType (_elementType::*)(void) const> : public mtsStateTableFilterCommon<_elementType, _outputType>
{
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;
    typedef mtsStateTableFilterCommon<_elementType, _outputType> BaseType;
    typedef _outputType (_elementType::*VoidReturnMethod)(void) const;
    VoidReturnMethod voidReturnMethod;
public:
    mtsStateTableFilter(AccessorType *sa, VoidReturnMethod vrm) : BaseType(sa), voidReturnMethod(vrm) {}
    ~mtsStateTableFilter() {}

    bool GetValue(const _elementType &stateValue, _outputType &value) const
    { value = (stateValue.*voidReturnMethod)(); return true; }
};

// Specialized for filter method of form:
//      bool ConvertMethod(const InputType &, OutputType &)
// where:
//      InputType is the class that was added to the State Table
//      OutputType is the class that will be returned by the command object

template<class _elementType, class _outputType>
class mtsStateTableFilter<_elementType, _outputType, bool (*)(const _elementType &, _outputType &)> : public mtsStateTableFilterCommon<_elementType, _outputType>
{
    typedef typename mtsStateTable::Accessor<_elementType> AccessorType;
    typedef mtsStateTableFilterCommon<_elementType, _outputType> BaseType;
    typedef bool (*ConvertFunction)(const _elementType &, _outputType &);
    ConvertFunction convertFunction;
public:
    mtsStateTableFilter(AccessorType *sa, ConvertFunction cf) : BaseType(sa), convertFunction(cf) {}
    ~mtsStateTableFilter() {}

    bool GetValue(const _elementType &stateValue, _outputType &value) const
    { return (*convertFunction)(stateValue, value); }
};

#endif // _mtsStateTableFilter_h
