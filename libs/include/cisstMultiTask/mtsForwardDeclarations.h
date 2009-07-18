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
#include <cisstMultiTask/mtsGenericObjectProxy.h>


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

template <class _argumentType> class mtsFunctionReadOrWrite;

// two arguments commands
template <class _argument2BaseType> class mtsCommandQualifiedReadOrWriteBase;
typedef mtsCommandQualifiedReadOrWriteBase<mtsGenericObject> mtsCommandQualifiedReadBase;
typedef mtsCommandQualifiedReadOrWriteBase<const mtsGenericObject> mtsCommandQualifiedWriteBase;

template <class _argumentType> class mtsFunctionQualifiedReadOrWrite;
typedef mtsFunctionReadOrWrite<mtsGenericObject> mtsFunctionRead;
typedef mtsFunctionReadOrWrite<const mtsGenericObject> mtsFunctionWrite;

// multicast commands
class mtsMulticastCommandWriteBase;
template <class _argumentType> class mtsMulticastCommandWrite;

// interfaces and tasks
class mtsDeviceInterface;
typedef mtsDeviceInterface mtsProvidedInterface;
class mtsDevice;
class mtsTaskInterface;
class mtsRequiredInterface;
class mtsTask;
class mtsTaskPeriodic;
class mtsTaskManager;

class mtsMailBox;
class mtsStateTable;

// mts vectors
template <class _elementType> class mtsVector;
typedef mtsVector<double> mtsDoubleVec;


// global function to retrieve object name if available
inline std::string mtsObjectName(const void * CMN_UNUSED(object)) {
    return "UnnamedObject";
}

// overload for mtsGenericObject to give at least the class name
inline std::string mtsObjectName(const mtsGenericObject * object) {
    return object->Services()->GetName();
}


// flag for required interfaces lists of commands
const bool mtsRequired = true;
const bool mtsOptional = false;


#endif  // _mtsForwardDeclarations_h

