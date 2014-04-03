/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides
  Created on: 2010-09-24

  (C) Copyright 2010-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsEventReceiver_h
#define _mtsEventReceiver_h

/*!
  \file
  \brief Declaration of mtsEventReceiverBase, mtsEventReceiverVoid, and mtsEventReceiverWrite
 */


/*!
  \ingroup cisstMultiTask

  This class implements event receivers for void and write events. The idea is similar to the
  mtsFunction classes (e.g., mtsFunctionVoid, mtsFunctionRead) -- these event receivers should
  be added to a required interface and will be bound to the corresponding event generators
  in the connected provided interface. Then, it is possible for the component to specify an
  event handler at any time (see SetHandler method), or to wait for the event to occur (see
  Wait method).

  Previously, it was only possible to add event handlers to a required interface (see
  mtsInterfaceRequired::AddEventHandlerVoid, mtsInterfaceRequired::AddEventHandlerWrite).
  For backward compatibility, this design continues to support direct addition of an event
  handler to a required interface. Thus, the following three blocks of code can all be used
  to set an event handler:

  \code
  mtsInterfaceRequired required;

  // Method 1: Add event handler directly to required interface (no event receiver)
  required->AddEventHandlerVoid(&MyClass::Handler, this, "EventName");

  // Method 2: Add event handler via event receiver
  mtsEventReceiverVoid MyReceiver;
  required->AddEventReceiver(MyReceiver, "EventName");
  MyReceiver.SetHandler(&MyClass::Handler, this);

  // Method 3: Add event handler and event receiver to required interface
  //           (although this works, Method 2 is the preferred implementation)
  mtsEventReceiverVoid MyReceiver;
  required->AddEventReceiver(MyReceiver, "EventName");
  required->AddEventHandlerVoid(&MyClass::Handler, this, "EventName");
  \endcode

  One thing to note about an event receiver is that, by default, it is considered required
  (i.e., #MTS_REQUIRED) when added to a required interface, whereas event handlers are not.

  There are two main reasons for the introduction of event handlers:
  -# They allow the component to wait for an event.
  -# They allow event handlers to be added at any time. Previously, if an event handler was
     added to a required interface AFTER that interface was connected, it was ignored (i.e.,
     it was not added as an observer to the event generator in the provided interface).

  The implementation of the event receiver class requires an osaThreadSignal to block the
  component when the Wait method is called. This design uses the ThreadSignal member of
  the required interface mailbox, which is present for any component derived from mtsTask.
  If the required interface does not have a mailbox, the event receiver will create its own
  instance of osaThreadSignal. This is done to support use of event receivers by low-level
  components that do not have their own thread of execution (e.g., mtsComponent), as long
  as the Wait method is only called from a single thread. Note that the Wait method returns
  a bool; a false return indicates that the Wait failed for some reason (such as being called
  from more than one thread). An alternate implementation would be to introduce an osaThreadSignal
  member to mtsComponent and use that instead of the one in the required interface mailbox.
*/

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstOSAbstraction/osaForwardDeclarations.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>


// EventReceivers must be added before Bind (should add check for InterfaceProvidedOrOutput==0)
// EventHandlers can be added at any time.
// When Bind called,
//    if no EventReceiver, directly add EventHandler
//    if EventReceiver, set handler in it

class CISST_EXPORT mtsEventReceiverBase {
protected:
    std::string Name;
    mtsInterfaceRequired * Required;   // Pointer to the required interface
    osaThreadSignal * EventSignal;
    bool Waiting;
    bool OwnEventSignal;   // true if we created our own thread signal

    bool CheckRequired() const;
    bool WaitCommon();

public:
    mtsEventReceiverBase();
    virtual ~mtsEventReceiverBase();

    void SetName(const std::string &name) { Name = name; }
    virtual std::string GetName() const { return Name; }

    /*! Called from mtsInterfaceRequired::AddEventReceiver */
    virtual void SetRequired(const std::string &name, mtsInterfaceRequired *req);

    virtual void SetThreadSignal(osaThreadSignal *signal);

    /*! Wait for event to be issued.
        \returns true if successful, false if failed. */
    virtual bool Wait();

    /*! Wait for event to be issued, up to specified timeout.
        \returns true if successful, false if failed or timeout occurred. */
    virtual bool WaitWithTimeout(double timeoutInSec);

    virtual void Detach();

    /*! Human readable output to stream. */
    virtual void ToStream(std::ostream & outputStream) const = 0;
};


/*! Stream out operator. */
inline std::ostream & operator << (std::ostream & output,
                                   const mtsEventReceiverBase & receiver) {
    receiver.ToStream(output);
    return output;
}

class CISST_EXPORT mtsEventReceiverVoid : public mtsEventReceiverBase {
protected:
    mtsCommandVoid * Command;      // Command object for calling EventHandler method
    mtsCommandVoid * UserHandler;  // User supplied event handler

    // This one always gets added non-queued
    void EventHandler(void);

public:
    mtsEventReceiverVoid();
    ~mtsEventReceiverVoid();

    /*! Called from mtsInterfaceRequired::BindCommandsAndEvents */
    mtsCommandVoid * GetCommand(void);

    /*! Called from mtsInterfaceRequired::AddEventHandlerVoid */
    void SetHandlerCommand(mtsCommandVoid * commandHandler);

    // Same functionality as mtsInterfaceRequired::AddEventHandlerVoid.
    template <class __classType>
    inline mtsCommandVoid * SetHandler(void (__classType::*method)(void),
                                       __classType * classInstantiation,
                                       mtsEventQueueingPolicy queueingPolicy = MTS_INTERFACE_EVENT_POLICY) {
        return CheckRequired() ? (Required->AddEventHandlerVoid(method, classInstantiation, this->GetName(), queueingPolicy)) : 0;
    }

    inline mtsCommandVoid * SetHandler(void (*function)(void),
                                       mtsEventQueueingPolicy queueingPolicy = MTS_INTERFACE_EVENT_POLICY) {
        return CheckRequired() ? (Required->AddEventHandlerVoid(function, this->GetName(), queueingPolicy)) : 0;
    }

    bool RemoveHandler(void);

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;
};

class CISST_EXPORT mtsEventReceiverWrite : public mtsEventReceiverBase {
protected:
    mtsCommandWriteBase *Command;      // Command object for calling EventHandler method
    mtsCommandWriteBase *UserHandler;  // User supplied event handler
    mtsGenericObject *ArgPtr;

    // This one always gets added non-queued
    void EventHandler(const mtsGenericObject &arg);

public:
    mtsEventReceiverWrite();
    ~mtsEventReceiverWrite();

    /*! Called from mtsInterfaceRequired::BindCommandsAndEvents */
    mtsCommandWriteBase *GetCommand();

    /*! Called from mtsInterfaceRequired::AddEventHandlerWrite */
    void SetHandlerCommand(mtsCommandWriteBase * commandHandler);

    // Same functionality as mtsInterfaceRequired::AddEventHandlerWrite.
    template <class __classType, class __argumentType>
    inline mtsCommandWriteBase * SetHandler(void (__classType::*method)(const __argumentType &),
                                            __classType * classInstantiation,
                                            mtsEventQueueingPolicy queueingPolicy = MTS_INTERFACE_EVENT_POLICY) {
        return CheckRequired() ? (Required->AddEventHandlerWrite(method, classInstantiation, this->GetName(), queueingPolicy)) : 0;
    }

    // PK: Do we need the "generic" version (AddEventHandlerWriteGeneric)?


    // Note that we are using the Wait and WaitWithTimeout member functions from the base class.
    using mtsEventReceiverBase::Wait;
    using mtsEventReceiverBase::WaitWithTimeout;

    /*! Wait for event to be issued and return received argument.
        \returns true if successful, false if failed (including case where wait succeeded but return value obj
                 is invalid) */
    virtual bool Wait(mtsGenericObject &obj);
    virtual bool WaitWithTimeout(double timeoutInSec, mtsGenericObject &obj);

    //PK: might be nice to have this
    //const mtsGenericObject *GetArgumentPrototype() const;

    bool RemoveHandler(void);

    /*! Human readable output to stream. */
    void ToStream(std::ostream & outputStream) const;
};


#endif // _mtsEventReceiver_h
