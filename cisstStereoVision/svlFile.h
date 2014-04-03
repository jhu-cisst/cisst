/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
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

#ifndef _svlFile_h
#define _svlFile_h

#include <cisstStereoVision/svlTypes.h>


class svlFile
{
public:
    enum OpenMode {
        R = 1,
        W = 2
    };

public:
    svlFile();
    svlFile(const svlFile& file);
    svlFile(const std::string& filepath, const OpenMode mode = R);
    virtual ~svlFile();

    virtual int Open(const std::string& filepath, const OpenMode mode = R);
    virtual int Close();
    virtual bool IsOpen();

    virtual long long int Read(char* buffer, const long long int length);
    virtual long long int Write(const char* buffer, const long long int length);

    template<class _ValueType>
    bool Read(_ValueType& value)
    {
        long long int len = sizeof(_ValueType);
        if (Read(reinterpret_cast<char*>(&value), len) < len) return false;
        return true;
    }

    template<class _ValueType>
    bool Write(const _ValueType& value)
    {
        long long int len = sizeof(_ValueType);
        if (Write(reinterpret_cast<const char*>(&value), len) < len) return false;
        return true;
    }

    virtual long long int GetLength();
    virtual long long int GetPos();
    virtual int Seek(const long long int abspos);

private:
    OpenMode      Mode;
    bool          Opened;
    long long int Length;

    // Internals that are OS-dependent in some way
    enum {INTERNALS_SIZE = 16};
    char* Internals;

    // Return the size of the actual object used by the OS.
    // This is used for testing only.
    static unsigned int SizeOfInternals();
};

#endif // _svlFile_h

