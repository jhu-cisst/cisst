/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id:$

  Author(s):  Peter Kazanzides

  (C) Copyright 2005-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Define an internal command for cisstMultiTask
*/


#ifndef _mtsCommandFilteredQueuedWrite_h
#define _mtsCommandFilteredQueuedWrite_h

#include <cisstMultiTask/mtsCommandQueuedWrite.h>


/*!
  \ingroup cisstMultiTask

 */
template <class _argumentType, class _filteredType>
class mtsCommandFilteredQueuedWrite: public mtsCommandQueuedWrite<_filteredType>
{
public:
    typedef mtsCommandQueuedWrite<_filteredType> BaseType;
    typedef _argumentType ArgumentType;
    typedef _filteredType FilteredType;
    typedef typename mtsGenericTypes<FilteredType>::FinalType FilteredFinalType;

    /*! This type. */
    typedef mtsCommandFilteredQueuedWrite<ArgumentType, FilteredType> ThisType;

protected:
    mtsCommandQualifiedReadBase * ActualFilter;
    FilteredFinalType filterOutput;

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCommandFilteredQueuedWrite(const ThisType & CMN_UNUSED(other));

public:

    inline mtsCommandFilteredQueuedWrite(void):
        BaseType(), ActualFilter(0), filterOutput()
    {}


    inline mtsCommandFilteredQueuedWrite(mtsCommandQualifiedReadBase *actualFilter, mtsCommandWriteBase * actualCommand):
        BaseType(actualCommand), ActualFilter(actualFilter)
    {
        const FilteredFinalType *ptr = dynamic_cast<const FilteredFinalType*>(actualFilter->GetArgument2Prototype());
        if (ptr)
            filterOutput = *ptr;
        else
            CMN_LOG_INIT_ERROR << "mtsCommandFilteredQueuedWrite: arg2 is NULL" << std::endl;
    }


    inline mtsCommandFilteredQueuedWrite(mtsMailBox * mailBox, mtsCommandQualifiedReadBase * actualFilter, mtsCommandWriteBase * actualCommand, unsigned int size):
        BaseType(mailBox, actualCommand, size), ActualFilter(actualFilter)
    {
        const FilteredFinalType *ptr = dynamic_cast<const FilteredFinalType*>(actualFilter->GetArgument2Prototype());
        if (ptr)
            filterOutput = *ptr;
        else
            CMN_LOG_INIT_ERROR << "mtsCommandFilteredQueuedWrite: arg2 is NULL" << std::endl;
    }


    // ArgumentsQueue destructor should get called
    inline virtual ~mtsCommandFilteredQueuedWrite() {}


    inline virtual mtsCommandFilteredQueuedWrite * Clone(mtsMailBox * mailBox, unsigned int size) const {
        return new mtsCommandFilteredQueuedWrite(mailBox, this->ActualFilter, this->ActualCommand, size);
    }

    /*! Return a pointer on the argument prototype */
    inline virtual const mtsGenericObject * GetArgumentPrototype(void) const {
        return this->ActualFilter->GetArgument1Prototype();
    }

    virtual mtsCommandBase::ReturnType Execute(const mtsGenericObject & argument) {
        if (this->IsEnabled()) {
            // First, call the filter (qualified read)
            mtsCommandBase::ReturnType ret = ActualFilter->Execute(argument, filterOutput);
            if (ret != mtsCommandBase::DEV_OK) return ret;
            // Next, queue the write command
            return BaseType::Execute(filterOutput);
        }
        return mtsCommandBase::DISABLED;
    }

};

#endif // _mtsCommandFilteredQueuedWrite_h

