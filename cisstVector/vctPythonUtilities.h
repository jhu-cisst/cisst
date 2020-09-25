/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2005-08-21

  (C) Copyright 2005-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/* This file is to be used only for the generation of SWIG wrappers.
   It includes all the regular header files from the libraries as well
   as some header files created only for the wrapping process
   (e.g. vctDynamicMatrixRotation3.h).

   For any wrapper using %import "cisstVector.i", the file
   cisstVector/vctPython.h should be included in the %header %{ ... %}
   section of the interface file. */


#ifndef _vctPythonUtilities_h
#define _vctPythonUtilities_h

/* Put header files here */
#include <Python.h>
#include <numpyconfig.h>
// Not using anything deprecated as of Numpy Version 1.9.
// Newer versions of Numpy not yet tested, but based on numpyconfig.h, no new deprecated
// items were added between Numpy Versions 1.8 and 1.15.
#if defined(NPY_1_8_API_VERSION)
#define NPY_NO_DEPRECATED_API NPY_1_8_API_VERSION
#else
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#endif
#include <arrayobject.h>
// Numpy 1.7+ API requires PyArrayObject instead of PyObject for a number of methods.
// The following macro performs this cast, without checking that the underlying object (A)
// is an array because the code always either first checks if it is an array (e.g.,
// by calling vctThrowUnlessIsPyArray, which calls PyArray_Check) or has just created
// an array (e.g., by calling PyArray_SimpleNew).
#define cast_array(A) reinterpret_cast<PyArrayObject *>(A)

#include <typeinfo>
#include <cisstConfig.h>
#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnPortability.h>
#include <cisstVector/vctFixedSizeConstVectorBase.h>
#include <cisstVector/vctDynamicConstVectorBase.h>
#include <cisstVector/vctFixedSizeConstMatrixBase.h>
#include <cisstVector/vctDynamicConstMatrixBase.h>
#include <cisstVector/vctDynamicConstNArrayBase.h>
#include <cisstVector/vctTransformationTypes.h>


bool vctThrowUnlessIsPyArray(PyObject * input)
{
    if (!PyArray_Check(input)) {
        PyErr_SetString(PyExc_TypeError, "Object must be a NumPy array");
        return false;
    }
    return true;
}


template <class _elementType>
bool vctThrowUnlessIsSameTypeArray(PyObject * CMN_UNUSED(input))
{
    PyErr_Format(PyExc_ValueError, "Unsupported data type: %s", typeid(_elementType).name());
    return false;
}


template <>
bool vctThrowUnlessIsSameTypeArray<bool>(PyObject * input)
{
    if (PyArray_ObjectType(input, 0) != NPY_BOOL) {
        PyErr_SetString(PyExc_ValueError, "Array must be of type bool");
        return false;
    }
    return true;
}


template <>
bool vctThrowUnlessIsSameTypeArray<char>(PyObject * input)
{
    if (PyArray_ObjectType(input, 0) != NPY_INT8) {
        PyErr_SetString(PyExc_ValueError, "Array must be of type char (int8)");
        return false;
    }
    return true;
}


template <>
bool vctThrowUnlessIsSameTypeArray<unsigned char>(PyObject * input)
{
    if (PyArray_ObjectType(input, 0) != NPY_UINT8) {
        PyErr_SetString(PyExc_ValueError, "Array must be of type unsigned char (uint8)");
        return false;
    }
    return true;
}


template <>
bool vctThrowUnlessIsSameTypeArray<short>(PyObject * input)
{
    if (PyArray_ObjectType(input, 0) != NPY_INT16) {
        PyErr_SetString(PyExc_ValueError, "Array must be of type short (int16)");
        return false;
    }
    return true;
}


template <>
bool vctThrowUnlessIsSameTypeArray<unsigned short>(PyObject * input)
{
    if (PyArray_ObjectType(input, 0) != NPY_UINT16) {
        PyErr_SetString(PyExc_ValueError, "Array must be of type unsigned short (uint16)");
        return false;
    }
    return true;
}


template <>
bool vctThrowUnlessIsSameTypeArray<int>(PyObject * input)
{
    // NPY_INT and NPY_LONG are considered different types; NPY_INT32 is an alias for one of these
#if (CISST_DATA_MODEL == CISST_ILP32) || (CISST_DATA_MODEL == CISST_LLP64)
    if ((PyArray_ObjectType(input, 0) != NPY_INT) && (PyArray_ObjectType(input, 0) != NPY_LONG))
#else
    if (PyArray_ObjectType(input, 0) != NPY_INT)
#endif
    {
        PyErr_SetString(PyExc_ValueError, "Array must be of type int (int32)");
        return false;
    }
    return true;
}


template <>
bool vctThrowUnlessIsSameTypeArray<unsigned int>(PyObject * input)
{
    // NPY_UINT and NPY_ULONG are considered different types; NPY_UINT32 is an alias for one of these
#if (CISST_DATA_MODEL == CISST_ILP32) || (CISST_DATA_MODEL == CISST_LLP64)
    if ((PyArray_ObjectType(input, 0) != NPY_UINT) && (PyArray_ObjectType(input, 0) != NPY_ULONG))
#else
    if (PyArray_ObjectType(input, 0) != NPY_UINT)
#endif
    {
        PyErr_SetString(PyExc_ValueError, "Array must be of type unsigned int (uint32)");
        return false;
    }
    return true;
}


template <>
bool vctThrowUnlessIsSameTypeArray<long int>(PyObject * input)
{
#if (CISST_DATA_MODEL == CISST_ILP32) || (CISST_DATA_MODEL == CISST_LLP64)
    if (PyArray_ObjectType(input, 0) != NPY_INT32) {
        PyErr_SetString(PyExc_ValueError, "Array must be of type long int (int32 on this platform)");
        return false;
    }
#else
    if (PyArray_ObjectType(input, 0) != NPY_INT64) {
        PyErr_SetString(PyExc_ValueError, "Array must be of type long int (int64 on this platform)");
        return false;
    }
#endif
    return true;
}


template <>
bool vctThrowUnlessIsSameTypeArray<unsigned long int>(PyObject * input)
{
#if (CISST_DATA_MODEL == CISST_ILP32) || (CISST_DATA_MODEL == CISST_LLP64)
    if (PyArray_ObjectType(input, 0) != NPY_UINT32) {
        PyErr_SetString(PyExc_ValueError, "Array must be of type unsigned long int (uint32 on this platform)");
        return false;
    }
#else
    if (PyArray_ObjectType(input, 0) != NPY_UINT64) {
        PyErr_SetString(PyExc_ValueError, "Array must be of type unsigned long int (uint64 on this platform)");
        return false;
    }
#endif
    return true;
}


#if CISST_LONG_LONG_NATIVE
template <>
bool vctThrowUnlessIsSameTypeArray<long long int>(PyObject * input)
{
    if (PyArray_ObjectType(input, 0) != NPY_INT64) {
        PyErr_SetString(PyExc_ValueError, "Array must be of type long long int (int64)");
        return false;
    }
    return true;
}

template <>
bool vctThrowUnlessIsSameTypeArray<unsigned long long int>(PyObject * input)
{
    if (PyArray_ObjectType(input, 0) != NPY_UINT64) {
        PyErr_SetString(PyExc_ValueError, "Array must be of type unsigned long long int (uint64)");
        return false;
    }
    return true;
}
#endif

#if CISST_SIZE_T_NATIVE
template <>
bool vctThrowUnlessIsSameTypeArray<size_t>(PyObject * input)
{
    if (PyArray_ObjectType(input, 0) != NPY_UINT64) {
        PyErr_SetString(PyExc_ValueError, "Array must be of type size_t (uint64)");
        return false;
    }
    return true;
}
#endif

template <>
bool vctThrowUnlessIsSameTypeArray<float>(PyObject * input)
{
    if (PyArray_ObjectType(input, 0) != NPY_FLOAT) {
        PyErr_SetString(PyExc_ValueError, "Array must be of type float");
        return false;
    }
    return true;
}


template <>
bool vctThrowUnlessIsSameTypeArray<double>(PyObject * input)
{
    if (PyArray_ObjectType(input, 0) != NPY_DOUBLE) {
        PyErr_SetString(PyExc_ValueError, "Array must be of type double");
        return false;
    }
    return true;
}


template <class _elementType>
int vctPythonType(void)
{
    return NPY_NOTYPE; // unsupported type
}


template <>
int vctPythonType<bool>(void)
{
    return NPY_BOOL;
}


template <>
int vctPythonType<char>(void)
{
    return NPY_INT8;
}


template <>
int vctPythonType<unsigned char>(void)
{
    return NPY_UINT8;
}


template <>
int vctPythonType<short>(void)
{
    return NPY_INT16;
}


template <>
int vctPythonType<unsigned short>(void)
{
    return NPY_UINT16;
}


template <>
int vctPythonType<int>(void)
{
    return NPY_INT32;
}


template <>
int vctPythonType<unsigned int>(void)
{
    return NPY_UINT32;
}


#if (CISST_DATA_MODEL == CISST_ILP32) || (CISST_DATA_MODEL == CISST_LLP64)

template <>
int vctPythonType<long int>(void)
{
    return NPY_INT32;
}


template <>
int vctPythonType<unsigned long int>(void)
{
    return NPY_UINT32;
}

#else

template <>
int vctPythonType<long int>(void)
{
    return NPY_INT64;
}


template <>
int vctPythonType<unsigned long int>(void)
{
    return NPY_UINT64;
}

#endif

#if CISST_LONG_LONG_NATIVE
template <>
int vctPythonType<long long int>(void)
{
    return NPY_INT64;
}


template <>
int vctPythonType<unsigned long long int>(void)
{
    return NPY_UINT64;
}
#endif

#if CISST_SIZE_T_NATIVE

template <>
int vctPythonType<size_t>(void)
{
    return NPY_UINT64;
}

#endif

template <>
int vctPythonType<double>(void)
{
    return NPY_DOUBLE;
}


bool vctThrowUnlessDimension1(PyArrayObject * input)
{
    if (PyArray_NDIM(input) != 1) {
        PyErr_SetString(PyExc_ValueError, "Array must be 1D (vector)");
        return false;
    }
    return true;
}


bool vctThrowUnlessDimension2(PyArrayObject * input)
{
    if (PyArray_NDIM(input) != 2) {
        PyErr_SetString(PyExc_ValueError, "Array must be 2D (matrix)");
        return false;
    }
    return true;
}


template <vct::size_type _dimension>
bool vctThrowUnlessDimensionN(PyArrayObject * input)
{
    if (PyArray_NDIM(input) != _dimension) {
        std::stringstream stream;
        stream << "Array must have " << _dimension << " dimension(s)";
        std::string msg = stream.str();
        PyErr_SetString(PyExc_ValueError, msg.c_str());
        return false;
    }
    return true;
}


bool vctThrowUnlessIsWritable(PyArrayObject * input)
{
    const int flags = PyArray_FLAGS(input);
    if(!(flags & NPY_ARRAY_WRITEABLE)) {
        PyErr_SetString(PyExc_ValueError, "Array must be writable");
        return false;
    }
    return true;
}


template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
bool vctThrowUnlessCorrectVectorSize(PyArrayObject * input,
                                     const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & target)
{
    const vct::size_type inputSize = PyArray_DIM(input, 0);
    const vct::size_type targetSize = target.size();
    if (inputSize != targetSize) {
        std::stringstream stream;
        stream << "Input vector's size must be " << targetSize;
        std::string msg = stream.str();
        PyErr_SetString(PyExc_ValueError, msg.c_str());
        return false;
    }
    return true;
}


template <class _vectorOwnerType, typename _elementType>
bool vctThrowUnlessCorrectVectorSize(PyArrayObject * CMN_UNUSED(input),
                                     const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & CMN_UNUSED(target))
{
    return true;
}


template <vct::size_type _rows, vct::size_type _cols,
          vct::stride_type _rowStride, vct::stride_type _colStride,
          class _elementType, class _dataPtrType>
bool vctThrowUnlessCorrectMatrixSize(PyArrayObject * input,
                                     const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & target)
{
    const vct::size_type inputRows = PyArray_DIM(input, 0);
    const vct::size_type inputCols = PyArray_DIM(input, 1);
    const vct::size_type targetRows = target.rows();
    const vct::size_type targetCols = target.cols();
    if (   inputRows != targetRows
        || inputCols != targetCols) {
        std::stringstream stream;
        stream << "Input matrix's size must be " << targetRows << " rows by " << targetCols << " columns";
        std::string msg = stream.str();
        PyErr_SetString(PyExc_ValueError, msg.c_str());
        return false;
    }
    return true;
}


template <class _matrixOwnerType, typename _elementType>
bool vctThrowUnlessCorrectMatrixSize(PyArrayObject * CMN_UNUSED(input),
                                     const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & CMN_UNUSED(target))
{
    return true;
}


bool vctThrowUnlessOwnsData(PyArrayObject * input)
{
    const int flags = PyArray_FLAGS(input);
    if(!(flags & NPY_ARRAY_OWNDATA)) {
        PyErr_SetString(PyExc_ValueError, "Array must own its data");
        return false;
    }
    return true;
}


template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
bool vctThrowUnlessOwnsData(PyArrayObject * CMN_UNUSED(input),
                            const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & CMN_UNUSED(target))
{
    return true;
}


template <class _vectorOwnerType, typename _elementType>
bool vctThrowUnlessOwnsData(PyArrayObject * input,
                            const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & CMN_UNUSED(target))
{
    const int flags = PyArray_FLAGS(input);
    if(!(flags & NPY_ARRAY_OWNDATA)) {
        PyErr_SetString(PyExc_ValueError, "Array must own its data");
        return false;
    }
    return true;
}


template <vct::size_type _rows, vct::size_type _cols,
	  vct::stride_type _rowStride, vct::stride_type _colStride,
	  class _elementType, class _dataPtrType>
bool vctThrowUnlessOwnsData(PyArrayObject * CMN_UNUSED(input),
                            const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & CMN_UNUSED(target))
{
    return true;
}


template <class _matrixOwnerType, typename _elementType>
bool vctThrowUnlessOwnsData(PyArrayObject * input,
                            const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & CMN_UNUSED(target))
{
    const int flags = PyArray_FLAGS(input);
    if(!(flags & NPY_ARRAY_OWNDATA)) {
        PyErr_SetString(PyExc_ValueError, "Array must own its data");
        return false;
    }
    return true;
}


bool vctThrowUnlessNotReferenced(PyObject * input)
{
    if (PyArray_REFCOUNT(input) > 4) {
        PyErr_SetString(PyExc_ValueError, "Array must not be referenced by other objects.  Try making a deep copy of the array and call the function again.");
        return false;
    }
    return true;
}


template <vct::size_type _size, vct::stride_type _stride, class _elementType, class _dataPtrType>
bool vctThrowUnlessNotReferenced(PyObject * CMN_UNUSED(input),
                                 const vctFixedSizeConstVectorBase<_size, _stride, _elementType, _dataPtrType> & CMN_UNUSED(target))
{
    return true;
}


template <class _vectorOwnerType, typename _elementType>
bool vctThrowUnlessNotReferenced(PyObject * input,
                                 const vctDynamicConstVectorBase<_vectorOwnerType, _elementType> & CMN_UNUSED(target))
{
    if (PyArray_REFCOUNT(input) > 4) {
        PyErr_SetString(PyExc_ValueError, "Array must not be referenced by other objects.  Try making a deep copy of the array and call the function again.");
        return false;
    }
    return true;
}


template <vct::size_type _rows, vct::size_type _cols,
	  vct::stride_type _rowStride, vct::stride_type _colStride,
	  class _elementType, class _dataPtrType>
bool vctThrowUnlessNotReferenced(PyObject * CMN_UNUSED(input),
                                 const vctFixedSizeConstMatrixBase<_rows, _cols, _rowStride, _colStride, _elementType, _dataPtrType> & CMN_UNUSED(target))
{
    return true;
}


template <class _matrixOwnerType, typename _elementType>
bool vctThrowUnlessNotReferenced(PyObject * input,
                                 const vctDynamicConstMatrixBase<_matrixOwnerType, _elementType> & CMN_UNUSED(target))
{
    if (PyArray_REFCOUNT(input) > 4) {
        PyErr_SetString(PyExc_ValueError, "Array must not be referenced by other objects.  Try making a deep copy of the array and call the function again.");
        return false;
    }
    return true;
}



#endif // _vctPythonUtilities_h
