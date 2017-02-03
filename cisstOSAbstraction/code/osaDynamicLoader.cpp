/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Peter Kazanzides
  Created on: 2007-01-16

  (C) Copyright 2007-2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <string>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnPath.h>
#include <cisstOSAbstraction/osaDynamicLoader.h>

#if (CISST_OS == CISST_WINDOWS)
const char DOS_SEP = '\\';  // path separator for DOS/Windows
#endif

const char UNIX_SEP = '/';  // path separator for UNIX


#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
inline void *OpenLibrary(const char *libname)
       { return (void *)LoadLibrary(libname); }
inline void CloseLibrary(void *handle)
       { FreeLibrary((HMODULE)handle); }
inline int GetTheError()
       { return GetLastError(); }
const char CURRENT_SEP = DOS_SEP;

#else
// Everything else (hopefully) uses the dlopen interface. This is
// true for Linux, RTAI, and Mac OS X 10.3 or higher. For older Macs,
// the dlopen interface is available if the dlcompat package is installed.
#include <dlfcn.h>
inline void *OpenLibrary(const char *libname)
       { return dlopen(libname, RTLD_LAZY); }  // could be RTLD_NOW
inline void CloseLibrary(void *handle)
       { dlclose(handle); }
inline const char *GetTheError()
       { return dlerror(); }
const char CURRENT_SEP = UNIX_SEP;
#endif

bool osaDynamicLoader::Load(const char *file, const char *path)
{
    // Don't call reset in case any objects created with the previous library still exist.
    // Reset();
    std::string fullpath;
    // if the file exists, use it as is...
    if (cmnPath::Exists(file)) {
        fullpath = file;
    } else {
        // check if a path has been provided
        if (path) {
            fullpath = path;
            // Add trailing separator if not already specified.
            if (fullpath[fullpath.size()-1] != CURRENT_SEP)
                fullpath.append(1, CURRENT_SEP);
        }
        fullpath.append(CISST_SHARED_LIBRARY_PREFIX);
        fullpath.append(file);
        fullpath.append(CISST_SHARED_LIBRARY_SUFFIX);
    }
#if (CISST_OS == CISST_WINDOWS)
    // Microsoft claims that LoadLibrary() requires backslashes, so make sure we have them
    // even though it seems to work with forward slashes.
    replace(fullpath.begin(), fullpath.end(), UNIX_SEP, CURRENT_SEP);
#endif
    handle = OpenLibrary(fullpath.c_str());
    if (!handle) {
        CMN_LOG_INIT_ERROR << "osaDynamicLoader: cannot open library " << fullpath << ", error = " << GetTheError() << std::endl;
        return false;
    }

#if (CISST_OS == CISST_WINDOWS)
    char fullname[120];
    GetModuleFileName((HMODULE)handle, (LPTSTR)fullname, sizeof(fullname)-1);
    fullname[sizeof(fullname)-1] = 0;  // Make sure null-terminated
    CMN_LOG_INIT_VERBOSE << "osaDynamicLoader: loading " << fullname << std::endl;
#else
    CMN_LOG_INIT_VERBOSE << "osaDynamicLoader: loading " << fullpath << std::endl;
#endif
    return true;
}

void osaDynamicLoader::UnLoad()
{
    if (handle) {
        CloseLibrary(handle);
        handle = 0;
    }
}
