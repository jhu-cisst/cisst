/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2004-08-18

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
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

#ifndef _cmnClassServices_h
#define _cmnClassServices_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnClassServicesBase.h>
#include <cisstCommon/cmnClassRegisterMacros.h>


/*! This class is a helper for cmnClassServices.  Its goal is to
  specialize the method Create() based on the _hasDynamicCreation
  parameter to either create a new object of type _class (which
  requires a public default constructor) or do nothing and return 0
  (null pointer).  The later means that dynamic creation is disabled.
*/
template <bool _hasDynamicCreation>
class cmnConditionalObjectFactory; 


/*! Specialization of cmnConditionalObjectFactory with disabled
  dynamic creation.
*/
template<>
class cmnConditionalObjectFactory<CMN_NO_DYNAMIC_CREATION>
{
public:
    template <class _class>
    class ClassSpecialization {
    public:
        /*! Specialization of create when dynamic creation is
          disabled.  Returns 0 (null pointer). */
        inline static cmnGenericObject * Create(void) {
            /* may be throw an exception instead */
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
        };

        /*! Specialization of delete */
        inline static bool Delete(cmnGenericObject * CMN_UNUSED(existing)) {
            return false;
        };

    };
};


/*! Specialization of cmnConditionalObjectFactory with enabled dynamic
  creation.
*/
template<>
class cmnConditionalObjectFactory<CMN_DYNAMIC_CREATION>
{
public:
    template <class _class>
    class ClassSpecialization {
        typedef _class value_type;
    public:
        /*! Specialization of create when dynamic creation is
          enabled.  Call new for the given class.  This methods
          requires a default constructor for the aforementioned
          class. */
        inline static cmnGenericObject * Create(void) {
            return new value_type;
        }

        /*! Specialization of create(other) when dynamic creation is
          enabled.  Call new for the given class.  This methods
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
          enabled.  Call new for the given class.  This methods
          requires a copy constructor for the aforementioned class. */
        inline static bool Create(cmnGenericObject * existing, const cmnGenericObject & other) {
            const value_type * otherPointer = dynamic_cast<const value_type *>(&other);
            if (otherPointer) {
                new(existing) value_type(*otherPointer);
                return true;
            } else {
                return false;
            }
        }

        /*! Specialization of delete when dynamic creation is
          enabled.  Call new for the given class. */
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
        typedef cmnConditionalObjectFactory<_hasDynamicCreation> FactoryType; 
        typedef typename FactoryType::template ClassSpecialization<_class> CreatorType;
        return CreatorType::Create();
    }

    /* documented in base class */
    virtual cmnGenericObject * Create(const cmnGenericObject & other) const {
        typedef cmnConditionalObjectFactory<_hasDynamicCreation> FactoryType; 
        typedef typename FactoryType::template ClassSpecialization<_class> CreatorType;
        return CreatorType::Create(other);
    }

    /* documented in base class */
    virtual bool Create(cmnGenericObject * existing, const cmnGenericObject & other) const {
        typedef cmnConditionalObjectFactory<_hasDynamicCreation> FactoryType; 
        typedef typename FactoryType::template ClassSpecialization<_class> CreatorType;
        return CreatorType::Create(existing, other);
    }

    /* documented in base class */
    virtual bool Delete(cmnGenericObject * existing) const {
        typedef cmnConditionalObjectFactory<_hasDynamicCreation> FactoryType; 
        typedef typename FactoryType::template ClassSpecialization<_class> CreatorType;
        return CreatorType::Delete(existing);
    }

};


#endif // _cmnClassServices_h

