#include <cisstDevices/robotcomponents/osg/devOSGStereo.h>
#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEBody.h>

#include <cisstVector/vctAxisAngleRotation3.h>

#include <cisstMultiTask/mtsTaskManager.h>

#include <cisstCommon/cmnGetChar.h>

int main(){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  devODEWorld* world = new devODEWorld( 0.001, OSA_CPU1 );
  taskManager->AddComponent( world );

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

  // Create objects
  std::string data( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/objects/" );

  // Create a rigid body. Make up some mass + com + moit
  double mass = 1.0;
  vctFixedSizeVector<double,3> com( 0.0 );
  vctFixedSizeMatrix<double,3,3> moit = vctFixedSizeMatrix<double,3,3>::Eye();
  
  vctFixedSizeVector<double,3> u( 0.780004, 0.620257, 0.082920 );
  u.NormalizedSelf();
  vctFrame4x4<double> Rt( vctAxisAngleRotation3<double>( u, 0.7391 ),
			  vctFixedSizeVector<double,3>( 0.0, 0.0, 1.0 ) );
  devODEBody* hubble = NULL;
  hubble = new devODEBody( "hubble",                              // name
			   Rt,                                    // pos+ori
			   data+"hst.3ds", // model
			   world,
			   mass, com, moit,
			   world->GetSpaceID() );


  // Create a static body. This body has no mass and cannot be moved
  devODEBody* background = NULL;
  background = new devODEBody( "background", 
			       vctFrm3(), 
			       data+"background.3ds",
			       world );


  // Start the camera
  taskManager->CreateAll();
  taskManager->StartAll();

  cmnGetChar();

  taskManager->KillAll();
  taskManager->Cleanup();

  return 0;

}
