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

#ifndef _robManipulator_h
#define _robManipulator_h

#include <string>

#include <cisstRobot/robLink.h>
#include <cisstRobot/robJoint.h>
#include <cisstRobot/robActuator.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robManipulator{
  
protected:
  
public:
  
  //! Position and orientation of the first link
  /**
     Simply put, this is the position and orientation of the base of the first
     link with respect to a known world frame
  */
  vctFrame4x4<double,VCT_ROW_MAJOR> Rtw0;
  
  //! Body Jacobian
  /**
     The (geometric) body Jacobian in column major
  */
  double** Jn;
  
  //! Spatial Jacobian
  /**
     The (geometric) spatial Jacobian in column major
  */
  double** Js;
  
  //! A tool
  robLink *tool;
  
  //! A vector of links
  std::vector<robLink> links;
  //! A vector of joints
  std::vector<robJoint> joints;
  //! A vector of actuators
  std::vector<robActuator*> actuators;
  
  //! Load the kinematics and the dynamics of the robot
  robError LoadRobot( const std::string& linkfile );
  
  //! Load the tool of the robot
  robError LoadTool( const std::string& toolfile );

  //! Evaluate the body Jacobian
  /**
     Evaluates the geometric body Jacobian. This implements the algorithm of 
     Paul, Shimano, Mayer (SMC81)
  */
  void JacobianBody( const vctDynamicVector<double>& q ) const;
  
  //! Evaluate the spatial Jacobian
  /**
     Evaluate the geometric spatial Jacobian.
     \warning To evaluate the spatial Jacobian you must first evaluate the
     body Jacobia
  */
  void JacobianSpatial( const vctDynamicVector<double>& q ) const;
  
  //! Recursive Newton-Euler altorithm
  /**
     Evaluate the inverse dynamics through RNE. The joints positions, 
     velocities and accelerations must be set before calling this method. It
     returns a vector of forces/torques that realize the desired state.
     \param q The joints positions
     \param qd The joints velocities
     \param qdd The joints accelerations
     \param fext An external force/moment acting on the tool control point
     \param g The gravity acceleration
  */
  vctDynamicVector<double> 
  RNE( const vctDynamicVector<double>& q,
       const vctDynamicVector<double>& qd,
       const vctDynamicVector<double>& qdd,
       const vctFixedSizeVector<double,6>& f=vctFixedSizeVector<double,6>(0.0),
       double g = 9.81 ) const;
  
  //! Coriolis/centrifugal and gravity
  /**
     Evaluate the coriolis/centrifugal and gravitational forces acting on the
     manipulator. The joints positions, velocities and accelerations must be
     set before calling this method. It returns a vector of forces/torques 
     that realize the given positions and accelerations. This method is akin
     to calling RNE without the joints accelerations
  */
  vctDynamicVector<double> 
  CCG( const vctDynamicVector<double>& q,
       const vctDynamicVector<double>& qd ) const;

  //! End-effector accelerations
  /**
     Compute the linear and angular accelerations of the last link. This is 
     akin to compute the forward recursion of the RNE.
  */
  vctFixedSizeVector<double,6> 
  Acceleration( const vctDynamicVector<double>& q,
		const vctDynamicVector<double>& qd,
		const vctDynamicVector<double>& qdd ) const ;
  
  //! Compute the bias acceleration
  /**
     The bias acceleration is the 6D vector Jdqd that is used to evaluate the
     inverse dynamics in operations space. This vector is derived from
     d (J qd) / dt = Jdqd + J qdd
  */
  vctFixedSizeVector<double,6> 
  BiasAcceleration( const vctDynamicVector<double>& q,
		    const vctDynamicVector<double>& qd ) const;

  
  //! Compute the NxN manipulator inertia matrix
  /**
     \param[input] A A pointer to an NxN matrix
     \param[output] The NxN manipulator inertia matrix
  */
  void JSinertia(double** A,
		 const vctDynamicVector<double>& q ) const;
		 
  
  //! Compute the 6x6 manipulator inertia matrix in operation space
  /**
     \param[input] A A pointer to an 6x6 matrix
     \param[output] The 6x6 manipulator inertia matrix in operation space
  */
  void OSinertia(double Ac[6][6],
		 const vctDynamicVector<double>& q) const;
  
  //! Actuator positions to joint positions
  /**
     This is the default behavior for converting actuator positions to 
     motor positions. This method simply copies the values from the actuators
     to the joints. You must implement your own behavior (gear ratios, 
     differential drives) if you need anything more specific.
     \param dt The time increment to compute joint velocities and accelerations
  */
  virtual
  vctDynamicVector<double> 
  ActuatorsPos2JointsPos( const vctDynamicVector<double>& q ) const;
  
  //! Actuator forces/torques to joint forces/torques
  /**
     This is the default behavior for converting actuator forces/torques to 
     motor forces/torques. This method simply copies the values from the 
     actuators to the joints. You must implement your own behavior (gear 
     ratios, differential drives) if you need anything more specific.
  */
  virtual 
  vctDynamicVector<double>
  ActuatorsFT2JointsFT( const vctDynamicVector<double>& ft ) const;
  
  //! Joint positions to actuator positions
  /**
     This is the default behavior for converting joint positions to 
     actuator positions. This method simply copies the values from the 
     joints to the actuators. You must implement your own behavior (gear 
     ratios, differential drives) if you need anything more specific.
  */
  virtual
  vctDynamicVector<double>
  JointsPos2ActuatorsPos( const vctDynamicVector<double>& q ) const;
  
  //! Joint forces/torques to actuators forces/torques
  /**
     This is the default behavior for converting joint forces/torques to 
     actuators forces/torques. This method simply copies the values from the 
     joints to the actuators. You must implement your own behavior (gear 
     ratios, differential drives) if you need anything more specific.
  */
  virtual 
  vctDynamicVector<double> 
  JointsFT2ActuatorsFT( const vctDynamicVector<double>& ft ) const;
  
public:
  
  //! Manipulator generic constructor
  /**
     This constructor initializes a manipulator with the kinematics and dynamics
     contained in a file.
     \param robotfilename The file with the kinematics and dynamics parameters
     \param toolfilename The file with the kinmatics and dynamics of a tool
     \param Rtw0 The offset transformation of the robot base
  */
  robManipulator( const std::string& robotfilename,
		  const std::string& toolfilename = std::string(),
		  const vctFrame4x4<double,VCT_ROW_MAJOR>& Rtw0 
		  = vctFrame4x4<double,VCT_ROW_MAJOR>() );
  
  //! Evaluate the forward kinematics
  /**
     Compute the position and orientations of each link wrt to the world frame
     This method is non-const since it needs to update the position and 
     orientation of each link in order to render them in OpenGL
  */
  vctFrame4x4<double,VCT_ROW_MAJOR> 
  ForwardKinematics( const vctDynamicVector<double>& q );
  
  vctFrame4x4<double,VCT_ROW_MAJOR> 
  ForwardKinematics( const vctDynamicVector<double>& q ) const;

  //! Evaluate the inverse kinematics
  /**
     Compute the inverse kinematics. The solution is computed with from 
     Newton's algorithm.
     \param[input] q An initial guess of the solution
     \param[output] q The inverse kinematics solution
     \param Rts The desired position and orientation of the tool control point
     \param tolerance The error tolerance of the solution
     \param Niteration The maximum number of iteration allowed to find a solution
     \return SUCCESS if a solution was found within the given tolerance and 
                     number of iterations. ERROR otherwise.
  */
  robError InverseKinematics( vctDynamicVector<double>& q, 
			      const vctFrame4x4<double,VCT_ROW_MAJOR>& Rts, 
			      double tolerance=1e-12, 
			      size_t Niteration=1000 );
  
  //! Inverse dynamics in joint space
  /**
     Compute and return the inverse dynamics of the manipulator in joint space.
     InverseDynamics returns the joint torques that corresponds to a manipulator
     with the given the joints positions, velocities and accelerations.
     \param q A vector of joints positions
     \param qd A vector of joints velocities
     \param qdd A vector of joints accelerations
     \return A vector of joints torques
  */
  vctDynamicVector<double> 
  InverseDynamics( const vctDynamicVector<double>& q,
		   const vctDynamicVector<double>& qd,
		   const vctDynamicVector<double>& qdd ) const;
  
  //! Inverse dynamics in operation space
  /**
     Compute and return the inverse dynamics of the manipulator in operation
     space. InverseDynamics returns the joint torques that corresponds to a 
     manipulator with the given the joints positions, velocities and 
     the tool control point (TCP) accelerations. The reason why joints positions 
     and velocities are given instead of the position and velocity of the TCP is
     that the coriolis, centrifugal and gravitational forces are uniquely 
     determined by the joints positions and velocties.
     \param q A vector of joints positions
     \param qd A vector of joints velocities
     \param vdwd A 6D vector of the TCP linear and angular accelerations
     \return A vector of joints torques
  */
  vctDynamicVector<double> 
  InverseDynamics( const vctDynamicVector<double>& q,
		   const vctDynamicVector<double>& qd,
		   const vctFixedSizeVector<double,6>& vdwd ) const;
  
  void Print() const ;
  
};

#endif
