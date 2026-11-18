/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2026-11-17

  (C) Copyright 2010-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <pybind11/pybind11.h>
#include "mtsPeriodicTaskTest.h"

namespace py = pybind11;

PYBIND11_MODULE(cisstMultiTaskPythonTestPython, m)
{
    m.doc() = "pybind11 bindings for cisstMultiTaskPythonTest";

    py::module_::import("cisstCommonPython");
    py::module_::import("cisstMultiTaskPython");

    py::class_<mtsPeriodicTaskTest, mtsTaskPeriodic>(m, "mtsPeriodicTaskTest", py::dynamic_attr())
        .def(py::init<double>());
}
