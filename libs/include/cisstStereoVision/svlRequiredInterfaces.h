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

#ifndef _svlRequiredInterfaces_h
#define _svlRequiredInterfaces_h

#include <cisstMultitask/mtsComponent.h>
#include <cisstMultitask/mtsFunctionVoid.h>
#include <cisstMultitask/mtsFunctionRead.h>
#include <cisstMultitask/mtsFunctionQualifiedRead.h>
#include <cisstMultitask/mtsFunctionWrite.h>


/*******************************/
/*** IReqStreamManager class ***/
/*******************************/

class IReqStreamManager
{
public:
    IReqStreamManager(const std::string& name, mtsComponent* owner);
    ~IReqStreamManager();

    mtsFunctionWrite SetSourceFilter;
    mtsFunctionVoid  Initialize;
    mtsFunctionVoid  Release;
    mtsFunctionVoid  Play;

private:
    IReqStreamManager();
    IReqStreamManager(const IReqStreamManager& instance);

    std::string Name;
    mtsComponent* Owner;
};


/***************************************/
/*** IReqFilterSourceVideoFile class ***/
/***************************************/

class IReqFilterSourceVideoFile
{
public:
    IReqFilterSourceVideoFile(const std::string& name, mtsComponent* owner);
    ~IReqFilterSourceVideoFile();

    // svlFilterSourceBase commands
    mtsFunctionWrite         SetFramerate;
    mtsFunctionWrite         SetLoop;
    mtsFunctionVoid          Pause;
    mtsFunctionVoid          Play;
    mtsFunctionWrite         PlayFrames;

    // svlFilterSourceVideoFile commands
    mtsFunctionRead          Get;
    mtsFunctionWrite         Set;
    mtsFunctionWrite         SetChannels;
    mtsFunctionWrite         SetFilename;
    mtsFunctionWrite         SetLeftFilename;
    mtsFunctionWrite         SetRightFilename;
    mtsFunctionWrite         SetPosition;
    mtsFunctionWrite         SetLeftPosition;
    mtsFunctionWrite         SetRightPosition;
    mtsFunctionWrite         SetRange;
    mtsFunctionWrite         SetLeftRange;
    mtsFunctionWrite         SetRightRange;
    mtsFunctionQualifiedRead GetPositionAtTime;
    mtsFunctionQualifiedRead GetLeftPositionAtTime;
    mtsFunctionQualifiedRead GetRightPositionAtTime;
    mtsFunctionQualifiedRead GetTimeAtPosition;
    mtsFunctionQualifiedRead GetLeftTimeAtPosition;
    mtsFunctionQualifiedRead GetRightTimeAtPosition;

private:
    IReqFilterSourceVideoFile();
    IReqFilterSourceVideoFile(const IReqFilterSourceVideoFile& instance);

    std::string Name;
    mtsComponent* Owner;
};


/******************************************/
/*** IReqFilterSourceVideoCapture class ***/
/******************************************/

class IReqFilterSourceVideoCapture
{
public:
    IReqFilterSourceVideoCapture(const std::string& name, mtsComponent* owner);
    ~IReqFilterSourceVideoCapture();

    // svlFilterSourceBase commands
    mtsFunctionVoid          Pause;
    mtsFunctionVoid          Play;
    mtsFunctionWrite         PlayFrames;

    // svlFilterSourceVideoCapture commands
    mtsFunctionVoid          EnumerateDevices;
    mtsFunctionRead          GetDeviceList;
    mtsFunctionRead          Get;
    mtsFunctionWrite         Set;
    mtsFunctionWrite         SetChannels;
    mtsFunctionWrite         SetDevice;
    mtsFunctionWrite         SetLeftDevice;
    mtsFunctionWrite         SetRightDevice;
    mtsFunctionWrite         SetInput;
    mtsFunctionWrite         SetLeftInput;
    mtsFunctionWrite         SetRightInput;
    mtsFunctionWrite         SetFormat;
    mtsFunctionWrite         SetLeftFormat;
    mtsFunctionWrite         SetRightFormat;
    mtsFunctionWrite         SelectFormat;
    mtsFunctionWrite         SelectLeftFormat;
    mtsFunctionWrite         SelectRightFormat;
    mtsFunctionWrite         SetTrigger;
    mtsFunctionWrite         SetLeftTrigger;
    mtsFunctionWrite         SetRightTrigger;
    mtsFunctionWrite         SetImageProperties;
    mtsFunctionWrite         SetLeftImageProperties;
    mtsFunctionWrite         SetRightImageProperties;
    mtsFunctionWrite         SaveSettings;
    mtsFunctionWrite         LoadSettings;
    mtsFunctionQualifiedRead GetFormatList;
    mtsFunctionQualifiedRead GetTrigger;
    mtsFunctionQualifiedRead GetImageProperties;

private:
    IReqFilterSourceVideoCapture();
    IReqFilterSourceVideoCapture(const IReqFilterSourceVideoCapture& instance);

    std::string Name;
    mtsComponent* Owner;
};


#endif // _svlRequiredInterfaces_h

