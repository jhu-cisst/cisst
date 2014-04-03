/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Anton Deguet
  Created on: 2004-10-05

  (C) Copyright 2004-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "myDerivedClass.h"
#include "myReDerivedClass.h"


#include <cisstConfig.h>
#include <cisstCommon/cmnObjectRegister.h>
#include <cisstCommon/cmnPath.h>


#include <Python.h>


int main(void) {
    std::cout << "*** Start" << std::endl;
    std::cout << "*** List of registered classes: " << cmnClassRegister::ToString() << std::endl;
    myDerivedClass derived1;
    derived1.FixedSizeVector().SetAll(10);
    derived1.DynamicVector().SetSize(5);
    derived1.DynamicVector().SetAll(-10);
    std::cout << "*** Fixed size vector of derived1 before python: " << derived1.FixedSizeVector() << std::endl;
    
    myReDerivedClass rederived1;
    rederived1.FixedSizeVector().SetAll(1000);
    
    cmnObjectRegister::Register("derived1", &derived1);
    cmnObjectRegister::Register("rederived1", &rederived1);
    
    std::cout << "*** List from C: " << cmnObjectRegister::ToString() << std::endl;
    
    cmnGenericObject* ptr = cmnObjectRegister::FindObject("derived1");
    std::cout << "*** Addresses of derived1: " 
              << &derived1 << " "
              << ptr << std::endl;
    ptr = cmnObjectRegister::FindObject("rederived1");
    std::cout << "*** Addresses of rederived1: " 
              << &rederived1 << " "
              << ptr << std::endl;
    std::cout << "*** List from C: " << cmnObjectRegister::ToString() << std::endl;

    std::string cisstRoot;
    if (!cmnPath::GetCisstRoot(cisstRoot)) {
        std::cout << "Can't find environment variable CISST_ROOT" << std::endl;
        exit(0);
    }
    Py_Initialize();
    PyRun_SimpleString("import sys; sys.path.append(\".\");");
    std::string command = std::string("sys.path.append(\"") +  cisstRoot + "/lib\");";
    std::cout << "*** Path for cisst libraries: " << cisstRoot << std::endl;
    PyRun_SimpleString(const_cast<char*>(command.c_str()));
#if (CISST_OS == CISST_WINDOWS)
#ifdef _DEBUG
    command = std::string("sys.path.append(\"") + cisstRoot + "/lib/debug\");";
    std::cout << "*** Path for cisst libraries: " << cisstRoot << "/lib/debug" << std::endl;
#else
    command = std::string("sys.path.append(\"") + cisstRoot + "/lib/release\");";
    std::cout << "*** Path for cisst libraries: " << cisstRoot << "/lib/release" << std::endl;
#endif
    PyRun_SimpleString(const_cast<char*>(command.c_str()));
#endif

    std::cout << "*** Import cisstCommonPython and cisstVectorPython" << std::endl;
    if (PyRun_SimpleString("import cisstCommonPython; from cisstCommonPython import *") != 0) {
        std::cout << "Failed importing cisstCommonPython!" << std::endl;
        exit(0);
    }
    if (PyRun_SimpleString("import cisstVectorPython; from cisstVectorPython import *") != 0) {
        std::cout << "Failed importing cisstVectorPython!" << std::endl;
        exit(0);
    }

    std::cout << "*** Import myDerivedClassPython and myReDerivedClassPython" << std::endl;
    if (PyRun_SimpleString("import myDerivedClassPython; from myDerivedClassPython import *") != 0) {
        std::cout << "Failed importing myDerivedClassPython!" << std::endl;
        exit(0);
    }
    if (PyRun_SimpleString("import myReDerivedClassPython; from myReDerivedClassPython import *") != 0) {
        std::cout << "Failed importing myReDerivedClassPython!" << std::endl;
        exit(0);
    }

    std::cout << "*** Run some Python commands from C" << std::endl;
    PyRun_SimpleString("derived1 = cmnObjectRegister.FindObject(\"derived1\"); print derived1.__class__; print derived1.FixedSizeVector()");
    PyRun_SimpleString("rederived1 = cmnObjectRegister.FindObject(\"rederived1\"); print rederived1.__class__; print rederived1.Services().GetName()");
    PyRun_SimpleString("rederived2 = myReDerivedClass(); cmnObjectRegister.Register(\"rederived2\", rederived2);");

    std::cout << "*** Load a Python script and execute it" << std::endl;
    bool FileFound = true;
    FILE * fp = NULL;
    cmnPath path;
    path.AddRelativeToCisstRoot("/bin");
    std::string fullName = path.Find("pythonEmbedded.py");
    if (fullName == "") {
        std::cout << "*** Can't find file \"pythonEmbedded.py\" in path " << path << std::endl;
        FileFound = false;
    } else {
        fp = fopen(fullName.c_str(), "r");
        if (fp == NULL) {
            std::cout << "*** Can't open \"" << fullName << "\"" << std::endl;
            FileFound = false;
        }
    }

    if (FileFound) {
#if (CISST_OS != CISST_WINDOWS)
        PyRun_SimpleFile(fp, "pythonEmbedded.py");
#endif
        PyRun_InteractiveLoop(fp, "pythonEmbedded.py");
        fclose(fp);
    }
    std::cout << "*** List from C: " << cmnObjectRegister::ToString() << std::endl;
    std::cout << "*** Fixed size vector of derived1 after python: " << derived1.FixedSizeVector() << std::endl;
    Py_Finalize();

    return 0;
}

