/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Daniel Li, Anton Deguet
  Created on: 2009-05-20

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _vctDynamicMatrixTypemaps_i
#define _vctDynamicMatrixTypemaps_i

/*****************************************************************************
 PLACEHOLDER STRINGS TO LOOK FOR:

   TODO
*****************************************************************************/

#define CISST_EXPORT
#define CISST_DEPRECATED

%include "std_streambuf.i"
%include "std_iostream.i"

%rename(__str__) ToString;


%init %{
        import_array()   /* Initial function for NumPy */
%}


/******************************************************************************
  TYPEMAPS (in, out) FOR vctDynamicMatrix
******************************************************************************/


%typemap(in) vctDynamicMatrix
{
    /*****************************************************************************
    *   %typemap(in) vctDynamicMatrix
    *   Passing a vctDynamicMatrix by copy
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this type map.
    *****************************************************************************/

    /*****************************************************************************
     CHECK IF THE PYTHON OBJECT (NAMED `$input') THAT WAS PASSED TO THIS TYPE MAP
     IS A PYARRAY, IS OF THE CORRECT DTYPE, AND IS TWO-DIMENSIONAL
    *****************************************************************************/

    if (!(   vctThrowUnlessIsPyArray($input)
          && vctThrowUnlessIsSameTypeArray<$1_ltype::value_type>($input)
          && vctThrowUnlessDimension2($input)
          && vctThrowUnlessCorrectMatrixSize($input, $1))
        ) {
          return NULL;
    }

    /*****************************************************************************
     COPY THE DATA OF THE PYARRAY (NAMED `$input') TO THE vctDynamicMatrix
    *****************************************************************************/

    // Create a temporary vctDynamicMatrixRef container
    const npy_intp size0 = PyArray_DIM($input, 0);
    const npy_intp size1 = PyArray_DIM($input, 1);
    const npy_intp stride0 = PyArray_STRIDE($input, 0) / sizeof($1_ltype::value_type);
    const npy_intp stride1 = PyArray_STRIDE($input, 1) / sizeof($1_ltype::value_type);
    const $1_ltype::pointer data = reinterpret_cast<$1_ltype::pointer>(PyArray_DATA($input));

    const vctDynamicMatrixRef<$1_ltype::value_type> tempContainer(size0, size1, stride0, stride1, data);

    // Copy the data from the temporary container to the vctDynamicMatrix
    $1.ForceAssign(tempContainer);
}


// TODO: Search for other uses of %typemap(out, optimal="1")


%typemap(out) vctDynamicMatrix
{
    /*****************************************************************************
    *   %typemap(out) vctDynamicMatrix
    *   Return a vctDynamicMatrix by copy
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this type map.
    *****************************************************************************/

    /*****************************************************************************
     CREATE A NEW PYARRAY OBJECT
    *****************************************************************************/

    npy_intp *sizes = PyDimMem_NEW(2);
    sizes[0] = $1.rows();
    sizes[1] = $1.cols();
    int type = vctPythonType<$1_ltype::value_type>();
    $result = PyArray_SimpleNew(2, sizes, type);

    /*****************************************************************************
     COPY THE DATA FROM THE vctDynamicConstMatrixRef TO THE PYARRAY
    *****************************************************************************/

    // Create a temporary vctDynamicMatrixRef container
    const npy_intp size0 = PyArray_DIM($result, 0);
    const npy_intp size1 = PyArray_DIM($result, 1);
    const npy_intp stride0 = PyArray_STRIDE($result, 0) / sizeof($1_ltype::value_type);
    const npy_intp stride1 = PyArray_STRIDE($result, 1) / sizeof($1_ltype::value_type);
    const $1_ltype::pointer data = reinterpret_cast<$1_ltype::pointer>(PyArray_DATA($result));

    vctDynamicMatrixRef<$1_ltype::value_type> tempContainer(size0, size1, stride0, stride1, data);

    // Copy the data from the vctDynamicConstMatrixRef to the temporary container
    tempContainer.Assign($1);
}


/******************************************************************************
  TYPEMAPS (in, argout, out) FOR vctDynamicMatrix &
******************************************************************************/


%typemap(in) vctDynamicMatrix &
{
    /*****************************************************************************
    *   %typemap(in) vctDynamicMatrix &
    *   Passing a vctDynamicMatrix by reference
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this typemap.
    *****************************************************************************/

    /*****************************************************************************
     CHECK IF THE PYTHON OBJECT (NAMED `$input') THAT WAS PASSED TO THIS TYPE MAP
     IS A PYARRAY, IS OF THE CORRECT DTYPE, IS ONE-DIMENSIONAL, AND IS WRITABLE
    *****************************************************************************/

    if (!(   vctThrowUnlessIsPyArray($input)
          && vctThrowUnlessIsSameTypeArray<$*1_ltype::value_type>($input)
          && vctThrowUnlessDimension2($input)
          && vctThrowUnlessCorrectMatrixSize($input, *($1))
          && vctThrowUnlessIsWritable($input)
          && vctThrowUnlessOwnsData($input, *($1))
          && vctThrowUnlessNotReferenced($input, *($1)))
        ) {
          return NULL;
    }

    /*****************************************************************************
     COPY THE DATA OF THE PYARRAY (NAMED `$input') TO THE vctDynamicMatrix
    *****************************************************************************/

    // Create a temporary vctDynamicMatrixRef container
    const npy_intp size0 = PyArray_DIM($input, 0);
    const npy_intp size1 = PyArray_DIM($input, 1);
    const npy_intp stride0 = PyArray_STRIDE($input, 0) / sizeof($*1_ltype::value_type);
    const npy_intp stride1 = PyArray_STRIDE($input, 1) / sizeof($*1_ltype::value_type);
    const $*1_ltype::pointer data = reinterpret_cast<$*1_ltype::pointer>(PyArray_DATA($input));

    const vctDynamicMatrixRef<$*1_ltype::value_type> tempContainer(size0, size1, stride0, stride1, data);

    // Create the vctDynamicMatrix
    $1 = new $*1_ltype(tempContainer);
}


%typemap(argout) vctDynamicMatrix &
{
    /*****************************************************************************
    *   %typemap(argout) vctDynamicMatrix &
    *   Passing a vctDynamicMatrix by reference
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this type map.
    *****************************************************************************/

    /*************************************************************************
     CHECK IF THE CONTAINER HAS BEEN RESIZED
    *************************************************************************/

    const $*1_ltype::size_type input_size0 = PyArray_DIM($input, 0);
    const $*1_ltype::size_type input_size1 = PyArray_DIM($input, 1);
    const $*1_ltype::size_type output_size0 = $1->sizes()[0];
    const $*1_ltype::size_type output_size1 = $1->sizes()[1];

    if (   input_size0 != output_size0
        || input_size1 != output_size1) {
        // Resize the PyArray by:
        //  1)  Creating an array containing the new size
        //  2)  Pass that array to the resizing function given by NumPy API

        npy_intp *sizes = PyDimMem_NEW(2);              // create an array of sizes; dimension 2
        sizes[0] = output_size0;                        // set the size
        sizes[1] = output_size1;
        PyArray_Dims dims;                              // create a PyArray_Dims object to hand to PyArray_Resize
        dims.ptr = sizes;
        dims.len = 2;
        PyArray_Resize((PyArrayObject *) $input, &dims, 0, NPY_CORDER);
    }

    /*************************************************************************
     COPY THE DATA TO THE PYARRAY
    *************************************************************************/

    // Create a temporary vctDynamicMatrixRef container
    const npy_intp size0 = PyArray_DIM($input, 0);
    const npy_intp size1 = PyArray_DIM($input, 1);
    const npy_intp stride0 = PyArray_STRIDE($input, 0) / sizeof($*1_ltype::value_type);
    const npy_intp stride1 = PyArray_STRIDE($input, 1) / sizeof($*1_ltype::value_type);
    const $*1_ltype::pointer data = reinterpret_cast<$*1_ltype::pointer>(PyArray_DATA($input));

    vctDynamicMatrixRef<$*1_ltype::value_type> tempContainer(size0, size1, stride0, stride1, data);

    // Copy the data from the temporary container to the vctDynamicMatrix
    tempContainer.Assign(*($1));

    /*************************************************************************
     CLEAN UP
    *************************************************************************/

    // Don't forget to free the memory we allocated in the `in' typemap
    delete $1;
}


%typemap(out) vctDynamicMatrix &
{
    /* Return vector by reference
       Using: %typemap(out) vctDynamicMatrix &
     */

    // Create new size array and set size
    npy_intp *sizes = PyDimMem_NEW(2);
    sizes[0] = $1->rows();
    sizes[1] = $1->cols();

    // Look at the NumPy C API to see how these lines work: http://projects.scipy.org/numpy/wiki/NumPyCAPI
    int type = vctPythonType<$*1_ltype::value_type>();
    PyArray_Descr *descr = PyArray_DescrFromType(type);
    $result = PyArray_NewFromDescr(&PyArray_Type, descr, 2, sizes, NULL, $1->Pointer(), $1->StorageOrder() ? NPY_CARRAY : NPY_FARRAY, NULL);
}


/******************************************************************************
  TYPEMAPS (in, argout, out) FOR const vctDynamicMatrix &
******************************************************************************/


%typemap(in) const vctDynamicMatrix &
{
    /*****************************************************************************
    *   %typemap(in) const vctDynamicMatrix &
    *   Passing a vctDynamicMatrix by const &
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this type map.
    *****************************************************************************/

    /*****************************************************************************
     CHECK IF THE PYTHON OBJECT (NAMED `$input') THAT WAS PASSED TO THIS TYPE MAP
     IS A PYARRAY, IS OF THE CORRECT DTYPE, AND IS ONE-DIMENSIONAL
    *****************************************************************************/

    if (!(   vctThrowUnlessIsPyArray($input)
          && vctThrowUnlessIsSameTypeArray<$*1_ltype::value_type>($input)
          && vctThrowUnlessDimension2($input)
          && vctThrowUnlessCorrectMatrixSize($input, *($1)))
        ) {
          return NULL;
    }

    /*****************************************************************************
     COPY THE DATA OF THE PYARRAY (NAMED `$input') TO THE vctDynamicMatrix
    *****************************************************************************/

    // Create a temporary vctDynamicMatrixRef container
    const npy_intp size0 = PyArray_DIM($input, 0);
    const npy_intp size1 = PyArray_DIM($input, 1);
    const npy_intp stride0 = PyArray_STRIDE($input, 0) / sizeof($*1_ltype::value_type);
    const npy_intp stride1 = PyArray_STRIDE($input, 1) / sizeof($*1_ltype::value_type);
    const $*1_ltype::pointer data = reinterpret_cast<$*1_ltype::pointer>(PyArray_DATA($input));

    const vctDynamicMatrixRef<$*1_ltype::value_type> tempContainer(size0, size1, stride0, stride1, data);

    // Create the vctDynamicMatrix
    $1 = new $*1_ltype(tempContainer);
}


%typemap(argout) const vctDynamicMatrix &
{
    /**************************************************************************
    *   %typemap(argout) const vctDynamicMatrix &
    *   Passing a vctDynamicMatrix by const reference
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this type map.
    **************************************************************************/

    /**************************************************************************
     CLEAN UP
    **************************************************************************/

    // Don't forget to free the memory we allocated in the `in' typemap
    delete $1;
}


%typemap(out) const vctDynamicMatrix &
{
    /* Return vector by const reference
       Using: %typemap(out) const vctDynamicMatrix &
     */

    // Create new size array and set size
    npy_intp *sizes = PyDimMem_NEW(2);
    sizes[0] = $1->rows();
    sizes[1] = $1->cols();

    // Look at the NumPy C API to see how these lines work: http://projects.scipy.org/numpy/wiki/NumPyCAPI
    int type = vctPythonType<$*1_ltype::value_type>();
    PyArray_Descr *descr = PyArray_DescrFromType(type);
    $result = PyArray_NewFromDescr(&PyArray_Type, descr, 2, sizes, NULL, $1->Pointer(), NPY_CARRAY_RO, NULL);
}


/******************************************************************************
  TYPEMAPS (in, out) FOR vctDynamicMatrixRef
******************************************************************************/


%typemap(in) vctDynamicMatrixRef
{
    /*************************************************************************
    *   %typemap(in) vctDynamicMatrixRef
    *   Passing a vctDynamicMatrixRef by copy
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic
    *   behind this typemap.
    *************************************************************************/

    /*************************************************************************
     CHECK IF THE PYTHON OBJECT (NAMED `$input') THAT WAS PASSED TO THIS
     TYPEMAP IS A PYARRAY, IS OF THE CORRECT DTYPE, IS ONE-DIMENSIONAL, AND
     IS WRITABLE
    *************************************************************************/

    if (!(   vctThrowUnlessIsPyArray($input)
          && vctThrowUnlessIsSameTypeArray<$1_ltype::value_type>($input)
          && vctThrowUnlessDimension2($input)
          && vctThrowUnlessCorrectMatrixSize($input, $1)
          && vctThrowUnlessIsWritable($input))
        ) {
          return NULL;
    }

    /*************************************************************************
     SET THE SIZE, STRIDE AND DATA POINTER OF THE vctDynamicMatrixRef
     OBJECT (NAMED `$1') TO MATCH THAT OF THE PYARRAY (NAMED `$input')
    *************************************************************************/

    const npy_intp size0 = PyArray_DIM($input, 0);
    const npy_intp size1 = PyArray_DIM($input, 1);
    const npy_intp stride0 = PyArray_STRIDE($input, 0) / sizeof($1_ltype::value_type);
    const npy_intp stride1 = PyArray_STRIDE($input, 1) / sizeof($1_ltype::value_type);
    const $1_ltype::pointer data = reinterpret_cast<$1_ltype::pointer>(PyArray_DATA($input));

    $1.SetRef(size0, size1, stride0, stride1, data);
}


%typemap(out, optimal="1") vctDynamicMatrixRef
{
    /*****************************************************************************
    *   %typemap(out, optimal="1") vctDynamicMatrixRef
    *   Returning a vctDynamicMatrixRef
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this type map.
    *****************************************************************************/

    /*****************************************************************************
     CREATE A NEW PYARRAY OBJECT
    *****************************************************************************/

    npy_intp *sizes = PyDimMem_NEW(2);
    sizes[0] = $1.rows();
    sizes[1] = $1.cols();
    int type = vctPythonType<$1_ltype::value_type>();
    $result = PyArray_SimpleNew(2, sizes, type);

    /*****************************************************************************
     COPY THE DATA FROM THE vctDynamicMatrixRef TO THE PYARRAY
    *****************************************************************************/

    // Create a temporary vctDynamicMatrixRef container
    const npy_intp size0 = $1.rows();
    const npy_intp size1 = $1.cols();
    const npy_intp stride0 = size1;
    const npy_intp stride1 = 1;
    const $1_ltype::pointer data = reinterpret_cast<$1_ltype::pointer>(PyArray_DATA($result));

    vctDynamicMatrixRef<$1_ltype::value_type> tempContainer(size0, size1, stride0, stride1, data);

    // Copy the data from the vctDynamicMatrixRef to the temporary container
    tempContainer.Assign($1);
}


/******************************************************************************
  TYPEMAPS (in, argout) FOR const vctDynamicMatrixRef &
******************************************************************************/


%typemap(in) const vctDynamicMatrixRef &
{
    /**************************************************************************
    *   %typemap(in) const vctDynamicMatrixRef &
    *   Passing a vctDynamicMatrixRef by const reference
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this type map.
    **************************************************************************/

    /*****************************************************************************
     CHECK IF THE PYTHON OBJECT (NAMED `$input') THAT WAS PASSED TO THIS TYPE MAP
     IS A PYARRAY, IS OF THE CORRECT DTYPE, AND IS ONE-DIMENSIONAL
    *****************************************************************************/

    if (!(   vctThrowUnlessIsPyArray($input)
          && vctThrowUnlessIsSameTypeArray<$*1_ltype::value_type>($input)
          && vctThrowUnlessDimension2($input)
          && vctThrowUnlessCorrectMatrixSize($input, *($1)))
        ) {
          return NULL;
    }

    /*****************************************************************************
     CREATE A vctDynamicMatrixRef TO POINT TO THE DATA OF THE PYARRAY
    *****************************************************************************/

    // Create the vctDynamicMatrixRef
    const npy_intp size0 = PyArray_DIM($input, 0);
    const npy_intp size1 = PyArray_DIM($input, 1);
    const npy_intp stride0 = PyArray_STRIDE($input, 0) / sizeof($*1_ltype::value_type);
    const npy_intp stride1 = PyArray_STRIDE($input, 1) / sizeof($*1_ltype::value_type);
    const $*1_ltype::pointer data = reinterpret_cast<$*1_ltype::pointer>(PyArray_DATA($input));

    $1 = new $*1_ltype(size0, size1, stride0, stride1, data);
}


%typemap(argout) const vctDynamicMatrixRef &
{
    /**************************************************************************
    *   %typemap(argout) const vctDynamicMatrixRef &
    *   Passing a vctDynamicMatrixRef by const reference
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this type map.
    **************************************************************************/

    /**************************************************************************
     CLEAN UP
    **************************************************************************/

    // Don't forget to free the memory we allocated in the `in' typemap
    delete $1;
}


/******************************************************************************
  TYPEMAPS (in, out) FOR vctDynamicConstMatrixRef
******************************************************************************/


%typemap(in) vctDynamicConstMatrixRef
{
    /*****************************************************************************
    *   %typemap(in) vctDynamicConstMatrixRef
    *   Passing a vctDynamicConstMatrixRef by copy
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this type map.
    *****************************************************************************/

    /*****************************************************************************
     CHECK IF THE PYTHON OBJECT (NAMED `$input') THAT WAS PASSED TO THIS TYPE MAP
     IS A PYARRAY, IS OF THE CORRECT DTYPE, AND IS ONE-DIMENSIONAL
    *****************************************************************************/

    if (!(   vctThrowUnlessIsPyArray($input)
          && vctThrowUnlessIsSameTypeArray<$1_ltype::value_type>($input)
          && vctThrowUnlessDimension2($input)
          && vctThrowUnlessCorrectMatrixSize($input, $1))
        ) {
          return NULL;
    }

    /*****************************************************************************
     SET THE SIZE, STRIDE, AND DATA POINTER OF THE
     vctDynamicConstMatrixRef OBJECT (NAMED `$1') TO MATCH THAT OF THE
     PYARRAY (NAMED `$input')
    *****************************************************************************/

    const npy_intp size0 = PyArray_DIM($input, 0);
    const npy_intp size1 = PyArray_DIM($input, 1);
    const npy_intp stride0 = PyArray_STRIDE($input, 0) / sizeof($1_ltype::value_type);
    const npy_intp stride1 = PyArray_STRIDE($input, 1) / sizeof($1_ltype::value_type);
    const $1_ltype::pointer data = reinterpret_cast<$1_ltype::pointer>(PyArray_DATA($input));

    $1.SetRef(size0, size1, stride0, stride1, data);
}


// TODO: Why does this out typemap work without the ``optimal'' flag?
%typemap(out) vctDynamicConstMatrixRef
{
    /*****************************************************************************
    *   %typemap(out) vctDynamicConstMatrixRef
    *   Returning a vctDynamicConstMatrixRef
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this type map.
    *****************************************************************************/

    /*****************************************************************************
     CREATE A NEW PYARRAY OBJECT
    *****************************************************************************/

    npy_intp *sizes = PyDimMem_NEW(2);
    sizes[0] = $1.rows();
    sizes[1] = $1.cols();

    // Look at the NumPy C API to see how these lines work: http://projects.scipy.org/numpy/wiki/NumPyCAPI
    int type = vctPythonType<$1_ltype::value_type>();
    PyArray_Descr *descr = PyArray_DescrFromType(type);
    $result = PyArray_NewFromDescr(&PyArray_Type, descr,  2, sizes, NULL, NULL, NPY_CARRAY, NULL);
    PyArray_FLAGS($result) &= ~NPY_WRITEABLE;

    /*****************************************************************************
     COPY THE DATA FROM THE vctDynamicConstMatrixRef TO THE PYARRAY
    *****************************************************************************/

    // Create a temporary vctDynamicMatrixRef container
    const npy_intp size0 = PyArray_DIM($result, 0);
    const npy_intp size1 = PyArray_DIM($result, 1);
    const npy_intp stride0 = PyArray_STRIDE($result, 0) / sizeof($1_ltype::value_type);
    const npy_intp stride1 = PyArray_STRIDE($result, 1) / sizeof($1_ltype::value_type);
    const $1_ltype::pointer data = reinterpret_cast<$1_ltype::pointer>(PyArray_DATA($result));

    vctDynamicMatrixRef<$1_ltype::value_type> tempContainer(size0, size1, stride0, stride1, data);

    // Copy the data from the vctDynamicConstMatrixRef to the temporary container
    tempContainer.Assign($1);
}


/******************************************************************************
  TYPEMAPS (in, argout) FOR const vctDynamicConstMatrixRef &
******************************************************************************/


%typemap(in) const vctDynamicConstMatrixRef &
{
    /**************************************************************************
    *   %typemap(in) const vctDynamicConstMatrixRef &
    *   Passing a vctDynamicConstMatrixRef by const reference
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this type map.
    **************************************************************************/

    /*****************************************************************************
     CHECK IF THE PYTHON OBJECT (NAMED `$input') THAT WAS PASSED TO THIS TYPE MAP
     IS A PYARRAY, IS OF THE CORRECT DTYPE, AND IS ONE-DIMENSIONAL
    *****************************************************************************/

    if (!(   vctThrowUnlessIsPyArray($input)
          && vctThrowUnlessIsSameTypeArray<$*1_ltype::value_type>($input)
          && vctThrowUnlessDimension2($input)
          && vctThrowUnlessCorrectMatrixSize($input, *($1)))
        ) {
          return NULL;
    }

    /*****************************************************************************
     CREATE A vctDynamicConstMatrixRef TO POINT TO THE DATA OF THE PYARRAY
    *****************************************************************************/

    // Create the vctDynamicConstMatrixRef
    const npy_intp size0 = PyArray_DIM($input, 0);
    const npy_intp size1 = PyArray_DIM($input, 1);
    const npy_intp stride0 = PyArray_STRIDE($input, 0) / sizeof($*1_ltype::value_type);
    const npy_intp stride1 = PyArray_STRIDE($input, 1) / sizeof($*1_ltype::value_type);
    const $*1_ltype::pointer data = reinterpret_cast<$*1_ltype::pointer>(PyArray_DATA($input));

    $1 = new $*1_ltype(size0, size1, stride0, stride1, data);
}


%typemap(argout) const vctDynamicConstMatrixRef &
{
    /**************************************************************************
    *   %typemap(argout) const vctDynamicConstMatrixRef &
    *   Passing a vctDynamicConstMatrixRef by const reference
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this type map.
    **************************************************************************/

    /**************************************************************************
     CLEAN UP
    **************************************************************************/

    // Don't forget to free the memory we allocated in the `in' typemap
    delete $1;
}


/**************************************************************************
*                           Applying Typemaps
**************************************************************************/

%define VCT_TYPEMAPS_APPLY_DYNAMIC_MATRICES(elementType)
%apply vctDynamicMatrix         {vctDynamicMatrix<elementType>};
%apply vctDynamicMatrix &       {vctDynamicMatrix<elementType> &};
%apply const vctDynamicMatrix & {const vctDynamicMatrix<elementType> &};
%apply vctDynamicMatrixRef         {vctDynamicMatrixRef<elementType>};
%apply const vctDynamicMatrixRef & {const vctDynamicMatrixRef<elementType> &};
%apply vctDynamicConstMatrixRef         {vctDynamicConstMatrixRef<elementType>};
%apply const vctDynamicConstMatrixRef & {const vctDynamicConstMatrixRef<elementType> &};
%enddef

VCT_TYPEMAPS_APPLY_DYNAMIC_MATRICES(char);
VCT_TYPEMAPS_APPLY_DYNAMIC_MATRICES(unsigned char);
VCT_TYPEMAPS_APPLY_DYNAMIC_MATRICES(short);
VCT_TYPEMAPS_APPLY_DYNAMIC_MATRICES(unsigned short);
VCT_TYPEMAPS_APPLY_DYNAMIC_MATRICES(int);
VCT_TYPEMAPS_APPLY_DYNAMIC_MATRICES(unsigned int);
VCT_TYPEMAPS_APPLY_DYNAMIC_MATRICES(long long int);
VCT_TYPEMAPS_APPLY_DYNAMIC_MATRICES(unsigned long long int);
VCT_TYPEMAPS_APPLY_DYNAMIC_MATRICES(double);

%apply vctDynamicMatrix         {vctDoubleMat, vctMat, vctIntMat};
%apply vctDynamicMatrix &       {vctDoubleMat &, vctMat &, vctIntMat &};
%apply const vctDynamicMatrix & {const vctDoubleMat &, const vctMat &, const vctIntMat &};

#endif // _vctDynamicMatrixTypemaps_i
