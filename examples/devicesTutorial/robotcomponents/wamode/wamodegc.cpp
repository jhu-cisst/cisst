// Include this to create the ODE world and the manipulator
#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>

// To use a control loop
#include <cisstDevices/robotcomponents/controllers/devGravityCompensation.h>

// To render geometries
#include <cisstDevices/robotcomponents/glut/devGLUT.h>

// To run the show
#include <cisstMultiTask/mtsTaskManager.h>

using namespace std;

int main(int argc, char** argv){

  // log configuration
  //cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
  //cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create and initialize 
  devGLUT glut(argc, argv);

  vctDynamicVector<double> qinit(7, 0.0);              // Initial joint values
  vctMatrixRotation3<double> Rw0;
  vctFixedSizeVector<double,3> tw0;
  vctFrame4x4<double> Rtw0(Rw0,tw0);                   // base transformation

  vector<string> geomfiles;
  string path("libs/etc/cisstRobot/WAM/");
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

  // The WAM
  devODEManipulator WAM( "WAM", 
			 0.001, 
			 true,
			 devManipulator::FORCETORQUE,
			 world, 
			 "wam7.rob", 
			 Rtw0, 
			 qinit,
			 geomfiles );
  taskManager->AddTask( &WAM );

  // the controller
  devGravityCompensation controller("controller", 0.001, true,
				    "wam7.rob",  Rtw0 );
  taskManager->AddTask(&controller);

  taskManager->Connect( "controller", devController::Output,
			"WAM",        devManipulator::Input );

  taskManager->Connect( "controller", devController::Feedback,
			"WAM",        devManipulator::Output );

  taskManager->CreateAll();
  taskManager->StartAll();

  glutMainLoop();

  return 0;
}
