/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2004-08-18

  (C) Copyright 2004-2019 Johns Hopkins University (JHU), All Rights Reserved.

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
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnClassRegisterMacros.h>

/*! These classes are helpers for cmnClassServices.  Their goal is to
  specialize the various Create methods based on the first template parameter
  to either create a new object of type _class or do nothing and return 0 (null pointer).
  The latter means that dynamic creation is disabled.
*/

//************************ Factory with Default Constructor ***********************

/*! Default implementation of cmnConditionalObjectFactoryDefault with disabled
    dynamic creation.
*/
template<bool _enabled, typename _class>
class cmnConditionalObjectFactoryDefault
{
public:
     /*! Specialization of create when dynamic creation is
      disabled.  Returns 0 (null pointer). */
    inline static cmnGenericObject * Create(void) {
        return 0;
    }

    /*! Specialization of create when dynamic creation is
       disabled.  Returns 0 (null pointer). */
    inline static cmnGenericObject * CreateArray(size_t CMN_UNUSED(size)) {
        return 0;
    }

    inline static bool DefaultConstructorAvailable(void) { return false; }
};


/*! Specialization of cmnConditionalObjectFactoryDefault with enabled dynamic
  creation.  Requires default constructor.
*/
template <typename _class>
class cmnConditionalObjectFactoryDefault<true, _class>
{
    typedef _class value_type;
public:

    /*! Specialization of create when dynamic creation is
      enabled.  Call new for the given class.  This method
      requires a default constructor for the aforementioned
      class. */
    inline static cmnGenericObject * Create(void) {
        return new value_type;
    }

    /*! Specialization of create when dynamic creation is
      enabled.  Call new[size] for the given class.  This method
      requires a default constructor for the aforementioned
      class. */
    inline static cmnGenericObject * CreateArray(size_t size) {
        return new value_type[size];
    }

    inline static bool DefaultConstructorAvailable(void) { return true; }
};

//************************ Factory with Copy Constructor ***********************

/*! Default implementation of cmnConditionalObjectFactoryCopy with disabled
  dynamic creation.
*/
template<bool _enabled, typename _class>
class cmnConditionalObjectFactoryCopy
{
public:

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
    inline static cmnGenericObject * CreateArray(size_t CMN_UNUSED(size), const cmnGenericObject & CMN_UNUSED(other)) {
        return 0;
    }

    inline static bool CopyConstructorAvailable(void) { return false; }
};


/*! Specialization of cmnConditionalObjectFactoryCopy with enabled dynamic
  creation. Requires copy constructor.
*/
template <typename _class>
class cmnConditionalObjectFactoryCopy<true, _class>
{
public:
    typedef _class value_type;
    typedef _class * pointer;

    /*! Specialization of create(other) when dynamic creation is
      enabled.  Call new for the given class.  This method
      requires a copy constructor for the aforementioned class. */
    inline static cmnGenericObject * Create(const cmnGenericObject & other) {
        const value_type * otherPointer = dynamic_cast<const value_type *>(&other);
        if (otherPointer)
            return new value_type(*otherPointer);
        else
            return 0;
    }

    /*! Specialization of create(other) when dynamic creation is
      enabled.  Call new for the given class.  This method
      requires a copy constructor for the aforementioned class.
      Returns false on failure, in which case input parameter (existing) is
      not modified.
     */
    inline static bool Create(cmnGenericObject * existing, const cmnGenericObject & other) {
        // If they already point to the same memory, just return
        if (existing == &other)
            return true;
        if (existing->Services() != other.Services()) {
            CMN_LOG_RUN_WARNING << "cmnClassServices::Create with in-place new called for different classes, existing = "
                                << existing->Services()->GetName()
                                << ", other = " << other.Services()->GetName() << std::endl;
            return false;
        }
        const value_type * otherPointer = dynamic_cast<const value_type *>(&other);
        if (otherPointer) {
            existing->Services()->Delete(existing);
            new(existing) value_type(*otherPointer);
            return true;
        } else {
            return false;
        }
    }

    /*! Specialization of create when dynamic creation is
      enabled.  Call the global operator new to allocate a block of memory
      and then use the placement new for each object. */
    inline static cmnGenericObject * CreateArray(size_t size, const cmnGenericObject & other) {
        const value_type * otherPointer = dynamic_cast<const value_type *>(&other);
        if (otherPointer) {
            pointer data = static_cast<pointer>(::operator new(sizeof(value_type) * size));
            size_t index;
            for (index = 0; index < size; index++) {
                new(&(data[index])) value_type(*otherPointer); // placement new with copy constructor
            }
            return data;
        }
        return 0;
    }

    inline static bool CopyConstructorAvailable(void) { return true; }
};

//************************ Factory with One Arg Constructor ***********************

/*! Default implementation of cmnConditionalObjectFactoryOneArg with disabled
  dynamic creation.
*/
template<int _dynamicCreation, typename _class, typename _argType>
class cmnConditionalObjectFactoryOneArg
{
public:

    /*! Specialization of create when dynamic create is disabled.
        Returns 0 (null pointer) .*/
    inline static cmnGenericObject * Create(const cmnGenericObject & CMN_UNUSED(arg)) {
         return 0;
    }

    inline static bool OneArgConstructorAvailable(void) { return false; }

    inline static const cmnClassServicesBase *GetConstructorArgServices(void) {
        return 0;
    }
};

/*! Specialization of cmnConditionalObjectFactoryOneArg with enabled
  dynamic creation.  Here, _argType must be derived from cmnGenericObject.
  Note that the cisstMultiTask library removes this requirement by using
  automatic wrapping with mtsGenericObjectProxy.
*/
template<typename _class, typename _argType>
class cmnConditionalObjectFactoryOneArg<CMN_DYNAMIC_CREATION_ONEARG, _class, _argType>
{
public:
    typedef _class value_type;

    /*! Specialization of create when dynamic create is enabled. */
    inline static _class * Create(const cmnGenericObject & arg) {
        const _argType *argTyped = dynamic_cast<const _argType *>(&arg);
        if (argTyped)
            return new value_type(*argTyped);
        else {
            CMN_LOG_INIT_WARNING << "cmnConditionalObjectFactoryOneArg: failed to dynamic cast from "
                                 << arg.Services()->GetName() << " to "
                                 << _argType::ClassServices()->GetName() << std::endl;
            return 0;
        }
    }

    inline static bool OneArgConstructorAvailable(void) { return true; }

    inline static const cmnClassServicesBase *GetConstructorArgServices(void) {
        return _argType::ClassServices();
    }
};

//************************ Destructors ***********************

/*! Default implementation with dynamic creation enabled */
template<int _hasDynamicCreation, typename _class>
class cmnConditionalObjectDestructor
{
    typedef _class value_type;
    typedef _class * pointer;
    typedef cmnGenericObject * generic_pointer;
public:
    /*! Default implementation of DeleteArray.  Call the destructor for each object
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


    /*! Default implementation of delete when dynamic creation is
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

/*! Specialization when dynamic creation is disabled */
template<typename _class>
class cmnConditionalObjectDestructor<CMN_NO_DYNAMIC_CREATION, _class>
{
    typedef cmnGenericObject * generic_pointer;
public:
    inline static bool DeleteArray(generic_pointer & CMN_UNUSED(data), size_t & CMN_UNUSED(size)) {
        return false;
    }

    inline static bool Delete(cmnGenericObject * CMN_UNUSED(existing)) {
        return false;
    }
};


//************************************ Class Services ****************************
/*!
  \brief Class services

  If dynamic creation is enabled, class must have public default constructor and
  copy constructor.

  \sa cmnClassRegister cmnClassServicesBase
 */
template <int _dynamicCreation, class _class, class _argType = _class>
class cmnClassServices: public cmnClassServicesBase {

    typedef cmnConditionalObjectFactoryDefault<
		((_dynamicCreation&CMN_DYNAMIC_CREATION_DEFAULT) == CMN_DYNAMIC_CREATION_DEFAULT), _class> FactoryDefaultType;
    typedef cmnConditionalObjectFactoryCopy<
		((_dynamicCreation&CMN_DYNAMIC_CREATION_COPY) == CMN_DYNAMIC_CREATION_COPY), _class> FactoryCopyType;
    typedef cmnConditionalObjectFactoryOneArg<_dynamicCreation, _class, _argType> FactoryOneArgType;
    typedef cmnConditionalObjectDestructor<_dynamicCreation, _class> DestructorType;

 public:

    /*! Type of the base class, i.e. cmnClassServicesBase. */
    typedef cmnClassServicesBase BaseType;

    /* documented in base class */
    typedef cmnGenericObject * generic_pointer;

    /*!  Constructor. Sets the name of the class and the Level of Detail
      setting for the class.

      \param className The name to be associated with the class.
      \param typeInfo Type information as defined by typeid() (see
      C++ RTTI)
      \param mask The log mask to be used with this class.
    */
    cmnClassServices(const std::string & className, const std::type_info * typeInfo,
                     const cmnClassServicesBase * parentServices,
                     const std::string & libraryName,
                     cmnLogMask mask = CMN_LOG_ALLOW_DEFAULT):
        BaseType(className, typeInfo, parentServices, libraryName, mask)
    {}

    /* documented in base class */
    virtual cmnGenericObject * Create(void) const {
        return FactoryDefaultType::Create();
    }

    /* documented in base class */
    virtual cmnGenericObject * Create(const cmnGenericObject & other) const {
        return FactoryCopyType::Create(other);
    }

    /* documented in base class */
    virtual cmnGenericObject * CreateWithArg(const cmnGenericObject & arg) const {
        return FactoryOneArgType::Create(arg);
    }

    /* documented in base class */
    virtual bool Create(cmnGenericObject * existing, const cmnGenericObject & other) const {
        return FactoryCopyType::Create(existing, other);
    }

    /* documented in base class */
    virtual cmnGenericObject * CreateArray(size_t size) const {
        return FactoryDefaultType::CreateArray(size);
    }

    /* documented in base class */
    virtual cmnGenericObject * CreateArray(size_t size, const cmnGenericObject & other) const {
        return FactoryCopyType::CreateArray(size, other);
    }

    /* documented in base class */
    virtual bool DeleteArray(generic_pointer & data, size_t & size) const {
        return DestructorType::DeleteArray(data, size);
    }

    /* documented in base class */
    virtual bool Delete(cmnGenericObject * existing) const {
        return DestructorType::Delete(existing);
    }

    /* documented in base class */
    virtual size_t GetSize(void) const {
        return sizeof(_class);
    }

    /* documented in base class */
    bool HasDynamicCreation(void) const { return (_dynamicCreation != CMN_NO_DYNAMIC_CREATION); }
    bool DefaultConstructorAvailable(void) const { return FactoryDefaultType::DefaultConstructorAvailable(); }
    bool CopyConstructorAvailable(void) const { return FactoryCopyType::CopyConstructorAvailable(); }
    bool OneArgConstructorAvailable(void) const { return FactoryOneArgType::OneArgConstructorAvailable(); }

    const cmnClassServicesBase *GetConstructorArgServices(void) const {
        return FactoryOneArgType::GetConstructorArgServices();
    }
};

#endif // _cmnClassServices_h

