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

#ifndef _robManipulatorODE_h
#define _robManipulatorODE_h

#ifdef CISST_ODE_SUPPORT

#include <cisstRobot/robManipulator.h>
#include <cisstRobot/robExport.h>

class CISST_EXPORT robManipulatorODE : public robManipulator{

public: 

  //! ODE Manipulator generic constructor
  /**
     This constructor initializes an ODE manipulator with the kinematics and 
     dynamics contained in a file. Plus it initializes the ODE elements of the
     manipulators (bodies and joints) for the engine.
     \param robotfilename The file with the kinematics and dynamics parameters
     \param toolfilename The file with the kinmatics and dynamics of a tool
     \param Rtw0 The offset transformation of the robot base
  */
  robManipulatorODE( dWorldID worldid, 
		     dSpaceID spaceid,
		     const std::string& robotfilename,
		     const std::string& toolfilename = std::string(),
		     const vctFrame4x4<double,VCT_ROW_MAJOR>& Rtw0 
		     = vctFrame4x4<double,VCT_ROW_MAJOR>() );
  
  void SetJointsForcesTorques( const vctDynamicVector<double>& ft );


  vctDynamicVector<double> GetJointsPositions() const ;

  vctDynamicVector<double> GetJointsVelocities() const ;

  void Update();

};

#endif

#endif
