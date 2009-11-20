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

#include <cisstRobot/robDefines.h>
#include <cisstRobot/robExport.h>

//! DH parameters of a link
class CISST_EXPORT robDH{
private:
  
  bool modifiedDH;    // modified DH?
  double sigma;       // prismatic joint=1.0, revolute=0.0
  
  double alpha, a;    // x components
  double theta, d;    // z components
  
  double   offset;    // offset angle (revolute) or length (prismatic)

  //! Set the DH parameters
  /**
     \param alpha
     \param a
     \param theta
     \param d
     \param offset Offset value of the joint. This offset is added to the joint 
                   "0" value. 
     \param type "REVOLUTE" or "PRISMATIC"
     \param convention "MODIFIED" or "STANDARD"
  */
  void SetKinematicsParameters( double alpha, double a, 
				double theta, double d, 
				double offset,
				const std::string& type,
				const std::string& convention ); 

public:
  
  //! Default constructor
  robDH();

  //! Default destructor
  ~robDH();
  
  //! Is the link revolute
  /**
     \return true if the joint is revolute
  */
  bool IsRevolute()  const;

  //! Is the link prismatic
  /**
     \return true if the joint is prismatic
  */
  bool IsPrismatic() const;

  //! Modified DH convention
  /**
     \return true if "modified DH" convention is used
  */
  bool IsModifiedDH() const;
  
  //! Sigma (prismatic/revolute)
  /**
     \return 1.0 for a revolute joint. 0.0 for a prismatic joint.
  */
  double Sigma() const;
  
  //! Return the position of the next link
  /**
     This method returns the \$XYZ\$ coordinates of the origin of the distal link
     in the coordinate frame of the proximal link. "PStar" is not a good name
     for this but the literature uses \$ \mathbf{p}^* \$ to denote this value.
     \return The position of the next coordinate frame wrt to the current
             frame
  */
  vctFixedSizeVector<double,3> PStar() const;

  //! Get the position and orientation of the link
  /**
     Returns the position and orientation of the link with respect to the
     proximal link for a given joint vale.
     \param q The joint value.
     \return The position and orientation associated with the DH parameters
  */
  vctFrame4x4<double,VCT_ROW_MAJOR> ForwardKinematics( double q ) const;

  //! Get the orientation of the link
  /**
     Returns the orientation of the link with respect to the proximal link for 
     a given joint vale.
     \param q The joint value.
     \return The orientation associated with the DH parameters
  */
  vctMatrixRotation3<double,VCT_ROW_MAJOR> Orientation( double q ) const;

  //! Read the parameters from an input stream
  /**
     Read the parameters from an input stream. The parameters are in the
     following order: alpha, a, \f$\theta\f$-offset, d
     \param is The input stream
     \param dh The parameters
  */
  void Read(std::istream& is);
  
  //! Write the parameters to an output stream
  /**
     Write the parameters to an output stream. The parameters are in the
     following order: alpha, a, theta, d
     \param os The output stream
     \param dh The parameters
  */
  void Write( std::ostream& os ) const; 

};

#endif
