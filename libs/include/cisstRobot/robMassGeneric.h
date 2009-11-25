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

#ifndef _robMassGeneric_h
#define _robMassGeneric_h

#include <iostream>

#include <cisstRobot/robMassBase.h>

class robMassGeneric : public robMassBase{

protected:

  //! The mass
  double mass; 

  //! The center of mass
  /**
     The center of mass is expressed with respect to the coordinate frame
     of the body. That is, this member represents the translation of the center 
     of mass with respect to the body's coordinate frame.This implies that the 
     coordinate frame of the body does not necessarily coincide with the center 
     of mass. 
  */
  vctFixedSizeVector<double,3> com;

  //! The principal moments of inertia
  /**
     The principal moments of inertia are defined in a coordinate frame that
     is centered at the center of mass. The principal moments are contained in 
     the diagonal matrix 
     \f$ D = 
     \begin{bmatrix} I_x & 0 & 0 \\ 0 & I_y & 0 \\ 0 & 0 & I_z \end{bmatrix}
     \f$
  */
  vctFixedSizeMatrix<double,3,3,VCT_ROW_MAJOR> D; 

  //! The principal axes
  /**
     This matrix represents the coordinates of the principal axes associated 
     with the principal moment of inertia. The three axes are defined with 
     respect to the body's coordinate frame. That is, the axes represent a
     similarity transformation that diagonalizes a moment of inertia tensor. 
     Given a moment of inertia tensor \f$ I \$, then the principal axes define a
     similarity transformation \f$V\f$ that diagonalizes the \f$ I \f$
     with \f$ D = V^T I V \f$, where \f$ V\f$ are the eigenvectors of \f$ I\f$
     given by \f$ IV = VD\f$. The matrix \f$ V\f$ refpresents the principal axes
     as follow 
     \f$V=\begin{bmatrix}\mathbf{e}_1&\mathbf{e}_2&\mathbf{e}_3\end{bmatrix}\f$.
  */
  vctMatrixRotation3<double,VCT_ROW_MAJOR> V;

public:

  //! Default constructor
  /**
     Set the mass, center of mass and inertia to zero
  */
  robMassGeneric(){
    mass = 0.0;       // set the mass to zero
    com.SetAll(0.0);  // set the center of mass to zero
    D.Eye();          // set the principal moment of inertia to zero
    V.Eye();          // set the principal axes to identity
  }

  //! Default destructor
  ~robMassGeneric(){}

  //! Return the mass
  /**
     \return The mass of the body
  */
  double Mass() const { return mass; }

  //! Return the center of mass
  /**
     Return the center of mass. The center of mass is with respect to the 
     coordinate frame of the body.
     \return A 3D vector representing the center of mass
  */
  vctFixedSizeVector<double,3> CenterOfMass() const { return com; }

  //! Return the moment of inertia tensor
  /**
     Return the moment of inertia tensor. The tensor is with respect to the
     coordinate frame of the body which does not necessarily coincide with the
     center of mass of the body and the principal axes.
     \return A 3x3 moment of inertia tensor
  */
  vctFixedSizeMatrix<double,3,3,VCT_ROW_MAJOR> MomentOfInertia() const { 
    // Rotate and translate the moment of inertia to the body's origin.
    return ParallelAxis( Mass(), -CenterOfMass(), V.Transpose() * D * V );
  }
  
  //! Set the mass parameters
  /**
     Set the mass parameters. This copies and transforms the parameters. The
     mass is simply copied. The center of mass is also copied as it is already
     expressed in the body's coordinate frame. The inertia, however, is 
     expressed about a coordinate frame coinciding with the center of mass and
     aligned with the body's coordinate frame. The reason why the inertia must 
     be expressed about the center of mass is because it enables to translate 
     the tensor with the parallel axis theorem.
     \param mass The mass of the body
     \param COM The center of gravity with respect to the body's coordinate 
                frame
     \param D The principal moments of inertia
     \param V The principal axis with respect to the body's coordinate frame.
              \f$ V = \begin{bmatrix}\mathbf{e}_1&\mathbf{e}_2&\mathbf{e}_3
	              \end{bmatrix} \f$
  */
  void SetDynamicsParameters( double mass, 
			      const vctFixedSizeVector<double,3>& com,
			      const vctFixedSizeVector<double,3>& d,
			      const vctMatrixRotation3<double>& V ){
    this->mass = mass;
    this->com = com;
    this->D = vctFixedSizeMatrix<double,3,3,VCT_ROW_MAJOR>( d[0],  0.0, 0.0,
							    0.0,  d[1], 0.0,
							    0.0,   0.0, d[2] );
    this->V = V;
  }

};

#endif
