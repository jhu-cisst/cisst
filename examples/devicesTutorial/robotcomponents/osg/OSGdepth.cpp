
#include <osgGA/TrackballManipulator>

#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
#include <cisstDevices/robotcomponents/osg/devOSGBody.h>
#include <cisstDevices/robotcomponents/osg/devOSGCamera.h>

#include <cisstVector/vctMatrixRotation3.h>

#include <cisstMultiTask/mtsTaskManager.h>

#include <cisstCommon/cmnGetChar.h>

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  devOSGWorld* world = new devOSGWorld;

  std::string data( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/objects/" );

  // Create a rotation/translation
  vctFrame4x4<double> Rt( vctMatrixRotation3<double>(),
			  vctFixedSizeVector<double,3>(0.0, 0.0, 0.3) );

  // Load an object and shift it to Rt
  devOSGBody* hubble;
  hubble = new devOSGBody( "hubble", Rt, data+"hst.3ds", world );


  // Load another object
  vctFrame4x4<double> eye;
  devOSGBody* background;
  background = new devOSGBody( "background", eye, data+"background.3ds", world);

  int width = 640, height = 480;
  devOSGCamera* camera = new devOSGCamera( "camera",
					   world,
					   0, 0, width, height,
					   55, ((double)width)/((double)height),
					   0.01, 10.0 );
  // Add+configure the trackball of the camera
  camera->setCameraManipulator( new osgGA::TrackballManipulator );
  camera->getCameraManipulator()->setHomePosition( osg::Vec3d( 1,0,1 ),
						   osg::Vec3d( 0,0,0 ),
						   osg::Vec3d( 0,0,1 ) );
  camera->home();

  // add a bit more light
  osg::ref_ptr<osg::Light> light = new osg::Light;
  light->setAmbient( osg::Vec4( 1, 1, 1, 1 ) );
  camera->setLight( light );

  // Add the camera component
  taskManager->AddComponent( camera );

#ifdef CISST_STEREOVISION

  std::cout << "adsf" << std::endl;

  svlFilterSourceBuffer depthsource;
  svlStreamManager depthstream(1);
  svlFilterImageWindow depthwindow;

  depthsource.SetBuffer( *(camera->GetDepthBufferSample()) );
  depthsource.SetTargetFrequency( 0.3 );

  depthstream.SetSourceFilter( &depthsource );
  depthsource.GetOutput()->Connect( depthconverter.GetInput() );
  depthconverter.GetOutput()->Connect( depthwindow.GetInput() );

#endif

  taskManager->CreateAll();
  taskManager->StartAll();

  cmnGetChar();

  taskManager->KillAll();

}
