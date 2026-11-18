/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

#include <pybind11/pybind11.h>
#include "cmnThrowPythonTest.h"

namespace py = pybind11;

PYBIND11_MODULE(cisstCommonPythonTestPython, m)
{
    py::class_<cmnThrowPythonTest>(m, "cmnThrowPythonTest")
        .def(py::init<>())
        .def("ThisWillThrowRuntimeError", &cmnThrowPythonTest::ThisWillThrowRuntimeError)
        .def("ThisWillThrowLogicError", &cmnThrowPythonTest::ThisWillThrowLogicError)
        .def("ThisWillThrowWithoutSpecification", &cmnThrowPythonTest::ThisWillThrowWithoutSpecification);
}
