/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-    */
/* ex: set filetype=cpp softtabstop=2 shiftwidth=2 tabstop=2 cindent expandtab: */

/*
  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robManipulator_h
#define _robManipulator_h

#include <string>
#include <vector>

#include <cisstVector/vctTransformationTypes.h>
#include <cisstRobot/robLink.h>

#if CISST_HAS_JSON
#include <json/json.h>
#endif

#include <cisstRobot/robExport.h>

class CISST_EXPORT robManipulator{

 protected:

  //! A vector of tools
  typedef std::vector<robManipulator*> ToolsType;
  ToolsType tools;

 public:

  enum Errno{ ESUCCESS, EFAILURE };

  std::string mLastError;

  //! Position and orientation of the first link
  /**
     Simply put, this is the position and orientation of the base of the first
     link with respect to a known world frame
  */
  vctFrame4x4<double> Rtw0;

  //! Body Jacobian
  /**
     The (geometric) body Jacobian in column major order
  */
  double** Jn;

  //! Spatial Jacobian
  /**
     The (geometric) spatial Jacobian in column major order
  */
  double** Js;

  //! A vector of links
  std::vector<robLink> links;


  //! Load the kinematics and the dynamics of the robot
  virtual robManipulator::Errno LoadRobot( const std::string& linkfile );

#if CISST_HAS_JSON
  //! Load the kinematics and the dynamtics of the robot from a JSON file
  virtual robManipulator::Errno LoadRobot(const Json::Value & config);
#endif

  robManipulator::Errno LoadRobot(std::vector<robKinematics *> KinParms);

  //! Evaluate the body Jacobian
  /**
     Evaluates the geometric body Jacobian. This implements the algorithm of
     Paul, Shimano, Mayer (SMC81)
  */
  void JacobianBody( const vctDynamicVector<double>& q ) const;

  //! Evaluate the body Jacobian and return it in the dynamic matrix J
  //  Returns true if successful; false otherwise (e.g., J is wrong size)
  bool JacobianBody(const vctDynamicVector<double>& q,
                    vctDynamicMatrix<double>& J) const;


  //! Evaluate the spatial Jacobian
  /**
     Evaluate the geometric spatial Jacobian.
     \warning To evaluate the spatial Jacobian you must first evaluate the
     body Jacobian
  */
  void JacobianSpatial( const vctDynamicVector<double>& q ) const;

  //! Evaluate the spatial Jacobian and return it in the dynamic matrix J
  //  Returns true if successful; false otherwise (e.g., J is wrong size)
  bool JacobianSpatial(const vctDynamicVector<double>& q,
                       vctDynamicMatrix<double>& J) const;

  //! Recursive Newton-Euler altorithm
  /**
     Evaluate the inverse dynamics through RNE. The joint positions,
     velocities and accelerations must be set before calling this method. It
     returns a vector of forces/torques that realize the desired state.
     \param q The joint positions
     \param qd The joint velocities
     \param qdd The joint accelerations
     \param fext An external force/moment acting on the tool control point
     \param g The gravity acceleration
  */
  vctDynamicVector<double>
  RNE( const vctDynamicVector<double>& q,
       const vctDynamicVector<double>& qd,
       const vctDynamicVector<double>& qdd,
       const vctFixedSizeVector<double,6>& f,//=vctFixedSizeVector<double,6>(0.0),
       double g = 9.81) const;

  vctDynamicVector<double>
  RNE_MDH( const vctDynamicVector<double>& q,
           const vctDynamicVector<double>& qd,
           const vctDynamicVector<double>& qdd,
           const vctFixedSizeVector<double,6>& f,//=vctFixedSizeVector<double,6>(0.0),
           double g = 9.81) const;

  //! Coriolis/centrifugal and gravity
  /**
     Evaluate the coriolis/centrifugal and gravitational forces acting on the
     manipulator. The joint positions, velocities and accelerations must be
     set before calling this method. It returns a vector of forces/torques
     that realize the given positions and accelerations. This method is akin
     to calling RNE without the joint accelerations
  */
  vctDynamicVector<double>
  CCG( const vctDynamicVector<double>& q,
       const vctDynamicVector<double>& qd,
       double g = 9.81 ) const;

  vctDynamicVector<double>
  CCG_MDH( const vctDynamicVector<double>& q,
           const vctDynamicVector<double>& qd,
           double g = 9.81 ) const;

  //! End-effector accelerations
  /**
     Compute the linear and angular accelerations of the last link. This is
     akin to computing the forward recursion of the RNE.
  */
  /*
  vctFixedSizeVector<double,6>
  Acceleration( const vctDynamicVector<double>& q,
		const vctDynamicVector<double>& qd,
		const vctDynamicVector<double>& qdd ) const;
  */
  //! Compute the bias acceleration
  /**
     The bias acceleration is the 6D vector Jdqd that is used to evaluate the
     inverse dynamics in operation space. This vector is derived from
     d (J qd) / dt = Jdqd + J qdd
  */
  vctFixedSizeVector<double,6>
  BiasAcceleration( const vctDynamicVector<double>& q,
                    const vctDynamicVector<double>& qd ) const;


  //! Compute the NxN manipulator inertia matrix
  /**
     \param[input] A A pointer to an NxN matrix
     \param[output] A The NxN manipulator inertia matrix
  */
  void JSinertia(double** A, const vctDynamicVector<double>& q ) const;

  vctDynamicMatrix<double> JSinertia( const vctDynamicVector<double>& q ) const;


  //! Compute the 6x6 manipulator inertia matrix in operation space
  /**
     \param[input] A A pointer to a 6x6 matrix
     \param[output] The 6x6 manipulator inertia matrix in operation space
  */
  void OSinertia(double Ac[6][6], const vctDynamicVector<double>& q) const;

  vctFixedSizeMatrix<double,4,4>
    SE3Difference( const vctFrame4x4<double>& Rt1,
                   const vctFrame4x4<double>& Rt2 ) const;

  void
    AddIdentificationColumn( vctDynamicMatrix<double>& J,
                             vctFixedSizeMatrix<double,4,4>& delRt ) const;

public:

  enum LinkID{ L0, L1, L2, L3, L4, L5, L6, L7, L8, L9, LN };

  robManipulator( const vctFrame4x4<double>& Rtw0 = vctFrame4x4<double>() );

  //! Manipulator generic constructor
  /**
     This constructor initializes a manipulator with the kinematics and dynamics
     contained in a file.
     \param robotfilename The file with the kinematics and dynamics parameters
     \param Rtw0 The offset transformation of the robot base
  */
  robManipulator( const std::string& robotfilename,
                  const vctFrame4x4<double>& Rtw0 = vctFrame4x4<double>() );

  robManipulator( const std::vector<robKinematics *> linkParms,
                  const vctFrame4x4<double>& Rtw0 = vctFrame4x4<double>() );

  //! Manipulator destructor
  virtual ~robManipulator();

  /*! Set joint limits */
  virtual void
    SetJointLimits(const vctDynamicVector<double> & lowerLimits,
                   const vctDynamicVector<double> & upperLimits);

  /*! Get joint limits */
  virtual void
    GetJointLimits(vctDynamicVectorRef<double> lowerLimits,
                   vctDynamicVectorRef<double> upperLimits) const;

  /*! Get force/torque max */
  virtual void
    GetFTMaximums(vctDynamicVectorRef<double> ftMaximums) const;

  /*! Get joint names */
  virtual void
    GetJointNames(std::vector<std::string> & names) const;

  /*! Get joint types */
  virtual void
    GetJointTypes(std::vector<robJoint::Type> & types) const;

  //! Evaluate the forward kinematics
  /**
     Compute the position and orientation of each link wrt to the world frame
     \param[input] q The vector of joint positions
     \param[input] N The link number (0 => base, negative => end-effector)
     \return The position and orientation, as a 4x4 frame
  */
  virtual
    vctFrame4x4<double>
    ForwardKinematics( const vctDynamicVector<double>& q, int N = -1 ) const;

  //! Evaluate the inverse kinematics
  /**
     Compute the inverse kinematics. The solution is computed numerically using
     Newton's algorithm.
     \param[input] q An initial guess of the solution
     \param[output] q The inverse kinematics solution
     \param Rts The desired position and orientation of the tool control point
     \param tolerance The error tolerance of the solution
     \param Niteration The maximum number of iterations allowed to find a solution
     \return SUCCESS if a solution was found within the given tolerance and
                     number of iterations. ERROR otherwise.
  */
  virtual
    robManipulator::Errno
    InverseKinematics( vctDynamicVector<double>& q,
                       const vctFrame4x4<double>& Rts,
                       double tolerance=1e-12,
                       size_t Niteration=1000,
                       double LAMBDA=0.001 );


  virtual
    robManipulator::Errno
    InverseKinematics( vctDynamicVector<double>& q,
                       const vctFrm3& Rts,
                       double tolerance=1e-12,
                       size_t Niteration=1000 );

  //! Normalize angles to -pi to pi
  virtual void NormalizeAngles( vctDynamicVector<double>& q );

  //! Inverse dynamics in joint space
  /**
     Compute and return the inverse dynamics of the manipulator in joint space.
     InverseDynamics returns the joint torques that correspond to a manipulator
     with the given joint positions, velocities and accelerations.
     \param q A vector of joint positions
     \param qd A vector of joint velocities
     \param qdd A vector of joint accelerations
     \return A vector of joint torques
  */
  virtual
    vctDynamicVector<double>
    InverseDynamics( const vctDynamicVector<double>& q,
                     const vctDynamicVector<double>& qd,
                     const vctDynamicVector<double>& qdd ) const;

  //! Inverse dynamics in operation space
  /**
     Compute and return the inverse dynamics of the manipulator in operation
     space. InverseDynamics returns the joint torques that correspond to a
     manipulator with the given joint positions, velocities and
     the tool control point (TCP) accelerations. The reason why joint positions
     and velocities are given instead of the position and velocity of the TCP is
     that the coriolis, centrifugal and gravitational forces are uniquely
     determined by the joint positions and velocities.
     \param q A vector of joint positions
     \param qd A vector of joint velocities
     \param vdwd A 6D vector of the TCP linear and angular accelerations
     \return A vector of joint torques
  */
  virtual
    vctDynamicVector<double>
    InverseDynamics( const vctDynamicVector<double>& q,
                     const vctDynamicVector<double>& qd,
                     const vctFixedSizeVector<double,6>& vdwd ) const;


  //! Compute Jacobian for kinematics identification
  /**
     Computes the Jacobian for kinematics identification by numerically
     differentiating with respect to the kinematics parameters (DH parameters).
     \param q The vector of joint positions
     \param epsilon The DH parameter difference to use for numerical differentiation
     \return The Jacobian matrix to use for kinematics identification
  */
  virtual
    vctDynamicMatrix<double>
    JacobianKinematicsIdentification( const vctDynamicVector<double>& q,
                                      double epsilon = 1e-6 ) const;

  //! Print the kinematics parameters to the specified output stream
  virtual void PrintKinematics( std::ostream& os ) const;

  //! Attach a tool
  virtual void Attach( robManipulator* tool );

  void DeleteTools(void);

  //! Remove all links expect n first ones
  /**
      This method also resizes internal data members as needed
      (jacobian matrices).  Returns EFAILURE if the current
      manipulator doesn't have at least n links.
  */
  virtual
    robManipulator::Errno
    Truncate(const size_t linksToKeep);

  /*! Get last error message */
  inline const std::string & LastError(void) const {
    return mLastError;
  }

  /*! Clamp joint value between joint limits and update the last error
    message if the value provided is outside joint limits.  Return
    true if clamping was necessary. */
  bool ClampJointValueAndUpdateError(const size_t jointIndex,
                                     double & value,
                                     const double & tolerance = 1e-6);
};

#endif // _robManipulator_h
