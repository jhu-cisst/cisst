/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Peter Kazanzides, Anton Deguet

  (C) Copyright 2005-2014 Johns Hopkins University (JHU), All Rights Reserved.

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


#ifndef _mtsCommandFilteredWrite_h
#define _mtsCommandFilteredWrite_h

#include <cisstMultiTask/mtsCommandWrite.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask
 */

class CISST_EXPORT mtsCommandFilteredWrite: public mtsCommandWriteBase
{
public:
    typedef mtsCommandWriteBase BaseType;
    typedef mtsCommandFilteredWrite ThisType;

protected:
    mtsCommandWriteBase * Command;
    mtsCommandQualifiedRead * Filter;
    mtsGenericObject * FilterOutput;

private:
    /*! Private copy constructor to prevent copies */
    inline mtsCommandFilteredWrite(const ThisType & CMN_UNUSED(other));

public:

    mtsCommandFilteredWrite(mtsCommandQualifiedRead * filter,
                            mtsCommandWriteBase * command);

    virtual ~mtsCommandFilteredWrite();

    /* documented in base class */
    mtsExecutionResult Execute(const mtsGenericObject & argument, mtsBlockingType blocking);

    inline mtsExecutionResult Execute(const mtsGenericObject & argument, mtsBlockingType blocking,
                                      mtsCommandWriteBase * CMN_UNUSED(finishedEventHandler)) {
        return Execute(argument, blocking);
    }

    /* documented in base class */
    const mtsGenericObject * GetArgumentPrototype(void) const;

    /* documented in base class */
    void ToStream(std::ostream & outputStream) const;
};


#endif // _mtsCommandFilteredWrite_h
