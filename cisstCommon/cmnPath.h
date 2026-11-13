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

#include <filesystem>
#include <string>
#include <list>
#include <iostream>

// Always the last cisst include
#include <cisstCommon/cmnExport.h>

/*!  \brief Search path to find a file.  This class contains a list of
  directories used to locate a file.

  \ingroup cisstCommon

  The directories can be added either at the head or the tail of the
  list.  This class doesn't check weither the directories in the path
  actually exist, mostly to allow a portable program (e.g. one program
  can add both "/usr/local/bin" and "C:\Program Files").
*/

// TODO BB: why is this a cmnGenericObject?
class CISST_EXPORT cmnPath: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    /*! Container used to store the directories. */
    typedef std::list<std::filesystem::path> ContainerType;

    /*! STL like iterators. */
    //@{
    typedef ContainerType::iterator iterator;
    typedef ContainerType::const_iterator const_iterator;
    //@}

private:
    ContainerType Paths;

public:
    enum class Position {
        HEAD,
        TAIL
    };

    /*! Create an empty search path */
    cmnPath();

    /*! Create a search path from a std::filesystem::path */
    cmnPath(const std::filesystem::path& path);

    /*! Create a search path from a string */
    cmnPath(const std::string& path);

    /*! Set the path from a std::filesystem::path. */
    void Set(const std::filesystem::path& path);

    /*! Set the path from a string. */
    void Set(const std::string& path);

    /*! Add a directory to the path. */
    void Add(const std::string& path, Position position = Position::HEAD);
    void Add(const std::filesystem::path& path, Position position = Position::HEAD);

    /*! Add one or more directories to the path using an environment variable. */
    bool AddFromEnvironment(const std::string& variableName, Position position = Position::HEAD);

    /*! Add path relative to the CISST_ROOT environment variable.
      This can be useful to find shared libraries.  To find data
      files, use AddRelativeToCisstShare. */
    bool AddRelativeToCisstRoot(const std::filesystem::path& relativePath, Position position = Position::HEAD);
    bool AddRelativeToCisstRoot(const std::string& relativePath, Position position = Position::HEAD);

    /*! Add path relative to the shared cisst directory,
      i.e. CISST_ROOT/share/cisst-<version>. */
    bool AddRelativeToCisstShare(const std::filesystem::path& relativePath, Position position = Position::HEAD);
    bool AddRelativeToCisstShare(const std::string& relativePath, Position position = Position::HEAD);

    /*! Find the full name for a given file.
      \return The full path including the filename or an empty string.
    */
    std::filesystem::path Find(const std::filesystem::path& filename) const;
    std::filesystem::path Find(const std::string& filename) const;

    /*! Find the full name for a given file using all directories in
      path as well as given "sub directory" in each directory.  E.g if
      the path contains /bin and /usr/bin and the sub-directory
      provided is "Release", the full search path if /bin/Release,
      /bin, /usr/bin/Release, /usr/bin.  One can also #include
      <cisstBuildType.h> which defines the string CISST_BUILD_SUFFIX.

      \return The full path including the filename or an empty string.
    */
    std::filesystem::path FindWithSubdirectory(
        const std::string& filename,
        const std::string& subdirectory
    ) const;

    /*! Remove the first occurence of a directory from the search list. */
    bool Remove(const std::filesystem::path& directory);
    bool Remove(const std::string& directory);

    /*! Indicates if a given directory is in the search list. */
    bool Has(const std::filesystem::path& directory) const;
    bool Has(const std::string& directory) const;

    /*! Write the path to a stream. */
    void ToStream(std::ostream & outputStream) const override;

    /*! Get CISST_ROOT from the environment variable */
    static bool GetCisstRoot(std::filesystem::path& result);

    /*! Get cisst shared directory based on CISST_ROOT from the
      environment variable */
    static bool GetCisstShare(std::filesystem::path& result);

    /*! Construct the shared library name based on OS prefix and
      suffix.  For example, the return value for "MyLib" would be
      "libMyLib.so" on most Unix systems and "MyLib.dll" on Windows.  Prefix
      and suffix are based on CMake and saved in cisstConfig.h. */
    static std::string SharedLibrary(const std::string & name) {
        return CISST_SHARED_LIBRARY_PREFIX + name + CISST_SHARED_LIBRARY_SUFFIX;
    }

    /*! Construct the executable name based on OS suffix.  For example,
      the return value for "MyProgram" would be "MyProgram" on most Unix
      systems and "MyProgram.exe" on Windows.  Suffix is based on CMake
      and saved in cisstConfig.h. */
    static std::string Executable(const std::string & name) {
        return name + CISST_EXECUTABLE_SUFFIX;
    }
};


// Add services instantiation
CMN_DECLARE_SERVICES_INSTANTIATION(cmnPath)


#endif // _cmnPath_h
