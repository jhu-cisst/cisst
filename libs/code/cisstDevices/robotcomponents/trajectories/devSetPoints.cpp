#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstDevices/robotcomponents/trajectories/devSetPoints.h>
#include <cisstVector/vctMatrixRotation3.h>

const std::string devSetPoints::Output          = "Output";
const std::string devSetPoints::NextSetPoint     = "NextSetPoint";

devSetPoints::devSetPoints( const std::string& taskname,
			    const vctDynamicVector<double>& q ) :
  devRobotComponent( taskname, 0.001, devSetPoints::ENABLED, OSA_CPUANY ),
  cnt(0),
  state( false ),
  stateold( false ){

  rnsetpoints.push_back( q );
  rnoutput = ProvideOutputRn( devSetPoints::Output,
			      devRobotComponent::POSITION,
			      rnsetpoints[0].size() );

  mtsInterfaceProvided* interface;
  interface = GetInterfaceProvided( devRobotComponent::Control );
  StateTable.AddData( state, "State" );
  interface->AddCommandWriteState
    ( StateTable, state, devSetPoints::NextSetPoint );

  rnoutput->SetPosition( rnsetpoints[0] );
}

void devSetPoints::Insert( const vctDynamicVector<double>& q )
{ rnsetpoints.push_back( q ); }

void devSetPoints::Latch(){ state = ~state; }

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

void devSetPoints::RunComponent(){

  if( state != stateold ){
    
    if( !rnsetpoints.empty() ){
      cnt++;
      if( cnt < rnsetpoints.size() )
	{ rnoutput->SetPosition( rnsetpoints[cnt] ); }
    }

    if( !r3setpoints.empty() ){
      cnt++;
      if( cnt < r3setpoints.size() )
	{ r3output->SetPosition( r3setpoints[cnt] ); }
    }

    if( !so3setpoints.empty() ){
      cnt++;
      if( cnt < so3setpoints.size() )
	{ so3output->SetRotation( so3setpoints[cnt] ); }
    }

    if( !se3setpoints.empty() ){
      cnt++;
      if( cnt < se3setpoints.size() )
	{ se3output->SetPosition( se3setpoints[cnt] ); }
    }

    stateold = state;
  }

}


