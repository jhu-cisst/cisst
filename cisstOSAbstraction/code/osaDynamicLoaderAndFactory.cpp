/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Peter Kazanzides
  Created on: 2007-01-16

  (C) Copyright 2007-2016 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <string>
#include <string.h>

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstOSAbstraction/osaDynamicLoaderAndFactory.h>

#if (CISST_OS == CISST_WINDOWS)
#include <windows.h>
inline void *GetSymbolAddr(void *handle, const char *name)
       { return (void *)GetProcAddress((HMODULE)handle,name); }

#else
// Everything else (hopefully) uses the dlopen interface. This is
// true for Linux, RTAI, and Mac OS X 10.3 or higher. For older Macs,
// the dlopen interface is available if the dlcompat package is installed.
#include <dlfcn.h>
inline void *GetSymbolAddr(void *handle, const char *name)
       { return dlsym(handle, name); }
#endif

void osaDynamicLoaderAndFactoryBase::Reset()
{
    UnLoad();
    create = 0;
    destroy = 0;
}

bool osaDynamicLoaderAndFactoryBase::Init(const char *name, const char *file, const char *path,
                                          const std::type_info& tinfo, int version)
{
    // Don't call Reset in case any objects created with the previous library still exist.
    // Reset();
    create = 0;
    destroy = 0;
    if (!Load(file, path))
        return false;
    std::string funcname(name);
    size_t namelen = funcname.size();
    funcname.append("BaseType");
    const std::type_info **basetypep = (const std::type_info **)GetSymbolAddr(handle, funcname.c_str());
    if (basetypep) {
        const char *loaded_basetype = (*basetypep)->name();
        if (strcmp(tinfo.name(), loaded_basetype) != 0) {
            CMN_LOG_INIT_ERROR << "osaDynamicLoaderAndFactory: inconsistent base types (expected=" << tinfo.name()
                               << ", loaded=" << loaded_basetype << ") for " << name << std::endl;
            Reset();
            return false;
        }
        // It is not clear whether comparing type_info structures will always work because this is just
        // a pointer (address) comparison and therefore relies on having a single type_info structure
        // for the base class (rather than one in the main module and another in the dynamically-loaded
        // module). So far, it seems to work, but if not we do not consider it an error.
        if (tinfo != **basetypep) {
            CMN_LOG_INIT_WARNING << "osaDynamicLoaderAndFactory: comparison of type_info failed (ignored)." << std::endl;
        }
    }
    else {
        CMN_LOG_INIT_ERROR << "osaDynamicLoaderAndFactory: could not find BaseType for " << name << std::endl;
        Reset();
        return false;
    }
    funcname.erase(namelen);
    funcname.append("Version");
    const int *versionp = (const int *)GetSymbolAddr(handle, funcname.c_str());
    if (versionp) {
        if (version != *versionp) {
            CMN_LOG_INIT_ERROR << "osaDynamicLoaderAndFactory: inconsistent versions (expected=" << version
                               << ", loaded=" << *versionp << ") for " << name << std::endl;
            Reset();
            return false;
        }
    }
    else {
        CMN_LOG_INIT_ERROR << "osaDynamicLoaderAndFactory: could not find Version for " << name << std::endl;
        Reset();
        return false;
    }
    funcname.erase(namelen);
    funcname.append("Create");
    create = GetSymbolAddr(handle, funcname.c_str());
    if (!create) {
        CMN_LOG_INIT_ERROR << "osaDynamicLoaderAndFactory: could not find create function for " << name << std::endl;
        Reset();
        return false;
    }
    funcname.erase(namelen);
    funcname.append("Destroy");
    destroy = GetSymbolAddr(handle, funcname.c_str());
    if (!destroy) {
        CMN_LOG_INIT_ERROR << "osaDynamicLoaderAndFactory: could not find destroy function for " << name << std::endl;
        Reset();
        return false;
    }
    CMN_LOG_INIT_VERBOSE << "osaDynamicLoaderAndFactory: loaded " << name << " (derived from " << tinfo.name() << ", version "
                         << version << ")." << std::endl;
    return true;
}
