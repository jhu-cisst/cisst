/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2012-04-26

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <stdio.h>
#include <string>

extern "C" {
#include <cisstMatlab/mtlCreateComponent.h>
}


mxArray * mtlCreateComponent(const char * objectName)
{
    std::string code;

    mexPrintf("\ncmnCreateObject\n");
    mxArray * result;
    result = mxCreateStructMatrix(1, 1, 0, 0);

    int objectField = mxAddField(result, "object");

//     // create object
//     cmlTestClass * object = new cmlTestClass;
//     unsigned long long int inter = reinterpret_cast <unsigned long long int>(object);
//     mexPrintf("set %llu\n", inter);
//     // convert to string, this needs tobe replaced by a long long int sent to Matlab
//     char pointer[256];
//     sprintf(pointer, "%llu", inter);
//     mxSetFieldByNumber(result, 0, objectField, mxCreateString(pointer));

//     int methodsField = mxAddField(result, "methods");
//     mxArray * methods = mxCreateCellMatrix(2, 1); // first cell is object pointer, then methods
//     mxSetFieldByNumber(result, 0, methodsField, methods);
//     mxArray * method1 = mxCreateCellMatrix(2, 1); // first cell is method name, 
//     mxSetCell(method1, 0, mxCreateString("Function1"));
//     //     mxSetCell(method1, 1, mxCreateString);
//     //     mxSetCell(result, 1, method1);
//     const std::map<std::string, bool>::iterator found = CreatedClasses.find("classTest");
//     if (found == CreatedClasses.end()) {
//         std::ofstream classFile;
//         classFile.open("classTest.m");
//         classFile << "classdef classTest;properties;      Name = 'undef';end;methods;function self = set.Name(self, name);         self.Name = name;end;function name = GetName(self);name = self.Name;end;end;end" << std::endl << std::flush;
//         classFile.close();
//         mexPrintf("\nCreating class\n");
//         CreatedClasses["classTest"] = true;

//         // force Matlab to rehash the current directory so it sees the created file
//         code = "rehash;";
//         mexEvalString(code.c_str());

//     } else {
//         mexPrintf("\nClass already exist\n");
//     }
    
//     code = objectName;
//     code = code + " = classTest;";
//     mexEvalString(code.c_str());
//     mexPrintf("\n code is %s\n", code.c_str());
//     code = "testObject = 'classTest';";
//     mexEvalString(code.c_str());

//     code = "myCustomObj = eval('dynamicprops');";
//     mexEvalString(code.c_str());

//     code = "myCustomObj.addprop('mySine')";
//     mexEvalString("myCustomObj.mySine = @sin"); //  % this could be a function handle to anything.  e.g. calllib
    return result;
}
