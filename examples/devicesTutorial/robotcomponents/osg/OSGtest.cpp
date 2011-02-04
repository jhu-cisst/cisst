#include <cisstDevices/robotcomponents/osg/devOSGBody.h>
#include <cisstDevices/robotcomponents/osg/devOSGMono.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstCommon/cmnGetChar.h>

#include <cisstMultiTask/mtsTaskManager.h>

#include <cisstCommon/cmnGetChar.h>

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

  // The next section pipes OSG into SVL
#if CISST_SVL_HAS_OPENCV2

  // Now do SVL stuff

  // initialize
  svlInitialize();

  // Create the source and hook it to the OSG output
  svlFilterSourceBuffer depthsource;
  depthsource.SetBuffer( *camera->GetDepthBufferSample() );
  depthsource.SetTargetFrequency( 0.3 );
  
  svlFilterSourceBuffer colorsource;
  colorsource.SetBuffer( *camera->GetColorBufferSample() );
  colorsource.SetTargetFrequency( 0.3 );

  // Conver the depth to [0,255] range
  svlFilterStreamTypeConverter converter( svlTypeMatrixFloat, svlTypeImageRGB );
  converter.SetScaling( 255.0f / 50.0 );

  // chain filters to pipeline
  svlStreamManager colorstream(1);
  svlStreamManager depthstream(1);
  svlFilterImageWindow colorwindow;
  svlFilterImageWindow depthwindow;

  // Hook up the filters
  colorstream.SetSourceFilter( &colorsource );
  colorsource.GetOutput()->Connect( colorwindow.GetInput() );

  depthstream.SetSourceFilter( &depthsource );
  depthsource.GetOutput()->Connect( converter.GetInput() );
  converter.GetOutput()->Connect( depthwindow.GetInput() );

  // start the show
  if( colorstream.Play() != SVL_OK ){
    std::cout << "ERROR" << std::endl;
    exit(0);
  }

  if( depthstream.Play() != SVL_OK ){
    std::cout << "ERROR" << std::endl;
    exit(0);
  }

#endif

  // Start the camera
  taskManager->CreateAll();
  taskManager->StartAll();

  // Create objects
  std::string data( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/objects/" );

  vctFrame4x4<double> Rt( vctMatrixRotation3<double>(),
			  vctFixedSizeVector<double,3>(0.0, 0.0, 0.5) );
  osg::ref_ptr<devOSGBody> hubble;
  hubble = new devOSGBody( "hubble", 
			   Rt,
			   data+"hst.3ds",
			   world );

  vctFrame4x4<double> eye;
  osg::ref_ptr<devOSGBody> background;
  background = new devOSGBody( "background", 
			       eye, 
			       data+"background.3ds", 
			       world );

  cmnGetChar();

  // The next block reads and saves OpenCV images and save them to disk
#if CISST_SVL_HAS_OPENCV2
  const cv::Mat& depth = camera->GetDepthImage();
  cv::imwrite( "depth.bmp", depth );

  const cv::Mat& rgb = camera->GetColorImage();
  cv::imwrite( "rgb.bmp", rgb );
  
  cmnGetChar();
#endif

  // Kill everything
  taskManager->KillAll();
  taskManager->Cleanup();

#if CISST_SVL_HAS_OPENCV2
  colorstream.Release();
  depthstream.Release();
#endif


  return 0;

}
