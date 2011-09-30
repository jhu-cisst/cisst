/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstDevices/robotcomponents/ode/devODEManipulator.h>

devODEManipulator::devODEManipulator( const std::string& devname,
				      double period,
				      devManipulator::State state,
				      osaCPUMask mask,
				      devODEWorld* world,
				      devManipulator::Mode mode ) : 
  devOSGManipulator( devname, period, state, mask, mode ),
  worldid( world->GetWorldID() ),
  base( NULL )
{ CreateIO(); }

devODEManipulator::devODEManipulator( const std::string& devname,
				      double period,
				      devManipulator::State state,
				      osaCPUMask mask,
				      devODEWorld* world,
				      devManipulator::Mode mode,
				      const std::string& robotfilename,
				      const vctFrame4x4<double>& Rtw0,
				      const vctDynamicVector<double>& qinit,
				      const std::vector<std::string>& models,
				      const std::string& basemodel ) :

  devOSGManipulator( devname, period, state, mask, mode, robotfilename, Rtw0 ),
  worldid( world->GetWorldID() ),
  qinit( qinit ),
  base( NULL ){
  CreateIO();
  CreateManipulator( world, mode, models, basemodel );
}

devODEManipulator::devODEManipulator( const std::string& devname,
				      double period,
				      devManipulator::State state,
				      osaCPUMask mask,
				      devODEWorld* world,
				      devManipulator::Mode mode,
				      const std::string& robotfilename,
				      const vctFrame4x4<double>& Rtw0,
				      const vctDynamicVector<double>& qinit,
				      const std::vector<std::string>& models,
				      devODEBody* base ) :
  devOSGManipulator( devname, period, state, mask, mode, robotfilename, Rtw0 ),
  worldid( world->GetWorldID() ),
  qinit( qinit ),
  base( base ){
  CreateIO();
  CreateManipulator( world, mode, models, base );
}


devODEManipulator::devODEManipulator( const std::string& devname,
				      double period,
				      devManipulator::State state,
				      osaCPUMask mask,
				      devODEWorld* world,
				      devManipulator::Mode mode,
				      const std::string& robotfilename,
				      const vctFrm3& Rtw0,
				      const vctDynamicVector<double>& qinit,
				      const std::vector<std::string>& models,
				      const std::string& basemodel ) :

  devOSGManipulator( devname, period, state, mask, mode, robotfilename, 
		     vctFrame4x4<double>( Rtw0.Rotation(), Rtw0.Translation())),
  worldid( world->GetWorldID() ),
  qinit( qinit ),
  base( NULL ){
  CreateIO();
  CreateManipulator( world, mode, models, basemodel );
}

void devODEManipulator::CreateIO(){
  // Create a Rn input
  input = ProvideInputRn( devManipulator::Input,
			  devRobotComponent::POSITION |
			  devRobotComponent::VELOCITY |
			  devRobotComponent::FORCETORQUE, 
			  qinit.size() );

  // Create a Rn position output (which is the same as the input)
  output = ProvideOutputRn( devManipulator::Output,
			    devRobotComponent::POSITION,
			    qinit.size() );
}

void devODEManipulator::CreateManipulator(devODEWorld* world,
					  devManipulator::Mode mode,
					  const std::vector<std::string>& models,
					  devODEBody* base ){

  //! Create a new space for the manipulator
  dSpaceID spaceid = dSimpleSpaceCreate( world->GetSpaceID() );

  // Create the links
  for( size_t i=1; i<=links.size(); i++ ){

    std::ostringstream linkname;     // Links name
    linkname << "link" << i;

    // obtain the position and orientation of the ith link 
    vctFrame4x4<double> Rtwi = robManipulator::ForwardKinematics( qinit, i );
    devODEBody* link = NULL;
    link = new devODEBody( linkname.str(),                          // name
			   Rtwi,
			   models[i-1],
			   world,                                   // world
			   links[i-1].Mass(),                       // m   
			   links[i-1].CenterOfMass(),               // com
			   links[i-1].MomentOfInertiaAtCOM(),       // I  
			   spaceid );                               // space
    bodies.push_back( link );

  }

  // Initialize the joints
  vctFixedSizeVector<double,3> z(0.0, 0.0, 1.0); // the local Z axis

  dBodyID b1 = NULL;
  if( base != NULL )
    { b1 = base->GetBodyID(); }

  for( size_t i=0; i<links.size(); i++ ){

    // obtain the ID of the distal link 
    dBodyID b2 = bodies[i]->GetBodyID();
    
    // obtain the position and orientation of the ith link
    vctFrame4x4<double> Rtwi = robManipulator::ForwardKinematics( qinit, i );

    vctFixedSizeVector<double,3> anchor = Rtwi.Translation();
    vctFixedSizeVector<double,3> axis = Rtwi.Rotation() * z;

    dJointType type = dJointTypeHinge;
    if( links[i].GetType() == robKinematics::SLIDER )
      { type = dJointTypeSlider; }
    
    // This is a bit tricky. The min must be greater than -pi and the max must
    // be less than pi. Otherwise it really screw things up
    double qmin = links[i].PositionMin();
    double qmax = links[i].PositionMax();

    devODEJoint* joint = NULL;
    joint =  new devODEJoint( world->GetWorldID(), // the world ID
			      b1,                  // the proximal body
			      b2,                  // the distal body
			      type,                // the joint type
			      anchor,              // the XYZ position
			      axis,                // the Z axis 
			      qmin,                // the lower limit
			      qmax );              // the upper limit
    joints.push_back( joint );
    world->Insert( joint );
    
    if( mode == devManipulator::POSITION || mode == devManipulator::VELOCITY ){
      // for some all joints, save the first one must be inverted axis
      double sign = -1.0;

      Insert( new devODEServoMotor( world->GetWorldID(), 
				    b1,             // the first body
				    b2,             // the second body
				    axis*sign,      // the Z axis 
				    10,             // fudged values
				    links[i].ForceTorqueMax(),
				    type ) );
    }

    b1 = b2;  // proximal is now distal
  }

}

void devODEManipulator::CreateManipulator(devODEWorld* world,
					  devManipulator::Mode mode,
					  const std::vector<std::string>& models,
					  const std::string& basemodel ){
  //! Create a new space for the manipulator
  dSpaceID spaceid = dSimpleSpaceCreate( world->GetSpaceID() );

  // is there a base?
  if( !basemodel.empty() ){ 
    base = new devODEBody( "link0", 
			   Rtw0, 
			   basemodel, 
			   world, 
			   1.0,                                    // m   
			   vctFixedSizeVector<double,3>(0.0),      // com
			   vctFixedSizeMatrix<double,3,3>::Eye(),
			   spaceid );
    
    dJointID jid = dJointCreateFixed( WorldID(), 0 );
    dJointAttach( jid, NULL, base->GetBodyID() );
    dJointSetFixed( jid );
  }
  
  // Create the links
  for( size_t i=1; i<=links.size(); i++ ){

    std::ostringstream linkname;     // Links name
    linkname << "link" << i;

    // obtain the position and orientation of the ith link 
    vctFrame4x4<double> Rtwi = robManipulator::ForwardKinematics( qinit, i );
    devODEBody* link = NULL;
    link = new devODEBody( linkname.str(),                          // name
			   Rtwi,
			   models[i-1],
			   world,                                   // world
			   links[i-1].Mass(),                       // m   
			   links[i-1].CenterOfMass(),               // com
			   links[i-1].MomentOfInertiaAtCOM(),       // I  
			   spaceid );                               // space
    bodies.push_back( link );

  }

  // Initialize the joints
  vctFixedSizeVector<double,3> z(0.0, 0.0, 1.0); // the local Z axis

  dBodyID b1 = NULL;
  if( base != NULL )
    { b1 = base->GetBodyID(); }

  for( size_t i=0; i<links.size(); i++ ){

    // obtain the ID of the distal link 
    dBodyID b2 = bodies[i]->GetBodyID();
    
    // obtain the position and orientation of the ith link
    vctFrame4x4<double> Rtwi = robManipulator::ForwardKinematics( qinit, i );
    
    vctFixedSizeVector<double,3> anchor = Rtwi.Translation();
    vctFixedSizeVector<double,3> axis = Rtwi.Rotation() * z;

    dJointType type = dJointTypeHinge;
    if( links[i].GetType() == robKinematics::SLIDER )
      { type = dJointTypeSlider; }

    // This is a bit tricky. The min must be greater than -pi and the max must
    // be less than pi. Otherwise it really screw things up
    double qmin = links[i].PositionMin();
    double qmax = links[i].PositionMax();

    devODEJoint* joint;
    joint =  new devODEJoint( world->GetWorldID(), // the world ID
			      b1,                  // the proximal body
			      b2,                  // the distal body
			      type,                // the joint type
			      anchor,              // the XYZ position
			      axis,                // the Z axis 
			      qmin,                // the lower limit
			      qmax );              // the upper limit
    joints.push_back( joint );
    world->Insert( joint );

    if( mode == devManipulator::POSITION || mode == devManipulator::VELOCITY ){
      // for some all joints, save the first one must be inverted axis
      double sign = -1.0;
      if( base == NULL && i == 0 )
	sign = 1.0;
      
      Insert( new devODEServoMotor( world->GetWorldID(), 
				    b1,             // the first body
				    b2,             // the second body
				    axis*sign,      // the Z axis 
				    10,             // fudged values
				    links[i].ForceTorqueMax(),
				    type ) );
    }
    b1 = b2;  // proximal is now distal
  }

}

void devODEManipulator::Attach( robManipulator* tool ){

  devODEManipulator* odetool = dynamic_cast<devODEManipulator*>( tool );

  if( odetool != NULL ){
    // Create a fix joint between the last link and the tool
    dJointID jid = dJointCreateFixed( WorldID(), 0 );
    dJointAttach( jid, bodies.back()->GetBodyID(), odetool->GetBaseID() );
    dJointSetFixed( jid );
  }

  robManipulator::Attach( tool );

}

void devODEManipulator::Disable(){

  for( size_t i=0; i<bodies.size(); i++ )
    { bodies[i]->Disable(); }

  for( size_t i=0; i<tools.size(); i++ ){
    devODEManipulator* odetool = dynamic_cast<devODEManipulator*>( tools[i] );
    if( odetool != NULL )
      { odetool->Disable(); }
  }

}

void devODEManipulator::Enable(){

  for( size_t i=0; i<bodies.size(); i++ )
    { bodies[i]->Enable(); }

  for( size_t i=0; i<tools.size(); i++ ){
    devODEManipulator* odetool = dynamic_cast<devODEManipulator*>( tools[i] );
    if( odetool != NULL )
      { odetool->Enable(); }
  }

}

dBodyID devODEManipulator::GetBaseID() const {
  if( base == NULL ) return NULL;
  else               return base->GetBodyID();
}

void devODEManipulator::Insert( devODEBody* body )
{ bodies.push_back( body ); }

void devODEManipulator::Insert( devODEJoint* joint )
{ joints.push_back( joint ); }

void devODEManipulator::Insert( devODEServoMotor* servo )
{ servos.push_back( servo ); }

void devODEManipulator::Read()
{ output->SetPosition( GetJointsPositions() ); }

void devODEManipulator::Write(){
  
  switch( GetInputMode() ){

  case devManipulator::POSITION: 
    {
      vctDynamicVector<double> q;
      double t;
      input->GetPosition( q, t ); 
      if( q.size() == links.size() ) 
	SetPositions( q );
    }
    break;

  case devManipulator::VELOCITY: 
    {
      vctDynamicVector<double> qd;
      double t;
      input->GetVelocity( qd, t ); 
      if( qd.size() == links.size() ) 
	SetVelocities( qd );
    }
    break;

  case devManipulator::FORCETORQUE:
    {
      vctDynamicVector<double> ft;
      double t;
      input->GetForceTorque( ft, t ); 
      if( ft.size() == links.size() ) 
	SetForcesTorques( ft );
    }
    break;

  default: 
    break;

  }
}

devODEManipulator::Errno
devODEManipulator::SetPositions( const vctDynamicVector<double>& qs ){
  vctDynamicVector<double> q = GetJointsPositions();

  if( qs.size() == servos.size() && q.size() == servos.size() ){
    for( size_t i = 0; i<qs.size(); i++ )
      { servos[i]->SetPosition(  qs[i], q[i], GetPeriodicity() ); }
    return devODEManipulator::ESUCCESS;
  }

  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Expected " << servos.size() 
		      << " velocities. Got " << qs.size() 
		      << std::endl;
    return devODEManipulator::EFAILURE;
  }

}

devODEManipulator::Errno
devODEManipulator::SetVelocities( const vctDynamicVector<double>& qds ){

  if( qds.size() == servos.size() ){
    for( size_t i = 0; i<qds.size(); i++ )
      { servos[i]->SetVelocity(  qds[i] ); }
    return devODEManipulator::ESUCCESS;
  }

  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Expected " << servos.size() 
		      << " velocities. Got " << qds.size() 
		      << std::endl;
    return devODEManipulator::EFAILURE;
  }

}

devODEManipulator::Errno
devODEManipulator::SetForcesTorques( const vctDynamicVector<double>& ft) {

  if( ft.size() == joints.size() ){
    for(size_t i=0; i<joints.size() && i<ft.size(); i++ )
      { joints[i]->SetForceTorque( ft[i] ); }
    return devODEManipulator::ESUCCESS;
  }

  else{
    CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
		      << " Expected " << joints.size() 
		      << " forces/torques. Got " << ft.size() 
		      << std::endl;
    return devODEManipulator::EFAILURE;
  }

}

vctDynamicVector<double> devODEManipulator::GetJointsPositions() const {
  vctDynamicVector<double> q( joints.size(), 0.0 );
  for(size_t i=0; i<joints.size(); i++)
    { q[i] =  joints[i]->GetPosition(); }
  return q + qinit;
}

vctDynamicVector<double> devODEManipulator::GetJointsVelocities() const {
  vctDynamicVector<double> qd(joints.size(), 0.0);
  for(size_t i=0; i<joints.size(); i++)
    { qd[i] = joints[i]->GetVelocity(); }
  return qd;
}

#ifndef SWIG

devODEManipulator::State devODEManipulator::GetState() const {

  devODEManipulator::State state;
  
  for( size_t i=0; i<joints.size(); i++ ){
    
    dBodyID bid = joints[i]->GetDistalBody();

    devODEBody::State si;

    const dReal* R = dBodyGetRotation( bid );
    const dReal* t = dBodyGetPosition( bid );
    const dReal* v = dBodyGetLinearVel( bid );
    const dReal* w = dBodyGetAngularVel( bid );

    si.R = vctMatrixRotation3<double> ( R[0], R[1],  R[2], // R[3], 
					R[4], R[5],  R[6], // R[7], 
					R[8], R[9], R[10], // R[11], 
					VCT_NORMALIZE );
    si.t = vctFixedSizeVector<double,3>( t[0], t[1], t[2] );
    si.v = vctFixedSizeVector<double,3>( v[0], v[1], v[2] );
    si.w = vctFixedSizeVector<double,3>( w[0], w[1], w[2] );

    state.push_back( si );

  }

  return state;
}


void devODEManipulator::SetState( const devODEManipulator::State& state ){

  for( size_t i=0; i<state.size(); i++ ){
    
    dBodyID bid = joints[i]->GetDistalBody();

    dMatrix3 R = { state[i].R[0][0], state[i].R[0][1],  state[i].R[0][2], 0.0,
		   state[i].R[1][0], state[i].R[1][1],  state[i].R[1][2], 0.0,
		   state[i].R[2][0], state[i].R[2][1],  state[i].R[2][2], 0.0 };

    dBodySetRotation( bid, R );
    dBodySetPosition( bid, state[i].t[0], state[i].t[1], state[i].t[2] );
    dBodySetLinearVel(  bid, state[i].v[0], state[i].v[1], state[i].v[2] );
    dBodySetAngularVel( bid, state[i].w[0], state[i].w[1], state[i].w[2] );

  }

}

#endif

vctFrame4x4<double> 
devODEManipulator::ForwardKinematics( const vctDynamicVector<double>& q,int N )
const { return robManipulator::ForwardKinematics( q, N ); }

void devODEManipulator::ForwardKinematics( const vctDynamicVector<double>& q, 
					   vctFrm3& Rt,
					   int N ) const {

  vctFrame4x4<double> Rt4x4 = robManipulator::ForwardKinematics( q, N );
  vctMatrixRotation3<double> R( Rt4x4[0][0], Rt4x4[0][1], Rt4x4[0][2],
				Rt4x4[1][0], Rt4x4[1][1], Rt4x4[1][2],
				Rt4x4[2][0], Rt4x4[2][1], Rt4x4[2][2] );
  Rt = vctFrm3( R, Rt4x4.Translation() );
}
    
