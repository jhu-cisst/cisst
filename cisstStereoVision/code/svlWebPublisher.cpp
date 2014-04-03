/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 
 Author(s): Balazs Vagvolgyi
 Created on: 2009-11-04
 
 (C) Copyright 2005-2009 Johns Hopkins University (JHU), All Rights
 Reserved.
 
 --- begin cisst license - do not edit ---
 
 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.
 
 --- end cisst license ---
 
 */


#include "cisstStereoVision/svlWebPublisher.h"
#include "cisstStereoVision/svlWebFileObject.h"
#include "cisstStereoVision/svlStreamDefs.h"

#include <iostream>
#include <Python.h>

#define PYTHON_SERVER_AUTO_START


CMN_IMPLEMENT_SERVICES(svlWebPublisher)

svlWebPublisher::svlWebPublisher(unsigned int port, bool fileserver) :
    cmnGenericObject(),
    Port(port),
    FileServer(fileserver),
    ServerThread(0)
{
}

svlWebPublisher::~svlWebPublisher()
{
    Stop();
}

unsigned int svlWebPublisher::GetPort()
{
    return Port;
}

bool svlWebPublisher::IsFileServer()
{
    return FileServer;
}

int svlWebPublisher::Start()
{
    Stop();

    // Register object to Python
    cmnObjectRegister::Register("WebPublisher", this);

    ///////////////////////
    // Initialize Python //
    ///////////////////////

    Py_Initialize();
    PyRun_SimpleString("import sys; sys.path.append(\".\");");
    std::string command = std::string("sys.path.append(\"") + CISST_BUILD_LINK_DIRECTORIES + "\");";
    std::cout << "*** Path for cisst libraries: " << CISST_BUILD_LINK_DIRECTORIES << std::endl;
    PyRun_SimpleString(const_cast<char*>(command.c_str()));
#if (CISST_OS == CISST_WINDOWS)
#ifdef _DEBUG
    command = std::string("sys.path.append(\"") + CISST_BUILD_LINK_DIRECTORIES + "/debug\");";
    std::cout << "*** Path for cisst libraries: " << CISST_BUILD_LINK_DIRECTORIES << "/debug" << std::endl;
    PyRun_SimpleString(const_cast<char*>(command.c_str()));
#else
    command = std::string("sys.path.append(\"") + CISST_BUILD_LINK_DIRECTORIES + "/release\");";
    std::cout << "*** Path for cisst libraries: " << CISST_BUILD_LINK_DIRECTORIES << "/release" << std::endl;
    PyRun_SimpleString(const_cast<char*>(command.c_str()));
#endif
#endif

    ///////////////////////////
    // Import CISST wrappers //
    ///////////////////////////

    std::cout << "*** Importing cisstCommonPython..." << std::endl;
    if (PyRun_SimpleString("import cisstCommonPython; from cisstCommonPython import *") != 0) {
        std::cout << "Failed to import cisstCommonPython" << std::endl;
        ServerInitSuccess = false;
    }
    std::cout << "*** Importing cisstVectorPython..." << std::endl;
    if (PyRun_SimpleString("import cisstVectorPython; from cisstVectorPython import *") != 0) {
        std::cout << "Failed to import cisstVectorPython" << std::endl;
        ServerInitSuccess = false;
    }
    std::cout << "*** Importing cisstStereoVisionPython..." << std::endl;
    if (PyRun_SimpleString("import cisstStereoVisionPython; from cisstStereoVisionPython import *") != 0) {
        std::cout << "Failed to import cisstStereoVisionPython" << std::endl;
        ServerInitSuccess = false;
    }

    ///////////////////////////////
    // Import Python HTTP server //
    ///////////////////////////////

    std::cout << "*** Importing svlWebPublisherPython..." << std::endl;
    if (PyRun_SimpleString("import svlWebPublisherPython; from svlWebPublisherPython import *") != 0) {
        std::cout << "Failed to import svlWebPublisherPython" << std::endl;
        ServerInitSuccess = false;
    }

    // Start server thread
    ServerInitSuccess = false;
    ServerThread = new osaThread();
    ServerThread->Create<svlWebPublisher, int>(this, &svlWebPublisher::Proc, 0);
    ServerSignal.Wait();

    return SVL_OK;
}

void svlWebPublisher::Stop()
{
    if (ServerThread) {
        // Stop server
std::cerr<<"0"<<std::endl;
        PyObject* n = PyString_FromString("svlWebPublisherPython");
std::cerr<<"1"<<std::endl;
        PyObject* m = PyImport_Import(n);
std::cerr<<"2"<<std::endl;
        PyObject* d = PyModule_GetDict(m);
std::cerr<<"3"<<std::endl;
        PyObject* c = PyDict_GetItemString(d, "HTTPServerWithStop");
std::cerr<<"4"<<std::endl;
        PyObject* o = PyObject_CallObject(c, NULL);
std::cerr<<"5"<<std::endl;
        PyObject_CallMethod(o, "StopServer", NULL);
std::cerr<<"6"<<std::endl;

        Py_DECREF(o);
        Py_DECREF(c);
        Py_DECREF(d);
        Py_DECREF(m);
        Py_DECREF(n);

        PyRun_SimpleString("svlWebPublisherPython.StopServer()");
        ServerThread->Wait();
        delete ServerThread;
        ServerThread = 0;
    }
}

int svlWebPublisher::AddObject(svlWebObjectBase* object)
{
    if (object == 0 || object->GetName().empty()) return SVL_FAIL;

    int ret = SVL_FAIL;

    CS.Enter();
        if (Objects.find(object->GetName()) == Objects.end()) {
            Objects.insert(_ObjectPair(object->GetName(), object));
            ret = SVL_OK;
        }
    CS.Leave();

    return ret;
}

int svlWebPublisher::RemoveObject(const std::string & objectname)
{
    int ret;

    CS.Enter();
        ret = Objects.erase(objectname);
    CS.Leave();

    if (ret == 0) return SVL_FAIL;
    return SVL_OK;
}

int svlWebPublisher::RemoveObject(svlWebObjectBase* object)
{
    int ret = SVL_FAIL;

    CS.Enter();
        _ObjectMap::iterator iter;
        for (iter = Objects.begin(); iter != Objects.end(); iter ++) {
            if (iter->second == object) break;
        }
    if (iter != Objects.end()) {
            Objects.erase(iter->first);
            ret = SVL_OK;
        }
    CS.Leave();

    return ret;
}

svlWebObjectBase* svlWebPublisher::GetObject(const std::string & objectname, RequestType request)
{
    svlWebObjectBase* object = 0;

    CS.Enter();
        _ObjectMap::iterator iter = Objects.find(objectname);
        if (iter != Objects.end()) {
            switch (request) {
                case GET:
                    object = iter->second->clone();
                    object->Temporary = true;
                break;

                case POST:
                    object = iter->second;
                break;
            }
        }
        else {
            if (FileServer && request == GET) {
                svlWebFileObject* fileobject = new svlWebFileObject(objectname);
                fileobject->Temporary = true;
                if (fileobject->PrepareContents()) {
                    object = fileobject;
                }
                else {
                    delete fileobject;
                    object = 0;
                }
            }
        }
    CS.Leave();

    return object;
}

void svlWebPublisher::ReleaseObject(svlWebObjectBase* object)
{
    if (object && object->IsTemporary()) delete object;
}

void* svlWebPublisher::Proc(int CMN_UNUSED(param))
{
    ServerInitSuccess = true;
    ServerSignal.Raise();

    //////////////////
    // Start server //
    //////////////////

#ifdef PYTHON_SERVER_AUTO_START
    PyRun_SimpleString("svlWebPublisherPython.StartServer()");
#else
    char* parms[] = { "webgui" };
    Py_Main(1, parms);
#endif

    ////////////////////
    // Release Python //
    ////////////////////

    Py_Finalize();

    return this;
}

