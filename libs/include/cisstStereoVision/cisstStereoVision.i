/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Anton Deguet
  Created on:   2009-01-26

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


%module cisstStereoVisionPython


%include "std_string.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_pair.i"
%include "std_streambuf.i"
%include "std_iostream.i"

%include "swigrun.i"

%import "cisstConfig.h"
%import "cisstStereoVision/svlConfig.h"

%import "cisstCommon/cisstCommon.i"
%import "cisstVector/cisstVector.i"
%import "cisstMultiTask/cisstMultiTask.i"

%init %{
    import_array() // numpy initialization
%}

%header %{
#include <cisstStereoVision/svlPython.h>
%}

// Generate parameter documentation for IRE
%feature("autodoc", "1");

#define CISST_EXPORT
#define CISST_DEPRECATED

%include "cisstStereoVision/svlDefinitions.h"
%include "cisstStereoVision/svlInitializer.h"

%include "cisstStereoVision/svlStreamManager.h"

%include "cisstStereoVision/svlFilterBase.h"
%include "cisstStereoVision/svlFilterImageFileWriter.h"
%include "cisstStereoVision/svlFilterImageRectifier.h"
%include "cisstStereoVision/svlFilterImageResizer.h"
%include "cisstStereoVision/svlFilterSourceBase.h"
%include "cisstStereoVision/svlFilterSourceDummy.h"
%include "cisstStereoVision/svlFilterSourceVideoFile.h"
%include "cisstStereoVision/svlFilterVideoFileWriter.h"
%include "cisstStereoVision/svlFilterSourceBuffer.h"

%include "cisstStereoVision/svlFilterImageChannelSwapper.h"

%include "cisstStereoVision/svlFilterImageWindow.h"

%include "cisstStereoVision/svlFilterSourceImageFile.h"
#if CISST_SVL_HAS_OPENCV
%include "cisstStereoVision/svlCCCameraCalibration.h"
#endif //CISST_SVL_HAS_OPENCV
%{


template <class __ValueType>
PyObject* convert_vctDynamicMatrixRef_to_PyObject(vctDynamicMatrixRef<__ValueType> matrix_in){

    /*****************************************************************************
    *   %typemap(out, optimal="1") vctDynamicMatrixRef
    *   Returning a vctDynamicMatrixRef
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this type map.
    *****************************************************************************/

    /*****************************************************************************
     CREATE A NEW PYARRAY aOBJECT
    *****************************************************************************/

    npy_intp *sizes = PyDimMem_NEW(2);
    sizes[0] = matrix_in.rows();
    sizes[1] = matrix_in.cols();
    int type = vctPythonType<__ValueType>();
    PyObject* result = PyArray_SimpleNew(2, sizes, type);

    /*****************************************************************************
     COPY THE DATA FROM THE vctDynamicMatrixRef TO THE PYARRAY
    *****************************************************************************/

    // Create a temporary vctDynamicMatrixRef container
    const npy_intp size0 = PyArray_DIM(result,0);  //matrix_in.rows();
    const npy_intp size1 = PyArray_DIM(result,1);  //matrix_in.cols();
    const npy_intp stride0 = PyArray_STRIDE(result,0);  //size1;
    const npy_intp stride1 = PyArray_STRIDE(result,1);  //1;
    __ValueType * data = reinterpret_cast<__ValueType *>(PyArray_DATA(result));

    vctDynamicMatrixRef<__ValueType> tempContainer(size0, size1, stride0, stride1, data);

    // Copy the data from the vctDynamicMatrixRef to the temporary container
    tempContainer.Assign(matrix_in);

    return result;
}



template <class __ValueType, vct::size_type _dimension>
PyObject* convert_vctDynamicNArrayRef_to_PyObject(vctDynamicNArrayRef<__ValueType, _dimension> narray_in){
    /*****************************************************************************
    *   %typemap(out, optimal="1") vctDynamicNArrayRef
    *   Returning a vctDynamicNArrayRef
    *
    *   See the documentation ``Developer's Guide to Writing Typemaps'' for documentation on the logic behind
    *   this type map.
    *****************************************************************************/

    /*****************************************************************************
     CREATE A NEW PYARRAY OBJECT
    *****************************************************************************/

    typedef vctDynamicNArrayRef<__ValueType, _dimension> NArrayType;
    vct::size_type sz = _dimension;
    const vctFixedSizeVector<vct::size_type, _dimension> sizes(narray_in.sizes());
    npy_intp *shape = PyDimMem_NEW(sz);
    for (vct::size_type i = 0; i < sz; i++) {
        shape[i] = sizes.at(i);
    }

    int type = vctPythonType<__ValueType>();
    PyObject* result = PyArray_SimpleNew(sz, shape, type);

    /*****************************************************************************
     COPY THE DATA FROM THE vctDynamicNArrayRef TO THE PYARRAY
    *****************************************************************************/

    // Create a temporary vctDynamicNArrayRef container
    // `sizes' defined above, don't need to redefine it
    vctFixedSizeVector<vct::stride_type, _dimension> strides(narray_in.strides());
    __ValueType * data = reinterpret_cast<__ValueType *>(PyArray_DATA(result));

    vctDynamicNArrayRef< __ValueType, _dimension> tempContainer(data, sizes, strides);

    // Copy the data from the vctDynamicNArrayRef to the temporary container
    tempContainer.Assign(narray_in);


    return result;
}



/* Copied from http://www.swig.org/Doc1.1/HTML/Python.html#n11 */

static int PythonCallBack(void * imgdata, void *callbackdata)
{
   PyObject *func = 0, *arglist = 0;
   PyObject *result = 0;
   int    dres = 0;
   PyGILState_STATE state;

   PyObject *pythonImage = 0;
   vctDynamicMatrixRef<unsigned char>* image = 0;
   image = static_cast<vctDynamicMatrixRef<unsigned char>*>(imgdata);

    typedef vctDynamicNArrayRef<unsigned char,3> NArrayImageType;
    NArrayImageType svlBufferNArrayRef;
    SizeType svlBufferNArrayRefSize(image->rows(), image->cols()/3, 3);
    StrideType svlBufferNArrayRefStride(image->row_stride(), 3,image->col_stride());
    svlBufferNArrayRef.SetRef(image->Pointer(),svlBufferNArrayRefSize,svlBufferNArrayRefStride);

    pythonImage = convert_vctDynamicNArrayRef_to_PyObject(svlBufferNArrayRef); //manual hack - should be using typemaps somehow

   try{
	   func = (PyObject *) callbackdata;             // Get Python function

           state = PyGILState_Ensure();
	   arglist = Py_BuildValue("(O)",pythonImage);             // Build argument list

	   //arglist = Py_BuildValue("i",1);

	   if(func && arglist){
                   result = PyEval_CallObjectWithKeywords(func, arglist, (PyObject *)NULL);     // Call Python CallObjectWithKeywords does type check in python
		   Py_DECREF(arglist);                           // Trash arglist
		   /*if (result) {                                 // If no errors, return double
			 dres = PyFloat_AsDouble(result);
		   }*/
		   Py_XDECREF(result);
                   PyGILState_Release(state);
		   return dres;
	   }else{
		   return -1;
	   }
   } catch (...){
        PyGILState_Release(state);
	std::cout << "Unknown exception..." << std::endl << std::flush;
	return -1;
   }
}
%}



%{

   enum PlatformType {
        WinDirectShow   = 0,
        WinSVS          = 1,
        LinVideo4Linux2 = 2,
        LinLibDC1394    = 3,
        OpenCV          = 4,
        MatroxImaging   = 5,
        NumberOfPlatformTypes
    };

 typedef enum _PixelType {
        PixelRAW8    = 0,   //  8 bits per pixel
        PixelRAW16   = 1,   // 16 bits per pixel
        PixelRGB8    = 2,   // 24 bits per pixel
        PixelYUV444  = 3,   // 24 bits per pixel
        PixelYUV422  = 4,   // 16 bits per pixel
        PixelYUV411  = 5,   // 12 bits per pixel
        PixelMONO8   = 6,   //  8 bits per pixel
        PixelMONO16  = 7,   // 16 bits per pixel
        PixelUnknown = 8,
        PixelTypeCount
    } PixelType;

    typedef enum _PatternType {
        PatternRGGB    = 0,
        PatternGBRG    = 1,
        PatternGRBG    = 2,
        PatternBGGR    = 3,
        PatternUnknown = 4,
        PatternTypeCount
    } PatternType;

%}


%include "cisstStereoVision/svlFilterSourceVideoCapture.h"

%{

typedef svlFilterSourceVideoCapture::DeviceInfo DeviceInfo;
typedef svlFilterSourceVideoCapture::ImageFormat ImageFormat;
typedef svlFilterSourceVideoCapture::ExternalTrigger ExternalTrigger;
typedef svlFilterSourceVideoCapture::ImageProperties ImageProperties;

%}


%include "cisstStereoVision/svlFilterCallback.h"

/* Copied from http://www.swig.org/Doc1.1/HTML/Python.html#n11 */
%extend svlFilterCallback {
   // Set a Python function object as a callback function
   // Note : PyObject *pyfunc is remapped with a typempap
   void SetPythonCallback(PyObject *pyfunc) {
     PyEval_InitThreads();
     self->SetCallback(PythonCallBack, (void *) pyfunc);
     Py_INCREF(pyfunc);
   }
}

%include "cisstStereoVision/svlFilterBuffer.h"
%include "cisstStereoVision/svlBufferImage.h"
%include "cisstStereoVision/svlFilterOutput.h"
%include "cisstStereoVision/svlFilterSplitter.h"

%include "cisstStereoVision/svlBufferSample.h"

%include "cisstStereoVision/svlDefinitions.h"
%include "cisstStereoVision/svlSample.h"
%include "cisstStereoVision/svlSampleImageCustom.h"
%include "cisstStereoVision/svlSampleImageTypes.h"

%ignore svlSampleImageCustom<unsigned char,  3, 1>::GetMatrixRef;
%template(svlSampleImageRGB) svlSampleImageCustom<unsigned char,  3, 1>;

%extend svlSampleImageCustom<unsigned char,  3, 1> {
    int SetMatrix(vctDynamicMatrixRef<unsigned char> matrix){
        return $self->SetMatrix(matrix,0);
    }

    svlSample * GetsvlSample() {  //This is a workaround need to figure out how to get the inheritance right with SWIG
        return $self;
    }
}
