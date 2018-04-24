/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2008-01-17

  (C) Copyright 2008-2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


%module cisstMultiTaskPython


%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_pair.i"
%include "std_streambuf.i"
%include "std_iostream.i"

%include "swigrun.i"

%import "cisstConfig.h"

%import "cisstCommon/cisstCommon.i"
%import "cisstVector/cisstVector.i"
%import "cisstOSAbstraction/cisstOSAbstraction.i"
%import "cisstMultiTask/mtsConfig.h"

%init %{
    import_array() // numpy initialization
%}

%header %{
#include <cisstMultiTask/mtsPython.h>
%}

// PK TEMP: need time.sleep until blocking commands supported over network
%pythoncode %{
   import time
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
%include "cisstMultiTask/mtsFunctionVoidReturn.h"

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

        def AddInterfaceRequiredFromProvided(self, interfaceProvided):
            if not isinstance(interfaceProvided, mtsInterfaceProvidedDescription):
                print('Parameter must be of type mtsInterfaceProvidedDescription')
                return
            interfaceProvidedNoSpace = interfaceProvided.InterfaceName.replace(' ', '')
            interfaceRequired = self.AddInterfaceRequired('RequiredFor'+interfaceProvidedNoSpace, MTS_OPTIONAL)
            if not interfaceRequired:
                return
            self.__dict__[interfaceRequired.GetName()] = interfaceRequired
            for command in interfaceProvided.CommandsVoid:
                func = mtsFunctionVoid()
                interfaceRequired.AddFunction(command.Name, func)
                func.thisown = 0
            #for command in interfaceProvided.CommandsVoidReturn:
            #    func = mtsFunctionVoidReturn()
            #    interfaceRequired.AddFunction(command.Name, func)
            #    func.thisown = 0
            for command in interfaceProvided.CommandsWrite:
                func = mtsFunctionWrite()
                interfaceRequired.AddFunction(command.Name, func)
                func.thisown = 0
            #for command in interfaceProvided.CommandsWriteReturn:
            #    func = mtsFunctionWriteReturn()
            #    interfaceRequired.AddFunction(command.Name, func)
            #    func.thisown = 0
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
        def AddInterfaceRequiredAndConnect(self, otherComponentInterface, connectionAttempts = 1):
            try:
                localProcessName = mtsManagerLocal_GetInstance().GetProcessName()
                num = len(otherComponentInterface)
                if 2 <= num <= 3:
                    interfaceName = otherComponentInterface[num-1]
                    componentName = otherComponentInterface[num-2]
                    if num == 3:
                        processName = otherComponentInterface[num-3]
                    else:
                        processName = localProcessName
                    # Now do the work here
                    manager = self.GetManagerComponentServices()
                    if not manager:
                        print('Could not get manager component services')
                        return
                    interfaceDescription = manager.GetInterfaceProvidedDescription(processName, componentName, interfaceName)
                    if not interfaceDescription.InterfaceName:
                        print('No provided interface (empty string)')
                        return
                    interfaceRequired = self.AddInterfaceRequiredFromProvided(interfaceDescription)
                    attempt = 0
                    while (attempt < connectionAttempts):
                        attempt = attempt + 1
                        print('Trying to connect: ' + interfaceRequired.GetName() + ' - attempt # ' + str(attempt))
                        manager.Connect(localProcessName, self.GetName(), interfaceRequired.GetName(), processName, componentName, interfaceName)
                        # PK TEMP: need time.sleep until blocking commands supported over network
                        time.sleep(2.0)
                        interfaceRequired.UpdateFromC()
                        if interfaceRequired.GetConnectedInterface():
                            print('Required interface ' + interfaceRequired.GetName() + ' connected.')
                            return interfaceRequired
                    print('Unable to add required interface for ' + interfaceName)
                else:
                    print('Parameter error: must specify (process, component, interface) or (component, interface)')
            except TypeError as e:
                print('Parameter error: must specify (process, component, interface) or (component, interface), caught exception: ' + str(e))
    %}
}

// For IRE, because EnableDynamicComponentManagement is protected (see also mtsPython.h)
class mtsComponentWithManagement : public mtsComponent
{
public:
    mtsComponentWithManagement(const std::string &name);
    ~mtsComponentWithManagement();
};

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
      PyTuple_SetItem(required,i,PyString_FromString((*$1)[i].c_str()));
}

%typemap(argout) std::vector<std::string> & namesOfInterfacesProvided {
  PyObject *provided = PyTuple_New((*$1).size());
  PyTuple_SetItem($result,1,provided);

  size_t i;
  for (i = 0; i < (*$1).size(); i++)
      PyTuple_SetItem(provided,i,PyString_FromString((*$1)[i].c_str()));
}

%include "cisstMultiTask/mtsManagerComponentServices.h"

%include "cisstMultiTask/mtsTask.h"
%include "cisstMultiTask/mtsTaskContinuous.h"
%include "cisstMultiTask/mtsTaskPeriodic.h"
%include "cisstMultiTask/mtsTaskFromSignal.h"

%include "cisstMultiTask/mtsManagerLocalInterface.h"
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
    // Instantiate the template
    %template(name ## Base) mtsGenericObjectProxyBase<elementType>;
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
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsLong, long);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsULong, unsigned long);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsBool, bool);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsStdString, std::string);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsStdStringVecProxy, stdStringVec);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsStdDoubleVecProxy, stdDoubleVec);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsStdCharVecProxy, stdCharVec);

MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVct1, vct1);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVct2, vct2);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVct3, vct3);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVct4, vct4);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVct5, vct5);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVct6, vct6);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVct7, vct7);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVct8, vct8);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVct9, vct9);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctFloat1, vctFloat1);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctFloat2, vctFloat2);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctFloat3, vctFloat3);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctFloat4, vctFloat4);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctFloat5, vctFloat5);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctFloat6, vctFloat6);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctFloat7, vctFloat7);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctFloat8, vctFloat8);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctFloat9, vctFloat9);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctLong1, vctLong1);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctLong2, vctLong2);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctLong3, vctLong3);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctLong4, vctLong4);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctLong5, vctLong5);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctLong6, vctLong6);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctLong7, vctLong7);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctLong8, vctLong8);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctLong9, vctLong9);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctULong1, vctULong1);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctULong2, vctULong2);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctULong3, vctULong3);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctULong4, vctULong4);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctULong5, vctULong5);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctULong6, vctULong6);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctULong7, vctULong7);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctULong8, vctULong8);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctULong9, vctULong9);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctInt1, vctInt1);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctInt2, vctInt2);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctInt3, vctInt3);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctInt4, vctInt4);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctInt5, vctInt5);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctInt6, vctInt6);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctInt7, vctInt7);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctInt8, vctInt8);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctInt9, vctInt9);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUInt1, vctUInt1);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUInt2, vctUInt2);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUInt3, vctUInt3);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUInt4, vctUInt4);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUInt5, vctUInt5);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUInt6, vctUInt6);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUInt7, vctUInt7);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUInt8, vctUInt8);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUInt9, vctUInt9);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctShort1, vctShort1);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctShort2, vctShort2);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctShort3, vctShort3);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctShort4, vctShort4);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctShort5, vctShort5);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctShort6, vctShort6);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctShort7, vctShort7);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctShort8, vctShort8);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctShort9, vctShort9);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUShort1, vctUShort1);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUShort2, vctUShort2);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUShort3, vctUShort3);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUShort4, vctUShort4);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUShort5, vctUShort5);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUShort6, vctUShort6);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUShort7, vctUShort7);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUShort8, vctUShort8);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUShort9, vctUShort9);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctChar1, vctChar1);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctChar2, vctChar2);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctChar3, vctChar3);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctChar4, vctChar4);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctChar5, vctChar5);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctChar6, vctChar6);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctChar7, vctChar7);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctChar8, vctChar8);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctChar9, vctChar9);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUChar1, vctUChar1);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUChar2, vctUChar2);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUChar3, vctUChar3);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUChar4, vctUChar4);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUChar5, vctUChar5);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUChar6, vctUChar6);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUChar7, vctUChar7);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUChar8, vctUChar8);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUChar9, vctUChar9);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctBool1, vctBool1);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctBool2, vctBool2);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctBool3, vctBool3);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctBool4, vctBool4);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctBool5, vctBool5);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctBool6, vctBool6);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctBool7, vctBool7);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctBool8, vctBool8);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctBool9, vctBool9);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVct2x2, vct2x2);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVct3x3, vct3x3);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVct4x4, vct4x4);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctMatRot3, vctMatRot3);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctFrm3, vctFrm3);

MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctDoubleVec, vctDoubleVec);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctFloatVec, vctFloatVec);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctIntVec, vctIntVec);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUIntVec, vctUIntVec);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctCharVec, vctCharVec);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUCharVec, vctUCharVec);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctBoolVec, vctBoolVec);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctShortVec, vctShortVec);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctUShortVec, vctUShortVec);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctLongVec, vctLongVec);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctULongVec, vctULongVec);

MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctDoubleMat, vctDoubleMat);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctFloatMat, vctFloatMat);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsVctIntMat, vctIntMat);

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
%template(mtsCommandsWriteDescription)         std::vector<mtsCommandWriteDescription>;
%template(mtsCommandsReadDescription)          std::vector<mtsCommandReadDescription>;
%template(mtsCommandsQualifiedReadDescription) std::vector<mtsCommandQualifiedReadDescription>;
%template(mtsEventsVoidDescription)            std::vector<mtsEventVoidDescription>;
%template(mtsEventsWriteDescription)           std::vector<mtsEventWriteDescription>;

// Wrap mtsVector
%import "cisstMultiTask/mtsVector.h"

// define macro
%define MTS_INSTANTIATE_VECTOR(name, elementType)
    // suppress warning about base class not known
    %warnfilter(401) mtsVector<elementType>;
    // force instantiation and define name for SWIG
    %template(name) mtsVector<elementType>;
    %{
         typedef mtsVector<elementType> name;
    %}
    typedef mtsVector<elementType> name;
    %types(name *);
    // add two methods to retrieve the vector
    %extend mtsVector<elementType>{
        inline VectorType & Data(void) {
            return *self;
        }
        inline const VectorType & Data(void) const {
            return *self;
        }
   }
%enddef

// instantiate for types also instantiated in cisstVector wrappers
MTS_INSTANTIATE_VECTOR(mtsDoubleVec, double);
MTS_INSTANTIATE_VECTOR(mtsFloatVec, float);
MTS_INSTANTIATE_VECTOR(mtsLongVec, long);
MTS_INSTANTIATE_VECTOR(mtsULongVec, unsigned long);
MTS_INSTANTIATE_VECTOR(mtsIntVec, int);
MTS_INSTANTIATE_VECTOR(mtsUIntVec, unsigned int);
MTS_INSTANTIATE_VECTOR(mtsShortVec, short);
MTS_INSTANTIATE_VECTOR(mtsUShortVec, unsigned short);
MTS_INSTANTIATE_VECTOR(mtsCharVec, char);
MTS_INSTANTIATE_VECTOR(mtsUCharVec, unsigned char);
MTS_INSTANTIATE_VECTOR(mtsBoolVec, bool);
MTS_INSTANTIATE_VECTOR(mtsStdStringVec, std::string);

// Wrap mtsMatrix
%import "cisstMultiTask/mtsMatrix.h"

// define macro
%define MTS_INSTANTIATE_MATRIX(name, elementType)
    // suppress warning about base class not known
    %warnfilter(401) mtsMatrix<elementType>;
    // force instantiation and define name for SWIG
    %template(name) mtsMatrix<elementType>;
    %{
        typedef mtsMatrix<elementType> name;
    %}
    typedef mtsMatrix<elementType> name;
    %types(name *);
    // add two methods to retrieve the matrix
    %extend mtsMatrix<elementType>{
        inline MatrixType & Data(void) {
            return *self;
        }
        inline const MatrixType & Data(void) const {
            return *self;
        }
    }
%enddef

// instantiate for types also instantiated in cisstVector wrappers
MTS_INSTANTIATE_MATRIX(mtsDoubleMat, double);
MTS_INSTANTIATE_MATRIX(mtsIntMat, int);
MTS_INSTANTIATE_MATRIX(mtsShortMat, short);
MTS_INSTANTIATE_MATRIX(mtsLongMat, long);

%import "cisstMultiTask/mtsMacros.h"
%include "cisstMultiTask/mtsTransformationTypes.h"

// Wrap mtsStateIndex
%include "cisstMultiTask/mtsStateIndex.h"

 // Wrap mtsIntervalStatistics
%include "cisstMultiTask/mtsIntervalStatistics.h"
