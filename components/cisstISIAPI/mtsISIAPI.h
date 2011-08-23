/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Nicolas Padoy
  Created on: 2010-04-06

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsISIAPI_h
#define _mtsISIAPI_h

// cisst library headers
#include <cisstMultiTask/mtsMacros.h>
#include <cisstMultiTask/mtsTaskFromSignal.h>
#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstParameterTypes/prmVelocityCartesianGet.h>
#include <cisstParameterTypes/prmPositionJointGet.h>
#include <cisstParameterTypes/prmVelocityJointGet.h>
#include <cisstParameterTypes/prmEventButton.h>


#include <cisstParameterTypes/prmPositionCartesianSet.h>

/*!
  \file
  \brief Declaration of mtsISIAPI
  \ingroup cisstISIAPI
*/

/*!
  \ingroup cisstISIAPI

  This class wraps the ISI da Vinci BBAPI API (read-write) and is
  intended to provide a high-level interface to the da Vinci.

*/

// forward declarations
class mtsISIAPI;

namespace mtsISIAPIUtilities {
    void StreamCallbackInternal(void * userData);
    void EventCallbackInternal(int manipulatorId,
                               int eventId,
                               void * userData);
};


class mtsISIAPI: public mtsTaskPeriodic { //mtsTaskFromSignal {

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_WARNING);

    friend void mtsISIAPIUtilities::StreamCallbackInternal(void * userData);
    friend void mtsISIAPIUtilities::EventCallbackInternal(int manipulatorId,
                                                            int eventId,
                                                            void * userData);
public:

    /*! Constructor.*/
    mtsISIAPI(const std::string & name, unsigned int rateInHz);

    /*! Default destructor. Does nothing. */
    ~mtsISIAPI();

    void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    void Startup(void);
    void Run(void);
    void Cleanup(void);

    /*! Provided for compatibility with mtsISIAPI and C example */
    inline bool IsAPIConnected(void) const {
        return this->Connected;
    }

    /*! Type of manipulator.  Note that indices are contiguous by type
      of manipulator. */
    typedef enum {
        MTML = 0,           /*! Master Left */
        MTMR,               /*! Master Right */
        PSM1,               /*! Patient side 1 */
        PSM2,               /*! Patient side 2 */
        PSM3,               /*! Patient side 3 */
        ECM1,                /*! Endoscopic camera */
        CONSOLE,            /*! Console */
        NUMBER_MANIPULATORS /*! Number of manipulators */
    } ManipulatorIndexType;

protected:

    /*! Class to contain the data common to all arms */
    class ArmData {
    public:
        ArmData(void);
        mtsStateTable * StateTable;
        mtsInterfaceProvided * ProvidedInterface;
        prmPositionCartesianGet PositionCartesian;
        prmVelocityCartesianGet VelocityCartesian;
        prmPositionJointGet PositionJoint;
        prmVelocityJointGet VelocityJoint;
    };

    /*! Class to contain the data specific to the master arms */
    class MasterArmData: public ArmData {
    public:
        MasterArmData(void);
        mtsInterfaceProvided * SelectEventProvidedInterface;
        mtsFunctionWrite Select;
        bool Selected;
        static const double SelectAngle;
        mtsInterfaceProvided * ClutchEventProvidedInterface;
        mtsFunctionWrite Clutch;
        bool ClutchRestAngleNeedsUpdate;
        double ClutchRestAngle;
        bool Clutched;
        static const double ClutchAngle;
    };

    /*! Class to contain the data specific to slave arms */
    class SlaveArmData: public ArmData {
    public:
        SlaveArmData(void);
        prmPositionCartesianGet PositionCartesianRCM;
        prmPositionCartesianGet PositionCartesianSetup;
        prmPositionJointGet PositionJointSetup;
    };

    /*! Class to contain data specific to the console */
    class ConsoleData
    {
    public:
        ConsoleData(void);
        mtsInterfaceProvided * ProvidedInterface;
        mtsFunctionVoid HeadIn;
        mtsFunctionVoid HeadOut;
        mtsFunctionVoid ClutchQuickTap;
        mtsFunctionVoid CameraQuickTap;

        mtsInterfaceProvided * StandbyProvidedInterface;
        mtsFunctionWrite Standby;

        mtsInterfaceProvided * ReadyProvidedInterface;
        mtsFunctionWrite Ready;

        mtsInterfaceProvided * ClutchProvidedInterface;
        mtsFunctionWrite Clutch;
        bool Clutched;

        mtsInterfaceProvided * CameraProvidedInterface;
        mtsFunctionWrite Camera;

        mtsInterfaceProvided * MastersAsMiceProvidedInterface;
        mtsFunctionWrite MastersAsMice;
        bool MastersAsMiced;
    };

    /*! Info for all arms, this container is the primary one */
    ArmData * Arms[(ECM1 - MTML) + 1];

    /*! Info for all master arms, different way to access masters only */
    MasterArmData * MasterArms[(MTMR - MTML) + 1];

    /*! Info for all slave arms, including camera */
    SlaveArmData * SlaveArms[(ECM1 - PSM1) + 1];

    /*! Camera arm ECM1 */
    SlaveArmData * CameraArms[1];

    /*! Console data */
    ConsoleData Console;

    /*! Connect to the daVinci system. */
    bool Connect(void);

    /*! Disconnect from the daVinci system. */
    bool Disconnect(void);

    bool ConfigureStream(void);
    void StreamCallback(void);

    bool ConfigureEvents(void);
    void EventCallback(ManipulatorIndexType index, int eventId);

    bool StartStream(void);

    void StopStream(void);

    /*! Log system information */
    void LogSystemConfiguration(cmnLogLevel logLevel = CMN_LOG_LEVEL_INIT_VERBOSE) const;
    void LogManipulatorConfiguration(ManipulatorIndexType index,
                                     cmnLogLevel logLevel = CMN_LOG_LEVEL_INIT_VERBOSE) const;
    void LogToolConfiguration(ManipulatorIndexType index,
                              cmnLogLevel logLevel = CMN_LOG_LOD_INIT_VERBOSE) const;
    void LogManipulatorsAndToolsConfiguration(cmnLogLevel logLevel = CMN_LOG_LOD_INIT_VERBOSE) const;

    /*! Conversion from manipulator index to string */
    static const std::string & ManipulatorIndexToString(ManipulatorIndexType manipulatorIndex);

    /*! Get number of joints per arm */
    static size_t GetNumberOfJoints(ManipulatorIndexType manipulatorIndex);

    /*! Setup all interfaces and populate them with commands and events */
    //@{
    void SetupArmsInterfaces(void);
    void SetupMastersInterfaces(void);
    void SetupSlavesInterfaces(void);
    void SetupCameraInterfaces(void);
    void SetupConsoleInterfaces(void);
    void SetupAllInterfaces(void);
    //@}

    bool Connected;

    unsigned int RateInHz;
};


CMN_DECLARE_SERVICES_INSTANTIATION(mtsISIAPI);


#endif // _mtsISIAPI_h
