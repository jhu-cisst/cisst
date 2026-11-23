/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

#include <pybind11/pybind11.h>

#include "myReDerivedClass.h"

namespace py = pybind11;

PYBIND11_MODULE(myReDerivedClassPython, module)
{
    py::module_::import("myDerivedClassPython");

    py::class_<myReDerivedClass, myDerivedClass>(module, "myReDerivedClass")
        .def(py::init<>())
        .def("CommonMethod", &myReDerivedClass::CommonMethod)
        .def("DerivedMethod", &myReDerivedClass::DerivedMethod);
}