#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>
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

  devODEBody* background = new devODEBody( "background",
					   vctFrame4x4<double>(),
					   CISST_SOURCE_ROOT"/libs/etc/cisstRobot/objects/background.3ds",
					   world,
					   world->GetSpaceID() );

  vctDynamicVector<double> qinitl(7, 0.0);
  vctDynamicVector<double> qfinall(7,-0.3, -1.3, 1.5, 1.0, -0.1, -1.4, 0.1);
  vctDynamicVector<double> qdmaxl(7, 0.1 );

  devSetPoints setpointsl( "setpointsl", 7 );
  taskManager->AddComponent( &setpointsl );

  vctDynamicVector<double> qinitr(7, 0.0);
  vctDynamicVector<double> qfinalr(7, 0.3, -1.3, -1.5, 1.0, 0.1, -1.4, -0.1);
  vctDynamicVector<double> qdmaxr(7, 0.1 );

  devSetPoints setpointsr( "setpointsr", 7 );
  taskManager->AddComponent( &setpointsr );

  devLinearRn trajectoryl( "trajectoryl",
			  0.01,
			  devTrajectory::ENABLED,
			  OSA_CPU1,
			  devTrajectory::QUEUE,
			  devTrajectory::POSITION,
			  qinitl,
			  qdmaxl );
  taskManager->AddComponent( &trajectoryl );

  devLinearRn trajectoryr( "trajectoryr",
			   0.01,
			   devTrajectory::ENABLED,
			   OSA_CPU1,
			   devTrajectory::QUEUE,
			   devTrajectory::POSITION,
			   qinitr,
			   qdmaxr );
  taskManager->AddComponent( &trajectoryr );

  // WAM stuff
  std::string path(CISST_SOURCE_ROOT"/libs/etc/cisstRobot/WAM/");
  std::vector< std::string > models;
  models.push_back( path+"l1.obj" );
  models.push_back( path+"l2.obj" );
  models.push_back( path+"l3.obj" );
  models.push_back( path+"l4.obj" );
  models.push_back( path+"l5.obj" );
  models.push_back( path+"l6.obj" );
  models.push_back( path+"l7.obj" );

  vctFrame4x4<double> Rtw0l( vctMatrixRotation3<double>( 0.0, 0.0, -1.0,
							 0.0, 1.0, 0.0,
							 1.0, 0.0, 0.0 ),
			     vctFixedSizeVector<double,3>( 0.5,-0.5, 0.8 ) );

  devODEManipulator* WAMl = new devODEManipulator( "WAMl",
						   0.001,
						   devManipulator::ENABLED,
						   OSA_CPU1,
						   world,
						   devManipulator::POSITION,
						   path+"wam7.rob",
						   Rtw0l,
						   qinitl,
						   models,
						   path+"l0.obj" );
  taskManager->AddComponent( WAMl );

  vctFrame4x4<double> Rtw0r( vctMatrixRotation3<double>( 0.0, 0.0, -1.0,
							 0.0, 1.0, 0.0,
							 1.0, 0.0, 0.0 ),
			     vctFixedSizeVector<double,3>( 0.5, 0.5, 0.8 ) );

  devODEManipulator* WAMr = new devODEManipulator( "WAMr",
						   0.001,
						   devManipulator::ENABLED,
						   OSA_CPU1,
						   world,
						   devManipulator::POSITION,
						   path+"wam7.rob",
						   Rtw0r,
						   qinitr,
						   models,
						   path+"l0.obj" );
  taskManager->AddComponent( WAMr );

  // Connect trajectory to robot
  taskManager->Connect( setpointsl.GetName(),  devSetPoints::OutputRn,
			trajectoryl.GetName(), devLinearRn::Input );

  taskManager->Connect( trajectoryl.GetName(), devLinearRn::Output,
			WAMl->GetName(),       devManipulator::Input );


  taskManager->Connect( setpointsr.GetName(),  devSetPoints::OutputRn,
			trajectoryr.GetName(), devLinearRn::Input );

  taskManager->Connect( trajectoryr.GetName(), devLinearRn::Output,
			WAMr->GetName(),       devManipulator::Input );

  // Start everything
  taskManager->CreateAll();
  taskManager->StartAll();

  std::cout << "ENTER to move." << std::endl;
  cmnGetChar();
  setpointsr.Insert( qfinalr );
  setpointsr.Latch();
  setpointsl.Insert( qfinall );
  setpointsl.Latch();

  std::cout << "ENTER to exit." << std::endl;
  cmnGetChar();

  taskManager->KillAll();
  taskManager->Cleanup();
  
  return 0;
}
