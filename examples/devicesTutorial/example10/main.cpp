// Include this to create the ODE world and the manipulator
#include <cisstDevices/ode/devODEWorld.h>
#include <cisstDevices/ode/devODEManipulator.h>

// To use a control loop
#include <cisstDevices/controlloops/devGravityCompensation.h>
#include <cisstDevices/controlloops/devSE3PD.h>

// To render geometries
#include <cisstDevices/glut/devGLUT.h>

// To run the show
#include <cisstMultiTask/mtsMatrix.h>
#include <cisstMultiTask/mtsTaskManager.h>

using namespace std;

//! Declare a simple trajectory. (perhaps these should go in cisstDevices)
class Trajectory : public mtsTaskPeriodic {
private:
  // position increment between each iteration
  double dt;

  mtsMatrix<double> Rt;      // position of the trajectory
  mtsVector<double> vw;      // velocity of the trajectory
  mtsVector<double> vdwd;    // acceleration of the trajectory

public:

  static const string Interface;
  static const string OutputCommand;

  Trajectory( const string& taskname, 
	      double period, 
	      const vctFrame4x4<double>& Rtinit ) : 
    mtsTaskPeriodic( taskname, period, true ),
    dt(0.0001) {

    mtsProvidedInterface* interface;
    interface = AddProvidedInterface( Trajectory::Interface );

    // Configure the position interface
    if( interface ){

      // copy the initial joints positions to the state table
      Rt.SetSize(4, 4);   // set a 4x4 matrix
      vw.SetSize( 6 );    // reserve 6 velocities
      vdwd.SetSize( 6 );  // reserve 6 accelerations
      
      // copy the rotation
      Rt[0][0]=Rtinit[0][0]; Rt[0][1]=Rtinit[0][1]; Rt[0][2]=Rtinit[0][2]; 
      Rt[1][0]=Rtinit[1][0]; Rt[1][1]=Rtinit[0][1]; Rt[1][2]=Rtinit[0][2]; 
      Rt[2][0]=Rtinit[2][0]; Rt[2][1]=Rtinit[0][1]; Rt[2][2]=Rtinit[0][2]; 

      // copy the translation
      Rt[0][3]=Rtinit[0][3]; Rt[1][3]=Rtinit[1][3]; Rt[2][3]=Rtinit[2][3]; 

      vw.SetAll( 0.0 );   // clear the velocities
      vdwd.SetAll( 0.0 ); // clear the accelerations
      StateTable.AddData( Rt,   "CartesianPosition" );
      StateTable.AddData( vw,   "CartesianVelocities" );
      StateTable.AddData( vdwd, "CartesianAccelerations" );
      
      // Add commands such that the control loop can read the desired positions,
      // velocities and accelerations
      interface->AddCommandReadState
	( StateTable, Rt, devControlLoop::ReadReferencePositionCommand );
      interface->AddCommandReadState
	( StateTable, vw, devControlLoop::ReadReferenceVelocityCommand );
      interface->AddCommandReadState
	( StateTable, vdwd, devControlLoop::ReadReferenceAccelerationCommand );
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
    Rt[0][3] += 0.001;
    Rt[1][3] -= 0.001;
    Rt[2][3] -= 0.001;
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

  devODEWorld world( 0.00001 );                     // Create the ODE world
  world.Configure();
  taskManager->AddTask(&world);

  vctDynamicVector<double> qinit(7, 0.0);         // Initial joint values
  qinit[3] = M_PI_2;
  vctFrame4x4<double> Rtwb;                       // base transformation
  Rtwb[2][3] = 0.2;                               // lift the WAM up by .2m

  // The manipulator
  devODEManipulator wam( "WAM",0.001,world, "wam7.rob",qinit,Rtwb,geomfiles );
  wam.Configure();
  taskManager->AddTask(&wam);

  // Create the controller task 
  vctFixedSizeMatrix<double,3,3> Kv(0.0), Kvd(0.0), Kw(0.0), Kwd(0.0);
  Kv[0][0]  = 100; Kv[1][1]  = 100; Kv[2][2]  = 100;
  //Kvd[0][0] = 0.0001;   Kvd[1][1] = 0.0001;   Kvd[2][2] = 0.0001;

  // the controller
  devSE3PD ctrl(Kv, Kvd, Kw, Kwd, "controller", 0.001, "wam7.rob", qinit, Rtwb);
  ctrl.Configure();
  taskManager->AddTask(&ctrl);

  // Create a trajectory
  Trajectory trajectory( "trajectory", 0.1, ctrl.ForwardKinematics(qinit) );
  taskManager->AddTask(&trajectory);

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
