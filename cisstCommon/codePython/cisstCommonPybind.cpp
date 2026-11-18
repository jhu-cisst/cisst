/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2026-11-17

  (C) Copyright 2005-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sstream>

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassServicesBase.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnObjectRegister.h>
#include <cisstCommon/cmnGenericObjectProxy.h>
#include <cisstCommon/cmnLogLoD.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnPath.h>
#include <cisstCommon/cmnJointType.h>

namespace py = pybind11;

void cdgBind_cisstCommon_cmnJointType(py::module & m);

template <typename _elementType>
void bindGenericObjectProxy(py::module & m, const char * name)
{
    using ProxyType = cmnGenericObjectProxy<_elementType>;
    py::class_<ProxyType, cmnGenericObject>(m, name)
        .def(py::init<>())
        .def(py::init<const _elementType &>())
        .def_readwrite("Data", &ProxyType::Data);
}

PYBIND11_MODULE(cisstCommonPython, m)
{
    m.doc() = "pybind11 bindings for cisstCommon";

    py::class_<cmnGenericObject>(m, "cmnGenericObject")
        .def("Services", &cmnGenericObject::Services, py::return_value_policy::reference)
        .def("ToString", &cmnGenericObject::ToString)
        .def("__str__", &cmnGenericObject::ToString)
        .def("__repr__", &cmnGenericObject::ToString);

    py::class_<cmnClassServicesBase>(m, "cmnClassServicesBase")
        .def("GetName", &cmnClassServicesBase::GetName)
        .def("GetSize", &cmnClassServicesBase::GetSize)
        .def("__str__", &cmnClassServicesBase::GetName)
        .def("Create", py::overload_cast<>(&cmnClassServicesBase::Create, py::const_), py::return_value_policy::take_ownership)
        .def("Delete", &cmnClassServicesBase::Delete);

    py::class_<cmnClassRegister, std::unique_ptr<cmnClassRegister, py::nodelete>>(m, "cmnClassRegister")
        .def_static("Instance", &cmnClassRegister::Instance, py::return_value_policy::reference)
        .def_static("FindClassServices", py::overload_cast<const std::string &>(&cmnClassRegister::FindClassServices),
                    py::return_value_policy::reference)
        .def_static("Create", py::overload_cast<const std::string &>(&cmnClassRegister::Create),
                    py::return_value_policy::take_ownership)
        .def("__iter__", [](const cmnClassRegister & reg) {
            return py::make_iterator(reg.begin(), reg.end());
        }, py::keep_alive<0, 1>())
        .def("ToString", [](const cmnClassRegister & reg) {
            std::ostringstream oss;
            reg.ToStream(oss);
            return oss.str();
        })
        .def("__str__", [](const cmnClassRegister & reg) {
            std::ostringstream oss;
            reg.ToStream(oss);
            return oss.str();
        });

    py::class_<cmnObjectRegister, std::unique_ptr<cmnObjectRegister, py::nodelete>>(m, "cmnObjectRegister")
        .def_static("Instance", &cmnObjectRegister::Instance, py::return_value_policy::reference)
        .def_static("Register", &cmnObjectRegister::Register)
        .def_static("Remove", &cmnObjectRegister::Remove)
        .def_static("FindObject", &cmnObjectRegister::FindObject, py::return_value_policy::reference)
        .def("__iter__", [](const cmnObjectRegister & reg) {
            return py::make_iterator(reg.begin(), reg.end());
        }, py::keep_alive<0, 1>())
        .def("ToString", [](const cmnObjectRegister & reg) {
            std::ostringstream oss;
            reg.ToStream(oss);
            return oss.str();
        })
        .def("__str__", [](const cmnObjectRegister & reg) {
            std::ostringstream oss;
            reg.ToStream(oss);
            return oss.str();
        });

    bindGenericObjectProxy<double>(m, "cmnDouble");
    bindGenericObjectProxy<int>(m, "cmnInt");
    bindGenericObjectProxy<unsigned int>(m, "cmnUInt");
    bindGenericObjectProxy<short>(m, "cmnShort");
    bindGenericObjectProxy<unsigned short>(m, "cmnUShort");
    bindGenericObjectProxy<long>(m, "cmnLong");
    bindGenericObjectProxy<unsigned long>(m, "cmnULong");
    bindGenericObjectProxy<bool>(m, "cmnBool");

    m.attr("CMN_LOG_LEVEL_NONE") = CMN_LOG_LEVEL_NONE;
    m.attr("CMN_LOG_LEVEL_INIT_ERROR") = CMN_LOG_LEVEL_INIT_ERROR;
    m.attr("CMN_LOG_LEVEL_INIT_WARNING") = CMN_LOG_LEVEL_INIT_WARNING;
    m.attr("CMN_LOG_LEVEL_INIT_VERBOSE") = CMN_LOG_LEVEL_INIT_VERBOSE;
    m.attr("CMN_LOG_LEVEL_INIT_DEBUG") = CMN_LOG_LEVEL_INIT_DEBUG;
    m.attr("CMN_LOG_LEVEL_RUN_ERROR") = CMN_LOG_LEVEL_RUN_ERROR;
    m.attr("CMN_LOG_LEVEL_RUN_WARNING") = CMN_LOG_LEVEL_RUN_WARNING;
    m.attr("CMN_LOG_LEVEL_RUN_VERBOSE") = CMN_LOG_LEVEL_RUN_VERBOSE;
    m.attr("CMN_LOG_LEVEL_RUN_DEBUG") = CMN_LOG_LEVEL_RUN_DEBUG;

    py::class_<cmnLogger, std::unique_ptr<cmnLogger, py::nodelete>>(m, "cmnLogger")
        .def_static("SetMask", &cmnLogger::SetMask)
        .def_static("GetMask", &cmnLogger::GetMask)
        .def_static("SetMaskFunction", &cmnLogger::SetMaskFunction)
        .def_static("GetMaskFunction", &cmnLogger::GetMaskFunction)
        .def_static("SetMaskDefaultLog", &cmnLogger::SetMaskDefaultLog)
        .def_static("AddChannelToStdOut", &cmnLogger::AddChannelToStdOut,
                    py::arg("mask") = CMN_LOG_ALLOW_ALL)
        .def_static("AddChannelToStdErr", &cmnLogger::AddChannelToStdErr,
                    py::arg("mask") = CMN_LOG_ALLOW_ALL);

    {
        py::class_<cmnPath, cmnGenericObject> pathCls(m, "cmnPath");
        py::enum_<decltype(cmnPath::HEAD)>(pathCls, "Position")
            .value("HEAD", cmnPath::HEAD)
            .value("TAIL", cmnPath::TAIL)
            .export_values();
        pathCls
            .def(py::init<>())
            .def(py::init<const std::string &>())
            .def("Add", &cmnPath::Add, py::arg("path"), py::arg("position") = cmnPath::TAIL)
            .def("Find", [](const cmnPath & self, const std::string & filename) {
                return self.Find(filename);
            }, py::arg("filename"))
            .def_static("Exists", &cmnPath::Exists)
            .def_static("GetWorkingDirectory", &cmnPath::GetWorkingDirectory);
    }

    cdgBind_cisstCommon_cmnJointType(m);
}
