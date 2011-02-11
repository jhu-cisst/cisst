#include <cisstDevices/robotcomponents/osg/devOSGBody.h>
#include <cisstDevices/robotcomponents/osg/devOSGMono.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstCommon/cmnGetChar.h>

#include <cisstMultiTask/mtsTaskManager.h>

#include <cisstCommon/cmnGetChar.h>

#include <cisstDevices/devConfig.h>
#if CISST_DEV_HAS_OPENCV22
#include <opencv2/opencv.hpp>
#endif

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create the OSG world
  devOSGWorld* world = new devOSGWorld;

  // Create a camera
  int x = 0, y = 0;
  int width = 640, height = 480;
  double Znear = 0.01, Zfar = 10.0;
  devOSGMono* camera;
  camera = new devOSGMono( "camera",
			   world,
			   x, y, width, height,
			   55, ((double)width)/((double)height),
			   Znear, Zfar );
  // Add the camera component
  taskManager->AddComponent( camera );

  // Create objects
  std::string data( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/objects/" );

  vctFrame4x4<double> Rt( vctMatrixRotation3<double>(),
			  vctFixedSizeVector<double,3>(0.0, 0.0, 0.5) );
  osg::ref_ptr<devOSGBody> hubble;

  hubble = new devOSGBody( "hubble", 
			   Rt,
			   data+"hst.3ds",
			   world );

  hubble->setNodeMask( 0x0020 );


  vctFrame4x4<double> eye;
  osg::ref_ptr<devOSGBody> background;
  background = new devOSGBody( "background", 
			       eye, 
			       data+"background.3ds", 
			       world );

  background->setNodeMask( 0x0001 );

  // Start the camera
  taskManager->CreateAll();
  taskManager->StartAll();

  cmnGetChar();

  // The next block reads and saves OpenCV images and save them to disk
#if CISST_DEV_HAS_OPENCV22
  const cv::Mat& depth = camera->GetDepthImage();
  cv::imwrite( "depth.bmp", depth );

  const cv::Mat& rgb = camera->GetColorImage();
  cv::imwrite( "rgb.bmp", rgb );
  
  cmnGetChar();
#endif

  // Kill everything
  taskManager->KillAll();
  taskManager->Cleanup();

  return 0;

}
