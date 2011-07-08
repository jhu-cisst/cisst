#include <cisstDevices/robotcomponents/ode/devODEGripper.h>

devODEGripper::devODEGripper( const std::string& devname,
			      double period,
			      devManipulator::State state,
			      osaCPUMask cpumask,
			      devODEWorld* world,
			      devManipulator::Mode inputmode,
			      const std::string& f1filename,
			      const std::string& f2filename,
			      const vctFrame4x4<double>& Rtw0,
			      const std::string& palmmodel,
			      const std::string& f1modelname,
			      const std::string& f2modelname ) :
  devODEManipulator( devname, period, state, cpumask, world, inputmode ),
  f1( NULL ),
  f2( NULL ){

  // inertia tensor 
  vctFixedSizeMatrix<double,3,3> I(0.0);
  I[0][0] = ( 0.04*0.04 + 0.05*0.05 ) / 3.0;
  I[1][1] = ( 0.02*0.02 + 0.05*0.05 ) / 3.0;
  I[2][2] = ( 0.02*0.02 + 0.04*0.04 ) / 3.0;

  // Create the transformation of the base
  vctFrame4x4<double> Rt0b( vctMatrixRotation3<double>(),
			    vctFixedSizeVector<double,3>( 0.0, 0.0, 0.18 ) );
  vctFrame4x4<double> Rtwb = Rtw0 * Rt0b;

  // Create the body for the base (palm)
  base = new devODEBody( GetName() + "palm",      // name of the body
			 Rtwb,                    // position/orientation
			 palmmodel,               // geometry
			 world,                   // the world
			 1.17,                    // mass
			 vctFixedSizeVector<double,3>(0.0), // com
			 I,                       // iniertia tensor
			 world->GetSpaceID() );   // ODE world space

  //base = NULL;
  std::vector<std::string> f1model;
  f1model.push_back( f1modelname );

  // Rotate about X c-wise
  vctMatrixRotation3<double> Rbf1(  1.0,  0.0,  0.0,
				    0.0,  0.0, -1.0,
				    0.0,  1.0,  0.0  );  
  // base position/orientation of finger 1
  vctFrame4x4<double> Rtbf1( Rbf1, 
			     vctFixedSizeVector<double,3>(0.0,  0.05, 0.0 ) );

  f1 = new devODEManipulator( devname + "F1",
			      period,
			      state,
			      cpumask,
			      world,
			      inputmode,
			      f1filename,
			      Rtwb*Rtbf1,
			      vctDynamicVector<double>( 1, 0.0 ),
			      f1model,
			      base );

  std::vector<std::string> f2model;
  f2model.push_back( f2modelname );

  // Rotate about X cc-wise
  vctMatrixRotation3<double> Rbf2(  1.0,  0.0,  0.0,
				    0.0,  0.0,  1.0,
				    0.0, -1.0,  0.0  );
  // base position/orientation of finger 2
  vctFrame4x4<double> Rtbf2( Rbf2,
			     vctFixedSizeVector<double,3>(0.0, -0.05, 0.0 ) );

  f2 = new devODEManipulator( devname + "F2",
			      period,
			      state,
			      cpumask,
			      world,
			      inputmode,
			      f2filename,
			      Rtwb*Rtbf2,
			      vctDynamicVector<double>( 1, 0.0 ),
			      f2model,
			      base );

}

devODEGripper::devODEGripper( const std::string& devname,
			      double period,
			      devManipulator::State state,
			      osaCPUMask cpumask,
			      devODEWorld* world,
			      devManipulator::Mode inputmode,
			      const std::string& f1filename,
			      const std::string& f2filename,
			      const vctFrm3& Rtw0,
			      const std::string& palmmodel,
			      const std::string& f1modelname,
			      const std::string& f2modelname ) :
  devODEManipulator( devname, period, state, cpumask, world, inputmode ),
  f1( NULL ),
  f2( NULL ){

  // inertia tensor 
  vctFixedSizeMatrix<double,3,3> I(0.0);
  I[0][0] = ( 0.04*0.04 + 0.05*0.05 ) / 3.0;
  I[1][1] = ( 0.02*0.02 + 0.05*0.05 ) / 3.0;
  I[2][2] = ( 0.02*0.02 + 0.04*0.04 ) / 3.0;

  // Create the transformation of the base
  vctFrame4x4<double> Rt0b( vctMatrixRotation3<double>(),
			    vctFixedSizeVector<double,3>( 0.0, 0.0, 0.18 ) );
  vctFrame4x4<double> Rtwb;
  Rtwb = vctFrame4x4<double>( Rtw0.Rotation(), Rtw0.Translation() ) * Rt0b;

  // Create the body for the base (palm)
  base = new devODEBody( GetName() + "palm",      // name of the body
			 Rtwb,                    // position/orientation
			 palmmodel,               // geometry
			 world,                   // the world
			 1.17,                    // mass
			 vctFixedSizeVector<double,3>(0.0), // com
			 I,                       // iniertia tensor
			 world->GetSpaceID() );   // ODE world space

  //base = NULL;
  std::vector<std::string> f1model;
  f1model.push_back( f1modelname );

  // Rotate about X c-wise
  vctMatrixRotation3<double> Rbf1(  1.0,  0.0,  0.0,
				    0.0,  0.0, -1.0,
				    0.0,  1.0,  0.0  );  
  // base position/orientation of finger 1
  vctFrame4x4<double> Rtbf1( Rbf1, 
			     vctFixedSizeVector<double,3>(0.0,  0.05, 0.0 ) );

  f1 = new devODEManipulator( devname + "F1",
			      period,
			      state,
			      cpumask,
			      world,
			      inputmode,
			      f1filename,
			      Rtwb*Rtbf1,
			      vctDynamicVector<double>( 1, 0.0 ),
			      f1model,
			      base );

  std::vector<std::string> f2model;
  f2model.push_back( f2modelname );

  // Rotate about X cc-wise
  vctMatrixRotation3<double> Rbf2(  1.0,  0.0,  0.0,
				    0.0,  0.0,  1.0,
				    0.0, -1.0,  0.0  );
  // base position/orientation of finger 2
  vctFrame4x4<double> Rtbf2( Rbf2,
			     vctFixedSizeVector<double,3>(0.0, -0.05, 0.0 ) );

  f2 = new devODEManipulator( devname + "F2",
			      period,
			      state,
			      cpumask,
			      world,
			      inputmode,
			      f2filename,
			      Rtwb*Rtbf2,
			      vctDynamicVector<double>( 1, 0.0 ),
			      f2model,
			      base );

}

devODEGripper::~devODEGripper(){}

void devODEGripper::Read()
{ output->SetPosition( GetJointsPositions() ); }

void devODEGripper::Write(){

  vctDynamicVector<double> q;
  double t;
  input->GetPosition( q, t ); 
  SetPositions( q );

}

devODEGripper::Errno 
devODEGripper::SetPositions( const vctDynamicVector<double>& qs ){

  if( qs.size() == 1 ){

    if( f1 != NULL ){ 
      if( f1->SetPositions( qs ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set position of finger 1."
			  << std::endl;
	return devODEGripper::EFAILURE;
      }
    }

    if( f2 != NULL ){
      if( f2->SetPositions( qs ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set position of finger 2."
			  << std::endl;
	return devODEGripper::EFAILURE;
      }
    }

    return devODEGripper::ESUCCESS;

  }
  
  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << "Expected 1 joints values. Got " << qs.size()
		      << std::endl;

    return devODEGripper::EFAILURE;  
  }

}

devODEGripper::Errno
devODEGripper::SetVelocities( const vctDynamicVector<double>& qds ){

  if( qds.size() == 1 ){

    if( f1 != NULL ){ 
      if( f1->SetVelocities( qds ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set velocities of finger 1."
			  << std::endl;
	return devODEGripper::EFAILURE;
      }
    }

    if( f2 != NULL ){
      if( f2->SetVelocities( qds ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set velocities of finger 2."
			  << std::endl;
	return devODEGripper::EFAILURE;
      }
    }

    return devODEGripper::ESUCCESS;
  }

  CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		    << "Expected 4 joints velocities. Got " << qds.size()
		    << std::endl;

  return devODEGripper::EFAILURE;  

}

devODEGripper::Errno 
devODEGripper::SetForcesTorques( const vctDynamicVector<double>& fts ){

  if( fts.size() == 1 ){

    if( f1 != NULL ){ 
      if( f1->SetForcesTorques( fts ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set F/T of finger 1."
			  << std::endl;
	return devODEGripper::EFAILURE;
      }
    }

    if( f2 != NULL ){
      if( f2->SetForcesTorques( fts ) != devODEManipulator::ESUCCESS ){
	CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
			  << "Failed to set F/T of finger 2."
			  << std::endl;
	return devODEGripper::EFAILURE;
      }
    }

    return devODEGripper::ESUCCESS;
  }

  CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		    << "Expected 4 joints values. Got " << fts.size()
		    << std::endl;

  return devODEGripper::EFAILURE;  

}










