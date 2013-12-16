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
#include <cisstMultiTask/mtsFunctionWrite.h>

const char * mtlCallFunctionVoid(uint64_T voidPointerOnFunction)
{
    mtsFunctionVoid * functionPointer = reinterpret_cast<mtsFunctionVoid *>(voidPointerOnFunction);
    mtsExecutionResult result;
    result = functionPointer->Execute();
    if (!result.IsOK()) {
        std::string errorMessage = "mtlCallFunctionVoid: failed, returned \"";
        errorMessage = errorMessage + mtsExecutionResult::ToString(result.GetResult()) + "\"";
        mexErrMsgTxt(errorMessage.c_str());
    }
    return mtsExecutionResult::ToString(result.GetResult()).c_str();
}


struct phonebook
{
  const char *name;
  double phone;
};

mxArray * mtlCallFunctionRead(uint64_T voidPointerOnFunction)
{
    CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << std::endl;
    mtsFunctionRead * functionPointer = reinterpret_cast<mtsFunctionRead *>(voidPointerOnFunction);
    mtsExecutionResult result;
    mtsGenericObject * placeHolder = dynamic_cast<mtsGenericObject *>(functionPointer->GetArgumentPrototype()->Services()->Create());
    result = functionPointer->Execute(*placeHolder);
    if (!result.IsOK()) {
        std::string errorMessage = "mtlCallFunctionVoid: failed, returned \"";
        errorMessage = errorMessage + mtsExecutionResult::ToString(result.GetResult()) + "\"";
        mexErrMsgTxt(errorMessage.c_str());
    }
    // return mtsExecutionResult::ToString(result.GetResult()).c_str();
    // return placeHolder->ToMatlab();
CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << std::endl;
   const char *field_names[] = {"name", "phone"};
   struct phonebook friends[] = {{"Jordan Robert", 3386},{"Mary Smith",3912},
                                 {"Stacy Flora", 3238},{"Harry Alpert",3077}};

    mwSize dims[2] = {1, 2};
    int name_field, phone_field;
    mwIndex i;

    mxArray * resultArray;
    /* Create a 1-by-n array of structs. */ 
CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << std::endl;
    resultArray = mxCreateStructArray(2, dims, 2, field_names);
CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << std::endl;
    /* This is redundant, but here for illustration.  Since we just
       created the structure and the field number indices are zero
       based, name_field will always be 0 and phone_field will always
       be 1 */
CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << std::endl;
    name_field = mxGetFieldNumber(resultArray,"name");
    phone_field = mxGetFieldNumber(resultArray,"phone");
CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << std::endl;
    /* Populate the name and phone fields of the phonebook structure. */ 
    for (i=0; i<3; i++) {
        mxArray *field_value;
        /* Use mxSetFieldByNumber instead of mxSetField for efficiency
         * mxSetField(plhs[0],i,"name",mxCreateString(friends[i].name); */
CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << std::endl;
        mxSetFieldByNumber(resultArray, i, name_field, mxCreateString(friends[i].name));
CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << std::endl;
        field_value = mxCreateDoubleMatrix(1, 1, mxREAL);
CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << std::endl;
        *mxGetPr(field_value) = friends[i].phone;
CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << std::endl;
        /* Use mxSetFieldByNumber instead of mxSetField for efficiency
         * mxSetField(plhs[0],i,"name",mxCreateString(friends[i].name); */
        mxSetFieldByNumber(resultArray, i, phone_field, field_value);
CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << std::endl;
    }
CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS << std::endl;
return resultArray;
}


const char * mtlCallFunctionWrite(uint64_T voidPointerOnFunction, mxArray * input)
{
    mtsFunctionWrite * functionPointer = reinterpret_cast<mtsFunctionWrite *>(voidPointerOnFunction);
    mtsExecutionResult result;
    mtsGenericObject * inputCisst = dynamic_cast<mtsGenericObject *>(functionPointer->GetArgumentPrototype()->Services()->Create());

    try {
        inputCisst->FromMatlab(input);
    } catch (std::runtime_error exc) {
        mexErrMsgTxt(exc.what());
        return 0;
    }

    result = functionPointer->Execute(*inputCisst);
    if (!result.IsOK()) {
        std::string errorMessage = "mtlCallFunctionVoid: failed, returned \"";
        errorMessage = errorMessage + mtsExecutionResult::ToString(result.GetResult()) + "\"";
        mexErrMsgTxt(errorMessage.c_str());
    }

    return mtsExecutionResult::ToString(result.GetResult()).c_str();
}
