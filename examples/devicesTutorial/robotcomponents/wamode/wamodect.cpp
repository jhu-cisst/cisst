// Include this to create the ODE world and the manipulator
#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>

#include <cisstDevices/devKeyboard.h>

// To use a linear trajectory
#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>
#include <cisstDevices/robotcomponents/trajectories/devLinearRn.h>
#include <cisstDevices/robotcomponents/trajectories/devQLQRn.h>

// To use a control loop
#include <cisstDevices/robotcomponents/controllers/devComputedTorque.h>

// To render geometries
#include <cisstDevices/robotcomponents/glut/devGLUT.h>

// To run the show
#include <cisstMultiTask/mtsTaskManager.h>

using namespace std;

int main(int argc, char** argv){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create and initialize 
  devGLUT glut(argc, argv);

  vctDynamicVector<double> qinit(7, 0.0);              // Initial joint values
  vctMatrixRotation3<double> Rw0;
  vctFixedSizeVector<double,3> tw0;
  vctFrame4x4<double> Rtw0(Rw0,tw0);                   // base transformation

  vector<string> geomfiles;
  string path("/home/sleonard/Documents/CAD/WAM/");
  geomfiles.push_back( path + "l1.obj" );
  geomfiles.push_back( path + "l2.obj" );
  geomfiles.push_back( path + "l3.obj" );
  geomfiles.push_back( path + "l4.obj" );
  geomfiles.push_back( path + "l5.obj" );
  geomfiles.push_back( path + "l6.obj" );
  geomfiles.push_back( path + "l7.obj" );

  // Create the world
  devODEWorld world( 0.0001 );
  taskManager->AddTask(&world);

  devKeyboard kb;
  kb.SetQuitKey('q');
  kb.AddKeyWriteCommand('n', "NextSetPoint", devSetPoints::NextSetPoint, true);
  taskManager->AddTask( &kb );

  // Create a set point generator
  std::vector< vctDynamicVector<double> > q;
  vctDynamicVector<double> q1(7, 1.0);
  q.push_back( qinit );
  q.push_back( q1 );
  q.push_back( qinit );
  devSetPoints setpoints( "setpoints", q );
  taskManager->AddTask( &setpoints );

  // Create a rajectory
  vctDynamicVector<double> qdmax(7, 0.1), qddmax(7, .05);
  devQLQRn trajectory( "trajectory", 
		       0.1, 
		       true,
		       devTrajectory::QUEUE,
		       devTrajectory::POSITION,
		       qinit,
		       qdmax,
		       qddmax );
  taskManager->AddTask( &trajectory );

  // Create the controller
  vctDynamicMatrix<double> Kp(7, 7, 0.0), Kd(7, 7, 0.0);
  Kp[0][0] = 1200;    Kd[0][0] =  60;
  Kp[1][1] = 1200;    Kd[1][1] =  60;
  Kp[2][2] = 1000;    Kd[2][2] = 50;
  Kp[3][3] = 850;     Kd[3][3] = 45;  
  Kp[4][4] =  800;    Kd[4][4] =  50;  
  Kp[5][5] =  800;    Kd[5][5] =  50;  
  Kp[6][6] = 20000;   Kd[6][6] = 100;
  devComputedTorque controller( "controller",
				0.01,
				true,
				"wam7.rob",
				Rtw0, 
				Kp, 
				Kd );
  //taskManager->AddTask( &controller );

  // The WAM
  devODEManipulator WAM( "WAM",          // The task name "WAM"
			 0.001,          // The WAM runs at 200Hz
			 true,
			 devManipulator::POSITION,
			 world,          // The world used to simulate the WAM
			 "wam7.rob",     // The WAM configuration file
			 Rtw0,           // The transformation of the base
			 qinit,          // The initial joint positions
			 geomfiles );    // The geometries
  taskManager->AddTask( &WAM );

  taskManager->Connect( "keyboard", "NextSetPoint",
			"setpoints", devSetPoints::Control );

  taskManager->Connect( "trajectory", devTrajectory::Input,
			"setpoints", devSetPoints::Output );
  /*
  taskManager->Connect( "trajectory", devTrajectory::Output,
			"controller", devController::Input );

  taskManager->Connect( "controller", devController::Output,
			"WAM",      devManipulator::Input );

  taskManager->Connect( "controller", devController::Feedback,
			"WAM",      devManipulator::Output );
  */
  taskManager->Connect( "trajectory", devTrajectory::Output,
			"WAM", devManipulator::Input );

  taskManager->CreateAll();
  taskManager->StartAll();

  glutMainLoop();

  return 0;
}
