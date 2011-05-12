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

#ifndef _devODEManipulator_h
#define _devODEManipulator_h

#include <vector>


#include <cisstDevices/robotcomponents/osg/devOSGManipulator.h>

#include <cisstDevices/robotcomponents/ode/devODEBody.h>
#include <cisstDevices/robotcomponents/ode/devODEJoint.h>
#include <cisstDevices/robotcomponents/ode/devODEServoMotor.h>
#include <cisstDevices/robotcomponents/ode/devODEWorld.h>

#include <cisstDevices/devExport.h>

class CISST_EXPORT devODEManipulator : public devOSGManipulator {

 private:

  //! The world ID
  dWorldID worldid;

  //! vector of joints
  std::vector<devODEJoint*> joints;

  //! vector of bodies
  std::vector<devODEBody*>  bodies;

  //! vector of servo motors
  std::vector<devODEServoMotor*> servos;

  //! initial configuration
  vctDynamicVector<double> qinit;

  void CreateIO();
  void CreateManipulator( devODEWorld* world,
			  devManipulator::Mode mode,
			  const std::vector<std::string>& models,
			  const std::string& basemodel );

  void CreateManipulator( devODEWorld* world,
			  devManipulator::Mode mode,
			  const std::vector<std::string>& models,
			  devODEBody* base = NULL );

 protected:

  //! Return the world ID
  dWorldID WorldID() const { return worldid; }

  //! Pointer to the body of the base
  devODEBody* base;

  //! Insert a body in the manipulator
  void Insert( devODEBody* body );

  //! Insert a joint in the manipulator
  void Insert( devODEJoint* joint );

  //! Insert a joint in the manipulator
  void Insert( devODEServoMotor* servo );

  //! Read the state of the manipulator
  virtual void Read();

  //! Write the state of the manipulator
  virtual void Write();

 public:

  devODEManipulator( const std::string& devname, 
		     double period,
		     devManipulator::State state,
		     osaCPUMask mask,
		     devODEWorld* world,
		     devManipulator::Mode mode );

  //! ODE Manipulator constructor
  /**
     This constructor initializes an ODE manipulator with the kinematics and 
     dynamics contained in a file. Plus it initializes the ODE elements of the
     manipulators (bodies and joints) for the engine.
     \param devname The name of the task
     \param period The period of the task
     \param state The initial state of the manipulator
     \param world The ODE world used by the manipulator
     \param mode The initial mode of the manipulator
     \param robotfilename The file with the kinematics and dynamics parameters
     \param Rtw0 The offset transformation of the robot base
     \param qinit The initial joint angles
     \param models A vector of CAD models file name (one file per link)
     \param basemodel The filename of the CAD file for the base
  */
  devODEManipulator( const std::string& devname,
		     double period,
		     devManipulator::State state,
		     osaCPUMask mask,
		     devODEWorld* world,
		     devManipulator::Mode mode,
		     const std::string& robotfilename,
		     const vctFrame4x4<double>& Rtw0,
		     const vctDynamicVector<double>& qinit,
		     const std::vector<std::string>& models,
		     const std::string& basemodel );

  //! ODE Manipulator constructor
  /**
     This constructor initializes an ODE manipulator with the kinematics and 
     dynamics contained in a file. Plus it initializes the ODE elements of the
     manipulators (bodies and joints) for the engine.
     \param devname The name of the task
     \param period The period of the task
     \param state The initial state of the manipulator
     \param world The ODE world used by the manipulator
     \param mode The initial mode of the manipulator
     \param robotfilename The file with the kinematics and dynamics parameters
     \param Rtw0 The offset transformation of the robot base
     \param qinit The initial joint angles
     \param models A vector of CAD models file name (one file per link)
     \param basemodel The filename of the CAD file for the base
  */
  devODEManipulator( const std::string& devname,
		     double period,
		     devManipulator::State state,
		     osaCPUMask mask,
		     devODEWorld* world,
		     devManipulator::Mode mode,
		     const std::string& robotfilename,
		     const vctFrame4x4<double>& Rtw0,
		     const vctDynamicVector<double>& qinit,
		     const std::vector<std::string>& models,
		     devODEBody* base=NULL );

  //! ODE Manipulator constructor
  /**
     This constructor initializes an ODE manipulator with the kinematics and 
     dynamics contained in a file. Plus it initializes the ODE elements of the
     manipulators (bodies and joints) for the engine.
     \param devname The name of the task
     \param period The period of the task
     \param state The initial state of the manipulator
     \param world The ODE world used by the manipulator
     \param mode The initial mode of the manipulator
     \param robotfilename The file with the kinematics and dynamics parameters
     \param Rtw0 The offset transformation of the robot base
     \param qinit The initial joint angles
     \param models A vector of CAD models file name (one file per link)
     \param basemodel The filename of the CAD file for the base
  */
  devODEManipulator( const std::string& devname,
		     double period,
		     devManipulator::State state,
		     osaCPUMask mask,
		     devODEWorld* world,
		     devManipulator::Mode mode,
		     const std::string& robotfilename,
		     const vctFrm3& Rt,
		     const vctDynamicVector<double>& qinit,
		     const std::vector<std::string>& models,
		     const std::string& basemodel );

  ~devODEManipulator(){}

  //! Return the joints positions
  /**
     Query each ODE joint and return the joint positions
     \return A vector of joints positions
  */
  virtual vctDynamicVector<double> GetJointsPositions() const ;

  //! Return the joints velocities
  /**
     Query each ODE joint and return the joint velocities
     \return A vector of joints velocities
  */
  virtual vctDynamicVector<double> GetJointsVelocities() const ;

  //! Set the joint position
  /**
     This sets the position command of ODE (internal) servo motors. This does 
     not instantly changes the position. The position values are used to set the
     velocity of the ODE servo motors.
     \param qs A vector of joint positions
  */
  virtual 
    devODEManipulator::Errno
    SetPositions( const vctDynamicVector<double>& qs );

  //! Set the joint velocity
  /**
     This sets the velocity command of ODE (internal) servo motors. This does 
     not instantly changes the velocity. The velocity values are used to set the
     velocity of the ODE servo motors.
     \param qsd A vector of joint velocities
  */
  virtual 
    devODEManipulator::Errno 
    SetVelocities( const vctDynamicVector<double>& qsd );

  //! Set the joint forces or torques
  /**
     This sets the force/torque value of each joint. This method does NOT apply 
     the FT right away. The FT will be applied at the next iteration of the 
     world.
     \param ft A vector of joint forces/torques
  */
  virtual 
    devODEManipulator::Errno 
    SetForcesTorques( const vctDynamicVector<double>& ft);


  //! The ODE state of the manipulator
  typedef std::vector< devODEBody::State > State;

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
  virtual devODEManipulator::State GetState( ) const;

  //! Set the state of the robot
  /**
     For an ODE manipulator, the state of a manipulator is defined by the 
     position/orientation and the velocity of each link. It is not defined by 
     the position/velocity of each joint.
     This method set the state of each link of the robot: 
     (position/orientation + angular/linear velocities) and return them in a 
     vector.
  */
  virtual void SetState( const devODEManipulator::State& state );


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
  void Attach( robManipulator* tool );

  //! Disable all the bodies of the manipulator
  void Disable();

  //! Enable all the bodies of the manipulator
  void Enable();
  
};

#endif
