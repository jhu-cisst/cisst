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
    for( int i=0; i<7; i++ ); //{ q[i]  += dq/100.0; }
    for( int i=0; i<7; i++ ); //{ qd[i] = 1/100.0; }
    for( int i=0; i<7; i++ ); //{ qdd[i] = 0.0; }

  }
  void Cleanup(){}

};

const string Trajectory::Interface = "Interface";
const string Trajectory::OutputCommand = "Output";

int main(int argc, char** argv){

  mtsTaskManager* taskManager = mtsTaskManager::GetInstance();

  // Create and initialize 
  devGLUT glut(argc, argv);

  vector<string> geomfiles;
  string path("/home/sleonard/Documents/CAD/WAM/");
  geomfiles.push_back( path + "l1.obj" );
  geomfiles.push_back( path + "l2.obj" );
  geomfiles.push_back( path + "l3.obj" );
  geomfiles.push_back( path + "l4.obj" );
  geomfiles.push_back( path + "l5.obj" );
  geomfiles.push_back( path + "l6.obj" );
  geomfiles.push_back( path + "l7.obj" );

  devODEWorld world( 0.0001 );                         // Create the ODE world
  world.Configure();
  taskManager->AddTask(&world);

  vctDynamicVector<double> qinit(7, 0.0);              // Initial joint values
  qinit[1] = 1;
  Trajectory trajectory( "trajectory", 0.01, qinit );  // Create a trajectory
  taskManager->AddTask(&trajectory);

  // Create the controller task 
  vctFrame4x4<double> Rtwb;                            // base transformation
  Rtwb[2][3] = 0.2;                                    // lift the WAM up by .2m

  vctDynamicMatrix<double> Kp(7,7,0.0), Kd(7,7,0.0);   // controller gains
  Kp[0][0] = 20; Kp[1][1] = 20; Kp[2][2] = 20; Kp[3][3] = 15;
  //Kp[4][4] =  2; Kp[5][5] =  2; Kp[6][6] =  1;
  Kd[0][0] = 5; Kd[1][1] =  5; Kd[2][2] =  5; Kd[3][3] =   2;
  //Kd[4][4] =  1; Kd[5][5] =  1; Kd[6][6] =  1;

  // the controller
  devJointsPD ctrl( Kp, Kd, "controller", 0.001, "wam7.rob", qinit, Rtwb );
  //devGravityCompensation ctrl( "controller", 0.001, "wam7.rob", Rtwb);
  ctrl.Configure();
  taskManager->AddTask(&ctrl);

  // The manipulator
  devODEManipulator wam( "WAM",0.0001,world,"wam7.rob",qinit,Rtwb,geomfiles );
  wam.Configure();
  taskManager->AddTask(&wam);

  // Connect the output of the trajectory to the reference input of the ctrl
  taskManager->Connect( "controller", devControlLoop::ReferenceInterface, 
                        "trajectory", Trajectory::Interface );

  // Connect the feedback input of the ctrl to the position output of the WAM
  taskManager->Connect( "controller", devControlLoop::FeedbackInterface, 
                        "WAM",  devODEManipulator::PositionsInterface );

  // Connect the torque output of the ctrl to the input of the WAM
  taskManager->Connect( "WAM",  devODEManipulator::FTInterface,
			"controller", devControlLoop::OutputInterface );

  taskManager->CreateAll();
  taskManager->StartAll();

  glutMainLoop();

  return 0;
}
