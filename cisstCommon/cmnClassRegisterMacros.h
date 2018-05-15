/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2006-10-30

  (C) Copyright 2006-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#pragma once

#ifndef _cmnClassRegisterMacros_h
#define _cmnClassRegisterMacros_h

#include <cisstCommon/cmnTypeTraits.h>

/*!
  \file
  \brief Class registration macros.


  These macros can be divided in three sets, one of each should be
  used for any registered class.

  - #CMN_DECLARE_SERVICES and #CMN_DECLARE_SERVICES_EXPORT are used in
    the header file to add the declaration of methods and data members
    within the scope of the registered class.

  - #CMN_DECLARE_SERVICES_INSTANTIATION and
    #CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT are used in the header
    file to declare a template specialization of the global function
    cmnClassServicesInstantiate for the registered class.

  - #CMN_IMPLEMENT_SERVICES and #CMN_IMPLEMENT_SERVICES_TEMPLATED are
     used in the source file to implement the methods declared by
     #CMN_DECLARE_SERVICES and the function specialization declared by
     #CMN_DECLARE_SERVICES_INSTANTIATION.

  The macros ending with _EXPORT are required to handle the generation
  of DLLs correctly with the different Microsoft compilers (see also
  #CISST_EXPORT) and the macro ending with _TEMPLATED is required to
  register a specialized version of a templated class.

  Finally, the definitions #CMN_DYNAMIC_CREATION and
  #CMN_NO_DYNAMIC_CREATION are provided to increase the code
  readability.
*/



/*!
  \name Methods declaration

  These macros are used to declare the required methods and data
  members for the class registration (see also cmnClassRegister).
  Every class which needs to be registered should include one of these
  macros within its declaration AND be derived from cmnGenericObject.

  To declare a registered class without dynamic creation, the header
  file myClass.h should have:

  \code
    class CISST_EXPORT myClass: public cmnGenericObject
    {
        CMN_DECLARE_SERVICES(#CMN_NO_DYNAMIC_CREATION, CMN_LOG_DEFAULT_LOD);
    public:
        ...
    };
  \endcode

  The macro #CMN_DECLARE_SERVICES defines amongst other things a
  static method which can not be inlined.  Therefore, when one wants
  to create a Dll (shared libraries on Windows) with a C++ compiler
  from Microsoft, one must explicitly export it.  In most cases, the
  class itself is "exported" using #CISST_EXPORT.  If the class is not
  wholly exported, it is possible to export some methods explicitly.
  In this case, the macro #CMN_DECLARE_SERVICES_EXPORT should be used:

  \code
    class myClass: public cmnGenericObject
    {
        CMN_DECLARE_SERVICES_EXPORT(CMN_NO_DYNAMIC_CREATION, CMN_LOG_DEFAULT_LOD);
    public:
        CISST_EXPORT void MethodA(void);
        ...
    };
  \endcode

  Finally, the macro #CMN_DECLARE_SERVICES_EXPORT_ALWAYS was introduced as a temporary
  fix for template classes, such as mtsGenericObjectProxy.

  \param hasDynamicCreation Set this parameter to
  #CMN_DYNAMIC_CREATION to enable dynamic creation and
  #CMN_NO_DYNAMIC_CREATION to disable dynamic creation.  Dynamic
  creation requires a public default constructor.

  \param lod The default Level of Detail used for this class (see also
  #CMN_LOG and #CMN_LOG_DEFAULT_LOD.
 */
//@{
#ifdef CMN_DECLARE_SERVICES
#undef CMN_DECLARE_SERVICES
#endif
#define CMN_DECLARE_SERVICES(hasDynamicCreation, lod) \
    public: \
      enum {HAS_DYNAMIC_CREATION = hasDynamicCreation}; \
      enum {InitialLoD = lod}; \
      static cmnClassServicesBase * ClassServices(void); \
      virtual const cmnClassServicesBase * Services(void) const; \
    private: \
      static cmnClassServicesBase * ClassServicesPointer;



#ifdef CMN_DECLARE_SERVICES_EXPORT
#undef CMN_DECLARE_SERVICES_EXPORT
#endif
#define CMN_DECLARE_SERVICES_EXPORT(hasDynamicCreation, lod) \
    public: \
      enum {HAS_DYNAMIC_CREATION = hasDynamicCreation}; \
      enum {InitialLoD = lod}; \
      CISST_EXPORT static cmnClassServicesBase * ClassServices(void); \
      virtual CISST_EXPORT const cmnClassServicesBase * Services(void) const; \
    private: \
      static cmnClassServicesBase * ClassServicesPointer;

#ifdef CMN_DECLARE_SERVICES_EXPORT_ALWAYS
#undef CMN_DECLARE_SERVICES_EXPORT_ALWAYS
#endif
#if WIN32 && _MSC_VER && CISST_DLL
#define CMN_DECLARE_SERVICES_EXPORT_ALWAYS(hasDynamicCreation, lod) \
    public: \
      enum {HAS_DYNAMIC_CREATION = hasDynamicCreation}; \
      enum {InitialLoD = lod}; \
      _declspec(dllexport) static cmnClassServicesBase * ClassServices(void); \
      virtual _declspec(dllexport) const cmnClassServicesBase * Services(void) const; \
    private: \
      static cmnClassServicesBase * ClassServicesPointer;
#else
#define CMN_DECLARE_SERVICES_EXPORT_ALWAYS(hasDynamicCreation, lod) CMN_DECLARE_SERVICES_EXPORT(hasDynamicCreation, lod)
#endif

//@}


/*!
  \name Global function declaration

  The templated function cmnClassServicesInstantiate is used to
  generate a unique object of type cmnClassServices for each class
  defined in cisst.  It contains a static data member which will be
  constructed the first time it is called.  The header file myClass.h
  should have to following code after the class declaration of
  myClass:

  \code
    CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(myClass)
  \endcode

  In most case, #CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT should be
  used.  One case where #CMN_DECLARE_SERVICES_INSTANTIATION should be
  used in when the class instantiation will reside in the executable,
  i.e. the programmer is not creating a library but is linking all is
  binaries directly into and executable.

  For a template class, it is necessary to specify which template
  specialization is being registered.  To do so, it is recommended to
  create a <code>typedef</code> before using this macro:
  \code
    template <class _templateParameter>
    myClass<_templateParameter>: public cmnGenericObject
    {
        CMN_DECLARE_SERVICES_EXPORT(CMN_NO_DYNAMIC_CREATION, CMN_LOG_DEFAULT_LOD);
    public:
        ...
    };

    typedef myClass<double> myClassDouble;
    CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(myClassDouble)
  \endcode

  \param className The name of the class being registered, without any
  quote.
*/
//@{

#ifdef CMN_DECLARE_SERVICES_INSTANTIATION
#undef CMN_DECLARE_SERVICES_INSTANTIATION
#endif
#define CMN_DECLARE_SERVICES_INSTANTIATION(className) \
template<> \
cmnClassServicesBase * cmnClassServicesInstantiate<className>(void);

#ifdef CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT
#undef CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT
#endif
#define CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(className) \
template<> CISST_EXPORT \
cmnClassServicesBase * cmnClassServicesInstantiate<className>(void);
//@}



/*!
  \name Methods and function implementation

  These macros implements the methods and function declared by
  #CMN_DECLARE_SERVICES and
  #CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT.  For a non templated
  class, the source file myClass.cpp must have:

  \code
    CMN_IMPLEMENT_SERVICES(myClass);
  \endcode

  For a templated class specialized and define using <code>typedef
  ... myClassDouble</code>, the source file must have:

  \code
    CMN_IMPLEMENT_SERVICES_TEMPLATED(myClassDouble);
  \endcode

  \param className The name of the class being registered, without any
  quote.
*/

#ifndef LIBRARY_NAME_FOR_CISST_REGISTER
#define LIBRARY_NAME_FOR_CISST_REGISTER ""
#endif

//@{
#ifdef CMN_IMPLEMENT_SERVICES
#undef CMN_IMPLEMENT_SERVICES
#endif
#define CMN_IMPLEMENT_SERVICES_INTERNAL(className, parentServices, argType) \
cmnClassServicesBase * className::ClassServices(void) \
{ \
    static cmnClassServicesBase * classServices = cmnClassServicesInstantiate<className>(); \
    return classServices; \
} \
cmnClassServicesBase * className::ClassServicesPointer = className::ClassServices(); \
const cmnClassServicesBase * className::Services(void) const    \
{ \
    return this->ClassServices(); \
} \
template<> \
cmnClassServicesBase * cmnClassServicesInstantiate<className>(void) \
{ \
    static cmnClassServices<className::HAS_DYNAMIC_CREATION, className, argType> \
        classServices(#className, &typeid(className), parentServices, LIBRARY_NAME_FOR_CISST_REGISTER, className::InitialLoD); \
    return static_cast<cmnClassServicesBase *>(&classServices); \
} \
static cmnClassServicesBase * className##ClassServicesPointer = className::ClassServices();

#define CMN_IMPLEMENT_SERVICES(className) \
    CMN_IMPLEMENT_SERVICES_INTERNAL(className, 0, className)

#define CMN_IMPLEMENT_SERVICES_DERIVED(className, parentName) \
            CMN_IS_DERIVED_FROM_ASSERT(className, parentName) \
            CMN_IMPLEMENT_SERVICES_INTERNAL(className, parentName::ClassServices(), className)

#ifdef CMN_IMPLEMENT_SERVICES_TEMPLATED
#undef CMN_IMPLEMENT_SERVICES_TEMPLATED
#endif
#define CMN_IMPLEMENT_SERVICES_TEMPLATED_INTERNAL(className, parentServices, argType) \
template<> \
cmnClassServicesBase * cmnClassServicesInstantiate<className>(void) \
{ \
    static cmnClassServices<className::HAS_DYNAMIC_CREATION, className, argType> \
        classServices(#className, &typeid(className), parentServices, LIBRARY_NAME_FOR_CISST_REGISTER, className::InitialLoD); \
    return static_cast<cmnClassServicesBase *>(&classServices); \
} \
template<> \
cmnClassServicesBase * className::ClassServices(void) \
{ \
    static cmnClassServicesBase * classServices = cmnClassServicesInstantiate<className>(); \
    return classServices; \
} \
template<> \
cmnClassServicesBase * className::ClassServicesPointer = className::ClassServices(); \
template<> \
const cmnClassServicesBase * className::Services(void) const \
{ \
   return this->ClassServices(); \
} \
static cmnClassServicesBase * className##ClassServicesPointer = className::ClassServices();

#define CMN_IMPLEMENT_SERVICES_TEMPLATED(className) \
        CMN_IMPLEMENT_SERVICES_TEMPLATED_INTERNAL(className, 0, className)

#define CMN_IMPLEMENT_SERVICES_DERIVED_TEMPLATED(className, parentName) \
            CMN_IS_DERIVED_FROM_ASSERT(className, parentName) \
            CMN_IMPLEMENT_SERVICES_TEMPLATED_INTERNAL(className, parentName::ClassServices(), className)

//@}


/*! \name Possible values used in combination with
  #CMN_DECLARE_SERVICES and #CMN_DECLARE_SERVICES_EXPORT.
 */
//@{
/* This parameter allows to either register a class without dynamic
  creation (just for logging, usually a "processing" class) or
  register the class with dynamic creation (the log is still
  available, mostly for "data" objects).  Dynamic creation can be used
  to create an object of a given type based on its name provided as a
  string.

  It is important to note that a class registered to allow dynamic
  creation must provide a default constructor.  The C++ compiler will
  not be able to compile the code generated if the default constructor
  is missing.
*/

// Possible options for dynamic creation
const int CMN_NO_DYNAMIC_CREATION = 0;       // no dynamic creation
const int CMN_DYNAMIC_CREATION_DEFAULT = 1;  // dynamic creation with default constructor
const int CMN_DYNAMIC_CREATION_COPY = 2;     // dynamic creation with copy constructor
const int CMN_DYNAMIC_CREATION = 3;          // dynamic creation with default and/or copy constructor (backward compatibility)
const int CMN_DYNAMIC_CREATION_SETNAME = 5;  // dynamic creation with default constructor and SetName method
const int CMN_DYNAMIC_CREATION_ONEARG = 8;   // dynamic creation with one argument constructor (but not copy constructor)

//@}

#endif // _cmnClassRegisterMacros_h

