/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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

#ifndef _ftImagePPM_h
#define _ftImagePPM_h

#include <cisstStereoVision/svlImageIO.h>


class ftImagePPM : public svlImageCodec, public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    ftImagePPM();
    virtual ~ftImagePPM();

    virtual int ReadDimensions(const std::string &filename, unsigned int &width, unsigned int &height);
    virtual int ReadDimensions(std::istream &stream, unsigned int &width, unsigned int &height);
    virtual int ReadDimensions(const unsigned char *buffer, const size_t buffersize, unsigned int &width, unsigned int &height);

    virtual int Read(svlSampleImageBase &image, const unsigned int videoch, const std::string &filename, bool noresize = false);
    virtual int Read(svlSampleImageBase &image, const unsigned int videoch, std::istream &stream, bool noresize = false);
    virtual int Read(svlSampleImageBase &image, const unsigned int videoch, const unsigned char *buffer, const size_t buffersize, bool noresize = false);

    virtual int Write(const svlSampleImageBase &image, const unsigned int videoch, const std::string &filename);
    virtual int Write(const svlSampleImageBase &image, const unsigned int videoch, std::ostream &stream);
    virtual int Write(const svlSampleImageBase &image, const unsigned int videoch, std::ostream &stream, const std::string &codec);
    virtual int Write(const svlSampleImageBase &image, const unsigned int videoch, unsigned char *buffer, size_t &buffersize);
    virtual int Write(const svlSampleImageBase &image, const unsigned int videoch, unsigned char *buffer, size_t &buffersize, const std::string &codec);

protected:
    int ppmOpen(std::istream &stream, unsigned int &width, unsigned int &height);
    int ppmOpen(const unsigned char *source, const size_t sourcesize, unsigned int &width, unsigned int &height);
    int ppmRead(std::istream &stream, unsigned char* buffer, unsigned int pixelcount, int magicnumber);
    int ppmRead(const unsigned char *source, const size_t sourcesize, unsigned char* buffer, unsigned int pixelcount, int magicnumber);
    int ppmWrite(const unsigned char* buffer, const unsigned int width, const unsigned int height, const int magicnumber, std::ostream &stream);
    int ppmWrite(const unsigned char* buffer, const unsigned int width, const unsigned int height, const int magicnumber, unsigned char *target, size_t &targetsize);

    unsigned char* ppmBuffer;
    unsigned int ppmBufferSize;
};

CMN_DECLARE_SERVICES_INSTANTIATION(ftImagePPM)

#endif // _ftImagePPM_h

