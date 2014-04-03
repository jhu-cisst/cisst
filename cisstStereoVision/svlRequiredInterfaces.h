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

#ifndef _svlRequiredInterfaces_h
#define _svlRequiredInterfaces_h

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctFixedSizeMatrixTypes.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionQualifiedRead.h>
#include <cisstMultiTask/mtsFunctionWrite.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

// Forward declarations
class mtsComponent;


/*********************************/
/*** SVL-MTS type declarations ***/
/*********************************/

#if 0   // Now defined in mtsGenericObjectProxy.h
typedef mtsGenericObjectProxy<float> svlFloat;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFloat);
typedef mtsGenericObjectProxy<vct2> svlDouble2;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlDouble2);
typedef mtsGenericObjectProxy<vct3> svlDouble3;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlDouble3);
typedef mtsGenericObjectProxy<vct3x3> svlDouble3x3;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlDouble3x3);
typedef mtsGenericObjectProxy<vctInt2> svlInt2;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlInt2);
#endif

/*******************************/
/*** IReqStreamManager class ***/
/*******************************/

class CISST_EXPORT IReqStreamManager
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
/*** IReqFilterSourceImageFile class ***/
/***************************************/

class CISST_EXPORT IReqFilterSourceImageFile
{
public:
    IReqFilterSourceImageFile(const std::string& name, mtsComponent* owner);
    ~IReqFilterSourceImageFile();

    // svlFilterSourceBase commands
    mtsFunctionWrite SetFramerate;
    mtsFunctionWrite SetLoop;
    mtsFunctionVoid  Pause;
    mtsFunctionVoid  Play;
    mtsFunctionWrite PlayFrames;

    // svlFilterSourceVideoFile commands
    mtsFunctionWrite SetChannels;
    mtsFunctionWrite SetFile;
    mtsFunctionRead  GetChannels;
    mtsFunctionRead  GetFile;
    mtsFunctionRead  GetDimensions;
    mtsFunctionRead  GetLeftDimensions;
    mtsFunctionRead  GetRightDimensions;

private:
    IReqFilterSourceImageFile();
    IReqFilterSourceImageFile(const IReqFilterSourceImageFile& instance);

    std::string Name;
    mtsComponent* Owner;
};


/**************************************/
/*** IReqFilterSourceTextFile class ***/
/**************************************/

class CISST_EXPORT IReqFilterSourceTextFile
{
public:
    IReqFilterSourceTextFile(const std::string& name, mtsComponent* owner);
    ~IReqFilterSourceTextFile();

    // svlFilterSourceBase commands
    mtsFunctionWrite SetFramerate;
    mtsFunctionWrite SetLoop;
    mtsFunctionVoid  Pause;
    mtsFunctionVoid  Play;
    mtsFunctionWrite PlayFrames;

    // svlFilterSourceVideoFile commands
    mtsFunctionWrite SetErrorValue;
    mtsFunctionWrite SetColumns;
    mtsFunctionRead  AddFile;
    mtsFunctionRead  GetErrorValue;
    mtsFunctionRead  GetColumns;
    mtsFunctionRead  RemoveFiles;

private:
    IReqFilterSourceTextFile();
    IReqFilterSourceTextFile(const IReqFilterSourceTextFile& instance);

    std::string Name;
    mtsComponent* Owner;
};


/******************************************/
/*** IReqFilterSourceVideoCapture class ***/
/******************************************/

class CISST_EXPORT IReqFilterSourceVideoCapture
{
public:
    IReqFilterSourceVideoCapture(const std::string& name, mtsComponent* owner);
    ~IReqFilterSourceVideoCapture();

    // svlFilterSourceBase commands
    mtsFunctionVoid  Pause;
    mtsFunctionVoid  Play;
    mtsFunctionWrite PlayFrames;

    // svlFilterSourceVideoCapture commands
    mtsFunctionVoid  EnumerateDevices;
    mtsFunctionRead  GetDeviceList;
    mtsFunctionRead  Get;
    mtsFunctionWrite Set;
    mtsFunctionWrite SetChannels;
    mtsFunctionWrite SetDevice;
    mtsFunctionWrite SetLeftDevice;
    mtsFunctionWrite SetRightDevice;
    mtsFunctionWrite SetInput;
    mtsFunctionWrite SetLeftInput;
    mtsFunctionWrite SetRightInput;
    mtsFunctionWrite SetFormat;
    mtsFunctionWrite SetLeftFormat;
    mtsFunctionWrite SetRightFormat;
    mtsFunctionWrite SelectFormat;
    mtsFunctionWrite SelectLeftFormat;
    mtsFunctionWrite SelectRightFormat;
    mtsFunctionWrite SetTrigger;
    mtsFunctionWrite SetLeftTrigger;
    mtsFunctionWrite SetRightTrigger;
    mtsFunctionWrite SetImageProperties;
    mtsFunctionWrite SetLeftImageProperties;
    mtsFunctionWrite SetRightImageProperties;
    mtsFunctionWrite SaveSettings;
    mtsFunctionWrite LoadSettings;
    mtsFunctionRead  GetChannels;
    mtsFunctionRead  GetDevice;
    mtsFunctionRead  GetLeftDevice;
    mtsFunctionRead  GetRightDevice;
    mtsFunctionRead  GetInput;
    mtsFunctionRead  GetLeftInput;
    mtsFunctionRead  GetRightInput;
    mtsFunctionRead  GetFormat;
    mtsFunctionRead  GetLeftFormat;
    mtsFunctionRead  GetRightFormat;
    mtsFunctionRead  GetDimensions;
    mtsFunctionRead  GetLeftDimensions;
    mtsFunctionRead  GetRightDimensions;
    mtsFunctionRead  GetFormatList;
    mtsFunctionRead  GetLeftFormatList;
    mtsFunctionRead  GetRightFormatList;
    mtsFunctionRead  GetTrigger;
    mtsFunctionRead  GetLeftTrigger;
    mtsFunctionRead  GetRightTrigger;
    mtsFunctionRead  GetImageProperties;
    mtsFunctionRead  GetLeftImageProperties;
    mtsFunctionRead  GetRightImageProperties;

private:
    IReqFilterSourceVideoCapture();
    IReqFilterSourceVideoCapture(const IReqFilterSourceVideoCapture& instance);

    std::string Name;
    mtsComponent* Owner;
};


/***************************************/
/*** IReqFilterSourceVideoFile class ***/
/***************************************/

class CISST_EXPORT IReqFilterSourceVideoFile
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
    mtsFunctionRead          GetChannels;
    mtsFunctionRead          GetFilename;
    mtsFunctionRead          GetLeftFilename;
    mtsFunctionRead          GetRightFilename;
    mtsFunctionRead          GetLength;
    mtsFunctionRead          GetLeftLength;
    mtsFunctionRead          GetRightLength;
    mtsFunctionRead          GetPosition;
    mtsFunctionRead          GetLeftPosition;
    mtsFunctionRead          GetRightPosition;
    mtsFunctionRead          GetRange;
    mtsFunctionRead          GetLeftRange;
    mtsFunctionRead          GetRightRange;
    mtsFunctionRead          GetDimensions;
    mtsFunctionRead          GetLeftDimensions;
    mtsFunctionRead          GetRightDimensions;
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


/********************************/
/*** IReqFilterSplitter class ***/
/********************************/

class CISST_EXPORT IReqFilterSplitter
{
public:
    IReqFilterSplitter(const std::string& name, mtsComponent* owner);
    ~IReqFilterSplitter();

    mtsFunctionWrite AddOutput;

private:
    IReqFilterSplitter();
    IReqFilterSplitter(const IReqFilterSplitter& instance);

    std::string Name;
    mtsComponent* Owner;
};


/***********************************************/
/*** IReqFilterImageExposureCorrection class ***/
/***********************************************/

class CISST_EXPORT IReqFilterImageExposureCorrection
{
public:
    IReqFilterImageExposureCorrection(const std::string& name, mtsComponent* owner);
    ~IReqFilterImageExposureCorrection();

    mtsFunctionWrite SetBrightness;
    mtsFunctionWrite SetContrast;
    mtsFunctionWrite SetGamma;
    mtsFunctionRead  GetBrightness;
    mtsFunctionRead  GetContrast;
    mtsFunctionRead  GetGamma;

private:
    IReqFilterImageExposureCorrection();
    IReqFilterImageExposureCorrection(const IReqFilterImageExposureCorrection& instance);

    std::string Name;
    mtsComponent* Owner;
};


/************************************/
/*** IReqFilterImageResizer class ***/
/************************************/

class CISST_EXPORT IReqFilterImageResizer
{
public:
    IReqFilterImageResizer(const std::string& name, mtsComponent* owner);
    ~IReqFilterImageResizer();

    mtsFunctionWrite SetInterpolation;
    mtsFunctionWrite SetOutputDimension;
    mtsFunctionWrite SetLeftOutputDimension;
    mtsFunctionWrite SetRightOutputDimension;
    mtsFunctionWrite SetOutputRatio;
    mtsFunctionWrite SetLeftOutputRatio;
    mtsFunctionWrite SetRightOutputRatio;
    mtsFunctionRead  GetInterpolation;
    mtsFunctionRead  GetOutputDimension;
    mtsFunctionRead  GetLeftOutputDimension;
    mtsFunctionRead  GetRightOutputDimension;
    mtsFunctionRead  GetOutputRatio;
    mtsFunctionRead  GetLeftOutputRatio;
    mtsFunctionRead  GetRightOutputRatio;

private:
    IReqFilterImageResizer();
    IReqFilterImageResizer(const IReqFilterImageResizer& instance);

    std::string Name;
    mtsComponent* Owner;
};


/****************************************/
/*** IReqFilterImageTranslation class ***/
/****************************************/

class CISST_EXPORT IReqFilterImageTranslation
{
public:
    IReqFilterImageTranslation(const std::string& name, mtsComponent* owner);
    ~IReqFilterImageTranslation();

    mtsFunctionWrite SetTranslation;
    mtsFunctionWrite SetLeftTranslation;
    mtsFunctionWrite SetRightTranslation;
    mtsFunctionRead  GetTranslation;
    mtsFunctionRead  GetLeftTranslation;
    mtsFunctionRead  GetRightTranslation;

private:
    IReqFilterImageTranslation();
    IReqFilterImageTranslation(const IReqFilterImageTranslation& instance);

    std::string Name;
    mtsComponent* Owner;
};


/****************************************/
/*** IReqFilterLightSourceBuddy class ***/
/****************************************/

class CISST_EXPORT IReqFilterLightSourceBuddy
{
public:
    IReqFilterLightSourceBuddy(const std::string& name, mtsComponent* owner);
    ~IReqFilterLightSourceBuddy();

    mtsFunctionWrite SetCalibration;
    mtsFunctionWrite SetLightBalance;
    mtsFunctionRead  GetCalibration;
    mtsFunctionRead  GetLightBalance;

private:
    IReqFilterLightSourceBuddy();
    IReqFilterLightSourceBuddy(const IReqFilterLightSourceBuddy& instance);

    std::string Name;
    mtsComponent* Owner;
};


/*****************************************/
/*** IReqFilterStereoImageJoiner class ***/
/*****************************************/

class CISST_EXPORT IReqFilterStereoImageJoiner
{
public:
    IReqFilterStereoImageJoiner(const std::string& name, mtsComponent* owner);
    ~IReqFilterStereoImageJoiner();

    mtsFunctionWrite SetLayout;
    mtsFunctionRead  GetLayout;

private:
    IReqFilterStereoImageJoiner();
    IReqFilterStereoImageJoiner(const IReqFilterStereoImageJoiner& instance);

    std::string Name;
    mtsComponent* Owner;
};


/***********************************/
/*** IReqFilterImageWindow class ***/
/***********************************/

class CISST_EXPORT IReqFilterImageWindow
{
public:
    IReqFilterImageWindow(const std::string& name, mtsComponent* owner);
    ~IReqFilterImageWindow();

    mtsFunctionWrite SetFullScreen;
    mtsFunctionWrite SetTitle;
    mtsFunctionWrite SetPosition;
    mtsFunctionWrite SetLeftPosition;
    mtsFunctionWrite SetRightPosition;
    mtsFunctionRead  GetFullScreen;
    mtsFunctionRead  GetTitle;
    mtsFunctionRead  GetPosition;
    mtsFunctionRead  GetLeftPosition;
    mtsFunctionRead  GetRightPosition;

private:
    IReqFilterImageWindow();
    IReqFilterImageWindow(const IReqFilterImageWindow& instance);

    std::string Name;
    mtsComponent* Owner;
};


/******************************/
/*** IReqVideoEncoder class ***/
/******************************/

class CISST_EXPORT IReqVideoEncoder
{
public:
    IReqVideoEncoder(const std::string& name, mtsComponent* owner);
    ~IReqVideoEncoder();

    mtsFunctionWrite SetExtension;
    mtsFunctionWrite SetEncoderID;
    mtsFunctionWrite SetCompressionLevel;
    mtsFunctionWrite SetQualityBased;
    mtsFunctionWrite SetTargetQuantizer;
    mtsFunctionWrite SetDatarate;
    mtsFunctionWrite SetKeyFrameEvery;
    mtsFunctionRead IsCompressionLevelEnabled;
    mtsFunctionRead IsEncoderListEnabled;
    mtsFunctionRead IsTargetQuantizerEnabled;
    mtsFunctionRead IsDatarateEnabled;
    mtsFunctionRead IsKeyFrameEveryEnabled;
    mtsFunctionRead GetCompressionLevel;
    mtsFunctionRead GetEncoderList;
    mtsFunctionRead GetEncoderID;
    mtsFunctionRead GetQualityBased;
    mtsFunctionRead GetTargetQuantizer;
    mtsFunctionRead GetDatarate;
    mtsFunctionRead GetKeyFrameEvery;

private:
    IReqVideoEncoder();
    IReqVideoEncoder(const IReqVideoEncoder& instance);

    std::string Name;
    mtsComponent* Owner;
};


/************************************/
/*** IReqFilterImageOverlay class ***/
/************************************/

class CISST_EXPORT IReqFilterImageOverlay
{
public:
    IReqFilterImageOverlay(const std::string& name, mtsComponent* owner);
    ~IReqFilterImageOverlay();

    mtsFunctionWrite SetTransform;
    mtsFunctionWrite SetTransforms;

private:
    IReqFilterImageOverlay();
    IReqFilterImageOverlay(const IReqFilterImageOverlay& instance);

    std::string Name;
    mtsComponent* Owner;
};


#endif // _svlRequiredInterfaces_h

