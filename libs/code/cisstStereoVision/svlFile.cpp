/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
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

#include <cisstStereoVision/svlFile.h>


svlFile::svlFile() :
    Stream(0),
    Opened(false),
    Length(0)
{
}

svlFile::svlFile(const std::string& filepath, const OpenMode mode) :
    Stream(0),
    Opened(false),
    Length(0)
{
    Open(filepath, mode);
}

svlFile::~svlFile()
{
    Close();
    delete Stream;
}

int svlFile::Open(const std::string& filepath, const OpenMode mode)
{
    if (Opened) return SVL_FAIL;

    Mode = mode;
    std::ios_base::openmode iosmode = (Mode == R) ? std::ios_base::in : std::ios_base::out;
    Stream = new std::fstream(filepath.c_str(), iosmode);
    if (Stream && Stream->fail() == false) Opened = true;

    if (Opened && Mode == R) {
        Seek(-1);
        Length = GetPos();
        Seek(0);
    }

    return (Opened ? SVL_OK : SVL_FAIL);
}

int svlFile::Close()
{
    if (!Opened) return SVL_FAIL;

    Stream->close();

    return SVL_OK;
}

bool svlFile::IsOpen()
{
    return Opened;
}

long long int svlFile::Read(char* buffer, const long long int length)
{
    if (!Opened || Mode != R) return -1;

    Stream->read(buffer, length);

    if (Stream->fail() && !Stream->eof()) return -1;
    return Stream->gcount();
}

long long int svlFile::Write(const char* buffer, const long long int length)
{
    if (!Opened || Mode != W) return -1;

    long long int pos = GetPos();

    Stream->write(buffer, length);

    if (Stream->bad()) return -1;
    return (GetPos() - pos);
}

long long int svlFile::GetLength()
{
    if (!Opened) return -1;

    if (Mode == R) return Length;

    long long int oripos, endpos;

    oripos = GetPos();
    if (oripos < 0) return -1;

    Seek(-1);
    endpos = GetPos();
    Seek(oripos);

    return endpos;
}

long long int svlFile::GetPos()
{
    if (!Opened) return -1;

    long long int pos;

    if (Mode == R) pos = Stream->tellg();
    else pos = Stream->tellp();

    return pos;
}

int svlFile::Seek(const long long int abspos)
{
    if (!Opened) return SVL_FAIL;

    if (abspos >= 0) {
        if (Mode == R) Stream->seekg(abspos, std::ios::beg);
        else Stream->seekp(abspos, std::ios::beg);

        if (Stream->eof()) return SVL_EOF;
    }
    else {
        if (Mode == R) Stream->seekg(0, std::ios::end);
        else Stream->seekp(0, std::ios::end);
    }

    if (Stream->fail() || Stream->bad()) return SVL_FAIL;
    return SVL_OK;
}

