

// Need to include these first otherwise there's a mess with
//  #defines in cisstConfig.h
#include <osgViewer/Viewer>
#include <osgDB/WriteFile>
#include <osgGA/TrackballManipulator>

#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
#include <cisstDevices/robotcomponents/osg/devOSGBody.h>

#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEBody.h>
#include <cisstCommon/cmnGetChar.h>

#include <cisstStereoVision.h>

#include <cisstMultiTask/mtsTaskManager.h>

#include <cisstCommon/cmnGetChar.h>

int main(){

  // Create the task manager
  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create the workd task and add it to the manager
  devODEWorld* world;
  world = new devODEWorld( 0.001,
			   OSA_CPU1,
  			   vctFixedSizeVector<double,3>(0.0,0.0,-9.81) );
  taskManager->AddComponent( world );

  // Create an object
  devODEBody* hubble;
  vctFrame4x4<double> Rt( vctMatrixRotation3<double>(),
			  vctFixedSizeVector<double,3>(0, 0, 5.1 ));
  hubble = new devODEBody( "hubble",                              // name
			   Rt,                                    // pos+ori
			   1.0,                                   // mass
			   vctFixedSizeVector<double,3>( 0.0 ),   // com
			   vctFixedSizeMatrix<double,3,3>::Eye(), // moit
			   "libs/etc/cisstRobot/objects/hst.3ds", // model
			   world->GetSpaceID(),                   // 
			   world );

  // Create the background
  devODEBody* background;
  background = new devODEBody( "background", 
			       vctFrame4x4<double>(),
			       "libs/etc/cisstRobot/objects/background.3ds",
			       world->GetSpaceID(),
			       world );

  // Create+Start all the task
  taskManager->CreateAll();
  taskManager->StartAll();

  // Create a viewer
  osgViewer::Viewer viewer;
  viewer.setSceneData( world );
  viewer.setUpViewInWindow( 0, 0, 1024, 480 );

  // Set the background to black
  viewer.getCamera()->setClearColor( osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
  
  // Set the stereo display
  osg::DisplaySettings* displaysettings;
  displaysettings = viewer.getCamera()->getDisplaySettings();
  if( displaysettings == 0 ){
    displaysettings = new osg::DisplaySettings();
    displaysettings->setStereo( true );
    displaysettings->setStereoMode( osg::DisplaySettings::HORIZONTAL_SPLIT );
    viewer.getCamera()->setDisplaySettings( displaysettings );
  }

  // Configure the home position camera
  osgGA::TrackballManipulator *manipulator = new osgGA::TrackballManipulator();
  manipulator->setHomePosition( osg::Vec3d( 0, 4, 4 ),
				osg::Vec3d( 0, 0, 0),
				osg::Vec3d( 0, 0, 1 ) );
  viewer.setCameraManipulator(manipulator);
  viewer.home();

  // SVL
  unsigned int srcwidth = 512;
  unsigned int srcheight = 480;
  int mindisparity = -100;
  int maxdisparity = 100;

  svlInitialize();

  svlCameraGeometry geometry;
  svlRect roi;

  svlStreamManager stream(2);
  svlFilterSourceBuffer source;
  svlFilterComputationalStereo stereo;
  svlFilterStreamTypeConverter converter(svlTypeMatrixFloat, svlTypeImageRGB);
  svlFilterImageWindow window;

  // configure the source
  svlBufferSample buffersample( svlTypeImageRGBStereo );
  source.SetBuffer( buffersample );
  source.SetTargetFrequency(10.0);

  svlSampleImageRGBStereo sample;
  sample.SetSize( SVL_LEFT, 512, 480 );
  sample.SetSize( SVL_RIGHT, 512, 480 );
  unsigned char* lbuf = sample.GetUCharPointer( SVL_LEFT );
  unsigned char* rbuf = sample.GetUCharPointer( SVL_RIGHT );

  geometry.SetIntrinsics((double)srcwidth, (double)srcwidth,
			 (double)srcwidth / 2, (double)srcheight / 2,
			 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			 SVL_LEFT);
  geometry.SetIntrinsics((double)srcwidth, (double)srcwidth,
			 (double)srcwidth / 2, (double)srcheight / 2,
			 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
			 SVL_RIGHT);
  geometry.SetExtrinsics(0.0, 0.0, 0.0,
			 0.0, 0.0, 0.0,
			 SVL_LEFT);
  geometry.SetExtrinsics(0.0, 0.0, 0.0,
			 10.0, 0.0, 0.0, // baseline = 10.0 mm
			 SVL_RIGHT);
  stereo.SetCameraGeometry(geometry);
  
  roi.Assign(5, 5, 640 - 100, 480 - 5);
  stereo.SetROI(roi);
  
  stereo.SetCrossCheck(false);
  stereo.SetSubpixelPrecision(false);
  stereo.SetDisparityRange(mindisparity, maxdisparity);
  stereo.SetScalingFactor(0);
  stereo.SetBlockSize(5);
  stereo.SetQuickSearchRadius(maxdisparity);
  stereo.SetSmoothnessFactor(40);
  stereo.SetTemporalFiltering(0);
  stereo.SetSpatialFiltering(0);
  
  // setup converter
  converter.SetScaling(255.0f / maxdisparity);
  
  // chain filters to pipeline
  stream.SetSourceFilter(&source);
  source.GetOutput()->Connect(stereo.GetInput());
  stereo.GetOutput()->Connect(converter.GetInput());
  converter.GetOutput()->Connect(window.GetInput());

  if (stream.Play() != SVL_OK){
    std::cout << "ERROR" << std::endl;
    exit(0);
  }

  // OSG images
  osg::ref_ptr<osg::Image> left  = new osg::Image;
  osg::ref_ptr<osg::Image> right = new osg::Image;

  /*
  while( !viewer.done() ){

    // render
    viewer.frame();

    left-> readPixels( 0,   0, 512, 480, GL_RGB, GL_UNSIGNED_BYTE );
    right->readPixels( 512, 0, 512, 480, GL_RGB, GL_UNSIGNED_BYTE );
    osgDB::writeImageFile(*left,"left.bmp"); 
    osgDB::writeImageFile(*right,"right.bmp");

    memcpy( lbuf, left ->data(), 512*480*3 );
    memcpy( rbuf, right->data(), 512*480*3 );

    

    usleep(30000);

  }
  */
  getchar();
  

  // Kill everything
  taskManager->KillAll();

  return 0;

}
