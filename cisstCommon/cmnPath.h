/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2005-04-18

  (C) Copyright 2005-2023 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Declaration of cmnPath
  \ingroup cisstCommon
*/
#pragma once

#ifndef _cmnPath_h
#define _cmnPath_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnTokenizer.h>

#include <string>
#include <list>
#include <sstream>
#include <iostream>

// Always the last cisst include
#include <cisstCommon/cmnExport.h>


#if (CISST_OS == CISST_WINDOWS)
#  include <io.h>
#else
#  include <unistd.h>
#endif


/*!  \brief Search path to find a file.  This class contains a list of
  directories used to locate a file.

  \ingroup cisstCommon

  The directories can be added either at the head or the tail of the
  list.  This class doesn't check weither the directories in the path
  actually exist, mostly to allow a portable program (e.g. one program
  can add both "/usr/local/bin" and "C:\Program Files").

  As a convention, the separator between subdirectories should be a
  "/" (slash) and the separation between two directories in the path
  is ";" (semi-colon).  For example, "/bin;/usr/bin" will add both
  "/bin" and "/usr/bin".
*/
class CISST_EXPORT cmnPath: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    /*! Container used to store the directories. */
    typedef std::list<std::string> ContainerType;

    /*! STL like iterators. */
    //@{
    typedef ContainerType::iterator iterator;
    typedef ContainerType::const_iterator const_iterator;
    //@}

private:
    ContainerType Path;
    cmnTokenizer Tokenizer;

    /*! Private method called by each constructor to configure the
      tokenizer. */
    void ConfigureTokenizer(void);

    /*! Private methods to convert from native format to internal
      format, i.e. convert directory separator from ":" to ";" */
    static std::string FromNative(const std::string & nativePath);

public:
    /*! Defines how to add a path to the search list. */
    enum {HEAD = true,
          TAIL = false
    };

    /*! Defines the mode to be used for a given file. */
#if (CISST_OS == CISST_WINDOWS)
    enum {EXIST = 00,
          WRITE = 02,
          READ = 04,
          EXECUTE = 04
    };
#else
    enum {READ = R_OK,
          WRITE = W_OK,
          EXECUTE = X_OK,
          EXIST = F_OK
    };
#endif

    cmnPath(void);

    /*! Create a search path from a string. */
    cmnPath(const std::string & path);

    /*! Destructor */
    virtual ~cmnPath(void) {}

    /*! Set the path from a string. */
    void Set(const std::string & path);

    /*! Add one or more directories to the path. */
    void Add(const std::string & path, bool head = HEAD);

    /*! Add one or more directories to the path using an environment variable. */
    bool AddFromEnvironment(const std::string & variableName, bool head = HEAD);

    /*! Add path relative to the CISST_ROOT environment variable.
      This can be useful to find shared libraries.  To find data
      files, use AddRelativeToCisstShare. */
    bool AddRelativeToCisstRoot(const std::string & relativePath, bool head = HEAD);

    /*! Add path relative to the shared cisst directory,
      i.e. CISST_ROOT/share/cisst-<version>. */
    bool AddRelativeToCisstShare(const std::string & relativePath, bool head = HEAD);

    /*! Find the full name for a given file.
      \return The full path including the filename or an empty string.
    */
    std::string Find(const std::string & filename, short mode = READ) const;

    /*! Find the full name for a given file using all directories in
      path as well as given "sub directory" in each directory.  E.g if
      the path contains /bin and /usr/bin and the sub-directory
      provided is "Release", the full search path if /bin/Release,
      /bin, /usr/bin/Release, /usr/bin.  One can also #include
      <cisstBuildType.h> which defines the string CISST_BUILD_SUFFIX.

      \return The full path including the filename or an empty string.
    */
    std::string FindWithSubdirectory(const std::string & filename,
                                     const std::string & subdirectory,
                                     short mode = READ) const;

    /*! Remove the first occurence of a directory from the search list. */
    bool Remove(const std::string & directory);

    /*! Indicates if a given directory is in the search list. */
    bool Has(const std::string & directory) const;

    /*! Write the path to a stream. */
    void ToStream(std::ostream & outputStream) const;

    /*! A platform-independent string for separating subdirectory or file
      name from the parent directory name.  It's equal to "/" on Linux, "\\"
      on Windows, etc.
    */
    static const std::string & DirectorySeparator(void);

    /*! Get working directory */
    static std::string GetWorkingDirectory(void);

    /*! Get CISST_ROOT from the environment variable */
    static bool GetCisstRoot(std::string & result);

    /*! Get cisst shared directory based on CISST_ROOT from the
      environment variable */
    static bool GetCisstShare(std::string & result);

    /*! Check if a file exists assuming the path provided is either
      relative or absolute, this function doesn't use any search
      path. */
    static bool Exists(const std::string & fullPath, short more = READ);

    /*! Delete file.  This function doesn't use any search path. */
    static bool DeleteFile(const std::string & fullPath);

    /*! Rename file.  This function doesn't use any search path. */
    static bool RenameFile(const std::string & fullPathOld,
                           const std::string & fullPathNew);

    /*! Construct the shared library name based on OS prefix and
      suffix.  For example, the return value for "MyLib" would be
      "libMyLib.so" on most Unix systems and "MyLib.dll" on Windows.  Prefix
      and suffix are based on CMake and saved in cisstConfig.h. */
    inline static std::string SharedLibrary(const std::string & name) {
        return CISST_SHARED_LIBRARY_PREFIX + name + CISST_SHARED_LIBRARY_SUFFIX;
    }

    /*! Construct the executable name based on OS suffix.  For example,
      the return value for "MyProgram" would be "MyProgram" on most Unix
      systems and "MyProgram.exe" on Windows.  Suffix is based on CMake
      and saved in cisstConfig.h. */
    inline static std::string Executable(const std::string & name) {
        return name + CISST_EXECUTABLE_SUFFIX;
    }
};


// Add services instantiation
CMN_DECLARE_SERVICES_INSTANTIATION(cmnPath)


#endif // _cmnPath_h
