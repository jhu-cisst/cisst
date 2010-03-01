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

#ifndef _robVariable_h
#define _robVariable_h

#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctFrame4x4.h>
#include <cisstRobot/robSpace.h>
#include <cisstRobot/robExport.h>

//! Variables of a robot.
/**
   robVariable defines the variables used to generate trajectories and control
   a robot. The variables include joint position, velocities and accelerations
   as well as Cartesian position, velocities and accelerations. Forces and
   torques are also available. Once a variable is create you can change the 
   values of the variable but you cannot change the basis of the space it 
   belongs.
*/
class CISST_EXPORT robVariable : public robSpace {
  
 public:

  /*** Time Variable ***/

  //! Time variable
  /**
     Use this variable for time varying trajectories
  */
  double time;

  //double x, xd, xdd;
  
  /*** Cartesian Space Variable ***/
  
  //! Cartesian orientation
  /**
     A SO3 variable. Use this variable for Cartesian position trajectories
  */
  vctMatrixRotation3<double> R;
  vctFixedSizeVector<double,3> w;
  vctFixedSizeVector<double,3> wd;

  vctFixedSizeVector<double,3> t;
  vctFixedSizeVector<double,3> v;
  vctFixedSizeVector<double,3> vd;

  vctFixedSizeVector<double,9> q;
  vctFixedSizeVector<double,9> qd;
  vctFixedSizeVector<double,9> qdd;

  //! Clear the data structures
  void Clear();
  
 public:
  
  //! Create a variables mask with all variables turned "off"
  robVariable();
  
  //! Create a variables with a space derived from basis
  /**
     This only defines the space of the variable with the basis of the given 
     parameter but clears all the variables.
     \param var The mask of variables to use
  */
  robVariable( robSpace::Basis basis );
  
  //! A variable from the time space
  /**
     Create a variables drawn from the time space. This sets TIME basis in the 
     space of the variable.
     \param t The value of the time variable
  */
  robVariable( double t );
  
  robVariable( robSpace::Basis basis, double x, double xd, double xdd );

  robVariable( robSpace::Basis basis,
	       const vctMatrixRotation3<double>& R,
	       const vctFixedSizeVector<double,3>& w,
	       const vctFixedSizeVector<double,3>& wd );

  robVariable( robSpace::Basis basis, 
	       const vctDynamicVector<double>& q,
	       const vctDynamicVector<double>& qd,
	       const vctDynamicVector<double>& qdd );

  void IncludeBasis( robSpace::Basis basis, double y, double yd, double ydd );

  void IncludeBasis( robSpace::Basis basis,
		     const vctMatrixRotation3<double>& R,
		     const vctFixedSizeVector<double,3>& w,
		     const vctFixedSizeVector<double,3>& wd );

};  

#endif
