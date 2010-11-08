// To create the ODE world
#include <cisstDevices/ode/devODEWorld.h>
// To create an ODE manipulator
#include <cisstDevices/ode/devODEManipulator.h>

#include <cisstDevices/devKeyboard.h>

// To use a trajectory
#include <cisstDevices/trajectories/devSetPoints.h>
#include <cisstDevices/trajectories/devLinearSE3.h>

// To use a workspace controller
#include <cisstDevices/controllers/devWorkspace.h>

// To render geometries
#include <cisstDevices/glut/devGLUT.h>

// To run the show
#include <cisstMultiTask/mtsTaskManager.h>

using namespace std;

int main(int argc, char** argv){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create and initialize the glut device
  devGLUT glut(argc, argv);

  // Initial joint angles
  vctDynamicVector<double> qinit(7, 0.0);

  // Transformation of the base coordinate frame
  vctMatrixRotation3<double> Rw0;
  vctFixedSizeVector<double,3> tw0;
  vctFrame4x4<double> Rtw0(Rw0,tw0);

  // List of geometrie for the manipulator
  vector<string> geomfiles;
  string path("/home/sleonard/Documents/CAD/WAM/");
  geomfiles.push_back( path + "l1.obj" );
  geomfiles.push_back( path + "l2.obj" );
  geomfiles.push_back( path + "l3.obj" );
  geomfiles.push_back( path + "l4.obj" );
  geomfiles.push_back( path + "l5.obj" );
  geomfiles.push_back( path + "l6.obj" );
  geomfiles.push_back( path + "l7.obj" );

  devKeyboard kb;
  kb.SetQuitKey('q');
  kb.AddKeyWriteCommand('n', "NextSetPoint", devSetPoints::NextSetPoint, true );
  taskManager->AddTask( &kb );

  // Create the world
  devODEWorld world( 0.0001 );//, vctFixedSizeVector<double,3>(0.0, 0.0, 0.0) );
  taskManager->AddTask(&world);

  // Create an "ODE" WAM
  devODEManipulator WAM( "WAM",       // The task name "WAM"
			 0.001,       // The WAM runs at 200Hz
			 world,       // The world used to simulate the WAM
			 "wam7.rob",  // The WAM configuration file
			 qinit,       // The initial joint positions
			 Rtw0,        // The transformation of the base
			 geomfiles ); // The geometries
  taskManager->AddTask( &WAM );

  // Create the controller
  // Proportional gains
  vctFixedSizeMatrix<double,6,6> Kp(0.0);
  vctFixedSizeMatrix<double,6,6> Kd(0.0);
  Kp[0][0] = 50;  Kp[1][1] = 50;  Kp[2][2] = 50;
  Kp[3][3] = 50;  Kp[4][4] = 50;  Kp[5][5] = 200;
  Kd[0][0] = 10;  Kd[1][1] = 10;  Kd[2][2] = 50;
  devWorkspace controller( "controller",      // The name of the controller
			   0.05,              // The controller runs at 100Hz
			   "wam7.rob",        // The WAM dynamics/kinematics
			   Rtw0,              // The base position/orientatoin
			   true,              // The controller is initially on
			   Kp,                // Proportional gains
			   Kd );              // Derivative gains
  taskManager->AddTask(&controller);


  // Initial position/orientation of the trajectory
  vctFrame4x4<double> Rtw1;
  Rtw1[2][3] = 1.2120;			 
  // Trajectory
  devLinearSE3 trajectory( "trajectory",      // The name of the trajectory
			   devSetPoints::GetSE3SetPoint,
			   0.05,              // The trajectory runs at 100Hz
			   true,              // The trajectory is initially on
			   Rtw1,              // Initial position/orientation
			   0.01,              // Max linear velocity
			   0.01 );            // Max angular velocity
  taskManager->AddTask( &trajectory );
  
  std::vector< vctFrame4x4<double> > Rt;
  vctMatrixRotation3<double> Rw2( 0.5403,   -0.0000   , 0.8415,
				  0.0000,    1.0000  ,  0.0000,
				 -0.8415,    0.0000,    0.5403,
				  VCT_NORMALIZE );
  vctFixedSizeVector<double,3> tw2( 0.7674, 0.0000, 0.7928 );
  Rt.push_back( Rtw1 );
  Rt.push_back( vctFrame4x4<double>( Rw2, tw2 ) );
  Rt.push_back( Rtw1 );
  devSetPoints setpoints( "setpoints", Rt );
  taskManager->AddTask( &setpoints );

  taskManager->Connect( "keyboard",   "NextSetPoint",
			"setpoints",  devSetPoints::ControlInterface );

  taskManager->Connect( "trajectory", devTrajectory::InputInterface,
			"setpoints",  devSetPoints::OutputInterface );

  taskManager->Connect( "trajectory", devTrajectory::OutputInterface,
			"controller", devController::InputInterface );

  taskManager->Connect( "controller", devController::OutputInterface,
			"WAM",        devManipulator::InputInterface );

  taskManager->Connect( "controller", devController::FeedbackInterface,
			"WAM",        devManipulator::OutputInterface );

  taskManager->CreateAll();
  taskManager->StartAll();

  glutMainLoop();

  return 0;
}
