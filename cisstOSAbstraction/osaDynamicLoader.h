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


/*!
  \file
  \brief Declaration of osaDynamicLoader
*/

#ifndef _osaDynamicLoader_h
#define _osaDynamicLoader_h

#include <cisstOSAbstraction/osaExport.h>

/*!
  \brief Dynamically load a shared library file.

  \ingroup cisstOSAbstraction

  This class is used to dynamically load a shared library file.
  If a path is specified, the library file must exist on that path;
  otherwise, the "standard" library paths are used.

  This class should be used to dynamically load a library file if
  it contains classes that are derived from \a cmnGenericObject, assuming
  that they are set up for dynamic creation (e.g., \a CMN_DYNAMIC_CREATION flag
  used with \a CMN_DECLARE_SERVICES). For example, if the class "derivedClass"
  exists in a library file with the same name, the file can be dynamically
  loaded and a new instance created as follows:
      \code
      osaDynamicLoader dload;
      dload.Load("derivedClass");
      cmnGenericObject *obj = cmnObjectRegister::Instance()->Create("derivedClass");
      \endcode
  Note that \c dynamic_cast can be used to cast \a obj up the hierarchy.
 */

class CISST_EXPORT osaDynamicLoader
{
protected:
    void* handle;

public:
    /*! Default constructor. Does nothing. */
    osaDynamicLoader() : handle(0) {}

    /*! Destructor. Does not unload library in case any objects created from
        within the library still exist. To unload library, call Unload(). */
    virtual ~osaDynamicLoader() {};

    /*! Dynamically load the specified shared library.
        \param file Name of shared library file to load (do not include extension)
        \param path Path to file (0 -> use default library load paths)
        \return true if file successfully loaded; false otherwise
    */
    bool Load(const char *file, const char *path = 0);
    inline bool Load(const std::string & file,
                     const std::string & path = "") {
        if (path.empty()) {
            return Load(file.c_str());
        }
        return Load(file.c_str(), path.c_str());
    }

    /*! Unload the shared library file (if supported by operating system). */
    void UnLoad(void);
};

#endif  // _osaDynamicLoader_h
