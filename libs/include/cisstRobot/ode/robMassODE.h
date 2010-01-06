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

#include <ode/ode.h>

#include <cisstRobot/robMassBase.h>
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
class CISST_EXPORT robMassODE : public robMassBase {

private:

  dMass massode;
  
public:

  //! Default constructor
  robMassODE();

  //! Default destructor
  ~robMassODE();

  //! Set the mass of the body
  void ConfigureMassODE( dBodyID bodyid );
  
};

#endif
