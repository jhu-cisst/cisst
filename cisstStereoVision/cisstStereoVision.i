/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):	Anton Deguet
  Created on:   2009-01-26

  (C) Copyright 2006-2019 Johns Hopkins University (JHU), All Rights Reserved.

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


%include "cisstStereoVision/svlInitializer.h"

%include "cisstStereoVision/svlStreamManager.h"

%include "cisstStereoVision/svlFilterBase.h"
%include "cisstStereoVision/svlFilterImageFileWriter.h"
%include "cisstStereoVision/svlFilterImageRectifier.h"
%include "cisstStereoVision/svlFilterImageResizer.h"
%include "cisstStereoVision/svlFilterSourceBase.h"
%include "cisstStereoVision/svlFilterSourceVideoFile.h"
%include "cisstStereoVision/svlFilterVideoFileWriter.h"
%include "cisstStereoVision/svlFilterSourceBuffer.h"
%include "cisstStereoVision/svlFilterImageChannelSwapper.h"

 // %include "cisstStereoVision/svlFilterRGBSwapper.h"

%include "cisstStereoVision/svlSampleImage.h"

//%ignore svlSampleImageCustom::GetMatrixRef;

%include "cisstStereoVision/svlSampleImageCustom.h"


%template(svlSampleImageRGB) svlSampleImageCustom<unsigned char,  3, 1>;

%ignore FitEllipse;
%ignore SetFilterCompactness;
%ignore SetFilterArea;

///\note SWIG is very picky about string matching the signature needs to be exactly the same
%rename(RectifyPython) Rectify(svlSampleImage* src_img,
                               unsigned int src_videoch,
                               svlSampleImage* dst_img,
                               unsigned int dst_videoch,
                               bool interpolation,
                               Internals& internals);

%include "cisstStereoVision/svlImageProcessing.h"

%rename(SetFromCameraCalibrationPython) SetFromCameraCalibration(unsigned int height,unsigned int width,vct3x3 R,vct2 f, vct2 c, vctFixedSizeVector<double,7> k, double alpha, unsigned int videoch);

%include "code/svlImageProcessingHelper.h"

%apply double& INOUT { double & framerate };

%apply unsigned int & INOUT { unsigned int & };

#if CISST_SVL_HAS_ZLIB
%include "code/svlVideoCodecCVI.h"

%extend svlVideoCodecCVI {
   int ReadPython(svlSampleImage &image, const unsigned int videoch, const bool noresize) {
       return $self->Read(NULL, image, videoch, noresize);
   }
}
#endif  // CISST_SVL_HAS_ZLIB

%include "cisstStereoVision/svlFilterImageWindow.h"
#if CISST_SVL_HAS_OPENCV2
%include "cisstStereoVision/svlFilterImageCameraCalibrationOpenCV.h"
#endif //CISST_SVL_HAS_OPENCV2

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
    const npy_intp size0 = matrix_in.rows();
    const npy_intp size1 = matrix_in.cols();
    const npy_intp stride0 = size1;
    const npy_intp stride1 = 1;
    __ValueType * data = reinterpret_cast<__ValueType *>(PyArray_DATA(reinterpret_cast<PyArrayObject *>(result)));

    vctDynamicMatrixRef<__ValueType> tempContainer(size0, size1, stride0, stride1, data);

    // Copy the data from the vctDynamicMatrixRef to the temporary container
    tempContainer.Assign(matrix_in);

    return result;
}



/* Copied from http://www.swig.org/Doc1.1/HTML/Python.html#n11 */

static int PythonCallBack(void * imgdata, void *callbackdata)
{
   PyObject *func = 0, *arglist = 0;
   PyObject *result = 0;
   int    dres = 0;

   PyObject *pythonImage = 0;
   vctDynamicMatrixRef<unsigned char>* image = 0;
   image = static_cast<vctDynamicMatrixRef<unsigned char>*>(imgdata);
   //pythonImage = SWIG_NewPointerObj(SWIG_as_voidptr(image), SWIGTYPE_vctDynamicMatrixRef, SWIG_POINTER_NEW | 0 );
   pythonImage = convert_vctDynamicMatrixRef_to_PyObject(*image); //manual hack - should be using typemaps

   try{
	   func = (PyObject *) callbackdata;             // Get Python function

	   arglist = Py_BuildValue("(O)",pythonImage);             // Build argument list

	   //arglist = Py_BuildValue("i",1);

	   if(func && arglist){
		   result = PyEval_CallObject(func,arglist);     // Call Python
		   Py_DECREF(arglist);                           // Trash arglist
		   /*if (result) {                                 // If no errors, return double
			 dres = PyFloat_AsDouble(result);
		   }*/
		   Py_XDECREF(result);
		   return dres;
	   }else{
		   return -1;
	   }
   } catch (...){
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


#if 0 // adeguet1, this was introduced by Dan Mirota (I think, need to figure out why it is not in branch)
%include "cisstStereoVision/svlFilterCallback.h"

/* Copied from http://www.swig.org/Doc1.1/HTML/Python.html#n11 */
// Attach a new method to our plot widget for adding Python functions
//%addmethods svlFilterCallback {
%extend svlFilterCallback {
   // Set a Python function object as a callback function
   // Note : PyObject *pyfunc is remapped with a typempap
   void SetPythonCallback(PyObject *pyfunc) {
     self->SetCallback(PythonCallBack, (void *) pyfunc);
     Py_INCREF(pyfunc);
   }
}

%include "cisstStereoVision/svlFilterBuffer.h"
#endif
%include "cisstStereoVision/svlFilterOutput.h"


