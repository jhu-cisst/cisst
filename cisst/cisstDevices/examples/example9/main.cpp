#include <cisstDevices/ode/devODEWorld.h>
#include <cisstDevices/ode/devODEManipulator.h>
#include <cisstDevices/ode/devODEBody.h>

// To use a control loop
#include <cisstDevices/controlloops/devGravityCompensation.h>
#include <cisstDevices/controlloops/devJointsPD.h>

// To render geometries
#include <cisstDevices/glut/devGLUT.h>

// To run the show
#include <cisstMultiTask/mtsTaskManager.h>

int main(int argc, char** argv){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create and initialize 
  devGLUT glut(argc, argv);
  
  // Create the ODE world
  devODEWorld world( 0.0001 , vctFixedSizeVector<double,3>( 0.0, 0.0, -9.81 ));
  world.Configure();
  taskManager->AddTask(&world);

  vctMatrixRotation3<double> R( 1, 0.0000,  0.0000,
				0, 0.9239, -0.3827,
				0, 0.3827,  0.9239, 
				VCT_NORMALIZE );
  vctFixedSizeVector<double,3> t( 0.0, 0.0, 1.0 );
  vctFrame4x4<double> Rt(R,t);

  vctFixedSizeVector<double,3> com(0.0, 0.1, 0.);
  vctFixedSizeMatrix<double,3,3> I = vctFixedSizeMatrix<double,3,3>::Eye();
  I[0][0] = 0.095; I[0][1] = 0;     I[0][2] = 0;
  I[1][0] = 0;     I[1][1] = 0.092; I[1][2] =0;
  I[2][0] = 0;     I[2][1] = 0;     I[2][2] = 0.050;

  devODEBody body( world.WorldID(),
		   world.SpaceID(),
		   Rt,
		   8,
		   com,
		   I,
		   "l1.obj" );
  world.Insert( &body );

  Rt[2][3] = 2;
  devODEBody body1( world.WorldID(),
		    world.SpaceID(),
		    Rt,
		    8,
		    com,
		    I,
		    "l1.obj" );
  world.Insert( &body1 );

  taskManager->CreateAll();
  taskManager->StartAll();

  glutMainLoop();

  return 0;
}

