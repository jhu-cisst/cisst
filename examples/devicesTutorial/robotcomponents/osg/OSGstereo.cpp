#include <cisstDevices/robotcomponents/osg/devOSGBody.h>
#include <cisstDevices/robotcomponents/osg/devOSGStereo.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstCommon/cmnGetChar.h>

#include <cisstMultiTask/mtsTaskManager.h>

#include <cisstCommon/cmnGetChar.h>

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create the OSG World
  devOSGWorld* world = new devOSGWorld;

  // Create a stereo rig
  int x = 0, y = 0;
  int width = 320, height = 240;
  double Znear = 0.01, Zfar = 10.0;
  double baseline = 0.1;
  devOSGStereo* stereocam=new devOSGStereo( "stereo",
					    world,
					    x, y, width, height,
					    55, ((double)width)/((double)height),
					    Znear, Zfar,
					    baseline );
  // Add the camera component
  taskManager->AddComponent( stereocam );

  // The next section pipes OSG into SVL and compute disparity
#if CISST_SVL_HAS_OPENCV2

  // Now do SVL stuff
  int mindisparity = -200;
  int maxdisparity =  200;
  double fc1 = 1000, fc2 = 1000;         // x-y scalings
  double o1 = width/2, o2 = height/2;    // optical center

  // initialize
  svlInitialize();

  // Create the source and hook it to the OSG output
  svlFilterSourceBuffer depthsource;
  depthsource.SetBuffer( *stereocam->GetDepthBufferSample() );
  depthsource.SetTargetFrequency( 0.3 );

  svlFilterSourceBuffer colorsource;
  colorsource.SetBuffer( *stereocam->GetColorBufferSample() );
  colorsource.SetTargetFrequency( 0.3 );

  // Create the SVL cameras
  svlCameraGeometry geometry;
  geometry.SetIntrinsics( fc1, fc2, o1, o2,
			  0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			  SVL_LEFT);
  geometry.SetIntrinsics( fc1, fc2, o1, o2,
			  0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			  SVL_RIGHT);
  geometry.SetExtrinsics( 0.0, 0.0, 0.0,  0.0, 0.0, 0.0, SVL_LEFT );
  geometry.SetExtrinsics( 0.0, 0.0, 0.0, 10.0, 0.0, 0.0, SVL_RIGHT );

  // Stereo configuration
  svlFilterComputationalStereo stereo;
  stereo.SetCameraGeometry(geometry);
  
  stereo.SetCrossCheck( false );
  stereo.SetSubpixelPrecision( false );
  stereo.SetDisparityRange( mindisparity, maxdisparity );
  stereo.SetScalingFactor( 0 );
  stereo.SetBlockSize( 11 );
  stereo.SetQuickSearchRadius( maxdisparity );
  stereo.SetSmoothnessFactor( 11 );
  stereo.SetTemporalFiltering( 0 );
  stereo.SetSpatialFiltering( 0 );

  svlRect roi;
  roi.Assign(5, 5, width-5, height-5);
  stereo.SetROI(roi);

  // setup converter for depth and disparity
  svlFilterStreamTypeConverter stereoconverter( svlTypeMatrixFloat, 
						svlTypeImageRGB );
  stereoconverter.SetScaling(255.0f / maxdisparity);

  svlFilterStreamTypeConverter depthconverter( svlTypeMatrixFloat, 
					       svlTypeImageRGB );
  depthconverter.SetScaling( 255.0f / 50.0 );

  // chain filters to pipeline
  svlStreamManager stereostream(2), depthstream(1);
  svlFilterImageWindow stereowindow, depthwindow;

  // Hook up the filters
  stereostream.SetSourceFilter( &colorsource );
  colorsource.GetOutput()->Connect( stereo.GetInput() );
  stereo.GetOutput()->Connect( stereoconverter.GetInput() );
  stereoconverter.GetOutput()->Connect( stereowindow.GetInput() );

  depthstream.SetSourceFilter( &depthsource );
  depthsource.GetOutput()->Connect( depthconverter.GetInput() );
  depthconverter.GetOutput()->Connect( depthwindow.GetInput() );

  // start the show
  if( stereostream.Play() != SVL_OK ){
    std::cout << "ERROR" << std::endl;
    exit(0);
  }

  if( depthstream.Play() != SVL_OK ){
    std::cout << "ERROR" << std::endl;
    exit(0);
  }

#endif

  // Start the cameras
  taskManager->CreateAll();
  taskManager->StartAll();

  // Create objects
  std::string data( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/objects/" );

  vctFrame4x4<double> Rt( vctMatrixRotation3<double>(),
			  vctFixedSizeVector<double,3>(0.0, 0.0, 0.5) );
  devOSGBody* hubble;
  hubble = new devOSGBody( "hubble", Rt, data+"hst.3ds", world );

  vctFrame4x4<double> eye;
  devOSGBody* background;
  background = new devOSGBody( "background", eye, data+"background.3ds", world );

  cmnGetChar();

  // The next block reads and saves OpenCV images and save them to disk
#if CISST_SVL_HAS_OPENCV2
  const cv::Mat& depth = stereocam->GetDepthImage();
  cv::imwrite( "depth.bmp", depth );

  const cv::Mat& leftrgb = stereocam->GetColorImage( 0 );
  cv::imwrite( "LEFTrgb.bmp", leftrgb );

  const cv::Mat& rightrgb = stereocam->GetColorImage( 1 );
  cv::imwrite( "RIGHTrgb.bmp", rightrgb );
  
  cmnGetChar();
#endif

  // Kill everything
  taskManager->KillAll();
  taskManager->Cleanup();

#if CISST_SVL_HAS_OPENCV2

  stereostream.Release();
  depthstream.Release();

#endif


  return 0;

}
