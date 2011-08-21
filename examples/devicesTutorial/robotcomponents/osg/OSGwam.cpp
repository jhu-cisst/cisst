#include <cisstDevices/robotcomponents/osg/devOSGManipulator.h>
#include <cisstDevices/robotcomponents/osg/devOSGMono.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>

#include <cisstDevices/robotcomponents/trajectories/devLinearRn.h>
#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>

#include <cisstCommon/cmnGetChar.h>

int main(){

  cmnLogger::SetMask( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskFunction( CMN_LOG_ALLOW_ALL );
  cmnLogger::SetMaskDefaultLog( CMN_LOG_ALLOW_ALL );

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create the OSG World
  devOSGWorld* world = new devOSGWorld;

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

  vctDynamicVector<double> qinit(7, 0.0);
  vctDynamicVector<double> qfinal(7,  -1.0, -0.5, -1.5, 2.2, 0.6, -1.1,  0.54 );
  vctDynamicVector<double> qdmax(7, 0.25 );

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

  // WAM stuff
  std::string path( CISST_SOURCE_ROOT"/libs/etc/cisstRobot/WAM/");
  std::vector< std::string > models;
  models.push_back( path+"l1.obj" );
  models.push_back( path+"l2.obj" );
  models.push_back( path+"l3.obj" );
  models.push_back( path+"l4.obj" );
  models.push_back( path+"l5.obj" );
  models.push_back( path+"l6.obj" );
  models.push_back( path+"l7.obj" );

  vctFrame4x4<double> Rtw0( vctMatrixRotation3<double>( 0.0, 0.0, -1.0,
							  0.0, 1.0, 0.0,
							  1.0, 0.0, 0.0 ),
			    vctFixedSizeVector<double,3>( 0.0 ) );


  devOSGManipulator* WAM = new devOSGManipulator( "WAM",
						  0.01,
						  devManipulator::ENABLED,
						  OSA_CPU1,
						  world,
						  path+"wam7.rob",
						  Rtw0,
						  qinit,
						  models,
						  path+"l0.obj" );
  taskManager->AddComponent( WAM );

  // Connect trajectory to robot
  taskManager->Connect( setpoints.GetName(),  devSetPoints::OutputRn,
			trajectory.GetName(), devLinearRn::Input );

  taskManager->Connect( trajectory.GetName(), devLinearRn::Output,
			WAM->GetName(),       devOSGManipulator::Input );

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
