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

private:

  //! The mass
  double mass; 

  //! The center of mass
  /**
     The center of mass is expressed with respect to the coordinate frame
     of the body. This implies that the coordinate frame of the body does not
     necessarily coincide with the center of mass.
  */
  vctFixedSizeVector<double,3> com;

  //! The moment of inertia tensor
  /**
     The tensor is expressed with respect to the coordinate frame
     of the body. This implies that the coordinate frame of the body does not
     necessarily coincide with the center of mass.
  */
  vctFixedSizeMatrix<double,3,3> moit;

public:

  //! Default constructor
  /**
     Set the mass, center of mass and inertia to zero
  */
  robMassGeneric(){
    mass = 0.0;
    com.SetAll(0.0);
    moit.SetAll(0.0);
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
     center of mass of the body.
     \return A 3x3 moment of inertia tensor
  */
  vctFixedSizeMatrix<double,3,3> MomentOfInertia() const { return moit; }
  
public:

  //! Set the mass parameters
  /**
     Set the mass parameters. This copies and transforms the parameters. The
     mass is simply copied. The center of mass is also copied as it is already
     expressed in the body's coordinate frame. The inertia, however, is expressed
     about a coordinate frame coinciding with the center of mass and aligned 
     with the body's coordinate frame. The reason why the inertia must be 
     expressed about the center of mass is because it enables to translate the
     tensor with the parallel axis theorem.
     \param mass The mass of the body
     \param COM The center of gravity with respect to the body's coordinate frame
     \param MOIT The moment of inertia tensor with respect to a coordinate frame 
                 with the origin at the center of mass and aligned with the 
		 body's coordinate frame                 
  */
  void SetDynamicsParameters( double mass, 
			      const vctFixedSizeVector<double,3>& com,
			      const vctFixedSizeMatrix<double,3,3>& moit,
			      const vctFrame4x4<double,VCT_ROW_MAJOR>& = 
			      vctFrame4x4<double,VCT_ROW_MAJOR>()){
    this->mass = mass;
    this->com = com;
    this->moit = moit;
    this->moit = ParallelAxis( -com ); // shift the moment of inertia from the 
                                       // center of mass to the body's coordinate
                                       // frame
  }

};

#endif
