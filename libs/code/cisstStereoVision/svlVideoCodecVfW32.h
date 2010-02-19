/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
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

#ifndef _svlVideoCodecVfW32_h
#define _svlVideoCodecVfW32_h

#include <cisstStereoVision/svlVideoIO.h>
#include <cisstStereoVision/svlStreamDefs.h>

#include "windows.h"
#include "vfw.h"


class svlVideoCodecVfW32 : public svlVideoCodecBase, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlVideoCodecVfW32();
    virtual ~svlVideoCodecVfW32();

    virtual int Open(const std::string &filename, unsigned int &width, unsigned int &height, double &framerate);
    virtual int Create(const std::string &filename, const unsigned int width, const unsigned int height, const double framerate);
    virtual int Close();

    virtual int GetBegPos() const;
    virtual int GetEndPos() const;
    virtual int GetPos() const;
    virtual int SetPos(const int pos);

    virtual svlVideoIO::Compression* GetCompression() const;
    virtual int SetCompression(const svlVideoIO::Compression *compression);
    virtual int DialogCompression();

    virtual int Read(svlProcInfo* procInfo, svlSampleImageBase &image, const unsigned int videoch, const bool noresize = false);
    virtual int Write(svlProcInfo* procInfo, const svlSampleImageBase &image, const unsigned int videoch);

protected:
    void Flip(unsigned char* image);
    void FlipCopy(const unsigned char* source, unsigned char* dest);

    unsigned int Width;
    unsigned int Height;
    int BegPos;
    int EndPos;
    int Pos;
    int KeyFrameEvery;
    bool Writing;
    bool Opened;

    PAVIFILE        pAviFile;
    PAVISTREAM      pAviStream;
    PAVISTREAM      pAviStreamCompressed;
    PGETFRAME       pGetFrame;
    AVIFILEINFO     AviFileInfo;
    AVISTREAMINFO   AviStreamInfo;

    bool UpsideDown;
	unsigned char* comprBuffer;
	unsigned int comprBufferSize;

    void LogError(HRESULT ret);
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlVideoCodecVfW32)

#endif // _svlVideoCodecVfW32_h

