/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeVector.i,v 1.7 2007/04/26 19:33:58 anton Exp $

  Author(s):  Anton Deguet
  Created on: 2004-03-29

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


// Typemaps(out) for references
%typemap(out) vctDouble2 & {
    vctDynamicVectorRef<double>* resultRef = new vctDynamicVectorRef<double>(*result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVectorRef<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleVecRef");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultRef), typeInfo, SWIG_POINTER_DISOWN);
    }
}

%typemap(out) vctInt2 & {
    vctDynamicVectorRef<int>* resultRef = new vctDynamicVectorRef<int>(*result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVectorRef<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntVecRef");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultRef), typeInfo, SWIG_POINTER_DISOWN);
    }
}


// Typemaps(out) for copies
%typemap(out) vctDouble2 {
    vctDynamicVector<double>* resultCopy = new vctDynamicVector<double>(result.size());
    resultCopy->Assign(result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVector<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleVec");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultCopy), typeInfo, SWIG_POINTER_DISOWN);
    }
}

%typemap(out) vctInt2 {
    vctDynamicVector<int>* resultCopy = new vctDynamicVector<int>(result.size());
    resultCopy->Assign(result);
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVector<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntVec");
        SWIG_fail;
    } else {
        resultobj = SWIG_NewPointerObj((void*)(resultCopy), typeInfo, SWIG_POINTER_DISOWN);
    }
}





// Typemaps(in) for references
%typemap(in) vctDouble2 & {
    vctDynamicVector<double>* resultPointer = (vctDynamicVector<double>*) NULL;
    // convert the input result to a dynamic vector
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVector<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleVec");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic vector
    if ($1->size() != resultPointer->size()) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    // check the stride of the dynamic vector
    if ($1->stride() != 1) {
        PyErr_SetString(PyExc_RuntimeError, "Stride is not 1");
        SWIG_fail;
    }
    $1 = reinterpret_cast<$1_ltype>(resultPointer->Pointer());
}


%typemap(in) vctInt2 & {
    vctDynamicVector<int>* resultPointer = (vctDynamicVector<int>*) NULL;
    // convert the input result to a dynamic vector
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVector<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntVec");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic vector
    if ($1->size() != resultPointer->size()) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    // check the stride of the dynamic vector
    if ($1->stride() != 1) {
        PyErr_SetString(PyExc_RuntimeError, "Stride is not 1");
        SWIG_fail;
    }
    $1 = reinterpret_cast<$1_ltype>(resultPointer->Pointer());
}




// Typemaps(in) for copies
%typemap(in) vctDouble2 {
    vctDynamicVector<double>* resultPointer = (vctDynamicVector<double>*) NULL;
    // convert the input result to a dynamic vector
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVector<double> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctDoubleVec");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic vector
    if ($1.size() != resultPointer->size()) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    // copy to temporary vector
    $1.Assign(resultPointer->Pointer());
}

%typemap(in) vctInt2 {
    vctDynamicVector<int>* resultPointer = (vctDynamicVector<int>*) NULL;
    // convert the input result to a dynamic vector
    swig_type_info* typeInfo = SWIG_TypeQuery("vctDynamicVector<int> *");
    if (typeInfo == NULL) {
        PyErr_SetString(PyExc_TypeError, "Sorry, can't create a python object of type vctIntVec");
        SWIG_fail;
    }
    SWIG_Python_ConvertPtr($input, (void **)&resultPointer, typeInfo, SWIG_POINTER_EXCEPTION | 0);
    if (SWIG_arg_fail(1)) {
        SWIG_fail;
    }
    // check the size of the dynamic vector
    if ($1.size() != resultPointer->size()) {
        PyErr_SetString(PyExc_RuntimeError, "Size mismatched");
        SWIG_fail;
    }
    // copy to temporary vector
    $1.Assign(resultPointer->Pointer());
}



// Copy of typemaps
%apply vctDouble2 & {vctDouble3 &, vctDouble4 &, vctDouble5 &, vctDouble6 &, vctDouble7 &, vctDouble8 &, vctDouble9 &, vctDouble10 &};
%apply vctDouble2 {vctDouble3, vctDouble4, vctDouble5, vctDouble6, vctDouble7, vctDouble8, vctDouble9, vctDouble10};

%apply vctInt2 & {vctInt3 &, vctInt4 &, vctInt5 &, vctInt6 &, vctInt7 &, vctInt8 &, vctInt9 &, vctInt10 &};
%apply vctInt2 {vctInt3, vctInt4, vctInt5, vctInt6, vctInt7, vctInt8, vctInt9, vctInt10};


