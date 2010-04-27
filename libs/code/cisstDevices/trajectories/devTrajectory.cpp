#include <cisstDevices/trajectories/devTrajectory.h>
#include <cisstDevices/controllers/devController.h>
#include <cisstOSAbstraction/osaGetTime.h>

const std::string devTrajectory::ControlInterface   = "TrajectoryIOCTL";
const std::string devTrajectory::Enable             = "Enable";

const std::string devTrajectory::InputInterface     = "TrajectoryInput";
const std::string devTrajectory::OutputInterface    = "TrajectoryOutput";

devTrajectory::devTrajectory( const std::string& taskname, 
			      const std::string& InputFunctionName,
			      double period,
			      bool enabled,
			      devTrajectory::Space space,
			      const vctDynamicVector<double>& yinit ) :
  mtsTaskPeriodic( taskname, period, true ),
  mtsEnabled( enabled ),
  risingedge( false ),
  inputold( yinit ),
  space(space) {

  // Create the control interface
  {  
    mtsProvidedInterface* interface;
    interface = AddProvidedInterface( devTrajectory::ControlInterface );
    if( interface ){
      StateTable.AddData( mtsEnabled, "Enabled" );
      interface->AddCommandWriteState
	( StateTable, mtsEnabled, devTrajectory::Enable );
    }
  }

  // Create the input interface
  {
    mtsRequiredInterface* interface;
    interface = this->AddRequiredInterface( devTrajectory::InputInterface );
    if( interface )
      { interface->AddFunction( InputFunctionName, mtsFnGetInput ); }
  }

  // Create the output interface
  {
    mtsRequiredInterface* interface;
    interface = this->AddRequiredInterface( devTrajectory::OutputInterface );
    if( interface ){

      if( space & devTrajectory::JOINTS ){
	interface->AddFunction
	  ( devController::InputJointPosition, mtsFnSetJointsPosition );
	interface->AddFunction
	  ( devController::InputJointVelocity, mtsFnSetJointsVelocity );
	interface->AddFunction
	  ( devController::InputJointAcceleration, mtsFnSetJointsAcceleration );
      }
      
      if( space & devTrajectory::R3 ){
	interface->AddFunction
	  ( devController::InputPosition, mtsFnSetPosition );
	interface->AddFunction
	  ( devController::InputLinearVelocity, mtsFnSetLinearVelocity );
	interface->AddFunction
	  ( devController::InputLinearAcceleration, mtsFnSetLinearAcceleration);
      }

      if( devTrajectory::SO3 ){
	interface->AddFunction
	  ( devController::InputOrientation, mtsFnSetOrientation );
	interface->AddFunction
	  ( devController::InputAngularVelocity, mtsFnSetAngularVelocity );
	interface->AddFunction
	  (devController::InputAngularAcceleration,mtsFnSetAngularAcceleration);
      }

      if( space & devTrajectory::SE3 ){
	interface->AddFunction
	  ( devController::InputPosition, mtsFnSetPosition );
	interface->AddFunction
	  ( devController::InputLinearVelocity, mtsFnSetLinearVelocity );
	interface->AddFunction
	  ( devController::InputLinearAcceleration, mtsFnSetLinearAcceleration);
	interface->AddFunction
	  ( devController::InputOrientation, mtsFnSetOrientation );
	interface->AddFunction
	  ( devController::InputAngularVelocity, mtsFnSetAngularVelocity );
	interface->AddFunction
	  (devController::InputAngularAcceleration,mtsFnSetAngularAcceleration);
      }
    }
  }
}

void devTrajectory::Startup()
{ tstart = osaGetTime(); }

void devTrajectory::Run(){

  ProcessQueuedCommands();

  if( mtsEnabled ){

    // Read the input
    mtsVector<double> mtsInput;
    mtsFnGetInput( mtsInput );


    // Estimate the difference between the previous input 
    vctDynamicVector<double> input( mtsInput );
    vctDynamicVector<double> diff = input - inputold;
    if( 0 < diff.Norm() ){ 
      Reset( osaGetTime() - tstart, input ); 
      inputold = input;
      tstart = osaGetTime();
    }

    // Evaluate the trajectory
    vctDynamicVector<double> y, yd, ydd;
    Evaluate( osaGetTime() - tstart, y, yd, ydd );

    // Write the output to the controllers
    if( space & devTrajectory::JOINTS ){
      mtsFnSetJointsPosition( mtsVector<double>( y ) );
      mtsFnSetJointsVelocity( mtsVector<double>( yd ) );
      mtsFnSetJointsAcceleration( mtsVector<double>( ydd ) );
    }

    if( space & devTrajectory::R3 ){
      mtsFnSetPosition( mtsVector<double>( y ) );
      mtsFnSetLinearVelocity( mtsVector<double>( yd ) );
      mtsFnSetLinearAcceleration( mtsVector<double>( ydd ) );
    }

    if( space & devTrajectory::SO3 ){
      mtsFnSetOrientation( mtsVector<double>( y ) );
      mtsFnSetAngularVelocity( mtsVector<double>( yd ) );
      mtsFnSetAngularAcceleration( mtsVector<double>( ydd ) );
    }

    if( space & devTrajectory::SE3 ){
      vctDynamicVector<double> t(3,0.0), v(3,0.0), vd(3,0.0);
      t[0] = y[0];       t[1] = y[1];       t[2] = y[2]; 
      v[0] = yd[0];      v[1] = yd[1];      v[2] = yd[2]; 
      vd[0] =ydd[0];     vd[1] =ydd[1];     vd[2] =ydd[2]; 

      vctDynamicVector<double> q(4,0.0), w(3,0.0), wd(3,0.0);
      q[0] = y[3];       q[1] = y[4];       q[2] = y[5];       q[3] = y[6]; 
      w[0] = yd[3];      w[1] = yd[4];      w[2] = yd[5]; 
      wd[0] =ydd[3];     wd[1] =ydd[4];     wd[2] =ydd[5]; 

      mtsFnSetPosition( mtsVector<double>( t ) );
      mtsFnSetLinearVelocity( mtsVector<double>( v ) );
      mtsFnSetLinearAcceleration( mtsVector<double>( vd ) );

      mtsFnSetOrientation( mtsVector<double>( q ) );
      mtsFnSetAngularVelocity( mtsVector<double>( w ) );
      mtsFnSetAngularAcceleration( mtsVector<double>( wd ) );
    }

    if( !risingedge )
      { risingedge=true; }

  }

  else{
    if( risingedge )
      { risingedge = false; }
  }

}
