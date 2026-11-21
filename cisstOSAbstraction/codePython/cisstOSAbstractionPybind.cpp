/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2026-11-17

  (C) Copyright 2008-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <pybind11/pybind11.h>
#include <pybind11/stl/filesystem.h>

#include <cisstOSAbstraction/osaSerialPort.h>
#include <cisstOSAbstraction/osaDynamicLoader.h>
#include <cisstOSAbstraction/osaSleep.h>

namespace py = pybind11;

PYBIND11_MODULE(cisstOSAbstractionPython, m)
{
    m.doc() = "pybind11 bindings for cisstOSAbstraction";

    py::module_::import("cisstCommonPython");

    py::class_<osaSerialPort, cmnGenericObject>(m, "osaSerialPort")
        .def(py::init<>())
        .def("SetPortName", &osaSerialPort::SetPortName)
        .def("GetPortName", &osaSerialPort::GetPortName)
        .def("Open", &osaSerialPort::Open, py::arg("blocking") = false)
        .def("Close", &osaSerialPort::Close)
        .def("IsOpened", &osaSerialPort::IsOpened);

    py::class_<osaDynamicLoader>(m, "osaDynamicLoader")
        .def(py::init<>())
        .def("Load", &osaDynamicLoader::Load, py::arg("file"))
        .def("UnLoad", &osaDynamicLoader::UnLoad);

    m.def("osaSleep", static_cast<void (*)(double)>(&osaSleep), py::arg("timeInSeconds"));
}
