/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Anton Deguet
  Created on: 2010-09-16

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

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


#ifndef _mtsCommandQueuedWriteReturn_h
#define _mtsCommandQueuedWriteReturn_h

#include <cisstMultiTask/mtsCommandWriteReturn.h>


/*!
  \ingroup cisstMultiTask

 */

/*! WriteReturn queued command using templated _returnType parameter */
class mtsCommandQueuedWriteReturn: public mtsCommandWriteReturn
{
public:
    typedef mtsCommandWriteReturn BaseType;

    /*! This type. */
    typedef mtsCommandQueuedWriteReturn ThisType;

protected:
    /*! Mailbox used to queue the commands */
    mtsMailBox * MailBox;

private:
    /*! Private copy constructor to prevent copies */
    mtsCommandQueuedWriteReturn(const ThisType & CMN_UNUSED(other));

public:

    mtsCommandQueuedWriteReturn(mtsCallableWriteReturnBase * callable, const std::string & name,
                                const mtsGenericObject * argumentPrototype,
                                const mtsGenericObject * resultPrototype,
                                mtsMailBox * mailBox);

    // ReturnsQueue destructor should get called
    virtual ~mtsCommandQueuedWriteReturn();

    mtsCommandQueuedWriteReturn * Clone(mtsMailBox * mailBox) const;

    mtsExecutionResult Execute(const mtsGenericObject & argument,
                               mtsGenericObject & result);

    const mtsGenericObject * GetArgumentPointer(void);

    mtsGenericObject * GetResultPointer(void);

    std::string GetMailBoxName(void) const;

    void ToStream(std::ostream & outputStream) const;

protected:
    /*! Pointer on caller provided placeholder for result */
    const mtsGenericObject * ArgumentPointer;
    mtsGenericObject * ResultPointer;

};


#endif // _mtsCommandQueuedWriteReturn_h

