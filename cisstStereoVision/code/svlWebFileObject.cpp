/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 
 Author(s): Balazs Vagvolgyi
 Created on: 2009-11-04
 
 (C) Copyright 2005-2009 Johns Hopkins University (JHU), All Rights
 Reserved.
 
 --- begin cisst license - do not edit ---
 
 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.
 
 --- end cisst license ---
 
 */


#include "cisstStereoVision/svlWebFileObject.h"

#include <iostream>


svlWebFileObject::svlWebFileObject(const std::string & name) :
    svlWebObjectBase(name),
    Buffer(0),
    BufferSize(0)
{
    Readable = true;
}

svlWebFileObject::~svlWebFileObject()
{
    if (Buffer) delete [] Buffer;
}

svlWebFileObject* svlWebFileObject::clone() const
{
    return new svlWebFileObject(*this);
}

bool svlWebFileObject::PrepareContents()
{
    std::string filename = "./" + GetName();
    if (filename.find("..") != std::string::npos ||
        filename.find("//") != std::string::npos) return false;

    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp) return false;

    // Get file size
    fseek(fp, 0, SEEK_END);
    FileSize = ftell(fp);
    if (FileSize > BufferSize) {
        BufferSize = FileSize;
        delete [] Buffer;
        Buffer = new char[BufferSize];
    }
    fseek(fp, 0, SEEK_SET);
    fread(Buffer, FileSize, 1,fp);
    fclose(fp);

    // Get content type for file
    int dotpos = filename.find_last_of('.');
    std::string ext = filename.substr(dotpos + 1);
         if (ext.compare("html") == 0) SetContentType("text/html");
    else if (ext.compare("css")  == 0) SetContentType("text/css");
    else if (ext.compare("js")   == 0) SetContentType("text/javascript");
    else if (ext.compare("xml")  == 0) SetContentType("text/xml");
    else if (ext.compare("jpg")  == 0) SetContentType("image/jpeg");
    else if (ext.compare("jpeg") == 0) SetContentType("image/jpeg");
    else if (ext.compare("png")  == 0) SetContentType("image/png");
    else return false;

    return true;
}

void svlWebFileObject::Read(char*& data, int& datasize)
{
    data = Buffer;
    datasize = FileSize;
}

