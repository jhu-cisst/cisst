/*

  Author(s): Simon Leonard
  Created on: November 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robBodyODE_h
#define _robBodyODE_h

#ifdef CISST_ODE_SUPPORT

#include <ode/ode.h>
#include <cisstRobot/robBodyGeneric.h>

class robBodyODE : public robBodyGeneric {

private:
  
  //! The ODE body ID
  dBodyID bodyid;

  //! Set position
  void SetPosition( const vctFixedSizeVector<double,3>& t0i ){ 
    // position of the center of mass wrt body frame
    vctFixedSizeVector<double,3> ticom = CenterOfMass();
    // orientation of body frame wrt inertial frame
    vctMatrixRotation3<double,VCT_ROW_MAJOR> R0i = GetOrientation();
    // position of center of mass wrt inertial frame
    vctFixedSizeVector<double,3> t0com = t0i + R0i*ticom;
    dBodySetPosition( BodyID(), t0com[0], t0com[1], t0com[2] );
  }

  //! Set orientation
  void SetOrientation( const vctMatrixRotation3<double,VCT_ROW_MAJOR>& R ){  
    dMatrix3 r = { R[0][0], R[0][1], R[0][2], 0.0,
		   R[1][0], R[1][1], R[1][2], 0.0,
		   R[2][0], R[2][1], R[2][2], 0.0 };
    dBodySetRotation( BodyID(), r );
  }

public:

  //! Default constructor
  robBodyODE(){}
  ~robBodyODE(){}
  

  //! Configure the body
  /**
     Create the ODE body and set the collision space of its geometry
     \param world The ODE world of the body
     \param space The ODE space in which the geometry will be added
  */
  void Configure( dWorldID world, 
		  dSpaceID space ){
    bodyid = dBodyCreate( world );                  // create the body
    robMassODE::Configure( BodyID() );
    robMeshODE::Configure( space );
  }

  //! Query the ID of the body
  dBodyID BodyID() const { return bodyid; }

  //! Set the position and orientation of the body
  /**
     Overload the assignment operator to set the position and orienation of the
     body.
     \param Rt The new position and orientation of the body coordinate frame 
               with respect to an inertial coordinate frame.
  */
  robBodyODE& operator=( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt ) {

    // ensures that the reference isn't the same as the current body
    if( *this != Rt ){
      // call the base class operator
      robBodyGeneric::operator=( Rt );

      // Position of the body's coordinate frame
      vctFixedSizeVector<double,3> t( Rt[0][3], Rt[1][3], Rt[2][3] );
      // Orientation of body's coordinate frame
      vctMatrixRotation3<double,VCT_ROW_MAJOR> R(Rt[0][0], Rt[0][1], Rt[0][2],
						 Rt[1][0], Rt[1][1], Rt[1][2],
						 Rt[2][0], Rt[2][1], Rt[2][2]);

      // Set the ODE position and ODE orientation. This differs from the
      // generic one by shifting the coordinate frame to the center of mass
      SetOrientation( R ); // order matter since the position depends on the 
      SetPosition( t );    // orientation
    }
    
    return *this;
  }

  //! Add a force
  /**
     For an ODE body, this adds a force at the body's center of mass. Forces
     are cumulative so you each call adds a force (as oppose to set a force).
     Also, the forces are cleared after each simulation step.
     \param f The 3D force. 
  */
  void AddForce( const vctFixedSizeVector<double,3>& f )
  { dBodyAddForce( BodyID(), f[0], f[1], f[2] ); }

  //! Add a torque
  /**
     For an ODE body, this adds a torque at with respect to the body's center of 
     mass. Torques are cumulative so you each call adds a torque (as oppose to 
     set a torque). Also, the torques are cleared after each simulation step.
     \param t The 3D force. 
  */
  void AddTorque( const vctFixedSizeVector<double,3>& t )
  { dBodyAddTorque( BodyID(), t[0], t[1], t[2] ); }

  //! Get total force
  /**
     Return the effective force acting on the body's center of mass
     \return The effective force
  */
  vctFixedSizeVector<double,3> GetForce()  const{ 
    const dReal* f = dBodyGetForce( BodyID() );
    return vctFixedSizeVector<double,3>( f[0], f[1], f[2] );
  }

  //! Get total torque
  /**
     Return the effective torque acting on the body's center of mass
     \return The effective torque
  */
  vctFixedSizeVector<double,3> GetTorque() const{
    const dReal* t = dBodyGetTorque( BodyID() );
    return vctFixedSizeVector<double,3>( t[0], t[1], t[2] );
  }

  //! Set linear velocity
  void SetLinearVelocity( const vctFixedSizeVector<double,3>& v )
  { dBodySetLinearVel( BodyID(), v[0], v[1], v[2] ); }

  //! Set angular velocity
  void SetAngularVelocity( const vctFixedSizeVector<double,3>& w )
  { dBodySetAngularVel( BodyID(), w[0], w[1], w[2] ); }
  
  //! Get linear velocity
  vctFixedSizeVector<double,3> GetLinearVelocity()  const{
    const dReal* v = dBodyGetLinearVel( BodyID() );
    return vctFixedSizeVector<double,3>( v[0], v[1], v[2] );
  }

  //! Get angular velocity
  vctFixedSizeVector<double,3> GetAngularVelocity() const{
    const dReal* w = dBodyGetAngularVel( BodyID() );
    return vctFixedSizeVector<double,3>( w[0], w[1], w[2] );
  }

  //! Get position
  vctFixedSizeVector<double,3> GetPosition() const{
    // get the ODE position (position of the center of mass) wrt inertial frame
    const dReal* t = dBodyGetPosition( BodyID() );
    vctFixedSizeVector<double,3> t0com( t[0], t[1], t[2] );

    // position of the center of mass with respect to the body frame
    vctFixedSizeVector<double,3> ticom = CenterOfMass();
    
    // orientation of the body wrt inertial frame
    vctMatrixRotation3<double,VCT_ROW_MAJOR> R0i = GetOrientation();

    // position of the body coordinate frame wrt inertial frame
    vctFixedSizeVector<double,3> t0i = t0com - R0i*ticom;

    return vctFixedSizeVector<double,3>( t0i[0], t0i[1], t0i[2] );
  }

  //! Get orientation
  vctMatrixRotation3<double,VCT_ROW_MAJOR> GetOrientation() const{
    const dReal* R = dBodyGetRotation( BodyID() );
    return vctMatrixRotation3<double,VCT_ROW_MAJOR>(R[0*4+0],R[0*4+1],R[0*4+2],
						    R[1*4+0],R[1*4+1],R[1*4+2],
						    R[2*4+0],R[2*4+1],R[2*4+2]);
  }

  //! Enable the body for ODE
  void Enable(){ dBodyEnable( BodyID() ); }

  //! Disable the body for ODE
  void Disable(){ dBodyDisable( BodyID() ); }
  
  void Update(){
    vctMatrixRotation3<double,VCT_ROW_MAJOR> R = GetOrientation();
    vctFixedSizeVector<double,3> t = GetPosition();
    vctFrame4x4<double,VCT_ROW_MAJOR> Rt(R,t);
    robBodyGeneric::operator=( Rt );
  }

};

#endif

#endif
