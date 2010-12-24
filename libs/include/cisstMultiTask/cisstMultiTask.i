/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Anton Deguet
  Created on:   2008-01-17

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

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

// use class type to create the correct Python type
%apply cmnGenericObject * {mtsGenericObject *};

%template(mtsStringVector) std::vector<std::string>;

// Generate parameter documentation for IRE
%feature("autodoc", "1");

%ignore *::AddCommandVoid;
%ignore *::AddEventVoid;

#define CISST_EXPORT
#define CISST_DEPRECATED

// enums defined in mtsForwardDeclarations
%include "cisstMultiTask/mtsForwardDeclarations.h"

%include "cisstMultiTask/mtsExecutionResult.h"

// Wrap commands
%include "cisstMultiTask/mtsCommandBase.h"
%include "cisstMultiTask/mtsCommandVoid.h"
%include "cisstMultiTask/mtsCommandVoidReturn.h"
%include "cisstMultiTask/mtsCommandRead.h"
%include "cisstMultiTask/mtsCommandWriteBase.h"
%include "cisstMultiTask/mtsCommandWriteReturn.h"
%include "cisstMultiTask/mtsCommandQualifiedRead.h"
// Wrap event receivers
%include "cisstMultiTask/mtsEventReceiver.h"

// Extend mtsCommandVoid
%extend mtsCommandVoid {
    %pythoncode {
        def __call__(self):
            return self.Execute(MTS_NOT_BLOCKING).GetResult()
    }
}

// Extend mtsCommandVoidReturn
%extend mtsCommandVoidReturn {
    %pythoncode {
        def UpdateFromC(self):
            try:
                tmpObject = self.GetResultPrototype().Services().Create()
                self.ArgumentType = tmpObject.__class__
            except Exception, e:
                print 'VoidReturn command ', self.GetName(), ': ', e


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
    }
}

// Extend mtsCommandWrite
%extend mtsCommandWriteBase {
    %pythoncode {
        def UpdateFromC(self):
            try:
                tmpObject = self.GetArgumentClassServices().Create()
                self.ArgumentType = tmpObject.__class__
            except Exception, e:
                print 'Write command ', self.GetName(), ': ', e

        def __call__(self, argument):
            if isinstance(argument, self.ArgumentType):
                return self.Execute(argument, MTS_NOT_BLOCKING).GetResult()
            else:
                realArgument = self.ArgumentType(argument)
                return self.Execute(realArgument, MTS_NOT_BLOCKING).GetResult()
    }
}

// Extend mtsCommandWriteReturn
%extend mtsCommandWriteReturn {
    %pythoncode {
        def UpdateFromC(self):
            try:
                tmp1Object = self.GetArgumentPrototype().Services().Create()
                self.ArgumentType = tmp1Object.__class__
                tmp2Object = self.GetResultPrototype().Services().Create()
                self.ResultType = tmp2Object.__class__
            except Exception, e:
                print 'WriteReturn command ', self.GetName(), ': ', e

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
    }
}

// Extend mtsCommandRead
%extend mtsCommandRead {
    %pythoncode {
        def UpdateFromC(self):
            try:
                tmpObject = self.GetArgumentPrototype().Services().Create()
                self.ArgumentType = tmpObject.__class__
            except Exception, e:
                print 'Read command ', self.GetName(), ': ', e
            except AttributeError, e:
                print 'Read command ', self.GetName(), ': ', e


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
    }
}

// Extend mtsCommandQualifiedRead
%extend mtsCommandQualifiedRead {
    %pythoncode {
        def UpdateFromC(self):
            try:
                tmp1Object = self.GetArgument1Prototype().Services().Create()
                self.Argument1Type = tmp1Object.__class__
                tmp2Object = self.GetArgument2Prototype().Services().Create()
                self.Argument2Type = tmp2Object.__class__
            except Exception, e:
                print 'Qualified read command ', self.GetName(), ': ', e

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
    }
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
    %pythoncode {
        def __call__(self):
            return self.Execute().GetResult()
    }
}

// Extend mtsFunctionVoidReturn
%extend mtsFunctionVoidReturn {
    %pythoncode {
        def UpdateFromC(self):
            if self.IsValid():
                try:
                    tmpObject = self.GetResultPrototype().Services().Create()
                    self.ArgumentType = tmpObject.__class__
                except Exception, e:
                    print 'VoidReturn function ', self.GetCommand().GetName(), ': ', e
            else:
                print 'VoidReturn function not valid'


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
    }
}

// Extend mtsFunctionRead
%extend mtsFunctionRead {
    %pythoncode {
        def UpdateFromC(self):
            if self.IsValid():
                try:
                    tmpObject = self.GetArgumentPrototype().Services().Create()
                    self.ArgumentType = tmpObject.__class__
                except Exception, e:
                    print 'Read function ', self.GetCommand().GetName(), ': ', e
            else:
                print 'Read function not valid'


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
    }
}

// Extend mtsFunctionWrite
%extend mtsFunctionWrite {
    %pythoncode {
        def UpdateFromC(self):
            if self.IsValid():
                try:
                    tmpObject = self.GetArgumentPrototype().Services().Create()
                    self.ArgumentType = tmpObject.__class__
                except Exception, e:
                    print 'Write function ', self.GetCommand().GetName(), ': ', e
            else:
                print 'Write function not valid'

        def __call__(self, argument):
            if isinstance(argument, self.ArgumentType):
                return self.Execute(argument).GetResult()
            else:
                realArgument = self.ArgumentType(argument)
                return self.Execute(realArgument).GetResult()
    }
}

// Extend mtsFunctionWriteReturn
%extend mtsFunctionWriteReturn {
    %pythoncode {
        def UpdateFromC(self):
            if self.IsValid():
                try:
                    tmp1Object = self.GetArgumentPrototype().Services().Create()
                    self.ArgumentType = tmp1Object.__class__
                    tmp2Object = self.GetResultPrototype().Services().Create()
                    self.ResultType = tmp2Object.__class__
                except Exception, e:
                    print 'WriteReturn function ', self.GetCommand().GetName(), ': ', e
            else:
                print 'WriteReturn function not valid'

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
    }
}

// Extend mtsFunctionQualifiedRead
%extend mtsFunctionQualifiedRead {
    %pythoncode {
        def UpdateFromC(self):
            if self.IsValid():
                try:
                    tmp1Object = self.GetArgument1Prototype().Services().Create()
                    self.Argument1Type = tmp1Object.__class__
                    tmp2Object = self.GetArgument2Prototype().Services().Create()
                    self.Argument2Type = tmp2Object.__class__
                except Exception, e:
                    print 'Qualified read function ', self.GetCommand().GetName(), ': ', e
            else:
                print 'QualifiedRead function not valid'

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
    }
}


// Wrap tasks and components
%include "cisstMultiTask/mtsComponent.h"
%extend mtsComponent {
    %pythoncode {
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
            if not isinstance(interfaceProvided, mtsInterfaceProvided):
                print 'Parameter must be of type mtsInterfaceProvided'
                return
            interfaceRequired = self.AddInterfaceRequired('RequiredFor'+interfaceProvided.GetName(), MTS_OPTIONAL)
            for command in interfaceProvided.GetNamesOfCommandsVoid():
                self.__dict__[command] = mtsFunctionVoid()
                interfaceRequired.AddFunction(command, self.__dict__[command])
            for command in interfaceProvided.GetNamesOfCommandsVoidReturn():
                self.__dict__[command] = mtsFunctionVoidReturn()
                interfaceRequired.AddFunction(command, self.__dict__[command])
            for command in interfaceProvided.GetNamesOfCommandsWrite():
                self.__dict__[command] = mtsFunctionWrite()
                interfaceRequired.AddFunction(command, self.__dict__[command])
            for command in interfaceProvided.GetNamesOfCommandsWriteReturn():
                self.__dict__[command] = mtsFunctionWriteReturn()
                interfaceRequired.AddFunction(command, self.__dict__[command])
            for command in interfaceProvided.GetNamesOfCommandsQualifiedRead():
                self.__dict__[command] = mtsFunctionQualifiedRead()
                interfaceRequired.AddFunction(command, self.__dict__[command])
            for command in interfaceProvided.GetNamesOfCommandsRead():
                self.__dict__[command] = mtsFunctionRead()
                interfaceRequired.AddFunction(command, self.__dict__[command])
            
        # otherComponentInterface should be a tuple ('process', 'component', 'interfaceProvided')
        # or ('component', 'interfaceProvided')
        def AddInterfaceRequiredAndConnect(self, otherComponentInterface):
            try:
                num = len(otherComponentInterface)
                if 2 <= num <= 3:
                    interfaceName = otherComponentInterface[num-1]
                    componentName = otherComponentInterface[num-2]
                    if num == 3:
                        processName = otherComponentInterface[num-3]
                    else:
                        processName = ''  # local
                    # Now do the work here...(TBD)
                else:
                    print 'Parameter error: must specify (process, component, interface) or (component, interface)'
            except TypeError, e:
                print 'Parameter error: must specify (process, component, interface) or (component, interface)'
    }
}

%include "cisstMultiTask/mtsInterfaceProvidedOrOutput.h"
%include "cisstMultiTask/mtsInterfaceProvided.h"
%extend mtsInterfaceProvided {
    %pythoncode {
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
    }
}

%include "cisstMultiTask/mtsInterfaceRequiredOrInput.h"
%include "cisstMultiTask/mtsInterfaceRequired.h"
%extend mtsInterfaceRequired {
    %pythoncode {
        def UpdateFromC(self):
            if not self.GetConnectedInterface():
                print 'Required interface ', self.GetName(), ' not yet connected.'
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
    }
}

%include "cisstMultiTask/mtsTask.h"
%include "cisstMultiTask/mtsTaskContinuous.h"
%include "cisstMultiTask/mtsTaskPeriodic.h"
%include "cisstMultiTask/mtsTaskFromSignal.h"

%include "cisstMultiTask/mtsManagerLocalInterface.h"
%include "cisstMultiTask/mtsManagerLocal.h"
%extend mtsManagerLocal {
    %pythoncode {
        def UpdateFromC(self):
            comps = mtsManagerLocal.GetNamesOfComponents(self)
            for comp in comps:
                self.__dict__[comp] = mtsManagerLocal.GetComponent(self, comp)
                self.__dict__[comp].UpdateFromC()
    }
}

%include "cisstMultiTask/mtsCollectorBase.h"
%include "cisstMultiTask/mtsCollectorState.h"

// Wrap base class
%include "cisstMultiTask/mtsGenericObject.h"

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
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsInt, int);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsUInt, unsigned int);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsShort, short);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsUShort, unsigned short);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsLong, long);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsULong, unsigned long);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsBool, bool);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsStdString, std::string);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsStdStringVecProxy, stdStringVec);

%include "cisstMultiTask/mtsParameterTypes.h"
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsDescriptionConnectionVec, mtsDescriptionConnectionStdVec);

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
MTS_INSTANTIATE_VECTOR(mtsIntVec, int);
MTS_INSTANTIATE_VECTOR(mtsShortVec, short);
MTS_INSTANTIATE_VECTOR(mtsLongVec, long);
MTS_INSTANTIATE_VECTOR(mtsUCharVec, unsigned char);
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

// Wrap mtsStateIndex
%include "cisstMultiTask/mtsStateIndex.h"
