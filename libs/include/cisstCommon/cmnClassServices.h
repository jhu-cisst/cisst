/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2004-08-18

  (C) Copyright 2004-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Defines cmnClassServices
*/
#pragma once

#ifndef _cmnClassServices_h
#define _cmnClassServices_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnForwardDeclarations.h>
#include <cisstCommon/cmnClassServicesBase.h>


/*! This class is a helper for cmnClassServices.  Its goal is to
  specialize the method Create() based on the _hasDynamicCreation
  parameter to either create a new object of type _class (which
  requires a public default constructor) or do nothing and return 0
  (null pointer).  The latter means that dynamic creation is disabled.
*/
template <bool _hasDynamicCreation, typename _class>
class cmnConditionalObjectFactory;


/*! Specialization of cmnConditionalObjectFactory with disabled
  dynamic creation.
*/
template<typename _class>
class cmnConditionalObjectFactory<CMN_NO_DYNAMIC_CREATION, _class>
{
public:
    typedef cmnGenericObject * generic_pointer;

     /*! Specialization of create when dynamic creation is
      disabled.  Returns 0 (null pointer). */
    inline static cmnGenericObject * Create(void) {
        return 0;
    }

    /*! Specialization of create when dynamic create is disabled.
        Returns 0 (null pointer) .*/
    inline static cmnGenericObject * Create(const cmnGenericObject & CMN_UNUSED(other)) {
         return 0;
    }

    /*! Specialization of create for in place new with copy constructor */
    inline static bool Create(cmnGenericObject * CMN_UNUSED(existing), const cmnGenericObject & CMN_UNUSED(other)) {
        return false;
    }

    /*! Specialization of create when dynamic creation is
       disabled.  Returns 0 (null pointer). */
    inline static cmnGenericObject * CreateArray(size_t CMN_UNUSED(size)) {
        return 0;
    }


    /*! Specialization of create when dynamic creation is
       disabled.  Returns 0 (null pointer). */
    inline static cmnGenericObject * CreateArray(size_t CMN_UNUSED(size), const cmnGenericObject & CMN_UNUSED(other)) {
        return 0;
    }

    /*! Specialization of create when dynamic creation is
       disabled.  Returns false. */
    inline static bool DeleteArray(generic_pointer & CMN_UNUSED(data), size_t & CMN_UNUSED(size)) {
        return false;
    }

    /*! Specialization of delete */
    inline static bool Delete(cmnGenericObject * CMN_UNUSED(existing)) {
        return false;
    }

};


/*! Specialization of cmnConditionalObjectFactory with enabled dynamic
  creation.
*/
template <typename _class>
class cmnConditionalObjectFactory<CMN_DYNAMIC_CREATION, _class>
{
public:
    typedef _class value_type;
    typedef _class * pointer;
    typedef cmnGenericObject * generic_pointer;

    /*! Specialization of create when dynamic creation is
      enabled.  Call new for the given class.  This method
      requires a default constructor for the aforementioned
      class. */
    inline static cmnGenericObject * Create(void) {
        return new value_type;
    }


    /*! Specialization of create(other) when dynamic creation is
      enabled.  Call new for the given class.  This method
      requires a copy constructor for the aforementioned class. */
    inline static cmnGenericObject * Create(const cmnGenericObject & other) {
        const value_type * otherPointer = dynamic_cast<const value_type *>(&other);
        if (otherPointer) {
            return new value_type(*otherPointer);
        } else {
            return 0;
        }
    }

    /*! Specialization of create(other) when dynamic creation is
      enabled.  Call new for the given class.  This method
      requires a copy constructor for the aforementioned class. 
      Returns false on failure, in which case input parameter (existing) is
      not modified.
     */
    inline static bool Create(cmnGenericObject * existing, const cmnGenericObject & other) {
        const value_type * otherPointer = dynamic_cast<const value_type *>(&other);
        if (otherPointer) {
            new(existing) value_type(*otherPointer);
            return true;
        } else {
            return false;
        }
    }


    /*! Specialization of create when dynamic creation is
      enabled.  Call new[size] for the given class.  This method
      requires a default constructor for the aforementioned
      class. */
    inline static cmnGenericObject * CreateArray(size_t size) {
        return new value_type[size];
    }


    /*! Specialization of create when dynamic creation is
      enabled.  Call the global operator new to allocate a block of memory
      and then use the placement new for each object.  The code followed by
      in place new.  This method requires a default constructor for the
      aforementioned class. */
    inline static cmnGenericObject * CreateArray(size_t size, const cmnGenericObject & other) {
        const value_type * otherPointer = dynamic_cast<const value_type *>(&other);
        if (otherPointer) {
            pointer data = static_cast<pointer>(::operator new(sizeof(value_type) * size));
            pointer dummy;
            size_t index;
            for (index = 0; index < size; index++) {
                dummy = new(&(data[index])) value_type(*otherPointer); // placement new with copy constructor
            }
            return data;
        }
        return 0;
    }


    /*! Specialization of DeleteArray.  Call the destructor for each object
      and then delete the container */
    inline static bool DeleteArray(generic_pointer & data, size_t & size) {
        pointer typedData = dynamic_cast<pointer>(data);
        if (typedData) {
            size_t index;
            for (index = 0; index < size; index++) {
                typedData[index].~value_type();
            }
            delete typedData;
            data = 0;
            size = 0;
            return true;
        }
        return false;
    }


    /*! Specialization of delete when dynamic creation is
      enabled.  Call destructor for the given class. */
    inline static bool Delete(cmnGenericObject * existing) {
        value_type * existingPointer = dynamic_cast<value_type *>(existing);
        if (existingPointer) {
            existingPointer->~value_type();
            return true;
        } else {
            return false;
        }
    }
};



/*!
  \brief Class services

  \sa cmnClassRegister cmnClassServicesBase
 */
template <bool _hasDynamicCreation, class _class>
class cmnClassServices: public cmnClassServicesBase {
 public:
    /*! Type of the base class, i.e. cmnClassServicesBase. */
    typedef cmnClassServicesBase BaseType;

    /* documented in base class */
    typedef BaseType::LogLoDType LogLoDType;

    /* documented in base class */
    typedef cmnGenericObject * generic_pointer;

    /*!  Constructor. Sets the name of the class and the Level of Detail
      setting for the class.

      \param className The name to be associated with the class.
      \param typeInfo Type information as defined by typeid() (see
      C++ RTTI)
      \param lod The Log Level of Detail setting to be used with this class.
    */
    cmnClassServices(const std::string & className, const std::type_info * typeInfo, LogLoDType lod = CMN_LOG_LOD_RUN_ERROR):
        BaseType(className, typeInfo, lod)
    {}

    /* documented in base class */
    virtual cmnGenericObject * Create(void) const {
        typedef cmnConditionalObjectFactory<_hasDynamicCreation, _class> FactoryType;
        return FactoryType::Create();
    }

    /* documented in base class */
    virtual cmnGenericObject * Create(const cmnGenericObject & other) const {
        typedef cmnConditionalObjectFactory<_hasDynamicCreation, _class> FactoryType;
        return FactoryType::Create(other);
    }

    /* documented in base class */
    virtual bool Create(cmnGenericObject * existing, const cmnGenericObject & other) const {
        typedef cmnConditionalObjectFactory<_hasDynamicCreation, _class> FactoryType;
        return FactoryType::Create(existing, other);
    }

    /* documented in base class */
    virtual cmnGenericObject * CreateArray(size_t size) const {
        typedef cmnConditionalObjectFactory<_hasDynamicCreation, _class> FactoryType;
        return FactoryType::CreateArray(size);
    }

    /* documented in base class */
    virtual cmnGenericObject * CreateArray(size_t size, const cmnGenericObject & other) const {
        typedef cmnConditionalObjectFactory<_hasDynamicCreation, _class> FactoryType;
        return FactoryType::CreateArray(size, other);
    }

    /* documented in base class */
    virtual bool DeleteArray(generic_pointer & data, size_t & size) const {
        typedef cmnConditionalObjectFactory<_hasDynamicCreation, _class> FactoryType;
        return FactoryType::DeleteArray(data, size);
    }

    /* documented in base class */
    virtual bool Delete(cmnGenericObject * existing) const {
        typedef cmnConditionalObjectFactory<_hasDynamicCreation, _class> FactoryType;
        return FactoryType::Delete(existing);
    }

};


#endif // _cmnClassServices_h

