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

#ifndef _svlFileHandlers_h
#define _svlFileHandlers_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

#define SVL_FH_MAX_EXTENSIONS       64
#define SVL_FH_MAX_EXTENSION_LEN    64

/*************************************/
/*** svlImageFile class **************/
/*************************************/

class CISST_EXPORT svlImageFile
{
public:
    svlImageFile();
    virtual ~svlImageFile();
    virtual svlImageFile* GetInstance();

    virtual int ExtractDimensions(const char * filepath, int & width, int & height);
    virtual int Open(const char * filepath, svlImageProperties & properties);
    virtual int ReadAndClose(unsigned char * buffer, unsigned int size);
    virtual int Create(const char * filepath, svlImageProperties * properties, unsigned char * buffer);
};

/*************************************/
/*** svlImageFileTypeList class ******/
/*************************************/

class CISST_EXPORT svlImageFileTypeList
{
public:
    svlImageFileTypeList();
    ~svlImageFileTypeList();

    void AddType(const char* extension, svlImageFile* prototype);
    svlImageFile* GetHandlerInstance(const char* extension);

private:
    char Extensions[SVL_FH_MAX_EXTENSIONS][SVL_FH_MAX_EXTENSION_LEN];
    svlImageFile* Prototypes[SVL_FH_MAX_EXTENSIONS];
};

#endif // _svlFileHandlers_h

