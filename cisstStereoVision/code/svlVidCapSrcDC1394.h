/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlVidCapSrcDC1394_h
#define _svlVidCapSrcDC1394_h

#include <cisstStereoVision/svlFilterSourceVideoCapture.h>


class svlBufferImage;
class osaThread;
class svlVidCapSrcDC1394Thread;


/*********************************************************************
** LibDC1394                                                        **
**  - compatibility: DC1394 2.0.2+                                  **
**  - get it from:                                                  **
**    http://sourceforge.net/project/showfiles.php?group_id=8157    **
**                                 or                               **
**                       MacPorts (OS X 10.5+)                      **
*********************************************************************/
#include "dc1394/dc1394.h"

/*******************************************
** Video modes                            **
********************************************
typedef enum {
    DC1394_VIDEO_MODE_160x120_YUV444    = 64,
    DC1394_VIDEO_MODE_320x240_YUV422    = 65,
    DC1394_VIDEO_MODE_640x480_YUV411    = 66,
    DC1394_VIDEO_MODE_640x480_YUV422    = 67,
    DC1394_VIDEO_MODE_640x480_RGB8      = 68,
    DC1394_VIDEO_MODE_640x480_MONO8     = 69,
    DC1394_VIDEO_MODE_640x480_MONO16    = 70,
    DC1394_VIDEO_MODE_800x600_YUV422    = 71,
    DC1394_VIDEO_MODE_800x600_RGB8      = 72,
    DC1394_VIDEO_MODE_800x600_MONO8     = 73,
    DC1394_VIDEO_MODE_1024x768_YUV422   = 74,
    DC1394_VIDEO_MODE_1024x768_RGB8     = 75,
    DC1394_VIDEO_MODE_1024x768_MONO8    = 76,
    DC1394_VIDEO_MODE_800x600_MONO16    = 77,
    DC1394_VIDEO_MODE_1024x768_MONO16   = 78,
    DC1394_VIDEO_MODE_1280x960_YUV422   = 79,
    DC1394_VIDEO_MODE_1280x960_RGB8     = 80,
    DC1394_VIDEO_MODE_1280x960_MONO8    = 81,
    DC1394_VIDEO_MODE_1600x1200_YUV422  = 82,
    DC1394_VIDEO_MODE_1600x1200_RGB8    = 83,
    DC1394_VIDEO_MODE_1600x1200_MONO8   = 84,
    DC1394_VIDEO_MODE_1280x960_MONO16   = 85,
    DC1394_VIDEO_MODE_1600x1200_MONO16  = 86,
    DC1394_VIDEO_MODE_EXIF              = 87,
    DC1394_VIDEO_MODE_FORMAT7_0         = 88,
    DC1394_VIDEO_MODE_FORMAT7_1         = 89,
    DC1394_VIDEO_MODE_FORMAT7_2         = 90,
    DC1394_VIDEO_MODE_FORMAT7_3         = 91,
    DC1394_VIDEO_MODE_FORMAT7_4         = 92,
    DC1394_VIDEO_MODE_FORMAT7_5         = 93,
    DC1394_VIDEO_MODE_FORMAT7_6         = 94,
    DC1394_VIDEO_MODE_FORMAT7_7         = 95
} dc1394video_mode_t;
*******************************************/

/*******************************************
** Color codings                          **
********************************************
typedef enum {
    DC1394_COLOR_CODING_MONO8   = 320,
    DC1394_COLOR_CODING_YUV411  = 321,
    DC1394_COLOR_CODING_YUV422  = 322,
    DC1394_COLOR_CODING_YUV444  = 323,
    DC1394_COLOR_CODING_RGB8    = 324,
    DC1394_COLOR_CODING_MONO16  = 325,
    DC1394_COLOR_CODING_RGB16   = 326,
    DC1394_COLOR_CODING_MONO16S = 327,
    DC1394_COLOR_CODING_RGB16S  = 328,
    DC1394_COLOR_CODING_RAW8    = 329,
    DC1394_COLOR_CODING_RAW16   = 330
} dc1394color_coding_t;
*******************************************/

/*******************************************
** Bayer patterns                         **
********************************************
typedef enum {
    DC1394_COLOR_FILTER_RGGB = 512,
    DC1394_COLOR_FILTER_GBRG = 513,
    DC1394_COLOR_FILTER_GRBG = 514,
    DC1394_COLOR_FILTER_BGGR = 515
} dc1394color_filter_t;
*******************************************/

/*******************************************
** Framerates                             **
********************************************
typedef enum {
    DC1394_FRAMERATE_1_875 = 32,
    DC1394_FRAMERATE_3_75  = 33,
    DC1394_FRAMERATE_7_5   = 34,
    DC1394_FRAMERATE_15    = 35,
    DC1394_FRAMERATE_30    = 36,
    DC1394_FRAMERATE_60    = 37,
    DC1394_FRAMERATE_120   = 38,
    DC1394_FRAMERATE_240   = 39
} dc1394framerate_t;
*******************************************/

/*******************************************
** Trigger modes                          **
********************************************
typedef enum {
  DC1394_TRIGGER_MODE_0  = 352,
  DC1394_TRIGGER_MODE_1  = 353,
  DC1394_TRIGGER_MODE_2  = 354,
  DC1394_TRIGGER_MODE_3  = 355,
  DC1394_TRIGGER_MODE_4  = 356,
  DC1394_TRIGGER_MODE_5  = 357,
  DC1394_TRIGGER_MODE_14 = 358,
  DC1394_TRIGGER_MODE_15 = 359
} dc1394trigger_mode_t;
#define DC1394_TRIGGER_MODE_MIN \
        DC1394_TRIGGER_MODE_0
#define DC1394_TRIGGER_MODE_MAX \
        DC1394_TRIGGER_MODE_15
#define DC1394_TRIGGER_MODE_NUM \
        (DC1394_TRIGGER_MODE_MAX - \
         DC1394_TRIGGER_MODE_MIN + 1)
*******************************************/

/*******************************************
** Trigger sources                        **
********************************************
typedef enum {
  DC1394_TRIGGER_SOURCE_0 = 576,
  DC1394_TRIGGER_SOURCE_1 = 577,
  DC1394_TRIGGER_SOURCE_2 = 578,
  DC1394_TRIGGER_SOURCE_3 = 579
} dc1394trigger_source_t;
#define DC1394_TRIGGER_SOURCE_MIN \
        DC1394_TRIGGER_SOURCE_0
#define DC1394_TRIGGER_SOURCE_MAX \
        DC1394_TRIGGER_SOURCE_3
#define DC1394_TRIGGER_SOURCE_NUM \
        (DC1394_TRIGGER_SOURCE_MAX - \
         DC1394_TRIGGER_SOURCE_MIN + 1)
*******************************************/

/*******************************************
** Trigger polarity                       **
********************************************
typedef enum {
  DC1394_TRIGGER_ACTIVE_LOW  = 0,
  DC1394_TRIGGER_ACTIVE_HIGH = 1
} dc1394trigger_polarity_t;
*******************************************/

/*******************************************
** Log levels                             **
********************************************
typedef enum {
    DC1394_LOG_ERROR   = 768,
    DC1394_LOG_WARNING = 769,
    DC1394_LOG_DEBUG   = 770
} dc1394log_t;
*******************************************/

class svlVidCapSrcDC1394Context
{
public:
    static svlVidCapSrcDC1394Context* Instance();

    dc1394_t* GetContext();
	int GetDeviceList(svlFilterSourceVideoCapture::DeviceInfo **deviceinfo);
	dc1394camera_t** GetCameras();
	unsigned int GetNumberOfCameras();
    dc1394operation_mode_t* GetBestOpMode();
    dc1394speed_t* GetBestISOSpeed();
	unsigned int GetPixelTypeBitSize(svlFilterSourceVideoCapture::PixelType type);

private:
    svlVidCapSrcDC1394Context();
    ~svlVidCapSrcDC1394Context();

    void Enumerate();
    int TestIEEE1394Interface(dc1394camera_t* camera, dc1394operation_mode_t opmode, dc1394speed_t isospeed);
    void ReleaseEnumeration();

    dc1394_t* Context;
    dc1394camera_list_t* CameraList;
    dc1394camera_t** Cameras;
    svlFilterSourceVideoCapture::DeviceInfo* DeviceInfos;
    unsigned int NumberOfCameras;
    dc1394operation_mode_t* BestOpMode;
    dc1394speed_t* BestISOSpeed;
    unsigned int PixelTypeBitSize[svlFilterSourceVideoCaptureTypes::PixelTypeCount];
};


class svlVidCapSrcDC1394 : public svlVidCapSrcBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

friend class svlVidCapSrcDC1394Thread;

public:
    svlVidCapSrcDC1394();
	~svlVidCapSrcDC1394();

public:
    svlFilterSourceVideoCapture::PlatformType GetPlatformType();
    int SetStreamCount(unsigned int numofstreams);
	int GetDeviceList(svlFilterSourceVideoCapture::DeviceInfo **deviceinfo);
	int Open();
	void Close();
	int Start();
    svlImageRGB* GetLatestFrame(bool waitfornew, unsigned int videoch = 0);
	int Stop();
	bool IsRunning();
    int SetDevice(int devid, int inid, unsigned int videoch = 0);
	int GetWidth(unsigned int videoch = 0);
	int GetHeight(unsigned int videoch = 0);

    int GetFormatList(unsigned int deviceid, svlFilterSourceVideoCapture::ImageFormat **formatlist);
    int SetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch = 0);
    int GetFormat(svlFilterSourceVideoCapture::ImageFormat& format, unsigned int videoch = 0);
    int SetImageProperties(svlFilterSourceVideoCapture::ImageProperties& properties, unsigned int videoch = 0);
    int GetImageProperties(svlFilterSourceVideoCapture::ImageProperties& properties, unsigned int videoch = 0);
    int SetTrigger(svlFilterSourceVideoCapture::ExternalTrigger & trigger, unsigned int videoch = 0);
    int GetTrigger(svlFilterSourceVideoCapture::ExternalTrigger & trigger, unsigned int videoch = 0);

private:
    unsigned int NumOfStreams;
    bool Initialized;
    bool Running;

    svlVidCapSrcDC1394Thread** CaptureProc;
    osaThread** CaptureThread;

    int* CameraFileNo;
    fd_set* CameraFDSet;

    osaMutex Mutex;
    dc1394_t* Context;
    dc1394camera_list_t* CameraList;
    dc1394camera_t** Cameras;
    unsigned int NumberOfCameras;
    dc1394operation_mode_t* BestOpMode;
    dc1394speed_t* BestISOSpeed;
    int* DeviceID;
    svlFilterSourceVideoCapture::ImageFormat** Format;
    svlFilterSourceVideoCapture::ExternalTrigger* Trigger;
    int* Width;
    int* Height;
    unsigned int* ColorCoding;
    dc1394video_frame_t** Frame;
    svlBufferImage** OutputBuffer;

    int CaptureFrame(unsigned int videoch);

    void Release();
    int GetModeFromFormat(unsigned int width, unsigned int height, svlFilterSourceVideoCapture::PixelType colspc, unsigned int& mode);
    int GetSupportedFrameratesForFormat(unsigned int devid, svlFilterSourceVideoCapture::ImageFormat& format, double **fpslist, unsigned int& listsize);
    int GetFramerateFromFPS(double fps, unsigned int& framerate);
    int GetFormatFromMode(unsigned int mode, svlFilterSourceVideoCapture::ImageFormat& format);
    dc1394color_coding_t GetColorCodingFromPixelType(svlFilterSourceVideoCapture::PixelType pixeltype);
    svlFilterSourceVideoCapture::PixelType GetPixelTypeFromColorCoding(dc1394color_coding_t colorcoding);
    dc1394color_filter_t GetColorFilterFromPatternType(svlFilterSourceVideoCapture::PatternType patterntype);
    svlFilterSourceVideoCapture::PatternType GetPatternTypeFromColorFilter(dc1394color_filter_t colorfilter);
    void SwapRGBBuffer(unsigned char* buffer, const unsigned int numberofpixels);
};


class svlVidCapSrcDC1394Thread
{
public:
    svlVidCapSrcDC1394Thread(int streamid) { StreamID = streamid; InitSuccess = false; }
    ~svlVidCapSrcDC1394Thread() {}
    void* Proc(svlVidCapSrcDC1394* baseref);

    bool WaitForInit() { InitEvent.Wait(); return InitSuccess; }
    bool IsError() { return Error; }

private:
    int StreamID;
    bool Error;
    osaThreadSignal InitEvent;
    bool InitSuccess;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlVidCapSrcDC1394)

#endif // _svlVidCapSrcDC1394_h

