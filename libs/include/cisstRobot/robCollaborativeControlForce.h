/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: robCollaborativeControlForce.h 683 2009-08-14 21:40:14Z gsevinc1 $

  Author(s): Gorkem Sevinc, Anton Deguet
  Created on: 2009-08-20

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robCollaborativeControlForce_h
#define _robCollaborativeControlForce_h

#include <cisstCommon.h>
#include <cisstVector.h>

//Always include last
#include <cisstRobot/robExport.h>

class CISST_EXPORT robCollaborativeControlForce {

public:
    robCollaborativeControlForce(void);
    ~robCollaborativeControlForce(void) {};
    /*!
        Set Parameters function takes arguments from the parent class to mutate the values of;
        Force Limit, Scale Factor Force Coefficient, Force Mode, Master & Slave Clutch.
        These values are currently being used by devicesTutorialExample3 GUI via 
        devSensableHDMasterSlave class.
    */
    void SetParameters(const double & commandedForceLimit, const double & commandedScaleFactor, 
                       const double & commandedForceCoefficient, const int & commandedForceMode, 
                       const bool & commandedMasterClutchGUI, const bool & commandedSlaveClutchGUI,
                       const bool & commandedMasterSlaveClutchGUI);

    /*!
        Update function's goal is to carry out position to force control. Two position parameters
        and two clutch parameters are provided by the parent class. This function uses those positions
        and the clutch parameters to calculate the desired force values for teleoperation. 
        ** Designed for Phantom Omni teleoperation, has not been tested on other platforms.
    */
    void Update(const vct3 & p1, 
                const vct3 & p2,
                const bool ClutchMaster,
                const bool ClutchSlave,
                vctFixedSizeVector<double, 6> & ForceMaster,
                vctFixedSizeVector<double, 6> & ForceSlave);

protected:
    /*! 
        Parameters being used by the Update function for teleoperation
        
        vct3 declarations include the offsets for the workspace, clutch overall,
        
        clutch left and right, and the goals for first and second device.
        prm declarations include the forces, current and temporary positions. 
        
        Other declarations are mostly used to manipulate the above mentioned
        data, such as temporary booleans, scale factor, force limit, etc.
    */
    vct3 WorkspaceOffset;
    vct3 ClutchOffset;
    vct3 LeftClutchOffset;
    vct3 RightClutchOffset;
    vct3 LeftClutchMSOffset;
    vct3 RightClutchMSOffset;
    vct3 Error;
    vct3 p1Goal;
    vct3 p2Goal;
    vct3 p1RGoal;
    vct3 p2RGoal;
    vct3 p1Clutched;
    vct3 p2Clutched;
    vct3 p1R;
    vct3 p2R;
    vct3 p1Last;
    vct3 p2Last;
    
    bool        firstIteration;
    bool        clutchDone;
    bool        bothClutched;
    bool        clutchOffsetAdd;
    bool     MasterClutch;
    bool     SlaveClutch;
    bool     MasterSlaveClutch;
    int      clutchMode;
    int      ForceMode;
    double   ForceMasterCoefficient;
    double   ScaleFactor;
    double   FMax;
    double      ForceFeedNormMaster;
    double      ForceFeedNormSlave;
    int         MasterDeviceNo;
    int         SlaveDeviceNo;

private:

};

#endif //robCollaborativeControlForce
