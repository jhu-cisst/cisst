/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultitask/mtsInterfaceRequired.h>
#include <cisstStereoVision/svlRequiredInterfaces.h>


/*******************************/
/*** IReqStreamManager class ***/
/*******************************/

IReqStreamManager::IReqStreamManager(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name);
        if (required) {
            required->AddFunction("SetSourceFilter", SetSourceFilter);
            required->AddFunction("Initialize",      Initialize);
            required->AddFunction("Release",         Release);
            required->AddFunction("Play",            Play);
        }
    }
}

IReqStreamManager::~IReqStreamManager()
{
    if (Owner) Owner->RemoveInterfaceRequired(Name);
}


/***************************************/
/*** IReqFilterSourceVideoFile class ***/
/***************************************/

IReqFilterSourceVideoFile::IReqFilterSourceVideoFile(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name);
        if (required) {
            required->AddFunction("Get",                    Get);
            required->AddFunction("Set",                    Set);
            required->AddFunction("SetChannels",            SetChannels);
            required->AddFunction("SetFilename",            SetFilename);
            required->AddFunction("SetLeftFilename",        SetLeftFilename);
            required->AddFunction("SetRightFilename",       SetRightFilename);
            required->AddFunction("SetPosition",            SetPosition);
            required->AddFunction("SetLeftPosition",        SetLeftPosition);
            required->AddFunction("SetRightPosition",       SetRightPosition);
            required->AddFunction("SetRange",               SetRange);
            required->AddFunction("SetLeftRange",           SetLeftRange);
            required->AddFunction("SetRightRange",          SetRightRange);
            required->AddFunction("GetPositionAtTime",      GetPositionAtTime);
            required->AddFunction("GetLeftPositionAtTime",  GetLeftPositionAtTime);
            required->AddFunction("GetRightPositionAtTime", GetRightPositionAtTime);
            required->AddFunction("GetTimeAtPosition",      GetTimeAtPosition);
            required->AddFunction("GetLeftTimeAtPosition",  GetLeftTimeAtPosition);
            required->AddFunction("GetRightTimeAtPosition", GetRightTimeAtPosition);
            required->AddFunction("SetFramerate",           SetFramerate);
            required->AddFunction("SetLoop",                SetLoop);
            required->AddFunction("Pause",                  Pause);
            required->AddFunction("Play",                   Play);
            required->AddFunction("PlayFrames",             PlayFrames);
        }
    }
}

IReqFilterSourceVideoFile::~IReqFilterSourceVideoFile()
{
    if (Owner) Owner->RemoveInterfaceRequired(Name);
}

