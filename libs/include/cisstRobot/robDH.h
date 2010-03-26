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

#ifndef _robDH_h
#define _robDH_h

#include <iostream>

#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctMatrixRotation3.h>
#include <cisstVector/vctFrame4x4.h>

#include <cisstRobot/robJoint.h>
#include <cisstRobot/robExport.h>

//! DH parameters of a link
/**
   The DH class is used for kinematics parameters. It is derived from a joint
   class to provide joint types and joint limits.
*/
class CISST_EXPORT robDH : public robJoint {

 public:
  //! The DH convention used by the link
  /**
     For a kinematic chain, the standard DH defines the coordinate frame of a 
     link at its distal joint whereas the modified DH defines the coordinate 
     frame atits proximal joint. Modified DH have the advantage of being able to 
     represent open-loop, closed-loop and tree structure robots (Khalil ICRA'86).
  */
  enum Convention {  
    STANDARD,
    MODIFIED
  };

  enum Errno{ ESUCCESS, EFAILURE };

private:

  //! Determine if the link uses DH or modified DH convention
  robDH::Convention convention; // modified DH?
  
  //! DH parameters
  double alpha, a;            // x components
  double theta, d;            // z components
  
public:
  
  //! Default constructor
  robDH();
  
  //! Default destructor
  ~robDH();
  
  //! Return the DH convention
  /**
     \return The DH convention: robDHStandard or robDHModified
  */
  robDH::Convention GetConvention() const;
  
  //! Return the position of the next (distal) link coordinate frame
  /**
     This method returns the \$XYZ\$ coordinates of the origin of the distal 
     link in the coordinate frame of the proximal link. "PStar" is not a good 
     name for this but the literature uses \$ \mathbf{p}^* \$ to denote this 
     value.
     \return The position of the next coordinate frame wrt to the current
              frame
  */
  vctFixedSizeVector<double,3> PStar() const;

  //! Get the position and orientation of the link
  /**
     Returns the position and orientation of the link with respect to the
     proximal link for a given joint vale.
     \param joint The joint associated with the link
     \return The position and orientation associated with the DH parameters
  */
  vctFrame4x4<double> ForwardKinematics( double ) const;

  //! Get the orientation of the link
  /**
     Returns the orientation of the link with respect to the proximal link for 
     a given joint vale.
     \param joint The joint associated with the link
     \return The orientation associated with the DH parameters
  */
  vctMatrixRotation3<double> Orientation( double ) const;
  
  //! Read the parameters from an input stream
  /**
     Read the parameters from an input stream. The parameters are in the
     following order: convention, \f$\alpha\f$, a, \f$\theta\f$, d, <joint>
     \param is The input stream
     \param dh The parameters
     \return SUCCESS if no error occurred. ERROR otherwise.
  */
  robDH::Errno ReadDH( std::istream& is );
  
  //! Write the parameters to an output stream
  /**
     Write the parameters to an output stream. The parameters are in the
     following order: alpha, a, theta, d
     \param os The output stream
     \param dh The parameters
     \return SUCCESS if no error occurred. ERROR otherwise.
  */
  robDH::Errno WriteDH( std::ostream& os ) const; 

};

#endif
