/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstStereoVision/svlRequiredInterfaces.h>


#if 0  // Following now in mtsClassServices.cpp
/*********************************/
/*** SVL-MTS type declarations ***/
/*********************************/

CMN_IMPLEMENT_SERVICES_TEMPLATED(svlFloat)
CMN_IMPLEMENT_SERVICES_TEMPLATED(svlDouble2)
CMN_IMPLEMENT_SERVICES_TEMPLATED(svlDouble3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(svlDouble3x3)
CMN_IMPLEMENT_SERVICES_TEMPLATED(svlInt2)
#endif


/*******************************/
/*** IReqStreamManager class ***/
/*******************************/

IReqStreamManager::IReqStreamManager(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name, MTS_OPTIONAL);
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
/*** IReqFilterSourceImageFile class ***/
/***************************************/

IReqFilterSourceImageFile::IReqFilterSourceImageFile(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name, MTS_OPTIONAL);
        if (required) {
            required->AddFunction("SetFramerate",       SetFramerate);
            required->AddFunction("SetLoop",            SetLoop);
            required->AddFunction("Pause",              Pause);
            required->AddFunction("Play",               Play);
            required->AddFunction("PlayFrames",         PlayFrames);
            required->AddFunction("SetChannels",        SetChannels);
            required->AddFunction("SetFile",            SetFile);
            required->AddFunction("GetChannels",        GetChannels);
            required->AddFunction("GetFile",            GetFile);
            required->AddFunction("GetDimensions",      GetDimensions);
            required->AddFunction("GetLeftDimensions",  GetLeftDimensions);
            required->AddFunction("GetRightDimensions", GetRightDimensions);
        }
    }
}

IReqFilterSourceImageFile::~IReqFilterSourceImageFile()
{
    if (Owner) Owner->RemoveInterfaceRequired(Name);
}


/***************************************/
/*** IReqFilterSourceTextFile class ***/
/***************************************/

IReqFilterSourceTextFile::IReqFilterSourceTextFile(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name, MTS_OPTIONAL);
        if (required) {
            required->AddFunction("SetFramerate",  SetFramerate);
            required->AddFunction("SetLoop",       SetLoop);
            required->AddFunction("Pause",         Pause);
            required->AddFunction("Play",          Play);
            required->AddFunction("PlayFrames",    PlayFrames);
            required->AddFunction("SetErrorValue", SetErrorValue);
            required->AddFunction("SetColumns",    SetColumns);
            required->AddFunction("AddFile",       AddFile);
            required->AddFunction("GetErrorValue", GetErrorValue);
            required->AddFunction("GetColumns",    GetColumns);
            required->AddFunction("RemoveFiles",   RemoveFiles);
        }
    }
}

IReqFilterSourceTextFile::~IReqFilterSourceTextFile()
{
    if (Owner) Owner->RemoveInterfaceRequired(Name);
}


/******************************************/
/*** IReqFilterSourceVideoCapture class ***/
/******************************************/

IReqFilterSourceVideoCapture::IReqFilterSourceVideoCapture(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name, MTS_OPTIONAL);
        if (required) {
            required->AddFunction("Pause",                   Pause);
            required->AddFunction("Play",                    Play);
            required->AddFunction("PlayFrames",              PlayFrames);
            required->AddFunction("EnumerateDevices",        EnumerateDevices);
            required->AddFunction("GetDeviceList",           GetDeviceList);
            required->AddFunction("Get",                     Get);
            required->AddFunction("Set",                     Set);
            required->AddFunction("SetChannels",             SetChannels);
            required->AddFunction("SetDevice",               SetDevice);
            required->AddFunction("SetLeftDevice",           SetLeftDevice);
            required->AddFunction("SetRightDevice",          SetRightDevice);
            required->AddFunction("SetInput",                SetInput);
            required->AddFunction("SetLeftInput",            SetLeftInput);
            required->AddFunction("SetRightInput",           SetRightInput);
            required->AddFunction("SetFormat",               SetFormat);
            required->AddFunction("SetLeftFormat",           SetLeftFormat);
            required->AddFunction("SetRightFormat",          SetRightFormat);
            required->AddFunction("SelectFormat",            SelectFormat);
            required->AddFunction("SelectLeftFormat",        SelectLeftFormat);
            required->AddFunction("SelectRightFormat",       SelectRightFormat);
            required->AddFunction("SetTrigger",              SetTrigger);
            required->AddFunction("SetLeftTrigger",          SetLeftTrigger);
            required->AddFunction("SetRightTrigger",         SetRightTrigger);
            required->AddFunction("SetImageProperties",      SetImageProperties);
            required->AddFunction("SetLeftImageProperties",  SetLeftImageProperties);
            required->AddFunction("SetRightImageProperties", SetRightImageProperties);
            required->AddFunction("SaveSettings",            SaveSettings);
            required->AddFunction("LoadSettings",            LoadSettings);
            required->AddFunction("GetChannels",             GetChannels);
            required->AddFunction("GetDevice",               GetDevice);
            required->AddFunction("GetLeftDevice",           GetLeftDevice);
            required->AddFunction("GetRightDevice",          GetRightDevice);
            required->AddFunction("GetInput",                GetInput);
            required->AddFunction("GetLeftInput",            GetLeftInput);
            required->AddFunction("GetRightInput",           GetRightInput);
            required->AddFunction("GetFormat",               GetFormat);
            required->AddFunction("GetLeftFormat",           GetLeftFormat);
            required->AddFunction("GetRightFormat",          GetRightFormat);
            required->AddFunction("GetDimensions",           GetDimensions);
            required->AddFunction("GetLeftDimensions",       GetLeftDimensions);
            required->AddFunction("GetRightDimensions",      GetRightDimensions);
            required->AddFunction("GetFormatList",           GetFormatList);
            required->AddFunction("GetLeftFormatList",       GetLeftFormatList);
            required->AddFunction("GetRightFormatList",      GetRightFormatList);
            required->AddFunction("GetTrigger",              GetTrigger);
            required->AddFunction("GetLeftTrigger",          GetLeftTrigger);
            required->AddFunction("GetRightTrigger",         GetRightTrigger);
            required->AddFunction("GetImageProperties",      GetImageProperties);
            required->AddFunction("GetLeftImageProperties",  GetLeftImageProperties);
            required->AddFunction("GetRightImageProperties", GetRightImageProperties);
        }
    }
}

IReqFilterSourceVideoCapture::~IReqFilterSourceVideoCapture()
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
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name, MTS_OPTIONAL);
        if (required) {
            required->AddFunction("SetFramerate",           SetFramerate);
            required->AddFunction("SetLoop",                SetLoop);
            required->AddFunction("Pause",                  Pause);
            required->AddFunction("Play",                   Play);
            required->AddFunction("PlayFrames",             PlayFrames);
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
            required->AddFunction("GetChannels",            GetChannels);
            required->AddFunction("GetFilename",            GetFilename);
            required->AddFunction("GetLeftFilename",        GetLeftFilename);
            required->AddFunction("GetRightFilename",       GetRightFilename);
            required->AddFunction("GetLength",              GetLength);
            required->AddFunction("GetLeftLength",          GetLeftLength);
            required->AddFunction("GetRightLength",         GetRightLength);
            required->AddFunction("GetPosition",            GetPosition);
            required->AddFunction("GetLeftPosition",        GetLeftPosition);
            required->AddFunction("GetRightPosition",       GetRightPosition);
            required->AddFunction("GetRange",               GetRange);
            required->AddFunction("GetLeftRange",           GetLeftRange);
            required->AddFunction("GetRightRange",          GetRightRange);
            required->AddFunction("GetDimensions",          GetDimensions);
            required->AddFunction("GetLeftDimensions",      GetLeftDimensions);
            required->AddFunction("GetRightDimensions",     GetRightDimensions);
            required->AddFunction("GetPositionAtTime",      GetPositionAtTime);
            required->AddFunction("GetLeftPositionAtTime",  GetLeftPositionAtTime);
            required->AddFunction("GetRightPositionAtTime", GetRightPositionAtTime);
            required->AddFunction("GetTimeAtPosition",      GetTimeAtPosition);
            required->AddFunction("GetLeftTimeAtPosition",  GetLeftTimeAtPosition);
            required->AddFunction("GetRightTimeAtPosition", GetRightTimeAtPosition);
        }
    }
}

IReqFilterSourceVideoFile::~IReqFilterSourceVideoFile()
{
    if (Owner) Owner->RemoveInterfaceRequired(Name);
}


/********************************/
/*** IReqFilterSplitter class ***/
/********************************/

IReqFilterSplitter::IReqFilterSplitter(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name, MTS_OPTIONAL);
        if (required) {
            required->AddFunction("AddOutput", AddOutput);
        }
    }
}

IReqFilterSplitter::~IReqFilterSplitter()
{
    if (Owner) Owner->RemoveInterfaceRequired(Name);
}


/***********************************************/
/*** IReqFilterImageExposureCorrection class ***/
/***********************************************/

IReqFilterImageExposureCorrection::IReqFilterImageExposureCorrection(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name, MTS_OPTIONAL);
        if (required) {
            required->AddFunction("SetBrightness", SetBrightness);
            required->AddFunction("SetContrast",   SetContrast);
            required->AddFunction("SetGamma",      SetGamma);
            required->AddFunction("GetBrightness", GetBrightness);
            required->AddFunction("GetContrast",   GetContrast);
            required->AddFunction("GetGamma",      GetGamma);
        }
    }
}

IReqFilterImageExposureCorrection::~IReqFilterImageExposureCorrection()
{
    if (Owner) Owner->RemoveInterfaceRequired(Name);
}


/************************************/
/*** IReqFilterImageResizer class ***/
/************************************/

IReqFilterImageResizer::IReqFilterImageResizer(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name, MTS_OPTIONAL);
        if (required) {
            required->AddFunction("SetInterpolation",        SetInterpolation);
            required->AddFunction("SetOutputDimension",      SetOutputDimension);
            required->AddFunction("SetLeftOutputDimension",  SetLeftOutputDimension);
            required->AddFunction("SetRightOutputDimension", SetRightOutputDimension);
            required->AddFunction("SetOutputRatio",          SetOutputRatio);
            required->AddFunction("SetLeftOutputRatio",      SetLeftOutputRatio);
            required->AddFunction("SetRightOutputRatio",     SetRightOutputRatio);
            required->AddFunction("GetInterpolation",        GetInterpolation);
            required->AddFunction("GetLeftOutputDimension",  GetLeftOutputDimension);
            required->AddFunction("GetRightOutputDimension", GetRightOutputDimension);
            required->AddFunction("GetOutputRatio",          GetOutputRatio);
            required->AddFunction("GetLeftOutputRatio",      GetLeftOutputRatio);
            required->AddFunction("GetRightOutputRatio",     GetRightOutputRatio);
        }
    }
}

IReqFilterImageResizer::~IReqFilterImageResizer()
{
    if (Owner) Owner->RemoveInterfaceRequired(Name);
}


/****************************************/
/*** IReqFilterImageTranslation class ***/
/****************************************/

IReqFilterImageTranslation::IReqFilterImageTranslation(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name, MTS_OPTIONAL);
        if (required) {
            required->AddFunction("SetTranslation",      SetTranslation);
            required->AddFunction("SetLeftTranslation",  SetLeftTranslation);
            required->AddFunction("SetRightTranslation", SetRightTranslation);
            required->AddFunction("GetTranslation",      GetTranslation);
            required->AddFunction("GetLeftTranslation",  GetLeftTranslation);
            required->AddFunction("GetRightTranslation", GetRightTranslation);
        }
    }
}

IReqFilterImageTranslation::~IReqFilterImageTranslation()
{
    if (Owner) Owner->RemoveInterfaceRequired(Name);
}


/****************************************/
/*** IReqFilterLightSourceBuddy class ***/
/****************************************/

IReqFilterLightSourceBuddy::IReqFilterLightSourceBuddy(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name, MTS_OPTIONAL);
        if (required) {
            required->AddFunction("SetCalibration",  SetCalibration);
            required->AddFunction("SetLightBalance", SetLightBalance);
            required->AddFunction("GetCalibration",  GetCalibration);
            required->AddFunction("GetLightBalance", GetLightBalance);
        }
    }
}

IReqFilterLightSourceBuddy::~IReqFilterLightSourceBuddy()
{
    if (Owner) Owner->RemoveInterfaceRequired(Name);
}


/*****************************************/
/*** IReqFilterStereoImageJoiner class ***/
/*****************************************/

IReqFilterStereoImageJoiner::IReqFilterStereoImageJoiner(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name, MTS_OPTIONAL);
        if (required) {
            required->AddFunction("SetLayout", SetLayout);
            required->AddFunction("GetLayout", GetLayout);
        }
    }
}

IReqFilterStereoImageJoiner::~IReqFilterStereoImageJoiner()
{
    if (Owner) Owner->RemoveInterfaceRequired(Name);
}


/***********************************/
/*** IReqFilterImageWindow class ***/
/***********************************/

IReqFilterImageWindow::IReqFilterImageWindow(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name, MTS_OPTIONAL);
        if (required) {
            required->AddFunction("SetFullScreen",    SetFullScreen);
            required->AddFunction("SetTitle",         SetTitle);
            required->AddFunction("SetPosition",      SetPosition);
            required->AddFunction("SetLeftPosition",  SetLeftPosition);
            required->AddFunction("SetRightPosition", SetRightPosition);
            required->AddFunction("GetFullScreen",    GetFullScreen);
            required->AddFunction("GetTitle",         GetTitle);
            required->AddFunction("GetPosition",      GetPosition);
            required->AddFunction("GetLeftPosition",  GetLeftPosition);
            required->AddFunction("GetRightPosition", GetRightPosition);
        }
    }
}

IReqFilterImageWindow::~IReqFilterImageWindow()
{
    if (Owner) Owner->RemoveInterfaceRequired(Name);
}


/******************************/
/*** IReqVideoEncoder class ***/
/******************************/

IReqVideoEncoder::IReqVideoEncoder(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name, MTS_OPTIONAL);
        if (required) {
            required->AddFunction("SetExtension",              SetExtension);
            required->AddFunction("SetEncoderID",              SetEncoderID);
            required->AddFunction("SetCompressionLevel",       SetCompressionLevel);
            required->AddFunction("SetQualityBased",           SetQualityBased);
            required->AddFunction("SetTargetQuantizer",        SetTargetQuantizer);
            required->AddFunction("SetDatarate",               SetDatarate);
            required->AddFunction("SetKeyFrameEvery",          SetKeyFrameEvery);
            required->AddFunction("IsCompressionLevelEnabled", IsCompressionLevelEnabled);
            required->AddFunction("IsEncoderListEnabled",      IsEncoderListEnabled);
            required->AddFunction("IsTargetQuantizerEnabled",  IsTargetQuantizerEnabled);
            required->AddFunction("IsDatarateEnabled",         IsDatarateEnabled);
            required->AddFunction("IsKeyFrameEveryEnabled",    IsKeyFrameEveryEnabled);
            required->AddFunction("GetCompressionLevel",       GetCompressionLevel);
            required->AddFunction("GetEncoderList",            GetEncoderList);
            required->AddFunction("GetEncoderID",              GetEncoderID);
            required->AddFunction("GetQualityBased",           GetQualityBased);
            required->AddFunction("GetTargetQuantizer",        GetTargetQuantizer);
            required->AddFunction("GetDatarate",               GetDatarate);
            required->AddFunction("GetKeyFrameEvery",          GetKeyFrameEvery);
        }
    }
}

IReqVideoEncoder::~IReqVideoEncoder()
{
    if (Owner) Owner->RemoveInterfaceRequired(Name);
}


/************************************/
/*** IReqFilterImageOverlay class ***/
/************************************/

IReqFilterImageOverlay::IReqFilterImageOverlay(const std::string& name, mtsComponent* owner) :
    Name(name),
    Owner(owner)
{
    if (Owner) {
        mtsInterfaceRequired* required = Owner->AddInterfaceRequired(name, MTS_OPTIONAL);
        if (required) {
            required->AddFunction("SetTransform",  SetTransform);
            required->AddFunction("SetTransforms", SetTransforms);
        }
    }
}

IReqFilterImageOverlay::~IReqFilterImageOverlay()
{
    if (Owner) Owner->RemoveInterfaceRequired(Name);
}

