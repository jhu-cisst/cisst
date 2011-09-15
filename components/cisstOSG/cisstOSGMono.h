

#ifndef _cisstOSGMono_h
#define _cisstOSGMono_h

#include <cisstOSG/cisstOSGConfig.h>

#if CISST_OSG_OPENCV
#include <opencv2/opencv.hpp>
#endif

#include <cisstVector/vctDynamicNArray.h>
#include <cisstOSG/cisstOSGCamera.h>
#include <cisstOSG/cisstOSGExport.h>


class CISST_EXPORT cisstOSGMono : public cisstOSGCamera {

 private:

  //! X pixel coordinate of the window's top left corner
  int x;
  //! Y pixel coordinate of the window's top left corner
  int y;
  //! Image width
  int width;
  //! Image height
  int height;

 public : 

  //! Create an OSG mono camera (master camera)
  /**
     Create an OSG mono wrapped in a MTS continuous task. The camera also
     creates an MTS required interface called "Transformation" if a function
     name is provided. This function is used to update the position of the
     camera at each update traversal.
     \param world The world the camera belongs to
     \param x The X offset of the camera window
     \param y The Y offset of the camera window
     \param width The width of the camera image
     \param height The height of the camera image
     \param fovy The field of view angle
     \param aspectRatio The aspect ratio of the camera
     \param zNear The near buffer distance
     \param zFar  The far buffer distance
     \param trackball Create the default trackball
     \param offscreenrendering Set to true if you want to render off screen
  */
  cisstOSGMono( cisstOSGWorld* world,
		int x, int y, int width, int height,
		double fovy, double aspectRatio,
		double zNear, double zFar,
		bool trackball = true,
		bool offscreenrendering = false );
  
  ~cisstOSGMono();

  void Initialize();
    
#if CISST_OSG_OPENCV

 public:

  cisstOSGCamera::Errno GetRangeData( vctDynamicMatrix<double>& rangedata );
  cisstOSGCamera::Errno GetRGBPlanarImage( vctDynamicNArray<unsigned char,3>& rgb );
  cisstOSGCamera::Errno GetRGBImage( cv::Mat& rgb );
  cisstOSGCamera::Errno GetDepthImage( cv::Mat& depth );

#endif
  
};

#endif
