/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Anton Deguet
  Created on:	2007-10-07

  (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

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
//include <cisstMultiTask/mtsGenericObjectProxy.h>

#include <cisstMultiTask/mtsConfig.h>

/*! Queuing policy for the interface */
typedef enum {MTS_COMPONENT_POLICY, MTS_COMMANDS_SHOULD_NOT_BE_QUEUED, MTS_COMMANDS_SHOULD_BE_QUEUED} mtsInterfaceQueuingPolicy;

/*! Queuing policy, i.e. what the user would like to do for
  individual commands added using AddCommandVoid or
  AddCommandWrite as well as event handlers */
typedef enum {MTS_INTERFACE_COMMAND_POLICY, MTS_COMMAND_QUEUED, MTS_COMMAND_NOT_QUEUED} mtsCommandQueuingPolicy;

/*! Queuing policy, i.e. what the user would like to do for
  individual event handlers added using AddEventHandlerVoid or
  AddEventHandlerWrite. */
typedef enum {MTS_INTERFACE_EVENT_POLICY, MTS_EVENT_QUEUED, MTS_EVENT_NOT_QUEUED} mtsEventQueuingPolicy;

/*! Type for optional functions and interfaces */
typedef enum {MTS_OPTIONAL, MTS_REQUIRED} mtsRequiredType;

// commands
class mtsCommandBase;

// void commands
class mtsCommandVoidBase;
template <class _interfaceType> class mtsCommandVoid;
class mtsFunctionVoid;

// one argument commands
template <class _argumentBaseType> class mtsCommandReadOrWriteBase;
typedef mtsCommandReadOrWriteBase<mtsGenericObject> mtsCommandReadBase;
typedef mtsCommandReadOrWriteBase<const mtsGenericObject> mtsCommandWriteBase;

class mtsFunctionRead;
class mtsFunctionWrite;

// two arguments commands
template <class _argument2BaseType> class mtsCommandQualifiedReadOrWriteBase;
typedef mtsCommandQualifiedReadOrWriteBase<mtsGenericObject> mtsCommandQualifiedReadBase;
typedef mtsCommandQualifiedReadOrWriteBase<const mtsGenericObject> mtsCommandQualifiedWriteBase;

class mtsFunctionQualifiedRead;

// multicast commands
class mtsMulticastCommandWriteBase;
template <class _argumentType> class mtsMulticastCommandWrite;

// interfaces and tasks
class mtsInterfaceProvidedOrOutput;
class mtsInterfaceProvided;
class mtsInterfaceOutput;
class mtsInterfaceRequiredOrInput;
class mtsInterfaceRequired;
class mtsInterfaceInput;

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


// global function to retrieve object name if available
inline std::string mtsObjectName(const void * CMN_UNUSED(object)) {
    return "UnnamedObject";
}

// overload for mtsGenericObject to give at least the class name
inline std::string mtsObjectName(const mtsGenericObject * object) {
    return object->Services()->GetName();
}


// classes defined when ICE is used
#if CISST_MTS_HAS_ICE
class mtsComponentProxy;
class mtsProxySerializer;
class mtsManagerProxyServer;
class mtsManagerProxyClient;
class mtsComponentInterfaceProxyServer;
class mtsComponentInterfaceProxyClient;
class mtsManagerProxyServer;
class mtsManagerProxyClient;
#endif // CISST_MTS_HAS_ICE


class mtsManagerLocal;
class mtsManagerLocalInterface;
class mtsManagerGlobal;
class mtsManagerGlobalInterface;

#endif  // _mtsForwardDeclarations_h

