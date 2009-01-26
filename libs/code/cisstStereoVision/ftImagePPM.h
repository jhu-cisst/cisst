/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ftImagePPM.h,v 1.2 2008/02/12 19:57:39 vagvoba Exp $
  
  Author(s):  Ben Mitchell
  Created on: 2006 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _ftImagePPM_h
#define _ftImagePPM_h

// ftImagePPM is a class designed to read PPM or PGM files
// for use in the SVL architecture.
//
// created by Ben Mitchell
// adapted from ImageBMP (created by Balazs Vagvolgyi)

#include <string>
#include <fstream>
//#include "svlImageFileSource.h"
#include <cisstStereoVision/svlFileHandlers.h>

#define IMAGEPPM_MAX_DIMENISION     8192


class ftImagePPM : public svlImageFile
{
public:
    ftImagePPM();
    ~ftImagePPM();
    svlImageFile* GetInstance();

    int ExtractDimensions(const char* filepath, int & width, int & height);
    int Open(const char* filepath, svlImageProperties& properties);
    int ReadAndClose(unsigned char* buffer, unsigned int size);
    int Create(const char* filepath, svlImageProperties* properties, unsigned char* buffer);

protected:
    std::fstream myfile;
    std::string magic;
    unsigned int myrows;
    unsigned int mycols;
    unsigned int mycolors;
    unsigned long mysize;

    bool pgm;
    unsigned char *tmpBuffer;
    unsigned char tmpSwapBuffer[12288];

    void Close();
};

#endif // _ftImagePPM_h

