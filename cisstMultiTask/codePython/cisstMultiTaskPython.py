# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
# ex: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:

# Author(s):  Anton Deguet
# Date:       2008-01-17
#
# (C) Copyright 2008-2026 Johns Hopkins University (JHU), All Rights Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

import copy
from _cisstMultiTaskPython import *


mtsExecutionResult.IsOK = lambda self: self.Value() in (mtsExecutionResult.COMMAND_SUCCEEDED, mtsExecutionResult.COMMAND_QUEUED)
mtsExecutionResult.__bool__ = mtsExecutionResult.IsOK


def _mtsCommandVoid_call(self, blocking=MTS_NOT_BLOCKING):
    return self.Execute(blocking)


def _mtsCommandVoid_UpdateFromC(self):
    self.__doc__ = self.GetName() + "() -> mtsExecutionResult"


mtsCommandVoid.__call__ = _mtsCommandVoid_call
mtsCommandVoid.UpdateFromC = _mtsCommandVoid_UpdateFromC


def _mtsCommandVoidReturn_call(self):
    result = copy.copy(self.GetResultPrototype())
    executionResult = self.Execute(result)
    if executionResult.IsOK():
        return result
    else:
        raise RuntimeError(executionResult)


def _mtsCommandVoidReturn_UpdateFromC(self):
    resName = self.GetResultPrototype().Services().GetName()
    self.__doc__ = self.GetName() + "() -> " + resName


mtsCommandVoidReturn.__call__ = _mtsCommandVoidReturn_call
mtsCommandVoidReturn.UpdateFromC = _mtsCommandVoidReturn_UpdateFromC


def _mtsCommandRead_call(self):
    argument = copy.copy(self.GetArgumentPrototype())
    executionResult = self.Execute(argument)
    if executionResult.IsOK():
        return argument
    else:
        raise RuntimeError(executionResult)


def _mtsCommandRead_UpdateFromC(self):
    argName = self.GetArgumentPrototype().Services().GetName()
    self.__doc__ = self.GetName() + "() -> " + argName


mtsCommandRead.__call__ = _mtsCommandRead_call
mtsCommandRead.UpdateFromC = _mtsCommandRead_UpdateFromC


def _mtsCommandWriteBase_call(self, argument, blocking=MTS_NOT_BLOCKING):
    if not isinstance(argument, mtsGenericObject):
        proto = self.GetArgumentPrototype()
        argumentObject = copy.copy(proto)
        argumentObject.Data = argument
    else:
        argumentObject = argument
    return self.Execute(argumentObject, blocking)


def _mtsCommandWriteBase_UpdateFromC(self):
    argName = self.GetArgumentPrototype().Services().GetName()
    self.__doc__ = self.GetName() + "(" + argName + ") -> mtsExecutionResult"


mtsCommandWriteBase.__call__ = _mtsCommandWriteBase_call
mtsCommandWriteBase.UpdateFromC = _mtsCommandWriteBase_UpdateFromC


def _mtsCommandWriteReturn_call(self, argument):
    if not isinstance(argument, mtsGenericObject):
        proto = self.GetArgumentPrototype()
        argumentObject = copy.copy(proto)
        argumentObject.Data = argument
    else:
        argumentObject = argument
    result = copy.copy(self.GetResultPrototype())
    executionResult = self.Execute(argumentObject, result)
    if executionResult.IsOK():
        return result
    else:
        raise RuntimeError(executionResult)


def _mtsCommandWriteReturn_UpdateFromC(self):
    argName = self.GetArgumentPrototype().Services().GetName()
    resName = self.GetResultPrototype().Services().GetName()
    self.__doc__ = self.GetName() + "(" + argName + ") -> " + resName


mtsCommandWriteReturn.__call__ = _mtsCommandWriteReturn_call
mtsCommandWriteReturn.UpdateFromC = _mtsCommandWriteReturn_UpdateFromC


def _mtsCommandQualifiedRead_call(self, argument1):
    if not isinstance(argument1, mtsGenericObject):
        proto = self.GetArgumentPrototype()
        argument1Object = copy.copy(proto)
        argument1Object.Data = argument1
    else:
        argument1Object = argument1
    argument2 = copy.copy(self.GetResultPrototype())
    executionResult = self.Execute(argument1Object, argument2)
    if executionResult.IsOK():
        return argument2
    else:
        raise RuntimeError(executionResult)


def _mtsCommandQualifiedRead_UpdateFromC(self):
    argName = self.GetArgumentPrototype().Services().GetName()
    resName = self.GetResultPrototype().Services().GetName()
    self.__doc__ = self.GetName() + "(" + argName + ") -> " + resName


mtsCommandQualifiedRead.__call__ = _mtsCommandQualifiedRead_call
mtsCommandQualifiedRead.UpdateFromC = _mtsCommandQualifiedRead_UpdateFromC


def _mtsFunctionVoid_call(self, blocking=MTS_NOT_BLOCKING):
    if blocking == MTS_BLOCKING:
        return self.ExecuteBlocking()
    return self.Execute()


def _mtsFunctionVoid_UpdateFromC(self):
    self.__doc__ = "() -> mtsExecutionResult"


mtsFunctionVoid.__call__ = _mtsFunctionVoid_call
mtsFunctionVoid.UpdateFromC = _mtsFunctionVoid_UpdateFromC


def _mtsFunctionVoidReturn_call(self):
    result = copy.copy(self.GetResultPrototype())
    executionResult = self.ExecuteGeneric(result)
    if executionResult.IsOK():
        return result
    else:
        raise RuntimeError(executionResult)


def _mtsFunctionVoidReturn_UpdateFromC(self):
    resName = self.GetResultPrototype().Services().GetName()
    self.__doc__ = "() -> " + resName


mtsFunctionVoidReturn.__call__ = _mtsFunctionVoidReturn_call
mtsFunctionVoidReturn.UpdateFromC = _mtsFunctionVoidReturn_UpdateFromC


def _mtsFunctionRead_call(self):
    argument = copy.copy(self.GetArgumentPrototype())
    executionResult = self.ExecuteGeneric(argument)
    if executionResult.IsOK():
        return argument
    else:
        raise RuntimeError(executionResult)


def _mtsFunctionRead_UpdateFromC(self):
    argName = self.GetArgumentPrototype().Services().GetName()
    self.__doc__ = "() -> " + argName


mtsFunctionRead.__call__ = _mtsFunctionRead_call
mtsFunctionRead.UpdateFromC = _mtsFunctionRead_UpdateFromC


def _mtsFunctionWrite_call(self, argument, blocking=MTS_NOT_BLOCKING):
    if not isinstance(argument, mtsGenericObject):
        proto = self.GetArgumentPrototype()
        argumentObject = copy.copy(proto)
        argumentObject.Data = argument
    else:
        argumentObject = argument
    if blocking == MTS_BLOCKING:
        return self.ExecuteBlockingGeneric(argumentObject)
    return self.ExecuteGeneric(argumentObject)


def _mtsFunctionWrite_UpdateFromC(self):
    argName = self.GetArgumentPrototype().Services().GetName()
    self.__doc__ = "(" + argName + ") -> mtsExecutionResult"


mtsFunctionWrite.__call__ = _mtsFunctionWrite_call
mtsFunctionWrite.UpdateFromC = _mtsFunctionWrite_UpdateFromC


def _mtsFunctionWriteReturn_call(self, argument):
    if not isinstance(argument, mtsGenericObject):
        proto = self.GetArgumentPrototype()
        argumentObject = copy.copy(proto)
        argumentObject.Data = argument
    else:
        argumentObject = argument
    result = copy.copy(self.GetResultPrototype())
    executionResult = self.ExecuteGeneric(argumentObject, result)
    if executionResult.IsOK():
        return result
    else:
        raise RuntimeError(executionResult)


def _mtsFunctionWriteReturn_UpdateFromC(self):
    argName = self.GetArgumentPrototype().Services().GetName()
    resName = self.GetResultPrototype().Services().GetName()
    self.__doc__ = "(" + argName + ") -> " + resName


mtsFunctionWriteReturn.__call__ = _mtsFunctionWriteReturn_call
mtsFunctionWriteReturn.UpdateFromC = _mtsFunctionWriteReturn_UpdateFromC


def _mtsFunctionQualifiedRead_call(self, argument1):
    if not isinstance(argument1, mtsGenericObject):
        proto = self.GetArgument1Prototype()
        argument1Object = copy.copy(proto)
        argument1Object.Data = argument1
    else:
        argument1Object = argument1
    argument2 = copy.copy(self.GetArgument2Prototype())
    executionResult = self.ExecuteGeneric(argument1Object, argument2)
    if executionResult.IsOK():
        return argument2
    else:
        raise RuntimeError(executionResult)


def _mtsFunctionQualifiedRead_UpdateFromC(self):
    argName = self.GetArgument1Prototype().Services().GetName()
    resName = self.GetArgument2Prototype().Services().GetName()
    self.__doc__ = "(" + argName + ") -> " + resName


mtsFunctionQualifiedRead.__call__ = _mtsFunctionQualifiedRead_call
mtsFunctionQualifiedRead.UpdateFromC = _mtsFunctionQualifiedRead_UpdateFromC


def _mtsInterfaceProvided_UpdateFromC(self):
    commands = self.GetNamesOfCommandsVoid()
    for command in commands:
        self.__dict__[command] = self.GetCommandVoid(command)
        self.__dict__[command].UpdateFromC()
    commands = self.GetNamesOfCommandsVoidReturn()
    for command in commands:
        self.__dict__[command] = self.GetCommandVoidReturn(command)
        self.__dict__[command].UpdateFromC()
    commands = self.GetNamesOfCommandsWrite()
    for command in commands:
        self.__dict__[command] = self.GetCommandWrite(command)
        self.__dict__[command].UpdateFromC()
    commands = self.GetNamesOfCommandsWriteReturn()
    for command in commands:
        self.__dict__[command] = self.GetCommandWriteReturn(command)
        self.__dict__[command].UpdateFromC()
    commands = self.GetNamesOfCommandsQualifiedRead()
    for command in commands:
        self.__dict__[command] = self.GetCommandQualifiedRead(command)
        self.__dict__[command].UpdateFromC()
    commands = self.GetNamesOfCommandsRead()
    for command in commands:
        self.__dict__[command] = self.GetCommandRead(command)
        self.__dict__[command].UpdateFromC()


mtsInterfaceProvided.UpdateFromC = _mtsInterfaceProvided_UpdateFromC


def _mtsInterfaceRequired_UpdateFromC(self):
    functions = self.GetNamesOfFunctionsVoid()
    for function in functions:
        self.__dict__[function] = self.GetFunctionVoid(function)
        self.__dict__[function].UpdateFromC()
    functions = self.GetNamesOfFunctionsVoidReturn()
    for function in functions:
        self.__dict__[function] = self.GetFunctionVoidReturn(function)
        self.__dict__[function].UpdateFromC()
    functions = self.GetNamesOfFunctionsWrite()
    for function in functions:
        self.__dict__[function] = self.GetFunctionWrite(function)
        self.__dict__[function].UpdateFromC()
    functions = self.GetNamesOfFunctionsWriteReturn()
    for function in functions:
        self.__dict__[function] = self.GetFunctionWriteReturn(function)
        self.__dict__[function].UpdateFromC()
    functions = self.GetNamesOfFunctionsQualifiedRead()
    for function in functions:
        self.__dict__[function] = self.GetFunctionQualifiedRead(function)
        self.__dict__[function].UpdateFromC()
    functions = self.GetNamesOfFunctionsRead()
    for function in functions:
        self.__dict__[function] = self.GetFunctionRead(function)
        self.__dict__[function].UpdateFromC()


mtsInterfaceRequired.UpdateFromC = _mtsInterfaceRequired_UpdateFromC


def _mtsComponent_UpdateFromC(self):
    interfaces = self.GetNamesOfInterfacesProvided()
    for interface in interfaces:
        interface_attr = interface.replace(" ", "")
        interfaceFrontEnd = self.GetInterfaceProvided(interface)
        self.__dict__[interface_attr] = interfaceFrontEnd.GetEndUserInterface("Python")
        self.__dict__[interface_attr].UpdateFromC()
    interfaces = self.GetNamesOfInterfacesRequired()
    for interface in interfaces:
        interface_attr = interface.replace(" ", "")
        self.__dict__[interface_attr] = self.GetInterfaceRequired(interface)
        if self.__dict__[interface_attr].GetConnectedInterface():
            self.__dict__[interface_attr].UpdateFromC()


def _mtsComponent_AddInterfaceRequiredFromProvided(self, interfaceRequiredName, serverName, serverInterfaceName):
    interfaceRequired = self.AddInterfaceRequired(interfaceRequiredName, MTS_OPTIONAL)
    if not interfaceRequired:
        return None
    self.__dict__[interfaceRequired.GetName()] = interfaceRequired
    LCM = mtsManagerLocal.GetInstance()
    server = LCM.GetComponent(serverName)
    if not server:
        print("AddInterfaceRequiredFromProvided: failed to find server " + serverName)
        return None
    interfaceProvided = server.GetInterfaceProvided(serverInterfaceName)
    if not interfaceProvided:
        print("AddInterfaceRequiredFromProvided: failed to find interface " + serverInterfaceName + " on " + serverName)
        return None
    for name in interfaceProvided.GetNamesOfCommandsVoid():
        func = mtsFunctionVoid()
        interfaceRequired.AddFunction(name, func)
    for name in interfaceProvided.GetNamesOfCommandsVoidReturn():
        func = mtsFunctionVoidReturn()
        interfaceRequired.AddFunction(name, func)
    for name in interfaceProvided.GetNamesOfCommandsWrite():
        func = mtsFunctionWrite()
        interfaceRequired.AddFunction(name, func)
    for name in interfaceProvided.GetNamesOfCommandsWriteReturn():
        func = mtsFunctionWriteReturn()
        interfaceRequired.AddFunction(name, func)
    for name in interfaceProvided.GetNamesOfCommandsRead():
        func = mtsFunctionRead()
        interfaceRequired.AddFunction(name, func)
    for name in interfaceProvided.GetNamesOfCommandsQualifiedRead():
        func = mtsFunctionQualifiedRead()
        interfaceRequired.AddFunction(name, func)
    return interfaceRequired


def _mtsComponent_AddInterfaceRequiredAndConnect(self, serverTuple):
    serverName = serverTuple[0]
    serverInterfaceName = serverTuple[1]
    interfaceRequiredName = serverName + "_" + serverInterfaceName
    interfaceRequired = self.AddInterfaceRequiredFromProvided(interfaceRequiredName, serverName, serverInterfaceName)
    if not interfaceRequired:
        return None
    LCM = mtsManagerLocal.GetInstance()
    if not LCM.Connect(self.GetName(), interfaceRequiredName, serverName, serverInterfaceName):
        print("AddInterfaceRequiredAndConnect: failed to connect " + self.GetName() + ":" + interfaceRequiredName + " to " + serverName + ":" + serverInterfaceName)
        return None
    interfaceRequired.UpdateFromC()
    return interfaceRequired


mtsComponent.UpdateFromC = _mtsComponent_UpdateFromC
mtsComponent.AddInterfaceRequiredFromProvided = _mtsComponent_AddInterfaceRequiredFromProvided
mtsComponent.AddInterfaceRequiredAndConnect = _mtsComponent_AddInterfaceRequiredAndConnect


def _mtsManagerLocal_UpdateFromC(self):
    comps = self.GetNamesOfComponents()
    for comp in comps:
        self.__dict__[comp] = self.GetComponent(comp)
        self.__dict__[comp].UpdateFromC()


mtsManagerLocal.UpdateFromC = _mtsManagerLocal_UpdateFromC


class mtsInterfaceRequiredPython:
    def __init__(self):
        pass

    def _add_entry(self, cmdName, cmdFunc):
        cmdNameFixed = cmdName.replace('-', '_')
        items = cmdNameFixed.split('/')
        obj = self
        for i in range(0, len(items) - 1):
            if not hasattr(obj, items[i]):
                setattr(obj, items[i], mtsInterfaceRequiredPython())
            obj = getattr(obj, items[i])
        setattr(obj, items[len(items) - 1], cmdFunc)


def mtsLoadAndCreateServer(libraryName, className, componentName, argSerialized):
    print('Loading ' + libraryName)
    import cisstOSAbstractionPython as cisstOSAbstraction
    DynLoader = cisstOSAbstraction.osaDynamicLoader()
    if not DynLoader.Load(libraryName):
        print('Failed to load ' + libraryName + ' (see cisstLog.txt)')
        return None

    print('Creating ' + componentName + ' (' + className + ')')
    LCM = mtsManagerLocal.GetInstance()
    if not argSerialized:
        comp = LCM.CreateComponentDynamically(className, componentName, '')
    else:
        comp = LCM.CreateComponentDynamically(className, argSerialized)
    if comp:
        print('Component created')
        LCM.AddComponent(comp)
    return comp


def mtsCreateClientInterface(clientName, serverName, interfaceName):
    LCM = mtsManagerLocal.GetInstance()
    if LCM.FindComponent(clientName):
        print('mtsCreateClientInterface: component ' + clientName + ' already exists')
        client = LCM.GetComponent(clientName)
    else:
        print('Creating ' + clientName)
        client = LCM.CreateComponentDynamically('mtsComponent', clientName, '')
        if not client:
            print('Failed to create ' + clientName)
            return None
        LCM.AddComponent(client)
    interface = None
    if serverName and interfaceName:
        print('Connecting ' + clientName + ' to ' + serverName + ' (' + interfaceName + ')')
        interface = client.AddInterfaceRequiredAndConnect((serverName, interfaceName))
    interfacePython = None
    if interface:
        interfacePython = mtsInterfaceRequiredPython()
        for name in interface.GetNamesOfFunctions():
            interfacePython._add_entry(name, getattr(interface, name))
    return interfacePython
