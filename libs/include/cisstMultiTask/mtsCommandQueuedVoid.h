/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2005-05-03

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
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


#ifndef _mtsCommandQueuedVoid_h
#define _mtsCommandQueuedVoid_h

#include <cisstMultiTask/mtsCommandQueuedVoidBase.h>


/*!
  \ingroup cisstMultiTask

 */


// Not really needed (same as mtsCommandQueuedVoidBase) -- kept for
// consistency with mtsCommandQueuedWriteBase and
// mtsCommandQueuedWrite.
class mtsCommandQueuedVoid: public mtsCommandQueuedVoidBase
{
    typedef mtsCommandQueuedVoidBase BaseType;
public:

    inline mtsCommandQueuedVoid(void):
        BaseType() 
    {}

    inline mtsCommandQueuedVoid(mtsMailBox * mailBox, mtsCommandVoidBase * actualCommand):
        BaseType(mailBox, actualCommand)
    {}
    
    virtual ~mtsCommandQueuedVoid() {}

    inline virtual mtsCommandQueuedVoidBase * Clone(mtsMailBox * mailBox) const {
        return new mtsCommandQueuedVoid(mailBox, this->ActualCommand);
    }

    virtual mtsCommandBase::ReturnType Execute(void) {
        if (MailBox->Write(this)) {
            return mtsCommandBase::DEV_OK;
        }
        CMN_LOG(5) << "Class mtsCommandQueuedVoid: Execute(): Mailbox full for " 
                   << this->Name << std::endl;
        return mtsCommandBase::MAILBOX_FULL;
    }

    // no allocation required since the command has no argument
    inline virtual void Allocate(unsigned int CMN_UNUSED(size)) {}

};

#endif // _mtsCommandQueuedVoid_h

