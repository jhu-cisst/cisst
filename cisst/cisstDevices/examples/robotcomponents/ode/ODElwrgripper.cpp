#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>
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

  // LWR stuff
  vctDynamicVector<double> LWRqinit(7, 0.0);       // initial point
  vctDynamicVector<double> LWRqfinal( 7, 1.0 );    // final point
  vctDynamicVector<double> LWRqdmax( 7, 0.01 );    // velocity

  devSetPoints LWRsetpoints( "LWRsetpoints", 7 ); // setpoints generator
  taskManager->AddComponent( &LWRsetpoints );

  devLinearRn LWRtrajectory( "LWRtrajectory",        // trajectory generator
			     0.01,
			     devTrajectory::ENABLED,
			     OSA_CPU1,
			     devTrajectory::QUEUE,
			     devTrajectory::POSITION,
			     LWRqinit,
			     LWRqdmax );
  taskManager->AddComponent( &LWRtrajectory );

  std::string LWRpath(CISST_SOURCE_ROOT"/libs/etc/cisstRobot/LWR/");
  std::vector< std::string > models;
  models.push_back( LWRpath+"l1.obj" );
  models.push_back( LWRpath+"l2.obj" );
  models.push_back( LWRpath+"l3.obj" );
  models.push_back( LWRpath+"l4.obj" );
  models.push_back( LWRpath+"l5.obj" );
  models.push_back( LWRpath+"l6.obj" );
  models.push_back( LWRpath+"l7.obj" );

  devODEManipulator* LWR=new devODEManipulator( "LWR",
						0.002,
						devManipulator::ENABLED,
						OSA_CPU1,
						world,
						devManipulator::POSITION,
						LWRpath+"lwr.rob",
						vctFrame4x4<double>(),
						LWRqinit,
						models,
						LWRpath+"l0.obj" );
  taskManager->AddComponent( LWR );

  // Gripper stuff
  vctDynamicVector<double> gripperqinit( 1, 0.0 );    // initial point
  vctDynamicVector<double> gripperqfinal( 1, 1.04 );  // final point
  vctDynamicVector<double> gripperqdmax( 1, 0.001 );  // velocity

  devSetPoints grippersetpoints( "grippersetpoints", 1 );
  taskManager->AddComponent( &grippersetpoints );

  devLinearRn grippertrajectory( "grippertrajectory",
				 0.01,
				 devTrajectory::ENABLED,
				 OSA_CPU1,
				 devTrajectory::QUEUE,
				 devTrajectory::POSITION,
				 gripperqinit,
				 gripperqdmax );
  taskManager->AddComponent( &grippertrajectory );

  std::string gripperpath(CISST_SOURCE_ROOT"/libs/etc/cisstRobot/gripper/");
  devODEGripper* gripper = new devODEGripper( "Gripper",
					      0.002,
					      devManipulator::ENABLED,
					      OSA_CPU1,
					      world,
					      devManipulator::POSITION,
					      gripperpath+"f1.rob",
					      gripperpath+"f2.rob",
					      LWR->ForwardKinematics(LWRqinit),
					      gripperpath+"l0.obj",
					      gripperpath+"l1.obj",
					      gripperpath+"l1.obj" );
  taskManager->AddComponent( gripper );

  LWR->Attach( gripper );

  // Connect trajectory to robot
  taskManager->Connect( LWRsetpoints.GetName(),  devSetPoints::OutputRn,
			LWRtrajectory.GetName(), devLinearRn::Input );

  taskManager->Connect( LWRtrajectory.GetName(), devLinearRn::Output,
			LWR->GetName(),          devOSGManipulator::Input );

  taskManager->Connect( grippersetpoints.GetName(),  devSetPoints::OutputRn,
			grippertrajectory.GetName(), devLinearRn::Input );

  taskManager->Connect( grippertrajectory.GetName(), devLinearRn::Output,
			gripper->GetName(),          devOSGManipulator::Input );

  // Start everything
  taskManager->CreateAll();
  taskManager->StartAll();

  std::cout << "ENTER to move." << std::endl;
  cmnGetChar();
  LWRsetpoints.Insert( LWRqfinal );
  LWRsetpoints.Latch();
  grippersetpoints.Insert( gripperqfinal );
  grippersetpoints.Latch();

  std::cout << "ENTER to exit." << std::endl;
  cmnGetChar();

  taskManager->KillAll();
  taskManager->Cleanup();
  
  return 0;
}
