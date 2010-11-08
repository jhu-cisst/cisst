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

#ifndef _robLink_h
#define _robLink_h

#include <iostream>

#include <cisstRobot/robDH.h>
#include <cisstRobot/robMass.h>

#include <cisstRobot/robExport.h>

//! A robot link
/**
   robLink implements the methods necessary to model a robot link. The class is
   derived from robBody to store the dynamics parameters of the body and the 
   link's position and orientation. The link is also derived from robDH to
   determine the link's position and orientation from joint values
*/
class CISST_EXPORT robLink : public robDH, public robMass {

public:

  enum Errno { ESUCCESS, EFAILURE };
  
  //! Default constructor
  robLink();

  //!
  robLink( const robDH& dh, const robMass& mass );
  
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
     center of mass (3 double): \f$ \begin{matrix}x&y&z\end{matrix} \f$
     principal moment of inertia (3 double): \f$\begin{bmatrix}I_{xx}&I_{yy}&I_{zz}\end{matrix} \f$
     body principal axis (9 double): 
  */
  robLink::Errno ReadLink( std::istream& is );
  
  //! Write the DH and body parameters
  robLink::Errno WriteLink( std::ostream& os ) const;
  
};

#endif
