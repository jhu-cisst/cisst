/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCommandQueuedWriteProxy.h 75 2009-02-24 16:47:20Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2009-04-29

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines base class for a queued write command.
*/

#ifndef _mtsCommandQueuedWriteProxy_h
#define _mtsCommandQueuedWriteProxy_h

#include <cisstMultiTask/mtsCommandQueuedWriteBase.h>

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsCommandQueuedWriteProxy: public mtsCommandQueuedWriteBase {
public:
    typedef mtsCommandQueuedWriteBase BaseType;
    typedef const mtsGenericObject * ArgumentPointerType;

protected:
    mtsQueue<ArgumentPointerType> ArgumentsQueue;

public:
    inline mtsCommandQueuedWriteProxy(void) : 
        BaseType(), 
        ArgumentsQueue(0, ArgumentPointerType())
    {}

    inline mtsCommandQueuedWriteProxy(mtsCommandWriteProxy * actualCommand)
        : BaseType(0, actualCommand),
          ArgumentsQueue(0, ArgumentPointerType())
        //
        // TODO:
        //      MailBox, Execute() method implementation!!!
        //
        //MailBox(mailBox),
        //ActualCommand(actualCommand)
    {}

    inline mtsCommandQueuedWriteProxy(mtsMailBox * mailBox, 
                                      mtsCommandWriteBase * actualCommand, 
                                      unsigned int size) :
        BaseType(mailBox, actualCommand),
        ArgumentsQueue(0, ArgumentPointerType())
    {
        ArgumentPointerType argumentPointPrototype = dynamic_cast<ArgumentPointerType>(this->GetArgumentPrototype());
        if (argumentPointPrototype) {
            ArgumentsQueue.SetSize(size, argumentPointPrototype);
        } else {
            //CMN_LOG(1) << "Class mtsCommandQueuedWrite: constructor: Can't find argument prototype from actual command."
            //           << std::endl;
        }
    }

    inline virtual ~mtsCommandQueuedWriteProxy() {}

    //!!!!!!!!!! FIX THIS
    //mtsCommandQueuedWriteProxy * Clone(mtsMailBox* mailBox, unsigned int size) const    
    //{ 
    //    return new mtsCommandQueuedWriteProxy(mailBox, ActualCommand, size);
    //}

    const mtsGenericObject * GetArgumentPrototype(void) const
    { 
        return ActualCommand->GetArgumentPrototype();
    }
    
    // Allocate should be called when a task calls GetMethodXXX().
    inline void Allocate(unsigned int size)
    {
        if (ArgumentsQueue.GetSize() != size) {
            if (ArgumentsQueue.GetSize() > 0) {
                // Probably should never happen
                //CMN_LOG(3) << "Class mtsCommandQueuedWriteProxy: Allocate(): Changing ArgumentsQueue size from " << ArgumentsQueue.GetSize()
                //           << " to " << size << std::endl;
            }
            ArgumentPointerType argumentPointerPrototype = dynamic_cast<ArgumentPointerType>(this->GetArgumentPrototype());
            if (argumentPointerPrototype) {
                ArgumentsQueue.SetSize(size, argumentPointerPrototype);
            } else {
                //CMN_LOG(1) << "Class mtsCommandQueuedWriteProxy: constructor: Can't find argument prototype from actual command."
                //           << std::endl;
            }
        }
    }

    mtsCommandBase::ReturnType Execute(const mtsGenericObject & argument) {
        static int cnt = 0;
        std::cout << "mtsCommandQueuedWriteProxy called (" << ++cnt << "): " << Name << std::endl;
        return BaseType::DEV_OK;
    }
    
    const mtsGenericObject * ArgumentPeek(void) const
    {
        return *ArgumentsQueue.Peek();
    }

    mtsGenericObject * ArgumentGet(void)
    {
        // TODO: FIX THIS OR REMOVE THIS!
        //return ArgumentsQueue.Get();
        return reinterpret_cast<mtsGenericObject*>(0x1234);
    }
};

#endif // _mtsCommandQueuedWrite_h

