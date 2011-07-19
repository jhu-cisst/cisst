#include <cisstRobot/robLinearSE3.h>
#include <cisstDevices/robotcomponents/trajectories/devInverseKinematics.h>
#include <cisstVector/vctQuaternionRotation3.h>
devInverseKinematics::devInverseKinematics( const std::string& name, 
					    double period, 
					    devTrajectory::State state,
					    osaCPUMask cpumask,
					    devTrajectory::Mode mode,
					    const vctDynamicVector<double>& qinit,
					    double vmax,
					    double wmax,
					    const std::string& robfile,
					    const vctFrame4x4<double>& Rtw0 ):
  devTrajectory( name, period, state, cpumask, mode ),
  input( NULL ),
  output( NULL ),
  robot( NULL ),
  vmax( vmax ),
  wmax( wmax ),
  oldoutput( qinit ){

  // Input: SE3
  input = RequireInputSE3( devTrajectory::Input, devRobotComponent::POSITION );

  // Output: Rn
  output = RequireOutputRn( devTrajectory::Output, 
			    devRobotComponent::POSITION, 
			    qinit.size() );  
  output->SetPosition( oldoutput );

  // create the robot
  try{ 
    robot = new robManipulator( robfile, Rtw0 ); 
    oldinput = robot->ForwardKinematics( qinit );
  }
  catch( std::bad_alloc& ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to allocate robManipulator."
		      << std::endl;
  }

}

devInverseKinematics::devInverseKinematics( const std::string& name, 
					    double period, 
					    devTrajectory::State state,
					    osaCPUMask cpumask,
					    devTrajectory::Mode mode,
					    const vctDynamicVector<double>& qinit,
					    double vmax, 
					    double wmax,
					    const std::string& robfile,
					    const vctFrm3& Rtw0 ):
  devTrajectory( name, period, state, cpumask, mode ),
  input( NULL ),
  output( NULL ),
  robot( NULL ),
  vmax( vmax ),
  wmax( wmax ),
  oldoutput( qinit ){

  // Input: SE3
  input = RequireInputSE3( devTrajectory::Input, devRobotComponent::POSITION );

  // Output: Rn
  output = RequireOutputRn( devTrajectory::Output, 
			    devRobotComponent::POSITION, 
			    qinit.size() );  
  output->SetPosition( oldoutput );

  // Hack to avoid non-normalized rotations!
  const vctMatrixRotation3<double>& R = Rtw0.Rotation();
  vctQuaternionRotation3<double> q( R, VCT_NORMALIZE );
  vctFrame4x4<double> Rt( q, Rtw0.Translation() );

  // create the robot
  try{
    robot = new robManipulator( robfile, Rt );
    oldinput = robot->ForwardKinematics( qinit );
  }
  catch( std::bad_alloc& ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Failed to allocate robManipulator."
		      << std::endl;
  }

}

devInverseKinematics::~devInverseKinematics()
{ if( robot != NULL ) { delete robot; } }

void devInverseKinematics::Reset( const vctDynamicVector<double>& q ){
  oldinput = robot->ForwardKinematics( q );
  oldoutput = q;
}

// Get an input 
vctFrame4x4<double> devInverseKinematics::GetInput(){

  // Get the input SE3
  if( input != NULL ){ 
    vctFrame4x4<double> Rt( oldinput );
    double t;
    input->GetPosition( Rt, t );
    return Rt;
  }

  CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS << "Failed to read input." << std::endl;
  return oldinput;

}

// called from the base class
bool devInverseKinematics::IsInputNew(){ 
  if( oldinput == GetInput() ){ return false; }
  else                        { return true;  }
}

robFunction* devInverseKinematics::Queue( double t, robFunction* function ){ 

  // previous and next functions
  robLinearSE3* previous = dynamic_cast<robLinearSE3*>( function );
  robLinearSE3* next = NULL;

  // previous and next positions
  vctFrame4x4<double> Rt1( oldinput );
  vctFrame4x4<double> Rt2 = GetInput();
  oldinput = Rt2;

  if( previous != NULL ){

    // Get the final position of the previous function
    vctFixedSizeVector<double,6> vw, vdwd;
    previous->FinalState( Rt1, vw, vdwd );

    // Make the next function
    // Queue the next function at the current time 

    if( previous->StopTime() < t )
      { next = new robLinearSE3( Rt1, Rt2, vmax, wmax, t ); }

    // Queue the next function after the current function
    else{
      next = new robLinearSE3( Rt1, Rt2, vmax, wmax );
      previous->Blend( next, vmax, wmax );
    }
  }

  // No previous function
  else
    { next = new robLinearSE3( Rt1, Rt2, vmax, wmax, t ); }

  return next;

}

robFunction* devInverseKinematics::Track( double t, robFunction* function ){

  // previous and next functions
  robLinearSE3* previous = dynamic_cast<robLinearSE3*>( function );
  robLinearSE3* next = NULL;

  // previous and next positions
  vctFrame4x4<double> Rt1( oldinput );
  vctFrame4x4<double> Rt2 = GetInput();
  oldinput = Rt2;

  if( previous != NULL ){
    
    // Get the current position of the previous function
    vctFixedSizeVector<double,6> vw, vdwd;
    previous->Evaluate( t, Rt1, vw, vdwd );

    // Make the next function
    // Queue the next function at the current time 
    if( previous->StopTime() < t )
      { next = new robLinearSE3( Rt1, Rt2, vmax, wmax, t ); }

    // Queue the next function after the current function
    else{
      next = new robLinearSE3( Rt1, Rt2, vmax, wmax );
      previous->Blend( next, vmax, wmax );
    }
  }

  // No previous function
  else
    { next = new robLinearSE3( Rt1, Rt2, vmax, wmax, t ); }

  return next;

}

void devInverseKinematics::Evaluate( double t, robFunction* function ){

  robLinearSE3* linearse3 = dynamic_cast<robLinearSE3*>( function );

  if( linearse3 != NULL ){

    vctFrame4x4<double> Rt;
    vctFixedSizeVector<double,6> vw(0.0), vdwd(0.0);
    
    linearse3->Evaluate( t, Rt, vw, vdwd );

    // Solve the inverse kinematics using the previous solution (qold)
    robManipulator::Errno manerrno;
    vctDynamicVector<double> newoutput( oldoutput );

    manerrno = robot->InverseKinematics( newoutput, Rt );

    // Did ikin screwed up?
    if( manerrno == robManipulator::ESUCCESS ){
      output->SetPosition( newoutput );
      vctDynamicVector<double> qd( newoutput.size(), 0.0 );
      vctDynamicVector<double> qdd( newoutput.size(), 0.0 );
      oldoutput = newoutput;
    }
    else
      { output->SetPosition( oldoutput ); }
  }
  else
    { output->SetPosition( oldoutput); }
  
}

