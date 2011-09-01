/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Simon DiMaio, Anton Deguet
  Created on: 2007-07-01

  (C) Copyright 2007-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstISIAPI/mtsISIAPI.h>
#include <cisstMultiTask.h>
#include <cisstOSAbstraction.h>
#include <iostream>


class MyCallBacks {
public:
    mtsCommandWriteBase * cbStandbySwitch;
    mtsCommandWriteBase * cbReadySwitch;
    mtsCommandWriteBase * cbMasterClutch;
    mtsCommandWriteBase * cbCameraClutch;
    mtsCommandVoid * cbHeadIn;
    mtsCommandVoid * cbHeadOut;
    mtsCommandVoid * cbClutchQuickTap;
    mtsCommandVoid * cbCameraQuickTap;
    mtsCommandWriteBase * cbMastersAsMice;
    mtsCommandWriteBase * cbMTMLClutch;
    mtsCommandWriteBase * cbMTMRClutch;
    mtsCommandWriteBase * cbMTMLeftButton;
    mtsCommandWriteBase * cbMTMRightButton;

    MyCallBacks() {}
    ~MyCallBacks() {}

    ///////////////////
    // Event callbacks.

    // Callback function for Standby Switch presses.
    void standby_switch_callback(const prmEventButton & payload)
    {
        std::cout << "EVENT: Standby Switch:" << payload << std::endl;
    }

    // Callback function for Ready Switch presses.
    void ready_switch_callback(const prmEventButton & payload)
    {
        std::cout << "EVENT: Ready Switch:" << payload << std::endl;
    }

    // Callback function for Master Clutch pedal presses.
    void master_clutch_callback(const prmEventButton & payload)
    {
        std::cout << "EVENT: Master Clutch:" << payload << std::endl;
    }

    // Callback function for Camera Clutch pedal presses.
    void camera_clutch_callback(const prmEventButton & payload)
    {
        std::cout << "EVENT: Camera Clutch:" << payload << std::endl;
    }

    // Callback function for Head In.
    void head_in_callback(void)
    {
        std::cout << "EVENT: Head in." << std::endl;
    }

    // Callback function for Head In.
    void head_out_callback(void)
    {
        std::cout << "EVENT: Head out." << std::endl;
    }

    // Callback function for Arm Swaps.
    void master_quicktap_callback(void)
    {
        std::cout << "EVENT: Clutch Quicktap." << std::endl;
    }

    // Callback function for Video Swaps.
    void camera_quicktap_callback(void)
    {
        std::cout << "EVENT: Camera Quicktap." << std::endl;
    }

    // Callback function for MaM mode.
    void masters_as_mice_callback(const prmEventButton & payload)
    {
        std::cout << "EVENT: MaM:" << payload << std::endl;
    }

    // Callback function for MTM Clutch LEFT
    void mtm_left_clutch_callback(const prmEventButton & payload)
    {
        std::cout << "EVENT: MTM Clutch LEFT:" << payload << std::endl;
    }

    // Callback function for MTM Clutch RIGHT
    void mtm_right_clutch_callback(const prmEventButton & payload)
    {
        std::cout << "EVENT: MTM Clutch RIGHT:" << payload << std::endl;
    }

    // Callback function for MTM left button pressed
    void mtm_left_button_callback(const prmEventButton & payload)
    {
        std::cout << "EVENT: MTM Left Button:" << payload << std::endl;
    }

    // Callback function for MTM right button pressed
    void mtm_right_button_callback(const prmEventButton & payload)
    {
        std::cout << "EVENT: MTM Right Button:" << payload << std::endl;
    }

    template <class _classType>
    mtsCommandVoid * MakeCommandVoid(void (_classType::*method)(void),
                                     _classType * classInstantiation, const std::string & commandName) {
        return new mtsCommandVoid(new mtsCallableVoidMethod<_classType>(method, classInstantiation),
                                  commandName);
    }

    template <class _classType, class _parameterType>
    mtsCommandWriteBase * MakeCommandWrite(void (_classType::*method)(const _parameterType&),
                                           _classType * classInstantiation, const std::string & commandName,
                                           const _parameterType & argumentPrototype) {
        return new mtsCommandWrite<_classType, _parameterType>(method, classInstantiation, commandName, argumentPrototype);
    }

    bool AddHandlers(mtsComponent * component)
    {
        mtsInterfaceProvided * providedInterface = component->GetInterfaceProvided("Console");
        if (!providedInterface) {
            CMN_LOG_INIT_ERROR << "Could not find daVinci MainInterface" << std::endl;
            return false;
        }

        cbHeadIn = MakeCommandVoid(&MyCallBacks::head_in_callback,
                                   this, "HeadIn");
        providedInterface->AddObserver("HeadIn", cbHeadIn);

        cbHeadOut = MakeCommandVoid(&MyCallBacks::head_out_callback,
                                    this, "HeadOut");
        providedInterface->AddObserver("HeadOut", cbHeadOut);

        cbClutchQuickTap = MakeCommandVoid(&MyCallBacks::master_quicktap_callback,
                                           this, "ClutchQuickTap");
        providedInterface->AddObserver("ClutchQuickTap", cbClutchQuickTap);

        cbCameraQuickTap = MakeCommandVoid(&MyCallBacks::camera_quicktap_callback,
                                           this, "CameraQuickTap");
        providedInterface->AddObserver("CameraQuickTap", cbCameraQuickTap);

        // Standby switch interface
        providedInterface = component->GetInterfaceProvided("Standby");
        if (!providedInterface) {
            CMN_LOG_INIT_ERROR << "Could not find daVinci Standby" << std::endl;
            return false;
        }
        cbStandbySwitch = MakeCommandWrite(&MyCallBacks::standby_switch_callback,
                                           this, "Standby", prmEventButton());
        providedInterface->AddObserver("Button", cbStandbySwitch);

        // Ready switch interface
        providedInterface = component->GetInterfaceProvided("Ready");
        if (!providedInterface) {
            CMN_LOG_INIT_ERROR << "Could not find daVinci Ready" << std::endl;
            return false;
        }
        cbReadySwitch = MakeCommandWrite(&MyCallBacks::ready_switch_callback,
                                         this, "Ready", prmEventButton());
        providedInterface->AddObserver("Button", cbReadySwitch);

        // Master Clutch interface
        providedInterface = component->GetInterfaceProvided("Clutch");
        if (!providedInterface) {
            CMN_LOG_INIT_ERROR << "Could not find daVinci Clutch" << std::endl;
            return false;
        }
        cbMasterClutch = MakeCommandWrite(&MyCallBacks::master_clutch_callback,
                                          this, "Clutch", prmEventButton());
        providedInterface->AddObserver("Button", cbMasterClutch);

        // Camera Control interface
        providedInterface = component->GetInterfaceProvided("Camera");
        if (!providedInterface) {
            CMN_LOG_INIT_ERROR << "Could not find daVinci Camera" << std::endl;
            return false;
        }
        cbCameraClutch = MakeCommandWrite(&MyCallBacks::camera_clutch_callback,
                                          this, "Camera", prmEventButton());
        providedInterface->AddObserver("Button", cbCameraClutch);

        // MaM interface
        providedInterface = component->GetInterfaceProvided("MastersAsMice");
        if (!providedInterface) {
            CMN_LOG_INIT_ERROR << "Could not find daVinci MastersAsMice" << std::endl;
            return false;
        }
        cbMastersAsMice = MakeCommandWrite(&MyCallBacks::masters_as_mice_callback,
                                           this, "MastersAsMice", prmEventButton());
        providedInterface->AddObserver("Button", cbMastersAsMice);

        // MaM clutches
        providedInterface = component->GetInterfaceProvided("MTMLClutch");
        if (!providedInterface) {
            CMN_LOG_INIT_ERROR << "Could not find daVinci MTMLClutch" << std::endl;
            return false;
        }
        cbMTMLClutch = MakeCommandWrite(&MyCallBacks::mtm_left_clutch_callback,
                                        this, "MTMLClutch", prmEventButton());
        providedInterface->AddObserver("Button", cbMTMLClutch);
        providedInterface = component->GetInterfaceProvided("MTMRClutch");
        if (!providedInterface) {
            CMN_LOG_INIT_ERROR << "Could not find daVinci MTMRClutch" << std::endl;
            return false;
        }
        cbMTMRClutch = MakeCommandWrite(&MyCallBacks::mtm_right_clutch_callback,
                                        this, "MTMRClutch", prmEventButton());
        providedInterface->AddObserver("Button", cbMTMRClutch);

        // MTML button interface
        providedInterface = component->GetInterfaceProvided("MTMLSelect");

        if (!providedInterface) {
            CMN_LOG_INIT_ERROR << "Could not find daVinci MTMLSelect" << std::endl;
            return false;
        }
        cbMTMLeftButton = MakeCommandWrite(&MyCallBacks::mtm_left_button_callback,
                                           this, "MTMLeftSelect", prmEventButton());
        providedInterface->AddObserver("Button", cbMTMLeftButton);

        // MTMR button interface
        providedInterface = component->GetInterfaceProvided("MTMRSelect");
        if (!providedInterface) {
            CMN_LOG_INIT_ERROR << "Could not find daVinci MTMRSelct" << std::endl;
            return false;
        }
        cbMTMRightButton = MakeCommandWrite(&MyCallBacks::mtm_right_button_callback,
                                            this, "MTMRightSelect", prmEventButton());
        providedInterface->AddObserver("Button", cbMTMRightButton);

        return true;
    }

    /* TEMP -- needed for mtsCommandReadOrWrite. */
    virtual std::string GetName(void) const {
        return "daVinci example";
    }


};

/*=== ENTRY POINT ================================================================================*/
int main()
{
    prmPositionCartesianGet robot_tip;
    MyCallBacks callbacks;
    mtsFunctionRead GetPositionTipPSM1;
    mtsFunctionRead GetPositionTipPSM2;
    mtsFunctionRead GetPositionTipPSM3;
    mtsFunctionRead GetPositionTipECM1;
    mtsFunctionRead GetPositionTipMTMR;
    mtsFunctionRead GetPositionTipMTML;

    // log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::AddChannel(std::cout, CMN_LOG_ALLOW_ERRORS_AND_WARNINGS);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("cdv", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("mts", CMN_LOG_ALLOW_ALL);

    std::cout << "Starting" << std::endl;  // Without this, it crashes

    //simond
    // const long daVinciPeriod = 20; // in milliseconds
    const double daVinciPeriod = 1.0; // in milliseconds

    // create the component manager and the components
    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
    mtsISIAPI * daVinci = new mtsISIAPI("daVinci", 60 /* Hz */ );

    componentManager->AddComponent(daVinci);

    // create an event collector
#if 0
    mtsCollectorEvent * eventCollector =
        new mtsCollectorEvent("EventCollector",
                              mtsCollectorBase::COLLECTOR_FILE_FORMAT_CSV);
    componentManager->AddComponent(eventCollector);

    // add events to observe
    eventCollector->AddObservedComponent(daVinci);

    // connect all interfaces for event collector
    eventCollector->Connect();
#endif

    componentManager->CreateAll();

    // Initialize the da Vinci API
    componentManager->StartAll();

#if 0
    eventCollector->StartCollection(mtsDouble(0.0));
#endif

    osaSleep(1.0 * cmn_s);

    mtsInterfaceProvided * PSM1_Interface = daVinci->GetInterfaceProvided("PSM1");
    if (!PSM1_Interface) {
        CMN_LOG_INIT_ERROR << "Could not find daVinci PSM1" << std::endl;
        return -1;
    }
    mtsInterfaceProvided * PSM2_Interface = daVinci->GetInterfaceProvided("PSM2");
    if (!PSM2_Interface) {
        CMN_LOG_INIT_ERROR << "Could not find daVinci PSM2" << std::endl;
        return -1;
    }
    mtsInterfaceProvided * PSM3_Interface = daVinci->GetInterfaceProvided("PSM3");
    if (!PSM3_Interface) {
        CMN_LOG_INIT_ERROR << "Could not find daVinci PSM3" << std::endl;
        return -1;
    }
    mtsInterfaceProvided * ECM1_Interface = daVinci->GetInterfaceProvided("ECM1");
    if (!ECM1_Interface) {
        CMN_LOG_INIT_ERROR << "Could not find daVinci ECM1" << std::endl;
        return -1;
    }
    mtsInterfaceProvided * MTMR_Interface = daVinci->GetInterfaceProvided("MTMR");
    if (!MTMR_Interface) {
        CMN_LOG_INIT_ERROR << "Could not find daVinci MTMR" << std::endl;
        return -1;
    }
    mtsInterfaceProvided * MTML_Interface = daVinci->GetInterfaceProvided("MTML");
    if (!MTML_Interface) {
        CMN_LOG_INIT_ERROR << "Could not find daVinci MTML" << std::endl;
        return -1;
    }

    GetPositionTipPSM1.Bind(PSM1_Interface->GetCommandRead("GetPositionCartesian"));
    GetPositionTipPSM2.Bind(PSM2_Interface->GetCommandRead("GetPositionCartesian"));
    GetPositionTipPSM3.Bind(PSM3_Interface->GetCommandRead("GetPositionCartesian"));
    GetPositionTipECM1.Bind(ECM1_Interface->GetCommandRead("GetPositionCartesian"));
    GetPositionTipMTMR.Bind(MTMR_Interface->GetCommandRead("GetPositionCartesian"));
    GetPositionTipMTML.Bind(MTML_Interface->GetCommandRead("GetPositionCartesian"));

    if (!callbacks.AddHandlers(daVinci)) {
        return -1;
    }

    osaSleep(5.0 * cmn_s);

    // Poll various states from the API client while connected.
    while (daVinci->IsAPIConnected()) {
        /* Get PSM1 tip position. */
        GetPositionTipPSM1(robot_tip);
        std::cout << "PSM(1):         "   << robot_tip.Position().Translation() << std::endl;
#if 0
        /* Get PSM2 tip position. */
        GetPositionTipPSM2(robot_tip);
        std::cout << "PSM(2):         "   << robot_tip.Position().Translation() << std::endl;

        /* Get PSM3 tip position. */
        GetPositionTipPSM3(robot_tip);
        std::cout << "PSM(3):         "   << robot_tip.Position().Translation() << std::endl;

        /* Get ECM1 tip position. */
        GetPositionTipECM1(robot_tip);
        std::cout << "ECM1:            "   << robot_tip.Position().Translation() << std::endl;

        /* Get MTMR tip position. */
        GetPositionTipMTMR(robot_tip);
        std::cout << "MTM(Right):     "   << robot_tip.Position().Translation() << std::endl;

        /* Get MTMR tip position. */
        GetPositionTipMTML(robot_tip);
        std::cout << "MTM(Left):      "   << robot_tip.Position().Translation() << std::endl;
#endif
        osaSleep(20.0 * cmn_s);
    }

    componentManager->KillAll();

    // stop all logs to avoid output to deleted streams
    cmnLogger::SetMask(CMN_LOG_LOD_NONE);
}
