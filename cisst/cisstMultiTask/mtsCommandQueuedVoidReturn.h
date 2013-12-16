/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Anton Deguet
  Created on: 2010-09-16

  (C) Copyright 2010-2013 Johns Hopkins University (JHU), All Rights Reserved.

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


#ifndef _mtsCommandQueuedVoidReturn_h
#define _mtsCommandQueuedVoidReturn_h

#include <cisstMultiTask/mtsCommandVoidReturn.h>

class mtsCommandWriteBase;

/*!
  \ingroup cisstMultiTask

 */

/*! VoidReturn queued command using templated _returnType parameter */
class mtsCommandQueuedVoidReturn: public mtsCommandVoidReturn
{
public:
    typedef mtsCommandVoidReturn BaseType;

    /*! This type. */
    typedef mtsCommandQueuedVoidReturn ThisType;

protected:
    /*! Mailbox used to queue the commands */
    mtsMailBox * MailBox;

private:
    /*! Private copy constructor to prevent copies */
    mtsCommandQueuedVoidReturn(const ThisType & CMN_UNUSED(other));

public:

    mtsCommandQueuedVoidReturn(mtsCallableVoidReturnBase * callable, const std::string & name,
                               const mtsGenericObject * resultPrototype,
                               mtsMailBox * mailBox);

    // ReturnsQueue destructor should get called
    virtual ~mtsCommandQueuedVoidReturn();

    mtsCommandQueuedVoidReturn * Clone(mtsMailBox * mailBox) const;

    mtsExecutionResult Execute(mtsGenericObject & result);

    mtsGenericObject * GetResultPointer(void);

    std::string GetMailBoxName(void) const;

    void EnableFinishedEvent(mtsCommandWriteBase *cmd);
    bool GenerateFinishedEvent(const mtsGenericObject &arg) const;

    void ToStream(std::ostream & outputStream) const;

protected:
    /*! Pointer on caller provided placeholder for result */
    mtsGenericObject * ResultPointer;

    /*! Event generator to indicate when execution is finished */
    mtsCommandWriteBase * FinishedEvent;

};


#endif // _mtsCommandQueuedVoidReturn_h

