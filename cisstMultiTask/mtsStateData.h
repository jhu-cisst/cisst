/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Peter Kazanzides

  (C) Copyright 2007-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Templated class used to define state data for a task.
*/

#ifndef _mtsStateData_h
#define _mtsStateData_h

#include <cisstMultiTask/mtsStateTable.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsTask.h>
#include <cisstMultiTask/mtsVector.h>

// PKAZ 3/17/09:  this entire class is deprecated and exists just for backward compatibility.
//
// This class handles the interface to the state table. Note that there are a few things mixed
// in here that could be separated:
//
// 1) The class can be used for variables that just need to be set (write-only). In this case, the
//    the Id and Table fields are not used, and AddToStateTable() would not be called. It would
//    only be necessary to call AddWriteCommandToInterface(). This functionality could be moved
//    to a base class.
//
// 2) The reading of data from the state table does not need to use the Data field.
//
// Also, it would be better to have a return value for a null table pointer. In general, we
// should rethink whether the underlying command methods (such as Get, GetLatest, Set) should
// return void.

template <class _elementType>
class mtsStateData {
protected:
    typedef mtsStateData<_elementType> ThisType;
    typedef mtsStateTable::Accessor<_elementType> AccessorType;
    typedef _elementType value_type;
    mtsStateDataId Id;
    mtsStateTable * Table;
    const AccessorType * Accessor;

public:  // PKAZ (was protected, but needed access for daVinci example)
    void Get(const mtsStateIndex & when, value_type & data) const {
        //if (!Table) return mtsExecutionResult::DEV_NOT_OK;
        //return Table->ReadFromReader(Id, when, data)?mtsExecutionResult::DEV_OK:mtsExecutionResult::DEV_NOT_OK;
        //if (Table) Table->ReadFromReader(Id, when, data);
        if (Accessor) Accessor->Get(when, data);
    }

    void GetLatest(value_type & obj) const {
        //if (!Table) return mtsExecutionResult::DEV_NOT_OK;
        //return Get(Table->GetIndexReader(), obj);
        //if (Table) Get(Table->GetIndexReader(), obj);
        if (Accessor) Accessor->GetLatest(obj);
    }

    // Get a vector of data, starting and ending at the specified time indices (inclusive).
    // For now, set the start index based on the vector size. In the future, we
    // should define a new parameter type that consists of a pair of mtsStateIndex.
    void GetHistory(const mtsStateIndex & end, mtsVector<value_type> & data) const {
        if (data.size() > 0) {
            //mtsStateIndex start = end;
            //start -= (data.size()-1);
            //if (Table) Table->ReadVectorFromReader(Id, start, end, data);
            if (Accessor) Accessor->GetHistory(end, data);
        }
    }

    void Set(const value_type & obj) {
        Data = obj;
    }

public:
    value_type Data;

    mtsStateData() : Id(-1), Table(0), Accessor(0) {}
    ~mtsStateData() {}

    /*! Conversion assignment.  This allows to assign from an object
      of the actual type without explicitly referencing the public
      member "Data". */
    inline ThisType & operator=(value_type data) {
        Data = data;
        return *this;
    }

    /*! Cast operator.  This allows to assign to an object of the
      actual type without explicitly referencing the public data
      member "Data". */
    inline operator value_type & (void) {
        return Data;
    }

    /*! Adds a new element (array) for this state data to the state
      table.  This is normally called from the constructor. The
      dataName is optional. */
    void AddToStateTable(mtsStateTable & table, const std::string & dataName = "") {
        Table = &table;
        Id = Table->NewElement(dataName, &Data);
        Accessor = dynamic_cast<const AccessorType *>(table.GetAccessorByName(dataName));
        if (!Accessor) {
            CMN_LOG_INIT_ERROR << "mtsStateData: could not get data accessor for " << dataName << std::endl;
        }
    }

    /*! Adds command objects to the specified device interface. Note
      that there are two command objects: a 'read' command to get the
      latest value, and a 'qualified read' command to get the value at
      the specified time.  This is normally called from the
      constructor, and the commandName would typically be GetXXX,
      where XXX is the name of the state data. */
    void AddReadCommandToInterface(mtsInterfaceProvided * interfaceProvided,
                                   const std::string & commandName) {
        interfaceProvided->AddCommandRead(&ThisType::GetLatest, this, commandName, this->Data);
        interfaceProvided->AddCommandQualifiedRead(&ThisType::Get, this, commandName, mtsStateIndex(), this->Data);
#ifdef CISST_GETVECTOR
        // PK: fix the following
        interfaceProvided->AddCommandQualifiedRead(&ThisType::GetHistory, this, commandName+"History", mtsStateIndex(), mtsVector<value_type>());
#endif
    }

    /*! Add write command to the specified task interface. Note that
      this must be a task so that the write is thread-safe. */
    void AddWriteCommandToInterface(mtsInterfaceProvided * interfaceProvided,
                                    const std::string & commandName) {
        interfaceProvided->AddCommandWrite(&ThisType::Set, this, commandName);
    }

};


// overload mtsObjectName to provide the class name
template <class _elementType>
std::string mtsObjectName(const mtsStateData<_elementType> * CMN_UNUSED(object))
{
    return "mtsStateData";
}


#endif // _mtsStateData_h
