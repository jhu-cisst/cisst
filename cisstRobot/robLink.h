/*
  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robLink_h
#define _robLink_h

#include <iostream>

#include <cisstRobot/robKinematics.h>
#include <cisstRobot/robMass.h>

#if CISST_HAS_JSON
#include <json/json.h>
#endif

#include <cisstRobot/robExport.h>

//! A robot link
/**
   robLink implements the methods necessary to model a robot link. The class is
   derived from robBody to store the dynamics parameters of the body and the
   link's position and orientation. The link is also derived from robDH to
   determine the link's position and orientation from joint values
*/
class CISST_EXPORT robLink {

 protected:

  robKinematics* kinematics;
  robMass        mass;

 public:

  enum Errno { ESUCCESS, EFAILURE };

  //! Default constructor
  robLink();

  //! Copy constructor
  robLink( const robLink& link );

  //! Overloaded constructor
  robLink( robKinematics* kinematics, const robMass& mass );

  //! Default destructor
  ~robLink();

  //! Read the DH and body parameters
  /**
     First read the DH parameters and then the body's parameters. At the
     end of the day, the parameters must be in the following order
     DH convention (string): modified or standard
     \f$ \alpha \f$ (1 double): DH parameter
     \f$ a \f$ (1 double): DH parameter
     \f$ \theta \f$ (1 double): DH parameter
     \f$ d \f$ (1 double): DH parameter
     joint type (string): revolute/hinge or prismatic/slider
     joint mode (string): active or passive
     joint offset position (1 double): value added to each joint value
     joint minimum position (1 double): lower joint limit
     joint maximum position (1 double): upper joint limit
     joint maximum force/torque (1 double): absolute force/torque limit
     mass (1 double): The mass of the body
     center of mass (3 double): \f$ \matrix{x & y & z} \f$
     principal moment of inertia (3 double):
                          \f$ \matrix{I_{xx} & I_{yy} & I_{zz}} \f$
     body principal axis (9 double):
  */
  robLink::Errno Read( std::istream& is );

#if CISST_HAS_JSON
  robLink::Errno Read( const Json::Value & linkConfig );
#endif

  //! Write the DH and body parameters
  robLink::Errno Write( std::ostream& os ) const;

  vctFrame4x4<double> ForwardKinematics( double q ) const;


  vctFixedSizeVector<double,3> PStar() const;
  vctMatrixRotation3<double> Orientation( double q ) const;

  robKinematics* GetKinematics() const;

  const robMass & MassData(void) const;
  robMass & MassData(void);

  robKinematics::Convention GetConvention() const;

  robJoint::Type GetType() const;

  double Mass() const { return mass.Mass(); }

  vctFixedSizeVector<double,3> CenterOfMass() const
  { return mass.CenterOfMass(); }

  vctFixedSizeMatrix<double,3,3> MomentOfInertiaAtCOM() const
  { return mass.MomentOfInertiaAtCOM(); }

  vctFixedSizeMatrix<double,3,3> MomentOfInertia() const
  { return mass.MomentOfInertia(); }

  double PositionMin() const {
    if( kinematics != NULL ) { return kinematics->PositionMin(); }
    return 0.0;
  }

  double PositionMax() const {
    if( kinematics != NULL ) { return kinematics->PositionMax(); }
    return 0.0;
  }

  double ForceTorqueMax() const {
    if( kinematics != NULL ) { return kinematics->ForceTorqueMax(); }
    return 0.0;
  }

};

#endif // _robLink_h
