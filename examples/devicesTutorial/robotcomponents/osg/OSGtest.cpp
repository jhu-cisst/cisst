
#include <cisstDevices/devKeyboard.h>

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
  double Znear = 0.1, Zfar = 10.0;
  devOSGMono* camera;
  camera = new devOSGMono( "camera",
			   world,
			   x, y, width, height,
			   55.0, ((double)width)/((double)height),
			   Znear, Zfar );
  // Add the camera component
  taskManager->AddComponent( camera );

  // Create objects
  std::string path( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/objects/" );
  vctFrame4x4<double> Rt( vctMatrixRotation3<double>(),
			  vctFixedSizeVector<double,3>(0.0, 0.0, 0.5) );
  osg::ref_ptr<devOSGBody> hubble;

  vctFrame4x4<double> eye;
  osg::ref_ptr<devOSGBody> background;
  hubble = new devOSGBody( "hubble", Rt, path+"hst.3ds", world );
  background = new devOSGBody( "background", 
			       eye, 
			       path+"background.3ds", 
			       world );

  // Start the camera
  taskManager->CreateAll();
  taskManager->StartAll();

  // The next block reads and saves OpenCV images and save them to disk
#if CISST_DEV_HAS_OPENCV22

  std::cout << "ENTER to render 3D data." << std::endl;
  cmnGetChar();

  // Get the 3D data from the camera
  vctDynamicMatrix<double> range = camera->GetRangeData();
  devOSGBody* data = new devOSGBody( "data", vctFrm3(), range, world );
  hubble->SetSwitch( false );
  background->SetSwitch( false );

  std::cout << "ENTER to turn off 3D data." << std::endl;
  cmnGetChar();
  data->SetSwitch( false );
  hubble->SetSwitch( true );
  background->SetSwitch( true );

  std::cout << "ENTER to save RGB data." << std::endl;
  cmnGetChar();
  cv::Mat rgb = camera->GetRGBImage();
  cv::imwrite( "rgb.bmp", rgb );

  std::cout << "ENTER to display depth image." << std::endl;
  cmnGetChar();
  cv::Mat depth = camera->GetDepthImage();
  double minval, maxval;
  cv::minMaxLoc( depth, &minval, &maxval );
  cv::namedWindow( "depth" );
  cv::imshow( "depth", depth / maxval );
  cv::waitKey( 10 );

  std::cout << "ENTER to compute visibility image. " 
	    << "This might take a while." << std::endl;
  cmnGetChar();
  vctDynamicMatrix< std::list<devOSGBody*> > vis = camera->GetVisibilityImage();

  size_t row, col;
  std::cout << "Enter pixels coordinates: row col ";
  std::cin >> row >> col;

  std::list<devOSGBody*> l = vis[row][col];
  while( !l.empty() ){
    std::cout << l.front()->GetName() << std::endl;
    l.pop_front();
  }


#endif

  std::cout << "ENTER to exit." << std::endl;
  cmnGetChar();


  // Kill everything
  taskManager->KillAll();
  taskManager->Cleanup();

  return 0;

}
