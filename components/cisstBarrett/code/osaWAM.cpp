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

#include <cisstBarrett/osaWAM.h>
#include <cisstCommon/cmnLogger.h>

// main constructor
osaWAM::osaWAM(	cisstCAN* canbus,
		osaWAM::Configuration configuration ) :
  
  configuration( configuration ),
  // create the groups
  broadcast(      osaGroup::BROADCAST,         canbus ),
  uppertorques(   osaGroup::UPPERARM,          canbus ),
  lowertorques(   osaGroup::FOREARM,           canbus ),
  upperpositions( osaGroup::UPPERARM_POSITION, canbus ),
  lowerpositions( osaGroup::FOREARM_POSITION,  canbus ),

  // create the safety module
  safetymodule(   osaPuck::SAFETY_MODULE_ID,   canbus ),

  qinit( qinit ) {


  // create the pucks
  pucks.push_back( osaPuck( osaPuck::PUCK_ID1, canbus ) );
  pucks.push_back( osaPuck( osaPuck::PUCK_ID2, canbus ) );
  pucks.push_back( osaPuck( osaPuck::PUCK_ID3, canbus ) );
  pucks.push_back( osaPuck( osaPuck::PUCK_ID4, canbus ) );

  if( GetConfiguration() == osaWAM::WAM_7DOF ){
    pucks.push_back( osaPuck( osaPuck::PUCK_ID5, canbus ) );
    pucks.push_back( osaPuck( osaPuck::PUCK_ID6, canbus ) );
    pucks.push_back( osaPuck( osaPuck::PUCK_ID7, canbus ) );
  }

  if( canbus == NULL )
    { CMN_LOG_RUN_ERROR << "CAN device missing" << std::endl; }

}

osaWAM::~osaWAM(){}

osaWAM::Errno osaWAM::Initialize(){

  // initialize the safety module
  if( safetymodule.InitializeSM() != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Failed to initialize safety module"
		      << std::endl;
    return osaWAM::EFAILURE;
  }

  // initialize each puck
  for( size_t i=0; i<pucks.size(); i++ ){
    if( pucks[i].InitializeMotor() != osaPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << "Failed to initialize puck " << pucks[i].GetID()
			<< std::endl;
      return osaWAM::EFAILURE;
    }
  }


  // initialize the broadcast group
  if( broadcast.Initialize() != osaGroup::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Failed to initialize broadcast group"
		      << std::endl;
    return osaWAM::EFAILURE;
  }

  // initialize the upper arm groups
  if( uppertorques.Initialize() != osaGroup::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Failed to initialize upper torques group"
		      << std::endl;
    return osaWAM::EFAILURE;
  }
  
  if( upperpositions.Initialize() != osaGroup::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Failed to initialize upper positions group"
		      << std::endl;
    return osaWAM::EFAILURE;
  }

  
  switch( GetConfiguration() ){

  case osaWAM::WAM_4DOF:

    // initialize the 4x4 transform matrices
    mpos2jpos.SetSize( 4, 4, VCT_ROW_MAJOR );
    jpos2mpos.SetSize( 4, 4, VCT_ROW_MAJOR );
    jtrq2mtrq.SetSize( 4, 4, VCT_ROW_MAJOR );

    mpos2jpos.SetAll( 0.0 );
    jpos2mpos.SetAll( 0.0 );
    jtrq2mtrq.SetAll( 0.0 );
    
    mpos2jpos[0][0] = -0.0238095;
    mpos2jpos[1][1] =  0.0176991;   mpos2jpos[1][2] = -0.0176991;
    mpos2jpos[2][1] = -0.0297345;   mpos2jpos[2][2] = -0.0297345;
    mpos2jpos[3][3] = -0.0555556;
    
    jpos2mpos[0][0] = -42.0;
    jpos2mpos[1][1] =  28.25;       jpos2mpos[1][2] = -16.8155;
    jpos2mpos[2][1] = -28.25;       jpos2mpos[2][2] = -16.8155;
    jpos2mpos[3][3] = -18.0;
    
    jtrq2mtrq[0][0] = -0.0238095;
    jtrq2mtrq[1][1] =  0.0176991;   jtrq2mtrq[1][2] = -0.0297345;
    jtrq2mtrq[2][1] = -0.0176991;   jtrq2mtrq[2][2] = -0.0297345;
    jtrq2mtrq[3][3] = -0.0555556;
    
    break;

  case osaWAM::WAM_7DOF:
    
    // for 7 dof initialize the lower arm groups
    if( lowertorques.Initialize() != osaGroup::ESUCCESS ){
      CMN_LOG_RUN_ERROR << "Failed to initialize lower torques group"
			<< std::endl;
      return osaWAM::EFAILURE;
    }
    
    if( lowerpositions.Initialize() != osaGroup::ESUCCESS ){
      CMN_LOG_RUN_ERROR << "Failed to initialize lower positions group"
			<< std::endl;
      return osaWAM::EFAILURE;
    }

    // initialize the 7x7 transform matrices
    mpos2jpos.SetSize( 7, 7, VCT_ROW_MAJOR );
    jpos2mpos.SetSize( 7, 7, VCT_ROW_MAJOR );
    jtrq2mtrq.SetSize( 7, 7, VCT_ROW_MAJOR );
    
    mpos2jpos.SetAll( 0.0 );
    jpos2mpos.SetAll( 0.0 );
    jtrq2mtrq.SetAll( 0.0 );

    mpos2jpos[0][0] = -0.0238095;
    mpos2jpos[1][1] =  0.0176991;   mpos2jpos[1][2] = -0.0176991;
    mpos2jpos[2][1] = -0.0297345;   mpos2jpos[2][2] = -0.0297345;
    mpos2jpos[3][3] = -0.0555556;
    
    jpos2mpos[0][0] = -42.0;
    jpos2mpos[1][1] =  28.25;       jpos2mpos[1][2] = -16.8155;
    jpos2mpos[2][1] = -28.25;       jpos2mpos[2][2] = -16.8155;
    jpos2mpos[3][3] = -18.0;
    
    jtrq2mtrq[0][0] = -0.0238095;
    jtrq2mtrq[1][1] =  0.0176991;   jtrq2mtrq[1][2] = -0.0297345;
    jtrq2mtrq[2][1] = -0.0176991;   jtrq2mtrq[2][2] = -0.0297345;
    jtrq2mtrq[3][3] = -0.0555556;

    mpos2jpos[4][4] =  0.0527426; mpos2jpos[4][5] = 0.0527426;
    mpos2jpos[5][4] = -0.0527426; mpos2jpos[5][5] = 0.0527426;
    mpos2jpos[6][6] = -0.0669792;

    jpos2mpos[4][4] =   9.48;     jpos2mpos[4][5] = -9.48;
    jpos2mpos[5][4] =   9.48;     jpos2mpos[5][5] =  9.48;
    jpos2mpos[6][6] = -14.93;

    jtrq2mtrq[4][4] =  0.0527426; jtrq2mtrq[4][5] = -0.0527426;
    jtrq2mtrq[5][4] =  0.0527426; jtrq2mtrq[5][5] =  0.0527426;
    jtrq2mtrq[6][6] = -0.0669792;

    break;

  }

  return osaWAM::ESUCCESS;

}


osaWAM::Errno osaWAM::SetVelocityWarning( Barrett::Value vw ){

  if( safetymodule.SetVelocityWarning( vw ) != osaSafetyModule::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Unable to set the velocity warning" << std::endl;
    return osaWAM::EFAILURE;
  }

  return osaWAM::ESUCCESS;

}

osaWAM::Errno osaWAM::SetVelocityFault( Barrett::Value vf ){

  if( safetymodule.SetVelocityFault( vf ) != osaSafetyModule::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Unable to set the velocity fault" << std::endl;
    return osaWAM::EFAILURE;
  }

  return osaWAM::ESUCCESS;
}

osaWAM::Errno osaWAM::SetTorqueWarning( Barrett::Value tw ){
  
  if( safetymodule.SetTorqueWarning( tw ) != osaSafetyModule::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Unable to set the torques warning" << std::endl;
    return osaWAM::EFAILURE;
  }
  return osaWAM::ESUCCESS;
}

osaWAM::Errno osaWAM::SetTorqueFault( Barrett::Value tf ){

  if( safetymodule.SetTorqueFault( tf ) != osaSafetyModule::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Unable to set the torques fault" << std::endl;
    return osaWAM::EFAILURE;
  }
  return osaWAM::ESUCCESS;
}

osaWAM::Errno osaWAM::SetMode( Barrett::Value mode ){

  if( broadcast.SetMode( mode ) != osaGroup::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Failed to set mode" << std::endl;
    return osaWAM::EFAILURE;
  }
  return osaWAM::ESUCCESS;

}

// set the motor positions 
osaWAM::Errno osaWAM::SetPositions( const vctDynamicVector<double>& jq ){

  // sanity check
  if( jq.size() != pucks.size() ){
    CMN_LOG_RUN_ERROR << "Expected " << pucks.size() << " joint angles. "
		      << "Got " << jq.size()
		      << std::endl;
    return osaWAM::EFAILURE;
  }

  // let the safety module ignore a few faults
  // this is necessary because otherwise the safety module will monitor a large
  // change of joint position in a short amount of time and trigger a velocity 
  // fault.  
  if( safetymodule.IgnoreFault( 8 ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Failed to configure the safety module" << std::endl;
    return osaWAM::EFAILURE;
  }
  
  // convert the joints positions to motor positions
  vctDynamicVector<double> mq = JointsPos2MotorsPos( jq );
  
  // for each puck, send a position 
  for(size_t i=0; i<pucks.size(); i++){

    // Set the motor position
    if( pucks[i].SetPosition( mq[i] ) != osaPuck::ESUCCESS ){
      CMN_LOG_RUN_ERROR << "Failed to set pos of puck#: " 
			<< (int)pucks[i].GetID()
			<< std::endl;
    }

  }

  // let the safety module ignore a few faults
  // this is necessary because otherwise the safety module will monitor a large
  // change of joint position in a short amount of time and trigger a velocity 
  // fault.  
  if( safetymodule.IgnoreFault( 1 ) != osaPuck::ESUCCESS ){
    CMN_LOG_RUN_ERROR << "Failed to configure the safety module" << std::endl;
    return osaWAM::EFAILURE;
  }
  
  return osaWAM::ESUCCESS;
}


// query the joint positions
osaWAM::Errno osaWAM::GetPositions( vctDynamicVector<double>& jq ){

  switch( GetConfiguration() ){

  case osaWAM::WAM_4DOF:

    {
      vctDynamicVector<double> mq;
      if( upperpositions.GetPositions( mq ) != osaGroup::ESUCCESS ){
	CMN_LOG_RUN_ERROR << "Failed to get the upper arm positions"<<std::endl;
	return osaWAM::EFAILURE;
      }
      jq = MotorsPos2JointsPos( mq );
    }

    break;

  case osaWAM::WAM_7DOF:

    {
      vctDynamicVector<double> mqu, mql;
      
      if( upperpositions.GetPositions( mqu ) != osaGroup::ESUCCESS ){
	CMN_LOG_RUN_ERROR << "Failed to get the upper arm positions"<<std::endl;
	return osaWAM::EFAILURE;
      }
      
      if( lowerpositions.GetPositions( mql ) != osaGroup::ESUCCESS ){
	CMN_LOG_RUN_ERROR << "Failed to get the lower arm positions"<<std::endl;
	return osaWAM::EFAILURE;
      }
      
      vctDynamicVector<double> mq( 7,
				   mqu[0], mqu[1], mqu[2], mqu[3], 
				   mql[0], mql[1], mql[2] );
      
      jq = MotorsPos2JointsPos( mq );
    }

    break;

  }

  return osaWAM::ESUCCESS;

}

osaWAM::Errno osaWAM::SetTorques( const vctDynamicVector<double>& jt ){

  switch( GetConfiguration() ){

  case osaWAM::WAM_4DOF:

    if( jt.size() != 4 ){
      vctDynamicVector<double> mt = JointsTrq2MotorsTrq( jt );

      vctFixedSizeVector<double,4> mtu( mt[0], mt[1], mt[2], mt[3] );
      if( uppertorques.SetTorques( mtu ) != osaGroup::ESUCCESS ){
	CMN_LOG_RUN_ERROR << "Failed to set the upper arm torques" << std::endl;
	return osaWAM::EFAILURE;
      }
      
    }
    else{
      CMN_LOG_RUN_ERROR << "Expected 4 values. Got " << jt.size() << std::endl;
      return osaWAM::EFAILURE;
    }

    break;
    
  case osaWAM::WAM_7DOF:

    if( jt.size() == 7 ){

      vctDynamicVector<double> mt = JointsTrq2MotorsTrq( jt );
      
      vctFixedSizeVector<double,4> mtu( mt[0], mt[1], mt[2], mt[3] );
      if( uppertorques.SetTorques( mtu ) != osaGroup::ESUCCESS ){
	CMN_LOG_RUN_ERROR << "Failed to set the upper arm torques" << std::endl;
	return osaWAM::EFAILURE;
      }
      
      vctFixedSizeVector<double,4> mtl( mt[4], mt[5], mt[6], 0.0 );
      if( lowertorques.SetTorques( mtl ) != osaGroup::ESUCCESS ){
	CMN_LOG_RUN_ERROR << "Failed to set the lower arm torques" << std::endl;
	return osaWAM::EFAILURE;
      }

    }      
    else{
      CMN_LOG_RUN_ERROR << "Expected 7 values. Got " << jt.size() << std::endl;
      return osaWAM::EFAILURE;
    }

    break;

  }

  return osaWAM::ESUCCESS;

}

vctDynamicVector<double> 
osaWAM::MotorsPos2JointsPos( const vctDynamicVector<double>& mq )
{  return mpos2jpos*mq;  }

vctDynamicVector<double> 
osaWAM::JointsPos2MotorsPos( const vctDynamicVector<double>& jq )
{  return jpos2mpos*jq;  }

vctDynamicVector<double> 
osaWAM::JointsTrq2MotorsTrq( const vctDynamicVector<double>& jt )
{  return jtrq2mtrq*jt;  }

