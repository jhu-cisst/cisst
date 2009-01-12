/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCommandQueuedVoidBase.h,v 1.5 2008/09/05 04:31:10 anton Exp $

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2005-05-02

  (C) Copyright 2005-2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines base class for a queued void command.
*/

#ifndef _mtsCommandQueuedVoidBase_h
#define _mtsCommandQueuedVoidBase_h

#include <cisstMultiTask/mtsCommandVoidBase.h>
#include <cisstMultiTask/mtsMailBox.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsCommandQueuedVoidBase: public mtsCommandVoidBase
{
 protected:
    typedef mtsCommandVoidBase BaseType;
    mtsMailBox * MailBox;
    mtsCommandVoidBase * ActualCommand;

 public:
    inline mtsCommandQueuedVoidBase(void):
        BaseType("??"),
        MailBox(0),
        ActualCommand(0)
    {}

    
    inline mtsCommandQueuedVoidBase(mtsMailBox * mailBox, mtsCommandVoidBase * actualCommand):
        BaseType(actualCommand->Name),
        MailBox(mailBox),
        ActualCommand(actualCommand)
    {}


    inline virtual ~mtsCommandQueuedVoidBase() {}

    
    virtual mtsCommandQueuedVoidBase * Clone(mtsMailBox * mailBox) const = 0;

    
    inline virtual void Allocate(unsigned int size) = 0;


    virtual void ToStream(std::ostream & out) const;

    
    virtual mtsCommandBase::ReturnType Execute(void) = 0;


    inline virtual mtsCommandVoidBase * GetActualCommand(void) {
        return this->ActualCommand;
    }


    inline virtual const std::string & GetMailBoxName(void) const {
        return this->MailBox->GetName();
    }
};


#endif // _mtsCommandQueuedVoidBase_h
