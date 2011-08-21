#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>
#include <cisstDevices/robotcomponents/ode/devODEBH.h>
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

  // WAM stuff
  vctDynamicVector<double> WAMqinit(7, 0.0);       // initial point
  vctDynamicVector<double> WAMqfinal( 7, 1.0 );    // final point
  vctDynamicVector<double> WAMqdmax( 7, 0.01 );     // velocity

  devSetPoints WAMsetpoints( "WAMsetpoints", 7 ); // setpoints generator
  taskManager->AddComponent( &WAMsetpoints );

  devLinearRn WAMtrajectory( "WAMtrajectory",        // trajectory generator
			     0.01,
			     devTrajectory::ENABLED,
			     OSA_CPU1,
			     devTrajectory::QUEUE,
			     devTrajectory::POSITION,
			     WAMqinit,
			     WAMqdmax );
  taskManager->AddComponent( &WAMtrajectory );

  std::string WAMpath(CISST_SOURCE_ROOT"/libs/etc/cisstRobot/WAM/");
  std::vector< std::string > models;
  models.push_back( WAMpath+"l1.obj" );
  models.push_back( WAMpath+"l2.obj" );
  models.push_back( WAMpath+"l3.obj" );
  models.push_back( WAMpath+"l4.obj" );
  models.push_back( WAMpath+"l5.obj" );
  models.push_back( WAMpath+"l6.obj" );
  models.push_back( WAMpath+"l7.obj" );

  vctMatrixRotation3<double> Rw0( 0.0, 0.0, -1.0,
				  0.0, 1.0,  0.0,
				  1.0, 0.0,  0.0 );
  vctFixedSizeVector<double,3> tw0( 1.0, 0.0, 1.0 );
  vctFrame4x4<double> Rtw0( Rw0, tw0 );
  
  devODEManipulator* WAM=new devODEManipulator( "WAM",
						0.002,
						devManipulator::ENABLED,
						OSA_CPU1,
						world,
						devManipulator::POSITION,
						WAMpath+"wam7.rob",
						Rtw0,
						WAMqinit,
						models,
						WAMpath+"l0.obj" );
  taskManager->AddComponent( WAM );

  // BH stuff
  vctDynamicVector<double> BHqinit(4, 0.0);      // initial point
  vctDynamicVector<double> BHqfinal( 4, 2.0 );   // final point
  vctDynamicVector<double> BHqdmax( 4, 0.1 );    // velocity

  devSetPoints BHsetpoints( "BHsetpoints", 4 );// setpoints generator
  taskManager->AddComponent( &BHsetpoints );

  devLinearRn BHtrajectory( "BHtrajectory",      // trajectory generator
			    0.01,
			    devTrajectory::ENABLED,
			    OSA_CPU1,
			    devTrajectory::QUEUE,
			    devTrajectory::POSITION,
			    BHqinit,
			    BHqdmax );
  taskManager->AddComponent( &BHtrajectory );

  std::string BHpath(CISST_SOURCE_ROOT"/libs/etc/cisstRobot/BH/");
  devODEBH* BH = new devODEBH( "BH",
			       0.002,
			       devManipulator::ENABLED,
			       OSA_CPU1,
			       world,
			       devManipulator::POSITION,
			       BHpath+"f1f2.rob",
			       BHpath+"f3.rob",
			       //vctFrame4x4<double>(),
			       WAM->ForwardKinematics( WAMqinit ),
			       BHpath+"l0.obj",
			       BHpath+"l1.obj",
			       BHpath+"l2.obj",
			       BHpath+"l3.obj" );
  taskManager->AddComponent( BH );

  WAM->Attach( BH );

  // Connect trajectory to robot
  taskManager->Connect( WAMsetpoints.GetName(),  devSetPoints::OutputRn,
			WAMtrajectory.GetName(), devLinearRn::Input );

  taskManager->Connect( WAMtrajectory.GetName(), devLinearRn::Output,
			WAM->GetName(),          devOSGManipulator::Input );

  taskManager->Connect( BHsetpoints.GetName(),  devSetPoints::OutputRn,
			BHtrajectory.GetName(), devLinearRn::Input );

  taskManager->Connect( BHtrajectory.GetName(), devLinearRn::Output,
			BH->GetName(),          devOSGManipulator::Input );


  // Start everything
  taskManager->CreateAll();
  taskManager->StartAll();

  std::cout << "ENTER to move." << std::endl;
  cmnGetChar();
  WAMsetpoints.Insert( WAMqfinal );
  WAMsetpoints.Latch();
  BHsetpoints.Insert( BHqfinal );
  BHsetpoints.Latch();

  std::cout << "ENTER to exit." << std::endl;
  cmnGetChar();

  taskManager->KillAll();
  taskManager->Cleanup();
  
  return 0;
}
