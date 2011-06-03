#ifndef _cisstOpenNI_h
#define _cisstOpenNI_h

#include <XnCppWrapper.h>
#include <XnOS.h>

#include <cisstVector.h>

#include <cisstOpenNI/cisstOpenNIExport.h>

class CISST_EXPORT cisstOpenNI {

private:

  //! OpenNI context
  xn::Context context;

  //! Depth image generator
  xn::DepthGenerator depthgenerator;

  //! RGB image generator
  xn::ImageGenerator rgbgenerator;

public:

  //! Default Constructor
  cisstOpenNI( const std::string& devname = "" );

  //! Get range data
  /**
     Query the depth generator to obtain a depth image and convert the image to a points
     cloud. If the depth image is MxN, then the point cloud is a matrix of size 3x(MxN) where
     each column are the X-Y-Z coordinate of a point. This method is non-const due to updating
     the context.
     \return A 3x(MxN) point cloud.
  */
  vctDynamicMatrix<double> GetRangeData();
  
  //! Get depth image
  /**
     Query the depth generator to obtain a depth image. The value at each pixel represents
     the depth of the picture element. This method is non-const due to updating the context.
  */
  vctDynamicMatrix<double> GetDepthImage();

  //! Get (interlaced) RGB image
  /**

   */
  vctDynamicMatrix<unsigned char> GetRGBImage();

  //! Get (planar) RGB image
  /**
   */
  vctDynamicNArray<unsigned char,3> GetRGBPlanarImage();

};

#endif
