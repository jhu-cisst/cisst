
// Include this to create the ODE world and the manipulator
#include <cisstDevices/ode/devODEWorld.h>
#include <cisstDevices/ode/devODEBH.h>

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

  vctFrame4x4<double> Rtwb( vctMatrixRotation3<double>(), 
			    vctFixedSizeVector<double,3>( 0.0, 0.0, 0.1 ) );
  string path("libs/etc/cisstRobot/BH/");
  devODEBH BH( "BH", 
	       0.01, 
	       world, 
	       Rtwb,
	       path+"l0.obj",
	       path+"l1.obj", 
	       path+"l2.obj",
	       path+"l3.obj" );
  taskManager->AddTask( &BH );

  taskManager->CreateAll();
  taskManager->StartAll();

  glutMainLoop();

  return 0;
}
