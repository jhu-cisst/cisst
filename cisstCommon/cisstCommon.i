/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Anton Deguet
  Created on:   2004-10-06

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_pair.i"

%module cisstCommonPython

%include "std_streambuf.i"
%include "std_iostream.i"

// Force insertion of code related to Python iterators in cxx file
// Version 1.3.37 removed symbols starting with Py
#if (SWIG_VERSION > 0x010336)
  %fragment("SwigPyIterator_T");
#else
  %fragment("PySwigIterator_T");
  #define SwigPyIterator PySwigIterator
#endif

// We define __setitem__ and __getitem__
%ignore *::operator[];


%include "swigrun.i"

%header %{
#include <cisstCommon/cmnPython.h>
%}

// Generate parameter documentation for IRE
%feature("autodoc", "1");

%rename (__str__) *::HumanReadable;
%ignore *::ToStream;
%ignore *::ToStreamRaw;
%ignore *::FromStreamRaw;
%ignore *::Serialize;
%ignore *::DeSerialize;
%ignore *::SerializeRaw;
%ignore *::DeSerializeRaw;


%ignore operator<<;

// CISST_EXPORT is not needed for SWIG 
#define CISST_EXPORT

// To load some cisst settings, mostly for conditional compilation
%import "cisstCommon/cmnPortability.h"

%ignore CMN_LOG_DETAILS;

// Typemap used so that C++ pointers on cmnGenericObject base class
// can be "casted" to the actual derived class in Python.  This is
// useful for objects stored in cmnObjectRegister.  Without the
// typemap, the user ends-up with a base class object and no derived
// feature whatsoever.
%typemap(out) cmnGenericObject * {
    if (result != NULL) {

        const cmnClassServicesBase *curServices = result->Services();
        swig_type_info *typeInfo = 0;
        // Loop through class hierarchy until SWIG type info is found
        for (; curServices && !typeInfo; curServices = curServices->GetParentServices()) {
            // create a string with a trailing "*" to retrieve the SWIG pointer type info
            std::string className = curServices->GetName() + " *";
            typeInfo = SWIG_TypeQuery(className.c_str());
        }
        // if the type info exists, i.e. this class has been wrapped, convert pointer
        if (typeInfo)
            resultobj = SWIG_NewPointerObj((void*)(result), typeInfo, $owner | %newpointer_flags);
        else {  // failed
            char buffer[256];
            sprintf(buffer, "cisstCommonPython.i: sorry, can't create a python object of type %s.  Make sure the python module which defines this type has been imported",
                result->Services()->GetName().c_str());
            PyErr_SetString(PyExc_TypeError, buffer);
            SWIG_fail;
        }
    } else {
        // Return None if object not found
        Py_INCREF(Py_None);
        resultobj = Py_None;
    }
}

// Wrap the generic object class
%include "cisstCommon/cmnGenericObject.h"
%import "cisstCommon/cmnAccessorMacros.h"

// Wrap the class register and add required code to generate python iterators
%newobject cmnClassServicesBase::Create;
%ignore cmnClassServicesBase::SetLoD;
%include "cisstCommon/cmnClassServicesBase.h"

%newobject cmnClassRegister::Create;
%ignore cmnClassRegister::SetLoD;
%ignore cmnClassRegister::SetLoDForAllClasses;
%ignore cmnClassRegister::SetLoDForMatchingClasses;
%include "cisstCommon/cmnClassRegister.h"
%include "cisstCommon/cmnClassRegisterMacros.h"
%template() std::pair<std::string, cmnClassServicesBase*>;
%template(cmnClassServicesContainer) std::map<std::string, cmnClassServicesBase*>;
%apply std::map<std::string, cmnClassServicesBase*>::const_iterator {
    cmnClassRegister::const_iterator
};
%newobject cmnClassRegister::iterator(PyObject **PYTHON_SELF);
%extend cmnClassRegister {
    swig::SwigPyIterator* iterator(PyObject **PYTHON_SELF) {
        return swig::make_output_iterator(self->begin(), self->begin(), self->end(), *PYTHON_SELF);
    }
    %pythoncode %{
        def __iter__(self):
            return self.iterator()
    %}
}


// Wrap the object register and add required code to generate python iterators
%include "cisstCommon/cmnObjectRegister.h"
%template() std::pair<std::string, cmnGenericObject*>;
%template(cmnGenericObjectContainer) std::map<std::string, cmnGenericObject*>;
%apply std::map<std::string, cmnGenericObject*>::const_iterator {
    cmnObjectRegister::const_iterator
};
%newobject cmnObjectRegister::iterator(PyObject **PYTHON_SELF);
%extend cmnObjectRegister {
    swig::SwigPyIterator* iterator(PyObject **PYTHON_SELF) {
        return swig::make_output_iterator(self->begin(), self->begin(), self->end(), *PYTHON_SELF);
    }
    %pythoncode %{
        def __iter__(self):
            return self.iterator()
    %}
}

// Wrap some basic types
%include "cisstCommon/cmnGenericObjectProxy.h"
%define CMN_GENERIC_OBJECT_PROXY_INSTANTIATE(name, elementType)
    // ignore the operator &
    %ignore cmnGenericObjectProxy<elementType>::operator value_type&;
    // Instantiate the template
    %template(name) cmnGenericObjectProxy<elementType>;
    // Type addition for dynamic type checking
    %{
        typedef cmnGenericObjectProxy<elementType> name;
    %}
    typedef cmnGenericObjectProxy<elementType> name;
    %types(name *);
%enddef

CMN_GENERIC_OBJECT_PROXY_INSTANTIATE(cmnDouble, double);
CMN_GENERIC_OBJECT_PROXY_INSTANTIATE(cmnInt, int);
CMN_GENERIC_OBJECT_PROXY_INSTANTIATE(cmnUInt, unsigned int);
CMN_GENERIC_OBJECT_PROXY_INSTANTIATE(cmnShort, short);
CMN_GENERIC_OBJECT_PROXY_INSTANTIATE(cmnUShort, unsigned short);
CMN_GENERIC_OBJECT_PROXY_INSTANTIATE(cmnLong, long);
CMN_GENERIC_OBJECT_PROXY_INSTANTIATE(cmnULong, unsigned long);
CMN_GENERIC_OBJECT_PROXY_INSTANTIATE(cmnBool, bool);


// Wrap stream code related to the logging system
%template() std::basic_streambuf<char,std::char_traits<char > >;
%include "cisstCommon/cmnLODMultiplexerStreambuf.h"
%template(cmnLODMultiplexerStreambufChar) cmnLODMultiplexerStreambuf<char>;
%include "cisstCommon/cmnCallbackStreambuf.h"
%template(cmnCallbackStreambufChar) cmnCallbackStreambuf<char>;
%include "cisstCommon/cmnLogLoD.h"

 // Ignore deprecated methods
%ignore cmnLogger::SetLoD;
%ignore cmnLogger::GetLoD;
%include "cisstCommon/cmnLogger.h"

// Wrap cmnPath
%include "cisstCommon/cmnPath.h"

// Wrap and instantiate useful type traits
%include "cisstCommon/cmnTypeTraits.h"
%template(cmnTypeTraitsDouble) cmnTypeTraits<double>;
%template(cmnTypeTraitsInt) cmnTypeTraits<int>;
