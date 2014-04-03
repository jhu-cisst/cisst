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

#ifndef _svlDefinitions_h
#define _svlDefinitions_h


#ifndef SETUP_QT_ENVIRONMENT
    // Do nothing if Qt is not available
    #define SETUP_QT_ENVIRONMENT(F) \
        int main(int argc, char** argv) \
        { \
            return F(argc, argv); \
        }
#endif // SETUP_QT_ENVIRONMENT


///////////////////////////////////
// Thread synchronization macros //
///////////////////////////////////

#define _OnSingleThread(_info) \
            if((_info)->ID==0)
#define _ParallelLoop(_info, _idx, _count) \
            _idx=(_info)->ID*std::max(1001*(_count)/(_info)->count,1000u)/1000; \
            for(const unsigned int _end=std::min((_count),((_info)->ID+1)*std::max(1001*(_count)/(_info)->count,1000u)/1000);_idx<_end;_idx++)
#define _ParallelInterleavedLoop(_info, _idx, _count) \
            _idx=(_info)->ID;for(const unsigned int _step=(_info)->count,_end=(_count);_idx<_end;_idx+=_step)
#define _GetParallelSubRange(_info, _count, _from, _to) \
            _to=(_count)/(_info)->count+1;_from=(_info)->ID*_to;_to+=_from;if(_to>(_count)){_to=(_count);}
#define _SynchronizeThreads(_info) \
            if((_info)->count>1){if((_info)->sync->Sync((_info)->ID)!=SVL_SYNC_OK){return SVL_FAIL;}}
#define _CriticalSection(_info) \
            if((_info)->count>1){(_info)->cs->Enter();}for(bool _incs=true;_incs;_incs=false,((_info)->count>1)?(_info)->cs->Leave():void())


///////////////////
// Helper macros //
///////////////////

#define _SkipIfAlreadyProcessed(input, output) \
            if(!IsNewSample(input)){syncOutput=output;return SVL_ALREADY_PROCESSED;}

#define _SkipIfDisabled() \
            if(IsDisabled())return SVL_OK;

#define STR2FOURCC(fcc_string) (reinterpret_cast<const unsigned int*>(fcc_string)[0])

#define MIN3(a, b, c)       (((a) <= (b)) ? \
                                (((a) <= (c)) ? (a) : (c)) : \
                                (((b) <= (c)) ? (b) : (c)))

#define MAX3(a, b, c)       (((a) >= (b)) ? \
                                (((a) >= (c)) ? (a) : (c)) : \
                                (((b) >= (c)) ? (b) : (c)))


////////////////
// GUI events //
////////////////

#define WM_USER_DESTROY             2000
#define WM_USER_PAINT               2002
#define DEFAULT_WINDOW_POS    0x7FFFFFFF

#define winInput_KEY_F1              112
#define winInput_KEY_F2              113
#define winInput_KEY_F3              114
#define winInput_KEY_F4              115
#define winInput_KEY_F5              116
#define winInput_KEY_F6              117
#define winInput_KEY_F7              118
#define winInput_KEY_F8              119
#define winInput_KEY_F9              120
#define winInput_KEY_F10             121
#define winInput_KEY_F11             122
#define winInput_KEY_F12             123

#define winInput_KEY_PAGEUP           33
#define winInput_KEY_PAGEDOWN         34
#define winInput_KEY_HOME             36
#define winInput_KEY_END              35
#define winInput_KEY_INSERT           45
#define winInput_KEY_DELETE           46

#define winInput_KEY_LEFT             37
#define winInput_KEY_RIGHT            39
#define winInput_KEY_UP               38
#define winInput_KEY_DOWN             40

#define winInput_MOUSEMOVE          1000
#define winInput_LBUTTONDOWN        1001
#define winInput_LBUTTONUP          1002
#define winInput_RBUTTONDOWN        1003
#define winInput_RBUTTONUP          1004


////////////////////////////
// Status and error codes //
////////////////////////////

#define SVL_ALL_CHANNELS                        0xFFFFFFFF
#define SVL_LEFT                                0
#define SVL_RIGHT                               1
#define SVL_OK                                  0
#define SVL_FAIL                               -1
#define SVL_YES                                 1
#define SVL_NO                                  0

#define SVL_EOF                                -2

#define SVL_SYNC_ERROR                         -1
#define SVL_SYNC_OK                             0
#define SVL_SYNC_TIMEOUT                        1

#define SVL_STREAM_RUNNING                      0
#define SVL_STREAM_CREATED                      1
#define SVL_STREAM_INITIALIZED                  2
#define SVL_STREAM_STOPPED                      3
#define SVL_STREAM_RELEASED                     4
#define SVL_ALREADY_PROCESSED                  10
#define SVL_STOP_REQUEST                       11
#define SVL_VID_END_REACHED                    12
#define SVL_VID_RETRY                          13

#define SVL_TYPE_MISMATCH                     -12
#define SVL_ALREADY_INITIALIZED               -13
#define SVL_NOT_INITIALIZED                   -14
#define SVL_ALREADY_RUNNING                   -15
#define SVL_NOT_SOURCE                        -17
#define SVL_NO_SOURCE_IN_LIST                 -18
#define SVL_INVALID_INPUT_TYPE                -19
#define SVL_INVALID_OUTPUT_TYPE               -20
#define SVL_NO_INPUT_DATA                     -21
#define SVL_WRONG_CHANNEL                     -22
#define SVL_CONSTRUCTION_FAILED               -23
#define SVL_ALLOCATION_ERROR                  -24
#define SVL_NOT_IMAGE                         -25
#define SVL_WAIT_TIMEOUT                      -26

#define SVL_VCS_DEVICE_NOT_INITIALIZED      -2000
#define SVL_VCS_UNABLE_TO_OPEN              -2001
#define SVL_VCS_UNABLE_TO_START_CAPTURE     -2002
#define SVL_VCS_UNABLE_TO_SET_INPUT         -2003
#define SVL_VCS_UNSUPPORTED_COLORSPACE      -2004
#define SVL_VCS_UNSUPPORTED_SIZE            -2005

#define SVL_STEREO_INPUT_MISMATCH           -5000
#define SVL_STEREO_INIT_ERROR               -5001

#define SVL_DMYSRC_DATA_NOT_INITIALIZED     -7000


/////////////////////////////////////////
// MAX and MIN values, timeouts, sizes //
/////////////////////////////////////////

#define SVL_MAX_CHANNELS                       2u
#define SVL_VCS_ARRAY_LENGTH                   50
#define SVL_VCS_STRING_LENGTH                 128
#define ST_DP_TEMP_BUFF_SIZE                 2048
#define MAX_DIMENSION                       65536
#define MAX_UI16_VAL                       0xFFFF
#define MAX_I32_VAL                    0x7FFFFFFF
#define BIG_I32_VAL                     100000000
#define DS_INIT_TIMEOUT_INTV                  500
#define INITIAL_TOLERANCE_WAIT_LENGTH         100 // [frames]
#define SVL_OCV_FONT_SCALE                   16.0


//////////////////////////////
// Stream type enumerations //
//////////////////////////////

enum svlStreamType
{
     svlTypeInvalid               // Default in base class
    ,svlTypeStreamSource          // Capture sources have an input connector of this type
    ,svlTypeStreamSink            // Render filters may have an output connector of this type
    ,svlTypeImageRGB              // Single RGB image
    ,svlTypeImageRGBA             // Single RGBA image
    ,svlTypeImageRGBStereo        // Dual RGB image
    ,svlTypeImageRGBAStereo       // Dual RGBA image
    ,svlTypeImageMono8            // Single Grayscale image (8bpp)
    ,svlTypeImageMono8Stereo      // Dual Grayscale image (8bpp)
    ,svlTypeImageMono16           // Single Grayscale image (16bpp)
    ,svlTypeImageMono16Stereo     // Dual Grayscale image (16bpp)
    ,svlTypeImageMono32           // Single Grayscale image (32bpp)
    ,svlTypeImageMono32Stereo     // Dual Grayscale image (32bpp)
    ,svlTypeImage3DMap            // Three floats per pixel for storing 3D coordinates
    ,svlTypeCUDAImageRGB          // Single RGB image (CUDA)
    ,svlTypeCUDAImageRGBA         // Single RGBA image (CUDA)
    ,svlTypeCUDAImageRGBStereo    // Dual RGB image (CUDA)
    ,svlTypeCUDAImageRGBAStereo   // Dual RGBA image (CUDA)
    ,svlTypeCUDAImageMono8        // Single Grayscale image (8bpp) (CUDA)
    ,svlTypeCUDAImageMono8Stereo  // Dual Grayscale image (8bpp) (CUDA)
    ,svlTypeCUDAImageMono16       // Single Grayscale image (16bpp) (CUDA)
    ,svlTypeCUDAImageMono16Stereo // Dual Grayscale image (16bpp) (CUDA)
    ,svlTypeCUDAImageMono32       // Single Grayscale image (32bpp) (CUDA)
    ,svlTypeCUDAImageMono32Stereo // Dual Grayscale image (32bpp) (CUDA)
    ,svlTypeCUDAImage3DMap        // Three floats per pixel for storing 3D coordinates (CUDA)
    ,svlTypeMatrixInt8            // Matrix of type 'char'
    ,svlTypeMatrixInt16           // Matrix of type 'short'
    ,svlTypeMatrixInt32           // Matrix of type 'int'
    ,svlTypeMatrixInt64           // Matrix of type 'long long int'
    ,svlTypeMatrixUInt8           // Matrix of type 'unsigned char'
    ,svlTypeMatrixUInt16          // Matrix of type 'unsigned short'
    ,svlTypeMatrixUInt32          // Matrix of type 'unsigned int'
    ,svlTypeMatrixUInt64          // Matrix of type 'unsigned long long int'
    ,svlTypeMatrixFloat           // Matrix of type 'float'
    ,svlTypeMatrixDouble          // Matrix of type 'double'
    ,svlTypeTransform3D           // 3D transformation
    ,svlTypeTargets               // Vector of N dimensional points
    ,svlTypeText                  // Textual data
    ,svlTypeCameraGeometry        // Geometry of a single or multiple camera rig
    ,svlTypeBlobs                 // Image blobs
};


////////////////////////////////
// Stereo layout enumerations //
////////////////////////////////

enum svlStereoLayout
{
     svlLayoutInterlaced
    ,svlLayoutInterlacedRL
    ,svlLayoutSideBySide
    ,svlLayoutSideBySideRL
    ,svlLayoutVertical
    ,svlLayoutInterlacedKeepSize
    ,svlLayoutInterlacedKeepSizeRL
};


/////////////////////////////////////////
// Kernel matching metric enumerations //
/////////////////////////////////////////

enum svlErrorMetric
{
     svlSAD
    ,svlSSD
    ,svlWSSD
    ,svlNCC
    ,svlFastNCC
    ,svlNotQuiteNCC
};


//////////////////////////////
// Color space enumerations //
//////////////////////////////

enum svlColorSpace
{
    svlColorSpaceRGB,
    svlColorSpaceHSV,
    svlColorSpaceHSL,
    svlColorSpaceYUV
};


/////////////////////////////
// Pixel type enumerations //
/////////////////////////////

enum svlPixelType
{
    svlPixelMono8,
    svlPixelMono16,
    svlPixelMono32,
    svlPixelRGB,
    svlPixelRGBA,
    svlPixelMonoFloat,
    svlPixel3DFloat,
    svlPixelUnknown
};

#endif // _svlDefinitions_h

