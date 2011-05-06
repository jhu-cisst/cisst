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

#ifndef _devOSGManipulator_h
#define _devOSGManipulator_h


#include <cisstRobot/robManipulator.h>
#include <cisstDevices/robotcomponents/manipulators/devManipulator.h>
#include <cisstDevices/robotcomponents/osg/devOSGBody.h>
#include <cisstDevices/robotcomponents/osg/devOSGWorld.h>
#include <cisstMultiTask/mtsTransformationTypes.h>
#include <cisstDevices/devExport.h>

class CISST_EXPORT devOSGManipulator : 

  public devManipulator,
  public robManipulator,
  public osg::Group {

 protected:

  //! Input of N joints
  RnIO* input;
  //! Output of N joints
  RnIO* output;

  //! Store the joints values
  vctDynamicVector<double> q;

  //! Read the state of the manipulator
  virtual void Read();
  
  //! Write the state of the manipulator
  virtual void Write();

  //! OSG Manipulator generic constructor
  /**
     This constructor initializes an OSG manipulator with the kinematics and 
     dynamics contained in a file. Plus it initializes the OSG elements of the
     manipulators (bodies and joints) for the engine.
     \param devname   The name of the task
     \param period    The period of the task
     \param state     The initial state of the manipulator
     \param mask      The CPU to host this task
     \param inputmode The input mode
  */
  devOSGManipulator( const std::string& devname,
		     double period,
		     devManipulator::State state,
		     osaCPUMask mask,
		     devManipulator::Mode inputmode );

  //! OSG Manipulator generic constructor
  /**
     This constructor initializes an OSG manipulator with the kinematics and 
     dynamics contained in a file. Plus it initializes the OSG elements of the
     manipulators (bodies and joints) for the engine.
     \param devname The name of the task
     \param period The period of the task
     \param state  The initial state of the manipulator
     \param mask   The CPU to host this task
     \param inputmode The input mode
     \param robotfile The file with the kinematics and dynamics parameters
     \param Rtw0 The offset transformation of the robot base
  */
  devOSGManipulator( const std::string& devname,
		     double period,
		     devManipulator::State state,
		     osaCPUMask mask,
		     devManipulator::Mode inputmode,
		     const std::string& robotfile,
		     const vctFrame4x4<double>& Rtw0 );

 public: 

  //! OSG Manipulator generic constructor
  /**
     This constructor initializes an OSG manipulator with the kinematics and 
     dynamics contained in a file. Plus it initializes the OSG elements of the
     manipulators (bodies and joints) for the engine.
     \param devname The name of the task
     \param period The period of the task
     \param state  The initial state of the manipulator
     \param mask   The CPU to host this task
     \param world  The OSG world
     \param robotfile The file with the kinematics and dynamics parameters
     \param Rtw0 The offset transformation of the robot base
     \param qinit The initial joint angles
     \param models A vector of 3D model file names
     \param basemodel The file name of the base 3D model
  */
  devOSGManipulator( const std::string& devname,
		     double period,
		     devManipulator::State state,
		     osaCPUMask mask,
		     devOSGWorld* world,
		     const std::string& robotfn,
		     const vctFrame4x4<double>& Rtw0,
		     const vctDynamicVector<double>& qinit,
		     const std::vector<std::string>& models,
		     const std::string& basemodel = "" );

  ~devOSGManipulator();

};

#endif
