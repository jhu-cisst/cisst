/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Alvin Liem, Anton Deguet
  Created on: 2002-08-01

  (C) Copyright 2002-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Class register definitions and log macros.
*/
#pragma once

#ifndef _cmnClassRegister_h
#define _cmnClassRegister_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnForwardDeclarations.h>
#include <cisstCommon/cmnLogger.h>

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <typeinfo>
#include <fstream>

#include <cisstCommon/cmnExport.h>


/*!
  \brief Main register for classes.

  \ingroup cisstCommon

  This class handles the registration of classes.  The registration
  process allows to retrieve by name some information about a specific
  class from a centralized point.  The current version of the class
  register allows to:

  - Create dynamically an object based on a name provided as a string.
    This feature is required for the deserialization process (from a
    buffer and a name, create a new object of a given type defined by
    a string).

  - Modify the logging level of detail for a given class.  See
    cmnLogger for more details regarding the logging in cisst.

  Since the dynamic creation requires a base class, we have introduced
  the cmnGenericObject class in cisstCommon.  One can only register
  classes derived from cmnGenericObject.  The registration requires
  the use of #CMN_DECLARE_SERVICES (or #CMN_DECLARE_SERVICES_EXPORT),
  #CMN_DECLARE_SERVICES_INSTANTIATION (or
  #CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT) and
  #CMN_IMPLEMENT_SERVICES (or #CMN_IMPLEMENT_SERVICES_TEMPLATED).

  Here are the details of the implementation. The macro
  #CMN_DECLARE_SERVICES defines a method called Services() for each
  registered class. This method returns a pointer on a static data
  member of type cmnClassServices. This cmnClassServices object is
  unique for that class. While the reasons for writing a method which
  returns a pointer on a static data member might seem confusing, they
  are actually quite simple. This method is used to ensure that the
  data member is initialized the first time it is used. This is done
  to avoid problems related to the order of instantiation of the
  static data members when used before <code>main()</code>.

  When the Services() method defined in the #CMN_IMPLEMENT_SERVICES
  macro is called, the cmnClassServices object registers itself within
  static class register. This allows us to keep a single list of all
  the classes that exist in a particular program. The register is
  implemented as a map of pointers to the cmnClassServices objects
  that exist.

  It is important to note that all classes to be registered will be
  registered by the time the <code>main()</code> function is called.
  Since we rely on the creation of global static objects for the
  registration and the order of instantiation of these objects before
  <code>main()</code> is unknown, classes might be register or not
  before the <code>main()</code> execution.
 */
class CISST_EXPORT cmnClassRegister {
 public:
    /*! Simple typedefs to ease the declaration of iterators */
    //@{
    typedef std::map<std::string, cmnClassServicesBase*> ServicesContainerType;
    typedef ServicesContainerType::value_type EntryType;

    typedef ServicesContainerType::const_iterator const_iterator;
    typedef ServicesContainerType::const_reverse_iterator const_reverse_iterator;

    typedef ServicesContainerType::size_type size_type;

 private:
    typedef ServicesContainerType::iterator iterator;
    typedef ServicesContainerType::reverse_iterator reverse_iterator;
    //@}

    /*! List of class services registered. */
    ServicesContainerType ServicesContainer;

 public:
    /*!
      Instance specific implementation of FindClassServices.

      \sa FindClassServices
    */
    cmnClassServicesBase * FindClassServicesInstance(const std::string & className);

    /*!
      Instance specific implementation of FindClassServices.

      \sa FindClassServices
    */
    cmnClassServicesBase * FindClassServicesInstance(const std::type_info & typeInfo);

    /*! Instance specific implementation of Register.
      \sa Register */
    const std::string *
    RegisterInstance(cmnClassServicesBase * classServicesPointer,
                     const std::string & className);

    /*! Instance specific implementation of SetLogMaskClassAll
      \sa SetLogMaskClassAll */
    bool SetLogMaskClassAllInstance(cmnLogMask mask);

    /*! Instance specific implementation of SetLogMaskClassMatching.
      \sa SetLogMaskClassMatching */
    bool SetLogMaskClassMatchingInstance(const std::string & stringToMatch, cmnLogMask mask);

    /*! Instance specific implementation of ToString.
      \sa ToString */
    std::string ToStringInstance(void) const;

    /*! Instance specific implementation of ToStream.
      \sa ToStream. */
    void ToStreamInstance(std::ostream & outputStream) const;

    /*! Instance specific implementation of size.
      \sa size */
    inline size_type sizeInstance(void) const {
        return ServicesContainer.size();
    }

    /*! Instance specific implementation of begin.
      \sa begin */
    inline const_iterator beginInstance(void) const {
        return ServicesContainer.begin();
    }

    /*! Instance specific implementation of end.
      \sa end */
    inline const_iterator endInstance(void) const {
        return ServicesContainer.end();
    }

protected:
    /*! Constructor.  The only constructor must be private in order to
      ensure that the class register is a singleton. */
    inline cmnClassRegister(void) {};

 public:

    /*! The class register is instantiated as a singleton.  To access
      the unique instantiation, one needs to use this static method.
      The instantiated class register is created at the first call of
      this method since it is a static variable declared in this
      method's scope.

      \return A pointer to the class register. */
    static cmnClassRegister * Instance(void);


    /*!  The Register method registers a class pointer in the static
      register. It MUST NOT be used directly.  It is used by the
      #CMN_DECLARE_SERVICES macro.

      \param classServicesPointer The pointer to the cmnClassServices
      object.

      \param className The name given to the class as a string.  To
      enforce the consistency, we use the macro string conversion
      (#).

      PK: FIX THIS
      \return bool True if successful, false if the class has not been
      registered (e.g. one can not register twice).  This might happen
      if a programmer doesn't give the right string name for the class
      to be registered with the macros #CMN_DECLARE_SERVICES,
      #CMN_DECLARE_SERVICES_INSTANTIATION and #CMN_IMPLEMENT_SERVICES.
    */
    static inline const std::string *
    Register(cmnClassServicesBase * classServicesPointer,
             const std::string & className)
    {
        return Instance()->RegisterInstance(classServicesPointer,
                                            className);
    }


    /*! The SetLogMaskClass method allows the user to specify the log
      mask for a specific class. It checks to see if the class is
      registered. If so, it updates the cmnClassServices object
      directly. Otherwise, it log a warning message.

      \param className The name of the class
      \param mask The log mask to be applied

      \return bool True if the class is registered.
    */
    static bool SetLogMaskClass(const std::string & className, cmnLogMask mask);

    static inline bool CISST_DEPRECATED SetLoD(const std::string & className, cmnLogMask mask) {
        return SetLogMaskClass(className, mask);
    }


    /*! The SetLogMaskClassAll method allows the user to specify the
      log mask for all registered classes.

      \param mask The log mask to be applied

      \return bool True if there is at least one class log mask was
      modified
    */
    static inline bool SetLogMaskClassAll(cmnLogMask mask) {
        return Instance()->SetLogMaskClassAllInstance(mask);
    }

    static inline bool CISST_DEPRECATED SetLoDForAllClasses(cmnLogMask mask) {
        return SetLogMaskClassAll(mask);
    }


    /*! The SetLogMaskClassMatching method allows the user to specify
      the log mask for all classes with a name matching a given
      string.  The string matching is case sensitive (exact match).

      \param stringToMatch A string found in class names (e.g. "cmn")
      \param mask The log mask to be applied

      \return bool True if there is at least one class log mask was
      modified
    */
    static inline bool SetLogMaskClassMatching(const std::string & stringToMatch, cmnLogMask mask) {
        return Instance()->SetLogMaskClassMatchingInstance(stringToMatch, mask);
    }

    static inline bool CISST_DEPRECATED SetLoDForMatchingClasses(const std::string & stringToMatch, cmnLogMask mask) {
        return Instance()->SetLogMaskClassMatching(stringToMatch, mask);
    }


    /*! Get the class services by name. Returns null if the class is
      not registered.

      \param className The name to look up.

      \return The pointer to the cmnClassServicesBase object
      corresponding to the className, or null if not registered.
    */
    static inline cmnClassServicesBase * FindClassServices(const std::string & className) {
        return Instance()->FindClassServicesInstance(className);
    }

    /*! Get the class services by std::type_info.  This allows to find
      a registered class base on its C++ RTTI type_info, as defined by
      the standard function <code>typeid()</code>.  Returns null if
      the class is not registered.

      \param typeInfo The std::type_info to look up.

      \return The pointer to the cmnClassServicesBase object
      corresponding to the type info, or null if not registered.

    */
    static inline cmnClassServicesBase * FindClassServices(const std::type_info & typeInfo) {
        return Instance()->FindClassServicesInstance(typeInfo);
    }


    /*! Dynamic creation of objects using the default constructor.

    \param className The name of the class of the object to be created.

    \return A pointer on cmnGenericObject, NULL if the register
    doesn't have this class registered. */
    static cmnGenericObject * Create(const std::string & className);

    /*! Dynamic creation of objects using copy constructor

    \param className The name of the class of the object to be created.
    \param other The object to be copied

    \return A pointer on cmnGenericObject, NULL if the register
    doesn't have this class registered or if the object provided is
    not of the right class type. */
    static cmnGenericObject * Create(const std::string & className,
                                     const cmnGenericObject & other);


    /*! Print the register content to an STL string and returns a copy
      of this string. */
    static std::string ToString(void);


    /*! Print the register content to a text stream. */
    static void ToStream(std::ostream & outputStream) {
        Instance()->ToStreamInstance(outputStream);
    }


    /*! Size of class register, i.e. number of classes. */
    static size_type size(void) {
        return Instance()->sizeInstance();
    }

    /*! Begin const iterator. */
    static const_iterator begin(void) {
        return Instance()->beginInstance();
    }


    /*! End const iterator. */
    static const_iterator end(void) {
        return Instance()->endInstance();
    }

};


/*! Stream out operator. */
inline
std::ostream & operator << (std::ostream & output,
                            const cmnClassRegister & classRegister) {
    classRegister.ToStream(output);
    return output;
}


#endif // _cmnClassRegister_h

