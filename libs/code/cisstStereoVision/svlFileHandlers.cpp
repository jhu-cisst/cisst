/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlFileHandlers.cpp,v 1.5 2008/10/22 20:04:13 vagvoba Exp $
  
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

#include <cisstStereoVision/svlFileHandlers.h>

#include "ftImageBMP.h"
#include "ftImagePPM.h"

using namespace std;

/*************************************/
/*** svlFileHandlers class ***********/
/*************************************/

svlImageFile::svlImageFile()
{
}

svlImageFile::~svlImageFile()
{
}

svlImageFile* svlImageFile::GetInstance()
{
    return 0;
}

int svlImageFile::ExtractDimensions(const char* filepath, int & width, int & height)
{
    return -1;
}

int svlImageFile::Open(const char* filepath, svlImageProperties& properties)
{
    return -1;
}

int svlImageFile::ReadAndClose(unsigned char* buffer, unsigned int size)
{
    return -1;
}

int svlImageFile::Create(const char* filepath, svlImageProperties* properties, unsigned char* buffer)
{
    return -1;
}

/*************************************/
/*** svlImageFileTypeList class ******/
/*************************************/

svlImageFileTypeList::svlImageFileTypeList()
{
    Extensions[0][0] = 0;
    Prototypes[0] = 0;

    svlImageFile* instance;

    instance = new ftImageBMP();
    AddType("bmp", instance);
    instance = new ftImagePPM();
    AddType("ppm", instance);
    instance = new ftImagePPM();
    AddType("pgm", instance);
}

svlImageFileTypeList::~svlImageFileTypeList()
{
    int idx = 0;

    while (idx < SVL_FH_MAX_EXTENSIONS &&
           Prototypes[idx] != 0) {
        delete Prototypes[idx];
        idx ++;
    }
}

void svlImageFileTypeList::AddType(const char* extension, svlImageFile* prototype)
{
    if (extension == 0 || prototype == 0) return;

    int idx = 0;

    while (idx < SVL_FH_MAX_EXTENSIONS &&
           Extensions[idx][0] != 0) {
        idx ++;
    }

    if (idx < SVL_FH_MAX_EXTENSIONS) {
        strcpy(Extensions[idx], extension);
        Prototypes[idx] = prototype;
        idx ++;
        if (idx < SVL_FH_MAX_EXTENSIONS) {
            Extensions[idx][0] = 0;
            Prototypes[idx] = 0;
        }
    }
}

svlImageFile* svlImageFileTypeList::GetHandlerInstance(const char* extension)
{
    if (extension == 0) return 0;

    int idx = 0;

    while (idx < SVL_FH_MAX_EXTENSIONS &&
           Extensions[idx][0] != 0 &&
           strcmp(Extensions[idx], extension) != 0) {
        idx ++;
    }

    if (idx >= SVL_FH_MAX_EXTENSIONS ||
        Extensions[idx][0] == 0) {
        return 0;
    }

    svlImageFile* instance = Prototypes[idx]->GetInstance();
    return instance;
}

