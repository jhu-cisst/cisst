/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2026-11-17

  (C) Copyright 2016-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <cisstRobot/robLinkDynamics.h>
#include <cisstRobot/robLinkKinematics.h>
#include <cisstRobot/robModifiedDH.h>

namespace py = pybind11;

PYBIND11_MODULE(cisstRobotPython, m)
{
    m.doc() = "pybind11 bindings for cisstRobot";

    py::class_<robLinkDynamics>(m, "robLinkDynamics")
        .def(py::init<double, const Eigen::Vector3d &, const Eigen::Matrix3d &>(),
             py::arg("mass"), py::arg("center_of_mass"), py::arg("moment_of_inertia"))
        .def("Mass", &robLinkDynamics::Mass)
        .def("CenterOfMass", &robLinkDynamics::CenterOfMass)
        .def("MomentOfInertiaAtCOM", &robLinkDynamics::MomentOfInertiaAtCOM);

    py::class_<robLinkKinematics>(m, "robLinkKinematics")
        .def("JointAxis", &robLinkKinematics::JointAxis)
        .def("JointOrigin", &robLinkKinematics::JointOrigin)
        .def("LinkFrame", [](const robLinkKinematics & self) {
            return self.LinkFrame().matrix();
        })
        .def("JointType", &robLinkKinematics::JointType)
        .def("isActive", &robLinkKinematics::isActive)
        .def("JointOffset", &robLinkKinematics::JointOffset);

    py::class_<robModifiedDH, robLinkKinematics>(m, "robModifiedDH")
        .def(py::init<double, double, double, double, cmnJointType, double, bool>(),
             py::arg("alpha"), py::arg("a"), py::arg("theta"), py::arg("d"),
             py::arg("joint_type"), py::arg("joint_offset"), py::arg("active") = true);
}
