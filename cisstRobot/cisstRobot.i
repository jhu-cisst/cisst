/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:   2016-03-21

  (C) Copyright 2016 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


%module cisstRobotPython

%include "swigrun.i"
%include "std_vector.i"

%import "cisstConfig.h"

%import "cisstCommon/cisstCommon.i"
%import "cisstVector/cisstVector.i"

%init %{
    import_array() // numpy initialization
%}

%header %{
#include <cisstRobot/robPython.h>
%}

// Generate parameter documentation for IRE
%feature("autodoc", "1");

#define CISST_EXPORT
#define CISST_DEPRECATED

// try to determine the pointer type, robDH, robModifiedDH, robHayati
// then "type" cast to SWIG wrapped
%typemap(out) robKinematics * {
    if (result != NULL) {
        std::string className;
        robDH * dh = dynamic_cast<robDH *>(result);
        if (dh) {
            className = "robDH *";
        } else {
            robModifiedDH * mod_dh = dynamic_cast<robModifiedDH *>(result);
            if (mod_dh) {
                className = "robModifiedDH *";
            } else {
                robHayati * hayati = dynamic_cast<robHayati *>(result);
                if (hayati) {
                    className = "robHayati *";
                }
            }
        }

        if (className != "") {
            swig_type_info *typeInfo = 0;
            typeInfo = SWIG_TypeQuery(className.c_str());
            // if the type info exists, i.e. this class has been wrapped, convert pointer
            if (typeInfo)
                resultobj = SWIG_NewPointerObj((void*)(result), typeInfo, $owner | %newpointer_flags);
            else {  // failed
                char buffer[256];
                sprintf(buffer, "cisstRobotPython.i: sorry, can't create a python object of type %s.",
                        className.c_str());
                PyErr_SetString(PyExc_TypeError, buffer);
                SWIG_fail;
            }
        } else {
            char buffer[256];
            sprintf(buffer, "cisstRobotPython.i: sorry, can't create a python object of type %s (not a supported derived class).",
                    className.c_str());
            PyErr_SetString(PyExc_TypeError, buffer);
            SWIG_fail;
        }
    } else {
        // Return None if object not found
        Py_INCREF(Py_None);
        resultobj = Py_None;
    }
}

// Wrap manipulator class
%include "cisstRobot/robManipulator.h"

%include "cisstRobot/robLink.h"
namespace std {
    %template(robLinkVector) vector<robLink>;
}

%include "cisstRobot/robJoint.h"
%include "cisstRobot/robKinematics.h"
%include "cisstRobot/robDH.h"
%include "cisstRobot/robModifiedDH.h"
%include "cisstRobot/robHayati.h"
