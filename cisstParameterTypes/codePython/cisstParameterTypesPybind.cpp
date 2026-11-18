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
#include <pybind11/stl.h>
#include <pybind11/eigen.h>

#include <cisstMultiTask/mtsGenericObjectProxy.h>
#include <cisstParameterTypes/prmMotionBase.h>
#include <cisstParameterTypes/prmRobotState.h>
#include <cisstParameterTypes/prmKeyValue.h>

namespace py = pybind11;

void cdgBind_cisstParameterTypes_prmActuatorJointCoupling(py::module & m);
void cdgBind_cisstParameterTypes_prmPositionCartesianGet(py::module & m);
void cdgBind_cisstParameterTypes_prmPositionCartesianArrayGet(py::module & m);
void cdgBind_cisstParameterTypes_prmPositionCartesianSet(py::module & m);
void cdgBind_cisstParameterTypes_prmVelocityCartesianGet(py::module & m);
void cdgBind_cisstParameterTypes_prmPositionJointGet(py::module & m);
void cdgBind_cisstParameterTypes_prmPositionJointSet(py::module & m);
void cdgBind_cisstParameterTypes_prmForceTorqueJointSet(py::module & m);
void cdgBind_cisstParameterTypes_prmCartesianImpedance(py::module & m);
void cdgBind_cisstParameterTypes_prmIMUSensors(py::module & m);
void cdgBind_cisstParameterTypes_prmStateJoint(py::module & m);
void cdgBind_cisstParameterTypes_prmStateCartesian(py::module & m);
void cdgBind_cisstParameterTypes_prmBaseFrame(py::module & m);
void cdgBind_cisstParameterTypes_prmServoJoint(py::module & m);
void cdgBind_cisstParameterTypes_prmServoCartesian(py::module & m);
void cdgBind_cisstParameterTypes_prmConfigurationJoint(py::module & m);
void cdgBind_cisstParameterTypes_prmInputData(py::module & m);
void cdgBind_cisstParameterTypes_prmKeyValue(py::module & m);
void cdgBind_cisstParameterTypes_prmOperatingState(py::module & m);
void cdgBind_cisstParameterTypes_prmImageFrame(py::module & m);
void cdgBind_cisstParameterTypes_prmCameraInfo(py::module & m);
void cdgBind_cisstParameterTypes_prmDepthMap(py::module & m);
void cdgBind_cisstParameterTypes_prmForwardKinematicsRequest(py::module & m);
void cdgBind_cisstParameterTypes_prmForwardKinematicsResponse(py::module & m);
void cdgBind_cisstParameterTypes_prmInverseKinematicsRequest(py::module & m);
void cdgBind_cisstParameterTypes_prmInverseKinematicsResponse(py::module & m);
void cdgBind_cisstParameterTypes_prmSimulationType(py::module & m);

PYBIND11_MODULE(cisstParameterTypesPython, m)
{
    m.doc() = "pybind11 bindings for cisstParameterTypes";

    py::module_::import("cisstCommonPython");
    py::module_::import("cisstMultiTaskPython");

    py::class_<prmMotionBase, mtsGenericObject>(m, "prmMotionBase");

    py::class_<prmRobotState, mtsGenericObject>(m, "prmRobotState")
        .def(py::init<>());

    cdgBind_cisstParameterTypes_prmActuatorJointCoupling(m);
    cdgBind_cisstParameterTypes_prmPositionCartesianGet(m);
    cdgBind_cisstParameterTypes_prmPositionCartesianArrayGet(m);
    cdgBind_cisstParameterTypes_prmPositionCartesianSet(m);
    cdgBind_cisstParameterTypes_prmVelocityCartesianGet(m);
    cdgBind_cisstParameterTypes_prmPositionJointGet(m);
    cdgBind_cisstParameterTypes_prmPositionJointSet(m);
    cdgBind_cisstParameterTypes_prmForceTorqueJointSet(m);
    cdgBind_cisstParameterTypes_prmCartesianImpedance(m);
    cdgBind_cisstParameterTypes_prmIMUSensors(m);
    cdgBind_cisstParameterTypes_prmStateJoint(m);
    cdgBind_cisstParameterTypes_prmStateCartesian(m);
    cdgBind_cisstParameterTypes_prmBaseFrame(m);
    cdgBind_cisstParameterTypes_prmServoJoint(m);
    cdgBind_cisstParameterTypes_prmServoCartesian(m);
    cdgBind_cisstParameterTypes_prmConfigurationJoint(m);
    cdgBind_cisstParameterTypes_prmInputData(m);
    cdgBind_cisstParameterTypes_prmKeyValue(m);
    cdgBind_cisstParameterTypes_prmOperatingState(m);
    cdgBind_cisstParameterTypes_prmImageFrame(m);
    cdgBind_cisstParameterTypes_prmCameraInfo(m);
    cdgBind_cisstParameterTypes_prmDepthMap(m);
    cdgBind_cisstParameterTypes_prmForwardKinematicsRequest(m);
    cdgBind_cisstParameterTypes_prmForwardKinematicsResponse(m);
    cdgBind_cisstParameterTypes_prmInverseKinematicsRequest(m);
    cdgBind_cisstParameterTypes_prmInverseKinematicsResponse(m);
    cdgBind_cisstParameterTypes_prmSimulationType(m);

    using KeyValueProxy = mtsGenericObjectProxy<prmKeyValue>;
    py::class_<KeyValueProxy, mtsGenericObject>(m, "prmKeyValueProxy")
        .def(py::init<>())
        .def(py::init<const prmKeyValue &>())
        .def_readwrite("Data", &KeyValueProxy::Data)
        .def("__copy__", [](const KeyValueProxy & self) {
            return KeyValueProxy(self);
        });
}
