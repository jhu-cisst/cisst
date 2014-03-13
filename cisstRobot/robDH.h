/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
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

#include <cisstRobot/robKinematics.h>
#include <cisstRobot/robExport.h>

//! Standard DH parameters of a coordinate frame
/**
   The DH class is used for kinematics parameters.
*/
class CISST_EXPORT robDH : public robKinematics {
  
 private:
  
  //! DH parameters
  double alpha;
  double     a;            // x components
  double theta;
  double     d;            // z components

 protected:
  
  //! Read the parameters from an input stream
  /**
     Read the parameters from an input stream. The parameters are in the
     following order: convention, \f$\alpha\f$, a, \f$\theta\f$, d, <joint>
     \param is The input stream
  */
  void ReadParameters( std::istream& is );
#if CISST_HAS_JSON
  void ReadParameters(const Json::Value &config);
#endif
  
  //! Write the parameters to an output stream
  /**
     Write the parameters to an output stream. The parameters are in the
     following order: \f$\alpha\f$, a, \f$\theta\f$, d
     \param os The output stream
  */
  void WriteParameters( std::ostream& os ) const; 

 public:
  
  //! Default constructor
  robDH();

  //! Overloaded constructor
  /**
     \param alpha 
     \param a
     \param theta
     \param d
     \param joint
  */
  robDH( double alpha, 
	 double a, 
	 double theta, 
	 double d,
	 const robJoint& joint );

  //! Default destructor
  ~robDH();
  
  //! Get the position and orientation of the coordinate frame
  /**
     Returns the position and orientation of the coordinate frame.
     \param joint The joint value
     \return The position and orientation of the coordinate frame
  */
  vctFrame4x4<double> ForwardKinematics( double q ) const;

  //! Get the orientation of the link
  /**
     Returns the orientation of the coordinate frame.
     \param joint The joint associated with the link
     \return The orientation of the frame
  */
  vctMatrixRotation3<double> Orientation( double q ) const;
  
  //! Return the position of the next (distal) link coordinate frame
  /**
     This method returns the \$XYZ\$ coordinates of the origin of the distal 
     link in the coordinate frame of the proximal link. "PStar" is not a good 
     name for this but the literature uses \$ \mathbf{p}^* \$ to denote this 
     value.
     \return The position of the next coordinate frame wrt to the current frame
  */
  vctFixedSizeVector<double,3> PStar() const;

  //! Clone
  robKinematics* Clone() const;

  double GetRotationX()    const { return alpha; }
  double GetRotationZ()    const { return theta; }
  double GetTranslationX() const { return a; }
  double GetTranslationZ() const { return d; }
  
  void SetRotationX( double x )    { alpha = x; }
  void SetRotationZ( double x )    { theta = x; }
  void SetTranslationX( double x ) { a = x; }
  void SetTranslationZ( double x ) { d = x; }
  
};

#endif
