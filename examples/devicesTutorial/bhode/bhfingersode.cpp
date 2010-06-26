
// Include this to create the ODE world and the manipulator
#include <cisstDevices/ode/devODEWorld.h>
#include <cisstDevices/ode/devODEBHF1.h>
#include <cisstDevices/ode/devODEBHF2.h>
#include <cisstDevices/ode/devODEBHF3.h>

#include <cisstDevices/trajectories/devSetPoints.h>

// To render geometries
#include <cisstDevices/glut/devGLUT.h>

// To run the show
#include <cisstMultiTask/mtsTaskManager.h>

using namespace std;

int main(int argc, char** argv){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create and initialize 
  devGLUT glut(argc, argv);
  
  // Create the world
  devODEWorld world( 0.0001, vctFixedSizeVector<double,3>( 0.0 ) );
  taskManager->AddTask(&world);

  string path("libs/etc/cisstRobot/BH/");
  devODEBHF1 F1( "F1", 
		 0.01, 
		 world, 
		 world.SpaceID(),
		 path+"l1.obj", 
		 path+"l2.obj",
		 path+"l3.obj",
		 NULL, 
		 0.5 );
  taskManager->AddTask( &F1 );

  devODEBHF2 F2( "F2", 
		 0.01, 
		 world, 
		 world.SpaceID(),
		 path+"l1.obj", 
		 path+"l2.obj",
		 path+"l3.obj",
		 NULL, 
		 0.5 );
  taskManager->AddTask( &F2 );

  devODEBHF3 F3( "F3", 
		 0.01, 
		 world, 
		 world.SpaceID(),
		 path+"l2.obj",
		 path+"l3.obj",
		 NULL, 
		 0.5 );
  taskManager->AddTask( &F3 );

  taskManager->CreateAll();
  taskManager->StartAll();

  glutMainLoop();

  return 0;
}
