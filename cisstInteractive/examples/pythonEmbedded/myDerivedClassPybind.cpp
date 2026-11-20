/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>

#include "myDerivedClass.h"

namespace py = pybind11;

PYBIND11_MODULE(myDerivedClassPython, module)
{
    py::module_::import("cisstCommonPython");

    py::class_<myDerivedClass, cmnGenericObject>(module, "myDerivedClass")
        .def(py::init<>())
        .def("CommonMethod", &myDerivedClass::CommonMethod)
        .def("FixedSizeVector", [](myDerivedClass & self) -> Eigen::Vector3d & {
            return self.FixedSizeVector();
        }, py::return_value_policy::reference_internal)
        .def("DynamicVector", [](myDerivedClass & self) -> Eigen::VectorXd & {
            return self.DynamicVector();
        }, py::return_value_policy::reference_internal);
}