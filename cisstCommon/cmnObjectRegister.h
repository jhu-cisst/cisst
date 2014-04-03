/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Andy LaMora, Anton Deguet
  Created on: 2004-05-11

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#pragma once

#ifndef _cmnObjectRegister_h
#define _cmnObjectRegister_h

/*!
  \file
  \brief Defines cmnObjectRegister
*/

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegister.h>

#include <map>
#include <iostream>
#include <sstream>
#include <string>

#include <cisstCommon/cmnExport.h>


/*!
  \brief Object Register

  \ingroup cisstCommon

  The class register allows to register some objects by name.  Since
  the object register is implemented as a singleton, it allows to
  retrieve an object from anywhere.

  This mechanism can be used to share an object between different
  threads.  Another example is the embedding of a scripting language
  such as Python.  In this case, the programmer can use the Register()
  method to register an object with a given name.  Within the Python
  shell, it is then possible to retrieve by name a "pointer" on the
  same object.

  The main restriction is that all the registered objects must be
  derived from cmnGenericObject.
 */
class CISST_EXPORT cmnObjectRegister {

    /*! Simple typedefs to ease the declaration of iterators */
    //@{
    typedef std::map<std::string, cmnGenericObject *> ContainerType;
    typedef ContainerType::iterator iterator;
    typedef ContainerType::reverse_iterator reverse_iterator;

public:
    typedef ContainerType::const_iterator const_iterator;
    typedef ContainerType::const_reverse_iterator const_reverse_iterator;
    //@}

private:

    /*! Map of class pointers registered. */
    ContainerType ObjectContainer;


    /*! Instance specific implementation of Register.
      \sa Register */
    bool RegisterInstance(const std::string & objectName,
                          cmnGenericObject * objectPointer);


    /*! Instance specific implementation of Remove.
      \sa Remove */
    bool RemoveInstance(const std::string & objectName);


    /*! Instance specific implementation of FindObject

      \param objectName The name to look up.
      \sa FindObject
    */
    cmnGenericObject* FindObjectInstance(const std::string & objectName) const;


    /*! Instance specific implementation of FindName

      \param objectPointer The object to look up.
      \sa FindName
    */
    std::string FindNameInstance(cmnGenericObject * objectPointer) const;


    /*! Instance specific implementation of ToStream.
      \sa ToStream */
    void ToStreamInstance(std::ostream & outputStream) const;

    /*! Instance specific implementation of begin.
      \sa begin */
    const_iterator beginInstance(void) const {
        return ObjectContainer.begin();
    }

    /*! Instance specific implementation of end.
      \sa end */
    const_iterator endInstance(void) const {
        return ObjectContainer.end();
    }

protected:
    /*! Constructor.  The only constructor must be private in order to
      ensure that the object register is a singleton. */
    cmnObjectRegister() {};

    /*! Destructor.  Does nothing specific. */
    virtual ~cmnObjectRegister() {};

 public:

    /*! The object register is instantiated as a singleton.  To access
      the unique instantiation, one needs to use this static method.
      The instantiated object register is created at the first call of
      this method since it is a static variable declared in this
      method's scope.

      \return A pointer to the object register. */
    static cmnObjectRegister * Instance(void);


    /*!  The Register method registers an object pointer with a given
      name in the static register.

      \param objectName The name given to the soon registered object.

      \param objectPointer The pointer to the cmnGenericObject object
      (or any derived class).

      \return True if successful, false if the object has already been
      registered or the name has already been used.
    */
    static inline bool Register(const std::string & objectName,
                                cmnGenericObject * objectPointer) {
        return Instance()->RegisterInstance(objectName, objectPointer);
    }


    /*! The remove method allows to "un-register" a registered object.
      It doesn't affect the object itself.

      \param objectName The name given to the object when it was registered.

      \return True if the object was actually registered, false if the
      object was not registered. */
    static inline bool Remove(const std::string & objectName) {
        return Instance()->RemoveInstance(objectName);
    }


    /*! Get the object by name. Returns null if the object is
      not registered.

      \param objectName The name to look up.

      \return The pointer to the cmnGenericObject object
      corresponding to the className, or null if not registered.
    */
    static inline cmnGenericObject * FindObject(const std::string & objectName) {
        return Instance()->FindObjectInstance(objectName);
    }


    /*! Get the name of an object. Returns "undefined" if the object is
      not registered.

      \param objectPointer The object to look up.
    */
    static inline std::string FindName(cmnGenericObject * objectPointer) {
        return Instance()->FindNameInstance(objectPointer);
    }


    /*! Print the register content to an STL string and returns a copy of this
      string */
    static std::string ToString(void) {
        std::stringstream outputStream;
        ToStream(outputStream);
        return outputStream.str();
    }


    /*! Print the register content to a text stream */
    static inline void ToStream(std::ostream & outputStream) {
        return Instance()->ToStreamInstance(outputStream);
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


#endif // _cmnObjectRegister_h

