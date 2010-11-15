

// Need to include these first otherwise there's a mess with
//  #defines in cisstConfig.h
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>

#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
#include <cisstDevices/robotcomponents/osg/devOSGBody.h>

#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEBody.h>
#include <cisstCommon/cmnGetChar.h>

#include <cisstMultiTask/mtsTaskManager.h>

#include <cisstCommon/cmnGetChar.h>

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  devODEWorld* world;
  world = new devODEWorld( 0.001,
			   OSA_CPU1,
  			   vctFixedSizeVector<double,3>(0.0,0.0,-9.81) );
  taskManager->AddComponent( world );

  devODEBody* hubble;
  hubble = new devODEBody( "hubble",                              // name
			   vctFrame4x4<double>( vctMatrixRotation3<double>(),
						vctFixedSizeVector<double,3>(0, 0, 5.1 )),
			   1.0,                                   // mass
			   vctFixedSizeVector<double,3>( 0.0 ),   // com
			   vctFixedSizeMatrix<double,3,3>::Eye(), // moit
			   "libs/etc/cisstRobot/objects/hst.3ds", // model
			   world->GetSpaceID(),                   // 
			   world );
  devODEBody* background;
  background = new devODEBody( "background", 
			       vctFrame4x4<double>(),
			       "libs/etc/cisstRobot/objects/background.3ds",
			       world->GetSpaceID(),
			       world );

  taskManager->CreateAll();
  taskManager->StartAll();

  osgViewer::Viewer viewer;
  viewer.setSceneData( world );
  viewer.setUpViewInWindow( 0, 0, 1024, 480 );

  viewer.getCamera()->setClearColor( osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
  
  osg::DisplaySettings* displaysettings;
  displaysettings = viewer.getCamera()->getDisplaySettings();
  if( displaysettings == 0 ){
    displaysettings = new osg::DisplaySettings();
    displaysettings->setStereo( true );
    displaysettings->setStereoMode( osg::DisplaySettings::HORIZONTAL_SPLIT );

    viewer.getCamera()->setDisplaySettings( displaysettings );
  }

  osgGA::TrackballManipulator *manipulator = new osgGA::TrackballManipulator();
  manipulator->setHomePosition( osg::Vec3d( 0, 4, 4 ),
				osg::Vec3d( 0, 0, 0),
				osg::Vec3d( 0, 0, 1 ) );
  viewer.setCameraManipulator(manipulator);
  viewer.home();
  viewer.run();

  taskManager->KillAll();

  return 0;

}
