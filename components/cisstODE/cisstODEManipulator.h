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

#ifndef _cisstODEManipulator_h
#define _cisstODEManipulator_h

#include <vector>

#include <cisstOSG/cisstOSGManipulator.h>

#include <cisstODE/cisstODEBody.h>
#include <cisstODE/cisstODEJoint.h>
#include <cisstODE/cisstODEServoMotor.h>
#include <cisstODE/cisstODEWorld.h>

#include <cisstODE/cisstODEExport.h>

class CISST_EXPORT cisstODEManipulator : public cisstOSGManipulator {

 private:

  //! The world ID
  cisstODEWorld* odeworld;

  //! vector of bodies
  std::vector< osg::ref_ptr<cisstODEBody> >  odebodies;

  //! vector of joints
  std::vector<cisstODEJoint*> odejoints;

  //! vector of servo motors
  std::vector<cisstODEServoMotor*> odeservos;

  //! initial configuration
  vctDynamicVector<double> qinit;

  void CreateManipulator( const std::vector<std::string>& models,
			  const std::string& basemodel );

  void CreateManipulator( const std::vector<std::string>& models );

 protected:

  //! Return the world ID
  cisstODEWorld* GetWorld() const{ return odeworld; }
  dWorldID GetWorldID() const { return odeworld->GetWorldID(); }

  //! Insert a body in the manipulator
  void Insert( cisstODEBody* body );

  //! Insert a joint in the manipulator
  void Insert( cisstODEJoint* joint );

  //! Insert a joint in the manipulator
  void Insert( cisstODEServoMotor* servo );

 public:
  /*
  cisstODEManipulator( const std::string& devname, 
		       double period,
		       devManipulator::State state,
		       osaCPUMask mask,
		       cisstODEWorld* world,
		       devManipulator::Mode mode );
  */
  //! ODE Manipulator constructor
  /**
     This constructor initializes an ODE manipulator with the kinematics and 
     dynamics contained in a file. Plus it initializes the ODE elements of the
     manipulators (bodies and joints) for the engine.
     \param models A vector of CAD models file name (one file per link)
     \param world The ODE world used by the manipulator
     \param Rtw0 The offset transformation of the robot base
     \param robotfilename The file with the kinematics and dynamics parameters
     \param basemodel The filename of the CAD file for the base
     \param qinit The initial joint angles
  */
  cisstODEManipulator( const std::vector<std::string>& models,
		       cisstODEWorld* world,
		       const vctFrame4x4<double>& Rtw0,
		       const std::string& robotfilename,
		       const std::string& basemodel,
		       const vctDynamicVector<double>& qinit );

  //! ODE Manipulator constructor
  /**
     This constructor initializes an ODE manipulator with the kinematics and 
     dynamics contained in a file. Plus it initializes the ODE elements of the
     manipulators (bodies and joints) for the engine.
     \param models A vector of CAD models file name (one file per link)
     \param world The ODE world used by the manipulator
     \param Rtw0 The offset transformation of the robot base
     \param robotfilename The file with the kinematics and dynamics parameters
     \param basemodel The filename of the CAD file for the base
     \param qinit The initial joint angles
  */
  cisstODEManipulator( const std::vector<std::string>& models,
		       cisstODEWorld* world,
		       const vctFrm3& Rtw0,
		       const std::string& robotfilename,
		       const std::string& basemodel,
		       const vctDynamicVector<double>& qinit );

  //! ODE Manipulator constructor
  /**
     This constructor initializes an ODE manipulator with the kinematics and 
     dynamics contained in a file. Plus it initializes the ODE elements of the
     manipulators (bodies and joints) for the engine.
     \param models A vector of CAD models file name (one file per link)
     \param world The ODE world used by the manipulator
     \param Rtw0 The offset transformation of the robot base
     \param robotfilename The file with the kinematics and dynamics parameters
     \param base A ODE body used for the base
     \param qinit The initial joint angles
  */
  cisstODEManipulator( const std::vector<std::string>& models,
		       cisstODEWorld* world,
		       const vctFrame4x4<double>& Rtw0,
		       const std::string& robotfilename,
		       cisstODEBody* base,
		       const vctDynamicVector<double>& qinit );
  //! ODE Manipulator constructor
  /**
     This constructor initializes an ODE manipulator with the kinematics and 
     dynamics contained in a file. Plus it initializes the ODE elements of the
     manipulators (bodies and joints) for the engine.
     \param models A vector of CAD models file name (one file per link)
     \param world The ODE world used by the manipulator
     \param Rtw0 The offset transformation of the robot base
     \param robotfilename The file with the kinematics and dynamics parameters
     \param base A ODE body used for the base
     \param qinit The initial joint angles
  */
  cisstODEManipulator( const std::vector<std::string>& models,
		       cisstODEWorld* world,
		       const vctFrm3& Rtw0,
		       const std::string& robotfilename,
		       cisstODEBody* base,
		       const vctDynamicVector<double>& qinit );

  ~cisstODEManipulator(){}

  //! Return the joints positions
  /**
     Query each ODE joint and return the joint positions
     \param q A vector of joint positions
     \return ESUCCESS if not error. EFAILURE otherwise.
  */
  virtual 
    cisstODEManipulator::Errno 
    GetPositions( vctDynamicVector<double>& q ) const ;

  //! Return the joints velocities
  /**
     Query each ODE joint and return the joint velocities
     \param qd A vector of joint velocities
     \return ESUCCESS if not error. EFAILURE otherwise.
  */
  virtual 
    cisstODEManipulator::Errno
    GetVelocities( vctDynamicVector<double>& qd ) const ;

  //! Set the joint position
  /**
     This sets the position command of ODE (internal) servo motors. This does 
     not instantly changes the position. The position values are used to set the
     velocity of the ODE servo motors.
     \param qs A vector of joint positions
     \return ESUCCESS if not error. EFAILURE otherwise.
  */
  virtual 
    cisstODEManipulator::Errno
    SetPositions( const vctDynamicVector<double>& qs );

  //! Set the joint velocity
  /**
     This sets the velocity command of ODE (internal) servo motors. This does 
     not instantly changes the velocity. The velocity values are used to set the
     velocity of the ODE servo motors.
     \param qsd A vector of joint velocities
     \return ESUCCESS if not error. EFAILURE otherwise.
  */
  virtual 
    cisstODEManipulator::Errno 
    SetVelocities( const vctDynamicVector<double>& qds );
  
  //! Set the joint forces or torques
  /**
     This sets the force/torque value of each joint. This method does NOT apply 
     the FT right away. The FT will be applied at the next iteration of the 
     world.
     \param ft A vector of joint forces/torques
  */
  virtual 
    cisstODEManipulator::Errno 
    SetForcesTorques( const vctDynamicVector<double>& ft);

#ifndef SWIG

  //! The ODE state of the manipulator
  typedef std::vector< cisstODEBody::State > State;

  //! Return the state of the robot
  /**
     For an ODE manipulator, the state of a manipulator is defined by the 
     position/orientation and the velocity of each link. It is not defined by 
     the position/velocity of each joint.
     This method queries each link of the robot for its state 
     (position/orientation + angular/linear velocities) and return them in a 
     vector.
     \return A vector containing the state of each link.
  */
  virtual cisstODEManipulator::State GetState( ) const;

  //! Set the state of the robot
  /**
     For an ODE manipulator, the state of a manipulator is defined by the 
     position/orientation and the velocity of each link. It is not defined by 
     the position/velocity of each joint.
     This method set the state of each link of the robot: 
     (position/orientation + angular/linear velocities) and return them in a 
     vector.
  */
  virtual void SetState( const cisstODEManipulator::State& state );

#endif

  //! Return the base ID of the manipulator.
  /**
     This returns the ODE body ID of the base of the robt. This is mostly used
     for attaching the robot to anothher body.
  */
  dBodyID GetBaseID() const;

  virtual
    vctFrame4x4<double> 
    ForwardKinematics( const vctDynamicVector<double>& q, int N=-1 ) const;

  void 
    ForwardKinematics( const vctDynamicVector<double>& q, 
		       vctFrm3& Rt, int N=-1 ) const;

  //! Attach a tool to the robot
  /**
     This attaches manipulator to the end-effector. This essentially create a 
     joint between the end-effector and the tool but that joint has 0 angles of
     rotation.
     \param tool A pointer to a robot tool
  */
  virtual void Attach( cisstOSGManipulator* osgtool );

  //! Disable all the bodies of the manipulator
  void Disable();

  //! Enable all the bodies of the manipulator
  void Enable();
  
};

#endif
