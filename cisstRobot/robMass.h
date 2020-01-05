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

#ifndef _robMass_h
#define _robMass_h

#include <iostream>

#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctFrame4x4.h>
#if CISST_HAS_JSON
#include <json/json.h>
#endif
#include <cisstRobot/robExport.h>

class CISST_EXPORT robMass{
 public:

  enum Errno{ ESUCCESS, EFAILURE };

 private:

  //! The mass (kg)
  double mass;

  //! The center of mass (m)
  /**
     The center of mass is expressed with respect to the coordinate frame
     of the body. That is, this member represents the translation of the center
     of mass with respect to the body's coordinate frame.This implies that the
     coordinate frame of the body does not necessarily coincide with the center
     of mass.
  */
  vctFixedSizeVector<double,3> com;

  //! The principal moments of inertia (\f$ \textrm{kgm}^2 \f$)
  /**
     The principal moments of inertia are defined in a coordinate frame that
     is centered at the center of mass and with axes aligned with the principal
     axes. The principal moments are contained in the diagonal matrix
     \f$ D =
     \matrix{ I_x & 0 & 0 \\ 0 & I_y & 0 \\ 0 & 0 & I_z }
     \f$
  */
  vctFixedSizeMatrix<double,3,3> D;

  //! The principal axes
  /**
     This matrix represents the coordinates of the principal axes associated
     with the principal moment of inertia. The three axes are defined with
     respect to the body's coordinate frame. The axes represent a similarity
     transformation that diagonalizes a moment of inertia tensor. Given a moment
     of inertia tensor \f$ I \f$, the principal axes define a similarity
     transformation \f$V\f$ that diagonalizes \f$ I \f$ with
     \f$ D = V^T I V \f$, where \f$ V\f$ are the eigenvectors of \f$ I\f$ given
     by \f$ IV = VD\f$. The matrix \f$ V\f$ refpresents the principal axes as
     follow
     \f$V=\matrix{\mathbf{e}_1&\mathbf{e}_2&\mathbf{e}_3}\f$.
  */
  vctMatrixRotation3<double> V;

public:

  //! Parallel Axis Theorem
  /**
     Finds the moment of inertia with respect to a parallel axis
  */
  vctFixedSizeMatrix<double,3,3>
  ParallelAxis( double m,
		const vctFixedSizeVector<double,3>& t,
		const vctFixedSizeMatrix<double,3,3>& I ) const;


  //! Default constructor
  /**
     Set the mass, center of mass and inertia to zero
  */
  robMass();

  robMass( double m,
	   const vctFixedSizeVector<double,3>& com,
	   const vctFixedSizeMatrix<double,3,3>& D,
	   const vctFixedSizeMatrix<double,3,3>& V );

  //! Return the mass
  /**
     \return The mass of the body
  */
  const double & Mass(void) const;
  double & Mass(void);

  //! Return the center of mass
  /**
     Return the center of mass. The center of mass is expressed in the
     coordinate frame of the body.
     \return A 3D vector representing the center of mass
  */
  vctFixedSizeVector<double,3> CenterOfMass() const;

  //! Return the moment of inertia tensor in the body frame
  /**
     Return the moment of inertia tensor. The tensor is with respect to the
     coordinate frame of the body which does not necessarily coincide with the
     center of mass of the body and the principal axes.
     \return A 3x3 moment of inertia tensor
  */
  vctFixedSizeMatrix<double,3,3> MomentOfInertia() const;

  vctFixedSizeMatrix<double,3,3> MomentOfInertiaAtCOM() const;

  //! Read the mass from a input stream
  robMass::Errno ReadMass( std::istream& is );
#if CISST_HAS_JSON
  robMass::Errno ReadMass( const Json::Value &config );
#endif

  //! Write the mass from a output stream
  robMass::Errno WriteMass( std::ostream& os ) const;

};

#endif
