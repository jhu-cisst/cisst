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
#include <mex.h>
}

// this is for testing/development
#include "testComponent.h"

const char * mtlCreateComponent(const char * componentName)
{
    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassAll(CMN_LOG_ALLOW_ALL);

    std::ofstream * out = new std::ofstream;
    out->open("cisstMatlab-log.txt");
    cmnLogger::AddChannel(*out, CMN_LOG_ALLOW_ALL);

    // create the component, for now use our testComponent class
    // instead of dynamic loading + creation
    mtsComponent * component = new testComponent(componentName);

    // add the component to the component manager
    mtsComponentManager * componentManager = mtsManagerLocal::GetInstance();
    componentManager->AddComponent(component);

    // create a proxy, for now use hard coded one
    testComponentProxy * componentProxy = new testComponentProxy(std::string("MatlabProxyFor") + componentName);
    componentManager->AddComponent(componentProxy);

    if (!componentManager->Connect(componentProxy->GetName(), "interface1",
                                   component->GetName(), "interface1")) {
        mexPrintf("failed to connect interface 1");
    }

    if (!componentManager->Connect(componentProxy->GetName(), "interface2",
                                   component->GetName(), "interface2")) {
        mexPrintf("failed to connect interface 2");
    }

    component->Create();
    componentProxy->Create();
    component->Start();
    componentProxy->Start();

    std::stringstream code;

    // create component proxy
    mexEvalString(code.str().c_str());
    code.str("");
    
    code << componentName << " = eval('mtlDynamicProps');";
    mexEvalString(code.str().c_str());
    code.str("");

    // create interface proxy
    code << componentName << ".addprop('interface1');";
    mexEvalString(code.str().c_str());
    code.str("");
    code << componentName << ".interface1 = eval('mtlDynamicProps');";
    mexEvalString(code.str().c_str());
    code.str("");

    // create function proxy
    code << componentName << ".interface1.addprop('Zero');";
    mexEvalString(code.str().c_str());
    code.str("");
    CMN_LOG_INIT_ERROR << "------------------------------------ " << &(componentProxy->Zero) << std::endl;

    // create interface proxy
    code << componentName << ".addprop('interface2');";
    mexEvalString(code.str().c_str());
    code.str("");
    code << componentName << ".interface2 = eval('mtlDynamicProps');";
    mexEvalString(code.str().c_str());
    code.str("");

    // create function proxy
    code << componentName << ".interface2.addprop('Get');";
    mexEvalString(code.str().c_str());
    code.str("");

    // create function proxy
    code << componentName << ".interface2.addprop('Set');";
    mexEvalString(code.str().c_str());
    code.str("");


    uint64_T inter = reinterpret_cast <uint64_T>(&(componentProxy->Zero));
    code << componentName << ".interface1.Zero = @()calllib('libcisstMatlab', 'mtlCallFunctionVoid', "
         << inter << ");";
    mexEvalString(code.str().c_str());
    code.str("");

    ///GET
    inter = reinterpret_cast <unsigned long long int>(&(componentProxy->Get));
    code << componentName << ".interface2.Get = @()calllib('libcisstMatlab', 'mtlCallFunctionRead', "
    << inter << ");";
    mexEvalString(code.str().c_str());
    code.str("");


    ///Set
    inter = reinterpret_cast <unsigned long long int>(&(componentProxy->Set));
    code << componentName << ".interface2.Set = @(x)calllib('libcisstMatlab', 'mtlCallFunctionWrite', "
         << inter << ", x);";
    mexEvalString(code.str().c_str());
    code.str("");

    return "no error";
}
