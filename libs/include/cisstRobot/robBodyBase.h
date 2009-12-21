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

#ifndef _robBodyBase_h
#define _robBodyBase_h

#include <iostream>

#include <cisstVector/vctFrame4x4.h>

#include <cisstRobot/robMass.h>
#include <cisstRobot/robGeom.h>
#include <cisstRobot/robDefines.h>
#include <cisstRobot/robExport.h>

//! A base class for a body
/**
   A body is defined as the union of a coordinate frame (position and 
   orientation), a mass and a geometry.
*/

class CISST_EXPORT robBodyBase : public vctFrame4x4<double,VCT_ROW_MAJOR>,
				 public robMass,
				 public robGeom {
  
public:
  
  //! Default constructor
  robBodyBase();

  //! Default destructor
  virtual ~robBodyBase();

  //! Read the body from an input stream
  virtual robError Read( std::istream& is );

  //! Write the body to an output stream
  virtual robError Write( std::ostream& os ) const;

  //! Set the position and orientation
  /**
     Overload the = operator to be able to set the position and orientation
     of the body with an assignment. This sets the position and orientation of
     the body as well as the position and orientation of the geometry.
     \param Rt The new position and orientation 
  */
  virtual robBodyBase& operator=( const vctFrame4x4<double,VCT_ROW_MAJOR>& Rt );

  //! Set the body's position
  /**
     This sets both the position of the body and the position of the geometry.
     \param t The body's position
  */
  virtual void SetBodyPosition( const vctFixedSizeVector<double,3>& t );

  //! Set the body's orientation
  /**
     This sets both the orientation of the body and the orientation of the
     geometry.
     \param r The body's orientation 
  */
  virtual 
    void 
    SetBodyOrientation( const vctMatrixRotation3<double,VCT_ROW_MAJOR>& R );

  //! Set the body's linear velocity
  virtual void SetBodyLinearVelocity( const vctFixedSizeVector<double,3>& v);

  //! Set the body's angular velocity
  virtual void SetBodyAngularVelocity( const vctFixedSizeVector<double,3>& w);

  //! Set force to the body
  virtual void SetBodyForce( const vctFixedSizeVector<double,3>& f);

  //! Set torque to the body
  virtual void SetBodyTorque( const vctFixedSizeVector<double,3>& t);

  //! Get the body's position
  /**
     This returns the position of the body and not the position of the geometry
     (even though they are equal)
     \return The body's position
  */
  virtual vctFixedSizeVector<double,3> GetBodyPosition();

  //! Get the body's orientation
  /**
     This returns the orientation of the body and not the orientation of the
     geometry
     (even though they are equal)
     \return The body's orientation
  */
  virtual vctMatrixRotation3<double,VCT_ROW_MAJOR> GetBodyOrientation();
  
  //! Get the body's linear velocity
  virtual vctFixedSizeVector<double,3> GetBodyLinearVelocity();

  //! Get the body's angular velocity
  virtual vctFixedSizeVector<double,3> GetBodyAngularVelocity();

  //! Get the force applied to the body
  virtual vctFixedSizeVector<double,3> GetBodyForce();

  //! Get the torque applied to the body
  virtual vctFixedSizeVector<double,3> GetBodyTorque();

};

#endif
