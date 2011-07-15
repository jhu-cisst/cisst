#include <cisstRobot/robLinearRn.h>
#include <cisstDevices/robotcomponents/trajectories/devOMNItoWAM.h>

devOMNItoWAM::devOMNItoWAM( const std::string& name, 
			    double period, 
			    devTrajectory::State state,
			    osaCPUMask cpumask,
			    const std::string& fileomni,
			    const vctDynamicVector<double>& qomni,
			    const std::string& filewam,
			    const vctFrame4x4<double>& Rtw0wam,
			    const vctDynamicVector<double>& qwam ) :

  devTrajectory( name, period, state, cpumask, devTrajectory::TRACK ),
  input( NULL ),
  output( NULL ),
  omni( NULL ),
  wam( NULL ),
  qomnioldsamp( qomni ),
  qomnioldinterp( qomni ),
  qwamoldsol( qwam ),
  told( 0.0 ){

  // Input from the OMNI: 6DOF position
  input  = RequireInputRn( devTrajectory::Input,
			   devRobotComponent::POSITION,
			   6 );

  // Output to the WAM: 7DOF position
  output = RequireOutputRn( devTrajectory::Output, 
			    devRobotComponent::POSITION, 
			    7 );  

  // create an omni manipulator
  omni = new robManipulator( fileomni );
  // create a wam manipulator
  wam = new robManipulator( filewam, Rtw0wam );

}

void devOMNItoWAM::Startup(){}

// Get an input from the OMNI
vctDynamicVector<double> devOMNItoWAM::GetInput(){
  if( input != NULL ){ 
    vctDynamicVector<double> qomni;
    double t;
    input->GetPosition( qomni, t );
    if( qomni.size() == 6 ) return qomni;
  }
  return qomnioldsamp;
}

// always new
bool devOMNItoWAM::IsInputNew(){ 
  if( qomnioldsamp == GetInput() )
    return false;
  return true; 
}

robFunction* devOMNItoWAM::Queue( double t, robFunction* function )
{ return Track( t, function ); }

robFunction* devOMNItoWAM::Track( double t1, robFunction* ){

  // Get the new omni input
  vctDynamicVector<double> qomni = GetInput();

  // estimate the velocity
  vctDynamicVector<double> qd( qomni.size(), 10.0 );
  // create a new interpolation
  /*
  std::cout << "qomniold: " << qomniold << std::endl
	    << "qomninew: " << qomninew << std::endl;
  */
  robLinearRn* fn = new robLinearRn( qomnioldsamp, qomni, qd, t1 );
  // rememer old state
  qomnioldsamp = qomni;
  return fn;

}

void devOMNItoWAM::Evaluate( double t, robFunction* function ){

  // We expect the function to be linear
  robLinearRn* linearrn = dynamic_cast<robLinearRn*>( function );

  if( linearrn != NULL ){

    vctDynamicVector<double> qomninew, qdomninew, qddomninew;
    linearrn->Evaluate( t, qomninew, qdomninew, qddomninew );

    if( qomninew.size() != 6 ){
      CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS 
			<< "Expected 6DOF got " << qomninew.size()
			<< std::endl;
    }
    
    else{

      // initialize the WAM variables
      vctDynamicVector<double> qwamnew( qwamoldsol );
      vctDynamicVector<double> qdwamnew( 7, 0.0 );
      vctDynamicVector<double> qddwamnew( 7, 0.0 );

      // The old/new position/orientation of the OMNI
      //omni[5] = -cmnPI;
      //std::cout << "old: " << qomnioldinterp << std::endl 
      //	<< "new: " << qomninew << std::endl << std::endl;
      vctFrame4x4<double> Rt_w_omni1 = omni->ForwardKinematics( qomnioldinterp);
      vctFrame4x4<double> Rt_w_omni2 = omni->ForwardKinematics( qomninew );

      // Invert
      vctFrame4x4<double> Rt_omni1_w( Rt_w_omni1 );
      Rt_omni1_w.InverseSelf();  

      // The relative position/orientation of the OMNI
      vctFrame4x4<double> Rt_omni1_omni2 = Rt_omni1_w * Rt_w_omni2;
      
      // Scale the position
      Rt_omni1_omni2[0][3] *=  5.0;
      Rt_omni1_omni2[1][3] *=  5.0;
      Rt_omni1_omni2[2][3] *=  5.0;

      // Set this as the relative motion of the WAM
      vctFrame4x4<double> Rt_wam1_wam2( Rt_omni1_omni2 );
      //std::cout << Rt_wam1_wam2 << std::endl;
      // The current position/orientation of the WAM
      vctFrame4x4<double> Rt_w_wam1 = wam->ForwardKinematics( qwamoldsol );

      // Solve the inverse kinematics using the previous solution (qold)
      robManipulator::Errno manerrno;
      manerrno = wam->InverseKinematics( qwamnew, 
					 Rt_w_wam1 * Rt_wam1_wam2, 
					 1e-12, 100 );

      // Did ikin screwed up?
      if( manerrno == robManipulator::ESUCCESS ){
	// No! Then estimate the velocity and acceleration
	output->SetPosition( qwamnew );
	output->SetVelocity( qdwamnew );
	output->SetAcceleration( qddwamnew );
	qwamoldsol = qwamnew;
      }

      else{
	// Yes! Then return the old solution with no velocity
	output->SetPosition( qwamoldsol );
	output->SetVelocity( qdwamnew );
	output->SetAcceleration( qddwamnew );
      }

      // Save the current position/orientation of the OMNI
      qomnioldinterp = qomninew;
      told = t;
      
    }
  }
  else{
    // Set the output in case the trajectory is started after other components
    vctDynamicVector<double>  qd( qwamoldsol.size(), 0.0);
    vctDynamicVector<double> qdd( qwamoldsol.size(), 0.0 );
    output->SetPosition( qwamoldsol );
    output->SetVelocity( qd );
    output->SetAcceleration( qdd );
  }
  
}

