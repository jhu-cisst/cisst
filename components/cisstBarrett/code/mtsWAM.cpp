/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstBarrett/mtsWAM.h>
#include <cisstCommon/cmnLogger.h>

// main constructor
mtsWAM::mtsWAM( const std::string& name,
	        cisstCAN* canbus,
		osaCPUMask cpumask,
		int priority,
		osaWAM::Configuration configuration ) :
  mtsTaskContinuous( name ),
  osaWAM( canbus, configuration ),
  input( NULL ),
  output( NULL ),
  ctl( NULL ),
  cpumask( cpumask ),
  priority( priority ){

  switch( GetConfiguration() ){

  case osaWAM::WAM_4DOF:
    qout.SetSize( 4 );
    tin.SetSize( 4 );
    break;


  case osaWAM::WAM_7DOF:
    qout.SetSize( 7 );
    tin.SetSize( 7 );
    break;

  }

  qout.Position().SetAll( 0.0 );
  tin.ForceTorque().SetAll( 0.0 );

  input = AddInterfaceProvided( "Input" );
  if( input ){
    
    StateTable.AddData( tin, "TorquesInput" );
    input->AddCommandWriteState( StateTable, tin, "SetTorqueJoint" );

  }
  else{
    CMN_LOG_RUN_ERROR << "Failed to create interface Input for " << GetName()
		      << std::endl;
  }

  output = AddInterfaceProvided( "Output" );
  if( output ){

    StateTable.AddData( qout, "PositionsOutput" );
    output->AddCommandReadState( StateTable, qout, "GetPositionJoint" );

  }
  else{
    CMN_LOG_RUN_ERROR << "Failed to create interface Output for " << GetName()
		      << std::endl;
  }

}

void mtsWAM::Startup(){
}

void mtsWAM::Run(){

  ProcessQueuedCommands();

  if( osaWAM::GetPositions( qout.Position() ) != osaWAM::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Failed to get position for " << GetName() 
		      << std::endl;
  }

  if( osaWAM::SetTorques( tin.ForceTorque() ) != osaWAM::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Failed to get position for " << GetName() 
		      << std::endl;
  }

}
