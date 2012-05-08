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

mxArray * mtlCreateComponent(const char * componentName)
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
    mxArray * result;

    // create component proxy
    code << componentName << " = eval('dynamicprops');";
    mexEvalString(code.str().c_str());
    code.str("");

    // create interface proxy
    code << componentName << ".addprop('interface1');";
    mexEvalString(code.str().c_str());
    code.str("");
    code << componentName << ".interface1 = eval('dynamicprops');";
    mexEvalString(code.str().c_str());
    code.str("");

    // create function proxy
    code << componentName << ".interface1.addprop('Zero');";
    mexEvalString(code.str().c_str());
    code.str("");
    CMN_LOG_INIT_ERROR << "------------------------------------ " << &(componentProxy->Zero) << std::endl;

    unsigned long long int inter = reinterpret_cast <unsigned long long int>(&(componentProxy->Zero));
    // convert to string, this needs tobe replaced by a long long int sent to Matlab
    char pointer[256];
    sprintf(pointer, "%llu", inter);

    code << componentName << ".interface1.Zero = @()calllib('libcisstMatlab', 'mtlCallFunctionVoid', '"
         << pointer << "');";
    mexEvalString(code.str().c_str());
    code.str("");

    //    mxArray * result; - to be removed, return bool or string for failure?, or void?
    result = mxCreateStructMatrix(1, 1, 0, 0);
    return result;
}
