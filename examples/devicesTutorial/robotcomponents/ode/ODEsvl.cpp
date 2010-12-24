

// Need to include these first otherwise there's a mess with
//  #defines in cisstConfig.h
#include <osgGA/TrackballManipulator>

#include <cisstDevices/robotcomponents/osg/devOSGStereo.h>
#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEBody.h>

#include <cisstStereoVision.h>

#include <cisstMultiTask/mtsTaskManager.h>

#include <cisstCommon/cmnGetChar.h>

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  devODEWorld* world = new devODEWorld( 0.001, OSA_CPU1 );
  taskManager->AddComponent( world );

  // Create a rigid body. Make up some mass + com + moit
  vctFixedSizeVector<double,3> u( 0.780004, 0.620257, 0.082920 );
  u.NormalizedSelf();
  vctFrame4x4<double> Rt( vctAxisAngleRotation3<double>( u, 0.7391 ),
			  vctFixedSizeVector<double,3>( 0.0, 0.0, 1.0 ) );
  devODEBody* hubble;
  hubble = new devODEBody( "hubble",                              // name
			   Rt,                                    // pos+ori
			   "libs/etc/cisstRobot/objects/hst.3ds", // model
			   world,
			   1.0,                                   // mass
			   vctFixedSizeVector<double,3>( 0.0 ),   // com
			   vctFixedSizeMatrix<double,3,3>::Eye(), // moit
			   world->GetSpaceID() );

  devODEBody* background;
  background = new devODEBody( "background", 
			       vctFrame4x4<double>(),
			       "libs/etc/cisstRobot/objects/background.3ds",
			       world,
			       world->GetSpaceID() );
  
  int width = 320, height = 240;
  // Create a viewer
  devOSGStereo* osgstereo = new devOSGStereo( "stereo",
					      world,
					      0, 0, width, height,
					      55, 
					      ((double)width)/((double)height),
					      0.01, 10.0, 
					      0.1 );
  // Add+configure the trackball of the camera
  osgstereo->setCameraManipulator( new osgGA::TrackballManipulator );
  osgstereo->getCameraManipulator()->setHomePosition( osg::Vec3d( 1,0,1 ),
						      osg::Vec3d( 0,0,0 ),
						      osg::Vec3d( 0,0,1 ) );
  osgstereo->home();

  // add a bit more light
  osg::ref_ptr<osg::Light> light = new osg::Light;
  light->setAmbient( osg::Vec4( 1, 1, 1, 1 ) );
  osgstereo->setLight( light );

  // Add the camera component
  taskManager->AddComponent( osgstereo );

#ifdef CISST_STEREOVISION

  // Now do SVL stuff
  int mindisparity = -200;
  int maxdisparity =  200;
  double fc1 = 1000, fc2 = 1000;         // x-y scalings
  double o1 = width/2, o2 = height/2;    // optical center

  // initialize
  svlInitialize();

  // Create the source and hook it to the OSG output
  svlFilterSourceBuffer colorsource, depthsource;
  depthsource.SetBuffer( *osgstereo->GetDepthBufferSample() );
  depthsource.SetTargetFrequency( 0.3 );

  colorsource.SetBuffer( *osgstereo->GetColorBufferSample() );
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

  // setup converter
  svlFilterStreamTypeConverter stereoconverter(svlTypeMatrixFloat, svlTypeImageRGB);
  stereoconverter.SetScaling(255.0f / maxdisparity);

  svlFilterStreamTypeConverter depthconverter(svlTypeMatrixFloat, svlTypeImageRGB);
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

  taskManager->CreateAll();
  taskManager->StartAll();

  cmnGetChar();

#ifdef CISST_STEREOVISION
  stereostream.Release();
  depthstream.Release();
#endif

  taskManager->KillAll();

  return 0;

}

