#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>
#include <cisstDevices/robotcomponents/osg/devOSGMono.h>

#include <cisstDevices/robotcomponents/trajectories/devLinearRn.h>
#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>

#include <cisstCommon/cmnGetChar.h>

#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsTaskManager.h>

int main(){

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

  vctDynamicVector<double> qinit(7, 0.0), qfinal( 7, 1.0 ), qdmax( 7, 0.1 );
  std::vector< vctDynamicVector<double> > Q;
  Q.push_back( qfinal );
  Q.push_back( qinit );

  devSetPoints setpoints( "setpoints", Q );
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

  // WAM stuff
  std::string path(CISST_SOURCE_ROOT"/libs/etc/cisstRobot/WAM/");
  std::vector< std::string > models;
  models.push_back( path+"l1.3ds" );
  models.push_back( path+"l2.3ds" );
  models.push_back( path+"l3.3ds" );
  models.push_back( path+"l4.3ds" );
  models.push_back( path+"l5.3ds" );
  models.push_back( path+"l6.3ds" );
  models.push_back( path+"l7.3ds" );

  vctMatrixRotation3<double> Rw0( 0.0, 0.0, -1.0,
				  0.0, 1.0,  0.0,
				  1.0, 0.0,  0.0 );
  vctFixedSizeVector<double,3> tw0( 0.0, 0.0, 1.0 );
  vctFrame4x4<double> Rtw0( Rw0, tw0 );
  
  devODEManipulator* WAM = new devODEManipulator( "WAM",
						  0.001,
						  devManipulator::ENABLED,
						  OSA_CPU1,
						  world,
						  devManipulator::POSITION,
						  path+"wam7.rob",
						  Rtw0,
						  qinit,
						  models,
						  //"" );
						  path+"l0.3ds" );
  taskManager->AddComponent( WAM );


  // Connect trajectory to robot
  taskManager->Connect( setpoints.GetName(),  devSetPoints::Output,
			trajectory.GetName(), devLinearRn::Input );

  taskManager->Connect( trajectory.GetName(), devLinearRn::Output,
			WAM->GetName(),       devOSGManipulator::Input );

  // Start everything
  taskManager->CreateAll();
  taskManager->StartAll();

  cmnGetChar();

  taskManager->KillAll();
  taskManager->Cleanup();
  
  return 0;
}
