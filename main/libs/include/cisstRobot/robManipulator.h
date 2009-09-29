#ifndef _robManipulator_h
#define _robManipulator_h

#include <cisstRobot/robLink.h>
#include <cisstRobot/robJoint.h>
#include <cisstRobot/robActuator.h>

#include <string>

namespace cisstRobot{


  class robManipulator{

  protected:
    robMutex mutex;

    //! Position and orientation of the first link
    /**
       Simply put, this is the position and orientation of the base of the first
       link with respect to a known world frame
     */
    SE3 Rtw0;
    
    //! Body Jacobian
    /**
       The (geometric) body Jacobian in column major
     */
    Real** Jn;
    //! Spatial Jacobian
    /**
       The (geometric) spatial Jacobian in column major
    */
    Real** Js;

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

    //! Evaluate the forward kinematics
    /**
       Compute the position and orientations of each link wrt to the world frame
     */
    void ForwardKinematics();

    //! Evaluate the body Jacobian
    /**
       Evaluates the geometric body Jacobian. This implements the algorithm of 
       Paul, Shimano, Mayer (SMC81)
     */
    void JacobianBody() const;

    //! Evaluate the spatial Jacobian
    /**
       Evaluate the geometric spatial Jacobian.
       \warning To evaluate the spatial Jacobian you must first evaluate the
       body Jacobia
    */
    void JacobianSpatial() const;

    //! Gravity
    /**
       The gravitational acceleration. Leave this to its default value (9.81) 
       unless you know what you're doing
     */
    Real G;

    //! Gravity vector
    /**
       Represents the "Z" axis of the base relative to the direction of the 
       gravity. For most robots, this vector is (0,0,1) as the Z axis 
       typically points upwards. 
     */
    R3 z0;

    //! Recursive Newton-Euler altorithm
    /**
       Evaluate the inverse dynamics through RNE. The joints positions, 
       velocities and accelerations must be set before calling this method. It
       returns a vector of forces/torques that realize the desired state.
       \param fext An external force/moment acting on the last link
     */
    Rn RNE(const R6& fext=R6(0.0));

    //! Coriolis/centrifugal and gravity
    /**
       Evaluate the coriolis/centrifugal and gravitational forces acting on the
       manipulator. The joints positions, velocities and accelerations must be
       set before calling this method. It returns a vector of forces/torques 
       that realize the given positions and accelerations. This method is akin
       to calling RNE without the joints accelerations
     */
    Rn CCG();                        // coriolis+centrifugal

    //! End-effector accelerations
    /**
       Compute the linear and angular acceleration of the last link. This is akin
       to compute the forward recursion of the RNE.
    */
    R6 Acceleration();               // cartesian acceleration

    //! Used to compute the manipulator's inertia matrix
    Rn InertiaSubroutine( size_t idx );

    //! Compute the bias acceleration
    /**
       The bias acceleration is the 6D vector Jdqd that is used to evaluate the
       inverse dynamics in operations space. This vector is derived from
       d (J qd) / dt = Jdqd + J qdd
    */
    R6 BiasAcceleration();

    //! Compute the NxN manipulator inertia matrix
    /**
       \param[input] A A pointer to an NxN matrix
       \param[output] The NxN manipulator inertia matrix
    */
    void JSinertia(Real** A);        // Joint Space inertia matrix

    //! Compute the 6x6 manipulator inertia matrix in operation space
    /**
       \param[input] A A pointer to an 6x6 matrix
       \param[output] The 6x6 manipulator inertia matrix in operation space
    */
    void OSinertia(Real Ac[6][6]);   // Operation Space inertia matrix

    //! Inverse dynamics (joint space)
    /**
       Compute and return the inverse dynamics of the manipulator in joint space.
       This method is intended for control purpose, so the joints positions, 
       velocities must be set prior to calling this method. Then, given a desired
       joints accelerations qdd, InverseDynamics will return the joint torques
       that will realize the known positions, velocities and the desired
       accelerations. Essentially, this evaluates the 
     */
    Rn InverseDynamics( const Rn& qdd );

    //! Inverse dynamics (operation space)
    /**
       Compute and return the inverse dynamics of the manipulator in operation
       space. This method is intended for control purpose, so the joints 
       positions, velocities must be set prior to calling this method. Then, 
       given a desired linear and angular acceleration vdwd, InverseDynamics will
       return the joint torques that will realize the known positions, velocities
       and the desired accelerations. Essentially, this evaluates the 
    */
    Rn InverseDynamics( const R6& vdwd );

    //! Actuator positions to joint positions
    /**
       This is the default behavior for converting actuator positions to 
       motor positions. This method simply copies the values from the actuators
       to the joints. You must implement your own behavior (gear ratios, 
       differential drives) if you need anything more specific.
       \param dt The time increment to compute joint velocities and accelerations
    */
    virtual robError Apos2Jpos( Real dt );

    //! Actuator forces/torques to joint forces/torques
    /**
       This is the default behavior for converting actuator forces/torques to 
       motor forces/torques. This method simply copies the values from the 
       actuators to the joints. You must implement your own behavior (gear 
       ratios, differential drives) if you need anything more specific.
    */
    virtual robError AFT2JFT( );

    //! Joint positions to actuator positions
    /**
       This is the default behavior for converting joint positions to 
       actuator positions. This method simply copies the values from the 
       joints to the actuators. You must implement your own behavior (gear 
       ratios, differential drives) if you need anything more specific.
    */
    virtual robError Jpos2Apos( );

    //! Joint forces/torques to actuators forces/torques
    /**
       This is the default behavior for converting joint forces/torques to 
       actuators forces/torques. This method simply copies the values from the 
       joints to the actuators. You must implement your own behavior (gear 
       ratios, differential drives) if you need anything more specific.
    */
    virtual robError JFT2AFT( );

  public:

    robManipulator( const std::string& linkfile, 
		    const std::string& toolfile = std::string(),
		    const SE3& Rtw0 = SE3(),
		    Real G = 9.81,
		    const R3& z0 = R3(0.0, 0.0, 1.0) );


    //! Return the position and orientation of a link
    /**
       \param i The index of the link. If no index is specified, the last link
       is assumed
     */
    SE3 PositionOrientation( int i = -1 ) const ;

    //! Set the joint positions
    robError JointPosition( const Rn& q );
    //! Get the joint positions
    Rn       JointPosition( );

    //! Set the joint forces/torques
    robError JointForceTorque( const Rn& q );
    //! Get the joint forces/torques
    Rn       JointForceTorque( ) const;

    //! Set the actuators forces/torques
    robError ActuatorForceTorque( const Rn& q );
    //! Get the actuators forces/torques
    Rn       ActuatorForceTorque( ) const;

    //! Return a reference to the links (used by the robGUI)
    const std::vector<robLink>& Links() const ;

    robError InverseKinematics( Rn& q, 
				const SE3& Rts, 
				Real tol=1e-12, 
				size_t Niter=1000 );

    void Print() const ;

  };
}

#endif
