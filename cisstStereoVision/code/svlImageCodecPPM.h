/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Ben Mitchell
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

Original code courtesy of Ben Mitchell.
*/

#ifndef _svlImageCodecPPM_h
#define _svlImageCodecPPM_h

#include <cisstStereoVision/svlImageIO.h>


class svlImageCodecPPM : public svlImageCodecBase
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    svlImageCodecPPM();
    virtual ~svlImageCodecPPM();

    virtual int ReadDimensions(const std::string &filename, unsigned int &width, unsigned int &height);
    virtual int ReadDimensions(std::istream &stream, unsigned int &width, unsigned int &height);
    virtual int ReadDimensions(const unsigned char *buffer, const size_t buffersize, unsigned int &width, unsigned int &height);

    virtual int Read(svlSampleImage &image, const unsigned int videoch, const std::string &filename, bool noresize = false);
    virtual int Read(svlSampleImage &image, const unsigned int videoch, std::istream &stream, bool noresize = false);
    virtual int Read(svlSampleImage &image, const unsigned int videoch, const unsigned char *buffer, const size_t buffersize, bool noresize = false);

    virtual int Write(const svlSampleImage &image, const unsigned int videoch, const std::string &filename, const int compression = -1);
    virtual int Write(const svlSampleImage &image, const unsigned int videoch, std::ostream &stream, const int compression = -1);
    virtual int Write(const svlSampleImage &image, const unsigned int videoch, std::ostream &stream, const std::string &codec, const int compression = -1);
    virtual int Write(const svlSampleImage &image, const unsigned int videoch, unsigned char *buffer, size_t &buffersize, const int compression = -1);
    virtual int Write(const svlSampleImage &image, const unsigned int videoch, unsigned char *buffer, size_t &buffersize, const std::string &codec, const int compression = -1);

protected:
    unsigned char* ppmBuffer;
    unsigned int ppmBufferSize;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlImageCodecPPM)

#endif // _svlImageCodecPPM_h

