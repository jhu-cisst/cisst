/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2008-01-17

  (C) Copyright 2008-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "mtsGenericObjectProxy.h"
%module cisstMultiTaskPython


%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_pair.i"
%include "std_streambuf.i"
%include "std_iostream.i"

%include "swigrun.i"

%import "cisstConfig.h"

%{
// Rely on cisstCommon.i for Numpy initialization
#define NO_IMPORT_ARRAY
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>
%}

%import "cisstCommon/cisstCommon.i"
%import "cisstOSAbstraction/cisstOSAbstraction.i"
%import "cisstMultiTask/mtsConfig.h"

%import "cisstCommon/cmnDataFunctions.h"
%import "cisstCommon/cmnDataFunctionsJSON.h"

%header %{
#include <cisstMultiTask/mtsPython.h>
%}

// use class type to create the correct Python type
%apply cmnGenericObject * {mtsGenericObject *};

%template(mtsStringVector) std::vector<std::string>;
%template(mtsDoubleVector) std::vector<double>;

// Generate parameter documentation for IRE
%feature("autodoc", "1");

%ignore *::AddCommandVoid;
%ignore *::AddEventVoid;
%ignore mtsComponent::AddRequiredInterface;
%ignore mtsComponent::AddProvidedInterface;
%ignore mtsManagerLocal::AddTask;
%ignore mtsManagerLocal::GetTask;
%ignore mtsManagerLocal::AddDevice;
%ignore mtsManagerLocal::GetDevice;

#define CISST_EXPORT
#define CISST_DEPRECATED

// enums defined in mtsForwardDeclarations
%include "cisstMultiTask/mtsForwardDeclarations.h"

%include "cisstMultiTask/mtsExecutionResult.h"

// Wrap base class
%include "cisstMultiTask/mtsGenericObject.h"

// Wrap commands
%include "cisstMultiTask/mtsCommandBase.h"
%include "cisstMultiTask/mtsCommandVoid.h"
%include "cisstMultiTask/mtsCommandVoidReturn.h"
%include "cisstMultiTask/mtsCommandRead.h"
%include "cisstMultiTask/mtsCommandWriteBase.h"
%include "cisstMultiTask/mtsCommandWriteReturn.h"
%include "cisstMultiTask/mtsCommandQualifiedRead.h"
%include "cisstMultiTask/mtsCommandVoidReturn.h"

// Wrap event receivers
%include "cisstMultiTask/mtsEventReceiver.h"

// Extend mtsCommandVoid
%extend mtsCommandVoid {
    %pythoncode %{
        def __call__(self):
            return self.Execute(MTS_NOT_BLOCKING).GetResult()
    %}
}

// Extend mtsCommandVoidReturn
%extend mtsCommandVoidReturn {
    %pythoncode %{
        def UpdateFromC(self):
            try:
                tmpObject = self.GetResultPrototype().Services().Create()
                self.ArgumentType = tmpObject.__class__
            except Exception as e:
                print('VoidReturn command ' + self.GetName() + ': ' + str(e))


        def __call__(self):
            # PK: figure out if the first statement (after try) is still needed
            try:
                argument = self.ArgumentType(self.GetResultPrototype())
            except Exception:
                argument = self.GetResultPrototype()
            # Probably should check return value below
            self.Execute(argument)
            # If argument has a GetDataCopy method, we assume it is derived from
            # mtsGenericObjectProxy (%extend is used to add this method).
            if hasattr(argument,"GetDataCopy"):
                return argument.GetDataCopy()
            else:
                return argument
    %}
}

// Extend mtsCommandWrite
%extend mtsCommandWriteBase {
    %pythoncode %{
        def UpdateFromC(self):
            try:
                tmpObject = self.GetArgumentClassServices().Create()
                self.ArgumentType = tmpObject.__class__
            except Exception as e:
                print('Write command ' + self.GetName() + ': ' + str(e))

        def __call__(self, argument):
            if isinstance(argument, self.ArgumentType):
                return self.Execute(argument, MTS_NOT_BLOCKING).GetResult()
            else:
                realArgument = self.ArgumentType(argument)
                return self.Execute(realArgument, MTS_NOT_BLOCKING).GetResult()
    %}
}

// Extend mtsCommandWriteReturn
%extend mtsCommandWriteReturn {
    %pythoncode %{
        def UpdateFromC(self):
            try:
                tmp1Object = self.GetArgumentPrototype().Services().Create()
                self.ArgumentType = tmp1Object.__class__
                tmp2Object = self.GetResultPrototype().Services().Create()
                self.ResultType = tmp2Object.__class__
            except Exception as e:
                print('WriteReturn command ' + self.GetName() + ': ' + str(e))

        def __call__(self, argument):
            result = self.ResultType(self.GetResultPrototype())
            # Probably should check return value of self.Execute
            if isinstance(argument, self.ArgumentType):
                self.Execute(argument, result)
            else:
                realArgument = self.ArgumentType(argument1)
                self.Execute(realArgument, result)
            # If result has a GetDataCopy method, we assume it is derived from
            # mtsGenericObjectProxy (%extend is used to add this method).
            if hasattr(result,"GetDataCopy"):
                return result.GetDataCopy()
            else:
                return result
    %}
}

// Extend mtsCommandRead
%extend mtsCommandRead {
    %pythoncode %{
        def UpdateFromC(self):
            try:
                tmpObject = self.GetArgumentPrototype().Services().Create()
                self.ArgumentType = tmpObject.__class__
            except Exception as e:
                print('Read command ' + self.GetName() + ': ' + str(e))
            except AttributeError as e:
                print('Read command ' + self.GetName() + ': ' + str(e))


        def __call__(self):
            # PK: figure out if the first statement (after try) is still needed
            try:
                argument = self.ArgumentType(self.GetArgumentPrototype())
            except Exception:
                argument = self.GetArgumentPrototype()
            # Probably should check return value below
            self.Execute(argument)
            # If argument has a GetDataCopy method, we assume it is derived from
            # mtsGenericObjectProxy (%extend is used to add this method).
            if hasattr(argument,"GetDataCopy"):
                return argument.GetDataCopy()
            else:
                return argument
    %}
}

// Extend mtsCommandQualifiedRead
%extend mtsCommandQualifiedRead {
    %pythoncode %{
        def UpdateFromC(self):
            try:
                tmp1Object = self.GetArgument1Prototype().Services().Create()
                self.Argument1Type = tmp1Object.__class__
                tmp2Object = self.GetArgument2Prototype().Services().Create()
                self.Argument2Type = tmp2Object.__class__
            except Exception as e:
                print('Qualified read command ' + self.GetName() + ': ' + str(e))

        def __call__(self, argument1):
            argument2 = self.Argument2Type(self.GetArgument2Prototype())
            # Probably should check return value of self.Execute
            if isinstance(argument1, self.Argument1Type):
                self.Execute(argument1, argument2)
            else:
                realArgument1 = self.Argument1Type(argument1)
                self.Execute(realArgument1, argument2)
            # If argument2 has a GetDataCopy method, we assume it is derived from
            # mtsGenericObjectProxy (%extend is used to add this method).
            if hasattr(argument2,"GetDataCopy"):
                return argument2.GetDataCopy()
            else:
                return argument2
    %}
}

// Wrap functions
%include "cisstMultiTask/mtsFunctionBase.h"
%include "cisstMultiTask/mtsFunctionVoid.h"
%include "cisstMultiTask/mtsFunctionVoidReturn.h"
%include "cisstMultiTask/mtsFunctionRead.h"
%include "cisstMultiTask/mtsFunctionWrite.h"
%include "cisstMultiTask/mtsFunctionWriteReturn.h"
%include "cisstMultiTask/mtsFunctionQualifiedRead.h"

// Extend mtsFunctionVoid
%extend mtsFunctionVoid {
    %pythoncode %{
        def __call__(self):
            return self.Execute().GetResult()
    %}
}

// Extend mtsFunctionVoidReturn
%extend mtsFunctionVoidReturn {
    %pythoncode %{
        def UpdateFromC(self):
            if self.IsValid():
                try:
                    tmpObject = self.GetResultPrototype().Services().Create()
                    self.ArgumentType = tmpObject.__class__
                except Exception as e:
                    print('VoidReturn function ' + self.GetCommand().GetName() + ': ' + str(e))
            else:
                print('VoidReturn function not valid')


        def __call__(self):
            # PK: figure out if the first statement (after try) is still needed
            try:
                argument = self.ArgumentType(self.GetResultPrototype())
            except Exception:
                argument = self.GetResultPrototype()
            # Probably should check return value below
            self.ExecuteGeneric(argument)
            # If argument has a GetDataCopy method, we assume it is derived from
            # mtsGenericObjectProxy (%extend is used to add this method).
            if hasattr(argument,"GetDataCopy"):
                return argument.GetDataCopy()
            else:
                return argument
    %}
}

// Extend mtsFunctionRead
%extend mtsFunctionRead {
    %pythoncode %{
        def UpdateFromC(self):
            if self.IsValid():
                try:
                    tmpObject = self.GetArgumentPrototype().Services().Create()
                    self.ArgumentType = tmpObject.__class__
                except Exception as e:
                    print('Read function ' + self.GetCommand().GetName() + ': ' + str(e))
            else:
                print('Read function not valid')


        def __call__(self):
            # PK: figure out if the first statement (after try) is still needed
            try:
                argument = self.ArgumentType(self.GetArgumentPrototype())
            except Exception:
                argument = self.GetArgumentPrototype()
            # Probably should check return value below
            self.ExecuteGeneric(argument)
            # If argument has a GetDataCopy method, we assume it is derived from
            # mtsGenericObjectProxy (%extend is used to add this method).
            if hasattr(argument,"GetDataCopy"):
                return argument.GetDataCopy()
            else:
                return argument
    %}
}

// Extend mtsFunctionWrite
%extend mtsFunctionWrite {
    %pythoncode %{
        def UpdateFromC(self):
            if self.IsValid():
                try:
                    tmpObject = self.GetArgumentPrototype().Services().Create()
                    self.ArgumentType = tmpObject.__class__
                except Exception as e:
                    print('Write function ' + self.GetCommand().GetName() + ': ' + str(e))
            else:
                print('Write function not valid')

        def __call__(self, argument):
            if isinstance(argument, self.ArgumentType):
                return self.ExecuteGeneric(argument).GetResult()
            else:
                realArgument = self.ArgumentType(argument)
                return self.ExecuteGeneric(realArgument).GetResult()
    %}
}

// Extend mtsFunctionWriteReturn
%extend mtsFunctionWriteReturn {
    %pythoncode %{
        def UpdateFromC(self):
            if self.IsValid():
                try:
                    tmp1Object = self.GetArgumentPrototype().Services().Create()
                    self.ArgumentType = tmp1Object.__class__
                    tmp2Object = self.GetResultPrototype().Services().Create()
                    self.ResultType = tmp2Object.__class__
                except Exception as e:
                    print('WriteReturn function ' + self.GetCommand().GetName() + ': ' + str(e))
            else:
                print('WriteReturn function not valid')

        def __call__(self, argument):
            result = self.ResultType(self.GetResultPrototype())
            # Probably should check return value of self.Execute
            if isinstance(argument, self.ArgumentType):
                self.ExecuteGeneric(argument, result)
            else:
                realArgument = self.ArgumentType(argument1)
                self.ExecuteGeneric(realArgument, result)
            # If result has a GetDataCopy method, we assume it is derived from
            # mtsGenericObjectProxy (%extend is used to add this method).
            if hasattr(result,"GetDataCopy"):
                return result.GetDataCopy()
            else:
                return result
    %}
}

// Extend mtsFunctionQualifiedRead
%extend mtsFunctionQualifiedRead {
    %pythoncode %{
        def UpdateFromC(self):
            if self.IsValid():
                try:
                    tmp1Object = self.GetArgument1Prototype().Services().Create()
                    self.Argument1Type = tmp1Object.__class__
                    tmp2Object = self.GetArgument2Prototype().Services().Create()
                    self.Argument2Type = tmp2Object.__class__
                except Exception as e:
                    print('Qualified read function ' + self.GetCommand().GetName() + ': ' + str(e))
            else:
                print('QualifiedRead function not valid')

        def __call__(self, argument1):
            argument2 = self.Argument2Type(self.GetArgument2Prototype())
            # Probably should check return value of self.Execute
            if isinstance(argument1, self.Argument1Type):
                self.ExecuteGeneric(argument1, argument2)
            else:
                realArgument1 = self.Argument1Type(argument1)
                self.ExecuteGeneric(realArgument1, argument2)
            # If argument2 has a GetDataCopy method, we assume it is derived from
            # mtsGenericObjectProxy (%extend is used to add this method).
            if hasattr(argument2,"GetDataCopy"):
                return argument2.GetDataCopy()
            else:
                return argument2
    %}
}


// Wrap tasks and components
%include "cisstMultiTask/mtsComponent.h"

%template(mtsComponentConstructorNameAndInt) mtsComponentConstructorNameAndArg<int>;
%template(mtsComponentConstructorNameAndUInt) mtsComponentConstructorNameAndArg<unsigned int>;
%template(mtsComponentConstructorNameAndLong) mtsComponentConstructorNameAndArg<long>;
%template(mtsComponentConstructorNameAndULong) mtsComponentConstructorNameAndArg<unsigned long>;
%template(mtsComponentConstructorNameAndDouble) mtsComponentConstructorNameAndArg<double>;
%template(mtsComponentConstructorNameAndString) mtsComponentConstructorNameAndArg<std::string>;

%extend mtsComponent {
    %pythoncode %{
        def UpdateFromC(self):
            interfaces = mtsComponent.GetNamesOfInterfacesProvided(self)
            for interface in interfaces:
                interfaceNoSpace = interface.replace(' ', '')
                interfaceFrontEnd = mtsComponent.GetInterfaceProvided(self, interface)
                self.__dict__[interfaceNoSpace] = mtsInterfaceProvided.GetEndUserInterface(interfaceFrontEnd, 'Python')
                self.__dict__[interfaceNoSpace].UpdateFromC()
            interfaces = mtsComponent.GetNamesOfInterfacesRequired(self)
            for interface in interfaces:
                interfaceNoSpace = interface.replace(' ', '')
                self.__dict__[interfaceNoSpace] = mtsComponent.GetInterfaceRequired(self, interface)
                # Only call UpdateFromC if required interface is connected to a provided interface
                if self.__dict__[interfaceNoSpace].GetConnectedInterface():
                    self.__dict__[interfaceNoSpace].UpdateFromC()

        def AddInterfaceRequiredFromProvided(self, interfaceProvided, interfaceRequiredName = ''):
            if not isinstance(interfaceProvided, mtsInterfaceProvidedDescription):
                print('Parameter must be of type mtsInterfaceProvidedDescription')
                return
            if not interfaceRequiredName:
                interfaceProvidedNoSpace = interfaceProvided.InterfaceName.replace(' ', '')
                interfaceRequiredName = 'RequiredFor' + interfaceProvidedNoSpace
            else:
                interfaceRequiredName = interfaceRequiredName.replace(' ', '')
            interfaceRequired = self.AddInterfaceRequired(interfaceRequiredName, MTS_OPTIONAL)
            if not interfaceRequired:
                return
            self.__dict__[interfaceRequired.GetName()] = interfaceRequired
            for command in interfaceProvided.CommandsVoid:
                func = mtsFunctionVoid()
                interfaceRequired.AddFunction(command.Name, func)
                func.thisown = 0
            for command in interfaceProvided.CommandsVoidReturn:
                func = mtsFunctionVoidReturn()
                interfaceRequired.AddFunction(command.Name, func)
                func.thisown = 0
            for command in interfaceProvided.CommandsWrite:
                func = mtsFunctionWrite()
                interfaceRequired.AddFunction(command.Name, func)
                func.thisown = 0
            for command in interfaceProvided.CommandsWriteReturn:
                func = mtsFunctionWriteReturn()
                interfaceRequired.AddFunction(command.Name, func)
                func.thisown = 0
            for command in interfaceProvided.CommandsQualifiedRead:
                func = mtsFunctionQualifiedRead()
                interfaceRequired.AddFunction(command.Name, func)
                func.thisown = 0
            for command in interfaceProvided.CommandsRead:
                func = mtsFunctionRead()
                interfaceRequired.AddFunction(command.Name, func)
                func.thisown = 0
            return interfaceRequired

        # otherComponentInterface should be a tuple ('process', 'component', 'interfaceProvided')
        # or ('component', 'interfaceProvided')
        def AddInterfaceRequiredAndConnect(self, otherComponentInterface):
            try:
                LCM = mtsManagerLocal.GetInstance()
                localProcessName = LCM.GetProcessName()
                num = len(otherComponentInterface)
                if 2 <= num <= 3:
                    interfaceName = otherComponentInterface[num-1]
                    componentName = otherComponentInterface[num-2]
                    if num == 3:
                        processName = otherComponentInterface[num-3]
                    else:
                        processName = localProcessName
                    interfaceDescription = LCM.GetInterfaceProvidedDescription(processName, componentName, interfaceName)
                    if not interfaceDescription.InterfaceName:
                        print('No provided interface (empty string)')
                        return None
                    interfaceRequiredName = 'RequiredFor' + componentName + '_' + interfaceName;
                    interfaceRequired = self.AddInterfaceRequiredFromProvided(interfaceDescription, interfaceRequiredName)
                    if (processName != localProcessName):
                        LCM.Connect(localProcessName, self.GetName(), interfaceRequired.GetName(),
                                    processName, componentName, interfaceName)
                    else:
                        LCM.Connect(self.GetName(), interfaceRequired.GetName(), componentName, interfaceName)
                    interfaceRequired.UpdateFromC()
                    if interfaceRequired.GetConnectedInterface():
                        print('Required interface ' + interfaceRequired.GetName() + ' connected.')
                    else:
                        print('Required interface ' + interfaceRequired.GetName() + ' not connected.')
                    return interfaceRequired
                else:
                    print('Parameter error: must specify (process, component, interface) or (component, interface)')
            except TypeError as e:
                print('Parameter error: must specify (process, component, interface) or (component, interface), caught exception: ' + str(e))
    %}
}

%include "cisstMultiTask/mtsInterface.h"
%include "cisstMultiTask/mtsInterfaceProvided.h"
%extend mtsInterfaceProvided {
    %pythoncode %{
        def UpdateFromC(self):
            commands = mtsInterfaceProvided.GetNamesOfCommandsVoid(self)
            for command in commands:
                self.__dict__[command] = mtsInterfaceProvided.GetCommandVoid(self, command)
            commands = mtsInterfaceProvided.GetNamesOfCommandsVoidReturn(self)
            for command in commands:
                self.__dict__[command] = mtsInterfaceProvided.GetCommandVoidReturn(self, command)
                self.__dict__[command].UpdateFromC()
            commands = mtsInterfaceProvided.GetNamesOfCommandsWrite(self)
            for command in commands:
                self.__dict__[command] = mtsInterfaceProvided.GetCommandWrite(self, command)
                self.__dict__[command].UpdateFromC()
            commands = mtsInterfaceProvided.GetNamesOfCommandsWriteReturn(self)
            for command in commands:
                self.__dict__[command] = mtsInterfaceProvided.GetCommandWriteReturn(self, command)
                self.__dict__[command].UpdateFromC()
            commands = mtsInterfaceProvided.GetNamesOfCommandsQualifiedRead(self)
            for command in commands:
                self.__dict__[command] = mtsInterfaceProvided.GetCommandQualifiedRead(self, command)
                self.__dict__[command].UpdateFromC()
            commands = mtsInterfaceProvided.GetNamesOfCommandsRead(self)
            for command in commands:
                self.__dict__[command] = mtsInterfaceProvided.GetCommandRead(self, command)
                self.__dict__[command].UpdateFromC()
    %}
}

%include "cisstMultiTask/mtsInterfaceRequired.h"
%extend mtsInterfaceRequired {
    %pythoncode %{
        def UpdateFromC(self):
            if not self.GetConnectedInterface():
                print('Required interface ' + self.GetName() + ' not yet connected.')
                return
            functions = mtsInterfaceRequired.GetNamesOfFunctionsVoid(self)
            for function in functions:
                self.__dict__[function] = mtsInterfaceRequired.GetFunctionVoid(self, function)
            functions = mtsInterfaceRequired.GetNamesOfFunctionsVoidReturn(self)
            for function in functions:
                self.__dict__[function] = mtsInterfaceRequired.GetFunctionVoidReturn(self, function)
                self.__dict__[function].UpdateFromC()
            functions = mtsInterfaceRequired.GetNamesOfFunctionsWrite(self)
            for function in functions:
                self.__dict__[function] = mtsInterfaceRequired.GetFunctionWrite(self, function)
                self.__dict__[function].UpdateFromC()
            functions = mtsInterfaceRequired.GetNamesOfFunctionsWriteReturn(self)
            for function in functions:
                self.__dict__[function] = mtsInterfaceRequired.GetFunctionWriteReturn(self, function)
                self.__dict__[function].UpdateFromC()
            functions = mtsInterfaceRequired.GetNamesOfFunctionsQualifiedRead(self)
            for function in functions:
                self.__dict__[function] = mtsInterfaceRequired.GetFunctionQualifiedRead(self, function)
                self.__dict__[function].UpdateFromC()
            functions = mtsInterfaceRequired.GetNamesOfFunctionsRead(self)
            for function in functions:
                self.__dict__[function] = mtsInterfaceRequired.GetFunctionRead(self, function)
                self.__dict__[function].UpdateFromC()
    %}
}

// Wrap manager component services (includes internal required interface)

// Typemaps below are for GetNamesOfInterfaces, so that the two vectors of strings
// (required and provided interfaces) can be returned as a list of lists of strings,
// i.e., ((req1, req2, ...),(prov1, prov2, ...))


%typemap(in, numinputs=0) std::vector<std::string> & namesOfInterfacesRequired (std::vector<std::string> temp) {
    $1 = &temp;
}

%typemap(in, numinputs=0) std::vector<std::string> & namesOfInterfacesProvided (std::vector<std::string> temp) {
    $1 = &temp;
}

%typemap(argout) std::vector<std::string> & namesOfInterfacesRequired {
  Py_DECREF($result);  // Garbage collect the returned "bool"
  if (!result) {
      PyErr_SetString(PyExc_RuntimeError, "Failed to get interfaces");
      return NULL;
  }
  $result = PyTuple_New(2);
  PyObject *required = PyTuple_New((*$1).size());
  PyTuple_SetItem($result,0,required);

  size_t i;
  for (i = 0; i < (*$1).size(); i++)
      PyTuple_SetItem(required,i,PyUnicode_FromString((*$1)[i].c_str()));
}

%typemap(argout) std::vector<std::string> & namesOfInterfacesProvided {
  PyObject *provided = PyTuple_New((*$1).size());
  PyTuple_SetItem($result,1,provided);

  size_t i;
  for (i = 0; i < (*$1).size(); i++)
      PyTuple_SetItem(provided,i,PyUnicode_FromString((*$1)[i].c_str()));
}

%include "cisstMultiTask/mtsManagerComponentServices.h"

%include "cisstMultiTask/mtsTask.h"
%include "cisstMultiTask/mtsTaskContinuous.h"
%include "cisstMultiTask/mtsTaskPeriodic.h"
%include "cisstMultiTask/mtsTaskFromSignal.h"

%include "cisstMultiTask/mtsManagerLocal.h"
%extend mtsManagerLocal {
    %pythoncode %{
        def UpdateFromC(self):
            comps = mtsManagerLocal.GetNamesOfComponents(self)
            for comp in comps:
                self.__dict__[comp] = mtsManagerLocal.GetComponent(self, comp)
                self.__dict__[comp].UpdateFromC()
    %}
}

%include "cisstMultiTask/mtsCollectorBase.h"
%include "cisstMultiTask/mtsCollectorState.h"

// Wrap some basic types
%include "cisstMultiTask/mtsGenericObjectProxy.h"
%define MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(name, elementType)
    // ignore the operator &
    %ignore mtsGenericObjectProxy<elementType>::operator value_type&;
    %ignore mtsGenericObjectProxy<elementType>::operator const value_type&;
    %ignore mtsGenericObjectProxyIntermediate<elementType>::operator value_type&;
    %ignore mtsGenericObjectProxyIntermediate<elementType>::operator const value_type&;
    // Instantiate the template
    %template(name ## Base) mtsGenericObjectProxyBase<elementType>;
    %template(name ## Intermediate) mtsGenericObjectProxyIntermediate<elementType>;
    %template(name) mtsGenericObjectProxy<elementType>;
    // Type addition for dynamic type checking
    %{
        typedef mtsGenericObjectProxy<elementType> name;
    %}
    typedef mtsGenericObjectProxy<elementType> name;
    %types(name *);
    %extend mtsGenericObjectProxy<elementType> {
        elementType GetDataCopy() const
        { return static_cast<elementType>($self->GetData()); }
    }
%enddef

MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsDouble, double);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsFloat, float);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsInt, int);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsUInt, unsigned int);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsShort, short);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsUShort, unsigned short);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsChar, char);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsUChar, unsigned char);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsLong, long);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsULong, unsigned long);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsBool, bool);
#ifdef SWIG_WINDOWS
%apply long long {size_t};
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsSizeT, size_t);
#endif
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsStdString, std::string);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsStdStringVecProxy, stdStringVec);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsStdDoubleVecProxy, stdDoubleVec);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsStdCharVecProxy, stdCharVec);

%include "cisstMultiTask/mtsParameterTypes.h"
%template(mtsDescriptionConnectionVec) std::vector<mtsDescriptionConnection>;
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsDescriptionConnectionVecProxy, mtsDescriptionConnectionVec);
%template(mtsDescriptionComponentClassVec) std::vector<mtsDescriptionComponentClass>;
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsDescriptionComponentClassVecProxy, mtsDescriptionComponentClassVec);

%include "cisstMultiTask/mtsComponentState.h"
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsComponentStateProxy, mtsComponentState);

%include "cisstMultiTask/mtsInterfaceCommon.h"
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsInterfaceProvidedDescriptionProxy, mtsInterfaceProvidedDescription);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsInterfaceRequiredDescriptionProxy, mtsInterfaceRequiredDescription);

%template(mtsCommandsVoidDescription)          std::vector<mtsCommandVoidDescription>;
%template(mtsCommandsVoidReturnDescription)    std::vector<mtsCommandVoidReturnDescription>;
%template(mtsCommandsWriteDescription)         std::vector<mtsCommandWriteDescription>;
%template(mtsCommandsWriteReturnDescription)   std::vector<mtsCommandWriteReturnDescription>;
%template(mtsCommandsReadDescription)          std::vector<mtsCommandReadDescription>;
%template(mtsCommandsQualifiedReadDescription) std::vector<mtsCommandQualifiedReadDescription>;
%template(mtsEventsVoidDescription)            std::vector<mtsEventVoidDescription>;
%template(mtsEventsWriteDescription)           std::vector<mtsEventWriteDescription>;

%import "cisstMultiTask/mtsMacros.h"

// Wrap mtsStateIndex
%ignore mtsStateIndex::operator--;
%include "cisstMultiTask/mtsStateIndex.h"

 // Wrap mtsIntervalStatistics
%include "cisstMultiTask/mtsIntervalStatistics.h"

%pythoncode %{

# Define separate class so that we have more flexibility to set up namespaces.
# For example, command /one/two/three will become one.two.three.
class mtsInterfaceRequiredPython:
    def __init__(self):
        pass

    def _add_entry(self, cmdName, cmdFunc):
        # Replace dashes with underscore
        cmdNameFixed = cmdName.replace('-', '_')
        # Split namespaces (/)
        items = cmdNameFixed.split('/')
        obj = self
        # Loop through any namespaces
        for i in range(0, len(items)-1):
            if not hasattr(obj, items[i]):
                setattr(obj, items[i], mtsInterfaceRequiredPython())
            obj = getattr(obj, items[i])
        # Last (or only) item is set to the func
        setattr(obj, items[len(items)-1], cmdFunc)

# Dynamically load and create a server component
#
# This function dynamically loads the specified library and constructs an object using the
# specified component class (className). The component name is specified through argSerialized
# (if not empty); otherwise componentName is used. Note that if argSerialized is not empty,
# componentName is ignored.

def mtsLoadAndCreateServer(libraryName, className, componentName, argSerialized):
    print('Loading ' + libraryName)
    import cisstOSAbstractionPython as cisstOSAbstraction
    DynLoader = cisstOSAbstraction.osaDynamicLoader()
    if not DynLoader.Load(libraryName):
        print('Failed to load ' + libraryName + ' (see cisstLog.txt)')
        return None

    print('Creating ' + componentName + ' (' + className + ')')
    LCM = mtsManagerLocal.GetInstance()
    if (not argSerialized):
        comp = LCM.CreateComponentDynamically(className, componentName, '')
    else:
        comp = LCM.CreateComponentDynamically(className, argSerialized)
    if comp:
        print('Component created')
        LCM.AddComponent(comp)
    return comp

# Create client component (if necessary) and then create required interface to server
# provided interface
def mtsCreateClientInterface(clientName, serverName, interfaceName):
    LCM = mtsManagerLocal.GetInstance()
    if (LCM.FindComponent(clientName)):
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
    if (serverName and interfaceName):
        print('Connecting ' + clientName + ' to ' + serverName + ' (' + interfaceName + ')')
        interface = client.AddInterfaceRequiredAndConnect((serverName, interfaceName))
    interfacePython = None
    if interface:
        interfacePython = mtsInterfaceRequiredPython()
        for name in interface.GetNamesOfFunctions():
            interfacePython._add_entry(name, getattr(interface, name))
    return interfacePython

%}
