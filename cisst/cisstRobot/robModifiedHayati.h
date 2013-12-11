/*

  Author(s): Simon Leonard, Min Yang Jung
  Created on: April 29 2013

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robModifiedHayati_h
#define _robModifiedHayati_h

#include <iostream>

#include <cisstRobot/robKinematics.h>
#include <cisstRobot/robExport.h>

//! Hayati parameters of a coordinate frame
/**
   This class implements the Hayati kinematic model which is suitable for modelling
   both nearly parallel and nearly orthogonal axes.
   Compared to robHayati, the convention of this model follows [siciliano2008springer]
   (Handbook of Robotics, Springer, 2008)
*/

class CISST_EXPORT robModifiedHayati : public robKinematics {
  
 private:
  
  //! Hayati parameters
  double  beta;         // rotation y
  double alpha;         // rotation x
  double     a;         // translation x
  double     d;         // translation z
  double theta;         // rotation z

 protected:
  
  //! Read the parameters from an input stream
  /**
     Read the parameters from an input stream. The parameters are in the
     following order: convention, \f$\beta\f$, \f$\alpha\f$, a, d, \f$\theta\f$
     \param is The input stream
  */
  void ReadParameters( std::istream& is );
#if CISST_HAS_JSON
  void ReadParameters(const Json::Value &config);
#endif
  
  //! Write the parameters to an output stream
  /**
     Write the parameters to an output stream. The parameters are in the
     following order: \f$\beta\f$, \f$\alpha\f$, a, d, \f$\theta\f$
     \param os The output stream
  */
  void WriteParameters( std::ostream& os ) const; 

 public:
  
  //! Default constructor
  robModifiedHayati(void);

  //! Overloaded constructor
  /**
     \param beta
     \param alpha
     \param a
     \param d
     \param theta
     \param joint
  */
  robModifiedHayati(double beta, 
                    double alpha, 
                    double a, 
                    double d,
                    double theta,
                    const robJoint& joint);

  //! Default destructor
  ~robModifiedHayati();
  
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
  vctFixedSizeVector<double,3> PStar(void) const;

  //! Clone
  robKinematics* Clone(void) const;

  double GetRotationX(void)    const { return alpha; }
  double GetRotationY(void)    const { return beta;  }
  double GetRotationZ(void)    const { return theta; }
  double GetTranslationX(void) const { return a; }
  double GetTranslationZ(void) const { return d; }
  
  void SetRotationX( double x )    { alpha = x; }
  void SetRotationY( double x )    { beta = x;  }
  void SetRotationZ( double x )    { theta = x; }
  void SetTranslationX( double x ) { a = x; }
  void SetTranslationZ( double x ) { d = x; }
  
  void PrintParams(void) const;
};

#endif
