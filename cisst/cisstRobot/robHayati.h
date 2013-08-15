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

#ifndef _robHayati_h
#define _robHayati_h

#include <iostream>

#include <cisstRobot/robKinematics.h>
#include <cisstRobot/robExport.h>

//! Hayati parameters of a coordinate frame
/**
   Hayati is a kinematics convention especially suitable for parallel 
   consecutive axes [HayatiJRS85]
*/

class CISST_EXPORT robHayati : public robKinematics {
  
 private:
  
  //! Hayati parameters
  double alpha;         // rotation x
  double  beta;         // rotation y
  double theta;         // rotation z
  double     d;         // translation x or z (depends on joint type)

 protected:
  
  //! Read the parameters from an input stream
  /**
     Read the parameters from an input stream. The parameters are in the
     following order: convention, \f$\alpha\f$, \f$\beta\f$, \f$\theta\f$, d, 
     <joint>
     \param is The input stream
  */
  void ReadParameters( std::istream& is );
#if CISST_HAS_JSON
  void ReadParameters(const Json::Value &config);
#endif
  
  //! Write the parameters to an output stream
  /**
     Write the parameters to an output stream. The parameters are in the
     following order: \f$alpha\f$, \f$\beta\f$, \f$\theta\f$, d
     \param os The output stream
  */
  void WriteParameters( std::ostream& os ) const; 

 public:
  
  //! Default constructor
  robHayati();

  //! Overloaded constructor
  /**
     \param alpha
     \param beta
     \param theta
     \param d
     \param joint
  */
  robHayati( double alpha, 
	     double beta, 
	     double theta, 
	     double d,
	     const robJoint& joint );

  //! Default destructor
  ~robHayati();
  
  //! Get the position and orientation of the coordinate frame
  /**
     Returns the position and orientation of the coordinate frame.
     \param joint The joint value
     \return The position and orientation of the coordinate frame
  */
  vctFrame4x4<double> ForwardKinematics( double q ) const;

  //! Get the orientation of the link
  /**
     Returns the orientation of the link with respect to the proximal link for 
     a given joint vale.
     \param joint The joint associated with the link
     \return The orientation associated with the Hayati parameters
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
  double GetRotationY()    const { return beta;  }
  double GetRotationZ()    const { return theta; }
  double GetTranslationX() const { return d; }
  double GetTranslationZ() const { return d; }
  
  void SetRotationX( double x )    { alpha = x; }
  void SetRotationY( double x )    { beta = x;  }
  void SetRotationZ( double x )    { theta = x; }
  void SetTranslationX( double x ) { d = x; }
  void SetTranslationZ( double x ) { d = x; }
  
};

#endif
