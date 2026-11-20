/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

#include <pybind11/pybind11.h>

#include "SineGenerator.h"

namespace py = pybind11;

PYBIND11_MODULE(SineGeneratorPython, module)
{
    py::module_::import("cisstCommonPython");

    py::class_<SineGenerator, cmnGenericObject>(module, "SineGenerator")
        .def(py::init<double, double>())
        .def("GetAmplitude", &SineGenerator::GetAmplitude)
        .def("SetAmplitude", &SineGenerator::SetAmplitude)
        .def("GetFrequency", &SineGenerator::GetFrequency)
        .def("SetFrequency", &SineGenerator::SetFrequency)
        .def("ComputeOutput", &SineGenerator::ComputeOutput)
        .def("GetOutput", &SineGenerator::GetOutput);
}