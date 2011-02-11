

#ifndef _devOSGStereo_h
#define _devOSGStereo_h

#include <cisstDevices/robotcomponents/osg/devOSGCamera.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devOSGStereo : public devOSGCamera {
  
 private:

  //! X pixel coordinate of the window's top left corner
  int x;
  //! Y pixel coordinate of the window's top left corner
  int y;
  //! Image width
  int width;
  //! Image height
  int height;

  //! The baseline between the cameras
  double baseline;
  
 public : 
  
  //! Create a stereo OSG viewer (actually it's a OSG viewer)
  /**
     Create an stereo OSG viewer wrapped in a MTS continuous task. The stereo 
     also creates an MTS required interface called "Transformation" if a 
     function name is provided. This function is used to update the position of 
     the camera at each update traversal.
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
     \param baseline The base line between the two cameras
     \param fnname The name of a MTS read command the body will connect
     \param trackball Use the default trackball
  */
  devOSGStereo( const std::string& name,
		devOSGWorld* world,
		int x, int y, int width, int height,
		double fovy, double aspectRatio,
		double zNear, double zFar,
		double baseline,
		const std::string& fnname = "",
		bool trackball = true );
  ~devOSGStereo( );

  void Startup();

  void Run();

#if CISST_DEV_HAS_OPENCV22
 public:
  cv::Mat GetDepthImage( size_t idx = 0 ) const;
  cv::Mat GetColorImage( size_t idx = 0 ) const;
#endif

};

#endif
