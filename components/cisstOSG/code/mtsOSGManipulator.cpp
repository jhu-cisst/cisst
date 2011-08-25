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

#include <cisstOSG/mtsOSGManipulator.h>
#include <cisstCommon/cmnLogger.h>

// main constructor
mtsOSGManipulator::mtsOSGManipulator( const std::string& name,
				      double period,
				      osaCPUMask cpumask,
				      int priority,
				      const std::vector<std::string>& models,
				      cisstOSGWorld* world,
				      const vctFrame4x4<double>& Rtw0,
				      const std::string& robfilename,
				      const std::string& basemodel ):
  mtsTaskPeriodic( name, period, true ),
  cisstOSGManipulator( models, world, Rtw0, robfilename, basemodel ),
  input( NULL ),
  output( NULL ),
  ctl( NULL ),
  cpumask( cpumask ),
  priority( priority ){

  qout.SetSize( links.size() );
  qin.SetSize( links.size() );

  qout.Position().SetAll( 0.0 );
  qin.Goal().SetAll( 0.0 );

  input = AddInterfaceProvided( "Input" );
  if( input ){
    
    StateTable.AddData( qin, "PositionsInput" );
    input->AddCommandWriteState( StateTable, qin, "SetPositionJoint" );

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

// main constructor
mtsOSGManipulator::mtsOSGManipulator( const std::string& name,
				      double period,
				      osaCPUMask cpumask,
				      int priority,
				      const std::vector<std::string>& models,
				      cisstOSGWorld* world,
				      const vctFrm3& Rtw0,
				      const std::string& robfilename,
				      const std::string& basemodel ):
  mtsTaskPeriodic( name, period, true ),
  cisstOSGManipulator( models, world, Rtw0, robfilename, basemodel ),
  input( NULL ),
  output( NULL ),
  ctl( NULL ),
  cpumask( cpumask ),
  priority( priority ){

  qout.SetSize( links.size() );
  qin.SetSize( links.size() );

  qout.Position().SetAll( 0.0 );
  qin.Goal().SetAll( 0.0 );

  input = AddInterfaceProvided( "Input" );
  if( input ){
    
    StateTable.AddData( qin, "PositionsInput" );
    input->AddCommandWriteState( StateTable, qin, "SetPositionJoint" );

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

void mtsOSGManipulator::Startup(){}

void mtsOSGManipulator::Run(){

  ProcessQueuedCommands();

  if( cisstOSGManipulator::SetPositions( qin.Goal() ) != 
      cisstOSGManipulator::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Failed to get position for " << GetName() 
		      << std::endl;
  }

  if( cisstOSGManipulator::GetPositions( qout.Position()) != 
      cisstOSGManipulator::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Failed to get position for " << GetName() 
		      << std::endl;
  }

}
