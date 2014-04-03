/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#include <cisstCommon/cmnAccessorMacros.h>
#include <cisstVector/vctTypes.h>

//Always include last
#include <cisstRobot/robExport.h>

#error "This class is deprecated, do not use"

class CISST_EXPORT robCollaborativeControlForce {

public:
    class CISST_EXPORT ParameterType {

    public:
        enum ForceModeType {RAW, RATCHETED, CAPPED};
        CMN_DECLARE_MEMBER_AND_ACCESSORS(ForceModeType, ForceMode);
        CMN_DECLARE_MEMBER_AND_ACCESSORS(double, ForceFeedbackRatio); //ForceMasterCoefficient
        CMN_DECLARE_MEMBER_AND_ACCESSORS(double, LinearGainMaster); //ScaleFactor
        CMN_DECLARE_MEMBER_AND_ACCESSORS(double, LinearGainSlave);
        CMN_DECLARE_MEMBER_AND_ACCESSORS(double, PositionLimit); // FMax
        CMN_DECLARE_MEMBER_AND_ACCESSORS(double, MasterToSlaveScale);
    public:
        /*! Human readable output to stream. */
        void ToStream(std::ostream & outputStream) const;

        /*! To stream raw data. */
        void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const;

        /*! Binary serialization */
        void SerializeRaw(std::ostream & outputStream) const;

        /*! Binary deserialization */
        void DeSerializeRaw(std::istream & inputStream);
    };

    robCollaborativeControlForce(void);
    robCollaborativeControlForce(double linearGainMaster,
                                 double linearGainSlave, 
                                 double forceLimit,
                                 ParameterType::ForceModeType forceMode,
                                 double masterToSlaveScale,
                                 double forceFeedbackRatio);
    ~robCollaborativeControlForce(void) {};
    /*!
        Set Parameters function takes arguments from the parent class to mutate the values of;
        Force Limit, Scale Factor Force Coefficient, Force Mode.
        These values are currently being used by devicesTutorialExample3 GUI via 
        devSensableHDMasterSlave class.
    */
    void SetParameters(const ParameterType & commandedParameter);

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
        
        Other declarations(bool, double) are mostly used to manipulate the above 
        mentioned data, such as temporary booleans, scale factor, force limit, etc.
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
    
    bool        FirstIteration;
    bool        ClutchDone;
    bool        BothClutched;
    bool        ClutchOffsetAdd;
    int         ClutchMode;
    double      ForceFeedNormMaster;
    double      ForceFeedNormSlave;
    vctFrm3 RelativeTransform;

    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, ApplicationMasterClutch);
    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, ApplicationSlaveClutch);
    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, ApplicationMasterSlaveClutch);
    CMN_DECLARE_MEMBER_AND_ACCESSORS(ParameterType, Parameter);

private:

};

#endif //robCollaborativeControlForce
