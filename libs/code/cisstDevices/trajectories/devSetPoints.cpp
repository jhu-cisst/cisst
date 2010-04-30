#include <cisstDevices/trajectories/devSetPoints.h>
#include <cisstVector/vctMatrixRotation3.h>

const std::string devSetPoints::OutputInterface = "OutputInterface";
const std::string devSetPoints::GetRnSetPoint   = "GetRnSetPoint";
const std::string devSetPoints::GetSE3SetPoint  = "GetSE3SetPoint";
    
const std::string devSetPoints::ControlInterface = "ControlInterface";
const std::string devSetPoints::NextSetPoint     = "NextSetPoint";

devSetPoints::devSetPoints
( const std::string& taskname,
  const std::vector< vctDynamicVector<double> >& setpoints ) :

  mtsTaskPeriodic( taskname, 0.1, true ), 
  rnsetpoints( setpoints ),
  cnt(0),
  state( false ),
  stateold( false ){
  
  mtsProvidedInterface* interface;
  interface = AddProvidedInterface( devSetPoints::OutputInterface );
  if( interface && !setpoints.empty() ){
    rnoutput.SetSize( setpoints[0].size() );
    rnoutput = setpoints[0];
    StateTable.AddData( rnoutput, "RnOutput" );
    interface->AddCommandReadState
      ( StateTable, rnoutput, devSetPoints::GetRnSetPoint );
  }
  
  interface = AddProvidedInterface( devSetPoints::ControlInterface );
  if( interface ){
    StateTable.AddData( state, "State" );
    interface->AddCommandWriteState
      ( StateTable, state, devSetPoints::NextSetPoint );
  }

}

devSetPoints::devSetPoints
( const std::string& taskname,
  const std::vector< vctFrame4x4<double> >& setpoints ) :

  mtsTaskPeriodic( taskname, 0.1, true ), 
  cnt(0),
  state( false ),
  stateold( false ){


  for( size_t i=0; i<setpoints.size(); i++ ){
    
    vctFrame4x4<double> Rt( setpoints[i] );
    vctFixedSizeVector<double,3> t = Rt.Translation();
    vctMatrixRotation3<double> R( Rt[0][0], Rt[0][1], Rt[0][2],
				  Rt[1][0], Rt[1][1], Rt[1][2],
				  Rt[2][0], Rt[2][1], Rt[2][2],
				  VCT_NORMALIZE );
    vctQuaternionRotation3<double> q( R );
    vctDynamicVector<double> rt(7, 0.0);
    rt[0] = t[0];    rt[1] = t[1];    rt[2] = t[2];
    rt[3] = q.X();   rt[4] = q.Y();   rt[5] = q.Z();   rt[6] = q.R();

    se3setpoints.push_back( rt );
    
  }

  // The manipulator provides an input interface
  mtsProvidedInterface* interface;
  interface = AddProvidedInterface( devSetPoints::OutputInterface );
  
  if( interface && 0 < !setpoints.empty() ){
    se3output.SetSize( 7 );
    se3output = se3setpoints[0];
    StateTable.AddData( se3output, "SE3Output" );
    interface->AddCommandReadState
      ( StateTable, se3output, devSetPoints::GetSE3SetPoint );
  }

  interface = AddProvidedInterface( devSetPoints::ControlInterface );
  if( interface ){
    StateTable.AddData( state, "State" );
    interface->AddCommandWriteState
      ( StateTable, state, devSetPoints::NextSetPoint );
  }

}

void devSetPoints::Run(void){

  ProcessQueuedCommands();

  if( state != stateold ){

    if( !rnsetpoints.empty() ){
      cnt++;
      if( cnt < rnsetpoints.size() )
	rnoutput = rnsetpoints[cnt];
    }

    if( !se3output.empty() ){
      cnt++;
      if( cnt < se3setpoints.size() )
	se3output = se3setpoints[cnt];
    }
    stateold = state;
  }

}


