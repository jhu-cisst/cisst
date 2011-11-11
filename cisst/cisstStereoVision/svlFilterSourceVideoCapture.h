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

#ifndef _svlFilterSourceVideoCapture_h
#define _svlFilterSourceVideoCapture_h

#include <cisstStereoVision/svlFilterSourceBase.h>
#include <cisstOSAbstraction/osaThreadSignal.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


// Forward declarations
class svlVidCapSrcBase;
class svlVidCapSrcDialogThread;


class CISST_EXPORT svlFilterSourceVideoCapture : public svlFilterSourceBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

friend class svlVidCapSrcDialogThread;

public:
    enum PlatformType {
        WinDirectShow   = 0,
        WinSVS          = 1,
        LinVideo4Linux2 = 2,
        LinLibDC1394    = 3,
        OpenCV          = 4,
        MatroxImaging   = 5,
        BlackMagicDeckLink = 6,
        NumberOfPlatformTypes
    };

    typedef struct _DeviceInfo {
	    int ID;
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

    class CISST_EXPORT Config
    {
    public:
        Config();
        Config(const Config& objref);

        int                               Channels;
        vctDynamicVector<int>             Device;
        vctDynamicVector<int>             Input;
        vctDynamicVector<ImageFormat>     Format;
        vctDynamicVector<ImageProperties> Properties;
        vctDynamicVector<ExternalTrigger> Trigger;

        void SetChannels(const int channels);
    };

    typedef svlFilterSourceVideoCapture ThisType;
    typedef vctDynamicVector<DeviceInfo> DeviceInfoListType;
    typedef vctDynamicVector<ImageFormat> FormatListType;

public:
    svlFilterSourceVideoCapture();
    svlFilterSourceVideoCapture(unsigned int channelcount);
    virtual ~svlFilterSourceVideoCapture();

    int EnumerateDevices();
    int SetChannelCount(unsigned int channelcount);

    void SetTargetFrequency(double hertz);
    double GetTargetFrequency() const;

    int DialogSetup(unsigned int videoch = SVL_LEFT);
    int DialogDevice();
    int DialogInput(unsigned int deviceid);
    int DialogFormat(unsigned int videoch = SVL_LEFT);
    int DialogTrigger(unsigned int videoch = SVL_LEFT);
    int DialogImageProperties(unsigned int videoch = SVL_LEFT);

    int GetDeviceList(DeviceInfo **deviceinfolist) const;
    void ReleaseDeviceList(DeviceInfo *deviceinfolist) const;
    int PrintDeviceList(bool update = false);
    int PrintInputList(int deviceid, bool update = false);
    int SetDevice(int deviceid, int inputid = 0, unsigned int videoch = SVL_LEFT);
    int GetDevice(int & deviceid, int & inputid, unsigned int videoch = SVL_LEFT) const;

    int GetFormatList(ImageFormat **formatlist, unsigned int videoch = SVL_LEFT) const;
    void ReleaseFormatList(ImageFormat *formatlist) const;
    int PrintFormatList(unsigned int videoch = SVL_LEFT);
    int SelectFormat(unsigned int formatid, unsigned int videoch = SVL_LEFT);
    int SetFormat(const ImageFormat& format, unsigned int videoch = SVL_LEFT);
    int GetFormat(ImageFormat& format, unsigned int videoch = SVL_LEFT) const;
    int SetTrigger(const ExternalTrigger& trigger, unsigned int videoch = SVL_LEFT);
    int GetTrigger(ExternalTrigger& trigger, unsigned int videoch = SVL_LEFT) const;
    int SetImageProperties(const ImageProperties& properties, unsigned int videoch = SVL_LEFT);
    int GetImageProperties(ImageProperties& properties, unsigned int videoch = SVL_LEFT) const;
    static std::string GetPixelTypeName(PixelType pixeltype);
    static std::string GetPatternTypeName(PatternType patterntype);

    unsigned int GetWidth(unsigned int videoch = SVL_LEFT) const;
    unsigned int GetHeight(unsigned int videoch = SVL_LEFT) const;

    int SaveSettings(const char* filepath);
    int LoadSettings(const char* filepath);

protected:
    virtual int Initialize(svlSample* &syncOutput);
    virtual int Process(svlProcInfo* procInfo, svlSample* &syncOutput);
    virtual int Release();

private:
    svlSampleImage* OutputImage;
    unsigned int NumberOfChannels;
    unsigned int NumberOfSupportedAPIs;
    vctDynamicVector<cmnClassServicesBase*> SupportedAPIs;
    vctDynamicVector<PlatformType> APIPlatforms;
    DeviceInfo* EnumeratedDevices;
    int NumberOfEnumeratedDevices;
    ImageFormat **FormatList;
    int *FormatListSize;
    vctDynamicVector<cmnGenericObject*> DeviceGenObj;
    vctDynamicVector<svlVidCapSrcBase*> DeviceObj;
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

protected:
    virtual void CreateInterfaces();
    virtual void EnumerateDevicesCommand(void);
    virtual void GetDeviceListCommand(ThisType::DeviceInfoListType & devicelist) const;
    virtual void GetCommand(ThisType::Config & objref) const;
    virtual void SetCommand(const ThisType::Config & objref);
    virtual void SetChannelsCommand(const int & channels);
    virtual void SetDeviceLCommand(const int & deviceid);
    virtual void SetDeviceRCommand(const int & deviceid);
    virtual void SetInputLCommand(const int & inputid);
    virtual void SetInputRCommand(const int & inputid);
    virtual void SetFormatLCommand(const ThisType::ImageFormat & format);
    virtual void SetFormatRCommand(const ThisType::ImageFormat & format);
    virtual void SelectFormatLCommand(const int & formatid);
    virtual void SelectFormatRCommand(const int & formatid);
    virtual void SetTriggerLCommand(const ThisType::ExternalTrigger & trigger);
    virtual void SetTriggerRCommand(const ThisType::ExternalTrigger & trigger);
    virtual void SetImagePropertiesLCommand(const ThisType::ImageProperties & properties);
    virtual void SetImagePropertiesRCommand(const ThisType::ImageProperties & properties);
    virtual void SaveSettingsCommand(const std::string & filepath);
    virtual void LoadSettingsCommand(const std::string & filepath);
    virtual void GetChannelsCommand(int & channels) const;
    virtual void GetDeviceLCommand(int & deviceid) const;
    virtual void GetDeviceRCommand(int & deviceid) const;
    virtual void GetInputLCommand(int & inputid) const;
    virtual void GetInputRCommand(int & inputid) const;
    virtual void GetFormatLCommand(ThisType::ImageFormat & format) const;
    virtual void GetFormatRCommand(ThisType::ImageFormat & format) const;
    virtual void GetDimensionsLCommand(vctInt2 & dimensions) const;
    virtual void GetDimensionsRCommand(vctInt2 & dimensions) const;
    virtual void GetFormatListLCommand(ThisType::FormatListType & formatlist) const;
    virtual void GetFormatListRCommand(ThisType::FormatListType & formatlist) const;
    virtual void GetTriggerLCommand(ThisType::ExternalTrigger & trigger) const;
    virtual void GetTriggerRCommand(ThisType::ExternalTrigger & trigger) const;
    virtual void GetImagePropertiesLCommand(ThisType::ImageProperties & properties) const;
    virtual void GetImagePropertiesRCommand(ThisType::ImageProperties & properties) const;
};


class CISST_EXPORT svlVidCapSrcBase : public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    svlVidCapSrcBase() : cmnGenericObject() {}
    virtual ~svlVidCapSrcBase() {}

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


class CISST_EXPORT svlVidCapSrcDialogThread
{
public:
    svlVidCapSrcDialogThread(unsigned int videoch) { VideoChannel = videoch; InitSuccess = false; Stopped = false; }
    ~svlVidCapSrcDialogThread() {}
    void* Proc(svlFilterSourceVideoCapture* baseref);

    bool WaitForInit() { InitEvent.Wait(); return InitSuccess; }
    void Kill() { Stopped = true; }

private:
    int VideoChannel;
    osaThreadSignal InitEvent;
    bool InitSuccess;
    bool Stopped;
};

typedef mtsGenericObjectProxy<svlFilterSourceVideoCapture::Config> svlFilterSourceVideoCapture_Config;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceVideoCapture_Config);
typedef mtsGenericObjectProxy<svlFilterSourceVideoCapture::DeviceInfoListType> svlFilterSourceVideoCapture_DeviceList;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceVideoCapture_DeviceList);
typedef mtsGenericObjectProxy<svlFilterSourceVideoCapture::FormatListType> svlFilterSourceVideoCapture_FormatList;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceVideoCapture_FormatList);
typedef mtsGenericObjectProxy<svlFilterSourceVideoCapture::ImageFormat> svlFilterSourceVideoCapture_Format;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceVideoCapture_Format);
typedef mtsGenericObjectProxy<svlFilterSourceVideoCapture::ImageProperties> svlFilterSourceVideoCapture_ImageProperties;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceVideoCapture_ImageProperties);
typedef mtsGenericObjectProxy<svlFilterSourceVideoCapture::ExternalTrigger> svlFilterSourceVideoCapture_Trigger;
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceVideoCapture_Trigger);

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlFilterSourceVideoCapture)
CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlVidCapSrcBase)


CISST_EXPORT std::ostream & operator << (std::ostream & stream, const svlFilterSourceVideoCapture::Config & objref);
CISST_EXPORT std::ostream & operator << (std::ostream & stream, const svlFilterSourceVideoCapture::DeviceInfo & objref);
CISST_EXPORT std::ostream & operator << (std::ostream & stream, const svlFilterSourceVideoCapture::ImageFormat & objref);
CISST_EXPORT std::ostream & operator << (std::ostream & stream, const svlFilterSourceVideoCapture::ImageProperties & objref);
CISST_EXPORT std::ostream & operator << (std::ostream & stream, const svlFilterSourceVideoCapture::ExternalTrigger & objref);
CISST_EXPORT std::ostream & operator << (std::ostream & stream, const svlFilterSourceVideoCapture::DeviceInfoListType & objref);
CISST_EXPORT std::ostream & operator << (std::ostream & stream, const svlFilterSourceVideoCapture::FormatListType & objref);


#endif // _svlFilterSourceVideoCapture_h
