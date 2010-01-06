#include <cisstRobot/ode/robBodyODE.h>

robBodyODE::robBodyODE(){}
robBodyODE::~robBodyODE(){}

dBodyID 
robBodyODE::BodyID() const { 

  return bodyid; 

}

void
robBodyODE::ConfigureBody( dWorldID world, dSpaceID space ){

  bodyid = dBodyCreate( world );
  ConfigureMassODE( BodyID() );
  ConfigureMeshTriangularODE( space );

}

//! Enable the body for ODE
void
robBodyODE::Enable(){ 

  dBodyEnable( BodyID() ); 

}

//! Disable the body for ODE
void 
robBodyODE::Disable(){

  dBodyDisable( BodyID() ); 

}
  
void robBodyODE::Update() {

  robBodyBase::SetBodyPosition( GetBodyPosition() );
  robBodyBase::SetBodyOrientation( GetBodyOrientation() );

}

robBodyODE& 
robBodyODE::operator=( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt ){

  // ensures that the reference isn't the same as the current body
  if( *this != Rt ){
    
    // Position of the body's coordinate frame
    vctFixedSizeVector<double,3> t( Rt[0][3], Rt[1][3], Rt[2][3] );
    // Orientation of body's coordinate frame
    vctMatrixRotation3<double,VCT_ROW_MAJOR> R( Rt[0][0], Rt[0][1], Rt[0][2],
						Rt[1][0], Rt[1][1], Rt[1][2],
						Rt[2][0], Rt[2][1], Rt[2][2] );
    
    SetBodyOrientation( R ); // order matter since the position depends on the 
    SetBodyPosition( t );    // orientation

  }
  
  return *this;

}

void 
robBodyODE::SetBodyPosition( const vctFixedSizeVector<double,3>& t0i ){ 

  robBodyBase::SetBodyPosition( t0i );

  // position of the center of mass wrt body frame
  vctFixedSizeVector<double,3> ticom = CenterOfMass();

  // orientation of body frame wrt inertial frame
  vctMatrixRotation3<double,VCT_ROW_MAJOR> R0i = GetBodyOrientation();

  // position of center of mass wrt inertial frame
  vctFixedSizeVector<double,3> t0com = t0i + R0i*ticom;

  dBodySetPosition( BodyID(), t0com[0], t0com[1], t0com[2] );

}

void 
robBodyODE::SetBodyOrientation(const vctMatrixRotation3<double,VCT_ROW_MAJOR>& R){ 

  robBodyBase::SetBodyOrientation( R );

  dMatrix3 r = { R[0][0], R[0][1], R[0][2], 0.0,
		 R[1][0], R[1][1], R[1][2], 0.0,
		 R[2][0], R[2][1], R[2][2], 0.0 };
  dBodySetRotation( BodyID(), r );


}

void 
robBodyODE::SetBodyLinearVelocity( const vctFixedSizeVector<double,3>& v ){ 

  robBodyBase::SetBodyLinearVelocity( v );

  dBodySetLinearVel( BodyID(), v[0], v[1], v[2] ); 

}

void 
robBodyODE::SetBodyAngularVelocity( const vctFixedSizeVector<double,3>& w ){ 

  robBodyBase::SetBodyAngularVelocity( w );

  dBodySetAngularVel( BodyID(), w[0], w[1], w[2] );

}

void
robBodyODE::SetBodyForce( const vctFixedSizeVector<double,3>& f ){
  
  robBodyBase::SetBodyForce( f );

  dBodyAddForce( BodyID(), f[0], f[1], f[2] ); 

}

void
robBodyODE::SetBodyTorque( const vctFixedSizeVector<double,3>& t ){ 

  robBodyBase::SetBodyTorque( t );

  dBodyAddTorque( BodyID(), t[0], t[1], t[2] );

}

vctFixedSizeVector<double,3> 
robBodyODE::GetBodyPosition(){

  // get the ODE position (position of the center of mass) wrt inertial frame
  const dReal* t = dBodyGetPosition( BodyID() );
  vctFixedSizeVector<double,3> t0com( t[0], t[1], t[2] );

  // position of the center of mass with respect to the body frame
  vctFixedSizeVector<double,3> ticom = CenterOfMass();
    
  // orientation of the body wrt inertial frame
  vctMatrixRotation3<double,VCT_ROW_MAJOR> R0i = GetBodyOrientation();

  // position of the body coordinate frame wrt inertial frame
  vctFixedSizeVector<double,3> t0i = t0com - R0i*ticom;

  // propagate the position to the base class
  robBodyBase::SetBodyPosition( t0i );

  return t0i;

}

vctMatrixRotation3<double,VCT_ROW_MAJOR> 
robBodyODE::GetBodyOrientation(){

  const dReal* r = dBodyGetRotation( BodyID() );
  vctMatrixRotation3<double,VCT_ROW_MAJOR> R( r[0*4+0], r[0*4+1], r[0*4+2],
					      r[1*4+0], r[1*4+1], r[1*4+2],
					      r[2*4+0], r[2*4+1], r[2*4+2]);

  // propagate to the base class
  robBodyBase::SetBodyOrientation( R );

  return R;

}

vctFixedSizeVector<double,3> 
robBodyODE::GetBodyLinearVelocity(){

  const dReal* v = dBodyGetLinearVel( BodyID() );
  vctFixedSizeVector<double,3> V( v[0], v[1], v[2] );

  // propagate to the base class
  robBodyBase::SetBodyLinearVelocity( V );

  return V;

}

vctFixedSizeVector<double,3>
robBodyODE::GetBodyAngularVelocity(){
  
  const dReal* w = dBodyGetAngularVel( BodyID() );
  vctFixedSizeVector<double,3> W( w[0], w[1], w[2] );
  
  // propagate to the base class
  robBodyBase::SetBodyAngularVelocity( W );

  return W;

}


vctFixedSizeVector<double,3> 
robBodyODE::GetBodyForce(){ 

  const dReal* f = dBodyGetForce( BodyID() );
  vctFixedSizeVector<double,3> F( f[0], f[1], f[2] );

  // propagate to the base class
  robBodyBase::SetBodyForce( F );

  return F;

}

vctFixedSizeVector<double,3> 
robBodyODE::GetBodyTorque(){

  const dReal* t = dBodyGetTorque( BodyID() );
  vctFixedSizeVector<double,3> T( t[0], t[1], t[2] );
  
  // propagate to the base class
  robBodyBase::SetBodyTorque( T );

  return T;

}

