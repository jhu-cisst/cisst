/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstMultiTask.i,v 1.4 2009/01/07 05:04:36 pkaz Exp $

  Author(s):	Anton Deguet
  Created on:   2008-01-17

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
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

// use class type to create the correct Python type
%apply cmnGenericObject * {mtsGenericObject *};

// It is useful to wrap osaTimeServer. This can be removed
// if cisstOSAbstraction is wrapped.
%include "cisstOSAbstraction/osaTimeServer.h"

%template(mtsStringVector) std::vector<std::string>;

%header %{
    // Put header files here
    #include "cisstMultiTask/cisstMultiTask.i.h"
%}

// Generate parameter documentation for IRE
%feature("autodoc", "1");

%rename(__str__) ToString;
%ignore *::ToStream;
%ignore operator<<;

%ignore *::operator[]; // We define __setitem__ and __getitem__

%ignore *::AddCommandVoid;
%ignore *::AddEventVoid;

#define CISST_EXPORT
#define CISST_DEPRECATED

// Wrap commands
%import "cisstMultiTask/mtsCommandBase.h"
%include "cisstMultiTask/mtsCommandVoidBase.h"
%include "cisstMultiTask/mtsCommandReadOrWriteBase.h"
%include "cisstMultiTask/mtsCommandQualifiedReadOrWriteBase.h"

%template(mtsCommandReadBase) mtsCommandReadOrWriteBase<mtsGenericObject>;
%template(mtsCommandWriteBase) mtsCommandReadOrWriteBase<const mtsGenericObject>; 
%template(mtsCommandQualifiedReadBase) mtsCommandQualifiedReadOrWriteBase<mtsGenericObject>;
%template(mtsCommandQualifiedWriteBase) mtsCommandQualifiedReadOrWriteBase<const mtsGenericObject>; 
%{
    typedef mtsCommandReadOrWriteBase<mtsGenericObject> mtsCommandReadBase;
    typedef mtsCommandReadOrWriteBase<const mtsGenericObject> mtsCommandWriteBase;
    typedef mtsCommandQualifiedReadOrWriteBase<mtsGenericObject> mtsCommandQualifiedReadBase;
    typedef mtsCommandQualifiedReadOrWriteBase<const mtsGenericObject> mtsCommandQualifiedWriteBase;
%}
typedef mtsCommandReadOrWriteBase<mtsGenericObject> mtsCommandReadBase;
typedef mtsCommandReadOrWriteBase<const mtsGenericObject> mtsCommandWriteBase;
typedef mtsCommandQualifiedReadOrWriteBase<mtsGenericObject> mtsCommandQualifiedReadBase;
typedef mtsCommandQualifiedReadOrWriteBase<const mtsGenericObject> mtsCommandQualifiedWriteBase;
%types(mtsCommandReadBase *);
%types(mtsCommandWriteBase *);
%types(mtsCommandQualifiedReadBase *);
%types(mtsCommandQualifiedWriteBase *);

// Extend mtsCommandVoid
%extend mtsCommandVoidBase {
    %pythoncode {
        def __call__(self):
            return self.Execute()
    }
}

// Extend mtsCommandWrite
%extend mtsCommandReadOrWriteBase<const mtsGenericObject> {
    %pythoncode {
        def UpdateFromC(self):
            tmpObject = self.GetArgumentClassServices().Create()
            self.ArgumentType = tmpObject.__class__

        def __call__(self, argument):
            if isinstance(argument, self.ArgumentType):
                return self.Execute(argument)
            else:
                realArgument = self.ArgumentType(argument)
                return self.Execute(realArgument)
    }
}

// Extend mtsCommandRead
%extend mtsCommandReadOrWriteBase<mtsGenericObject> {
    %pythoncode {
        def UpdateFromC(self):
            tmpObject = self.GetArgumentClassServices().Create()
            self.ArgumentType = tmpObject.__class__

        def __call__(self):
            argument = self.ArgumentType(self.GetArgumentPrototype())
            self.Execute(argument)
            return argument
    }
}

// Extend mtsCommandQualifiedRead
%extend mtsCommandQualifiedReadOrWriteBase<mtsGenericObject> {
    %pythoncode {
        def UpdateFromC(self):
            tmp1Object = self.GetArgument1ClassServices().Create()
            self.Argument1Type = tmp1Object.__class__
            tmp2Object = self.GetArgument2ClassServices().Create()
            self.Argument2Type = tmp2Object.__class__

        def __call__(self, argument1):
            argument2 = self.Argument2Type(self.GetArgument2Prototype())
            if isinstance(argument1, self.Argument1Type):
                self.Execute(argument1, argument2)
            else:
                realArgument1 = self.Argument1Type(argument1)
                self.Execute(realArgument1, argument2)
            return argument2
    }
}

// Wrap tasks and devices
%include "cisstMultiTask/mtsDevice.h"
%extend mtsDevice {
    %pythoncode {
        def UpdateFromC(self):
            interfaces = mtsDevice.GetNamesOfProvidedInterfaces(self)
            for interface in interfaces:
                self.__dict__[interface] = mtsDevice.GetProvidedInterface(self, interface)
                userId = self.__dict__[interface].AllocateResources('Python')
                self.__dict__[interface].UpdateFromC(userId)
    }
}

%include "cisstMultiTask/mtsDeviceInterface.h"
%extend mtsDeviceInterface {
    %pythoncode {
        def UpdateFromC(self, userId):
            commands = mtsDeviceInterface.GetNamesOfCommandsVoid(self)
            for command in commands:
                self.__dict__[command] = mtsDeviceInterface.GetCommandVoid(self, command, userId)
            commands = mtsDeviceInterface.GetNamesOfCommandsWrite(self)
            for command in commands:
                self.__dict__[command] = mtsDeviceInterface.GetCommandWrite(self, command, userId)
                self.__dict__[command].UpdateFromC()
            commands = mtsDeviceInterface.GetNamesOfCommandsQualifiedRead(self)
            for command in commands:
                self.__dict__[command] = mtsDeviceInterface.GetCommandQualifiedRead(self, command)
                self.__dict__[command].UpdateFromC()
            commands = mtsDeviceInterface.GetNamesOfCommandsRead(self)
            for command in commands:
                self.__dict__[command] = mtsDeviceInterface.GetCommandRead(self, command)
                self.__dict__[command].UpdateFromC()
    }
}

%include "cisstMultiTask/mtsTask.h"
%include "cisstMultiTask/mtsTaskInterface.h"

%include "cisstMultiTask/mtsTaskManager.h"
%extend mtsTaskManager {
    %pythoncode {
        def UpdateFromC(self):
            tasks = mtsTaskManager.GetNamesOfTasks(self)
            for task in tasks:
                self.__dict__[task] = mtsTaskManager.GetTask(self, task)
                self.__dict__[task].UpdateFromC()
            devices = mtsTaskManager.GetNamesOfDevices(self)
            for device in devices:
                self.__dict__[device] = mtsTaskManager.GetDevice(self, device)
                self.__dict__[device].UpdateFromC()
    }
}

%include "cisstMultiTask/mtsCollectorBase.h"
%include "cisstMultiTask/mtsCollectorState.h"

// Wrap base class
%include "cisstMultiTask/mtsGenericObject.h"

// Wrap some basic types
%include "cisstMultiTask/mtsGenericObjectProxy.h"
%define MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(name, elementType)
// Instantiate the template
%template(name) mtsGenericObjectProxy<elementType>;
// Type addition for dynamic type checking
%{
    typedef mtsGenericObjectProxy<elementType> name;
%}
typedef mtsGenericObjectProxy<elementType> name;
%types(name *);
%enddef

MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsDouble, double);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsInt, int);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsUInt, unsigned int);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsShort, short);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsUShort, unsigned short);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsLong, long);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsULong, unsigned long);
MTS_GENERIC_OBJECT_PROXY_INSTANTIATE(mtsBool, bool);

// Wrap mtsVector
%import "cisstMultiTask/mtsVector.h"

// define macro
%define MTS_INSTANTIATE_VECTOR(name, elementType)
%template(name) mtsVector<elementType>;
%{
    typedef mtsVector<elementType> name;
%}
typedef mtsVector<elementType> name;
%types(name *);
%enddef

// instantiate for types also instantiated in cisstVector wrappers
MTS_INSTANTIATE_VECTOR(mtsDoubleVec, double); 
MTS_INSTANTIATE_VECTOR(mtsIntVec, int); 
MTS_INSTANTIATE_VECTOR(mtsShortVec, short); 
MTS_INSTANTIATE_VECTOR(mtsLongVec, long); 
MTS_INSTANTIATE_VECTOR(mtsBoolVec, bool); 

// Wrap mtsMatrix
%import "cisstMultiTask/mtsMatrix.h"

// define macro
%define MTS_INSTANTIATE_MATRIX(name, elementType)
%template(name) mtsMatrix<elementType>;
%{
    typedef mtsMatrix<elementType> name;
%}
typedef mtsMatrix<elementType> name;
%types(name *);
%enddef

// instantiate for types also instantiated in cisstVector wrappers
MTS_INSTANTIATE_MATRIX(mtsDoubleMat, double); 
MTS_INSTANTIATE_MATRIX(mtsIntMat, int); 
MTS_INSTANTIATE_MATRIX(mtsShortMat, short); 
MTS_INSTANTIATE_MATRIX(mtsLongMat, long); 
MTS_INSTANTIATE_MATRIX(mtsBoolMat, bool); 

// Wrap mtsStateIndex
%include "cisstMultiTask/mtsStateIndex.h"
