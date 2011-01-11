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

  // The world ID
  dWorldID worldid;

  dWorldID WorldID() const { return worldid; }

  // vector of joints, bodies
  std::vector<devODEJoint*> joints;
  std::vector<devODEBody*>  bodies;
  std::vector<devODEServoMotor*> servos;

  vctDynamicVector<double> qinit;

  void Insert( devODEBody* body );
  void Insert( devODEJoint* joint );

  devODEBody* base;

  void SetPosition( const vctDynamicVector<double>& qs );
  void SetVelocity( const vctDynamicVector<double>& qsd );

 public:

  typedef std::vector< devODEBody::State > State;

  devODEManipulator( const std::string& devname, 
		     double period,
		     devManipulator::State state,
		     osaCPUMask mask,
		     devODEWorld* world,
		     devManipulator::Mode mode );

  //! ODE Manipulator generic constructor
  /**
     This constructor initializes an ODE manipulator with the kinematics and 
     dynamics contained in a file. Plus it initializes the ODE elements of the
     manipulators (bodies and joints) for the engine.
     \param devname The name of the task
     \param period The period of the task
     \param world The ODE world used by the manipulator
     \param robotfilename The file with the kinematics and dynamics parameters
     \param qinit The initial joint angles
     \param Rtw0 The offset transformation of the robot base
  */
  devODEManipulator( const std::string& devname,
		     double period,
		     devManipulator::State state,
		     osaCPUMask mask,
		     devODEWorld* world,
		     devManipulator::Mode mode,
		     const std::string& robotfile,
		     const vctFrame4x4<double>& Rtw0,
		     const vctDynamicVector<double>& qinit,
		     const std::vector<std::string>& models,
		     const std::string& basemodel );

  ~devODEManipulator(){}

  //! Return the joints positions
  /**
     Query each ODE joint for their positions and add the initial offset to 
     them
     \return A vector of joints positions
  */
  vctDynamicVector<double> GetJointsPositions() const ;

  //! Return the joints velocities
  /**
     Query each ODE joint for their velocities
     \return A vector of joints velocities
  */
  vctDynamicVector<double> GetJointsVelocities() const ;

  //! Set the joint forces or torques
  /**
     This sets the force/torque value of each joint. This task will be applied
     at each iteration of the world task. This method does NOT apply the FT 
     values to each joint.
   */
  void SetForcesTorques( const vctDynamicVector<double>& ft);

  void Read();
  void Write();
  void Write( const vctDynamicVector<double>& ft );

  devODEManipulator::State GetState( ) const;
  void SetState( const devODEManipulator::State& state );

  dBodyID BaseID() const;

  void Attach( robManipulator* tool );

  void Disable();
  void Enable();
  
};

#endif
