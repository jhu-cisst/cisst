/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsForwardDeclarations.h,v 1.16 2008/12/01 05:14:42 pkaz Exp $

  Author(s):	Anton Deguet
  Created on:	2007-10-07

  (C) Copyright 2007-2008 Johns Hopkins University (JHU), All Rights
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

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServices.h>

// commands
class mtsCommandBase;

// void commands
class mtsCommandVoidBase;
template <class _interfaceType> class mtsCommandVoid;
class mtsFunctionVoid;

// one argument commands
template <class _argumentBaseType> class mtsCommandReadOrWriteBase;
typedef mtsCommandReadOrWriteBase<cmnGenericObject> mtsCommandReadBase;
typedef mtsCommandReadOrWriteBase<const cmnGenericObject> mtsCommandWriteBase;

template <class _argumentType> class mtsFunctionReadOrWrite;

// two arguments commands
template <class _argument2BaseType> class mtsCommandQualifiedReadOrWriteBase;
typedef mtsCommandQualifiedReadOrWriteBase<cmnGenericObject> mtsCommandQualifiedReadBase;
typedef mtsCommandQualifiedReadOrWriteBase<const cmnGenericObject> mtsCommandQualifiedWriteBase;

template <class _argumentType> class mtsFunctionQualifiedReadOrWrite;

// multicast commands
class mtsMulticastCommandWriteBase;
template <class _argumentType> class mtsMulticastCommandWrite;

// interfaces and tasks
class mtsDeviceInterface;
class mtsDevice;
class mtsTaskInterface;
class mtsRequiredInterface;
class mtsTask;
class mtsTaskPeriodic;
class mtsTaskManager;

class mtsMailBox;

// mts vectors
template <class _elementType> class mtsVector;
typedef mtsVector<double> mtsDoubleVec;


// global function to retrieve object name if available
inline std::string mtsObjectName(const void * CMN_UNUSED(object)) {
    return "UnnamedObject";
}

// overload for cmnGenericObject to give at least the class name
inline std::string mtsObjectName(const cmnGenericObject * object) {
    return object->Services()->GetName();
}



#endif  // _mtsForwardDeclarations_h

