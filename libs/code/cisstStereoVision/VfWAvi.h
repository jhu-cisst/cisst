/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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

#ifndef _CVfWAvi_h
#define _CVfWAvi_h

#include "windows.h"
#include "vfw.h"


class CVfWAvi
{
    typedef struct _AVI_RGBSTRUCT {
        unsigned char B;
        unsigned char G;
        unsigned char R;
    } AVI_RGBSTRUCT;

public:
    CVfWAvi();
    virtual ~CVfWAvi();

protected:
    char PathName[1024];
    PAVIFILE PAviFile;
    AVIFILEINFO FileInfo;
    PAVISTREAM PAviStream;
    AVISTREAMINFO StreamInfo;
    PAVISTREAM PAviCompressedStream;
    AVICOMPRESSOPTIONS CompressionOptions;
    PGETFRAME PGetFrame;
    int UpsideDown;
    int FirstFrame;
    int LastFrame;
    int CurrentFrame;
    int Width;
    int Height;
    double Framerate;
    unsigned int KeyFrames;
    int FileIsOpened;

	unsigned char* WriteBuffer;
	int WriteBufferLen;

    AVI_RGBSTRUCT ConvLUT_16_24[65536];

protected:
    void InitConversionTables();
    int ConvertToTrueColor(unsigned char* source, unsigned char* dest, const DWORD size);
    void TopDownImage(unsigned char* image, int width, int height);
    void ResetStates();

public:
    int InitPlaying(LPCTSTR path);
	int InitCreation(LPCTSTR path, int width, int height, double fps);

    BOOL IsFileOpened() { return FileIsOpened; }
    LPCTSTR GetPathName() { return PathName; }
    int GetWidth() { return Width; }
    int GetHeight() { return Height; }
    double GetFramerate() { return Framerate; }
    int GetFirstFrameNumber() { return FirstFrame; }
    int GetLastFrameNumber() { return LastFrame; }
    int GetCurrentFrameNumber() { return CurrentFrame; }
    void GetCompressOptions(AVICOMPRESSOPTIONS &coptions) { coptions = CompressionOptions; }
    void SetCompressOptions(AVICOMPRESSOPTIONS &coptions) { CompressionOptions = coptions; }
    void SetKeyFrameInterval(unsigned int interval);

    int ShowCompressionDialog();

    int CopyAVIFrame(unsigned char* dest, int datasize, int framenum);
    int CopyNextAVIFrame(unsigned char* dest, int datasize);
	int AddFrameToAVI(unsigned char* buffer, int buffersize);

    void Close();
};

#endif // _CVfWAvi_h

