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

#include <cisstMultiTask/mtsCommandBase.h>
#include <cisstMultiTask/mtsCommandVoid.h>
#include <cisstMultiTask/mtsCommandVoidReturn.h>
#include <cisstMultiTask/mtsCommandRead.h>
#include <cisstMultiTask/mtsCommandWriteBase.h>
#include <cisstMultiTask/mtsCommandWriteReturn.h>
#include <cisstMultiTask/mtsCommandQualifiedRead.h>
#include <cisstMultiTask/mtsEventReceiver.h>
#include <cisstMultiTask/mtsInterface.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsFunctionBase.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionVoidReturn.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsFunctionWriteReturn.h>
#include <cisstMultiTask/mtsFunctionQualifiedRead.h>
#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsTask.h>
#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsTaskFromSignal.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsStateIndex.h>
#include <cisstMultiTask/mtsIntervalStatistics.h>
#include <cisstMultiTask/mtsCollectorBase.h>
#include <cisstMultiTask/mtsCollectorState.h>
#include <cisstMultiTask/mtsParameterTypes.h>
#include <cisstMultiTask/mtsManagerComponentServices.h>
#include <cisstMultiTask/mtsComponentState.h>
#include <cisstMultiTask/mtsInterfaceCommon.h>

namespace py = pybind11;

void cdgBind_cisstMultiTask_mtsExecutionResult(py::module & m);
void cdgBind_cisstMultiTask_mtsComponentState(py::module & m);
void cdgBind_cisstMultiTask_mtsInterfaceCommon(py::module & m);
void cdgBind_cisstMultiTask_mtsParameterTypes(py::module & m);

template <typename _elementType>
void bindGenericObjectProxy(py::module & m, const char * name)
{
    using ProxyType = mtsGenericObjectProxy<_elementType>;
    py::class_<ProxyType, mtsGenericObject>(m, name)
        .def(py::init<>())
        .def(py::init<const _elementType &>())
        .def_readwrite("Data", &ProxyType::Data)
        .def("GetData", py::overload_cast<>(&ProxyType::GetData, py::const_))
        .def("GetDataCopy", [](const ProxyType & self) -> _elementType {
            return static_cast<_elementType>(self.GetData());
        })
        .def("__copy__", [](const ProxyType & self) {
            return ProxyType(self);
        });
}

PYBIND11_MODULE(_cisstMultiTaskPython, m)
{
    m.doc() = "pybind11 bindings for cisstMultiTask";

    py::module_::import("cisstCommonPython");
    py::module_::import("cisstOSAbstractionPython");

    // Enums from mtsForwardDeclarations.h
    py::enum_<mtsInterfaceQueueingPolicy>(m, "mtsInterfaceQueueingPolicy")
        .value("MTS_COMPONENT_POLICY", MTS_COMPONENT_POLICY)
        .value("MTS_COMMANDS_SHOULD_NOT_BE_QUEUED", MTS_COMMANDS_SHOULD_NOT_BE_QUEUED)
        .value("MTS_COMMANDS_SHOULD_BE_QUEUED", MTS_COMMANDS_SHOULD_BE_QUEUED)
        .export_values();

    py::enum_<mtsCommandQueueingPolicy>(m, "mtsCommandQueueingPolicy")
        .value("MTS_INTERFACE_COMMAND_POLICY", MTS_INTERFACE_COMMAND_POLICY)
        .value("MTS_COMMAND_QUEUED", MTS_COMMAND_QUEUED)
        .value("MTS_COMMAND_NOT_QUEUED", MTS_COMMAND_NOT_QUEUED)
        .export_values();

    py::enum_<mtsEventQueueingPolicy>(m, "mtsEventQueueingPolicy")
        .value("MTS_INTERFACE_EVENT_POLICY", MTS_INTERFACE_EVENT_POLICY)
        .value("MTS_EVENT_QUEUED", MTS_EVENT_QUEUED)
        .value("MTS_EVENT_NOT_QUEUED", MTS_EVENT_NOT_QUEUED)
        .export_values();

    py::enum_<mtsRequiredType>(m, "mtsRequiredType")
        .value("MTS_OPTIONAL", MTS_OPTIONAL)
        .value("MTS_REQUIRED", MTS_REQUIRED)
        .export_values();

    py::enum_<mtsBlockingType>(m, "mtsBlockingType")
        .value("MTS_BLOCKING", MTS_BLOCKING)
        .value("MTS_NOT_BLOCKING", MTS_NOT_BLOCKING)
        .export_values();

    // mtsGenericObject (must be bound before CDG types that inherit from it)
    py::class_<mtsGenericObject, cmnGenericObject>(m, "mtsGenericObject")
        .def("SetTimestamp", &mtsGenericObject::SetTimestamp)
        .def("Timestamp", py::overload_cast<>(&mtsGenericObject::Timestamp, py::const_))
        .def("GetTimestamp", [](const mtsGenericObject & self) { return self.Timestamp(); })
        .def("SetAutomaticTimestamp", &mtsGenericObject::SetAutomaticTimestamp)
        .def("AutomaticTimestamp", py::overload_cast<>(&mtsGenericObject::AutomaticTimestamp, py::const_))
        .def("GetAutomaticTimestamp", [](const mtsGenericObject & self) { return self.AutomaticTimestamp(); })
        .def("SetValid", &mtsGenericObject::SetValid)
        .def("Valid", py::overload_cast<>(&mtsGenericObject::Valid, py::const_))
        .def("GetValid", [](const mtsGenericObject & self) { return self.Valid(); });

    // Generated CDG bindings
    cdgBind_cisstMultiTask_mtsExecutionResult(m);
    cdgBind_cisstMultiTask_mtsComponentState(m);
    cdgBind_cisstMultiTask_mtsInterfaceCommon(m);
    cdgBind_cisstMultiTask_mtsParameterTypes(m);

    // mtsCommandBase and subclasses
    py::class_<mtsCommandBase>(m, "mtsCommandBase", py::dynamic_attr())
        .def("GetName", &mtsCommandBase::GetName)
        .def("NumberOfArguments", &mtsCommandBase::NumberOfArguments)
        .def("Returns", &mtsCommandBase::Returns)
        .def("IsEnabled", &mtsCommandBase::IsEnabled)
        .def("Disable", &mtsCommandBase::Disable)
        .def("Enable", &mtsCommandBase::Enable);

    py::class_<mtsCommandVoid, mtsCommandBase>(m, "mtsCommandVoid")
        .def("Execute", py::overload_cast<mtsBlockingType>(&mtsCommandVoid::Execute));

    py::class_<mtsCommandVoidReturn, mtsCommandBase>(m, "mtsCommandVoidReturn")
        .def("Execute", py::overload_cast<mtsGenericObject &>(&mtsCommandVoidReturn::Execute))
        .def("GetResultPrototype", &mtsCommandVoidReturn::GetResultPrototype,
             py::return_value_policy::reference);

    py::class_<mtsCommandRead, mtsCommandBase>(m, "mtsCommandRead")
        .def("Execute", py::overload_cast<mtsGenericObject &>(&mtsCommandRead::Execute))
        .def("GetArgumentPrototype", &mtsCommandRead::GetArgumentPrototype,
             py::return_value_policy::reference)
        .def("GetResultPrototype", &mtsCommandRead::GetResultPrototype,
             py::return_value_policy::reference);

    py::class_<mtsCommandWriteBase, mtsCommandBase>(m, "mtsCommandWriteBase")
        .def("Execute", py::overload_cast<const mtsGenericObject &, mtsBlockingType>(&mtsCommandWriteBase::Execute))
        .def("GetArgumentPrototype", &mtsCommandWriteBase::GetArgumentPrototype,
             py::return_value_policy::reference);

    py::class_<mtsCommandWriteReturn, mtsCommandBase>(m, "mtsCommandWriteReturn")
        .def("Execute", py::overload_cast<const mtsGenericObject &, mtsGenericObject &>(&mtsCommandWriteReturn::Execute))
        .def("GetArgumentPrototype", &mtsCommandWriteReturn::GetArgumentPrototype,
             py::return_value_policy::reference)
        .def("GetResultPrototype", &mtsCommandWriteReturn::GetResultPrototype,
             py::return_value_policy::reference);

    py::class_<mtsCommandQualifiedRead, mtsCommandBase>(m, "mtsCommandQualifiedRead")
        .def("Execute", py::overload_cast<const mtsGenericObject &, mtsGenericObject &>(&mtsCommandQualifiedRead::Execute))
        .def("GetArgumentPrototype", &mtsCommandQualifiedRead::GetArgumentPrototype,
             py::return_value_policy::reference)
        .def("GetResultPrototype", &mtsCommandQualifiedRead::GetResultPrototype,
             py::return_value_policy::reference);

    // mtsFunctionBase and subclasses
    py::class_<mtsFunctionBase, std::unique_ptr<mtsFunctionBase, py::nodelete>>(m, "mtsFunctionBase", py::dynamic_attr())
        .def("IsValid", &mtsFunctionBase::IsValid);

    py::class_<mtsFunctionVoid, mtsFunctionBase>(m, "mtsFunctionVoid")
        .def(py::init<>())
        .def("Execute", &mtsFunctionVoid::Execute)
        .def("ExecuteBlocking", &mtsFunctionVoid::ExecuteBlocking);

    py::class_<mtsFunctionVoidReturn, mtsFunctionBase>(m, "mtsFunctionVoidReturn")
        .def(py::init<>())
        .def("ExecuteGeneric", &mtsFunctionVoidReturn::ExecuteGeneric)
        .def("GetResultPrototype", &mtsFunctionVoidReturn::GetResultPrototype,
             py::return_value_policy::reference);

    py::class_<mtsFunctionRead, mtsFunctionBase>(m, "mtsFunctionRead")
        .def(py::init<>())
        .def("ExecuteGeneric", &mtsFunctionRead::ExecuteGeneric)
        .def("GetArgumentPrototype", &mtsFunctionRead::GetArgumentPrototype,
             py::return_value_policy::reference);

    py::class_<mtsFunctionWrite, mtsFunctionBase>(m, "mtsFunctionWrite")
        .def(py::init<>())
        .def("ExecuteGeneric", &mtsFunctionWrite::ExecuteGeneric)
        .def("ExecuteBlockingGeneric", &mtsFunctionWrite::ExecuteBlockingGeneric)
        .def("GetArgumentPrototype", &mtsFunctionWrite::GetArgumentPrototype,
             py::return_value_policy::reference);

    py::class_<mtsFunctionWriteReturn, mtsFunctionBase>(m, "mtsFunctionWriteReturn")
        .def(py::init<>())
        .def("ExecuteGeneric", &mtsFunctionWriteReturn::ExecuteGeneric)
        .def("GetArgumentPrototype", &mtsFunctionWriteReturn::GetArgumentPrototype,
             py::return_value_policy::reference)
        .def("GetResultPrototype", &mtsFunctionWriteReturn::GetResultPrototype,
             py::return_value_policy::reference);

    py::class_<mtsFunctionQualifiedRead, mtsFunctionBase>(m, "mtsFunctionQualifiedRead")
        .def(py::init<>())
        .def("ExecuteGeneric", &mtsFunctionQualifiedRead::ExecuteGeneric)
        .def("GetArgument1Prototype", &mtsFunctionQualifiedRead::GetArgument1Prototype,
             py::return_value_policy::reference)
        .def("GetArgument2Prototype", &mtsFunctionQualifiedRead::GetArgument2Prototype,
             py::return_value_policy::reference);

    // mtsInterface, mtsInterfaceProvided, mtsInterfaceRequired
    py::class_<mtsInterface, cmnGenericObject>(m, "mtsInterface", py::dynamic_attr())
        .def("GetName", &mtsInterface::GetName)
        .def("GetComponent", &mtsInterface::GetComponent,
             py::return_value_policy::reference);

    py::class_<mtsInterfaceProvided, mtsInterface>(m, "mtsInterfaceProvided")
        .def("GetNamesOfCommands", &mtsInterfaceProvided::GetNamesOfCommands)
        .def("GetNamesOfCommandsVoid", &mtsInterfaceProvided::GetNamesOfCommandsVoid)
        .def("GetNamesOfCommandsVoidReturn", &mtsInterfaceProvided::GetNamesOfCommandsVoidReturn)
        .def("GetNamesOfCommandsWrite", &mtsInterfaceProvided::GetNamesOfCommandsWrite)
        .def("GetNamesOfCommandsWriteReturn", &mtsInterfaceProvided::GetNamesOfCommandsWriteReturn)
        .def("GetNamesOfCommandsRead", &mtsInterfaceProvided::GetNamesOfCommandsRead)
        .def("GetNamesOfCommandsQualifiedRead", &mtsInterfaceProvided::GetNamesOfCommandsQualifiedRead)
        .def("GetCommandVoid", &mtsInterfaceProvided::GetCommandVoid,
             py::arg("name"), py::arg("required") = MTS_REQUIRED,
             py::return_value_policy::reference)
        .def("GetCommandVoidReturn", &mtsInterfaceProvided::GetCommandVoidReturn,
             py::arg("name"), py::arg("required") = MTS_REQUIRED,
             py::return_value_policy::reference)
        .def("GetCommandWrite", &mtsInterfaceProvided::GetCommandWrite,
             py::arg("name"), py::arg("required") = MTS_REQUIRED,
             py::return_value_policy::reference)
        .def("GetCommandWriteReturn", &mtsInterfaceProvided::GetCommandWriteReturn,
             py::arg("name"), py::arg("required") = MTS_REQUIRED,
             py::return_value_policy::reference)
        .def("GetCommandRead", &mtsInterfaceProvided::GetCommandRead,
             py::arg("name"), py::arg("required") = MTS_REQUIRED,
             py::return_value_policy::reference)
        .def("GetCommandQualifiedRead", &mtsInterfaceProvided::GetCommandQualifiedRead,
             py::arg("name"), py::arg("required") = MTS_REQUIRED,
             py::return_value_policy::reference)
        .def("GetNamesOfEventsVoid", &mtsInterfaceProvided::GetNamesOfEventsVoid)
        .def("GetNamesOfEventsWrite", &mtsInterfaceProvided::GetNamesOfEventsWrite)
        .def("GetEndUserInterface", &mtsInterfaceProvided::GetEndUserInterface,
             py::return_value_policy::reference)
        .def("GetDescription", py::overload_cast<>(&mtsInterfaceProvided::GetDescription, py::const_));

    py::class_<mtsInterfaceRequired, mtsInterface>(m, "mtsInterfaceRequired")
        .def("AddFunction", py::overload_cast<const std::string &, mtsFunctionVoid &, mtsRequiredType>(&mtsInterfaceRequired::AddFunction),
             py::arg("name"), py::arg("function"), py::arg("required") = MTS_REQUIRED,
             py::keep_alive<1, 3>())
        .def("AddFunction", py::overload_cast<const std::string &, mtsFunctionVoidReturn &, mtsRequiredType>(&mtsInterfaceRequired::AddFunction),
             py::arg("name"), py::arg("function"), py::arg("required") = MTS_REQUIRED,
             py::keep_alive<1, 3>())
        .def("AddFunction", py::overload_cast<const std::string &, mtsFunctionWrite &, mtsRequiredType>(&mtsInterfaceRequired::AddFunction),
             py::arg("name"), py::arg("function"), py::arg("required") = MTS_REQUIRED,
             py::keep_alive<1, 3>())
        .def("AddFunction", py::overload_cast<const std::string &, mtsFunctionWriteReturn &, mtsRequiredType>(&mtsInterfaceRequired::AddFunction),
             py::arg("name"), py::arg("function"), py::arg("required") = MTS_REQUIRED,
             py::keep_alive<1, 3>())
        .def("AddFunction", py::overload_cast<const std::string &, mtsFunctionRead &, mtsRequiredType>(&mtsInterfaceRequired::AddFunction),
             py::arg("name"), py::arg("function"), py::arg("required") = MTS_REQUIRED,
             py::keep_alive<1, 3>())
        .def("AddFunction", py::overload_cast<const std::string &, mtsFunctionQualifiedRead &, mtsRequiredType>(&mtsInterfaceRequired::AddFunction),
             py::arg("name"), py::arg("function"), py::arg("required") = MTS_REQUIRED,
             py::keep_alive<1, 3>())
        .def("GetNamesOfFunctions", &mtsInterfaceRequired::GetNamesOfFunctions)
        .def("GetNamesOfFunctionsVoid", &mtsInterfaceRequired::GetNamesOfFunctionsVoid)
        .def("GetNamesOfFunctionsVoidReturn", &mtsInterfaceRequired::GetNamesOfFunctionsVoidReturn)
        .def("GetNamesOfFunctionsWrite", &mtsInterfaceRequired::GetNamesOfFunctionsWrite)
        .def("GetNamesOfFunctionsWriteReturn", &mtsInterfaceRequired::GetNamesOfFunctionsWriteReturn)
        .def("GetNamesOfFunctionsRead", &mtsInterfaceRequired::GetNamesOfFunctionsRead)
        .def("GetNamesOfFunctionsQualifiedRead", &mtsInterfaceRequired::GetNamesOfFunctionsQualifiedRead)
        .def("GetFunctionVoid", &mtsInterfaceRequired::GetFunctionVoid,
             py::return_value_policy::reference)
        .def("GetFunctionVoidReturn", &mtsInterfaceRequired::GetFunctionVoidReturn,
             py::return_value_policy::reference)
        .def("GetFunctionWrite", &mtsInterfaceRequired::GetFunctionWrite,
             py::return_value_policy::reference)
        .def("GetFunctionWriteReturn", &mtsInterfaceRequired::GetFunctionWriteReturn,
             py::return_value_policy::reference)
        .def("GetFunctionRead", &mtsInterfaceRequired::GetFunctionRead,
             py::return_value_policy::reference)
        .def("GetFunctionQualifiedRead", &mtsInterfaceRequired::GetFunctionQualifiedRead,
             py::return_value_policy::reference)
        .def("GetConnectedInterface", &mtsInterfaceRequired::GetConnectedInterface,
             py::return_value_policy::reference)
        .def("GetDescription", py::overload_cast<>(&mtsInterfaceRequired::GetDescription, py::const_));

    // mtsComponent and mtsTask hierarchy
    py::class_<mtsComponent, cmnGenericObject>(m, "mtsComponent", py::dynamic_attr())
        .def(py::init<const std::string &>())
        .def("GetName", py::overload_cast<>(&mtsComponent::GetName, py::const_))
        .def("GetNamesOfInterfacesProvided", &mtsComponent::GetNamesOfInterfacesProvided)
        .def("GetNamesOfInterfacesRequired", &mtsComponent::GetNamesOfInterfacesRequired)
        .def("GetInterfaceProvided", py::overload_cast<const std::string &>(&mtsComponent::GetInterfaceProvided, py::const_),
             py::return_value_policy::reference)
        .def("GetInterfaceRequired", py::overload_cast<const std::string &>(&mtsComponent::GetInterfaceRequired),
             py::return_value_policy::reference)
        .def("AddInterfaceProvided", &mtsComponent::AddInterfaceProvided,
             py::arg("interfaceProvidedName"),
             py::arg("interfaceQueueingPolicy") = MTS_COMPONENT_POLICY,
             py::return_value_policy::reference)
        .def("AddInterfaceRequired",
             py::overload_cast<const std::string &, mtsRequiredType>(&mtsComponent::AddInterfaceRequired),
             py::arg("interfaceRequiredName"),
             py::arg("required") = MTS_REQUIRED,
             py::return_value_policy::reference)
        .def("Configure", &mtsComponent::Configure, py::arg("filename") = "")
        .def("GetState", py::overload_cast<>(&mtsComponent::GetState, py::const_))
        .def("AreAllInterfacesRequiredConnected", &mtsComponent::AreAllInterfacesRequiredConnected);

    py::class_<mtsTask, mtsComponent>(m, "mtsTask")
        .def("Create", py::overload_cast<>(&mtsTask::Create))
        .def("Start", &mtsTask::Start)
        .def("Kill", &mtsTask::Kill)
        .def("WaitToStart", &mtsTask::WaitToStart, py::arg("timeout") = 0.0)
        .def("WaitToTerminate", &mtsTask::WaitToTerminate, py::arg("timeout") = 0.0)
        .def("IsRunning", &mtsTask::IsRunning)
        .def("IsTerminated", &mtsTask::IsTerminated);

    py::class_<mtsTaskContinuous, mtsTask>(m, "mtsTaskContinuous");

    py::class_<mtsTaskPeriodic, mtsTaskContinuous>(m, "mtsTaskPeriodic")
        .def("GetPeriodicity", &mtsTaskPeriodic::GetPeriodicity);

    py::class_<mtsTaskFromSignal, mtsTaskContinuous>(m, "mtsTaskFromSignal");

    // mtsManagerComponentServices
    py::class_<mtsManagerComponentServices>(m, "mtsManagerComponentServices")
        .def("GetNamesOfComponents", py::overload_cast<const std::string &>(&mtsManagerComponentServices::GetNamesOfComponents, py::const_),
             py::arg("processName") = "")
        .def("GetNamesOfInterfaces", [](mtsManagerComponentServices & self, const std::string & processName, const std::string & componentName) {
            std::vector<std::string> required, provided;
            if (!self.GetNamesOfInterfaces(processName, componentName, required, provided)) {
                throw std::runtime_error("Failed to get interfaces for component: " + componentName);
            }
            return py::make_tuple(required, provided);
        })
        .def("GetInterfaceProvidedDescription",
             py::overload_cast<const std::string &, const std::string &, const std::string &>(&mtsManagerComponentServices::GetInterfaceProvidedDescription, py::const_))
        .def("GetInterfaceRequiredDescription",
             py::overload_cast<const std::string &, const std::string &, const std::string &>(&mtsManagerComponentServices::GetInterfaceRequiredDescription, py::const_));

    // mtsManagerLocal (singleton)
    py::class_<mtsManagerLocal, cmnGenericObject, std::unique_ptr<mtsManagerLocal, py::nodelete>>(m, "mtsManagerLocal", py::dynamic_attr())
        .def_static("GetInstance", py::overload_cast<>(&mtsManagerLocal::GetInstance),
                    py::return_value_policy::reference)
        .def("AddComponent", &mtsManagerLocal::AddComponent)
        .def("GetComponent", &mtsManagerLocal::GetComponent,
             py::return_value_policy::reference)
        .def("FindComponent", &mtsManagerLocal::FindComponent)
        .def("GetProcessName", &mtsManagerLocal::GetProcessName)
        .def("GetInterfaceProvidedDescription",
             py::overload_cast<const std::string &, const std::string &, const std::string &>(&mtsManagerLocal::GetInterfaceProvidedDescription, py::const_))
        .def("GetNamesOfComponents", py::overload_cast<>(&mtsManagerLocal::GetNamesOfComponents, py::const_))
        .def("CreateComponentDynamically",
             py::overload_cast<const std::string &, const std::string &, const std::string &>(&mtsManagerLocal::CreateComponentDynamically),
             py::return_value_policy::reference)
        .def("CreateComponentDynamically",
             py::overload_cast<const std::string &, const mtsGenericObject &>(&mtsManagerLocal::CreateComponentDynamically),
             py::return_value_policy::reference)
        .def("CreateComponentDynamically",
             [](mtsManagerLocal & self, const std::string & className, const std::string & constructorArgSerialized) {
                 return self.CreateComponentDynamically(className, mtsStdString(constructorArgSerialized));
             },
             py::return_value_policy::reference)
        .def("Connect",
             py::overload_cast<const std::string &, const std::string &, const std::string &, const std::string &>(&mtsManagerLocal::Connect))
        .def("Connect",
             py::overload_cast<const std::string &, const std::string &, const std::string &, const std::string &, const std::string &, const std::string &>(&mtsManagerLocal::Connect))
        .def("Disconnect",
             py::overload_cast<const std::string &, const std::string &, const std::string &, const std::string &>(&mtsManagerLocal::Disconnect))
        .def("CreateAll", &mtsManagerLocal::CreateAll)
        .def("CreateAllAndWait", &mtsManagerLocal::CreateAllAndWait, py::arg("timeoutInSeconds"))
        .def("StartAll", &mtsManagerLocal::StartAll)
        .def("StartAllAndWait", &mtsManagerLocal::StartAllAndWait, py::arg("timeoutInSeconds"))
        .def("KillAll", &mtsManagerLocal::KillAll)
        .def("KillAllAndWait", &mtsManagerLocal::KillAllAndWait, py::arg("timeoutInSeconds"))
        .def("WaitForStateAll", &mtsManagerLocal::WaitForStateAll,
             py::arg("desiredState"), py::arg("timeoutInSeconds") = -1.0);

    // mtsCollectorBase & mtsCollectorState
    py::class_<mtsCollectorBase, mtsTaskFromSignal>(m, "mtsCollectorBase")
        .def("StartCollection", &mtsCollectorBase::StartCollection)
        .def("StopCollection", &mtsCollectorBase::StopCollection)
        .def("SetOutputToDefault", py::overload_cast<>(&mtsCollectorBase::SetOutputToDefault));

    py::class_<mtsCollectorState, mtsCollectorBase>(m, "mtsCollectorState")
        .def(py::init<const std::string &>(), py::arg("name"))
        .def("AddSignal", &mtsCollectorState::AddSignal, py::arg("signalName") = "")
        .def("SetSamplingInterval", &mtsCollectorState::SetSamplingInterval);

    // mtsStateIndex & mtsIntervalStatistics
    py::class_<mtsStateIndex, mtsGenericObject>(m, "mtsStateIndex")
        .def(py::init<>())
        .def("Ticks", &mtsStateIndex::Ticks)
        .def("Index", &mtsStateIndex::Index)
        .def("Length", &mtsStateIndex::Length);

    py::class_<mtsIntervalStatistics, mtsGenericObject>(m, "mtsIntervalStatistics")
        .def(py::init<>())
        .def("PeriodAvg", &mtsIntervalStatistics::PeriodAvg)
        .def("PeriodStdDev", &mtsIntervalStatistics::PeriodStdDev)
        .def("PeriodMin", &mtsIntervalStatistics::PeriodMin)
        .def("PeriodMax", &mtsIntervalStatistics::PeriodMax)
        .def("ComputeTimeAvg", &mtsIntervalStatistics::ComputeTimeAvg)
        .def("ComputeTimeStdDev", &mtsIntervalStatistics::ComputeTimeStdDev)
        .def("ComputeTimeMin", &mtsIntervalStatistics::ComputeTimeMin)
        .def("ComputeTimeMax", &mtsIntervalStatistics::ComputeTimeMax);

    // GenericObjectProxy instantiations
    bindGenericObjectProxy<double>(m, "mtsDouble");
    bindGenericObjectProxy<float>(m, "mtsFloat");
    bindGenericObjectProxy<int>(m, "mtsInt");
    bindGenericObjectProxy<unsigned int>(m, "mtsUInt");
    bindGenericObjectProxy<short>(m, "mtsShort");
    bindGenericObjectProxy<unsigned short>(m, "mtsUShort");
    bindGenericObjectProxy<char>(m, "mtsChar");
    bindGenericObjectProxy<unsigned char>(m, "mtsUChar");
    bindGenericObjectProxy<long>(m, "mtsLong");
    bindGenericObjectProxy<unsigned long>(m, "mtsULong");
    bindGenericObjectProxy<bool>(m, "mtsBool");
    bindGenericObjectProxy<std::string>(m, "mtsStdString");
    bindGenericObjectProxy<stdStringVec>(m, "mtsStdStringVecProxy");
    bindGenericObjectProxy<stdDoubleVec>(m, "mtsStdDoubleVecProxy");
    bindGenericObjectProxy<stdDouble3>(m, "mtsStdDouble3Proxy");
    bindGenericObjectProxy<stdCharVec>(m, "mtsStdCharVecProxy");
    bindGenericObjectProxy<mtsDescriptionConnectionVec>(m, "mtsDescriptionConnectionVecProxy");
    bindGenericObjectProxy<mtsDescriptionComponentClassVec>(m, "mtsDescriptionComponentClassVecProxy");
    bindGenericObjectProxy<mtsComponentState>(m, "mtsComponentStateProxy");
    bindGenericObjectProxy<mtsInterfaceProvidedDescription>(m, "mtsInterfaceProvidedDescriptionProxy");
    bindGenericObjectProxy<mtsInterfaceRequiredDescription>(m, "mtsInterfaceRequiredDescriptionProxy");

    // Eigen::VectorXd proxy (supports .Data() for mtsPeriodicTaskTest.py)
    using EigenVecProxy = mtsGenericObjectProxy<Eigen::VectorXd>;
    py::class_<EigenVecProxy, mtsGenericObject>(m, "mtsEigenVectorXd")
        .def(py::init<>())
        .def(py::init<const Eigen::VectorXd &>())
        .def("Data", [](const EigenVecProxy & self) -> const Eigen::VectorXd & {
            return self.Data;
        })
        .def("__copy__", [](const EigenVecProxy & self) {
            return EigenVecProxy(self);
        });
}
