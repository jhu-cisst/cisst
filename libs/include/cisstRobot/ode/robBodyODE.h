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

#include <ode/ode.h>

#include <cisstRobot/robBodyBase.h>

class robBodyODE : public robBodyBase {

private:
  
  //! The ODE body ID
  dBodyID bodyid;

public:

  //! Default constructor
  robBodyODE();

  //! Default destructor
  ~robBodyODE();
  
  //! Configure the body
  /**
     Create the ODE body and set the collision space of its geometry
     \param world The ODE world of the body
     \param space The ODE space in which the geometry will be added
  */
  void ConfigureBody( dWorldID world, dSpaceID space );

  //! Query the ID of the body
  dBodyID BodyID() const;

  //! Enable the body for ODE
  void Enable();
  
  //! Disable the body for ODE
  void Disable();

  //! Update the position and orientation
  /**
     This queries ODE to obtain the position and orientation of the body. Then
     it sets the position and orientation of the base class.
  */
  void Update();

  //! Set the position and orientation of the body
  /**
     Overload the assignment operator to set the position and orienation of the
     body.
     \param Rt The new position and orientation of the body coordinate frame 
               with respect to an inertial coordinate frame.
  */
  robBodyODE& operator=( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt );


  //! Set the body's position
  /**
     This sets the position of the body in ODE.
     \param t The body's position
  */
  void SetBodyPosition( const vctFixedSizeVector<double,3>& t );

  //! Set the body's orientation
  /**
     This sets the orientation of the body in ODE.
     geometry.
     \param r The body's orientation 
  */
  void SetBodyOrientation(const vctMatrixRotation3<double,VCT_ROW_MAJOR>& R);

  //! Set the body's linear velocity
  void SetBodyLinearVelocity( const vctFixedSizeVector<double,3>& v);

  //! Set the body's angular velocity
  void SetBodyAngularVelocity( const vctFixedSizeVector<double,3>& w);

  //! Set force to the body
  void SetBodyForce( const vctFixedSizeVector<double,3>& f);

  //! Set torque to the body
  void SetBodyTorque( const vctFixedSizeVector<double,3>& t);

  //! Get the body's position
  /**
     This returns the position of the body obtained from ODE.
     \return The body's position
  */
  vctFixedSizeVector<double,3> GetBodyPosition();

  //! Get the body's orientation
  /**
     This returns the orientation of the body from ODE.
     \return The body's orientation
  */
  vctMatrixRotation3<double,VCT_ROW_MAJOR> GetBodyOrientation();
  
  //! Get the body's linear velocity
  vctFixedSizeVector<double,3> GetBodyLinearVelocity();

  //! Get the body's angular velocity
  vctFixedSizeVector<double,3> GetBodyAngularVelocity();

  //! Get the force applied to the body
  vctFixedSizeVector<double,3> GetBodyForce();

  //! Get the torque applied to the body
  vctFixedSizeVector<double,3> GetBodyTorque();

};

#endif
