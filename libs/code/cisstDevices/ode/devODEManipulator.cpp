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

#include <cisstDevices/ode/devODEManipulator.h>

devODEManipulator::devODEManipulator( const std::string& devname,
				      double period,
				      const vctDynamicVector<double>& qinit ):

  devManipulator( devname, period, qinit.size() ),
  robManipulator( "", vctFrame4x4<double>() ),
  qinit( qinit ),
  base( NULL ){}

devODEManipulator::devODEManipulator(const std::string& devname,
				     double period,
				     devODEWorld& world,
				     const std::string& robotfilename,
				     const vctFrame4x4<double>& Rtw0,
				     const vctDynamicVector<double> qinit,
				     const std::vector<std::string>& geomfiles):
  devManipulator( devname, period, qinit.size() ),
  robManipulator( robotfilename, Rtw0 ),
  worldid( world.WorldID() ),
  qinit( qinit ),
  base( NULL ){

  //! Create a new space for the manipulator
  dSpaceID spaceid = dSimpleSpaceCreate( world.SpaceID() );

  // Initialize the links

  // a temporary vector to hold pointers to bodies
  //std::vector<devODEBody*> bodies;

  for( size_t i=0; i<links.size(); i++ ){

    // obtain the position and orientation of the ith link 
    vctFrame4x4<double> Rtwi = ForwardKinematics( qinit, i+1 );

    // create and initialize the ith link
    devODEBody* body;
    body = new devODEBody( world.WorldID(),                         // world
			   spaceid,                                 // space
			   Rtwi,                                    // pos+ori
			   links[i].Mass(),                         // m   
			   links[i].CenterOfMass(),                 // com
			   links[i].MomentOfInertiaAtCOM(),         // I  
			   geomfiles[i] );

    world.Insert( body );
    bodies.push_back( body );
  }

  // Initialize the joints
  dBodyID b1 = 0;                                // ID of initial proximal link
  vctFixedSizeVector<double,3> z(0.0, 0.0, 1.0); // the local Z axis

  for( size_t i=0; i<links.size(); i++ ){

    // obtain the ID of the distal link 
    dBodyID b2 = bodies[i]->BodyID();
    
    // obtain the position and orientation of the ith link
    vctFrame4x4<double> Rtwi = ForwardKinematics( qinit, i );
    
    vctFixedSizeVector<double,3> anchor = Rtwi.Translation();
    vctFixedSizeVector<double,3> axis = Rtwi.Rotation() * z;

    int type = dJointTypeHinge;
    if( links[i].GetType() == robLink::SLIDER )
      { type = dJointTypeSlider; }

    // This is a bit tricky. The min must be greater than -pi and the max must
    // be less than pi. Otherwise it really screw things up
    double qmin = links[i].PositionMin();
    double qmax = links[i].PositionMax();

    devODEJoint* joint;
    joint =  new devODEJoint( world.WorldID(),     // the world ID
			      b1,                  // the proximal body
			      b2,                  // the distal body
			      type,                // the joint type
			      anchor,              // the XYZ position
			      axis,                // the Z axis 
			      qmin,                // the lower limit
			      qmax );              // the upper limit
    joints.push_back( joint );
    world.Insert( joint );

    b1 = b2;                               // proximal is now distal
  }
}

void devODEManipulator::Attach( robManipulator* tool ){

  devODEManipulator* odetool = dynamic_cast<devODEManipulator*>( tool );

  if( odetool != NULL ){

    dJointID jid = dJointCreateSlider( WorldID(), 0 );
    dJointAttach( jid, bodies.back()->BodyID(), odetool->BaseID() );
    dJointSetSliderAxis( jid, 0.0, 0.0, 1.0 );
    
    dJointSetSliderParam( jid, dParamLoStop, 0.0 );
    dJointSetSliderParam( jid, dParamHiStop, 0.0 );

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

dBodyID devODEManipulator::BaseID() const {
  if( base == NULL ) return NULL;
  else               return base->BodyID();
}

void devODEManipulator::Insert( devODEBody* body )
{ bodies.push_back( body ); }

void devODEManipulator::Insert( devODEJoint* joint )
{ joints.push_back( joint ); }

vctDynamicVector<double> devODEManipulator::Read()
{ return GetJointsPositions(); }

void devODEManipulator::Write( const vctDynamicVector<double>& ft )
{ SetForcesTorques( ft ); }

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


void devODEManipulator::SetForcesTorques( const vctDynamicVector<double>& ft) {
  for(size_t i=0; i<joints.size(); i++ )
    { joints[i]->SetForceTorque( ft[i] ); }
}

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


