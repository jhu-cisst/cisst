#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>
#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
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
  devODEWorld* world = new devODEWorld( 0.0005, OSA_CPU1 );
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
  
  vctDynamicVector<double> qinit(7, 0.0), qfinal(7, 1.0), qdmax( 7, 0.1 );

  devSetPoints setpoints( "setpoints", 7 );
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

  // LWR stuff
  std::string path( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/LWR/");
  std::vector< std::string > models;
  models.push_back( path+"l1.obj" );
  models.push_back( path+"l2.obj" );
  models.push_back( path+"l3.obj" );
  models.push_back( path+"l4.obj" );
  models.push_back( path+"l5.obj" );
  models.push_back( path+"l6.obj" );
  models.push_back( path+"l7.obj" );

  devODEManipulator* LWR = new devODEManipulator( "LWR",
						  0.001,
						  devManipulator::ENABLED,
						  OSA_CPU1,
						  world,
						  devManipulator::POSITION,
						  path+"lwr.rob",
						  vctFrame4x4<double>(),
						  qinit,
						  models,
						  path+"l0.obj" );
  taskManager->AddComponent( LWR );

  // Connect trajectory to robot
  taskManager->Connect( setpoints.GetName(),  devSetPoints::OutputRn,
			trajectory.GetName(), devLinearRn::Input );

  taskManager->Connect( trajectory.GetName(), devLinearRn::Output,
			LWR->GetName(),       devOSGManipulator::Input );

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
