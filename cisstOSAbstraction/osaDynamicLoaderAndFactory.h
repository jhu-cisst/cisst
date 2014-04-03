/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Peter Kazanzides
  Created on: 2007-01-16 

  (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*! 
  \file 
  \brief Declaration of osaDynamicLoaderAndFactory
*/

#ifndef _osaDynamicLoaderAndFactory_h
#define _osaDynamicLoaderAndFactory_h

#include <typeinfo>
#include <cisstOSAbstraction/osaDynamicLoader.h>
#include <cisstOSAbstraction/osaExport.h>

// This class serves as the base class for osaDynamicLoaderAndFactory and is not
// intended to be accessed otherwise. Note that the constructor is protected.
class CISST_EXPORT osaDynamicLoaderAndFactoryBase : public osaDynamicLoader
{
protected:
    void* create;
    void* destroy;

    osaDynamicLoaderAndFactoryBase() : create(0), destroy(0) {}
    bool Init(const char* name, const char* file, const char* path,
              const std::type_info& tinfo, int version);

public:
    virtual ~osaDynamicLoaderAndFactoryBase() {}

    void Reset();
};

/*!
  \brief Dynamically load a shared library file and find the factory methods
         in the file.

  \ingroup cisstOSAbstraction

  This is a templated class that is used to dynamically load "plugin" modules
  that are implemented as derived classes of the specified base class (template parameter).
  The advantage of this class over \a osaDynamicLoader is that any class can serve as
  the base class.  It is not necessary for the base or derived class to inherit from
  \a cmnGenericObject.   The only requirement is that it contain a public \a VERSION enum
  that specifies the "interface version" of the base class. The \a VERSION should be changed
  for any base class changes that could affect the derived classes (e.g., adding member
  data, changing public methods, etc.).

  If the base class is \a cmnGenericObject (or derived from it), the \a osaDynamicLoader
  class should be used instead.

  \sa osaDynamicLoader

 */

template <class BaseClass>
class osaDynamicLoaderAndFactory : public osaDynamicLoaderAndFactoryBase
{
    typedef BaseClass* CreateFunc();
    typedef void DestroyFunc(BaseClass*);

public:
    /*! Default constructor. Calls Reset() method. */
    osaDynamicLoaderAndFactory() {}
    /*! Destructor. Does not unload library in case any objects created from
        within the library still exist. To close library, call Reset(). */
    virtual ~osaDynamicLoaderAndFactory() {}

    /*! Load the derived class from the specified file. Note that a file may
        contain more than one derived class, but each derived class must
        be accessed via a new dynamic loader object. The operating system
        ensures that a library is only loaded once.
        \param name Name of derived class
        \param file File name for derived class (do not include extension)
        \param path Path to file (0 -> use default library load paths)
        \return true if sucessful; false otherwise
    */
    bool Init(const char *name, const char *file, const char *path = 0)
    { return osaDynamicLoaderAndFactoryBase::Init(name, file, path, typeid(BaseClass), BaseClass::VERSION); }

    /*! Load the derived class from a file that has the same name as the
        derived class (no extension).
        \param name Name of derived class
        \return true if sucessful; false otherwise
    */
    bool Init(const char* name)
    { return osaDynamicLoaderAndFactoryBase::Init(name, name, 0, typeid(BaseClass), BaseClass::VERSION); }

    /*! Unload the library file and clear the factory functions. Do not call this method unless
        you are sure that all instances of the derived class have already been destroyed.
    */
    void Reset()
    { osaDynamicLoaderAndFactoryBase::Reset(); }

    /*! Create a new instance of the derived class (from the dynamically loaded library).
        \return a pointer to the new instance (0 if class not initialized) */
    BaseClass* CreateObject() const
    { return create ? (*(reinterpret_cast<CreateFunc*>(create)))() : 0; }

    /*! Delete an instance of the derived class.
        \param obj Pointer to object to be destroyed.
    */
    void DestroyObject(BaseClass* obj) const
    { if (destroy) (*reinterpret_cast<DestroyFunc*>(destroy))(obj); }
};


#endif  // _osaDynamicLoaderAndFactory_h
