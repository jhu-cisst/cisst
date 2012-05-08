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
#include <cisstMatlab/mtlCallFunction.h>
#include <mex.h>
}

#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionRead.h>

const char * mtlCallFunctionVoid(char * voidPointerOnFunction)
{
    unsigned long long int inter;
    sscanf(voidPointerOnFunction, "%llu", &inter);
    mtsFunctionVoid * functionPointer = reinterpret_cast<mtsFunctionVoid *>(inter);
    mtsExecutionResult result;
    result = functionPointer->Execute();
    if (!result.IsOK()) {
        std::string errorMessage = "mtlCallFunctionVoid: failed, returned \"";
        errorMessage = errorMessage + mtsExecutionResult::ToString(result.GetResult()) + "\"";
        mexErrMsgTxt(errorMessage.c_str());
    }
    return mtsExecutionResult::ToString(result.GetResult()).c_str();
}


mxArray * mtlCallFunctionRead(char * voidPointerOnFunction)
{
    unsigned long long int inter;
    sscanf(voidPointerOnFunction, "%llu", &inter);
    mtsFunctionRead * functionPointer = reinterpret_cast<mtsFunctionRead *>(inter);
    mtsExecutionResult result;
    mtsGenericObject * placeHolder = dynamic_cast<mtsGenericObject *>(functionPointer->GetArgumentPrototype()->Services()->Create());
    result = functionPointer->Execute(*placeHolder);
    if (!result.IsOK()) {
        std::string errorMessage = "mtlCallFunctionVoid: failed, returned \"";
        errorMessage = errorMessage + mtsExecutionResult::ToString(result.GetResult()) + "\"";
        mexErrMsgTxt(errorMessage.c_str());
    }
    // return mtsExecutionResult::ToString(result.GetResult()).c_str();
    return placeHolder->ToMatlab();
}