/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlVideoCaptureSource.h 545 2009-07-14 08:38:33Z adeguet1 $
  
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

#ifndef _svlFilterSourceVideoCapture_h
#define _svlFilterSourceVideoCapture_h

#include <cisstStereoVision/svlStreamManager.h>
#include <string.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

#define SVL_VCS_DEVICE_NOT_INITIALIZED      -2000
#define SVL_VCS_UNABLE_TO_OPEN              -2001
#define SVL_VCS_UNABLE_TO_START_CAPTURE     -2002
#define SVL_VCS_UNABLE_TO_SET_INPUT         -2003
#define SVL_VCS_UNSUPPORTED_COLORSPACE      -2004
#define SVL_VCS_UNSUPPORTED_SIZE            -2005

#define SVL_VCS_ARRAY_LENGTH                50
#define SVL_VCS_STRING_LENGTH               128


class CVideoCaptureSourceBase;
class CVideoCaptureSourceDialogThread;

class CISST_EXPORT svlFilterSourceVideoCapture : public svlFilterSourceBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

friend class CVideoCaptureSourceDialogThread;

public:
    enum PlatformType {
        WinDirectShow   = 0,
        WinSVS          = 1,
        LinVideo4Linux2 = 2,
        LinLibDC1394    = 3,
        OpenCV          = 4,
        MatroxImaging   = 5,
        NumberOfPlatformTypes
    };

    typedef struct _DeviceInfo {
	    int id;
	    char name[SVL_VCS_STRING_LENGTH];
        PlatformType platform;
	    int inputcount;
	    int activeinput;
	    char inputnames[SVL_VCS_ARRAY_LENGTH][SVL_VCS_STRING_LENGTH];
	    bool testok;
    } DeviceInfo;

    typedef enum _PixelType {
        PixelRAW8    = 0,   //  8 bits per pixel
        PixelRAW16   = 1,   // 16 bits per pixel
        PixelRGB8    = 2,   // 24 bits per pixel
        PixelYUV444  = 3,   // 24 bits per pixel
        PixelYUV422  = 4,   // 16 bits per pixel
        PixelYUV411  = 5,   // 12 bits per pixel
        PixelMONO8   = 6,   //  8 bits per pixel
        PixelMONO16  = 7,   // 16 bits per pixel
        PixelUnknown = 8,
        PixelTypeCount
    } PixelType;

    typedef enum _PatternType {
        PatternRGGB    = 0,
        PatternGBRG    = 1,
        PatternGRBG    = 2,
        PatternBGGR    = 3,
        PatternUnknown = 4,
        PatternTypeCount
    } PatternType;

    typedef struct _ImageFormat {
        unsigned int    width;                  // read/write
        unsigned int    height;                 // read/write
        PixelType       colorspace;             // read/write
        double          framerate;              // read/write
        bool            rgb_order;              // read/write
        bool            yuyv_order;             // read/write
        int             custom_mode;            // read/write
        unsigned int    custom_roileft;         // read/write
        unsigned int    custom_roitop;          // read/write
        unsigned int    custom_framerate;       // write
        unsigned int    custom_maxwidth;        // read only
        unsigned int    custom_maxheight;       // read only
        unsigned int    custom_unitwidth;       // read only
        unsigned int    custom_unitheight;      // read only
        unsigned int    custom_unitleft;        // read only
        unsigned int    custom_unittop;         // read only
        PixelType       custom_colorspaces[PixelTypeCount]; // read only
        PatternType     custom_pattern;         // read only
    } ImageFormat;

    typedef struct _ExternalTrigger {
        bool            enable;
        unsigned int    mode;
        unsigned int    source;
        unsigned int    polarity;
    } ExternalTrigger;

    typedef enum _ImagePropertiesMask {
        propShutter      = 1,
        propGain         = 1 << 1,
        propWhiteBalance = 1 << 2,
        propBrightness   = 1 << 3,
        propGamma        = 1 << 4,
        propSaturation   = 1 << 5,
    } ImagePropertiesMask;

    typedef struct _ImageProperties {
        unsigned int mask;
        unsigned int manual;
        unsigned int shutter;
        unsigned int gain;
        unsigned int wb_u_b;
        unsigned int wb_v_r;
        unsigned int brightness;
        unsigned int gamma;
        unsigned int saturation;
    } ImageProperties;

    svlFilterSourceVideoCapture();
    svlFilterSourceVideoCapture(unsigned int channelcount);
    virtual ~svlFilterSourceVideoCapture();

    int SetChannelCount(unsigned int channelcount);

    double GetTargetFrequency();
    int SetTargetFrequency(double hertz);

    int DialogSetup(unsigned int videoch = SVL_LEFT);
    int DialogDevice();
    int DialogInput(unsigned int deviceid);
    int DialogFormat(unsigned int videoch = SVL_LEFT);
    int DialogTrigger(unsigned int videoch = SVL_LEFT);
    int DialogImageProperties(unsigned int videoch = SVL_LEFT);

    int GetDeviceList(DeviceInfo **deviceinfolist, bool update = false);
    void ReleaseDeviceList(DeviceInfo *deviceinfolist);
    int PrintDeviceList(bool update = false);
    int PrintInputList(int deviceid, bool update = false);
    int SetDevice(int deviceid, int inputid = 0, unsigned int videoch = SVL_LEFT);
    int GetDevice(int & deviceid, int & inputid, unsigned int videoch = SVL_LEFT);

    int GetFormatList(ImageFormat **formatlist, unsigned int videoch = SVL_LEFT);
    void ReleaseFormatList(ImageFormat *formatlist);
    int PrintFormatList(unsigned int videoch = SVL_LEFT);
    int SelectFormat(unsigned int formatid, unsigned int videoch = SVL_LEFT);
    int SetFormat(ImageFormat& format, unsigned int videoch = SVL_LEFT);
    int GetFormat(ImageFormat& format, unsigned int videoch = SVL_LEFT);
    int SetTrigger(ExternalTrigger& trigger, unsigned int videoch = SVL_LEFT);
    int GetTrigger(ExternalTrigger& trigger, unsigned int videoch = SVL_LEFT);
    int SetImageProperties(ImageProperties& properties, unsigned int videoch = SVL_LEFT);
    int GetImageProperties(ImageProperties& properties, unsigned int videoch = SVL_LEFT);
    std::string GetPixelTypeName(PixelType pixeltype);
    std::string GetPatternTypeName(PatternType patterntype);

    int SaveSettings(const char* filepath);
    int LoadSettings(const char* filepath);

protected:
    virtual int Initialize();
    virtual int ProcessFrame(ProcInfo* procInfo);
    virtual int Release();

private:
    unsigned int NumberOfChannels;
    unsigned int NumberOfSupportedAPIs;
    vctDynamicVector<cmnClassServicesBase*> SupportedAPIs;
    vctDynamicVector<PlatformType> APIPlatforms;
    DeviceInfo* EnumeratedDevices;
    int NumberOfEnumeratedDevices;
    ImageFormat **FormatList;
    int *FormatListSize;
    vctDynamicVector<cmnGenericObject*> DeviceGenObj;
    vctDynamicVector<CVideoCaptureSourceBase*> DeviceObj;
    int *API;
    int *APIDeviceID;
    int *APIChannelID;
    int *DeviceID;
    int *InputID;
    ImageFormat **Format;
    ImageProperties **Properties;
    ExternalTrigger *Trigger;
    unsigned char **DevSpecConfigBuffer;
    unsigned int *DevSpecConfigBufferSize;

    void InitializeCaptureAPIs();
    int CreateCaptureAPIHandlers();
    int SetImageProperties(unsigned int videoch = SVL_LEFT);
    int GetImageProperties(unsigned int videoch = SVL_LEFT);
};


class CISST_EXPORT CVideoCaptureSourceBase
{
public:
    CVideoCaptureSourceBase() {}
    virtual ~CVideoCaptureSourceBase() {}

    virtual svlFilterSourceVideoCapture::PlatformType GetPlatformType() = 0;
    virtual int SetStreamCount(unsigned int numofstreams) = 0;
	virtual int GetDeviceList(svlFilterSourceVideoCapture::DeviceInfo **deviceinfo) = 0;
	virtual int Open() = 0;
	virtual void Close() = 0;
	virtual int Start() = 0;
    virtual svlImageRGB* GetLatestFrame(bool waitfornew, unsigned int videoch = 0) = 0;
	virtual int Stop() = 0;
	virtual bool IsRunning() = 0;
    virtual int SetDevice(int devid, int inid, unsigned int videoch = 0) = 0;
	virtual int GetWidth(unsigned int videoch = 0) = 0;
	virtual int GetHeight(unsigned int videoch = 0) = 0;

    virtual int GetFormatList(unsigned int deviceid, svlFilterSourceVideoCapture::ImageFormat ** formatlist);
    virtual int SetFormat(svlFilterSourceVideoCapture::ImageFormat & format, unsigned int videoch = 0);
    virtual int GetFormat(svlFilterSourceVideoCapture::ImageFormat & format, unsigned int videoch = 0);
    virtual int SetImageProperties(svlFilterSourceVideoCapture::ImageProperties & properties, unsigned int videoch = 0);
    virtual int GetImageProperties(svlFilterSourceVideoCapture::ImageProperties & properties, unsigned int videoch = 0);
    virtual int SetTrigger(svlFilterSourceVideoCapture::ExternalTrigger & trigger, unsigned int videoch = 0);
    virtual int GetTrigger(svlFilterSourceVideoCapture::ExternalTrigger & trigger, unsigned int videoch = 0);
};


class CVideoCaptureSourceDialogThread
{
public:
    CVideoCaptureSourceDialogThread(unsigned int videoch) { VideoChannel = videoch; InitSuccess = false; Stopped = false; }
    ~CVideoCaptureSourceDialogThread() {}
    void* Proc(svlFilterSourceVideoCapture* baseref);

    bool WaitForInit() { InitEvent.Wait(); return InitSuccess; }
    void Kill() { Stopped = true; }

private:
    int VideoChannel;
    osaThreadSignal InitEvent;
    bool InitSuccess;
    bool Stopped;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceVideoCapture)

#endif // _svlFilterSourceVideoCapture_h

