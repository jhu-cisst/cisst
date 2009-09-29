/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeMatrix.i,v 1.6 2007/04/26 19:33:58 anton Exp $

  Author(s):  Anton Deguet
  Created on: 2004-07-18

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


// Typemaps(out) for references
%typemap(out) vctDouble2x2 & {
    vctDynamicMatrixRef<double>* resultRef = new vctDynamicMatrixRef<double>(*result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrixRef<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleMatRef");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultRef), typeInfo, SWIG_POINTER_DISOWN);
    }
}

%typemap(out) vctInt2x2 & {
    vctDynamicMatrixRef<int>* resultRef = new vctDynamicMatrixRef<int>(*result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrixRef<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntMatRef");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultRef), typeInfo, SWIG_POINTER_DISOWN);
    }
}


// Typemaps(out) for copies
%typemap(out) vctDouble2x2 {
    vctDynamicMatrix<double>* resultCopy = new vctDynamicMatrix<double>(result.rows(), result.cols());
    resultCopy->Assign(result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrix<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleMat");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultCopy), typeInfo, SWIG_POINTER_DISOWN);
    }
}

%typemap(out) vctInt2x2 {
    vctDynamicMatrix<int>* resultCopy = new vctDynamicMatrix<int>(result.rows(), result.cols());
    resultCopy->Assign(result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrix<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntMat");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultCopy), typeInfo, SWIG_POINTER_DISOWN);
    }
}




// Typemaps(in) for references
%typemap(in) vctDouble2x2 & {
    vctDynamicMatrix<double>* resultPointer = (vctDynamicMatrix<double>*) NULL;
    // convert the input result to a dynamic matrix
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrix<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleMat");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic matrix
    if (($1->rows() != resultPointer->rows()) || ($1->cols() != resultPointer->cols())) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    $1 = reinterpret_cast<$1_ltype>(resultPointer->Pointer());
}


%typemap(in) vctInt2x2 & {
    vctDynamicMatrix<int>* resultPointer = (vctDynamicMatrix<int>*) NULL;
    // convert the input result to a dynamic matrix
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrix<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntMat");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic matrix
    if (($1->rows() != resultPointer->rows()) || ($1->cols() != resultPointer->cols())) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    $1 = reinterpret_cast<$1_ltype>(resultPointer->Pointer());
}


// Typemaps(in) for copies
%typemap(in) vctDouble2x2 {
    vctDynamicMatrix<double>* resultPointer = (vctDynamicMatrix<double>*) NULL;
    // convert the input result to a dynamic matrix
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrix<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleMat");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic matrix
    if (($1.rows() != resultPointer->rows()) || ($1.cols() != resultPointer->cols())) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    // copy to temporary matrix
    $1.Assign(resultPointer->Pointer());
}

%typemap(in) vctInt2x2 {
    vctDynamicMatrix<int>* resultPointer = (vctDynamicMatrix<int>*) NULL;
    // convert the input result to a dynamic matrix
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicMatrix<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntMat");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic matrix
    if (($1.rows() != resultPointer->rows()) || ($1.cols() != resultPointer->cols())) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    // copy to temporary matrix
    $1.Assign(resultPointer->Pointer());
}



// Copy of typemaps
%apply vctDouble2x2 & {vctDouble3x3 &, vctDouble4x4 &, vctDouble5x5 &, vctDouble6x6 &};
%apply vctDouble2x2 {vctDouble3x3, vctDouble4x4, vctDouble5x5, vctDouble6x6};

%apply vctInt2x2 & {vctInt3x3 &, vctInt4x4 &, vctInt5x5 &, vctInt6x6 &};
%apply vctInt2x2 {vctInt3x3, vctInt4x4, vctInt5x5, vctInt6x6};



