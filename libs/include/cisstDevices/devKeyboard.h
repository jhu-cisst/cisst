/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):	Gorkem Sevinc, Anton Deguet
  Created on:   2009-09-17

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _devKeyboard_h
#define _devKeyboard_h

#include <cisstCommon.h>
#include <cisstParameterTypes.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsTaskContinuous.h>
#include <map>

// Always include last
#include <cisstDevices/devExport.h>


class CISST_EXPORT devKeyboard: public mtsTaskContinuous {
    // declare services, requires dynamic creation
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    devKeyboard(void);
    ~devKeyboard(void) {};
    void Configure(const std::string & CMN_UNUSED(filename) = "") {};
    void Startup(void){};
    void Run(void);
    void Cleanup(void) {};
    
    enum TriggerType {BUTTON_EVENT, VOID_EVENT, WRITE_COMMAND};

    void AddKeyButtonEvent(char key, const std::string & interfaceName, bool toggle); 
    void AddKeyVoidEvent(char key, const std::string & interfaceName, const std::string & eventName);
    void AddKeyWriteCommand(char key, const std::string & interfaceName, const std::string & commandName, bool initialState);
    void SetQuitKey(char quitKey);

    struct KeyData {
        bool Toggle;
        bool State;
        mtsFunctionWrite WriteTrigger;
        mtsFunctionVoid VoidTrigger;
        TriggerType Type;
    };

protected:
    typedef std::multimap<char, KeyData *> KeyDataType;
    KeyDataType KeyboardDataMap;

    char QuitKey;
    char KeyboardInput; // temp
    CMN_DECLARE_MEMBER_AND_ACCESSORS(bool, Done);
};


CMN_DECLARE_SERVICES_INSTANTIATION(devKeyboard);


#endif // _devKeyboard_h
