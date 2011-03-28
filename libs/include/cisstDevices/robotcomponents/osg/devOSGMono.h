

#ifndef _devOSGMono_h
#define _devOSGMono_h

#include <cisstVector/vctDynamicNArray.h>
#include <cisstDevices/robotcomponents/osg/devOSGCamera.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devOSGMono : public devOSGCamera {

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
     \param name The name of the camera
     \param world The world the camera belongs to
     \param x The X offset of the camera window
     \param y The Y offset of the camera window
     \param width The width of the camera image
     \param height The height of the camera image
     \param fovy The field of view angle
     \param aspectRatio The aspect ratio of the camera
     \param zNear The near buffer distance
     \param zFar  The far buffer distance
     \param fnname The name of a MTS read command the body will connect
     \param trackball Create the default trackball
  */
  devOSGMono( const std::string& name,
	      devOSGWorld* world,
	      int x, int y, int width, int height,
	      double fovy, double aspectRatio,
	      double zNear, double zFar,
	      const std::string& fnname = "",
	      bool trackball = true );

  ~devOSGMono();

  void Startup();
  void Run();

#if CISST_DEV_HAS_OPENCV22
 public:
  vctDynamicMatrix< std::list< devOSGBody* > > GetVisibilityImage();
  vctDynamicMatrix<double> GetRangeData();
  cv::Mat GetDepthImage();
  cv::Mat GetRGBImage();
  vctDynamicNArray<unsigned char,3> GetRGBPlanarImage();
  vctDynamicMatrix<unsigned char> GetRGBPixelImage();
#endif

};

#endif
