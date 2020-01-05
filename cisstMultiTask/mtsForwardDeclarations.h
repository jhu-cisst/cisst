/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:	2007-10-07

  (C) Copyright 2007-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsForwardDeclarations_h
#define _mtsForwardDeclarations_h

/*!
  \file
  \brief Forward declarations and \#define for cisstMultiTask
*/

#include <cisstCommon/cmnClassServices.h>
#include <cisstMultiTask/mtsGenericObject.h>

// MJ: if this is an inappropriate place to define invalid connection id or
// #include is not proper, we could move this to somewhere else (e.g. mtsManagerGlobal.h)
#include <limits> // for invalid connection id

#include <cisstMultiTask/mtsConfig.h>

/*! Queueing policy for the interface */
typedef enum {MTS_COMPONENT_POLICY, MTS_COMMANDS_SHOULD_NOT_BE_QUEUED, MTS_COMMANDS_SHOULD_BE_QUEUED} mtsInterfaceQueueingPolicy;

/*! Queueing policy, i.e. what the user would like to do for
  individual commands added using AddCommandVoid or
  AddCommandWrite as well as event handlers */
typedef enum {MTS_INTERFACE_COMMAND_POLICY, MTS_COMMAND_QUEUED, MTS_COMMAND_NOT_QUEUED} mtsCommandQueueingPolicy;

/*! Queueing policy, i.e. what the user would like to do for
  individual event handlers added using AddEventHandlerVoid or
  AddEventHandlerWrite. */
typedef enum {MTS_INTERFACE_EVENT_POLICY, MTS_EVENT_QUEUED, MTS_EVENT_NOT_QUEUED} mtsEventQueueingPolicy;

/*! Type for optional functions and interfaces */
typedef enum {MTS_OPTIONAL, MTS_REQUIRED} mtsRequiredType;

/*! Type to define is a command is blocking or not */
typedef enum {MTS_BLOCKING, MTS_NOT_BLOCKING} mtsBlockingType;

// commands
class mtsCommandBase;

// void callables and commands
class mtsCallableVoidBase;
template <class _classType> class mtsCallableVoidMethod;
class mtsCallableVoidFunction;
class mtsCommandVoid;
class mtsCommandQueuedVoid;
class mtsFunctionVoid;

// void return callables and commands
class mtsCallableVoidReturnBase;
template <class _classType, class _returnType> class mtsCallableVoidReturnMethod;
class mtsCommandVoidReturn;
template <class _Base> class mtsCommandQueuedVoidReturnBase;
typedef mtsCommandQueuedVoidReturnBase<mtsCommandVoidReturn> mtsCommandQueuedVoidReturn;
class mtsFunctionVoidReturn;

// read commands
class mtsCallableReadBase;
template <class _classType, class _argumentType> class mtsCallableReadMethod;
template <class _classType, class _argumentType> class mtsCallableReadReturnVoidMethod;
class mtsCommandRead;
class mtsFunctionRead;
typedef mtsCommandQueuedVoidReturnBase<mtsCommandRead> mtsCommandQueuedRead;

// write commands
class mtsCommandWriteBase;
template <class _classType, class _argumentType> class mtsCommandWrite;
class mtsFunctionWrite;

// write with returned value commands
class mtsCallableWriteReturnBase;
template <class _classType, class _argumentType, class _returnType> class mtsCallableWriteReturnMethod;
class mtsCommandWriteReturn;
template <class _Base> class mtsCommandQueuedWriteReturnBase;
typedef mtsCommandQueuedWriteReturnBase<mtsCommandWriteReturn> mtsCommandQueuedWriteReturn;
class mtsFunctionWriteReturn;

// qualified read commands
class mtsCallableQualifiedReadBase;
template <class _classType, class _argument1Type, class _argument2Type> class mtsCallableQualifiedReadMethod;
template <class _classType, class _argument1Type, class _argument2Type> class mtsCallableQualifiedReadReturnVoidMethod;
class mtsCommandQualifiedRead;
typedef mtsCommandQueuedWriteReturnBase<mtsCommandQualifiedRead> mtsCommandQueuedQualifiedRead;
class mtsFunctionQualifiedRead;

// event receivers
class mtsEventReceiverBase;
class mtsEventReceiverVoid;
class mtsEventReceiverWrite;
class mtsEventHandlerList;

// multicast void command
class mtsMulticastCommandVoid;

// multicast write commands
class mtsMulticastCommandWriteBase;
template <class _argumentType> class mtsMulticastCommandWrite;

// status/warning/error messages
class mtsMessage;

// interfaces and tasks
class mtsInterfaceProvided;
class mtsInterfaceOutput;
class mtsInterfaceRequired;
class mtsInterfaceInput;

class mtsCommandState;
class mtsComponent;
typedef mtsComponent mtsDevice; // for backward compatibility
class mtsTask;
class mtsTaskContinuous;
class mtsTaskPeriodic;
class mtsTaskFromCallback;
class mtsTaskFromSignal;

// containers
class mtsMailBox;
class mtsStateTable;

// data collection class
class mtsCollectorBase;
class mtsCollectorState;

// classes for networked configurations
class mtsComponentProxy;
class mtsProxySerializer;
class mtsManagerProxyServer;
class mtsManagerProxyClient;

// managers
class mtsManagerLocal;
#define mtsComponentManager mtsManagerLocal
class mtsManagerLocalInterface;
class mtsManagerGlobal;
class mtsManagerGlobalInterface;
typedef unsigned int ConnectionIDType;
// MJ: Slice only supports int type (-2^31 to 2^31-1)
// See http://www.zeroc.com/doc/Ice-3.4.1/manual/Slice.5.8.html for details
const ConnectionIDType InvalidConnectionID = (ConnectionIDType) std::numeric_limits<int>::max();

// dynamic component composition
class mtsManagerComponentServices;
class mtsManagerComponentClient;
class mtsManagerComponentServer;

// global function to retrieve object name if available
inline std::string mtsObjectName(const void * CMN_UNUSED(object)) {
    return "UnnamedObject";
}

// overload for mtsGenericObject to give at least the class name
inline std::string mtsObjectName(const mtsGenericObject * object) {
    return object->Services()->GetName();
}

#endif  // _mtsForwardDeclarations_h
