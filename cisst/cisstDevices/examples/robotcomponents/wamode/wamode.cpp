// Include this to create the ODE world and the manipulator
#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>


#include <cisstDevices/robotcomponents/trajectories/devLinear.h>

// To use a control loop
#include <cisstDevices/robotcomponents/controllers/devGravityCompensation.h>
#include <cisstDevices/robotcomponents/controllers/devJointsPD.h>

// To render geometries
#include <cisstDevices/robotcomponents/glut/devGLUT.h>

#include <cisstDevices/devKeyboard.h>

// To run the show
#include <cisstMultiTask/mtsTaskManager.h>

using namespace std;

int main(int argc, char** argv){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create and initialize 
  devGLUT glut(argc, argv);

  vctDynamicVector<double> qinit(7, 0.10);              // Initial joint values
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

  devKeyboard kb;
  kb.SetQuitKey( 'q' );
  kb.AddKeyWriteCommand( 'e', "keyboardctrl", devController::Enable, true );
  kb.AddKeyWriteCommand( 'e', "keyboardtraj", devTrajectory::Enable, true );
  taskManager->AddTask(&kb);

  // Create the world
  devODEWorld world( 0.0001 );
  taskManager->AddTask(&world);

  // The WAM
  devODEManipulator WAM( "WAM", 
			 0.001, 
			 devManipulator::ENABLED,
			 OSA_CPU1,
			 world, 
			 "wam7.rob", 
			 qinit, 
			 Rtw0,
			 geomfiles);
  taskManager->AddTask( &WAM );

  // Create a (dummy) trajectory
  devTrajectoryRn trajectory( "trajectory", 0.001, qinit.size() );
  taskManager->AddTask(&trajectory);

  // the controller
  devGravityCompensation controller("controller", 0.001, "wam7.rob", Rtw0,true);
  taskManager->AddTask(&controller);

  // 
  taskManager->Connect( "keyboard",  "keyboardtraj",
                        "trajectory", devTrajectory::ControlInterface );

  taskManager->Connect( "keyboard",  "keyboardctrl",
                        "controller", devController::ControlInterface );

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
