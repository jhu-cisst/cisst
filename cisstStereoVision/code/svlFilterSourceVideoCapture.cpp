/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi
  Created on: 2006

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstStereoVision/svlFilterSourceVideoCapture.h>
#include <cisstStereoVision/svlFilterOutput.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstCommon/cmnGetChar.h>

#include "svlVidCapSrcInitializer.h"

#ifdef _MSC_VER
    // Quick fix for Visual Studio Intellisense:
    // The Intellisense parser can't handle the CMN_UNUSED macro
    // correctly if defined in cmnPortability.h, thus
    // we should redefine it here for it.
    // Removing this part of the code will not effect compilation
    // in any way, on any platforms.
    #undef CMN_UNUSED
    #define CMN_UNUSED(argument) argument
#endif

#define MAX_PROPERTIES_BUFFER_SIZE      65536


/*************************************************/
/*** svlFilterSourceVideoCapture::Config class ***/
/*************************************************/

#if 0
svlFilterSourceVideoCapture::Config::Config() :
    Channels(0)
{
}

svlFilterSourceVideoCapture::Config::Config(const svlFilterSourceVideoCapture::Config& objref)
{
    SetChannels(objref.Channels);
    Device     = objref.Device;
    Input      = objref.Input;
    Format     = objref.Format;
    Properties = objref.Properties;
    Trigger    = objref.Trigger;
}

void svlFilterSourceVideoCapture::Config::SetChannels(const int channels)
{
    if (channels < 0) return;

    Device.SetSize(channels);
    Input.SetSize(channels);
    Format.SetSize(channels);
    Properties.SetSize(channels);
    Trigger.SetSize(channels);

    Channels = channels;
    Device.SetAll(-1);
    Input.SetAll(-1);
    for (int i = 0; i < channels; i ++) {
        memset(&(Format[i]),     0, sizeof(svlFilterSourceVideoCapture::ImageFormat));
        memset(&(Properties[i]), 0, sizeof(svlFilterSourceVideoCapture::ImageProperties));
        memset(&(Trigger[i]),    0, sizeof(svlFilterSourceVideoCapture::ExternalTrigger));
    }
}
#endif

/*******************************************/
/*** svlFilterSourceVideoCapture class *****/
/*******************************************/

CMN_IMPLEMENT_SERVICES_DERIVED(svlFilterSourceVideoCapture, svlFilterSourceBase)
CMN_IMPLEMENT_SERVICES(svlVidCapSrcBase)

CMN_IMPLEMENT_SERVICES_TEMPLATED(svlFilterSourceVideoCapture_DeviceList)
CMN_IMPLEMENT_SERVICES_TEMPLATED(svlFilterSourceVideoCapture_FormatList)

svlFilterSourceVideoCapture::svlFilterSourceVideoCapture() :
    svlFilterSourceBase(),
    OutputImage(0),
    EnumeratedDevices(0),
    NumberOfEnumeratedDevices(-1),
    FormatList(0),
    FormatListSize(0)
{
    CreateInterfaces();

    AddOutput("output", true);
    SetAutomaticOutputType(false);

    InitializeCaptureAPIs();
}

svlFilterSourceVideoCapture::svlFilterSourceVideoCapture(unsigned int channelcount) :
    svlFilterSourceBase(),
    OutputImage(0),
    EnumeratedDevices(0),
    NumberOfEnumeratedDevices(-1),
    FormatList(0),
    FormatListSize(0)
{
    CreateInterfaces();

    AddOutput("output", true);
    SetAutomaticOutputType(false);

    InitializeCaptureAPIs();

    SetChannelCount(channelcount);
}

svlFilterSourceVideoCapture::~svlFilterSourceVideoCapture()
{
    Release();

    if (OutputImage) {
        delete OutputImage;

        unsigned int i;

        if (DeviceID) delete [] DeviceID;
        if (InputID) delete [] InputID;
        if (Trigger) delete [] Trigger;
        if (Format) {
            for (i = 0; i < NumberOfChannels; i ++) {
                if (Format[i]) delete Format[i];
            }
            delete [] Format;
        }
        if (Properties) {
            for (i = 0; i < NumberOfChannels; i ++) {
                if (Properties[i]) delete Properties[i];
            }
            delete [] Properties;
        }
        if (DevSpecConfigBuffer) {
            for (i = 0; i < NumberOfChannels; i ++) {
                if (DevSpecConfigBuffer[i]) delete DevSpecConfigBuffer[i];
            }
            delete [] DevSpecConfigBuffer;
        }
        if (DevSpecConfigBufferSize) delete [] DevSpecConfigBufferSize;
        if (APIChannelID) delete [] APIChannelID;
        if (APIDeviceID) delete [] APIDeviceID;
        if (API) delete [] API;
        if (EnumeratedDevices) delete [] EnumeratedDevices;
        if (FormatList) {
            for (int j = 0; j < NumberOfEnumeratedDevices; j ++) {
                if (FormatList[j]) delete [] FormatList[j];
            }
            delete [] FormatList;
        }
        if (FormatListSize) delete [] FormatListSize;
    }
}

int svlFilterSourceVideoCapture::EnumerateDevices()
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    // First enumeration or update
    int i;
    unsigned int j, sum;
    cmnGenericObject* go;
    svlVidCapSrcBase* api;
    DeviceInfo **apideviceinfos = new DeviceInfo*[NumberOfSupportedAPIs];
    int *apidevicecounts = new int[NumberOfSupportedAPIs];
    ImageFormat ***apiformats = new ImageFormat**[NumberOfSupportedAPIs];
    int **apiformatcounts = new int*[NumberOfSupportedAPIs];

    // Release previous results, if any
    if (EnumeratedDevices) {
        delete [] EnumeratedDevices;
        EnumeratedDevices = 0;
    }
    if (FormatList) {
        for (i = 0; i < NumberOfEnumeratedDevices; i ++) {
            if (FormatList[i]) delete [] FormatList[i];
        }
        delete [] FormatList;
        FormatList = 0;
    }
    if (FormatListSize) {
        delete [] FormatListSize;
        FormatListSize = 0;
    }
    NumberOfEnumeratedDevices = 0;

    // Enumerate registered APIs and store all results
    for (j = 0; j < NumberOfSupportedAPIs; j ++) {

        // Most device objects can be created dynamically
        go = SupportedAPIs[j]->Create();

        if (go == 0) {
#if CISST_SVL_HAS_MIL
            // MIL device object is a singleton, cannot be created dynamically
			if (APIPlatforms[j] == svlFilterSourceVideoCaptureTypes::MatroxImaging) {
                go = svlVidCapSrcMIL::GetInstance();
            }
#endif // CISST_SVL_HAS_MIL
        }

        api = dynamic_cast<svlVidCapSrcBase*>(go);

        if (api) {
            apideviceinfos[j] = 0;
            apidevicecounts[j] = api->GetDeviceList(&(apideviceinfos[j]));
            if (apidevicecounts[j] > 0) {
                apiformats[j] = new ImageFormat*[apidevicecounts[j]];
                apiformatcounts[j] = new int[apidevicecounts[j]];
                for (i = 0; i < apidevicecounts[j]; i ++)
                    apiformatcounts[j][i] = api->GetFormatList(i, &(apiformats[j][i]));
            }
            if (apidevicecounts[j] > 0) NumberOfEnumeratedDevices += apidevicecounts[j];
        }

        // Delete method will release only dynamically created objects
        SupportedAPIs[j]->Delete(go);
    }

    if (NumberOfEnumeratedDevices > 0) {
        // Allocate the ONE device info array
        EnumeratedDevices = new DeviceInfo[NumberOfEnumeratedDevices];
        // Allocate and reset format array
        FormatList = new ImageFormat*[NumberOfEnumeratedDevices];
        FormatListSize = new int[NumberOfEnumeratedDevices];
        memset(FormatList, 0, NumberOfEnumeratedDevices * sizeof(ImageFormat*));
        memset(FormatListSize, 0, NumberOfEnumeratedDevices * sizeof(int));

        // Copy API specific device info arrays into the ONE array
        sum = 0;
        for (j = 0; j < NumberOfSupportedAPIs; j ++) {
            if (apideviceinfos[j] && apidevicecounts[j] > 0) {

                // Store formats
                for (i = 0; i < apidevicecounts[j]; i ++) {
                    if (apiformatcounts[j][i] > 0) {
                        FormatList[sum + i] = apiformats[j][i];
                        FormatListSize[sum + i] = apiformatcounts[j][i];
                    }
                }
                delete [] apiformats[j];
                delete [] apiformatcounts[j];

                // Store device infos
                memcpy(&(EnumeratedDevices[sum]), apideviceinfos[j], apidevicecounts[j] * sizeof(DeviceInfo));

                sum += apidevicecounts[j];
            }
        }
    }

    // Free up allocated resources
    for (j = 0; j < NumberOfSupportedAPIs; j ++) {
        if (apideviceinfos[j]) delete [] apideviceinfos[j];
    }
    delete [] apideviceinfos;
    delete [] apidevicecounts;
    delete [] apiformats;
    delete [] apiformatcounts;

    return NumberOfEnumeratedDevices;
}

int svlFilterSourceVideoCapture::SetChannelCount(unsigned int channelcount)
{
    if (OutputImage) return SVL_FAIL;

    if (channelcount == 1) {
        GetOutput()->SetType(svlTypeImageRGB);
        // forcing output sample to handle external buffers
        OutputImage = new svlSampleImageRGB(false);
    }
    else if (channelcount == 2) {
        GetOutput()->SetType(svlTypeImageRGBStereo);
        // forcing output sample to handle external buffers
        OutputImage = new svlSampleImageRGBStereo(false);
    }
    else return SVL_FAIL;

    NumberOfChannels = channelcount;

    DeviceID = new int[NumberOfChannels];
    InputID = new int[NumberOfChannels];
    Format = new ImageFormat*[NumberOfChannels];
    Properties = new ImageProperties*[NumberOfChannels];
    Trigger = new ExternalTrigger[NumberOfChannels];
    DevSpecConfigBuffer = new unsigned char*[NumberOfChannels];
    DevSpecConfigBufferSize = new unsigned int[NumberOfChannels];
    APIChannelID = new int[NumberOfChannels];
    APIDeviceID = new int[NumberOfChannels];
    API = new int[NumberOfChannels];

    for (unsigned int i = 0; i < NumberOfChannels; i ++) {
        DeviceID[i] = -1;
        InputID[i] = -1;
        Format[i] = 0;
        Properties[i] = 0;
        memset(&(Trigger[i]), 0, sizeof(ExternalTrigger));
        DevSpecConfigBuffer[i] = 0;
        DevSpecConfigBufferSize[i] = 0;
        APIChannelID[i] = -1;
        APIDeviceID[i] = -1;
        API[i] = -1;
    }

    return SVL_OK;
}

int svlFilterSourceVideoCapture::Initialize(svlSample* &syncOutput)
{
    if (OutputImage == 0) return SVL_FAIL;
    syncOutput = OutputImage;

    PlatformType platform;
    unsigned int i;
    int ret = SVL_FAIL;

    Release();

    // Create capture API handlers
    if (CreateCaptureAPIHandlers() != SVL_OK) {
        ret = SVL_FAIL;
        goto labError;
    }

    // Set capture devices
    for (i = 0; i < NumberOfChannels; i ++) {
        if (DeviceObj[API[i]]->SetDevice(APIDeviceID[i], InputID[i], APIChannelID[i]) != SVL_OK) {
            ret = SVL_VCS_UNABLE_TO_SET_INPUT;
            goto labError;
        }
    }

    // Set format if available
    for (i = 0; i < NumberOfChannels; i ++) {
        platform = DeviceObj[API[i]]->GetPlatformType();

        if (platform == svlFilterSourceVideoCaptureTypes::WinDirectShow) {
#if CISST_SVL_HAS_DIRECTSHOW
            // DirectShow does not use the Format structure.
            // Instead, it has its own custom configuration format.
            if (DevSpecConfigBuffer[i] && DevSpecConfigBufferSize[i] > 0) {
                dynamic_cast<svlVidCapSrcDirectShow*>(DeviceObj[API[i]])->SetMediaType(DevSpecConfigBuffer[i], DevSpecConfigBufferSize[i], APIChannelID[i]);
            }
#endif // CISST_SVL_HAS_DIRECTSHOW
        }
#if CISST_SVL_HAS_MIL
        else if (platform == svlFilterSourceVideoCaptureTypes::MatroxImaging) {
            // Check if Matrox device supports capture
            if (dynamic_cast<svlVidCapSrcMIL*>(DeviceObj[API[i]])->IsCaptureSupported(APIDeviceID[i], InputID[i]) == false) {
                ret = SVL_VCS_UNABLE_TO_OPEN;
                goto labError;
            }
        }
#endif // CISST_SVL_HAS_MIL
        else {
            if (Format[i]) DeviceObj[API[i]]->SetFormat(Format[i][0], APIChannelID[i]);
        }
    }

    // Set trigger
    for (i = 0; i < NumberOfChannels; i ++) {
        DeviceObj[API[i]]->SetTrigger(Trigger[i], APIChannelID[i]);
    }

    // Open devices
    for (i = 0; i < NumberOfChannels; i ++) {
        if (DeviceObj[API[i]]->Open() != SVL_OK) {
            ret = SVL_VCS_UNABLE_TO_OPEN;
            goto labError;
        }
    }

    // Set image properties if available
    for (i = 0; i < NumberOfChannels; i ++) {
        platform = DeviceObj[API[i]]->GetPlatformType();

        if (platform == svlFilterSourceVideoCaptureTypes::WinDirectShow) {
#if CISST_SVL_HAS_DIRECTSHOW
            // DirectShow does not use the Properties structure.
            // Instead, it encodes image preperties into the custom
            // configuration format which was already set above.
#endif // CISST_SVL_HAS_DIRECTSHOW
        }
        else {
            if (Properties[i]) DeviceObj[API[i]]->SetImageProperties(Properties[i][0], APIChannelID[i]);
        }
    }

    // Requesting frames from the capture buffer to prepare output sample
    for (i = 0; i < NumberOfChannels; i ++) {
        if (NumberOfChannels == 2) {
            dynamic_cast<svlSampleImageRGBStereo*>(OutputImage)->SetMatrix(*(DeviceObj[API[i]]->GetLatestFrame(false, APIChannelID[i])), i);
        }
        else {
            dynamic_cast<svlSampleImageRGB*>(OutputImage)->SetMatrix(*(DeviceObj[API[i]]->GetLatestFrame(false)));
        }
    }

    // Start capturing in the background
    for (i = 0; i < NumberOfChannels; i ++) {
        if (DeviceObj[API[i]]->Start() != SVL_OK) {
            ret = SVL_VCS_UNABLE_TO_START_CAPTURE;
            goto labError;
        }
    }

    return SVL_OK;

labError:
    Release();
    return ret;
}

int svlFilterSourceVideoCapture::Process(svlProcInfo* procInfo, svlSample* &syncOutput)
{
    syncOutput = OutputImage;

    svlImageRGB* image;
    unsigned int idx;

    _ParallelLoop(procInfo, idx, NumberOfChannels)
    {
        // Requesting frame from the capture buffer
        image = DeviceObj[API[idx]]->GetLatestFrame(true, APIChannelID[idx]);
        if (image == 0) return SVL_FAIL;
        if (NumberOfChannels == 1) {
            dynamic_cast<svlSampleImageRGB*>(OutputImage)->SetMatrix(*image, idx);
        }
        else {
            dynamic_cast<svlSampleImageRGBStereo*>(OutputImage)->SetMatrix(*image, idx);
        }
    }

    return SVL_OK;
}

int svlFilterSourceVideoCapture::Release()
{
    for (unsigned int i = 0; i < NumberOfSupportedAPIs; i ++) {

#if CISST_SVL_HAS_MIL
        // MIL device object is a singleton, should not be deleted
        if (DeviceObj[i] &&
            DeviceObj[i]->GetPlatformType() == svlFilterSourceVideoCaptureTypes::MatroxImaging) {
            dynamic_cast<svlVidCapSrcMIL*>(DeviceObj[i])->Release();
            continue;
        }
#endif // CISST_SVL_HAS_MIL

        // Device object is released through the generic base class
        SupportedAPIs[i]->Delete(DeviceGenObj[i]);
        DeviceGenObj[i] = 0;
        DeviceObj[i] = 0;
    }

    return SVL_OK;
}

void svlFilterSourceVideoCapture::InitializeCaptureAPIs()
{
    NumberOfSupportedAPIs = 0;

    // Enumerate registered APIs
    cmnGenericObject* go;
    svlVidCapSrcBase* api;
    SupportedAPIs.SetSize(256);
    APIPlatforms.SetSize(256);

    // Go through all registered classes
    for (cmnClassRegister::const_iterator iter = cmnClassRegister::begin();
         iter != cmnClassRegister::end();
         iter ++) {

        // Avoid infinite recursion by skipping self
        if ((*iter).first != "svlFilterSourceVideoCapture" &&
            (*iter).second && (*iter).second->IsDerivedFrom<svlVidCapSrcBase>()) {

            // Most device objects can be created dynamically
            go = (*iter).second->Create();

            if (go == 0) {
#if CISST_SVL_HAS_MIL
                // MIL device object is a singleton, cannot be created dynamically
                if ((*iter).first == "svlVidCapSrcMIL") {
                    go = svlVidCapSrcMIL::GetInstance();
                }
#endif // CISST_SVL_HAS_MIL
            }

            api = dynamic_cast<svlVidCapSrcBase*>(go);

            if (api) {
                SupportedAPIs[NumberOfSupportedAPIs] = (*iter).second;
                APIPlatforms[NumberOfSupportedAPIs] = api->GetPlatformType();
                NumberOfSupportedAPIs ++;
            }

            // Delete method will release only dynamically created objects
            (*iter).second->Delete(go);
        }
    }

    SupportedAPIs.resize(NumberOfSupportedAPIs);
    APIPlatforms.resize(NumberOfSupportedAPIs);

    DeviceGenObj.resize(NumberOfSupportedAPIs);
    DeviceObj.resize(NumberOfSupportedAPIs);
    DeviceGenObj.SetAll(0);
    DeviceObj.SetAll(0);
}

int svlFilterSourceVideoCapture::CreateCaptureAPIHandlers()
{
    int ret = SVL_FAIL;
    unsigned int i, j;
    unsigned int *chperapi = new unsigned int[NumberOfSupportedAPIs];
    for (j = 0; j < NumberOfSupportedAPIs; j ++) chperapi[j] = 0;

    // Make sure devices are enumerated
    if (NumberOfEnumeratedDevices < 1) EnumerateDevices();

    // Count the number of video channels requested from capture APIs
    // and create API look up table for easy device handler access
    for (i = 0; i < NumberOfChannels; i ++) {

        if (DeviceID[i] < 0 || DeviceID[i] >= NumberOfEnumeratedDevices) goto labError;

        // Enumerate registered APIs and store all results
        for (j = 0; j < NumberOfSupportedAPIs; j ++) {

            if (EnumeratedDevices[DeviceID[i]].platform == APIPlatforms[j]) {
                // getting API specific device and channel IDs
                APIDeviceID[i] = EnumeratedDevices[DeviceID[i]].ID;
                APIChannelID[i] = chperapi[j];

                API[i] = j;
                chperapi[j] ++;
            }
        }
    }

    // Instantiate capture device handlers and
    // set the number of channels requested
    for (j = 0; j < NumberOfSupportedAPIs; j ++) {

        if (chperapi[j] > 0) {

            // Most device objects can be created dynamically
            DeviceGenObj[j] = SupportedAPIs[j]->Create();

            if (DeviceGenObj[j] == 0) {
#if CISST_SVL_HAS_MIL
                // MIL device object is a singleton, cannot be created dynamically
                if (APIPlatforms[j] == svlFilterSourceVideoCaptureTypes::MatroxImaging) {
                    DeviceGenObj[j] = svlVidCapSrcMIL::GetInstance();
                }
#endif // CISST_SVL_HAS_MIL
            }

            DeviceObj[j] = dynamic_cast<svlVidCapSrcBase*>(DeviceGenObj[j]);

            if (DeviceObj[j] == 0 ||
                DeviceObj[j]->SetStreamCount(chperapi[j]) != SVL_OK) goto labError;
        }
    }

    ret = SVL_OK;

labError:
    if (ret != SVL_OK) {
        for (j = 0; j < NumberOfSupportedAPIs; j ++) {
            // Device object is released through the generic base class
            SupportedAPIs[j]->Delete(DeviceGenObj[j]);
            DeviceGenObj[j] = 0;
            DeviceObj[j] = 0;
        }
    }
    if (chperapi) delete [] chperapi;

    return ret;
}

void svlFilterSourceVideoCapture::SetTargetFrequency(const double & CMN_UNUSED(hertz))
{
}

double svlFilterSourceVideoCapture::GetTargetFrequency() const
{
    return -1.0;
}

int svlFilterSourceVideoCapture::DialogSetup(unsigned int videoch)
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    int deviceid, inputid;

    std::cout << " === Capture device selection ===" << std::endl;
    deviceid = DialogDevice();
    if (deviceid < 0) return SVL_FAIL;

    std::cout << std::endl << "  ==== Device input selection ====" << std::endl;
    inputid = DialogInput(deviceid);

    SetDevice(deviceid, inputid, videoch);

    std::cout << std::endl << "  ===== Setup capture format =====" << std::endl;
    DialogFormat(videoch);

    if (EnumeratedDevices[DeviceID[videoch]].platform == svlFilterSourceVideoCaptureTypes::LinLibDC1394) {
#if CISST_SVL_HAS_DC1394
        std::cout << std::endl << "  ===== Setup external trigger =====" << std::endl;
        DialogTrigger(videoch);
        std::cout << std::endl;
#endif // CISST_SVL_HAS_DC1394
    }
    else {
        // External trigger is not supported in other APIs.
    }

    return SVL_OK;
}

int svlFilterSourceVideoCapture::DialogDevice()
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    int deviceid, listsize;

    listsize = PrintDeviceList();
    if (listsize < 1) {
        std::cout << " -!- No video capture devices have been found." << std::endl;
        return SVL_FAIL;
    }

    std::cout << std::endl << " # Enter device ID: ";
    std::cin >> deviceid;
    std::cin.ignore();
    if (deviceid < 0) deviceid = 0;
    if (deviceid >= listsize) deviceid = listsize - 1;

    return deviceid;
}

int svlFilterSourceVideoCapture::DialogInput(unsigned int deviceid)
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    int inputid, listsize;

    listsize = PrintInputList(deviceid);
    if (listsize > 0) {
        std::cout << std::endl << "  # Enter input ID: ";
        std::cin >> inputid;
        std::cin.ignore();
        if (inputid < 0) inputid = 0;
        if (inputid >= listsize) inputid = listsize - 1;
    }
    else {
        std::cout << "  -!- Input selection not available." << std::endl;
        inputid = 0;
    }

    return inputid;
}

int svlFilterSourceVideoCapture::DialogFormat(unsigned int videoch)
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;
    if (DeviceID[videoch] < 0)
        return SVL_VCS_UNABLE_TO_OPEN;

    // Make sure devices are enumerated
    if (NumberOfEnumeratedDevices < 1) EnumerateDevices();

    // Check if device ID is in range
    if (DeviceID[videoch] >= NumberOfEnumeratedDevices)
        return SVL_VCS_UNABLE_TO_OPEN;

    // Get capture API platform
    PlatformType platform = EnumeratedDevices[DeviceID[videoch]].platform;

    if (platform == svlFilterSourceVideoCaptureTypes::WinDirectShow) {
#if CISST_SVL_HAS_DIRECTSHOW
        // Create temporary DirectShow capture module and initialize it
        svlVidCapSrcDirectShow device;
        device.SetStreamCount(1);
        if (device.SetDevice(EnumeratedDevices[DeviceID[videoch]].ID,
                             InputID[videoch]) != SVL_OK) return SVL_VCS_UNABLE_TO_OPEN;
        // If device properties are already available, load them up to the device
        if (DevSpecConfigBuffer[videoch] && DevSpecConfigBufferSize[videoch] > 0) {
            device.SetMediaType(DevSpecConfigBuffer[videoch], DevSpecConfigBufferSize[videoch]);
        }
        // Open device
        if (device.Open() != SVL_OK) return SVL_VCS_UNABLE_TO_OPEN;
        // Open Format OLE Dialog and save new parameters
        if (device.ShowFormatDialog(0) == SVL_OK) {
            device.RequestMediaType();
            if (DevSpecConfigBuffer[videoch]) delete [] DevSpecConfigBuffer[videoch];
            DevSpecConfigBuffer[videoch] = 0;
            DevSpecConfigBufferSize[videoch] = 0;
            device.GetMediaType(DevSpecConfigBuffer[videoch], DevSpecConfigBufferSize[videoch]);
        }
        else return SVL_FAIL;
#endif // CISST_SVL_HAS_DIRECTSHOW
    }
    else {
        // No GUI available for other APIs.
        // Ask in command line.

        ImageFormat *formats = 0;
        int i, formatid, formatcount;

        formatcount = PrintFormatList(videoch);
        if (formatcount > 0) {
            std::cout << std::endl << "  # Enter format ID: ";
            std::cin >> formatid;
            std::cin.ignore();
            if (formatid < 0 || formatid >= formatcount) {
                std::cout << "  -!- Invalid format ID" << std::endl;
                return SVL_FAIL;
            }

            GetFormatList(&formats, videoch);
            if (formats[formatid].custom_mode < 0) {
                SelectFormat(formatid, videoch);
            }
            else {
                char input[256];
                int roiwidth, roiheight, roileft, roitop, framerate, colorspace, defaultval;

                defaultval = formats[formatid].width;
                std::cout << "  # Enter ROI width (max=" << formats[formatid].custom_maxwidth
                                            << "; unit=" << formats[formatid].custom_unitwidth
                                         << "; default=" << defaultval << "): ";
                std::cin.getline(input, 256);
                if (std::cin.gcount() > 1) roiwidth = atoi(input);
                else roiwidth = defaultval;
                std::cout << "    ROI width = " << roiwidth << std::endl;
                if (roiwidth < 1 || roiwidth > static_cast<int>(formats[formatid].custom_maxwidth)) {
                    std::cout << "  -!- Invalid ROI width" << std::endl;
                    return SVL_FAIL;
                }

                defaultval = formats[formatid].height;
                std::cout << "  # Enter ROI height (max=" << formats[formatid].custom_maxheight
                                             << "; unit=" << formats[formatid].custom_unitheight
                                          << "; default=" << defaultval << "): ";
                std::cin.getline(input, 256);
                if (std::cin.gcount() > 1) roiheight = atoi(input);
                else roiheight = defaultval;
                std::cout << "    ROI height = " << roiheight << std::endl;
                if (roiheight < 1 || roiheight > static_cast<int>(formats[formatid].custom_maxheight)) {
                    std::cout << "  -!- Invalid ROI height" << std::endl;
                    return SVL_FAIL;
                }

                defaultval = (formats[formatid].custom_maxwidth - roiwidth) / 2;
                std::cout << "  # Enter ROI left (max=" << formats[formatid].custom_maxwidth - 1
                                           << "; unit=" << formats[formatid].custom_unitleft
                                        << "; default=" << defaultval << "): ";
                std::cin.getline(input, 256);
                if (std::cin.gcount() > 1) roileft = atoi(input);
                else roileft = defaultval;
                std::cout << "    ROI left = " << roileft << std::endl;
                if (roileft < 0 || roileft > (static_cast<int>(formats[formatid].custom_maxwidth) - 1)) {
                    std::cout << "  -!- Invalid ROI left" << std::endl;
                    return SVL_FAIL;
                }

                defaultval = (formats[formatid].custom_maxheight - roiheight) / 2;
                std::cout << "  # Enter ROI top (max=" << formats[formatid].custom_maxheight - 1
                                          << "; unit=" << formats[formatid].custom_unittop
                                       << "; default=" << defaultval << "): ";
                std::cin.getline(input, 256);
                if (std::cin.gcount() > 1) roitop = atoi(input);
                else roitop = defaultval;
                std::cout << "    ROI top = " << roitop << std::endl;
                if (roitop < 0 || roitop > (static_cast<int>(formats[formatid].custom_maxheight) - 1)) {
                    std::cout << "  -!- Invalid ROI top" << std::endl;
                    return SVL_FAIL;
                }

                std::cout << "  # Enter framerate [percentage of maximum available framerate] (min=1; max=100; default=100): ";
                std::cin.getline(input, 256);
                if (std::cin.gcount() > 1) framerate = atoi(input);
                else framerate = 100;
                std::cout << "    Framerate = " << framerate << std::endl;
                if (framerate < 1 || framerate > 100) {
                    std::cout << "  -!- Invalid framerate" << std::endl;
                    return SVL_FAIL;
                }

                defaultval = 0;
                std::cout << "  == Select color space ==" << std::endl;
                for (i = 0; i < svlFilterSourceVideoCaptureTypes::PixelTypeCount
                         && formats[formatid].custom_colorspaces[i] != svlFilterSourceVideoCaptureTypes::PixelUnknown; i ++) {
                    std::cout << "  " << i << ") " << GetPixelTypeName(formats[formatid].custom_colorspaces[i]) << std::endl;
                    if (formats[formatid].custom_colorspaces[i] == svlFilterSourceVideoCaptureTypes::PixelRAW8) defaultval = i;
                }
                std::cout << "  # Enter color space ID";
                if (defaultval >= 0) std::cout << " (default=" << GetPixelTypeName(formats[formatid].custom_colorspaces[defaultval]) << ")";
                std::cout << ": ";
                std::cin.getline(input, 256);
                if (std::cin.gcount() > 1) colorspace = atoi(input);
                else colorspace = defaultval;
                std::cout << "    Color space = " << GetPixelTypeName(formats[formatid].custom_colorspaces[colorspace]) << std::endl;
                if (colorspace < 0 || colorspace >= i) {
                    std::cout << "  -!- Invalid color space" << std::endl;
                    return SVL_FAIL;
                }

                ImageFormat format;
                memcpy(&format, &(formats[formatid]), sizeof(ImageFormat));
                format.width = roiwidth;
                format.height = roiheight;
                format.colorspace = formats[formatid].custom_colorspaces[colorspace];
                format.custom_roileft= roileft;
                format.custom_roitop = roitop;
                format.custom_framerate = framerate;
                SetFormat(format, videoch);
            }

            ReleaseFormatList(formats);
        }
        else {
            std::cout << "  -!- Format selection not available." << std::endl;
        }
    }

    return SVL_OK;
}

int svlFilterSourceVideoCapture::DialogTrigger(unsigned int videoch)
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;
    if (DeviceID[videoch] < 0)
        return SVL_VCS_UNABLE_TO_OPEN;

    // Make sure devices are enumerated
    if (NumberOfEnumeratedDevices < 1) EnumerateDevices();

    // Check if device ID is in range
    if (DeviceID[videoch] >= NumberOfEnumeratedDevices)
        return SVL_VCS_UNABLE_TO_OPEN;

    if (EnumeratedDevices[DeviceID[videoch]].platform == svlFilterSourceVideoCaptureTypes::LinLibDC1394) {
        ExternalTrigger trigger;
        int ivalue;
        char input[256];
        std::string str;

        std::cout << "  # Enable external trigger? ['y' - YES; other - NO]: ";
        ivalue = cmnGetChar();
        if (ivalue != 'y') {
            memset(&trigger, 0, sizeof(ExternalTrigger));
            std::cout << "NO" << std::endl;
            SetTrigger(trigger, videoch);
            return SVL_OK;
        }
        trigger.enable = true;
        std::cout << "YES" << std::endl;

        std::cout << "  # Enter trigger mode ['0'-'5' or '14'-'15']: ";
        std::cin.getline(input, 256);
        if (std::cin.gcount() > 1) ivalue = atoi(input);
        else ivalue = 0;
        if (ivalue < 0 || (ivalue > 5 && (ivalue != 14 || ivalue != 15))) ivalue = 0;
        trigger.mode = ivalue;

        std::cout << "  # Enter trigger source ['0'-'3']: ";
        std::cin.getline(input, 256);
        if (std::cin.gcount() > 1) ivalue = atoi(input);
        else ivalue = 0;
        if (ivalue < 0 || ivalue > 3) ivalue = 0;
        trigger.source = ivalue;

        std::cout << "  # Enter trigger polarity ['h' - HIGH; other - LOW]: ";
        ivalue = cmnGetChar();
        if (ivalue == 'h') {
            trigger.polarity = 1;
            std::cout << "HIGH" << std::endl;
        }
        else {
            trigger.polarity = 0;
            std::cout << "LOW" << std::endl;
        }

        SetTrigger(trigger, videoch);
    }
    else {
        std::cout << "  -!- External trigger not supported." << std::endl;
    }

    return SVL_OK;
}

int svlFilterSourceVideoCapture::DialogImageProperties(unsigned int videoch)
{
    // Available only after initialization
    if (IsInitialized() == false)
        return SVL_NOT_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    // Get capture API platform and API specific device ID
    PlatformType platform = DeviceObj[API[videoch]]->GetPlatformType();

    if (platform == svlFilterSourceVideoCaptureTypes::WinDirectShow) {
#if CISST_SVL_HAS_DIRECTSHOW
        svlVidCapSrcDirectShow* device = dynamic_cast<svlVidCapSrcDirectShow*>(DeviceObj[API[videoch]]);
        if (device->ShowImageDialog(0, APIChannelID[videoch]) == SVL_OK) {
            // Store, whatever changes have been made
            device->RequestMediaType();
            if (DevSpecConfigBuffer[videoch]) delete [] DevSpecConfigBuffer[videoch];
            DevSpecConfigBuffer[videoch] = 0;
            DevSpecConfigBufferSize[videoch] = 0;
            device->GetMediaType(DevSpecConfigBuffer[videoch], DevSpecConfigBufferSize[videoch]);
        }
#endif // CISST_SVL_HAS_DIRECTSHOW
    }
    else {
        // No GUI available for other APIs
        // Ask in command line.

        printf("  ___________________________________________________________________________\r\n");
        printf("  IMAGE PROPERTIES (channel #%d)                         [press 'q' when done]\r\n", videoch);
        printf("   _________________________________________________________________________\r\n");
        printf("  | Shttr[!] | Gain[@] | WhiteBalance[#] | Brghtns[$] | Gamma[%%] | Satur[^] |\r\n");
        printf("  |    <1 2> |   <3 4> |  <5 6> |  <7 8> |      <9 0> |    <i o> |    <k l> |\r\n");

        int ret = SVL_FAIL;
        svlVidCapSrcDialogThread* proc;
        osaThread* thread;

        proc = new svlVidCapSrcDialogThread(videoch);
        thread = new osaThread;
        thread->Create<svlVidCapSrcDialogThread, svlFilterSourceVideoCapture*>(proc,
                                                                               &svlVidCapSrcDialogThread::Proc,
                                                                               this);
        if (proc->WaitForInit()) {
            GetImageProperties(videoch);

            int ch;
            do {
                ch = cmnGetChar();
                switch (ch) {
                    case '!':
                        Properties[videoch]->manual = ~(Properties[videoch]->manual);
                        Properties[videoch]->mask = propShutter;
                    break;

                    case '@':
                        Properties[videoch]->manual = ~(Properties[videoch]->manual);
                        Properties[videoch]->mask = propGain;
                    break;

                    case '#':
                        Properties[videoch]->manual = ~(Properties[videoch]->manual);
                        Properties[videoch]->mask = propWhiteBalance;
                    break;

                    case '$':
                        Properties[videoch]->manual = ~(Properties[videoch]->manual);
                        Properties[videoch]->mask = propBrightness;
                    break;

                    case '%':
                        Properties[videoch]->manual = ~(Properties[videoch]->manual);
                        Properties[videoch]->mask = propGamma;
                    break;

                    case '^':
                        Properties[videoch]->manual = ~(Properties[videoch]->manual);
                        Properties[videoch]->mask = propSaturation;
                    break;

                    case '1':
                        if (Properties[videoch]->shutter >= 10) Properties[videoch]->shutter -= 10;
                        else Properties[videoch]->shutter = 0;
                        Properties[videoch]->mask = propShutter;
                    break;

                    case '2':
                        Properties[videoch]->shutter += 10;
                        Properties[videoch]->mask = propShutter;
                    break;

                    case '3':
                        if (Properties[videoch]->gain >= 5) Properties[videoch]->gain -= 5;
                        else Properties[videoch]->gain = 0;
                        Properties[videoch]->mask = propGain;
                    break;

                    case '4':
                        Properties[videoch]->gain += 5;
                        Properties[videoch]->mask = propGain;
                    break;

                    case '5':
                        if (Properties[videoch]->wb_u_b >= 5) Properties[videoch]->wb_u_b -= 5;
                        else Properties[videoch]->wb_u_b = 0;
                        Properties[videoch]->mask = propWhiteBalance;
                    break;

                    case '6':
                        Properties[videoch]->wb_u_b += 5;
                        Properties[videoch]->mask = propWhiteBalance;
                    break;

                    case '7':
                        if (Properties[videoch]->wb_v_r >= 5) Properties[videoch]->wb_v_r -= 5;
                        else Properties[videoch]->wb_v_r = 0;
                        Properties[videoch]->mask = propWhiteBalance;
                    break;

                    case '8':
                        Properties[videoch]->wb_v_r += 5;
                        Properties[videoch]->mask = propWhiteBalance;
                    break;

                    case '9':
                        if (Properties[videoch]->brightness >= 5) Properties[videoch]->brightness -= 5;
                        else Properties[videoch]->brightness = 0;
                        Properties[videoch]->mask = propBrightness;
                    break;

                    case '0':
                        Properties[videoch]->brightness += 5;
                        Properties[videoch]->mask = propBrightness;
                    break;

                    case 'i':
                        if (Properties[videoch]->gamma >= 1) Properties[videoch]->gamma -= 1;
                        else Properties[videoch]->gamma = 0;
                        Properties[videoch]->mask = propGamma;
                    break;

                    case 'o':
                        Properties[videoch]->gamma += 1;
                        Properties[videoch]->mask = propGamma;
                    break;

                    case 'k':
                        if (Properties[videoch]->saturation >= 5) Properties[videoch]->saturation -= 5;
                        else Properties[videoch]->saturation = 0;
                        Properties[videoch]->mask = propSaturation;
                    break;

                    case 'l':
                        Properties[videoch]->saturation += 5;
                        Properties[videoch]->mask = propSaturation;
                    break;

                    default:
                    break;
                }

                SetImageProperties(videoch);
                osaSleep(0.08);
            } while (ch != 'q');
        }

        proc->Kill();
        thread->Wait();
        delete thread;
        delete proc;

        return ret;
    }

    return SVL_OK;
}

int svlFilterSourceVideoCapture::GetDeviceList(DeviceInfo **deviceinfolist) const
{
    if (OutputImage == 0)
        return SVL_FAIL;

    if (deviceinfolist && NumberOfEnumeratedDevices > 0) {
        deviceinfolist[0] = new DeviceInfo[NumberOfEnumeratedDevices];
        // The caller is responsible for deleting the deviceinfolist array after calling
        memcpy(deviceinfolist[0], EnumeratedDevices, NumberOfEnumeratedDevices * sizeof(DeviceInfo));
    }

    return NumberOfEnumeratedDevices;
}

void svlFilterSourceVideoCapture::ReleaseDeviceList(DeviceInfo *deviceinfolist) const
{
    if (deviceinfolist) delete [] deviceinfolist;
}

int svlFilterSourceVideoCapture::PrintDeviceList(bool update)
{
    if (OutputImage == 0)
        return SVL_FAIL;

    if (NumberOfEnumeratedDevices < 1 || update) EnumerateDevices();

    DeviceInfo *devices = 0;
    int devicecount = GetDeviceList(&devices);

    for (int i = 0; i < devicecount; i ++) {
        std::cout << " " << i << ") " << devices[i].name << std::endl;
    }

    ReleaseDeviceList(devices);

    return devicecount;
}

int svlFilterSourceVideoCapture::PrintInputList(int deviceid, bool update)
{
    if (OutputImage == 0)
        return SVL_FAIL;

    if (NumberOfEnumeratedDevices < 1 || update) EnumerateDevices();

    DeviceInfo *devices = 0;
    int devicecount = GetDeviceList(&devices);

    if (deviceid >= devicecount) {
        ReleaseDeviceList(devices);
        return SVL_FAIL;
    }

    int inputcount = devices[deviceid].inputcount;
    for (int i = 0; i < inputcount; i ++) {
        std::cout << "  " << i << ") " << devices[deviceid].inputnames[i] << std::endl;
    }

    ReleaseDeviceList(devices);

    return inputcount;
}

int svlFilterSourceVideoCapture::SetDevice(int deviceid, int inputid, unsigned int videoch)
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    DeviceID[videoch] = deviceid;
    InputID[videoch] = inputid;

    // Make sure devices are enumerated
    if (NumberOfEnumeratedDevices < 1) EnumerateDevices();

    // Set default format to the first format in the list
    SelectFormat(0, videoch);

    return SVL_OK;
}

int svlFilterSourceVideoCapture::GetDevice(int & deviceid, int & inputid, unsigned int videoch) const
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    deviceid = DeviceID[videoch];
    inputid = InputID[videoch];

    return SVL_OK;
}

int svlFilterSourceVideoCapture::GetFormatList(ImageFormat **formatlist, unsigned int videoch) const
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (NumberOfEnumeratedDevices < 1)
        return SVL_FAIL;
    if (formatlist == 0)
        return SVL_FAIL;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;
    if (DeviceID[videoch] < 0)
        return SVL_VCS_UNABLE_TO_OPEN;

    if (FormatList[DeviceID[videoch]] == 0 || FormatListSize[DeviceID[videoch]] < 1) return SVL_FAIL;

    formatlist[0] = new ImageFormat[FormatListSize[DeviceID[videoch]]];
    memcpy(formatlist[0], FormatList[DeviceID[videoch]], FormatListSize[DeviceID[videoch]] * sizeof(ImageFormat));

    return FormatListSize[DeviceID[videoch]];
}

void svlFilterSourceVideoCapture::ReleaseFormatList(ImageFormat *formatlist) const
{
    if (formatlist) delete [] formatlist;
}

int svlFilterSourceVideoCapture::PrintFormatList(unsigned int videoch)
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (NumberOfEnumeratedDevices < 1)
        return SVL_FAIL;

    int i, j;
    ImageFormat *formats = 0;
    int formatcount = GetFormatList(&formats, videoch);

    for (i = 0; i < formatcount; i ++) {
        std::cout << "  " << i << ") " << formats[i].width << "x" << formats[i].height << " ";
        std::cout << GetPixelTypeName(formats[i].colorspace);
        if (formats[i].framerate > 0.0) {
            std::cout << " (<=" << formats[i].framerate << "fps)" << std::endl;
        }
        else {
            std::cout << " (unknown framerate)" << std::endl;
        }
        if (formats[i].custom_mode >= 0) {
            std::cout << "      [CUSTOM mode=" << formats[i].custom_mode << std::endl;
            std::cout << "              maxsize=(" << formats[i].custom_maxwidth << ", " << formats[i].custom_maxheight << "); ";
            std::cout << "unit=(" << formats[i].custom_unitwidth << ", " << formats[i].custom_unitheight << ")" << std::endl;
            std::cout << "              roipos=(" << formats[i].custom_roileft << ", " << formats[i].custom_roitop << "); ";
            std::cout << "unit=(" << formats[i].custom_unitleft << ", " << formats[i].custom_unittop << ")" << std::endl;
            std::cout << "              colorspaces=(";
            for (j = 0; j < svlFilterSourceVideoCaptureTypes::PixelTypeCount
                     && formats[i].custom_colorspaces[j] != svlFilterSourceVideoCaptureTypes::PixelUnknown; j ++) {
                if (j > 0) std::cout << ", ";
                std::cout << GetPixelTypeName(formats[i].custom_colorspaces[j]);
            }
            std::cout << ")" << std::endl;
            std::cout << "              pattern=" << GetPatternTypeName(formats[i].custom_pattern) << "]" << std::endl;
        }
    }

    ReleaseFormatList(formats);

    return formatcount;
}

int svlFilterSourceVideoCapture::SelectFormat(unsigned int formatid, unsigned int videoch)
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (NumberOfEnumeratedDevices < 1)
        return SVL_FAIL;

    ImageFormat *formats = 0;
    int formatcount = GetFormatList(&formats, videoch);
    int ret = SVL_FAIL;

    if (static_cast<int>(formatid) < formatcount) {
        ret = SetFormat(formats[formatid], videoch);
    }

    ReleaseFormatList(formats);

    return ret;
}

int svlFilterSourceVideoCapture::SetFormat(const ImageFormat& format, unsigned int videoch)
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (NumberOfEnumeratedDevices < 1)
        return SVL_FAIL;
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    if (Format[videoch] == 0) Format[videoch] = new ImageFormat;
    memcpy(Format[videoch], &format, sizeof(ImageFormat));

    return SVL_OK;
}

int svlFilterSourceVideoCapture::GetFormat(ImageFormat& format, unsigned int videoch) const
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (NumberOfEnumeratedDevices < 1)
        return SVL_FAIL;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;
    if (Format[videoch] == 0)
        return SVL_FAIL;

    memcpy(&format, Format[videoch], sizeof(ImageFormat));

    return SVL_OK;
}

int svlFilterSourceVideoCapture::SetTrigger(const ExternalTrigger& trigger, unsigned int videoch)
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (NumberOfEnumeratedDevices < 1)
        return SVL_FAIL;
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    memcpy(&(Trigger[videoch]), &trigger, sizeof(ExternalTrigger));

    return SVL_OK;
}

int svlFilterSourceVideoCapture::GetTrigger(ExternalTrigger& trigger, unsigned int videoch) const
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (NumberOfEnumeratedDevices < 1)
        return SVL_FAIL;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    memcpy(&trigger, &(Trigger[videoch]), sizeof(ExternalTrigger));

    return SVL_OK;
}

int svlFilterSourceVideoCapture::SetImageProperties(const ImageProperties& properties, unsigned int videoch)
{
    // Available only after initialization
    if (IsInitialized() == false)
        return SVL_NOT_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    if (DeviceObj[API[videoch]]->SetImageProperties(const_cast<ImageProperties&>(properties), APIChannelID[videoch]) == SVL_OK) {
        if (Properties[videoch] == 0) Properties[videoch] = new ImageProperties;
        memcpy(Properties[videoch], &properties, sizeof(ImageProperties));
        Properties[videoch]->mask = propShutter & propGain & propWhiteBalance & propBrightness & propGamma & propSaturation;
        return SVL_OK;
    }

    return SVL_FAIL;
}

int svlFilterSourceVideoCapture::SetImageProperties(unsigned int videoch)
{
    // Available only after initialization
    if (IsInitialized() == false)
        return SVL_NOT_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;
    if (Properties[videoch] == 0)
        return SVL_FAIL;

    return DeviceObj[API[videoch]]->SetImageProperties(Properties[videoch][0], APIChannelID[videoch]);
}

int svlFilterSourceVideoCapture::GetImageProperties(ImageProperties& properties, unsigned int videoch) const
{
    // Available only after initialization
    if (IsInitialized() == false)
        return SVL_NOT_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    if (DeviceObj[API[videoch]]->GetImageProperties(properties, APIChannelID[videoch]) != SVL_OK) return SVL_FAIL;

    if (Properties[videoch] == 0) Properties[videoch] = new ImageProperties;
    memcpy(Properties[videoch], &properties, sizeof(ImageProperties));
    Properties[videoch]->mask = propShutter & propGain & propWhiteBalance & propBrightness & propGamma & propSaturation;

    return SVL_OK;
}

int svlFilterSourceVideoCapture::GetImageProperties(unsigned int videoch)
{
    // Available only after initialization
    if (IsInitialized() == false)
        return SVL_NOT_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    if (Properties[videoch] == 0) Properties[videoch] = new ImageProperties;
    Properties[videoch]->mask = propShutter & propGain & propWhiteBalance & propBrightness & propGamma & propSaturation;

    return DeviceObj[API[videoch]]->GetImageProperties(Properties[videoch][0], APIChannelID[videoch]);
}

std::string svlFilterSourceVideoCapture::GetPixelTypeName(PixelType pixeltype)
{
    switch (pixeltype) {
    case svlFilterSourceVideoCaptureTypes::PixelRAW8:    return "RAW8";
    case svlFilterSourceVideoCaptureTypes::PixelRAW16:   return "RAW16";
    case svlFilterSourceVideoCaptureTypes::PixelRGB8:    return "RGB24";
    case svlFilterSourceVideoCaptureTypes::PixelYUV444:  return "YUV444";
    case svlFilterSourceVideoCaptureTypes::PixelYUV422:  return "YUV422";
    case svlFilterSourceVideoCaptureTypes::PixelYUV411:  return "YUV411";
    case svlFilterSourceVideoCaptureTypes::PixelMONO8:   return "Mono8";
    case svlFilterSourceVideoCaptureTypes::PixelMONO16:  return "Mono16";
    case svlFilterSourceVideoCaptureTypes::PixelUnknown:
    default:                                             return "Unknown color space";
    }
}

std::string svlFilterSourceVideoCapture::GetPatternTypeName(PatternType patterntype)
{
    switch (patterntype) {
    case svlFilterSourceVideoCaptureTypes::PatternRGGB:   return "RGGB";
    case svlFilterSourceVideoCaptureTypes::PatternGBRG:   return "GBRG";
    case svlFilterSourceVideoCaptureTypes::PatternGRBG:   return "GRBG";
    case svlFilterSourceVideoCaptureTypes::PatternBGGR:   return "BGGR";
    case svlFilterSourceVideoCaptureTypes::PatternUnknown:
    default:                                              return "Unknown pattern";
    }
}

unsigned int svlFilterSourceVideoCapture::GetWidth(unsigned int videoch) const
{
    if (!IsInitialized()) return 0;
    return OutputImage->GetWidth(videoch);
}

unsigned int svlFilterSourceVideoCapture::GetHeight(unsigned int videoch) const
{
    if (!IsInitialized()) return 0;
    return OutputImage->GetHeight(videoch);
}

int svlFilterSourceVideoCapture::SaveSettings(const char* filepath)
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (NumberOfEnumeratedDevices < 1)
        return SVL_FAIL;

    unsigned int writelen;
    int err, devid, intvalue;
    unsigned char emptybuffer[SVL_VCS_STRING_LENGTH];
	FILE *fp;

    memset(emptybuffer, 0, SVL_VCS_STRING_LENGTH);
    err = SVL_FAIL;

    fp = fopen(filepath, "wb");
    if (fp == NULL) goto labError;

	// Write "number of channels"
    writelen = static_cast<unsigned int>(fwrite(&NumberOfChannels, sizeof(unsigned int), 1, fp));
	if (writelen < 1) goto labError;

    // For each channel
    for (unsigned int i = 0; i < NumberOfChannels; i ++) {

		// Write "device id"
        devid = DeviceID[i];
        // Check if device ID is in range
        if (devid >= NumberOfEnumeratedDevices) devid = -1;
        writelen = static_cast<unsigned int>(fwrite(&devid, sizeof(int), 1, fp));
		if (writelen < 1) goto labError;

        // Write "device name"
        if (devid >= 0) writelen = static_cast<unsigned int>(fwrite(EnumeratedDevices[DeviceID[i]].name, SVL_VCS_STRING_LENGTH, 1, fp));
        else writelen = static_cast<unsigned int>(fwrite(emptybuffer, SVL_VCS_STRING_LENGTH, 1, fp));
		if (writelen < 1) goto labError;

        // Write "input id"
        intvalue = InputID[i];
        writelen = static_cast<unsigned int>(fwrite(&intvalue, sizeof(int), 1, fp));
		if (writelen < 1) goto labError;

        // Write "format size"
        if (Format[i]) intvalue = sizeof(ImageFormat);
        else intvalue = 0;
        writelen = static_cast<unsigned int>(fwrite(&intvalue, sizeof(int), 1, fp));
		if (writelen < 1) goto labError;

        // Write "format"
        if (intvalue > 0) {
            writelen = static_cast<unsigned int>(fwrite(Format[i], 1, sizeof(ImageFormat), fp));
		    if (writelen < sizeof(ImageFormat)) goto labError;
	    }

        // Write "properties size"
        if (Properties[i]) intvalue = sizeof(ImageProperties);
        else intvalue = 0;
        writelen = static_cast<unsigned int>(fwrite(&intvalue, sizeof(int), 1, fp));
		if (writelen < 1) goto labError;

        // Write "properties"
        if (intvalue > 0) {
            writelen = static_cast<unsigned int>(fwrite(Properties[i], 1, sizeof(ImageProperties), fp));
		    if (writelen < sizeof(ImageProperties)) goto labError;
	    }

        // Write "trigger size"
        intvalue = sizeof(ExternalTrigger);
        writelen = static_cast<unsigned int>(fwrite(&intvalue, sizeof(int), 1, fp));
		if (writelen < 1) goto labError;

        // Write "trigger"
        if (intvalue > 0) {
            writelen = static_cast<unsigned int>(fwrite(&(Trigger[i]), 1, sizeof(ExternalTrigger), fp));
		    if (writelen < sizeof(ExternalTrigger)) goto labError;
	    }

        // Write "device specific configuration buffer size"
        writelen = static_cast<unsigned int>(fwrite(&(DevSpecConfigBufferSize[i]), sizeof(unsigned int), 1, fp));
		if (writelen < 1) goto labError;

        // Write "device specific configuration buffer"
        writelen = static_cast<unsigned int>(fwrite(DevSpecConfigBuffer[i], 1, DevSpecConfigBufferSize[i], fp));
		if (writelen < DevSpecConfigBufferSize[i]) goto labError;
    }

    err = SVL_OK;

labError:
    if (fp != NULL) fclose(fp);

    return err;
}

int svlFilterSourceVideoCapture::LoadSettings(const char* filepath)
{
    if (OutputImage == 0) {
        CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: video channel count has not been set" << std::endl;
        return SVL_FAIL;
    }
    if (IsInitialized() == true) {
        CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: filter already initialized" << std::endl;
        return SVL_ALREADY_INITIALIZED;
    }

    int intvalue;
    unsigned int i, readlen;
    unsigned int uintvalue;
    char buffer[SVL_VCS_STRING_LENGTH];
	FILE *fp;

    fp = fopen(filepath, "rb");
    if (fp == 0) return SVL_FAIL;

    // Read "number of channels"
    readlen = static_cast<unsigned int>(fread(&uintvalue, sizeof(unsigned int), 1, fp));
	if (readlen < 1 || uintvalue != NumberOfChannels) {
        fclose(fp);
        CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: failed to load \"Number-Of-Channels\"" << std::endl;
        return SVL_FAIL;
    }

    // Make sure devices are enumerated
    if (NumberOfEnumeratedDevices < 1) EnumerateDevices();

    // For each channel
    for (i = 0; i < NumberOfChannels; i ++) {
        bool device_id_out_of_range = false;

		// Read "device id"
        readlen = static_cast<unsigned int>(fread(&intvalue, sizeof(int), 1, fp));
        if (readlen < 1) {
            CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: failed to load \"Device-ID[" << i << "]\"" << std::endl;
            goto labError;
        }
        if (intvalue < -1 || intvalue >= NumberOfEnumeratedDevices) {
            device_id_out_of_range = true;
            CMN_LOG_CLASS_INIT_WARNING << "LoadSettings: Device-ID[" << i << "]=" << intvalue << " out of range [0, " << NumberOfEnumeratedDevices - 1 << "]" << std::endl;
            DeviceID[i] = -1;
        }
        else {
            DeviceID[i] = intvalue;
        }

        // Read "device name"
        readlen = static_cast<unsigned int>(fread(buffer, SVL_VCS_STRING_LENGTH, 1, fp));
        if (readlen < 1) {
            CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: failed to load \"Device-Name[" << i << "]\"" << std::endl;
            goto labError;
        }
        buffer[SVL_VCS_STRING_LENGTH - 1] = 0;
        if (DeviceID[i] >= 0) {
            buffer[SVL_VCS_STRING_LENGTH - 1] = 0;
            if (strcmp(EnumeratedDevices[DeviceID[i]].name, buffer) != 0) goto labError;
        }
        if (DeviceID[i] < 0) {
            // List available capture devices
            std::stringstream strstr;
            strstr << "LoadSettings: searching for available capture device matching the name specified in configuration file (\"" << buffer << "\")" << std::endl
                   << ". Available capture devices (" << NumberOfEnumeratedDevices << "):" << std::endl;
            for (int j = 0; j < NumberOfEnumeratedDevices; j ++) {
                strstr << ".     " << j << ") " << EnumeratedDevices[j].name << std::endl;
            }
            CMN_LOG_CLASS_INIT_WARNING << strstr.str();
            // Search for device name
            int j;
            for (j = 0; j < NumberOfEnumeratedDevices; j ++) {
                if (strcmp(EnumeratedDevices[j].name, buffer) == 0) {
                    CMN_LOG_CLASS_INIT_WARNING << "LoadSettings: found matching capture device (" << j << ")" << std::endl;
                    DeviceID[i] = j;
                    break;
                }
            }
            if (j >= NumberOfEnumeratedDevices) {
                CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: failed to find matching available capture device" << std::endl;
                goto labError;
            }
        }

        // Read "input id"
        readlen = static_cast<unsigned int>(fread(&intvalue, sizeof(int), 1, fp));
        if (readlen < 1) {
            CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: failed to load \"Input-ID[" << i << "]\"" << std::endl;
            goto labError;
        }
        if (DeviceID[i] >= 0) InputID[i] = intvalue;
        else InputID[i] = -1;

        // Read "format size"
        readlen = static_cast<unsigned int>(fread(&intvalue, sizeof(int), 1, fp));
        if (readlen < 1) {
            CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: failed to load \"Format-Size[" << i << "]\"" << std::endl;
            goto labError;
        }

        // Read "format"
        if (intvalue == sizeof(ImageFormat)) {
            if (Format[i] == 0) Format[i] = new ImageFormat;
            readlen = static_cast<unsigned int>(fread(Format[i], 1, sizeof(ImageFormat), fp));
            if (readlen < sizeof(ImageFormat)) {
                delete Format[i];
                Format[i] = 0;
                CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: failed to load \"Format[" << i << "]\"" << std::endl;
                goto labError;
            }
        }

        // Read "properties size"
        readlen = static_cast<unsigned int>(fread(&intvalue, sizeof(int), 1, fp));
        if (readlen < 1) {
            CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: failed to load \"Properties-Size[" << i << "]\"" << std::endl;
            goto labError;
        }

        // Read "properties"
        if (intvalue == sizeof(ImageProperties)) {
            if (Properties[i] == 0) Properties[i] = new ImageProperties;
            readlen = static_cast<unsigned int>(fread(Properties[i], 1, sizeof(ImageProperties), fp));
            if (readlen < sizeof(ImageProperties)) {
                delete Properties[i];
                Properties[i] = 0;
                CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: failed to load \"Properties[" << i << "]\"" << std::endl;
                goto labError;
            }
        }

        // Read "trigger size"
        readlen = static_cast<unsigned int>(fread(&intvalue, sizeof(int), 1, fp));
        if (readlen < 1) {
            CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: failed to load \"Trigger-Size[" << i << "]\"" << std::endl;
            goto labError;
        }

        // Read "trigger"
        if (intvalue == sizeof(ExternalTrigger)) {
            readlen = static_cast<unsigned int>(fread(&(Trigger[i]), 1, sizeof(ExternalTrigger), fp));
            if (readlen < sizeof(ExternalTrigger)) {
                memset(&(Trigger[i]), 0, sizeof(ExternalTrigger));
                CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: failed to load \"Trigger[" << i << "]\"" << std::endl;
                goto labError;
            }
        }

        // Read "device specific configuration buffer size"
        readlen = static_cast<unsigned int>(fread(&uintvalue, sizeof(unsigned int), 1, fp));
        if (readlen < 1) {
            CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: failed to load \"Dev-Spec-Config-Buffer-Size[" << i << "]\"" << std::endl;
            goto labError;
        }
        if (DeviceID[i] >= 0) {
            if (uintvalue >= MAX_PROPERTIES_BUFFER_SIZE) goto labError;
            DevSpecConfigBufferSize[i] = uintvalue;
        }
        else DevSpecConfigBufferSize[i] = 0;

        // Read "device specific configuration buffer"
        if (DevSpecConfigBuffer[i]) delete [] DevSpecConfigBuffer[i];
        DevSpecConfigBuffer[i] = 0;
        if (uintvalue > 0) {
            DevSpecConfigBuffer[i] = new unsigned char[uintvalue];
            readlen = static_cast<unsigned int>(fread(DevSpecConfigBuffer[i], 1, uintvalue, fp));
            if (readlen < uintvalue) {
                CMN_LOG_CLASS_INIT_ERROR << "LoadSettings: failed to load \"Dev-Spec-Config-Buffer[" << i << "]\"" << std::endl;
                goto labError;
            }
        }
    }

    fclose(fp);
    return SVL_OK;

labError:
    for (i = 0; i < NumberOfChannels; i ++) {
        DeviceID[i] = InputID[i] = -1;
        if (DevSpecConfigBuffer[i]) delete [] DevSpecConfigBuffer[i];
        DevSpecConfigBuffer[i] = 0;
        DevSpecConfigBufferSize[i] = 0;
    }
    fclose(fp);
    return SVL_FAIL;
}

int svlFilterSourceVideoCapture::SetDefaultSettings(unsigned int videoch)
{
    if (OutputImage == 0)
        return SVL_FAIL;
    if (NumberOfEnumeratedDevices < 1)
        return SVL_FAIL;
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    // Select first format in list of supported formats
    SelectFormat(0, videoch);

    if (Format[videoch] != 0)
        return SVL_FAIL;

    Format[videoch]->defaults = true;

    return SVL_OK;
}

void svlFilterSourceVideoCapture::CreateInterfaces()
{
    // Add NON-QUEUED provided interface for configuration management
    mtsInterfaceProvided* provided = AddInterfaceProvided("Settings", MTS_COMMANDS_SHOULD_NOT_BE_QUEUED);
    if (provided) {
        provided->AddCommandVoid (&svlFilterSourceBase::Pause, dynamic_cast<svlFilterSourceBase*>(this), "Pause");
        provided->AddCommandVoid (&svlFilterSourceBase::Play,  dynamic_cast<svlFilterSourceBase*>(this), "Play");
        provided->AddCommandWrite(&svlFilterSourceBase::Play,  dynamic_cast<svlFilterSourceBase*>(this), "PlayFrames");
        provided->AddCommandVoid (&svlFilterSourceVideoCapture::EnumerateDevicesCommand,    this, "EnumerateDevices");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetDeviceListCommand,       this, "GetDeviceList");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetCommand,                 this, "Get");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetCommand,                 this, "Set");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetChannelsCommand,         this, "SetChannels");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetDeviceLCommand,          this, "SetDevice");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetDeviceLCommand,          this, "SetLeftDevice");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetDeviceRCommand,          this, "SetRightDevice");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetInputLCommand,           this, "SetInput");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetInputLCommand,           this, "SetLeftInput");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetInputRCommand,           this, "SetRightInput");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetFormatLCommand,          this, "SetFormat");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetFormatLCommand,          this, "SetLeftFormat");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetFormatRCommand,          this, "SetRightFormat");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SelectFormatLCommand,       this, "SelectFormat");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SelectFormatLCommand,       this, "SelectLeftFormat");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SelectFormatRCommand,       this, "SelectRightFormat");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetTriggerLCommand,         this, "SetTrigger");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetTriggerLCommand,         this, "SetLeftTrigger");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetTriggerRCommand,         this, "SetRightTrigger");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetImagePropertiesLCommand, this, "SetImageProperties");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetImagePropertiesLCommand, this, "SetLeftImageProperties");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SetImagePropertiesRCommand, this, "SetRightImageProperties");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::SaveSettingsCommand,        this, "SaveSettings");
        provided->AddCommandWrite(&svlFilterSourceVideoCapture::LoadSettingsCommand,        this, "LoadSettings");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetChannelsCommand,         this, "GetChannels");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetDeviceLCommand,          this, "GetDevice");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetDeviceLCommand,          this, "GetLeftDevice");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetDeviceRCommand,          this, "GetRightDevice");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetInputLCommand,           this, "GetInput");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetInputLCommand,           this, "GetLeftInput");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetInputRCommand,           this, "GetRightInput");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetFormatLCommand,          this, "GetFormat");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetFormatLCommand,          this, "GetLeftFormat");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetFormatRCommand,          this, "GetRightFormat");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetDimensionsLCommand,      this, "GetDimensions");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetDimensionsLCommand,      this, "GetLeftDimensions");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetDimensionsRCommand,      this, "GetRightDimensions");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetFormatListLCommand,      this, "GetFormatList");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetFormatListLCommand,      this, "GetLeftFormatList");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetFormatListRCommand,      this, "GetRightFormatList");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetTriggerLCommand,         this, "GetTrigger");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetTriggerLCommand,         this, "GetLeftTrigger");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetTriggerRCommand,         this, "GetRightTrigger");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetImagePropertiesLCommand, this, "GetImageProperties");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetImagePropertiesLCommand, this, "GetLeftImageProperties");
        provided->AddCommandRead (&svlFilterSourceVideoCapture::GetImagePropertiesRCommand, this, "GetRightImageProperties");
    }
}

void svlFilterSourceVideoCapture::EnumerateDevicesCommand(void)
{
    int ret = EnumerateDevices();
    if (ret > 0) {
        CMN_LOG_CLASS_INIT_VERBOSE << "EnumerateDevicesCommand: number of enumerated devices: " << ret << std::endl;
    } else if (ret == 0) {
        CMN_LOG_CLASS_INIT_WARNING << "EnumerateDevicesCommand: no devices have been found" << std::endl;
    } else if (ret == SVL_ALREADY_INITIALIZED) {
        CMN_LOG_CLASS_INIT_ERROR   << "EnumerateDevicesCommand: filter is already initialized" << std::endl;
    } else if (ret == 0) {
        CMN_LOG_CLASS_INIT_ERROR   << "EnumerateDevicesCommand: unknown error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::GetDeviceListCommand(ThisType::DeviceInfoListType & devicelist) const
{
    DeviceInfo* devices = 0;
    int devicecount = GetDeviceList(&devices);
    if (devicecount < 1) {
        devicelist.SetSize(0);
        CMN_LOG_CLASS_INIT_WARNING << "GetDeviceListCommand: no devices have been found; "
                                   << "make sure to issue \"EnumerateDevices\" command prior to this call"
                                   << std::endl;
        return;
    }
    devicelist.SetSize(devicecount);
    for (int i = 0; i < devicecount; i ++) {
        memcpy(devicelist.Pointer(i), &(devices[i]), sizeof(DeviceInfo));
    }
    ReleaseDeviceList(devices);

    CMN_LOG_CLASS_INIT_VERBOSE << "GetDeviceListCommand: number of devices in the list: " << devicecount << std::endl;
}

void svlFilterSourceVideoCapture::GetCommand(svlFilterSourceVideoCapture::Config& objref) const
{
    objref.SetChannels(NumberOfChannels);

    for (int i = 0; i < objref.Channels; i ++) {
        if (GetDevice(objref.Device[i], objref.Input[i], i) != SVL_OK) {
            CMN_LOG_CLASS_INIT_ERROR << "GetCommand: \"GetDevice(" << objref.Device[i] << ", "
                                     << objref.Input[i] << ", " << i << ")\" returned error"
                                     << std::endl;
        }
        if (GetFormat(objref.Format[i], i) != SVL_OK) {
            CMN_LOG_CLASS_INIT_ERROR << "GetCommand: \"GetFormat(., " << i << ")\" returned error" << std::endl;
        }
        if (GetImageProperties(objref.Properties[i], i) != SVL_OK) {
            CMN_LOG_CLASS_INIT_ERROR << "GetCommand: \"GetImageProperties(., " << i << ")\" returned error" << std::endl;
        }
        if (GetTrigger(objref.Trigger[i], i) != SVL_OK) {
            CMN_LOG_CLASS_INIT_ERROR << "GetCommand: \"GetTrigger(., " << i << ")\" returned error" << std::endl;
        }
    }
}

void svlFilterSourceVideoCapture::SetCommand(const svlFilterSourceVideoCapture::Config& objref)
{
    if (objref.Channels < 0) return;

    // Make sure devices are enumerated
    if (NumberOfEnumeratedDevices < 1) EnumerateDevices();

    SetChannelCount(static_cast<unsigned int>(objref.Channels));
    for (int i = 0; i < objref.Channels; i ++) {
        if (SetDevice(objref.Device[i], objref.Input[i], i) != SVL_OK) {
            CMN_LOG_CLASS_INIT_ERROR << "SetCommand: \"SetDevice(" << objref.Device[i] << ", "
                                     << objref.Input[i] << ", " << i << ")\" returned error"
                                     << std::endl;
        }
        if (SetFormat(objref.Format[i], i) != SVL_OK) {
            CMN_LOG_CLASS_INIT_ERROR << "SetCommand: \"SetFormat(., " << i << ")\" returned error" << std::endl;
        }
        if (SetImageProperties(objref.Properties[i], i) != SVL_OK) {
            CMN_LOG_CLASS_INIT_ERROR << "SetCommand: \"SetImageProperties(., " << i << ")\" returned error" << std::endl;
        }
        if (SetTrigger(objref.Trigger[i], i) != SVL_OK) {
            CMN_LOG_CLASS_INIT_ERROR << "SetCommand: \"SetTrigger(., " << i << ")\" returned error" << std::endl;
        }

        // Set default format to the first format in the list
        SelectFormat(0, i);
    }
}

void svlFilterSourceVideoCapture::SetChannelsCommand(const int& channels)
{
    if (SetChannelCount(static_cast<unsigned int>(channels)) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetChannelsCommand: \"SetChannelCount(" << channels << ")\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::SetDeviceLCommand(const int & deviceid)
{
    if (!OutputImage) {
        CMN_LOG_CLASS_INIT_ERROR << "SetDeviceLCommand: failed to select device; "
                                 << "set channel count using \"SetChannelsCommand\" prior to this call" << std::endl;
        return;
    }
    if (IsInitialized()) {
        CMN_LOG_CLASS_INIT_ERROR << "SetDeviceLCommand: failed to select device; filter is already initialized" << std::endl;
        return;
    }
    DeviceID[SVL_LEFT] = deviceid;

    // Make sure devices are enumerated
    if (NumberOfEnumeratedDevices < 1) EnumerateDevices();

    // Set default format to the first format in the list
    SelectFormat(0, SVL_LEFT);
}

void svlFilterSourceVideoCapture::SetDeviceRCommand(const int & deviceid)
{
    if (!OutputImage) {
        CMN_LOG_CLASS_INIT_ERROR << "SetDeviceRCommand: failed to select device; "
                                 << "set channel count using \"SetChannelsCommand\" prior to this call" << std::endl;
        return;
    }
    if (IsInitialized()) {
        CMN_LOG_CLASS_INIT_ERROR << "SetDeviceRCommand: failed to select device; filter is already initialized" << std::endl;
        return;
    }
    DeviceID[SVL_RIGHT] = deviceid;

    // Make sure devices are enumerated
    if (NumberOfEnumeratedDevices < 1) EnumerateDevices();

    // Set default format to the first format in the list
    SelectFormat(0, SVL_RIGHT);
}

void svlFilterSourceVideoCapture::SetInputLCommand(const int & inputid)
{
    if (!OutputImage) {
        CMN_LOG_CLASS_INIT_ERROR << "SetInputLCommand: failed to select input; "
                                 << "set channel count using \"SetChannelsCommand\" prior to this call" << std::endl;
        return;
    }
    if (IsInitialized()) {
        CMN_LOG_CLASS_INIT_ERROR << "SetInputLCommand: failed to select input; filter is already initialized" << std::endl;
        return;
    }
    InputID[SVL_LEFT] = inputid;
}

void svlFilterSourceVideoCapture::SetInputRCommand(const int & inputid)
{
    if (!OutputImage) {
        CMN_LOG_CLASS_INIT_ERROR << "SetInputRCommand: failed to select input; "
                                 << "set channel count using \"SetChannelsCommand\" prior to this call" << std::endl;
        return;
    }
    if (IsInitialized()) {
        CMN_LOG_CLASS_INIT_ERROR << "SetInputRCommand: failed to select input; filter is already initialized" << std::endl;
        return;
    }
    InputID[SVL_RIGHT] = inputid;
}

void svlFilterSourceVideoCapture::SetFormatLCommand(const svlFilterSourceVideoCapture::ImageFormat & format)
{
    if (SetFormat(format, SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetFormatLCommand: \"SetFormat\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::SetFormatRCommand(const svlFilterSourceVideoCapture::ImageFormat & format)
{
    if (SetFormat(format, SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetFormatRCommand: \"SetFormat\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::SelectFormatLCommand(const int & formatid)
{
    if (SelectFormat(static_cast<unsigned int>(formatid), SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SelectFormatLCommand: \"SelectFormat(" << formatid << ", 0)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::SelectFormatRCommand(const int & formatid)
{
    if (SelectFormat(static_cast<unsigned int>(formatid), SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SelectFormatRCommand: \"SelectFormat(" << formatid << ", 1)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::SetTriggerLCommand(const svlFilterSourceVideoCapture::ExternalTrigger & trigger)
{
    if (SetTrigger(trigger, SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetTriggerLCommand: \"SetTrigger\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::SetTriggerRCommand(const svlFilterSourceVideoCapture::ExternalTrigger & trigger)
{
    if (SetTrigger(trigger, SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetTriggerRCommand: \"SetTrigger\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::SetImagePropertiesLCommand(const svlFilterSourceVideoCapture::ImageProperties & properties)
{
    if (SetImageProperties(properties, SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetImagePropertiesLCommand: \"SetImageProperties\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::SetImagePropertiesRCommand(const svlFilterSourceVideoCapture::ImageProperties & properties)
{
    if (SetImageProperties(properties, SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SetImagePropertiesRCommand: \"SetImageProperties\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::SaveSettingsCommand(const std::string & filepath)
{
    if (SaveSettings(filepath.c_str()) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "SaveSettingsCommand: \"SaveSettings(\"" << filepath << "\")\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::LoadSettingsCommand(const std::string & filepath)
{
    if (LoadSettings(filepath.c_str()) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "LoadSettingsCommand: \"LoadSettings(\"" << filepath << "\")\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::GetChannelsCommand(int & channels) const
{
    channels = NumberOfChannels;
}

void svlFilterSourceVideoCapture::GetDeviceLCommand(int & deviceid) const
{
    int intput;
    if (GetDevice(deviceid, intput, SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "GetDeviceLCommand: \"GetDevice(., ., 0)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::GetDeviceRCommand(int & deviceid) const
{
    int intput;
    if (GetDevice(deviceid, intput, SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "GetDeviceRCommand: \"GetDevice(., ., 1)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::GetInputLCommand(int & inputid) const
{
    int device;
    if (GetDevice(device, inputid, SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "GetInputLCommand: \"GetDevice(., ., 0)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::GetInputRCommand(int & inputid) const
{
    int device;
    if (GetDevice(device, inputid, SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "GetInputRCommand: \"GetDevice(., ., 1)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::GetFormatLCommand(ThisType::ImageFormat & format) const
{
    if (GetFormat(format, SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "GetFormatLCommand: \"GetFormat(., 0)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::GetFormatRCommand(ThisType::ImageFormat & format) const
{
    if (GetFormat(format, SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "GetFormatRCommand: \"GetFormat(., 1)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::GetDimensionsLCommand(vctInt2 & dimensions) const
{
    dimensions[0] = static_cast<int>(GetWidth(SVL_LEFT));
    dimensions[1] = static_cast<int>(GetHeight(SVL_LEFT));
}

void svlFilterSourceVideoCapture::GetDimensionsRCommand(vctInt2 & dimensions) const
{
    dimensions[0] = static_cast<int>(GetWidth(SVL_RIGHT));
    dimensions[1] = static_cast<int>(GetHeight(SVL_RIGHT));
}

void svlFilterSourceVideoCapture::GetFormatListLCommand(svlFilterSourceVideoCapture::FormatListType & formatlist) const
{
    ImageFormat* formats = 0;
    int formatcount = GetFormatList(&formats, SVL_LEFT);
    if (formatcount < 1) {
        CMN_LOG_CLASS_INIT_WARNING << "GetFormatListCommand: no available image formats have been found for channel 0"<< std::endl;
        formatlist.SetSize(0);
        return;
    }
    formatlist.SetSize(formatcount);
    for (int i = 0; i < formatcount; i ++) {
        memcpy(formatlist.Pointer(i), &(formats[i]), sizeof(ImageFormat));
    }
    ReleaseFormatList(formats);

    CMN_LOG_CLASS_INIT_VERBOSE << "GetFormatListCommand: number of available images formats for channel 0: " << formatcount << std::endl;
}

void svlFilterSourceVideoCapture::GetFormatListRCommand(svlFilterSourceVideoCapture::FormatListType & formatlist) const
{
    ImageFormat* formats = 0;
    int formatcount = GetFormatList(&formats, SVL_RIGHT);
    if (formatcount < 1) {
        CMN_LOG_CLASS_INIT_WARNING << "GetFormatListCommand: no available image formats have been found for channel 1"<< std::endl;
        formatlist.SetSize(0);
        return;
    }
    formatlist.SetSize(formatcount);
    for (int i = 0; i < formatcount; i ++) {
        memcpy(formatlist.Pointer(i), &(formats[i]), sizeof(ImageFormat));
    }
    ReleaseFormatList(formats);

    CMN_LOG_CLASS_INIT_VERBOSE << "GetFormatListCommand: number of available images formats for channel 1: " << formatcount << std::endl;
}

void svlFilterSourceVideoCapture::GetTriggerLCommand(svlFilterSourceVideoCapture::ExternalTrigger & trigger) const
{
    if (GetTrigger(trigger, SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "GetTriggerCommand: \"GetTrigger(., 0)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::GetTriggerRCommand(svlFilterSourceVideoCapture::ExternalTrigger & trigger) const
{
    if (GetTrigger(trigger, SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "GetTriggerCommand: \"GetTrigger(., 1)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::GetImagePropertiesLCommand(svlFilterSourceVideoCapture::ImageProperties & properties) const
{
    if (GetImageProperties(properties, SVL_LEFT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "GetImagePropertiesCommand: \"GetImageProperties(., 0)\" returned error" << std::endl;
    }
}

void svlFilterSourceVideoCapture::GetImagePropertiesRCommand(svlFilterSourceVideoCapture::ImageProperties & properties) const
{
    if (GetImageProperties(properties, SVL_RIGHT) != SVL_OK) {
        CMN_LOG_CLASS_INIT_ERROR << "GetImagePropertiesCommand: \"GetImageProperties(., 1)\" returned error" << std::endl;
    }
}


/***************************************/
/*** svlVidCapSrcBase class ************/
/***************************************/

int svlVidCapSrcBase::GetFormatList(unsigned int CMN_UNUSED(deviceid), svlFilterSourceVideoCapture::ImageFormat ** CMN_UNUSED(formatlist))
{
    return SVL_FAIL;
}

int svlVidCapSrcBase::SetFormat(svlFilterSourceVideoCapture::ImageFormat & CMN_UNUSED(format), unsigned int CMN_UNUSED(videoch))
{
    return SVL_FAIL;
}

int svlVidCapSrcBase::GetFormat(svlFilterSourceVideoCapture::ImageFormat & CMN_UNUSED(format), unsigned int CMN_UNUSED(videoch))
{
    return SVL_FAIL;
}

int svlVidCapSrcBase::SetImageProperties(svlFilterSourceVideoCapture::ImageProperties & CMN_UNUSED(properties), unsigned int CMN_UNUSED(videoch))
{
    return SVL_FAIL;
}

int svlVidCapSrcBase::GetImageProperties(svlFilterSourceVideoCapture::ImageProperties & CMN_UNUSED(properties), unsigned int CMN_UNUSED(videoch))
{
    return SVL_FAIL;
}

int svlVidCapSrcBase::SetTrigger(svlFilterSourceVideoCapture::ExternalTrigger & CMN_UNUSED(trigger), unsigned int CMN_UNUSED(videoch))
{
    return SVL_FAIL;
}

int svlVidCapSrcBase::GetTrigger(svlFilterSourceVideoCapture::ExternalTrigger & CMN_UNUSED(trigger), unsigned int CMN_UNUSED(videoch))
{
    return SVL_FAIL;
}


/***********************************************/
/*** svlVidCapSrcDialogThread class ************/
/***********************************************/

void* svlVidCapSrcDialogThread::Proc(svlFilterSourceVideoCapture* baseref)
{
    // signal success to main thread
    InitSuccess = true;
    InitEvent.Raise();

    svlFilterSourceVideoCapture::ImageProperties properties;

    // Create an empty line under the table
    printf("\r\n\x1b[1A");

    while (!Stopped) {

        if (baseref->GetImageProperties(properties, VideoChannel) == SVL_OK) {

            if (properties.manual & svlFilterSourceVideoCapture::propShutter) {
                printf("\r  |  %7d |", properties.shutter);
            }
            else {
                printf("\r  |  %6da |", properties.shutter);
            }
            if (properties.manual & svlFilterSourceVideoCapture::propGain) {
                printf("  %6d |", properties.gain);
            }
            else {
                printf("  %5da |", properties.gain);
            }
            if (properties.manual & svlFilterSourceVideoCapture::propWhiteBalance) {
                printf("  %5d | %6d |", properties.wb_u_b, properties.wb_v_r);
            }
            else {
                printf("  %4da | %5da |", properties.wb_u_b, properties.wb_v_r);
            }
            if (properties.manual & svlFilterSourceVideoCapture::propBrightness) {
                printf("  %9d |", properties.brightness);
            }
            else {
                printf("  %8da |", properties.brightness);
            }
            if (properties.manual & svlFilterSourceVideoCapture::propGamma) {
                printf("  %7d |", properties.gamma);
            }
            else {
                printf("  %6da |", properties.gamma);
            }
            if (properties.manual & svlFilterSourceVideoCapture::propSaturation) {
                printf("  %7d |", properties.saturation);
            }
            else {
                printf("  %6da |", properties.saturation);
            }
        }
        else {
            printf("\r  |      Image property adjustments not available. Press 'q' to return.     |");
        }

        fflush(stdout);
        osaSleep(0.1);
    }

	return this;
}


/****************************/
/*** Stream out operators ***/
/****************************/

std::ostream & operator << (std::ostream & stream, const svlFilterSourceVideoCapture::DeviceInfoListType & objref)
{
    const unsigned int size = static_cast<unsigned int>(objref.size());

    for (unsigned int i = 0; i < size; i ++) {

        stream << " "
               << i
               << ") "
               << objref[i].name
               << std::endl;
    }

    return stream;
}

std::ostream & operator << (std::ostream & stream, const svlFilterSourceVideoCapture::FormatListType & objref)
{
    const unsigned int size = static_cast<unsigned int>(objref.size());

    for (unsigned int i = 0; i < size; i ++) {

        stream << " "
               << i
               << ") "
               << objref[i].width
               << "x"
               << objref[i].height
               << " ";

        stream << svlFilterSourceVideoCapture::GetPixelTypeName(objref[i].colorspace);

        if (objref[i].framerate > 0.0) {

            stream << " (<="
                   << objref[i].framerate
                   << "fps)";
        }
        else {

            stream << " (unknown framerate)";
        }

        if (objref[i].custom_mode >= 0) {

            stream << " [CUSTOM mode="
                   << objref[i].custom_mode
                   << "]";
        }
        stream << std::endl;
    }

    return stream;
}
