// Include this to create the ODE world and the manipulator
#include <cisstDevices/ode/devODEWorld.h>
#include <cisstDevices/ode/devODEManipulator.h>

// To use a control loop
#include <cisstDevices/controlloops/devGravityCompensation.h>
#include <cisstDevices/controlloops/devJointsPD.h>

// To render geometries
#include <cisstDevices/glut/devGLUT.h>

// To run the show
#include <cisstMultiTask/mtsTaskManager.h>

using namespace std;

//! Declare a simple trajectory. (perhaps these should go in cisstDevices)
class Trajectory : public mtsTaskPeriodic {
private:
  // position increment between each iteration
  double dq;

  mtsVector<double> q;       // position of the trajectory
  mtsVector<double> qd;      // velocity of the trajectory
  mtsVector<double> qdd;     // acceleration of the trajectory

public:

  static const string Interface;
  static const string OutputCommand;

  Trajectory( const string& taskname, 
	      double period, 
	      const vctDynamicVector<double>& qinit ) : 
    mtsTaskPeriodic( taskname, period, true ),
    dq(period) {

    mtsProvidedInterface* interface;
    interface = AddProvidedInterface( Trajectory::Interface );

    // Configure the position interface
    if( interface ){
      // copy the initial joints positions to the state table
      q.SetSize( qinit.size() );
      qd.SetSize( qinit.size() );
      qdd.SetSize( qinit.size() );
      q = qinit;
      qd.SetAll( 0.0 );
      qdd.SetAll( 0.0 );
      StateTable.AddData( q,  "JointsPositions" );
      StateTable.AddData( qd, "JointsVelocities" );
      StateTable.AddData( qdd,"JointsAccelerations" );
      
      // Add commands such that the control loop can read the desired positions,
      // velocities and accelerations
      interface->AddCommandReadState( StateTable,
				      q,
				      devControlLoop::ReadReferencePositionCommand );
      interface->AddCommandReadState( StateTable,
				      qd,
				      devControlLoop::ReadReferenceVelocityCommand );
      interface->AddCommandReadState( StateTable,
				      qdd,
				      devControlLoop::ReadReferenceAccelerationCommand );
    }
    else{
      CMN_LOG_INIT_ERROR << CMN_LOG_DETAILS
			 << ": Failed to create the provided interface for "
			 << Trajectory::Interface
			 << std::endl;
      exit(-1);
    }
  }

  void Configure( const std::string& = "" ){}
  void Startup(){}
  void Run(){
    ProcessQueuedCommands();

    // meh, increment the position of each joint
    for( int i=0; i<7; i++ ) q[i] += dq/10.0;
    for( int i=0; i<7; i++ ) qd[i] += 1/10.0;
    for( int i=0; i<7; i++ ) qdd[i] = 0.0;

  }
  void Cleanup(){}

};

const string Trajectory::Interface = "Interface";
const string Trajectory::OutputCommand = "Output";

int main(int argc, char** argv){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create and initialize 
  devGLUT glut(argc, argv);

  // These are the list of models to be rendered in OpenGL
  vector<devGeometry*> geoms;
  geoms.push_back( glut.LoadOBJ("l1.obj") );
  geoms.push_back( glut.LoadOBJ("l2.obj") );
  geoms.push_back( glut.LoadOBJ("l3.obj") );
  geoms.push_back( glut.LoadOBJ("l4.obj") );
  geoms.push_back( glut.LoadOBJ("l5.obj") );
  geoms.push_back( glut.LoadOBJ("l6.obj") );
  geoms.push_back( glut.LoadOBJ("l7.obj") );

  // Initial joint values
  vctDynamicVector<double> qinit(7, 0.0);

  /*** A trajectory ***/
  Trajectory trajectory( "trajectory", 0.001, qinit );
  taskManager->AddTask(&trajectory);

  /*** Create the ODE world ***/
  devODEWorld world( 0.001 , vctFixedSizeVector<double,3>(0.0,0.0,-9.81) );
  world.Configure();
  taskManager->AddTask(&world);

  /*** Create the controller task ***/
  // base to world transformation
  vctFrame4x4<double> Rtwb;
  // controller gains
  vctDynamicMatrix<double> Kp(7,7,0.0), Kd(7,7,0.0);
  Kp[0][0] = 20.0;  Kp[1][1] = 20.0;  Kp[2][2] = 20.0;  Kp[3][3] = 10.0;
  Kp[4][4] = 2.0;   Kp[5][5] = 2.0;   Kp[6][6] = .5;
 
  Kd[0][0] = 3.5;   Kd[1][1] = 3.5;   Kd[2][2] = 3.5;  Kd[3][3] = 1.5;
  Kd[4][4] = 0.01;  Kd[5][5] = 0.01;  Kd[6][6] = 0.001;

  // the controller
  devJointsPD ctrl(Kp, Kd, "controller", 0.001, "wam7.rob", qinit, Rtwb, geoms);
  //devGravityCompensation ctrl( "ctrl", 0.001, "wam7.rob", Rtinit, geoms );
  ctrl.Configure();
  taskManager->AddTask(&ctrl);

  /*** The ODE device ***/
  devODEManipulator wam( "ODEWAM", 0.001, world, "wam7.rob", qinit );
  wam.Configure();
  taskManager->AddTask(&wam);

  // Connect the output of the trajectory to the reference input of the ctrl
  taskManager->Connect( "controller", devControlLoop::ReferenceInterface, 
                        "trajectory", Trajectory::Interface );

  // Connect the feedback input of the ctrl to the position output of the WAM
  taskManager->Connect( "controller", devControlLoop::FeedbackInterface, 
                        "ODEWAM",  devODEManipulator::PositionsInterface );

  // Connect the torque output of the ctrl to the input of the WAM
  taskManager->Connect( "ODEWAM",  devODEManipulator::FTInterface,
			"controller", devControlLoop::OutputInterface );

  taskManager->CreateAll();
  taskManager->StartAll();

  glutMainLoop();

  return 0;
}
