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

#ifndef _robMassODE_h
#define _robMassODE_h

#ifdef CISST_ODE_SUPPORT

#include <ode/ode.h>

#include <cisstRobot/robMassGeneric.h>
#include <cisstRobot/robExport.h>

//! A mass for the ODE physics engine
/**
   This mass is derived from the generic one. Hence it can be used for robot
   control. It extends to ODE by adding the dMass structure. The main difference
   between the mass parameters for ODE (dMass) and those already present in 
   robMassGeneric are the reference point. For robot control purposes, the 
   coordinate frame of each link is at a joint and the center of mass and 
   inertia are defined with respect to that frame. In general, this coordinate 
   frame does not coincide with the center of mass of the link. For ODE the 
   coordinate frame of the link (or body) *must* coincide with the center
   of mass and the inertia must be defined with respect to that frame.
*/
class CISST_EXPORT robMassODE : public robMassGeneric {

private:

  dMass mass;
  
public:
  
  //! Set the mass parameters
  /**
     \param mass The mass of the body
     \param COM The center of gravity with respect to the body's coordinate frame
     \param MOIT The moment of inertia tensor with respect to a coordinate frame 
                 with the origin at the center of mass and aligned with the 
		 body's coordinate frame                 
  */
  void SetParameters( double mass, 
		      const vctFixedSizeVector<double,3>& com,
		      const vctFixedSizeMatrix<double,3,3>& moit,
		      const vctFrame4x4<double,VCT_ROW_MAJOR>& =
		            vctFrame4x4<double,VCT_ROW_MAJOR>() ){

    // set the mass parameters used for the inverse dynamics of the robot
    // robMassGeneric::SetParameters will use the opposite of com to find 
    // the inertia wrt to the links coordinate frame (as opposed to the to the
    // inertia wrt to the center of mass)
    robMassGeneric::SetParameters( mass, com, moit );

    // Set the mass parameters for ODE the COM must be the body
    // reference (0,0,0) and the MOIT must be centered around the COM
    dMassSetParameters( &(this->mass),
			mass,
			0.0, 0.0, 0.0,
			moit[0][0], moit[1][1], moit[2][2],
			moit[0][1], moit[0][2], moit[1][2] );
  }
  
public:

  //! Default constructor
  robMassODE(){}
  //! Default destructor
  ~robMassODE(){}

  //! Set the mass of the body
  void Configure( dBodyID bodyid ) { dBodySetMass( bodyid, &mass ); }

};


#endif // ODE

#endif
