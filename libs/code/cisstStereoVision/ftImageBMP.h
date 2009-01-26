/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ftImageBMP.h,v 1.3 2008/02/12 19:57:39 vagvoba Exp $
  
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

#ifndef _ftImageBMP_h
#define _ftImageBMP_h

#include <cisstStereoVision/svlFileHandlers.h>

#define IMAGEBMP_MAX_DIMENISION     8192

class ftImageBMP : public svlImageFile
{
public:
    ftImageBMP();
    ~ftImageBMP();
    svlImageFile* GetInstance();

    int ExtractDimensions(const char* filepath, int & width, int & height);
    int Open(const char* filepath, svlImageProperties& properties);
    int ReadAndClose(unsigned char* buffer, unsigned int size);
    int Create(const char* filepath, svlImageProperties* properties, unsigned char* buffer);

protected:
    FILE* FileHandle;
    bool BufferPadded;
    unsigned int BufferSize;
    unsigned int LineSize;
    unsigned int Padding;
    unsigned int Height;
    bool UpsideDown;
    svlBMPFileHeader FileHeader;
    svlDIBHeader DIBHeader;

    void Close();
};

#endif // _ftImageBMP_h

