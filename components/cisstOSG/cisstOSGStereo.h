

#ifndef _cisstOSGStereo_h
#define _cisstOSGStereo_h

#include <cisstVector/vctDynamicNArray.h>
#include <cisstOSG/cisstOSGCamera.h>
#include <cisstOSG/cisstOSGExport.h>

class CISST_EXPORT cisstOSGStereo : public cisstOSGCamera {
  
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
     \param trackball Use the default trackball
  */
  cisstOSGStereo( cisstOSGWorld* world,
		  int x, int y, int width, int height,
		  double fovy, double aspectRatio,
		  double zNear, double zFar,
		  double baseline,
		  bool trackball = true );
  ~cisstOSGStereo( );

  void Initialize();
  
#if CISST_DEV_HAS_OPENCV22
 public:
  std::list< std::list< cisstOSGBody* > > GetVisibilityList( size_t idx );
  vctDynamicMatrix<double> GetRangeData( size_t idx );
  vctDynamicNArray<unsigned char,3> GetRGBPlanarImage( size_t idx );
  cv::Mat GetRGBImage( size_t idx );
#endif

};

#endif
