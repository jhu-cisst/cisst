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

#define CISST_EXPORT
#define CISST_DEPRECATED

// Wrap commands
%import "cisstMultiTask/mtsCommandBase.h"
%include "cisstMultiTask/mtsCommandVoidBase.h"
%include "cisstMultiTask/mtsCommandReadOrWriteBase.h"
%include "cisstMultiTask/mtsCommandQualifiedReadOrWriteBase.h"

%template(mtsCommandReadBase) mtsCommandReadOrWriteBase<cmnGenericObject>;
%template(mtsCommandWriteBase) mtsCommandReadOrWriteBase<const cmnGenericObject>; 
%template(mtsCommandQualifiedReadBase) mtsCommandQualifiedReadOrWriteBase<cmnGenericObject>;
%template(mtsCommandQualifiedWriteBase) mtsCommandQualifiedReadOrWriteBase<const cmnGenericObject>; 
%{
    typedef mtsCommandReadOrWriteBase<cmnGenericObject> mtsCommandReadBase;
    typedef mtsCommandReadOrWriteBase<const cmnGenericObject> mtsCommandWriteBase;
    typedef mtsCommandQualifiedReadOrWriteBase<cmnGenericObject> mtsCommandQualifiedReadBase;
    typedef mtsCommandQualifiedReadOrWriteBase<const cmnGenericObject> mtsCommandQualifiedWriteBase;
%}
typedef mtsCommandReadOrWriteBase<cmnGenericObject> mtsCommandReadBase;
typedef mtsCommandReadOrWriteBase<const cmnGenericObject> mtsCommandWriteBase;
typedef mtsCommandQualifiedReadOrWriteBase<cmnGenericObject> mtsCommandQualifiedReadBase;
typedef mtsCommandQualifiedReadOrWriteBase<const cmnGenericObject> mtsCommandQualifiedWriteBase;
%types(mtsCommandReadBase *);
%types(mtsCommandWriteBase *);
%types(mtsCommandQualifiedReadBase *);
%types(mtsCommandQualifiedWriteBase *);

%extend mtsCommandReadOrWriteBase<const cmnGenericObject> {
    %pythoncode {
        def UpdateFromC(self):
            tmpObject = self.GetArgumentClassServices().Create()
            self.ArgumentType = tmpObject.__class__

        def __call__(self, argument):
            if isinstance(argument, self.ArgumentType):
                self.Execute(argument)
            else:
                realArgument = self.ArgumentType(argument)
                self.Execute(realArgument)
    }
}

%extend mtsCommandReadOrWriteBase<cmnGenericObject> {
    %pythoncode {
        def UpdateFromC(self):
            tmpObject = self.GetArgumentClassServices().Create()
            self.ArgumentType = tmpObject.__class__

        def __call__(self):
            argument = self.ArgumentType()
            self.Execute(argument)
            return argument
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
                self.__dict__[interface].AllocateResourcesForCurrentThread()
                self.__dict__[interface].UpdateFromC()
    }
}

%include "cisstMultiTask/mtsDeviceInterface.h"
%extend mtsDeviceInterface {
    %pythoncode {
        def UpdateFromC(self):
            commands = mtsDeviceInterface.GetNamesOfCommandsVoid(self)
            for command in commands:
                self.__dict__[command] = mtsDeviceInterface.GetCommandVoid(self, command)
            commands = mtsDeviceInterface.GetNamesOfCommandsWrite(self)
            for command in commands:
                self.__dict__[command] = mtsDeviceInterface.GetCommandWrite(self, command)
                self.__dict__[command].UpdateFromC()
            commands = mtsDeviceInterface.GetNamesOfCommandsRead(self)
            for command in commands:
                self.__dict__[command] = mtsDeviceInterface.GetCommandRead(self, command)
                self.__dict__[command].UpdateFromC()
            commands = mtsDeviceInterface.GetNamesOfCommandsQualifiedRead(self)
            for command in commands:
                self.__dict__[command] = mtsDeviceInterface.GetCommandQualifiedRead(self, command)
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


