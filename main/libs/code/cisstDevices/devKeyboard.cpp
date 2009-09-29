/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsVector.cpp 535 2009-07-14 00:27:41Z adeguet1 $

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


#include <cisstDevices/devKeyboard.h>

CMN_IMPLEMENT_SERVICES(devKeyboard);

devKeyboard::devKeyboard(void):
    mtsTaskContinuous("keyboard")
{
    DoneMember = false;
}


void devKeyboard::AddKeyButtonEvent(char key, const std::string & interfaceName, bool toggle)
{
    // create new key data and add to list
    KeyData * keyData = new KeyData;
    KeyboardDataMap.insert(std::make_pair(key, keyData));

    // set key action
    keyData->Type = BUTTON_EVENT;
    keyData->Toggle = toggle;
    keyData->State = false;

    // add interface
    mtsProvidedInterface * providedInterface = this->GetProvidedInterface(interfaceName);
    if (!providedInterface) {
        providedInterface = this->AddProvidedInterface(interfaceName);
    }
    keyData->WriteTrigger.Bind(providedInterface->AddEventWrite("Button",
                               prmEventButton()));
}


void devKeyboard::AddKeyVoidEvent(char key, const std::string & interfaceName, const std::string & eventName)
{
    // create new key data and add to list
    KeyData * keyData = new KeyData;
    KeyboardDataMap.insert(std::make_pair(key, keyData));  

    // set key action
    keyData->Type = VOID_EVENT;
    keyData->State = false;

    // add interface
    mtsProvidedInterface * providedInterface = this->GetProvidedInterface(interfaceName);
    if (!providedInterface) {
        providedInterface = this->AddProvidedInterface(interfaceName);
    }
    keyData->VoidTrigger.Bind(providedInterface->AddEventVoid(eventName));
}


void devKeyboard::AddKeyWriteCommand(char key, const std::string & interfaceName, const std::string & commandName, bool initialState)
{
    // create new key data and add to list
    KeyData * keyData = new KeyData;
    KeyboardDataMap.insert(std::make_pair(key, keyData));

    // set key action
    keyData->Type = WRITE_COMMAND;
    keyData->State = initialState;

    // add interface
    mtsRequiredInterface * requiredInterface = this->GetRequiredInterface(interfaceName);
    if (!requiredInterface) {
        requiredInterface = this->AddRequiredInterface(interfaceName);
    }
    requiredInterface->AddFunction(commandName, keyData->WriteTrigger);
}


void devKeyboard::SetQuitKey(char quitKey)
{
    QuitKey = quitKey;
}


void devKeyboard::Run(void) 
{
    KeyboardInput = cmnGetChar();
    if (KeyboardInput == QuitKey) {
         DoneMember = true;
    }

    if (!this->Done()) {
        prmEventButton event;
        KeyData * keyData;
        KeyDataType::iterator iterator;
        const KeyDataType::iterator end = this->KeyboardDataMap.end();
        for (iterator = this->KeyboardDataMap.begin();
             iterator != end;
             iterator++)
        {
            if (iterator->first == KeyboardInput) {
                keyData = iterator->second;
                switch (keyData->Type)
                {
                case BUTTON_EVENT:
                    if (keyData->Toggle == true) {
                        if (keyData->State == true) {
                            event.SetType(prmEventButton::RELEASED);
                            keyData->State = false;
                        } else {
                            event.SetType(prmEventButton::PRESSED);
                            keyData->State = true;
                        }
                    } else {
                        event.SetType(prmEventButton::RELEASED);
                        keyData->State = true;
                    }
                    keyData->WriteTrigger(event);
                    CMN_LOG_CLASS_RUN_DEBUG << "Run " << KeyboardInput << " sending button event " << event << std::endl;
                    break;
                case VOID_EVENT:
                    keyData->VoidTrigger();
                    CMN_LOG_CLASS_RUN_DEBUG << "Run " << KeyboardInput << " sending void event " << std::endl;
                    break;
                case WRITE_COMMAND:
                    mtsBool value = keyData->State;
                    keyData->WriteTrigger(value);
                    keyData->State = !(keyData->State);
                    CMN_LOG_CLASS_RUN_DEBUG << "Run " << KeyboardInput << " sending write command " << value << std::endl;
                    break;
                }
            }
        }
    }
}

