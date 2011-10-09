#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEGripper.h>
#include <cisstDevices/robotcomponents/osg/devOSGMono.h>

#include <cisstDevices/robotcomponents/trajectories/devLinearRn.h>
#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>

#include <cisstCommon/cmnGetChar.h>

int main(){

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create the OSG World
  devODEWorld* world = NULL;
  world = new devODEWorld(0.001, OSA_CPU1, vctFixedSizeVector<double,3>( 0.0 ));
  taskManager->AddComponent( world );

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

  vctDynamicVector<double> qinit( 1, 0.0 ), qfinal( 1, 0.04 ), qdmax( 1, 0.01 );

  devSetPoints setpoints( "setpoints", 1 );
  taskManager->AddComponent( &setpoints );
  
  devLinearRn trajectory( "trajectory",
			  0.01,
			  devTrajectory::ENABLED,
			  OSA_CPU1,
			  devTrajectory::QUEUE,
			  devTrajectory::POSITION,
			  qinit,
			  qdmax );
  taskManager->AddComponent( &trajectory );
  
  // Gripper stuff
  std::string path(CISST_SOURCE_ROOT"/libs/etc/cisstRobot/gripper/");
  devODEGripper* gripper = new devODEGripper( "gripper",
					      0.002,
					      devManipulator::ENABLED,
					      OSA_CPU1,
					      world,
					      devManipulator::POSITION,
					      path+"f1.rob",
					      path+"f2.rob",
					      vctFrame4x4<double>(),
					      path+"l0.obj",
					      path+"l1.obj",
					      path+"l1.obj" );
  taskManager->AddComponent( gripper );

  // Connect trajectory to robot
  taskManager->Connect( setpoints.GetName(),  devSetPoints::OutputRn,
			trajectory.GetName(), devLinearRn::Input );

  taskManager->Connect( trajectory.GetName(), devLinearRn::Output,
			gripper->GetName(),   devManipulator::Input );
  
  // Start everything
  taskManager->CreateAll();
  taskManager->StartAll();

  std::cout << "ENTER to move." << std::endl;
  cmnGetChar();
  setpoints.Insert( qfinal );
  setpoints.Latch();

  std::cout << "ENTER to exit." << std::endl;
  cmnGetChar();
  
  taskManager->KillAll();
  taskManager->Cleanup();
  
  return 0;
}
