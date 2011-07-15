#include <cisstDevices/robotcomponents/ode/devODEBH.h>

devODEBH::devODEBH( const std::string& devname,
		    double period,
		    devManipulator::State state,
		    osaCPUMask cpumask,
		    devODEWorld* world,
		    devManipulator::Mode mode,
		    const std::string& f1f2filename,
		    const std::string& f3filename,
		    const vctFrame4x4<double>& Rtw0,
		    const std::string& palmmodel,
		    const std::string& metacarpalmodel,
		    const std::string& proximalmodel,
		    const std::string& intermediatemodel ) :
  devODEManipulator( devname, period, state, cpumask, world, mode ),
  f1( NULL ),
  f2( NULL ),
  f3( NULL ){
  //Rtw0( Rtw0 ) {

  // phone nertia tensor 
  vctFixedSizeMatrix<double,3,3> I(0.0);
  I[0][0] = ( 0.04*0.04 + 0.05*0.05 ) / 3.0;
  I[1][1] = ( 0.02*0.02 + 0.05*0.05 ) / 3.0;
  I[2][2] = ( 0.02*0.02 + 0.04*0.04 ) / 3.0;

  // Create the transformation of the base
  vctFrame4x4<double> Rt0b( vctMatrixRotation3<double>(),
			    vctFixedSizeVector<double,3>( 0.0, 0.0, 0.091 ) );
  vctFrame4x4<double> Rtwb = Rtw0 * Rt0b;

  // Create the body for the base (palm)
  base = new devODEBody( GetName() + "palm",      // name of the body
			 Rtwb,                    // position/orientation
			 palmmodel,               // geometry
			 world,                   // the world
			 1.17,                    // mass
			 vctFixedSizeVector<double,3>(0.0, 0.0, -0.025), // com
			 I,                       // iniertia tensor
			 world->GetSpaceID() );   // ODE world space

  std::vector<std::string> modelsf1f2;
  modelsf1f2.push_back( metacarpalmodel );
  modelsf1f2.push_back( proximalmodel );
  modelsf1f2.push_back( intermediatemodel );

  vctFrame4x4<double> Rtbf1( vctMatrixRotation3<double>(),
			     vctFixedSizeVector<double,3>( 0.0,  0.026,-0.0105));
  vctFrame4x4<double> Rtbf2( vctMatrixRotation3<double>(),
			     vctFixedSizeVector<double,3>( 0.0, -0.026,-0.0105));

  f1 = new devODEManipulator( devname + "F1",
			      period,
			      state,
			      cpumask,
			      world,
			      mode,
			      f1f2filename,
			      Rtwb*Rtbf1,
			      vctDynamicVector<double>( 3, 0.0 ),
			      modelsf1f2,
			      base );

  f2 = new devODEManipulator( devname + "F2",
			      period,
			      state,
			      cpumask,
			      world,
			      mode,
			      f1f2filename,
			      Rtwb*Rtbf2,
			      vctDynamicVector<double>( 3, 0.0 ),
			      modelsf1f2,
			      base );

  std::vector<std::string> modelsf3;
  modelsf3.push_back( proximalmodel );
  modelsf3.push_back( intermediatemodel );
  vctFrame4x4<double> Rtbf3( vctMatrixRotation3<double>( 1.0, 0.0, 0.0,
							 0.0, 0.0,-1.0,
							 0.0, 1.0, 0.0 ),
			     vctFixedSizeVector<double,3>( 0.05, 0.0,-0.0105));

  f3 = new devODEManipulator( devname + "F3",
			      period,
			      state,
			      cpumask,
			      world,
			      mode,
			      f3filename,
			      Rtwb*Rtbf3,
			      vctDynamicVector<double>( 2, 0.0 ),
			      modelsf3,
			      base );

}

devODEBH::devODEBH( const std::string& devname,
		    double period,
		    devManipulator::State state,
		    osaCPUMask cpumask,
		    devODEWorld* world,
		    devManipulator::Mode mode,
		    const std::string& f1f2filename,
		    const std::string& f3filename,
		    const vctFrm3& Rtw0,
		    const std::string& palmmodel,
		    const std::string& metacarpalmodel,
		    const std::string& proximalmodel,
		    const std::string& intermediatemodel ) :
  devODEManipulator( devname, period, state, cpumask, world, mode ),
  f1( NULL ),
  f2( NULL ),
  f3( NULL ){
  //Rtw0( vctFrame4x4<double>( Rtw0.Rotation(), Rtw0.Translation()) ){

  // phone nertia tensor 
  vctFixedSizeMatrix<double,3,3> I(0.0);
  I[0][0] = ( 0.04*0.04 + 0.05*0.05 ) / 3.0;
  I[1][1] = ( 0.02*0.02 + 0.05*0.05 ) / 3.0;
  I[2][2] = ( 0.02*0.02 + 0.04*0.04 ) / 3.0;

  // Create the transformation of the base
  vctFrame4x4<double> Rt0b( vctMatrixRotation3<double>(),
			    vctFixedSizeVector<double,3>( 0.0, 0.0, 0.09 ) );
  vctFrame4x4<double> Rtwb;
  Rtwb = vctFrame4x4<double>( Rtw0.Rotation(), Rtw0.Translation() ) * Rt0b;

  // Create the body for the base (palm)
  base = new devODEBody( devname + "palm",        // name of the body
			 Rtwb,                    // position/orientation
			 palmmodel,               // geometry
			 world,                   // the world
			 0.17,                    // mass
			 vctFixedSizeVector<double,3>(0.0, 0.0, -0.025), // com
			 I,                       // iniertia tensor
			 world->GetSpaceID() );   // ODE world space

  std::vector<std::string> modelsf1f2;
  modelsf1f2.push_back( metacarpalmodel );
  modelsf1f2.push_back( proximalmodel );
  modelsf1f2.push_back( intermediatemodel );

  vctFrame4x4<double> Rtbf1( vctMatrixRotation3<double>(),
			     vctFixedSizeVector<double,3>( 0.0,  0.026,-0.0105));
  vctFrame4x4<double> Rtbf2( vctMatrixRotation3<double>(),
			     vctFixedSizeVector<double,3>( 0.0, -0.026,-0.0105));

  f1 = new devODEManipulator( devname + "F1",
			      period,
			      state,
			      cpumask,
			      world,
			      mode,
			      f1f2filename,
			      Rtwb*Rtbf1,
			      vctDynamicVector<double>( 3, 0.0 ),
			      modelsf1f2,
			      base );

  f2 = new devODEManipulator( devname + "F2",
			      period,
			      state,
			      cpumask,
			      world,
			      mode,
			      f1f2filename,
			      Rtwb*Rtbf2,
			      vctDynamicVector<double>( 3, 0.0 ),
			      modelsf1f2,
			      base );

  std::vector<std::string> modelsf3;
  modelsf3.push_back( proximalmodel );
  modelsf3.push_back( intermediatemodel );
  vctFrame4x4<double> Rtbf3( vctMatrixRotation3<double>( 1.0, 0.0, 0.0,
							 0.0, 0.0,-1.0,
							 0.0, 1.0, 0.0 ),
			     vctFixedSizeVector<double,3>( 0.05, 0.0,-0.0105));

  f3 = new devODEManipulator( devname + "F3",
			      period,
			      state,
			      cpumask,
			      world,
			      mode,
			      f3filename,
			      Rtwb*Rtbf3,
			      vctDynamicVector<double>( 2, 0.0 ),
			      modelsf3,
			      base );
}

devODEBH::~devODEBH(){}

void devODEBH::Read()
{ output->SetPosition( GetJointsPositions() ); }

void devODEBH::Write(){
  vctDynamicVector<double> q(4,0.0);
  double t;
  input->GetPosition( q, t ); 
  SetPositions( q );
}

devODEBH::Errno devODEBH::SetPositions( const vctDynamicVector<double>& qs ){

  if( qs.size() == 4 ){

    if( f1 != NULL ){ 
      vctDynamicVector<double> q( 3, qs[3], qs[0], qs[0]*45.0/140.0 );
      if( f1->SetPositions( q ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set position of finger 1."
			  << std::endl;
	return devODEBH::EFAILURE;
      }
    }

    if( f2 != NULL ){
      vctDynamicVector<double> q( 3, -qs[3], qs[1], qs[1]*45.0/140.0 );
      if( f2->SetPositions( q ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set position of finger 2."
			  << std::endl;
	return devODEBH::EFAILURE;
      }
    }

    if( f3 != NULL ){
      vctDynamicVector<double> q( 2,         qs[2], qs[2]*45.0/140.0 );
      if( f3->SetPositions( q ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set position of finger 3."
			  << std::endl;
	return devODEBH::EFAILURE;
      }
      
    }

    return devODEBH::ESUCCESS;

  }
  
  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Expected 4 joints values. Got " << qs.size()
		      << std::endl;

    return devODEBH::EFAILURE;  
  }

}

devODEBH::Errno devODEBH::SetVelocities( const vctDynamicVector<double>& qds ){

  if( qds.size() == 4 ){

    if( f1 != NULL ){ 
      vctDynamicVector<double> qd( 3, qds[3], qds[0], qds[0]*45.0/140.0 );
      if( f1->SetVelocities( qd ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set velocities of finger 1."
			  << std::endl;
	return devODEBH::EFAILURE;
      }
    }

    if( f2 != NULL ){
      vctDynamicVector<double> qd( 3, -qds[3], qds[1], qds[1]*45.0/140.0 );
      if( f2->SetVelocities( qd ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set velocities of finger 2."
			  << std::endl;
	return devODEBH::EFAILURE;
      }
    }

    if( f3 != NULL ){
      vctDynamicVector<double> qd( 2,         qds[2], qds[2]*45.0/140.0 );
      if( f3->SetVelocities( qd ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set position of finger 3."
			  << std::endl;
	return devODEBH::EFAILURE;
      }
      
    }

    return devODEBH::ESUCCESS;
  }

  CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		    << "Expected 4 joints velocities. Got " << qds.size()
		    << std::endl;

  return devODEBH::EFAILURE;  

}

devODEBH::Errno 
devODEBH::SetForcesTorques( const vctDynamicVector<double>& fts ){

  if( fts.size() == 4 ){

    if( f1 != NULL ){ 
      vctDynamicVector<double> ft( 3, fts[3], fts[0], fts[0]*45.0/140.0 );
      if( f1->SetForcesTorques( ft ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set F/T of finger 1."
			  << std::endl;
	return devODEBH::EFAILURE;
      }
    }

    if( f2 != NULL ){
      vctDynamicVector<double> ft( 3, -fts[3], fts[1], fts[1]*45.0/140.0 );
      if( f2->SetForcesTorques( ft ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set F/T of finger 2."
			  << std::endl;
	return devODEBH::EFAILURE;
      }
    }

    if( f3 != NULL ){
      vctDynamicVector<double> ft( 2,         fts[2], fts[2]*45.0/140.0 );
      if( f3->SetForcesTorques( ft ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set F/T of finger 3."
			  << std::endl;
	return devODEBH::EFAILURE;
      }
      
    }

    return devODEBH::ESUCCESS;
  }

  CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		    << "Expected 4 joints values. Got " << fts.size()
		    << std::endl;

  return devODEBH::EFAILURE;  

}










