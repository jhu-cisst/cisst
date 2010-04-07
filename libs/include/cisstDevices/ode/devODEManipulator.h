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

#include <string>

#include <cisstMultiTask/mtsTaskPeriodic.h>

#include <cisstDevices/ode/devODEBody.h>
#include <cisstDevices/ode/devODEJoint.h>
#include <cisstDevices/ode/devODEWorld.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devODEManipulator : public mtsTaskPeriodic {

 private:

  //! MTS vector of joint positions
  mtsVector<double> jointspositions;

  //! MTS function used to querry joints torques
  mtsFunctionRead ReadFT;

  //! A vector of joints
  std::vector<devODEJoint*> joints;

  vctDynamicVector<double> qinit;

  //! Set the joint forces or torques
  /**
     This sets the force/torque value of each joint. This task will be applied
     at each iteration of the world task. This method does NOT apply the FT 
     values to each joint.
   */
  void SetForcesTorques( const vctDynamicVector<double>& ft);

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


 public: 

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
		     devODEWorld& world,
		     const std::string& robotfilename,
		     const vctDynamicVector<double> qinit,
		     const vctFrame4x4<double>& Rtw0,
		     const std::vector<std::string>& geomfiles );


  void Configure( const std::string& fn="");
  void Startup();
  void Run();
  void Cleanup();

  static const std::string PositionsInterface;
  static const std::string ReadPositionsCommand;

  static const std::string FTInterface;
  static const std::string WriteFTCommand;

};

//CMN_DECLARE_SERVICES_INSTANTIATION( devODEManipulator );

#endif
