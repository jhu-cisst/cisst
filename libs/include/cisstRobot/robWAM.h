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

#ifndef _robWAM_h
#define _robWAM_h

#include <cisstRobot/robManipulator.h>
#include <cisstRobot/robPuck.h>
#include <cisstRobot/robGroup.h>
#include <cisstRobot/robCAN.h>

namespace cisstRobot{

  //! This implements the "stuff" that is specific to a WAM
  /**
     This class implements all the stuff that is specific to a WAM (a real WAM
     not a simulated one). That is it handles the pucks, groups (of pucks), 
     differential drives and gear ratios.
  */
  class robWAM : public robManipulator{
    
  private:

    static const devPuckID SAFETY_NID = 10; // that the safety module node ID

    std::vector<devPuck>        pucks;      // a vector of pucks
    std::vector<devGroup>      groups;      // a vector of pucks groups
    devSafetyModule      safetymodule;      // the safety module

    robError Init( const std::string& wamfilename );

    //! matrix used to convert motors pos to joints pos
    double Mpos2Jpos[7][7];
    //! matrix used to convert joints pos to motors pos
    double Jpos2Mpos[7][7];
    //! matrix used to convert joints trq to motors trq
    double Jtrq2Mtrq[7][7];

    protected:

    //! Convert motor positions to joint angles
    /**
       This function is used to convert motor positions to joint angles. This
       account for the gear rations and the differential drive of the WAM.
    */
    void ActuatorsPositions2JointsPositions(double dt);

    //! Convert joint angles to motor positions
    /**
       This function is used to convert joint angles to motor positions. This
       account for the gear rations and the differential drive of the WAM.
     */
    void JointsPositions2ActuatorsPositions();

    //! Convert joint torques to motor torques
    /**
       This function is used to convert joint torques to motor torques. This
       account for the gear rations and the differential drive of the WAM.
     */
    void JointsFT2ActuatorsFT();

    //! Default constructor
    /**
       This initializes a WAM arm with its dynamics/kinematics.
       \param manfile The .man file used to initialize the kinematics and the 
                      dynamics of the manipulator
       \param toolfile The .tool file if any
       \param Rt The vctFrame4x4<double,VCT_ROW_MAJOR> representing the base of the WAM
       \param canbus The CAN bus device used to communicate with the WAM
       \param wamfile The file used to initialze the gear ratios and 
                      differential drive (this should be removed)
    */
    robWAM( const std::string& manfile,
	    const std::string& toolfile,
	    const math::vctFrame4x4<double,VCT_ROW_MAJOR>& Rt,
	    const math::vctDynamicVector<double>& q0,
	    const std::string& wamfile );

    devCANFrame PackMotorsTorques(openman::GroupID gid);

  };
}

#endif

























