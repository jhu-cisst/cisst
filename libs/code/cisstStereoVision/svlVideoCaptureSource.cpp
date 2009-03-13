/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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

#include <cisstStereoVision/svlVideoCaptureSource.h>
#include <cisstOSAbstraction/osaSleep.h>

#include <stdio.h>

#ifdef _WIN32
#include <conio.h>
#endif // _WIN32

#ifdef __GNUC__
#include <curses.h>
#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#endif // __GNUC__

#if (CISST_SVL_HAS_SVS == ON)
#include "vidSVSSource.h"
#endif // CISST_SVL_HAS_SVS

#if (CISST_SVL_HAS_DIRECTSHOW == ON)
#include "vidDirectShowSource.h"
#endif // CISST_SVL_HAS_DIRECTSHOW

#if (CISST_SVL_HAS_VIDEO4LINUX2 == ON)
#include "vidV4L2Source.h"
#endif // CISST_SVL_HAS_VIDEO4LINUX2

#if (CISST_SVL_HAS_LIBDC1394 == ON)
#include "vidDC1394Source.h"
#endif // CISST_SVL_HAS_LIBDC1394

#if (CISST_SVL_HAS_OPENCV == ON)
#include "vidOCVSource.h"
#endif // CISST_SVL_HAS_OPENCV

#if (CISST_SVL_HAS_MIL == ON)
#include "vidMILDevice.h"
#endif // CISST_SVL_HAS_MIL

using namespace std;

#define MAX_PROPERTIES_BUFFER_SIZE      65536


/*************************************/
/*** svlVideoCaptureSource class *****/
/*************************************/

svlVideoCaptureSource::svlVideoCaptureSource(bool stereo) :
    svlFilterBase(),
    EnumeratedDevices(0),
    NumberOfEnumeratedDevices(-1),
    FormatList(0),
    FormatListSize(0)
{
    if (stereo) {
        NumberOfChannels = 2;
        SetFilterToSource(svlTypeImageRGBStereo);
        // forcing output sample to handle external buffers
        OutputData = new svlSampleImageRGBStereo(false);
    }
    else {
        NumberOfChannels = 1;
        SetFilterToSource(svlTypeImageRGB);
        // forcing output sample to handle external buffers
        OutputData = new svlSampleImageRGB(false);
    }

    // Get the number of supported capture APIs
    NumberOfSupportedAPIs = 0;
#if (CISST_SVL_HAS_SVS == ON)
    NumberOfSupportedAPIs ++;
#endif // CISST_SVL_HAS_SVS
#if (CISST_SVL_HAS_DIRECTSHOW == ON)
    NumberOfSupportedAPIs ++;
#endif // CISST_SVL_HAS_DIRECTSHOW
#if (CISST_SVL_HAS_VIDEO4LINUX2 == ON)
    NumberOfSupportedAPIs ++;
#endif // CISST_SVL_HAS_VIDEO4LINUX2
#if (CISST_SVL_HAS_LIBDC1394 == ON)
    NumberOfSupportedAPIs ++;
#endif // CISST_SVL_HAS_LIBDC1394
#if (CISST_SVL_HAS_OPENCV == ON)
    NumberOfSupportedAPIs ++;
#endif // CISST_SVL_HAS_OPENCV
#if (CISST_SVL_HAS_MIL == ON)
    NumberOfSupportedAPIs ++;
#endif // CISST_SVL_HAS_MIL

    // Allocate capture API handler array
    DeviceObj = new CVideoCaptureSourceBase*[NumberOfSupportedAPIs];
    DeviceID = new int[NumberOfChannels];
    InputID = new int[NumberOfChannels];
    Format = new ImageFormat*[NumberOfChannels];
    Properties = new ImageProperties*[NumberOfChannels];
    DevSpecConfigBuffer = new unsigned char*[NumberOfChannels];
    DevSpecConfigBufferSize = new unsigned int[NumberOfChannels];
    APIChannelID = new int[NumberOfChannels];
    APIDeviceID = new int[NumberOfChannels];
    API = new int[NumberOfChannels];

    unsigned int i;
    for (i = 0; i < NumberOfSupportedAPIs; i ++) {
        DeviceObj[i] = 0;
    }
    for (i = 0; i < NumberOfChannels; i ++) {
        DeviceID[i] = -1;
        InputID[i] = -1;
        Format[i] = 0;
        Properties[i] = 0;
        DevSpecConfigBuffer[i] = 0;
        DevSpecConfigBufferSize[i] = 0;
        APIChannelID[i] = -1;
        APIDeviceID[i] = -1;
        API[i] = -1;
    }
}

svlVideoCaptureSource::~svlVideoCaptureSource()
{
    Release();

    if (OutputData) delete OutputData;
    if (DeviceObj) delete [] DeviceObj;
    if (DeviceID) delete [] DeviceID;
    if (InputID) delete [] InputID;
    if (Format) delete [] Format;
    if (Properties) delete [] Properties;
    if (DevSpecConfigBuffer) {
        for (unsigned int i = 0; i < NumberOfChannels; i ++) {
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
        for (int i = 0; i < NumberOfEnumeratedDevices; i ++) {
            if (FormatList[i]) delete [] FormatList[i];
        }
        delete [] FormatList;
    }
    if (FormatListSize) delete [] FormatListSize;
}

int svlVideoCaptureSource::Initialize(svlSample* inputdata)
{
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

        if (platform == WinDirectShow) {
#if (CISST_SVL_HAS_DIRECTSHOW == ON)
            // DirectShow does not use the Format structure.
            // Instead, it has its own custom configuration format.
            if (DevSpecConfigBuffer[i] && DevSpecConfigBufferSize[i] > 0) {
                dynamic_cast<CDirectShowSource*>(DeviceObj[API[i]])->SetMediaType(DevSpecConfigBuffer[i], DevSpecConfigBufferSize[i], APIChannelID[i]);
            }
#endif // CISST_SVL_HAS_DIRECTSHOW
        }
        else if (platform == MatroxImaging) {
#if (CISST_SVL_HAS_MIL == ON)
            // Check if Matrox device supports capture
            if (dynamic_cast<CMILDevice*>(DeviceObj[API[i]])->EnableCapture(APIDeviceID[i]) == false) {
                ret = SVL_VCS_UNABLE_TO_OPEN;
                goto labError;
            }
#endif // CISST_SVL_HAS_MIL
        }
        else {
            if (Format[i]) DeviceObj[API[i]]->SetFormat(Format[i][0], APIChannelID[i]);
        }
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

        if (platform == WinDirectShow) {
#if (CISST_SVL_HAS_DIRECTSHOW == ON)
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
            dynamic_cast<svlSampleImageRGBStereo*>(OutputData)->SetData(DeviceObj[API[i]]->GetLatestFrame(false, APIChannelID[i]), i);
        }
        else {
            dynamic_cast<svlSampleImageRGB*>(OutputData)->SetData(DeviceObj[API[i]]->GetLatestFrame(false));
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

int svlVideoCaptureSource::ProcessFrame(ProcInfo* procInfo, svlSample* inputdata)
{
    svlImageRGB* image;
    unsigned int idx;

    _ParallelLoop(procInfo, idx, NumberOfChannels)
    {
        // Requesting frame from the capture buffer
        image = DeviceObj[API[idx]]->GetLatestFrame(true, APIChannelID[idx]);
        if (image == 0) return SVL_FAIL;
        if (NumberOfChannels == 1) {
            dynamic_cast<svlSampleImageRGB*>(OutputData)->SetData(image, idx);
        }
        else {
            dynamic_cast<svlSampleImageRGBStereo*>(OutputData)->SetData(image, idx);
        }
    }

    return SVL_OK;
}

int svlVideoCaptureSource::Release()
{
    for (unsigned int i = 0; i < NumberOfSupportedAPIs; i ++) {
        if (DeviceObj[i]) {
            if (DeviceObj[i]->GetPlatformType() == MatroxImaging) {
#if (CISST_SVL_HAS_MIL == ON)
                // Object is a singleton, should not be deleted
                dynamic_cast<CMILDevice*>(DeviceObj[i])->Release();
#endif // CISST_SVL_HAS_MIL
            }
            else {
                delete DeviceObj[i];
            }
            DeviceObj[i] = 0;
        }
    }
    return SVL_OK;
}

int svlVideoCaptureSource::CreateCaptureAPIHandlers()
{
    int ret = SVL_FAIL;
    unsigned int i, j;
    unsigned int *chperapi = new unsigned int[NumberOfSupportedAPIs];
    for (j = 0; j < NumberOfSupportedAPIs; j ++) chperapi[j] = 0;

    // Make sure devices are enumerated
    GetDeviceList(0);

    // Count the number of video channels requested from capture APIs
    // and create API look up table for easy device handler access
    for (i = 0; i < NumberOfChannels; i ++) {
        if (DeviceID[i] < 0 || DeviceID[i] >= NumberOfEnumeratedDevices) goto labError;
        j = 0;
#if (CISST_SVL_HAS_SVS == ON)
        if (EnumeratedDevices[DeviceID[i]].platform == WinSVS) {
            // getting API specific device and channel IDs
            APIDeviceID[i] = EnumeratedDevices[DeviceID[i]].id;
            APIChannelID[i] = chperapi[j];

            API[i] = j;
            chperapi[j] ++;
        }
        j ++;
#endif // CISST_SVL_HAS_SVS
#if (CISST_SVL_HAS_DIRECTSHOW == ON)
        if (EnumeratedDevices[DeviceID[i]].platform == WinDirectShow) {
            // getting API specific device and channel IDs
            APIDeviceID[i] = EnumeratedDevices[DeviceID[i]].id;
            APIChannelID[i] = chperapi[j];

            API[i] = j;
            chperapi[j] ++;
        }
        j ++;
#endif // CISST_SVL_HAS_DIRECTSHOW
#if (CISST_SVL_HAS_VIDEO4LINUX2 == ON)
        if (EnumeratedDevices[DeviceID[i]].platform == LinVideo4Linux2) {
            // getting API specific device and channel IDs
            APIDeviceID[i] = EnumeratedDevices[DeviceID[i]].id;
            APIChannelID[i] = chperapi[j];

            API[i] = j;
            chperapi[j] ++;
        }
        j ++;
#endif // CISST_SVL_HAS_VIDEO4LINUX2
#if (CISST_SVL_HAS_LIBDC1394 == ON)
        if (EnumeratedDevices[DeviceID[i]].platform == LinLibDC1394) {
            // getting API specific device and channel IDs
            APIDeviceID[i] = EnumeratedDevices[DeviceID[i]].id;
            APIChannelID[i] = chperapi[j];

            API[i] = j;
            chperapi[j] ++;
        }
        j ++;
#endif // CISST_SVL_HAS_LIBDC1394
#if (CISST_SVL_HAS_OPENCV == ON)
        if (EnumeratedDevices[DeviceID[i]].platform == OpenCV) {
            // getting API specific device and channel IDs
            APIDeviceID[i] = EnumeratedDevices[DeviceID[i]].id;
            APIChannelID[i] = chperapi[j];

            API[i] = j;
            chperapi[j] ++;
        }
        j ++;
#endif // CISST_SVL_HAS_OPENCV
#if (CISST_SVL_HAS_MIL == ON)
        if (EnumeratedDevices[DeviceID[i]].platform == MatroxImaging) {
            // getting API specific device and channel IDs
            APIDeviceID[i] = EnumeratedDevices[DeviceID[i]].id;
            APIChannelID[i] = chperapi[j];

            API[i] = j;
            chperapi[j] ++;
        }
        j ++;
#endif // CISST_SVL_HAS_MIL
    }

    // Instantiate capture device handlers and
    // set the number of channels requested
    j = 0;
#if (CISST_SVL_HAS_SVS == ON)
    if (chperapi[j] > 0) {
        DeviceObj[j] = new CSVSSource();
        if (DeviceObj[j]->SetStreamCount(chperapi[j]) != SVL_OK) goto labError;
    }
    j ++;
#endif // CISST_SVL_HAS_SVS
#if (CISST_SVL_HAS_DIRECTSHOW == ON)
    if (chperapi[j] > 0) {
        DeviceObj[j] = new CDirectShowSource();
        if (DeviceObj[j]->SetStreamCount(chperapi[j]) != SVL_OK) goto labError;
    }
    j ++;
#endif // CISST_SVL_HAS_DIRECTSHOW
#if (CISST_SVL_HAS_VIDEO4LINUX2 == ON)
    if (chperapi[j] > 0) {
        DeviceObj[j] = new CV4L2Source();
        if (DeviceObj[j]->SetStreamCount(chperapi[j]) != SVL_OK) goto labError;
    }
    j ++;
#endif // CISST_SVL_HAS_VIDEO4LINUX2
#if (CISST_SVL_HAS_LIBDC1394 == ON)
    if (chperapi[j] > 0) {
        DeviceObj[j] = new CDC1394Source();
        if (DeviceObj[j]->SetStreamCount(chperapi[j]) != SVL_OK) goto labError;
    }
    j ++;
#endif // CISST_SVL_HAS_LIBDC1394
#if (CISST_SVL_HAS_OPENCV == ON)
    if (chperapi[j] > 0) {
        DeviceObj[j] = new COpenCVSource();
        if (DeviceObj[j]->SetStreamCount(chperapi[j]) != SVL_OK) goto labError;
    }
    j ++;
#endif // CISST_SVL_HAS_OPENCV
#if (CISST_SVL_HAS_MIL == ON)
    if (chperapi[j] > 0) {
        DeviceObj[j] = CMILDevice::GetInstance();
        if (DeviceObj[j]->SetStreamCount(chperapi[j]) != SVL_OK) goto labError;
    }
    j ++;
#endif // CISST_SVL_HAS_MIL

    ret = SVL_OK;

labError:
    if (ret != SVL_OK) {
        for (j = 0; j < NumberOfSupportedAPIs; j ++) {
            if (DeviceObj[j]) delete DeviceObj[j];
            DeviceObj[j] = 0;
        }
    }
    if (chperapi) delete [] chperapi;

    return ret;
}

int svlVideoCaptureSource::GetWidth(unsigned int videoch)
{
    if (IsDataValid(GetOutputType(), OutputData) != SVL_OK)
        return SVL_FAIL;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    return dynamic_cast<svlSampleImageBase*>(OutputData)->GetWidth(videoch);
}

int svlVideoCaptureSource::GetHeight(unsigned int videoch)
{
    if (IsDataValid(GetOutputType(), OutputData) != SVL_OK)
        return SVL_FAIL;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    return dynamic_cast<svlSampleImageBase*>(OutputData)->GetHeight(videoch);
}

int svlVideoCaptureSource::DialogSetup(unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    int deviceid, inputid;

    cout << " === Capture device selection ===" << endl;
    deviceid = DialogDevice();
    if (deviceid < 0) return SVL_FAIL;

    cout << endl << "  ==== Device input selection ====" << endl;
    inputid = DialogInput(deviceid);

    SetDevice(deviceid, inputid, videoch);

    cout << endl << "  ===== Setup capture format =====" << endl;
    DialogFormat(videoch);
    
    return SVL_OK;
}

int svlVideoCaptureSource::DialogDevice()
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    int deviceid, listsize;

    listsize = PrintDeviceList();
    if (listsize < 1) {
        cout << " -!- No video capture devices have been found." << endl;
        return SVL_FAIL;
    }

    cout << endl << " # Enter device ID: ";
    cin >> deviceid;
    if (deviceid < 0) deviceid = 0;
    if (deviceid >= listsize) deviceid = listsize - 1;
    
    return deviceid;
}

int svlVideoCaptureSource::DialogInput(unsigned int deviceid)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    int inputid, listsize;

    listsize = PrintInputList(deviceid);
    if (listsize > 0) {
        cout << endl << "  # Enter input ID: ";
        cin >> inputid;
        if (inputid < 0) inputid = 0;
        if (inputid >= listsize) inputid = listsize - 1;
    }
    else {
        cout << "  -!- Input selection not available." << endl;
        inputid = 0;
    }

    return inputid;
}

int svlVideoCaptureSource::DialogFormat(unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;
    if (DeviceID[videoch] < 0)
        return SVL_VCS_UNABLE_TO_OPEN;

    // Make sure devices are enumerated
    GetDeviceList(0);

    // Check if device ID is in range
    if (DeviceID[videoch] >= NumberOfEnumeratedDevices)
        return SVL_VCS_UNABLE_TO_OPEN;

    // Get capture API platform
    PlatformType platform = EnumeratedDevices[DeviceID[videoch]].platform;

    if (platform == WinDirectShow) {
#if (CISST_SVL_HAS_DIRECTSHOW == ON)
        // Create temporary DirectShow capture module and initialize it
        CDirectShowSource device;
        device.SetStreamCount(1);
        if (device.SetDevice(EnumeratedDevices[DeviceID[videoch]].id,
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

        int formatid, listsize;

        listsize = PrintFormatList(videoch);
        if (listsize > 0) {
            cout << endl << "  # Enter format ID: ";
            cin >> formatid;
            if (formatid < 0) formatid = 0;
            if (formatid >= listsize) formatid = listsize - 1;

            SelectFormat(formatid, videoch);
        }
        else {
            cout << "  -!- Format selection not available." << endl;
        }
    }

    return SVL_OK;
}

int svlVideoCaptureSource::DialogImageProperties(unsigned int videoch)
{
    // Available only after initialization
    if (IsInitialized() == false)
        return SVL_NOT_YET_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    // Get capture API platform and API specific device ID
    PlatformType platform = DeviceObj[API[videoch]]->GetPlatformType();

    if (platform == WinDirectShow) {
#if (CISST_SVL_HAS_DIRECTSHOW == ON)
        CDirectShowSource* device = dynamic_cast<CDirectShowSource*>(DeviceObj[API[videoch]]);
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
        svlVideoCaptureSourceDialogThread* proc;
        osaThread* thread;

        proc = new svlVideoCaptureSourceDialogThread(videoch);
        thread = new osaThread;
        thread->Create<svlVideoCaptureSourceDialogThread, svlVideoCaptureSource*>(proc,
                                                                                  &svlVideoCaptureSourceDialogThread::Proc,
                                                                                  this);
        if (proc->WaitForInit()) {
            GetImageProperties(videoch);

#ifdef __GNUC__
            ////////////////////////////////////////////////////
            // modify terminal settings for single key inputs
            struct  termios ksettings;
            struct  termios new_ksettings;
            int     kbrd;
            kbrd = open("/dev/tty",O_RDWR);
 #if (CISST_OS == CISST_LINUX)
            ioctl(kbrd, TCGETS, &ksettings);
            new_ksettings = ksettings;
            new_ksettings.c_lflag &= !ICANON;
            new_ksettings.c_lflag &= !ECHO;
            ioctl(kbrd, TCSETS, &new_ksettings);
            ioctl(kbrd, TIOCNOTTY);
 #endif // (CISST_OS == CISST_LINUX)
 #if (CISST_OS == CISST_DARWIN)
            ioctl(kbrd, TIOCGETA, &ksettings);
            new_ksettings = ksettings;
            new_ksettings.c_lflag &= !ICANON;
            new_ksettings.c_lflag &= !ECHO;
            ioctl(kbrd, TIOCSETA, &new_ksettings);
            ////////////////////////////////////////////////////
 #endif // (CISST_OS == CISST_DARWIN)
#endif

            // wait for keyboard input in command window
#ifdef _WIN32
            int ch;
#endif
#ifdef __GNUC__
            char ch;
#endif
            do {
#ifdef _WIN32
                ch = _getch();
#endif
#ifdef __GNUC__
                ch = getchar();
#endif
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

#ifdef __GNUC__
            ////////////////////////////////////////////////////
            // reset terminal settings    
 #if (CISST_OS == CISST_LINUX)
            ioctl(kbrd, TCSETS, &ksettings);
 #endif // (CISST_OS == CISST_LINUX)
 #if (CISST_OS == CISST_DARWIN)
            ioctl(kbrd, TIOCSETA, &ksettings);
 #endif // (CISST_OS == CISST_DARWIN)
            close(kbrd);
            ////////////////////////////////////////////////////
#endif

        }

        proc->Kill();
        thread->Wait();
        delete thread;
        delete proc;

        return ret;
    }

    return SVL_OK;
}

int svlVideoCaptureSource::GetDeviceList(DeviceInfo **deviceinfolist, bool update)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

    if (NumberOfEnumeratedDevices < 0 || update) {
        // First enumeration or update
        int i;
        unsigned int j, sum;
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

        // Enumerate API by API and store all results
        j = 0;
#if (CISST_SVL_HAS_SVS == ON)
        apideviceinfos[j] = 0;
        apidevicecounts[j] = 0;
        CSVSSource *svsdevice = new CSVSSource();
        if (svsdevice) {
            apidevicecounts[j] = svsdevice->GetDeviceList(&(apideviceinfos[j]));
            if (apidevicecounts[j] > 0) {
                apiformats[j] = new ImageFormat*[apidevicecounts[j]];
                apiformatcounts[j] = new int[apidevicecounts[j]];
                for (i = 0; i < apidevicecounts[j]; i ++)
                    apiformatcounts[j][i] = svsdevice->GetFormatList(i, &(apiformats[j][i]));
            }
            delete svsdevice;
            if (apidevicecounts[j] > 0) NumberOfEnumeratedDevices += apidevicecounts[j];
        }
        j ++;
#endif // CISST_SVL_HAS_SVS
#if (CISST_SVL_HAS_DIRECTSHOW == ON)
        apideviceinfos[j] = 0;
        apidevicecounts[j] = 0;
        CDirectShowSource *dsdevice = new CDirectShowSource();
        if (dsdevice) {
            apidevicecounts[j] = dsdevice->GetDeviceList(&(apideviceinfos[j]));
            if (apidevicecounts[j] > 0) {
                apiformats[j] = new ImageFormat*[apidevicecounts[j]];
                apiformatcounts[j] = new int[apidevicecounts[j]];
                for (i = 0; i < apidevicecounts[j]; i ++)
                    apiformatcounts[j][i] = dsdevice->GetFormatList(i, &(apiformats[j][i]));
            }
            delete dsdevice;
            if (apidevicecounts[j] > 0) NumberOfEnumeratedDevices += apidevicecounts[j];
        }
        j ++;
#endif // CISST_SVL_HAS_DIRECTSHOW
#if (CISST_SVL_HAS_VIDEO4LINUX2 == ON)
        apideviceinfos[j] = 0;
        apidevicecounts[j] = 0;
        CV4L2Source *v4l2device = new CV4L2Source();
        if (v4l2device) {
            apidevicecounts[j] = v4l2device->GetDeviceList(&(apideviceinfos[j]));
            if (apidevicecounts[j] > 0) {
                apiformats[j] = new ImageFormat*[apidevicecounts[j]];
                apiformatcounts[j] = new int[apidevicecounts[j]];
                for (i = 0; i < apidevicecounts[j]; i ++)
                    apiformatcounts[j][i] = v4l2device->GetFormatList(i, &(apiformats[j][i]));
            }
            delete v4l2device;
            if (apidevicecounts[j] > 0) NumberOfEnumeratedDevices += apidevicecounts[j];
        }
        j ++;
#endif // CISST_SVL_HAS_VIDEO4LINUX2
#if (CISST_SVL_HAS_LIBDC1394 == ON)
        apideviceinfos[j] = 0;
        apidevicecounts[j] = 0;
        CDC1394Source *dc1394device = new CDC1394Source();
        if (dc1394device) {
            apidevicecounts[j] = dc1394device->GetDeviceList(&(apideviceinfos[j]));
            if (apidevicecounts[j] > 0) {
                apiformats[j] = new ImageFormat*[apidevicecounts[j]];
                apiformatcounts[j] = new int[apidevicecounts[j]];
                for (i = 0; i < apidevicecounts[j]; i ++)
                    apiformatcounts[j][i] = dc1394device->GetFormatList(i, &(apiformats[j][i]));
            }
            delete dc1394device;
            if (apidevicecounts[j] > 0) NumberOfEnumeratedDevices += apidevicecounts[j];
        }
        j ++;
#endif // CISST_SVL_HAS_LIBDC1394
#if (CISST_SVL_HAS_OPENCV == ON)
        apideviceinfos[j] = 0;
        apidevicecounts[j] = 0;
        COpenCVSource *ocvdevice = new COpenCVSource();
        if (ocvdevice) {
            apidevicecounts[j] = ocvdevice->GetDeviceList(&(apideviceinfos[j]));
            if (apidevicecounts[j] > 0) {
                apiformats[j] = new ImageFormat*[apidevicecounts[j]];
                apiformatcounts[j] = new int[apidevicecounts[j]];
                for (i = 0; i < apidevicecounts[j]; i ++)
                    apiformatcounts[j][i] = ocvdevice->GetFormatList(i, &(apiformats[j][i]));
            }
            delete ocvdevice;
            if (apidevicecounts[j] > 0) NumberOfEnumeratedDevices += apidevicecounts[j];
        }
        j ++;
#endif // CISST_SVL_HAS_OPENCV
#if (CISST_SVL_HAS_MIL == ON)
        apideviceinfos[j] = 0;
        apidevicecounts[j] = 0;
        CMILDevice *mildevice = CMILDevice::GetInstance();
        if (mildevice) {
            apidevicecounts[j] = mildevice->GetDeviceList(&(apideviceinfos[j]));
            if (apidevicecounts[j] > 0) {
                apiformats[j] = new ImageFormat*[apidevicecounts[j]];
                apiformatcounts[j] = new int[apidevicecounts[j]];
                for (i = 0; i < apidevicecounts[j]; i ++)
                    apiformatcounts[j][i] = mildevice->GetFormatList(i, &(apiformats[j][i]));
            }
            if (apidevicecounts[j] > 0) NumberOfEnumeratedDevices += apidevicecounts[j];
        }
        j ++;
#endif // CISST_SVL_HAS_MIL

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
    }

    if (deviceinfolist && NumberOfEnumeratedDevices > 0) {
        deviceinfolist[0] = new DeviceInfo[NumberOfEnumeratedDevices];
        // The caller is responsible for deleting the deviceinfolist array after calling
        memcpy(deviceinfolist[0], EnumeratedDevices, NumberOfEnumeratedDevices * sizeof(DeviceInfo));
    }

    return NumberOfEnumeratedDevices;
}

void svlVideoCaptureSource::ReleaseDeviceList(DeviceInfo *deviceinfolist)
{
    if (deviceinfolist) delete [] deviceinfolist;
}

int svlVideoCaptureSource::PrintDeviceList(bool update)
{
    DeviceInfo *devices = 0;
    int devicecount = GetDeviceList(&devices, update);

    for (int i = 0; i < devicecount; i ++) {
        cout << " " << i << ") " << devices[i].name << endl;
    }

    ReleaseDeviceList(devices);

    return devicecount;
}

int svlVideoCaptureSource::PrintInputList(int deviceid, bool update)
{
    DeviceInfo *devices = 0;
    int devicecount = GetDeviceList(&devices, update);

    if (deviceid >= devicecount) {
        ReleaseDeviceList(devices);
        return SVL_FAIL;
    }

    int inputcount = devices[deviceid].inputcount;
    for (int i = 0; i < inputcount; i ++) {
        cout << "  " << i << ") " << devices[deviceid].inputnames[i] << endl;
    }

    ReleaseDeviceList(devices);

    return inputcount;
}

int svlVideoCaptureSource::SetDevice(int deviceid, int inputid, unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    DeviceID[videoch] = deviceid;
    InputID[videoch] = inputid;

    return SVL_OK;
}

int svlVideoCaptureSource::GetFormatList(ImageFormat **formatlist, unsigned int videoch)
{
    if (formatlist == 0)
        return SVL_FAIL;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;
    if (DeviceID[videoch] < 0)
        return SVL_VCS_UNABLE_TO_OPEN;

    // Make sure devices are enumerated
    GetDeviceList(0);

    if (FormatList[DeviceID[videoch]] == 0 || FormatListSize[DeviceID[videoch]] < 1) return SVL_FAIL;

    formatlist[0] = new ImageFormat[FormatListSize[DeviceID[videoch]]];
    memcpy(formatlist[0], FormatList[DeviceID[videoch]], FormatListSize[DeviceID[videoch]] * sizeof(ImageFormat));

    return FormatListSize[DeviceID[videoch]];
}

void svlVideoCaptureSource::ReleaseFormatList(ImageFormat *formatlist)
{
    if (formatlist) delete [] formatlist;
}

int svlVideoCaptureSource::PrintFormatList(unsigned int videoch)
{
    ImageFormat *formats = 0;
    int formatcount = GetFormatList(&formats, videoch);

    for (int i = 0; i < formatcount; i ++) {
        cout << "  " << i << ") " << formats[i].width << "x" << formats[i].height << " ";
        switch (formats[i].colorspace) {
            case PixelRGB8:
                cout << "RGB24";
            break;

            case PixelYUV444:
                cout << "YUV444";
            break;

            case PixelYUV422:
                cout << "YUV422";
            break;

            case PixelYUV411:
                cout << "YUV411";
            break;

            case PixelMONO8:
                cout << "Mono8";
            break;

            case PixelMONO16:
                cout << "Mono16";
            break;

            case PixelUnknown:
            default:
                cout << "Unknown color space";
            break;
        }
        if (formats[i].framerate > 0.0) {
            cout << " (<=" << formats[i].framerate << "fps)" << endl;
        }
        else {
            cout << " (unknown framerate)" << endl;
        }
    }

    ReleaseFormatList(formats);

    return formatcount;
}

int svlVideoCaptureSource::SelectFormat(unsigned int formatid, unsigned int videoch)
{
    ImageFormat *formats = 0;
    int formatcount = GetFormatList(&formats, videoch);
    int ret = SVL_FAIL;

    if (static_cast<int>(formatid) < formatcount) {
        ret = SetFormat(formats[formatid], videoch);
    }

    ReleaseFormatList(formats);

    return ret;
}

int svlVideoCaptureSource::SetFormat(ImageFormat& format, unsigned int videoch)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    if (Format[videoch] == 0) Format[videoch] = new ImageFormat;
    memcpy(Format[videoch], &format, sizeof(ImageFormat));

    return SVL_OK;
}

int svlVideoCaptureSource::GetFormat(ImageFormat& format, unsigned int videoch)
{
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;
    if (Format[videoch] == 0)
        return SVL_FAIL;

    memcpy(&format, Format[videoch], sizeof(ImageFormat));

    return SVL_OK;
}

int svlVideoCaptureSource::SetImageProperties(ImageProperties& properties, unsigned int videoch)
{
    // Available only after initialization
    if (IsInitialized() == false)
        return SVL_NOT_YET_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    if (DeviceObj[API[videoch]]->SetImageProperties(properties, APIChannelID[videoch]) == SVL_OK) {
        if (Properties[videoch] == 0) Properties[videoch] = new ImageProperties;
        memcpy(Properties[videoch], &properties, sizeof(ImageProperties));
        Properties[videoch]->mask = propShutter & propGain & propWhiteBalance & propBrightness & propGamma & propSaturation;
        return SVL_OK;
    }

    return SVL_FAIL;
}

int svlVideoCaptureSource::SetImageProperties(unsigned int videoch)
{
    // Available only after initialization
    if (IsInitialized() == false)
        return SVL_NOT_YET_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;
    if (Properties[videoch] == 0)
        return SVL_FAIL;

    return DeviceObj[API[videoch]]->SetImageProperties(Properties[videoch][0], APIChannelID[videoch]);
}

int svlVideoCaptureSource::GetImageProperties(ImageProperties& properties, unsigned int videoch)
{
    // Available only after initialization
    if (IsInitialized() == false)
        return SVL_NOT_YET_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    if (DeviceObj[API[videoch]]->GetImageProperties(properties, APIChannelID[videoch]) != SVL_OK) return SVL_FAIL;

    if (Properties[videoch] == 0) Properties[videoch] = new ImageProperties;
    memcpy(Properties[videoch], &properties, sizeof(ImageProperties));
    Properties[videoch]->mask = propShutter & propGain & propWhiteBalance & propBrightness & propGamma & propSaturation;

    return SVL_OK;
}

int svlVideoCaptureSource::GetImageProperties(unsigned int videoch)
{
    // Available only after initialization
    if (IsInitialized() == false)
        return SVL_NOT_YET_INITIALIZED;
    if (videoch >= NumberOfChannels)
        return SVL_WRONG_CHANNEL;

    if (Properties[videoch] == 0) Properties[videoch] = new ImageProperties;
    Properties[videoch]->mask = propShutter & propGain & propWhiteBalance & propBrightness & propGamma & propSaturation;

    return DeviceObj[API[videoch]]->GetImageProperties(Properties[videoch][0], APIChannelID[videoch]);
}

int svlVideoCaptureSource::SaveSettings(const char* filepath)
{
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

    // Make sure devices are enumerated
    GetDeviceList(0);

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

int svlVideoCaptureSource::LoadSettings(const char* filepath)
{
    if (IsInitialized() == true)
        return SVL_ALREADY_INITIALIZED;

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
        return SVL_FAIL;
    }

    // Make sure devices are enumerated
    GetDeviceList(0);

    // For each channel
    for (i = 0; i < NumberOfChannels; i ++) {

		// Read "device id"
        readlen = static_cast<unsigned int>(fread(&intvalue, sizeof(int), 1, fp));
		if (readlen < 1) goto labError;
        if (intvalue < -1 || intvalue >= NumberOfEnumeratedDevices) goto labError;
        DeviceID[i] = intvalue;

        // Read "device name"
        readlen = static_cast<unsigned int>(fread(buffer, SVL_VCS_STRING_LENGTH, 1, fp));
		if (readlen < 1) goto labError;
        if (DeviceID[i] >= 0) {
            buffer[SVL_VCS_STRING_LENGTH - 1] = 0;
            if (strcmp(EnumeratedDevices[DeviceID[i]].name, buffer) != 0) goto labError;
        }

        // Read "input id"
        readlen = static_cast<unsigned int>(fread(&intvalue, sizeof(int), 1, fp));
		if (readlen < 1) goto labError;
        if (DeviceID[i] >= 0) InputID[i] = intvalue;
        else InputID[i] = -1;

        // Read "format size"
        readlen = static_cast<unsigned int>(fread(&intvalue, sizeof(int), 1, fp));
		if (readlen < 1) goto labError;

        // Read "format"
        if (intvalue == sizeof(ImageFormat)) {
            if (Format[i] == 0) Format[i] = new ImageFormat;
            readlen = static_cast<unsigned int>(fread(Format[i], 1, sizeof(ImageFormat), fp));
            if (readlen < sizeof(ImageFormat)) {
                delete Format[i];
                Format[i] = 0;
                goto labError;
            }
        }

        // Read "properties size"
        readlen = static_cast<unsigned int>(fread(&intvalue, sizeof(int), 1, fp));
		if (readlen < 1) goto labError;

        // Read "properties"
        if (intvalue == sizeof(ImageProperties)) {
            if (Properties[i] == 0) Properties[i] = new ImageProperties;
            readlen = static_cast<unsigned int>(fread(Properties[i], 1, sizeof(ImageProperties), fp));
            if (readlen < sizeof(ImageProperties)) {
                delete Properties[i];
                Properties[i] = 0;
                goto labError;
            }
        }

        // Read "device specific configuration buffer size"
        readlen = static_cast<unsigned int>(fread(&uintvalue, sizeof(unsigned int), 1, fp));
		if (readlen < 1) goto labError;
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
    		if (readlen < uintvalue) goto labError;
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


/***********************************************/
/*** svlVideoCaptureSourceDialogThread class ***/
/***********************************************/

void* svlVideoCaptureSourceDialogThread::Proc(svlVideoCaptureSource* baseref)
{
    // signal success to main thread
    InitSuccess = true;
    InitEvent.Raise();

    svlVideoCaptureSource::ImageProperties properties;

    // Create an empty line under the table
    printf("\r\n\x1b[1A");

    while (!Stopped) {

        if (baseref->GetImageProperties(properties, VideoChannel) == SVL_OK) {

            if (properties.manual & svlVideoCaptureSource::propShutter) {
                printf("\r  |  %7d |", properties.shutter);
            }
            else {
                printf("\r  |  %6da |", properties.shutter);
            }
            if (properties.manual & svlVideoCaptureSource::propGain) {
                printf("  %6d |", properties.gain);
            }
            else {
                printf("  %5da |", properties.gain);
            }
            if (properties.manual & svlVideoCaptureSource::propWhiteBalance) {
                printf("  %5d | %6d |", properties.wb_u_b, properties.wb_v_r);
            }
            else {
                printf("  %4da | %5da |", properties.wb_u_b, properties.wb_v_r);
            }
            if (properties.manual & svlVideoCaptureSource::propBrightness) {
                printf("  %9d |", properties.shutter);
            }
            else {
                printf("  %8da |", properties.shutter);
            }
            if (properties.manual & svlVideoCaptureSource::propGamma) {
                printf("  %7d |", properties.gamma);
            }
            else {
                printf("  %6da |", properties.gamma);
            }
            if (properties.manual & svlVideoCaptureSource::propSaturation) {
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

