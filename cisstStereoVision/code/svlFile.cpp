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

/* Visual Studio.NET 2003 doesn't fully support the 64-bit functions _fseeki64, _ftelli64.
   It turns out that they are available in the static runtime libraries, but not the dynamic libraries.
   But, they are not defined in <stdio.h>, so it is necessary to add the following declarations:

   extern "C" int __cdecl _fseeki64(FILE *, __int64, int);
   extern "C" __int64 __cdecl _ftelli64(FILE *);

   Note that this only works for static libraries (with dynamic libraries, you get unresolved external
   symbols).

   In addition, the code uses other items that are not defined in the VS.NET 2003 header files,
   such as errno_t and SIZE_MAX.  I believe the following are the correct definitions:

   typedef int errno_t;
   #if defined(_WIN64) && defined(_UI64_MAX)
      #define SIZE_MAX _UI64_MAX
   #else
      #define SIZE_MAX UINT_MAX
   #endif

   Finally, VS.NET 2003 does not include the "secure" functions, such as fopen_s, so it is necessary
   to also define them:

   inline errno_t fopen_s(FILE ** pFile, const char *filename, const char *mode) {
      FILE *fp = fopen(filename, mode);
      if (fp != 0) {
         *pFile = fp;
         return 0;
      }
      else
         return EINVAL;
   }

   Even with all this, it is not known whether this would work for files > 2 GB with
   Visual Studio.NET 2003.

   For now, we just use the standard stream interface for VS.NET 2003. Note that
   the Read, Write, and Seek methods have been modified to only work for offsets less
   than INT_MAX/LONG_MAX (i.e., files < 2GB).
*/

#include <cisstStereoVision/svlFile.h>

#if (CISST_OS == CISST_WINDOWS) && (CISST_COMPILER > CISST_DOTNET2003)
    #include <errno.h>
#endif

/**********************************/
/*** svlFileInternals structure ***/
/**********************************/

struct svlFileInternals
{
#if (CISST_OS == CISST_WINDOWS) && (CISST_COMPILER > CISST_DOTNET2003)
    FILE *File;
#else // CISST_OS != CISST_WINDOWS
    std::fstream *Stream;
#endif // CISST_OS
};

#define INTERNALS(A) (reinterpret_cast<svlFileInternals*>(Internals)->A)


/*********************/
/*** svlFile class ***/
/*********************/

svlFile::svlFile() :
    Opened(false),
    Length(0)
{
    Internals = new char[INTERNALS_SIZE];

#if (CISST_OS == CISST_WINDOWS) && (CISST_COMPILER > CISST_DOTNET2003)
    INTERNALS(File) = 0;
#else // CISST_OS != CISST_WINDOWS
    INTERNALS(Stream) = 0;
#endif // CISST_OS
}

svlFile::svlFile(const svlFile& CMN_UNUSED(file)) :
    Opened(false),
    Length(0)
{
    Internals = new char[INTERNALS_SIZE];

#if (CISST_OS == CISST_WINDOWS) && (CISST_COMPILER > CISST_DOTNET2003)
    INTERNALS(File) = 0;
#else // CISST_OS != CISST_WINDOWS
    INTERNALS(Stream) = 0;
#endif // CISST_OS
}

svlFile::svlFile(const std::string& filepath, const OpenMode mode) :
    Opened(false),
    Length(0)
{
    Internals = new char[INTERNALS_SIZE];

#if (CISST_OS == CISST_WINDOWS) && (CISST_COMPILER > CISST_DOTNET2003)
    INTERNALS(File) = 0;
#else // CISST_OS != CISST_WINDOWS
    INTERNALS(Stream) = 0;
#endif // CISST_OS
    
    Open(filepath, mode);
}

svlFile::~svlFile()
{
    Close();

#if (CISST_OS == CISST_WINDOWS) && (CISST_COMPILER > CISST_DOTNET2003)
    // NOP
#else // CISST_OS != CISST_WINDOWS
    delete INTERNALS(Stream);
#endif // CISST_OS

    delete [] Internals;
}

unsigned int svlFile::SizeOfInternals()
{
    return sizeof(svlFileInternals);
}

int svlFile::Open(const std::string& filepath, const OpenMode mode)
{
    if (Opened) return SVL_FAIL;

    Mode = mode;

#if (CISST_OS == CISST_WINDOWS) && (CISST_COMPILER > CISST_DOTNET2003)
    errno_t err;
    if (Mode == R) err = fopen_s(&INTERNALS(File), filepath.c_str(), "rb");
    else           err = fopen_s(&INTERNALS(File), filepath.c_str(), "wb");
    if (INTERNALS(File) && err == 0) Opened = true;
#else // CISST_OS != CISST_WINDOWS
    std::ios_base::openmode iosmode = (Mode == R) ?
                                        std::ios_base::in  | std::ios_base::binary :
                                        std::ios_base::out | std::ios_base::binary | std::ios_base::trunc;
    INTERNALS(Stream) = new std::fstream(filepath.c_str(), iosmode);
    if (INTERNALS(Stream) && INTERNALS(Stream)->fail() == false) Opened = true;
#endif // CISST_OS

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

#if (CISST_OS == CISST_WINDOWS) && (CISST_COMPILER > CISST_DOTNET2003)
    if (INTERNALS(File)) {
        fclose(INTERNALS(File));
        INTERNALS(File) = 0;
    }
#else // CISST_OS != CISST_WINDOWS
    INTERNALS(Stream)->close();
#endif // CISST_OS

    Opened = false;

    return SVL_OK;
}

bool svlFile::IsOpen()
{
    return Opened;
}

long long int svlFile::Read(char* buffer, const long long int length)
{
    if (!Opened || Mode != R || length < 0) return -1;

#if (CISST_OS == CISST_WINDOWS) && (CISST_COMPILER > CISST_DOTNET2003)
    size_t readsize, size = 0;
    unsigned long long int _length = length;
    if (_length > static_cast<long long int>(SIZE_MAX)) readsize = SIZE_MAX;
    else readsize = static_cast<size_t>(_length);
    do {
        size += fread(buffer + size, 1, readsize - size, INTERNALS(File));
    }
    while (size < readsize && errno == EINVAL);
    if (size < readsize && !feof(INTERNALS(File))) return -1;
    return size;
#else // CISST_OS != CISST_WINDOWS
#if (CISST_COMPILER == CISST_DOTNET2003)
    INTERNALS(Stream)->read(buffer, (length > INT_MAX)?INT_MAX:static_cast<std::streamsize>(length));
#else
    INTERNALS(Stream)->read(buffer, length);
#endif
    if (INTERNALS(Stream)->fail()) {
        if (INTERNALS(Stream)->eof()) INTERNALS(Stream)->clear();
        else return -1;
    }
    return INTERNALS(Stream)->gcount();
#endif // CISST_OS
}

long long int svlFile::Write(const char* buffer, const long long int length)
{
    if (!Opened || Mode != W || length < 0) return -1;

#if (CISST_OS == CISST_WINDOWS) && (CISST_COMPILER > CISST_DOTNET2003)
    size_t writesize, size = 0;
    unsigned long long int _length = length;
    if (_length > static_cast<long long int>(SIZE_MAX)) writesize = SIZE_MAX;
    else writesize = static_cast<size_t>(_length);
    do {
        size += fwrite(buffer + size, 1, writesize - size, INTERNALS(File));
    }
    while (size < writesize && errno == EINVAL);
    return size;
#else // CISST_OS != CISST_WINDOWS
    long long int pos = GetPos();
#if (CISST_COMPILER == CISST_DOTNET2003)
    INTERNALS(Stream)->write(buffer, (length > INT_MAX)?INT_MAX:static_cast<std::streamsize>(length));
#else
    INTERNALS(Stream)->write(buffer, length);
#endif
    if (INTERNALS(Stream)->bad()) return -1;
    return (GetPos() - pos);
#endif // CISST_OS
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

#if (CISST_OS == CISST_WINDOWS) && (CISST_COMPILER > CISST_DOTNET2003)
    pos = _ftelli64(INTERNALS(File));
#else // CISST_OS != CISST_WINDOWS
    if (Mode == R) pos = INTERNALS(Stream)->tellg();
    else pos = INTERNALS(Stream)->tellp();
#endif // CISST_OS

    return pos;
}

int svlFile::Seek(const long long int abspos)
{
    if (!Opened) return SVL_FAIL;

    if (abspos >= 0) {

#if (CISST_OS == CISST_WINDOWS) && (CISST_COMPILER > CISST_DOTNET2003)
        if (_fseeki64(INTERNALS(File), abspos, SEEK_SET)) {
            if (feof(INTERNALS(File))) return SVL_EOF;
            else return SVL_FAIL;
        }
#else // CISST_OS != CISST_WINDOWS
#if (CISST_COMPILER == CISST_DOTNET2003)
        if (Mode == R) INTERNALS(Stream)->seekg((abspos>LONG_MAX)?LONG_MAX:static_cast<std::streamoff>(abspos), std::ios::beg);
        else INTERNALS(Stream)->seekp((abspos>LONG_MAX)?LONG_MAX:static_cast<std::streamoff>(abspos), std::ios::beg);
#else
        if (Mode == R) INTERNALS(Stream)->seekg(abspos, std::ios::beg);
        else INTERNALS(Stream)->seekp(abspos, std::ios::beg);
#endif
        if (INTERNALS(Stream)->eof()) {
            INTERNALS(Stream)->clear();
            return SVL_EOF;
        }
#endif // CISST_OS

    }
    else {

#if (CISST_OS == CISST_WINDOWS) && (CISST_COMPILER > CISST_DOTNET2003)
        if (_fseeki64(INTERNALS(File), 0, SEEK_END)) return SVL_FAIL;
#else // CISST_OS != CISST_WINDOWS
        if (Mode == R) INTERNALS(Stream)->seekg(0, std::ios::end);
        else INTERNALS(Stream)->seekp(0, std::ios::end);
#endif // CISST_OS

    }

#if (CISST_OS == CISST_WINDOWS) && (CISST_COMPILER > CISST_DOTNET2003)
    // NOP
#else // CISST_OS != CISST_WINDOWS
    if (INTERNALS(Stream)->fail() || INTERNALS(Stream)->bad()) return SVL_FAIL;
#endif // CISST_OS

    return SVL_OK;
}

