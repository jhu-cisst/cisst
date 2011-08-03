#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>
#include <cisstVector/vctMatrixRotation3.h>

const std::string devSetPoints::OutputRn        = "OutputRn";
const std::string devSetPoints::OutputR3        = "OutputR3";
const std::string devSetPoints::OutputSO3       = "OutputSO3";
const std::string devSetPoints::OutputSE3       = "OutputSE3";
const std::string devSetPoints::NextSetPoint     = "NextSetPoint";

devSetPoints::devSetPoints( const std::string& taskname, size_t dim ) :
  devRobotComponent( taskname, 0.001, devSetPoints::ENABLED, OSA_CPUANY ),
  state( false ),
  stateold( false ){

  rnoutput = ProvideOutputRn( devSetPoints::OutputRn,
			      devRobotComponent::POSITION,
			      dim );

  r3output = ProvideOutputR3( devSetPoints::OutputR3,
			      devRobotComponent::POSITION );

  so3output = ProvideOutputSO3( devSetPoints::OutputSO3,
  				devRobotComponent::POSITION );

  se3output = ProvideOutputSE3( devSetPoints::OutputSE3,
				devRobotComponent::POSITION );

  mtsInterfaceProvided* interface;
  interface = GetInterfaceProvided( devRobotComponent::Control );
  StateTable.AddData( state, "State" );
  interface->AddCommandWriteState
    ( StateTable, state, devSetPoints::NextSetPoint );

}

/*
devSetPoints::devSetPoints( const std::string& taskname,
			    const vctDynamicVector<double>& q ) :
  devRobotComponent( taskname, 0.001, devSetPoints::ENABLED, OSA_CPUANY ),
  cnt(0),
  state( false ),
  stateold( false ){

  rnoutput = ProvideOutputRn( devSetPoints::Output,
			      devRobotComponent::POSITION,
			      rnsetpoints[0].size() );

  mtsInterfaceProvided* interface;
  interface = GetInterfaceProvided( devRobotComponent::Control );
  StateTable.AddData( state, "State" );
  interface->AddCommandWriteState
    ( StateTable, state, devSetPoints::NextSetPoint );

  Insert( q );
  rnoutput->SetPosition( rnsetpoints[0] );
}

devSetPoints::devSetPoints( const std::string& taskname,
			    const vctFrame4x4<double>& Rt ) :
  devRobotComponent( taskname, 0.001, devSetPoints::ENABLED, OSA_CPUANY ),
  cnt(0),
  state( false ),
  stateold( false ){

  se3setpoints.push_back( Rt );
  se3output = ProvideOutputSE3( devSetPoints::Output,
				devRobotComponent::POSITION );

  mtsInterfaceProvided* interface;
  interface = GetInterfaceProvided( devRobotComponent::Control );
  StateTable.AddData( state, "State" );
  interface->AddCommandWriteState
    ( StateTable, state, devSetPoints::NextSetPoint );

  se3output->SetPosition( se3setpoints[0] );

}

devSetPoints::devSetPoints( const std::string& taskname, const vctFrm3& Rt ) :
  devRobotComponent( taskname, 0.001, devSetPoints::ENABLED, OSA_CPUANY ),
  cnt(0),
  state( false ),
  stateold( false ){

  // Hack to avoid non-normalized rotations!
  const vctMatrixRotation3<double>& R = Rt.Rotation();
  vctQuaternionRotation3<double> q( R, VCT_NORMALIZE );
  vctFrame4x4<double> Rtq( q, Rt.Translation() );

  se3setpoints.push_back( Rtq );
  se3output = ProvideOutputSE3( devSetPoints::Output,
				devRobotComponent::POSITION );

  mtsInterfaceProvided* interface;
  interface = GetInterfaceProvided( devRobotComponent::Control );
  StateTable.AddData( state, "State" );
  interface->AddCommandWriteState
    ( StateTable, state, devSetPoints::NextSetPoint );

  se3output->SetPosition( se3setpoints[0] );

}

devSetPoints::devSetPoints
( const std::string& taskname,
  const std::vector< vctDynamicVector<double> >& setpoints ) :

  devRobotComponent( taskname, 0.001, devSetPoints::ENABLED, OSA_CPUANY ), 
  rnsetpoints( setpoints ),
  cnt(0),
  state( false ),
  stateold( false ){
  
  rnoutput = ProvideOutputRn( devSetPoints::Output,
			      devRobotComponent::POSITION,
			      setpoints[0].size() );

  mtsInterfaceProvided* interface;
  interface = GetInterfaceProvided( devRobotComponent::Control );
  StateTable.AddData( state, "State" );
  interface->AddCommandWriteState
    ( StateTable, state, devSetPoints::NextSetPoint );

  rnoutput->SetPosition( rnsetpoints[0] );

}

devSetPoints::devSetPoints
( const std::string& taskname,
  const std::vector< vctFixedSizeVector<double,3> >& setpoints ) : 
  devRobotComponent( taskname, 0.001, devSetPoints::ENABLED, OSA_CPUANY ), 
  r3setpoints( setpoints ),
  cnt(0),
  state( false ),
  stateold( false ){
  
  r3output = ProvideOutputR3( devSetPoints::Output,
			      devRobotComponent::POSITION );

  mtsInterfaceProvided* interface;
  interface = GetInterfaceProvided( devRobotComponent::Control );
  StateTable.AddData( state, "State" );
  interface->AddCommandWriteState
    ( StateTable, state, devSetPoints::NextSetPoint );

  r3output->SetPosition( r3setpoints[0] );

}

devSetPoints::devSetPoints
( const std::string& taskname,
  const std::vector< vctQuaternionRotation3<double> >& setpoints ) :

  devRobotComponent( taskname, 0.001, devSetPoints::ENABLED, OSA_CPUANY ), 
  so3setpoints( setpoints ),
  cnt(0),
  state( false ),
  stateold( false ){
  
  so3output = ProvideOutputSO3( devSetPoints::Output,
				devRobotComponent::POSITION );

  mtsInterfaceProvided* interface;
  interface = GetInterfaceProvided( devRobotComponent::Control );
  StateTable.AddData( state, "State" );
  interface->AddCommandWriteState
    ( StateTable, state, devSetPoints::NextSetPoint );

  so3output->SetRotation( so3setpoints[0] );

}

devSetPoints::devSetPoints
( const std::string& taskname,
  const std::vector< vctFrame4x4<double> >& setpoints ) :
  
  devRobotComponent( taskname, 0.001, devSetPoints::ENABLED, OSA_CPUANY ), 
  se3setpoints( setpoints ),
  cnt(0),
  state( false ),
  stateold( false ){

  se3output = ProvideOutputSE3( devSetPoints::Output,
				devRobotComponent::POSITION );

  se3output->SetPosition( se3setpoints[0] );

  mtsInterfaceProvided* interface;
  interface = GetInterfaceProvided( devRobotComponent::Control );
  StateTable.AddData( state, "State" );
  interface->AddCommandWriteState
    ( StateTable, state, devSetPoints::NextSetPoint );


}
*/

void devSetPoints::Insert( const vctDynamicVector<double>& q )
{ setpoints.push_back( new devSetPoints::SetPointRn( q ) ); }

void devSetPoints::Insert( const vctFrame4x4<double>& Rt )
{ setpoints.push_back( new devSetPoints::SetPointSE3( Rt ) ); }

void devSetPoints::Insert( const vctFrm3& Rt ){
  // Hack to avoid non-normalized rotations!
  const vctMatrixRotation3<double>& R = Rt.Rotation();
  vctQuaternionRotation3<double> q( R, VCT_NORMALIZE );
  vctFrame4x4<double> Rt4x4( q, Rt.Translation() );
  setpoints.push_back( new devSetPoints::SetPointSE3( Rt4x4 ) );
}

void devSetPoints::Latch(){ state = !state; }

void devSetPoints::RunComponent(){

  if( state != stateold ){

    if( !setpoints.empty() ){
    
      devSetPoints::SetPoint* setpoint = setpoints.front();
      
      switch( setpoint->GetType() ){
	
      case SetPoint::RN:
	{
	  devSetPoints::SetPointRn* setpointrn = NULL;
	  setpointrn = dynamic_cast<devSetPoints::SetPointRn*>( setpoint );
	  if( setpointrn!= NULL )
	    { rnoutput->SetPosition( setpointrn->point ); }
    }
	break;

      case devSetPoints::SetPoint::R3:
	{
	  devSetPoints::SetPointR3* setpointr3 = NULL;
	  setpointr3 = dynamic_cast<devSetPoints::SetPointR3*>( setpoint );
	  if( setpointr3!= NULL )
	    { r3output->SetPosition( setpointr3->point ); }
    }
	break;
	  
      case devSetPoints::SetPoint::SO3:
	{
	  devSetPoints::SetPointSO3* setpointso3 = NULL;
	  setpointso3 = dynamic_cast<devSetPoints::SetPointSO3*>( setpoint );
	  if( setpointso3!= NULL )
	    { so3output->SetRotation( setpointso3->point ); }
	}
	break;

      case devSetPoints::SetPoint::SE3:
	{
	  devSetPoints::SetPointSE3* setpointse3 = NULL;
	  setpointse3 = dynamic_cast<devSetPoints::SetPointSE3*>( setpoint );
	  if( setpointse3!= NULL )
	    { se3output->SetPosition( setpointse3->point ); }
    }
	break;

    }

      setpoints.pop_front();
      delete setpoint;

    }

    stateold = state;

  }

}


