// Include this to create the ODE world and the manipulator
#include <cisstDevices/robotcomponents/ode/devODEWorld.h>
#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>

// To use a control loop
#include <cisstDevices/robotcomponents/controllers/devGravityCompensation.h>

// To render geometries
#include <cisstDevices/robotcomponents/glut/devGLUT.h>

// To run the show
#include <cisstMultiTask/mtsTaskManager.h>

#if (CISST_OS == CISST_LINUX_XENOMAI)
#include <sys/mman.h>
#include <native/task.h>
#endif

using namespace std;

int main(int argc, char** argv){

#if (CISST_OS == CISST_LINUX_XENOMAI)
  RT_TASK main;
  mlockall( MCL_CURRENT | MCL_FUTURE );
  rt_task_shadow( &main, "main", 30, 0 );
#endif

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create and initialize 
  devGLUT glut(argc, argv);

  vctDynamicVector<double> qinit(7, 0.0);              // Initial joint values
  vctMatrixRotation3<double> Rw0;
  vctFixedSizeVector<double,3> tw0(0.0);
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
  taskManager->AddComponent(&world);

  // The WAM
  devODEManipulator WAM( "WAM", 
			 0.001, 
			 true,
			 devManipulator::FORCETORQUE,
			 world, 
			 "libs/etc/cisstRobot/WAM/wam7.rob",
			 Rtw0, 
			 qinit,
			 geomfiles );
  taskManager->AddComponent( &WAM );

  // the controller
  devGravityCompensation controller( "controller", 
				     0.001, 
				     true,
				     "libs/etc/cisstRobot/WAM/wam7.rob",
				     Rtw0 );
  taskManager->AddComponent(&controller);

  taskManager->Connect( controller.GetName(), devController::Output,
			WAM.GetName(),        devManipulator::Input );

  taskManager->Connect( controller.GetName(), devController::Feedback,
			WAM.GetName(),        devManipulator::Output );
  
  taskManager->CreateAll();
  taskManager->StartAll();

  glutMainLoop();

  return 0;
}
