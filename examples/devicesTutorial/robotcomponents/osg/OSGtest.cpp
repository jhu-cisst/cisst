

// Need to include these first otherwise there's a mess with
//  #defines in cisstConfig.h
#include <osgGA/TrackballManipulator>

#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
#include <cisstDevices/robotcomponents/osg/devOSGBody.h>

#include <cisstDevices/robotcomponents/osg/devOSGBody.h>
#include <cisstDevices/robotcomponents/osg/devOSGCamera.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstCommon/cmnGetChar.h>

#include <cisstMultiTask/mtsTaskManager.h>

#include <cisstCommon/cmnGetChar.h>

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  devOSGWorld* world = new devOSGWorld;

  vctFrame4x4<double> Rt( vctMatrixRotation3<double>(),
			  vctFixedSizeVector<double,3>(0.0, 0.0, 0.5) );
  devOSGBody* hubble;
  hubble = new devOSGBody( "hubble",
			   Rt, 
			   "libs/etc/cisstRobot/objects/hst.3ds",
			   world );

  vctFrame4x4<double> eye;
  devOSGBody* background;
  background = new devOSGBody( "background",
			       eye,
			       "libs/etc/cisstRobot/objects/background.3ds",
			       world );

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

  taskManager->CreateAll();
  taskManager->StartAll();

  cmnGetChar();

  taskManager->KillAll();

  return 0;

}
