#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstDevices/robotcomponents/devRobotComponent.h>

const std::string devRobotComponent::Control       = "Control";
const std::string devRobotComponent::EnableCommand = "EnableCommand";

// Create an IO for a task/component
devRobotComponent::IO::IO( mtsTask* task, 
			   const std::string& name,
			   devRobotComponent::IO::Type type, 
			   devRobotComponent::Variables variables ):
  task( task ),
  type( type ),
  variables( variables ),
  pinterface( NULL ),
  rinterface( NULL ){

  if( type == devRobotComponent::IO::PROVIDE_OUTPUT ||
      type == devRobotComponent::IO::PROVIDE_INPUT     )
    { pinterface = task->AddInterfaceProvided( name ); }

  if( type == devRobotComponent::IO::REQUIRE_OUTPUT ||
      type == devRobotComponent::IO::REQUIRE_INPUT    )
    { rinterface = task->AddInterfaceRequired( name ); }

}

devRobotComponent::IO::~IO(){}

devRobotComponent::IO::Type devRobotComponent::IO::IOType() const
{ return type; }

mtsStateTable* devRobotComponent::IO::StateTable()
{ return task->GetDefaultStateTable(); }

bool devRobotComponent::IO::PositionEnabled()     const
{ return variables & devRobotComponent::POSITION; }

bool devRobotComponent::IO::VelocityEnabled()     const
{ return variables & devRobotComponent::VELOCITY; }

bool devRobotComponent::IO::AccelerationEnabled() const
{ return variables & devRobotComponent::ACCELERATION; }

bool devRobotComponent::IO::ForceTorqueEnabled()  const
{ return variables & devRobotComponent::FORCETORQUE; }
    

devRobotComponent::RnIO::RnIO( mtsTask* task,
			       const std::string& name,
			       devRobotComponent::IO::Type type,
			       devRobotComponent::Variables variables,
			       size_t N ) :

  devRobotComponent::IO( task, name, type, variables ){

  CreatePositionIO( N );
  CreateVelocityIO( N );
  CreateAccelerationIO( N );
  CreateForceTorqueIO( N );

}

void devRobotComponent::RnIO::CreatePositionIO( size_t N ){

  // should we do the position IO?
  if( PositionEnabled() ){

    mtsq.SetSize( N );
    mtsq.SetAll( 0.0 );

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadPositionFnName.assign( "ReadRnPosition" );
      StateTable()->AddData( mtsq, "Position" );
      pinterface->AddCommandReadState
	( *StateTable(), mtsq, ReadPositionFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){ 
      ReadPositionFnName.assign( "ReadRnPosition" );
      rinterface->AddFunction( ReadPositionFnName, mtsFnReadPosition );
    }
    

    if( IOType() == IO::PROVIDE_INPUT  ){
      WritePositionFnName.assign( "WriteRnPosition" );
      StateTable()->AddData( mtsq, "Position" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtsq, WritePositionFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WritePositionFnName.assign( "WriteRnPosition" );
      rinterface->AddFunction( WritePositionFnName, mtsFnWritePosition );
    }

  }

}

void devRobotComponent::RnIO::CreateVelocityIO( size_t N ){

  // should we do the velocity IO?
  if( VelocityEnabled() ){

    mtsqd.SetSize( N );
    mtsqd.SetAll( 0.0 );

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadVelocityFnName.assign( "ReadRnVelocity" );
      StateTable()->AddData( mtsqd, "Velocity" );
      pinterface->AddCommandReadState
	( *StateTable(), mtsqd, ReadVelocityFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){
      ReadVelocityFnName.assign( "ReadRnVelocity" );
      rinterface->AddFunction( ReadVelocityFnName, mtsFnReadVelocity ); 
    }
    
    if( IOType() == IO::PROVIDE_INPUT  ){
      WriteVelocityFnName.assign( "WriteRnVelocity" );
      StateTable()->AddData( mtsqd, "Velocity" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtsqd, WriteVelocityFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WriteVelocityFnName.assign( "WriteRnVelocity" );
      rinterface->AddFunction( WriteVelocityFnName, mtsFnWriteVelocity ); 
    }

  }

}

void devRobotComponent::RnIO::CreateAccelerationIO( size_t N ){

  // should we do the acceleration IO?
  if( AccelerationEnabled() ){

    mtsqdd.SetSize( N );
    mtsqdd.SetAll( 0.0 );

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadAccelerationFnName.assign( "ReadRnAcceleration" );
      StateTable()->AddData( mtsqdd, "Acceleration" );
      pinterface->AddCommandReadState
	( *StateTable(), mtsqdd, ReadAccelerationFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){ 
      ReadAccelerationFnName.assign( "ReadRnAcceleration" );
      rinterface->AddFunction( ReadAccelerationFnName, mtsFnReadAcceleration );
    }
    
    if( IOType() == IO::PROVIDE_INPUT  ){
      WriteAccelerationFnName.assign( "WriteRnAcceleration" );
      StateTable()->AddData( mtsqdd, "Acceleration" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtsqdd, WriteAccelerationFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WriteAccelerationFnName.assign( "WriteRnAcceleration" );
      rinterface->AddFunction(WriteAccelerationFnName, mtsFnWriteAcceleration);
    }

  }

}

void devRobotComponent::RnIO::CreateForceTorqueIO( size_t N ){

  // should we do the acceleration IO?
  if( ForceTorqueEnabled() ){
      
    mtsft.SetSize( N );
    mtsft.SetAll( 0.0 );

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadForceTorqueFnName.assign( "ReadRnForceTorque" );
      StateTable()->AddData( mtsft, "ForceTorque" );
      pinterface->AddCommandReadState
	( *StateTable(), mtsft, ReadForceTorqueFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){ 
      ReadForceTorqueFnName.assign( "ReadRnForceTorque" );
      rinterface->AddFunction( ReadForceTorqueFnName, mtsFnReadForceTorque );
    }
    
    if( IOType() == IO::PROVIDE_INPUT  ){
      WriteForceTorqueFnName.assign( "WriteRnForceTorque" );
      StateTable()->AddData( mtsft, "ForceTorque" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtsft, WriteForceTorqueFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WriteForceTorqueFnName.assign( "WriteRnForceTorque" );
      rinterface->AddFunction( WriteForceTorqueFnName, mtsFnWriteForceTorque );
    }

  }
  
}

void 
devRobotComponent::RnIO::SetPosition
( const vctDynamicVector<double>& q ){ 
  mtsq = q;
  if( IOType() == IO::REQUIRE_OUTPUT )
    { mtsFnWritePosition( mtsq ); }
}

void 
devRobotComponent::RnIO::SetVelocity
( const vctDynamicVector<double>& qd ){ 
  mtsqd = qd; 
  if( IOType() == IO::REQUIRE_OUTPUT )
    { mtsFnWriteVelocity( mtsqd ); }
}

void 
devRobotComponent::RnIO::SetAcceleration
( const vctDynamicVector<double>& qdd ){ 
  mtsqdd = qdd;
  if( IOType() == IO::REQUIRE_OUTPUT )
    { mtsFnWriteAcceleration( mtsqdd ); }
}

void 
devRobotComponent::RnIO::SetForceTorque
( const vctDynamicVector<double>& ft ){
  mtsft = ft; 
  if( IOType() == IO::REQUIRE_OUTPUT )
    { mtsFnWriteForceTorque( mtsft ); }
}


void 
devRobotComponent::RnIO::GetPosition
( vctDynamicVector<double>& q, double& t ){
  if( IOType() == IO::REQUIRE_INPUT )
    { mtsFnReadPosition( mtsq ); }
  q = vctDynamicVector<double>( mtsq );
  //std::cerr << "mts: " << mtsq.Timestamp() << std::endl;
  t = mtsq.Timestamp();
}

void 
devRobotComponent::RnIO::GetVelocity
( vctDynamicVector<double>& qd, double& t ){
  if( IOType() == IO::REQUIRE_INPUT )
    { mtsFnReadVelocity( mtsqd ); }
  qd = vctDynamicVector<double>( mtsqd );
  t = mtsqd.Timestamp();
}

void
devRobotComponent::RnIO::GetAcceleration
( vctDynamicVector<double>& qdd,double& t ){
  if( IOType() == IO::REQUIRE_INPUT )
    { mtsFnReadAcceleration( mtsqdd ); }
  qdd = vctDynamicVector<double>( mtsqdd );
  t = mtsqdd.Timestamp();
}

void
devRobotComponent::RnIO::GetForceTorque
( vctDynamicVector<double>& ft, double& t ){
  if( IOType() == IO::REQUIRE_INPUT )
    { mtsFnReadForceTorque( mtsft ); }
  ft = vctDynamicVector<double>( mtsft );
  t = mtsft.Timestamp();
}








devRobotComponent::R3IO::R3IO( mtsTask* task,
			       const std::string& name,
			       devRobotComponent::IO::Type type,
			       devRobotComponent::Variables variables ) :

  devRobotComponent::IO( task, name, type, variables ){

  CreatePositionIO();
  CreateVelocityIO();
  CreateAccelerationIO();
  CreateForceIO();

}

void devRobotComponent::R3IO::CreatePositionIO(){

  // should we do the position IO?
  if( PositionEnabled() ){

    mtsp.SetSize( 3 );
    mtsp.SetAll( 0.0 );

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadPositionFnName.assign( "ReadR3Position" );
      StateTable()->AddData( mtsp, "Position" );
      pinterface->AddCommandReadState
	( *StateTable(), mtsp, ReadPositionFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){ 
      ReadPositionFnName.assign( "ReadR3Position" );
      rinterface->AddFunction( ReadPositionFnName, mtsFnReadPosition );
    }
    

    if( IOType() == IO::PROVIDE_INPUT  ){
      WritePositionFnName.assign( "WriteR3Position" );
      StateTable()->AddData( mtsp, "Position" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtsp, WritePositionFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WritePositionFnName.assign( "WriteR3Position" );
      rinterface->AddFunction( WritePositionFnName, mtsFnWritePosition );
    }

  }

}

void devRobotComponent::R3IO::CreateVelocityIO(){

  // should we do the velocity IO?
  if( VelocityEnabled() ){

    mtsv.SetSize( 3 );
    mtsv.SetAll( 0.0 );

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadVelocityFnName.assign( "ReadR3Velocity" );
      StateTable()->AddData( mtsv, "Velocity" );
      pinterface->AddCommandReadState
	( *StateTable(), mtsv, ReadVelocityFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){
      ReadVelocityFnName.assign( "ReadR3Velocity" );
      rinterface->AddFunction( ReadVelocityFnName, mtsFnReadVelocity ); 
    }
    
    if( IOType() == IO::PROVIDE_INPUT  ){
      WriteVelocityFnName.assign( "WriteR3Velocity" );
      StateTable()->AddData( mtsv, "Velocity" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtsv, WriteVelocityFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WriteVelocityFnName.assign( "WriteR3Velocity" );
      rinterface->AddFunction( WriteVelocityFnName, mtsFnWriteVelocity ); 
    }

  }

}

void devRobotComponent::R3IO::CreateAccelerationIO(){

  // should we do the acceleration IO?
  if( AccelerationEnabled() ){

    mtsvd.SetSize( 3 );
    mtsvd.SetAll( 0.0 );

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadAccelerationFnName.assign( "ReadR3Acceleration" );
      StateTable()->AddData( mtsvd, "Acceleration" );
      pinterface->AddCommandReadState
	( *StateTable(), mtsvd, ReadAccelerationFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){ 
      ReadAccelerationFnName.assign( "ReadR3Acceleration" );
      rinterface->AddFunction( ReadAccelerationFnName, mtsFnReadAcceleration );
    }
    
    if( IOType() == IO::PROVIDE_INPUT  ){
      WriteAccelerationFnName.assign( "WriteR3Acceleration" );
      StateTable()->AddData( mtsvd, "Acceleration" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtsvd, WriteAccelerationFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WriteAccelerationFnName.assign( "WriteR3Acceleration" );
      rinterface->AddFunction(WriteAccelerationFnName, mtsFnWriteAcceleration);
    }

  }

}

void devRobotComponent::R3IO::CreateForceIO(){

  // should we do the acceleration IO?
  if( ForceTorqueEnabled() ){
      
    mtsf.SetSize( 3 );
    mtsf.SetAll( 0.0 );

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadForceTorqueFnName.assign( "ReadR3Force" );
      StateTable()->AddData( mtsf, "Force" );
      pinterface->AddCommandReadState
	( *StateTable(), mtsf, ReadForceTorqueFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){ 
      ReadForceTorqueFnName.assign( "ReadR3Force" );
      rinterface->AddFunction( ReadForceTorqueFnName, mtsFnReadForceTorque );
    }
    
    if( IOType() == IO::PROVIDE_INPUT  ){
      WriteForceTorqueFnName.assign( "WriteR3Force" );
      StateTable()->AddData( mtsf, "Force" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtsf, WriteForceTorqueFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WriteForceTorqueFnName.assign( "WriteR3Force" );
      rinterface->AddFunction( WriteForceTorqueFnName, mtsFnWriteForceTorque );
    }

  }
  
}

void 
devRobotComponent::R3IO::SetPosition
( const vctFixedSizeVector<double,3>& p ){ 
  mtsp = vctDynamicVector<double>( 3, p[0], p[1], p[2] );
  if( IOType() == IO::REQUIRE_OUTPUT )
    { mtsFnWritePosition( mtsp ); }
}

void 
devRobotComponent::R3IO::SetVelocity
( const vctFixedSizeVector<double,3>& v ){ 
  mtsv = vctDynamicVector<double>( 3, v[0], v[1], v[2] );
  if( IOType() == IO::REQUIRE_OUTPUT )
    { mtsFnWriteVelocity( mtsv ); }
}

void 
devRobotComponent::R3IO::SetAcceleration
( const vctFixedSizeVector<double,3>& vd ){ 
  mtsvd = vctDynamicVector<double>( 3, vd[0], vd[1], vd[2] );
  if( IOType() == IO::REQUIRE_OUTPUT )
    { mtsFnWriteAcceleration( mtsvd ); }
}

void 
devRobotComponent::R3IO::SetForce
( const vctFixedSizeVector<double,3>& f ){
  mtsf = vctDynamicVector<double>( 3, f[0], f[1], f[2] );
  if( IOType() == IO::REQUIRE_OUTPUT )
    { mtsFnWriteForceTorque( mtsf ); }
}


void 
devRobotComponent::R3IO::GetPosition
( vctFixedSizeVector<double,3>& p, double& t ){
  if( IOType() == IO::REQUIRE_INPUT )
    { mtsFnReadPosition( mtsp ); }
  p = vctFixedSizeVector<double,3>( mtsp[0], mtsp[1], mtsp[2] );
  t = mtsp.Timestamp();
}

void 
devRobotComponent::R3IO::GetVelocity
( vctFixedSizeVector<double,3>& v, double& t ){
  if( IOType() == IO::REQUIRE_INPUT )
    { mtsFnReadVelocity( mtsv ); }
  v = vctFixedSizeVector<double,3>( mtsv[0], mtsv[1], mtsv[2] );
  t = mtsv.Timestamp();
}

void
devRobotComponent::R3IO::GetAcceleration
( vctFixedSizeVector<double,3>& vd, double& t ){
  if( IOType() == IO::REQUIRE_INPUT )
    { mtsFnReadAcceleration( mtsvd ); }
  vd = vctFixedSizeVector<double,3>( mtsvd[0], mtsvd[1], mtsvd[2] );
  t = mtsvd.Timestamp();
}

void
devRobotComponent::R3IO::GetForce
( vctFixedSizeVector<double,3>& f, double& t ){
  if( IOType() == IO::REQUIRE_INPUT )
    { mtsFnReadForceTorque( mtsf ); }
  f = vctFixedSizeVector<double,3>( mtsf[0], mtsf[1], mtsf[2] );
  t = mtsf.Timestamp();
}










devRobotComponent::SO3IO::SO3IO( mtsTask* task,
				 const std::string& name,
				 devRobotComponent::IO::Type type,
				 devRobotComponent::Variables variables ):
  devRobotComponent::IO( task, name, type, variables ){

  CreateRotationIO( );
  CreateVelocityIO( );
  CreateAccelerationIO( );
  CreateTorqueIO( );

}

void devRobotComponent::SO3IO::CreateRotationIO( ){

  // should we do the position IO?
  if( PositionEnabled() ){

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadPositionFnName.assign( "ReadRotation" );
      StateTable()->AddData( mtsq, "Rotation" );
      pinterface->AddCommandReadState
	( *StateTable(), mtsq, ReadPositionFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){ 
      ReadPositionFnName.assign( "ReadRotation" );
      rinterface->AddFunction( ReadPositionFnName, mtsFnReadPosition );
    }
    

    if( IOType() == IO::PROVIDE_INPUT  ){
      WritePositionFnName.assign( "WriteRotation" );
      StateTable()->AddData( mtsq, "Rotation" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtsq, WritePositionFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WritePositionFnName.assign( "WriteRotation" );
      rinterface->AddFunction( WritePositionFnName, mtsFnWritePosition );
    }

  }

}

void devRobotComponent::SO3IO::CreateVelocityIO( ){

  // should we do the velocity IO?
  if( VelocityEnabled() ){

    mtsw.SetSize( 3 );
    mtsw.SetAll( 0.0 );

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadVelocityFnName.assign( "ReadAngularVelocity" );
      StateTable()->AddData( mtsw, "AngularVelocity" );
      pinterface->AddCommandReadState
	( *StateTable(), mtsw, ReadVelocityFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){
      ReadVelocityFnName.assign( "ReadAngularVelocity" );
      rinterface->AddFunction( ReadVelocityFnName, mtsFnReadVelocity ); 
    }
    
    if( IOType() == IO::PROVIDE_INPUT  ){
      WriteVelocityFnName.assign( "WriteAngularVelocity" );
      StateTable()->AddData( mtsw, "AngularVelocity" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtsw, WriteVelocityFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WriteVelocityFnName.assign( "WriteAngularVelocity" );
      rinterface->AddFunction( WriteVelocityFnName, mtsFnWriteVelocity ); 
    }

  }

}

void devRobotComponent::SO3IO::CreateAccelerationIO( ){

  // should we do the acceleration IO?
  if( AccelerationEnabled() ){

    mtswd.SetSize( 3 );
    mtswd.SetAll( 0.0 );

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadAccelerationFnName.assign( "ReadAngularAcceleration" );
      StateTable()->AddData( mtswd, "AngularAcceleration" );
      pinterface->AddCommandReadState
	( *StateTable(), mtswd, ReadAccelerationFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){ 
      ReadAccelerationFnName.assign( "ReadAngularAcceleration" );
      rinterface->AddFunction( ReadAccelerationFnName, mtsFnReadAcceleration );
    }
    
    if( IOType() == IO::PROVIDE_INPUT  ){
      WriteAccelerationFnName.assign( "WriteAngularAcceleration" );
      StateTable()->AddData( mtswd, "AngularAcceleration" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtswd, WriteAccelerationFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WriteAccelerationFnName.assign( "WriteAngularAcceleration" );
      rinterface->AddFunction(WriteAccelerationFnName, mtsFnWriteAcceleration);
    }

  }

}

void devRobotComponent::SO3IO::CreateTorqueIO( ){

  // should we do the acceleration IO?
  if( ForceTorqueEnabled() ){
      
    mtstau.SetSize( 3 );
    mtstau.SetAll( 0.0 );

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadForceTorqueFnName.assign( "ReadTorque" );
      StateTable()->AddData( mtstau, "Torque" );
      pinterface->AddCommandReadState
	( *StateTable(), mtstau, ReadForceTorqueFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){ 
      ReadForceTorqueFnName.assign( "ReadTorque" );
      rinterface->AddFunction( ReadForceTorqueFnName, mtsFnReadForceTorque );
    }
    
    if( IOType() == IO::PROVIDE_INPUT  ){
      WriteForceTorqueFnName.assign( "WriteTorque" );
      StateTable()->AddData( mtstau, "Torque" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtstau, WriteForceTorqueFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WriteForceTorqueFnName.assign( "WriteTorque" );
      rinterface->AddFunction( WriteForceTorqueFnName, mtsFnWriteForceTorque );
    }

  }
  
}

void devRobotComponent::SO3IO::SetRotation
( const vctQuaternionRotation3<double>& q ){ 
  mtsq = q;
  if( IOType() == IO::REQUIRE_OUTPUT )
    { mtsFnWritePosition( mtsq ); }
}

void devRobotComponent::SO3IO::SetVelocity
( const vctFixedSizeVector<double,3>& w ){ 

  if( mtsw.size() != 3 )               { mtsw.SetSize( 3 ); }
  for( size_t i=0; i<3; i++ )          { mtsw[i] = w[i]; }
  if( IOType() == IO::REQUIRE_OUTPUT ) { mtsFnWriteVelocity( mtsw ); }

}

void devRobotComponent::SO3IO::SetAcceleration
( const vctFixedSizeVector<double,3>& wd ){ 

  if( mtswd.size() != 3 )              { mtswd.SetSize( 3 ); }
  for( size_t i=0; i<3; i++ )          { mtswd[i] = wd[i]; }
  if( IOType() == IO::REQUIRE_OUTPUT ) { mtsFnWriteAcceleration( mtswd ); }

}

void 
devRobotComponent::SO3IO::SetTorque
( const vctFixedSizeVector<double,3>& tau ){

  if( mtstau.size() != 3 )             { mtstau.SetSize( 3 ); }
  for( size_t i=0; i<3; i++ )          { mtstau[i] = tau[i]; }
  if( IOType() == IO::REQUIRE_OUTPUT ) { mtsFnWriteForceTorque( mtstau ); }

}


void 
devRobotComponent::SO3IO::GetRotation
( vctQuaternionRotation3<double>& q, double& t ){

  if( IOType() == IO::REQUIRE_INPUT )  { mtsFnReadPosition( mtsq ); }
  q = vctQuaternionRotation3<double>( mtsq );
  t = mtsq.Timestamp();

}

void 
devRobotComponent::SO3IO::GetVelocity
( vctFixedSizeVector<double,3>& w, double& t ){

  if( IOType() == IO::REQUIRE_INPUT )  { mtsFnReadVelocity( mtsw ); }
  if( mtsw.size() != 3 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected sizeof(mtsw) = 3. Got " << mtsw.size()
		      << std::endl;
  }
  for( size_t i=0; i<3 && i<mtsw.size(); i++ )
    { w[i] = mtsw[i]; }
  t = mtsw.Timestamp();

}

void
devRobotComponent::SO3IO::GetAcceleration
( vctFixedSizeVector<double,3>& wd, double& t ){

  if( IOType() == IO::REQUIRE_INPUT )  { mtsFnReadAcceleration( mtswd ); }
  if( mtswd.size() != 3 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected sizeof(mtswd) = 3. Got " <<mtswd.size()
		      << std::endl;
  }
  for( size_t i=0; i<3 && i<mtswd.size(); i++ )
    { wd[i] = mtswd[i]; }
  t = mtswd.Timestamp();

}

void 
devRobotComponent::SO3IO::GetTorque
( vctFixedSizeVector<double,3>& tau, double& t ){

  if( IOType() == IO::REQUIRE_INPUT )  { mtsFnReadForceTorque( mtstau ); }
  if( mtstau.size() != 3 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected sizeof(mtstau) = 3. Got " << mtstau.size()
		      << std::endl;
  }

  for( size_t i=0; i<3 && i<mtstau.size(); i++ )
    { tau[i] = mtstau[i]; }
  t = mtstau.Timestamp();

}






devRobotComponent::SE3IO::SE3IO( mtsTask* task,
				 const std::string& name,
				 devRobotComponent::IO::Type type,
				 devRobotComponent::Variables variables ):
  devRobotComponent::IO( task, name, type, variables ){

  CreatePositionIO( );
  CreateVelocityIO( );
  CreateAccelerationIO( );
  CreateForceTorqueIO( );

}

void devRobotComponent::SE3IO::CreatePositionIO( ){

  // should we do the position IO?
  if( PositionEnabled() ){

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadPositionFnName.assign( "ReadSE3Position" );
      StateTable()->AddData( mtsRt, "Position" );
      pinterface->AddCommandReadState
	( *StateTable(), mtsRt, ReadPositionFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){ 
      ReadPositionFnName.assign( "ReadSE3Position" );
      rinterface->AddFunction( ReadPositionFnName, mtsFnReadPosition );
    }
    

    if( IOType() == IO::PROVIDE_INPUT  ){
      WritePositionFnName.assign( "WriteSE3Position" );
      StateTable()->AddData( mtsRt, "Position" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtsRt, WritePositionFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WritePositionFnName.assign( "WriteSE3Position" );
      rinterface->AddFunction( WritePositionFnName, mtsFnWritePosition );
    }

  }

}

void devRobotComponent::SE3IO::CreateVelocityIO( ){

  // should we do the velocity IO?
  if( VelocityEnabled() ){

    mtsvw.SetSize( 6 );
    mtsvw.SetAll( 0.0 );

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadVelocityFnName.assign( "ReadSE3Velocity" );
      StateTable()->AddData( mtsvw, "Velocity" );
      pinterface->AddCommandReadState
	( *StateTable(), mtsvw, ReadVelocityFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){
      ReadVelocityFnName.assign( "ReadSE3Velocity" );
      rinterface->AddFunction( ReadVelocityFnName, mtsFnReadVelocity ); 
    }
    
    if( IOType() == IO::PROVIDE_INPUT  ){
      WriteVelocityFnName.assign( "WriteSE3Velocity" );
      StateTable()->AddData( mtsvw, "Velocity" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtsvw, WriteVelocityFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WriteVelocityFnName.assign( "WriteSE3Velocity" );
      rinterface->AddFunction( WriteVelocityFnName, mtsFnWriteVelocity ); 
    }

  }

}

void devRobotComponent::SE3IO::CreateAccelerationIO( ){

  // should we do the acceleration IO?
  if( AccelerationEnabled() ){

    mtsvdwd.SetSize( 6 );
    mtsvdwd.SetAll( 0.0 );

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadAccelerationFnName.assign( "ReadSE3Acceleration" );
      StateTable()->AddData( mtsvdwd, "Acceleration" );
      pinterface->AddCommandReadState
	( *StateTable(), mtsvdwd, ReadAccelerationFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){ 
      ReadAccelerationFnName.assign( "ReadSE3Acceleration" );
      rinterface->AddFunction( ReadAccelerationFnName, mtsFnReadAcceleration );
    }
    
    if( IOType() == IO::PROVIDE_INPUT  ){
      WriteAccelerationFnName.assign( "WriteSE3Acceleration" );
      StateTable()->AddData( mtsvdwd, "Acceleration" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtsvdwd, WriteAccelerationFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WriteAccelerationFnName.assign( "WriteSE3Acceleration" );
      rinterface->AddFunction(WriteAccelerationFnName, mtsFnWriteAcceleration);
    }

  }

}

void devRobotComponent::SE3IO::CreateForceTorqueIO( ){

  // should we do the acceleration IO?
  if( ForceTorqueEnabled() ){
      
    mtsft.SetSize( 6 );
    mtsft.SetAll( 0.0 );

    if( IOType() == IO::PROVIDE_OUTPUT ){
      ReadForceTorqueFnName.assign( "ReadSE3ForceTorque" );
      StateTable()->AddData( mtsft, "ForceTorque" );
      pinterface->AddCommandReadState
	( *StateTable(), mtsft, ReadForceTorqueFnName );
    }

    // must read input
    if( IOType() == IO::REQUIRE_INPUT  ){ 
      ReadForceTorqueFnName.assign( "ReadSE3ForceTorque" );
      rinterface->AddFunction( ReadForceTorqueFnName, mtsFnReadForceTorque );
    }
    
    if( IOType() == IO::PROVIDE_INPUT  ){
      WriteForceTorqueFnName.assign( "WriteSE3ForceTorque" );
      StateTable()->AddData( mtsft, "ForceTorque" );
      pinterface->AddCommandWriteState
	( *StateTable(), mtsft, WriteForceTorqueFnName );
    }

    // must write output
    if( IOType() == IO::REQUIRE_OUTPUT ){ 
      WriteForceTorqueFnName.assign( "WriteSE3ForceTorque" );
      rinterface->AddFunction( WriteForceTorqueFnName, mtsFnWriteForceTorque );
    }

  }
  
}

void 
devRobotComponent::SE3IO::SetPosition
( const vctFrame4x4<double>& Rt ){ 
  mtsRt = Rt;
  if( IOType() == IO::REQUIRE_OUTPUT )
    { mtsFnWritePosition( mtsRt ); }
}

void 
devRobotComponent::SE3IO::SetVelocity
( const vctFixedSizeVector<double,6>& vw ){ 

  if( mtsvw.size() != 6 )              { mtsvw.SetSize( 6 ); }
  for( size_t i=0; i<6; i++ )          { mtsvw[i] = vw[i]; }
  if( IOType() == IO::REQUIRE_OUTPUT ) { mtsFnWriteVelocity( mtsvw ); }

}

void 
devRobotComponent::SE3IO::SetAcceleration
( const vctFixedSizeVector<double,6>& vdwd ){ 

  if( mtsvdwd.size() != 6 )            { mtsvdwd.SetSize( 6 ); }
  for( size_t i=0; i<6; i++ )          { mtsvdwd[i] = vdwd[i]; }
  if( IOType() == IO::REQUIRE_OUTPUT ) { mtsFnWriteAcceleration( mtsvdwd ); }

}

void 
devRobotComponent::SE3IO::SetForceTorque
( const vctFixedSizeVector<double,6>& ft ){

  if( mtsft.size() != 6 )              { mtsft.SetSize( 6 ); }
  for( size_t i=0; i<6; i++ )          { mtsft[i] = ft[i]; }
  if( IOType() == IO::REQUIRE_OUTPUT ) { mtsFnWriteForceTorque( mtsft ); }

}


void 
devRobotComponent::SE3IO::GetPosition
( vctFrame4x4<double>& Rt, double& t ){

  if( IOType() == IO::REQUIRE_INPUT )  { mtsFnReadPosition( mtsRt ); }
  Rt = vctFrame4x4<double>( mtsRt );
  t = mtsRt.Timestamp();

}

void 
devRobotComponent::SE3IO::GetVelocity
( vctFixedSizeVector<double,6>& vw, double& t ){

  if( IOType() == IO::REQUIRE_INPUT )  { mtsFnReadVelocity( mtsvw ); }
  if( mtsvw.size() != 6 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected sizeof(mtsvw) = 6. Got " << mtsvw.size()
		      << std::endl;
  }
  for( size_t i=0; i<6 && i<mtsvw.size(); i++ )
    { vw[i] = mtsvw[i]; }
  t = mtsvw.Timestamp();

}

void
devRobotComponent::SE3IO::GetAcceleration
( vctFixedSizeVector<double,6>& vdwd, double& t ){

  if( IOType() == IO::REQUIRE_INPUT )  { mtsFnReadAcceleration( mtsvdwd ); }
  if( mtsvdwd.size() != 6 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected sizeof(mtsvdwd) = 6. Got " <<mtsvdwd.size()
		      << std::endl;
  }
  for( size_t i=0; i<6 && i<mtsvdwd.size(); i++ )
    { vdwd[i] = mtsvdwd[i]; }
  t = mtsvdwd.Timestamp();

}

void 
devRobotComponent::SE3IO::GetForceTorque
( vctFixedSizeVector<double,6>& ft, double& t ){

  if( IOType() == IO::REQUIRE_INPUT )  { mtsFnReadForceTorque( mtsft ); }
  if( mtsft.size() != 6 ){
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << ": Expected sizeof(mtsft) = 6. Got " << mtsft.size()
		      << std::endl;
  }

  for( size_t i=0; i<6 && i<mtsft.size(); i++ )
    { ft[i] = mtsft[i]; }
  t = mtsft.Timestamp();

}




















devRobotComponent::devRobotComponent( const std::string& name, 
				      double period, 
				      bool enabled ) :
  mtsTaskPeriodic( name, period, true ),
  mtsEnabled( enabled ),
  risingedge( false ) {
  
  // Create the control interface
  mtsInterfaceProvided* cinterface;
  cinterface = AddInterfaceProvided( devRobotComponent::Control );
  if( cinterface ){
    StateTable.AddData( mtsEnabled, "Enabled" );
    cinterface->AddCommandWriteState
      ( StateTable, mtsEnabled, devRobotComponent::EnableCommand );
  }

}

// Create IO for joints

devRobotComponent::RnIO* 
devRobotComponent::ProvideOutputRn
( const std::string& name,
  devRobotComponent::Variables variables,
  size_t N ){ 
  return new devRobotComponent::RnIO( this, 
				      name,
				      devRobotComponent::IO::PROVIDE_OUTPUT, 
				      variables, 
				      N ); 
}

devRobotComponent::RnIO* 
devRobotComponent::ProvideInputRn
( const std::string& name,
  devRobotComponent::Variables variables,
  size_t N ){ 
  return new devRobotComponent::RnIO( this, 
				      name, 
				      devRobotComponent::IO::PROVIDE_INPUT, 
				      variables,
				      N ); 
}

devRobotComponent::RnIO* 
devRobotComponent::RequireOutputRn
( const std::string& name,
  devRobotComponent::Variables variables,
  size_t N ){ 
  return new devRobotComponent::RnIO( this, 
				      name,
				      devRobotComponent::IO::REQUIRE_OUTPUT, 
				      variables, 
				      N ); 
}

devRobotComponent::RnIO* 
devRobotComponent::RequireInputRn
( const std::string& name,
  devRobotComponent::Variables variables,
  size_t N ){ 
  return new devRobotComponent::RnIO( this, 
				      name, 
				      devRobotComponent::IO::REQUIRE_INPUT, 
				      variables,
				      N ); 
}



// Create IO for R3

devRobotComponent::R3IO* 
devRobotComponent::ProvideOutputR3
( const std::string& name,
  devRobotComponent::Variables variables ){ 
  return new devRobotComponent::R3IO( this, 
				      name,
				      devRobotComponent::IO::PROVIDE_OUTPUT, 
				      variables );
}

devRobotComponent::R3IO* 
devRobotComponent::ProvideInputR3
( const std::string& name,
  devRobotComponent::Variables variables ){
  return new devRobotComponent::R3IO( this, 
				      name, 
				      devRobotComponent::IO::PROVIDE_INPUT, 
				      variables );
}

devRobotComponent::R3IO* 
devRobotComponent::RequireOutputR3
( const std::string& name,
  devRobotComponent::Variables variables ){
  return new devRobotComponent::R3IO( this, 
				      name,
				      devRobotComponent::IO::REQUIRE_OUTPUT, 
				      variables );
}

devRobotComponent::R3IO* 
devRobotComponent::RequireInputR3
( const std::string& name,
  devRobotComponent::Variables variables ){
  return new devRobotComponent::R3IO( this, 
				      name, 
				      devRobotComponent::IO::REQUIRE_INPUT, 
				      variables );
}


// Create IO for SO3

devRobotComponent::SO3IO* 
devRobotComponent::ProvideOutputSO3
( const std::string& name,
  devRobotComponent::Variables variables ){ 
  
  return new devRobotComponent::SO3IO( this, 
				       name,
				       devRobotComponent::IO::PROVIDE_OUTPUT, 
				       variables );
}

devRobotComponent::SO3IO* 
devRobotComponent::ProvideInputSO3
( const std::string& name,
  devRobotComponent::Variables variables ){
  return new devRobotComponent::SO3IO( this, 
				       name, 
				       devRobotComponent::IO::PROVIDE_INPUT, 
				       variables );
}

devRobotComponent::SO3IO* 
devRobotComponent::RequireOutputSO3
( const std::string& name,
  devRobotComponent::Variables variables ){
  return new devRobotComponent::SO3IO( this, 
				       name,
				       devRobotComponent::IO::REQUIRE_OUTPUT, 
				       variables );
}

devRobotComponent::SO3IO* 
devRobotComponent::RequireInputSO3
( const std::string& name,
  devRobotComponent::Variables variables ){
  return new devRobotComponent::SO3IO( this, 
				       name, 
				       devRobotComponent::IO::REQUIRE_INPUT, 
				       variables );
}



// Create IO for SE3

devRobotComponent::SE3IO* 
devRobotComponent::ProvideOutputSE3
( const std::string& name,
  devRobotComponent::Variables variables ){ 
  
  return new devRobotComponent::SE3IO( this, 
				       name,
				       devRobotComponent::IO::PROVIDE_OUTPUT, 
				       variables );
}

devRobotComponent::SE3IO* 
devRobotComponent::ProvideInputSE3
( const std::string& name,
  devRobotComponent::Variables variables ){
  return new devRobotComponent::SE3IO( this, 
				       name, 
				       devRobotComponent::IO::PROVIDE_INPUT, 
				       variables );
}

devRobotComponent::SE3IO* 
devRobotComponent::RequireOutputSE3
( const std::string& name,
  devRobotComponent::Variables variables ){
  return new devRobotComponent::SE3IO( this, 
				       name,
				       devRobotComponent::IO::REQUIRE_OUTPUT, 
				       variables );
}

devRobotComponent::SE3IO* 
devRobotComponent::RequireInputSE3
( const std::string& name,
  devRobotComponent::Variables variables ){
  return new devRobotComponent::SE3IO( this, 
				       name, 
				       devRobotComponent::IO::REQUIRE_INPUT, 
				       variables );
}


bool devRobotComponent::Enabled() const
{ return mtsEnabled; }

void devRobotComponent::Run(){

  ProcessQueuedCommands();

  if( Enabled() ){

    if( !risingedge ){
      CMN_LOG_RUN_WARNING << GetName() << " is enabled." << std::endl;
      risingedge = true;
    }

    RunComponent(); 

  }

  else{

    if( risingedge ){
      CMN_LOG_RUN_WARNING << GetName() << " is disabled." << std::endl;
      risingedge = false;
    }

  }

}
