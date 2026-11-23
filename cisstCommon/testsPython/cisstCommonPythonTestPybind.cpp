/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

#include <pybind11/pybind11.h>
#include "../codePython/cmnEigenGeometryCaster.hpp" // IWYU pragma: keep
#include "cmnThrowPythonTest.h"

namespace py = pybind11;

PYBIND11_MODULE(cisstCommonPythonTestPython, m)
{
    m.def("round_trip_isometry3d",
        [](const Eigen::Isometry3d& transform) { return transform; },
        "Convert a 4x4 array-like value to and from Eigen::Isometry3d.");
        m.def("round_trip_isometry3f",
            [](const Eigen::Isometry3f& transform) { return transform; },
            "Convert a 4x4 array-like value to and from Eigen::Isometry3f.");
    m.def("round_trip_isometry3d_noconvert",
        [](const Eigen::Isometry3d& transform) { return transform; },
        py::arg("transform").noconvert(),
        "Round-trip an exact-dtype NumPy array without implicit conversion.");
    m.def("round_trip_affine3f",
        [](const Eigen::Affine3f& transform) { return transform; },
        "Convert a 4x4 array-like value to and from Eigen::Affine3f.");
        m.def("round_trip_affine3d",
            [](const Eigen::Affine3d& transform) { return transform; },
            "Convert a 4x4 array-like value to and from Eigen::Affine3d.");
    m.def("round_trip_projective2d",
        [](const Eigen::Projective2d& transform) { return transform; },
        "Convert a 3x3 array-like value to and from Eigen::Projective2d.");
        m.def("round_trip_projective2f",
            [](const Eigen::Projective2f& transform) { return transform; },
            "Convert a 3x3 array-like value to and from Eigen::Projective2f.");

    py::class_<cmnThrowPythonTest>(m, "cmnThrowPythonTest")
        .def(py::init<>())
        .def("ThisWillThrowRuntimeError", &cmnThrowPythonTest::ThisWillThrowRuntimeError)
        .def("ThisWillThrowLogicError", &cmnThrowPythonTest::ThisWillThrowLogicError)
        .def("ThisWillThrowWithoutSpecification", &cmnThrowPythonTest::ThisWillThrowWithoutSpecification);
}
