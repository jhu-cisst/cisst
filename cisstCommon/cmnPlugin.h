/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Peter Kazanzides
  Created on: 2007-01-16

  (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Macro for generating plugin wrappers (factory methods).

  This macro should be used if the base class is NOT derived from
  \a cmnGenericObject.  In this case, the macro creates the factory methods
  and associated data to ensure safe creation of instances of the derived
  class. If this macro is used, the shared library should be loaded using
  the \a osaDynamicLoaderAndFactory class.  Note that this factory verifies
  the base class type and its version (to ensure that the shared library was
  compiled with the same version of the base class as the executable).

  If the base class is (or is derived from) \a cmnGenericObject, then this
  macro should not be used.  Instead, it is better to use \a osaDynamicLoader
  to load the shared library and to use the class services (\a cmnClassRegister)
  to create instances of the derived class.

  \sa osaDynamicLoader, osaDynamicLoaderAndFactory
*/
#pragma once

#ifndef _cmnPlugin_h
#define _cmnPlugin_h

#include <typeinfo>
#include <cisstCommon/cmnExport.h>

/*! This macro creates the functions and data necessary to allow the derived class
    to be dynamically loaded (as a "plugin") into a running application, using
    the \a osaDynamicLoaderAndFactory class. The only requirement on the base class
    is that it define a public \a VERSION enum that specifies its "interface version."
    \param base Base class that defines the interface (usually abstract)
    \param derived Derived class implemented in the plugin module
 */

#define CMN_PLUGIN_WRAPPERS(base, derived) \
   extern "C" CISST_EXPORT base* derived##Create() \
   { return new derived; } \
   extern "C" CISST_EXPORT void derived##Destroy(base* obj) \
   { delete obj; obj = 0; } \
   extern "C" CISST_EXPORT const std::type_info &derived##BaseType = typeid(base); \
   extern "C" CISST_EXPORT const int derived##Version = derived::VERSION;


#endif // _cmnPlugin_h

