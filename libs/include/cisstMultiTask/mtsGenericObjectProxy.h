/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):	Ankur Kapoor, Anton Deguet, Peter Kazanzides
  Created on:	2006-05-05

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsGenericObjectProxy_h
#define _mtsGenericObjectProxy_h

#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>
#include <cisstCommon/cmnTypeTraits.h>

#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsGenericObject.h>

// Always include last!
#include <cisstMultiTask/mtsExport.h>


// Forward declarations
template <class _elementType> class mtsGenericObjectProxy;
template <class _elementType> class mtsGenericObjectProxyRef;

/*!  Proxy class used to create a simple mtsGenericObject, i.e. data
  object with a registered type, dynamic creation, serialization and
  de-serialization.  Note that there are really three classes defined
  here:  mtsGenericObjectProxyBase, mtsGenericObjectProxy, and
  mtsGenericObjectProxyRef (henceforth called ProxyBase, Proxy, and
  ProxyRef for brevity).  ProxyBase provides a common base class.
  Proxy contains one data member of the actual type provided as the
  template parameter.  ProxyRef contains a pointer to a data member
  of the actual type provided as the template parameter.

  ProxyRef is used internally by the library to enable arbitrary data types T
  (not just ones derived from mtsGenericObject) to be added to the State Table.
  In this case, the StateVector (State Table history) will be an array of
  Proxy<T> and the StateVectorElements (pointers to working copy) will be of
  type ProxyRef<T>.

  To create an mtsGenericObject from an existing type, one
  would have to specialize mtsGenericObjectProxy as follows in the
  header file (it is not necessary to worry about the ProxyBase or ProxyRef --
  these are handled automatically):
  \code
  class UserType {
      ....
  };

  typedef mtsGenericObjectProxy<UserType> UserTypeProxy;
  CMN_DECLARE_SERVICES_INSTANTIATION(UserTypeProxy);
  \endcode

  It is also required to instantiate the associated services in the
  compiled code (.cpp) file with:
  \code
  CMN_IMPLEMENT_SERVICES_TEMPLATED(UserTypeProxy);
  \endcode

  \note It is really important to note that this simple way of
  creating mtsGenericObject derived classes works as is for classes
  which can be serialized as a memory block, i.e. native types,
  compact structs, ...  For classes which require a more complex way
  to serialize and deserialize, the user will also have to overload
  the functions cmnSerializeRaw and cmnDeSerializeRaw.

  \note As it is, this class cannot be used in libraries other than
  cisstCommon if a DLL is to be created.   The issue is that the class
  will be declared as imported in the header file because it is part of
  cisstCommon.  (PK: OBSOLETE COMMENT?)

  \note There are some "dirty tricks" in this class now, so that we can
  have a Proxy class and a ProxyRef class, and enable assignments between
  them. This provides a bit of polymorphic behavior for the command objects,
  because the Execute method can be parameterized as a ProxyBase object,
  and thus can be called with either a Proxy or ProxyRef object.  This
  is used for "read from state" commands.
  One particularly dirty trick is that the class services are actually
  defined in the Proxy class, but are "stolen" by the ProxyRef class,
  via the ProxyBase class.

  \sa mtsGenericObject, cmnClassServices, cmnSerializer, cmnDeSerializer

  \param _elementType The actual type for which we want to provide a
  proxy object.
*/
template <class _elementType>
class mtsGenericObjectProxyBase: public mtsGenericObject
{
public:
    typedef mtsGenericObjectProxyBase<_elementType> ThisType;
    typedef mtsGenericObject BaseType;
    typedef mtsGenericObjectProxy<_elementType> DeRefType;
    typedef mtsGenericObjectProxyRef<_elementType> RefType;

    typedef _elementType value_type;

    // CMN_DECLARE_SERVICES_EXPORT(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    // Dirty Trick: this class uses the class services of the dereferenced (derived) type
    //enum {HAS_DYNAMIC_CREATION = DeRefType::HAS_DYNAMIC_CREATION};
    //enum {InitialLoD = DeRefType::InitialLoD};
    enum {HAS_DYNAMIC_CREATION = true};
    enum {InitialLoD = CMN_LOG_LOD_RUN_ERROR};
    static cmnClassServicesBase * ClassServices(void) { return DeRefType::ClassServices(); }
    virtual const cmnClassServicesBase * Services(void) const
    {
        return DeRefType::ClassServices();
    }

    /*! Default constructor.  The data member is initialized using its
        default constructor. */
    inline mtsGenericObjectProxyBase(void)
    {}

    /*! Copy constructor. */
    inline mtsGenericObjectProxyBase(const ThisType & other) :
        BaseType(other)
    {}

    inline ~mtsGenericObjectProxyBase(void) {}

    /*! Assignment from DeRef and Ref classes. */
    virtual void Assign(const DeRefType &)
    { CMN_LOG_INIT_WARNING << "ProxyBase Assign DeRefType called for " << this->Services()->GetName() << std::endl; }

#if 0
    /*! Conversion assignment.  This allows to assign from a dereferenced object. */
    inline ThisType & operator=(const DeRefType &data) {
        this->Assign(data);
        return *this;
    }
#endif

    virtual void Assign(const RefType &)
    { CMN_LOG_INIT_WARNING << "ProxyBase Assign RefType called for " << this->Services()->GetName() << std::endl; }

#if 0
    /*! Conversion assignment.  This allows to assign from a reference object. */
    inline ThisType & operator=(const RefType &data) {
        this->Assign(data);
        return *this;
    }
#endif
};


template <class _elementType>
class mtsGenericObjectProxy: public mtsGenericObjectProxyBase<_elementType>
{
    CMN_DECLARE_SERVICES_EXPORT(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    typedef mtsGenericObjectProxy<_elementType> ThisType;
    typedef mtsGenericObjectProxyBase<_elementType> BaseType;
    typedef typename mtsGenericObjectProxyBase<_elementType>::value_type value_type;
    typedef typename mtsGenericObjectProxyBase<_elementType>::DeRefType DeRefType;
    typedef typename mtsGenericObjectProxyBase<_elementType>::RefType RefType;
    value_type Data;

    /*! Default constructor.  The data member is initialized using its
        default constructor. */
    inline mtsGenericObjectProxy(void)
    {}

    /*! Copy constructor. */
    inline mtsGenericObjectProxy(const ThisType & other) :
        BaseType(other), Data(other.Data)
    {}

    /*! Conversion constructor.  This allows to construct the proxy
      object using an object of the actual type.  This method will
      set the Valid flag to true. */
    inline mtsGenericObjectProxy(const value_type & data):
        Data(data)
    {
        this->SetValid(true);
    }

    inline ~mtsGenericObjectProxy(void) {}

    /*! Assignment from same type (DeRefType and ThisType are the same). */
    void Assign(const DeRefType & other) {
        *this = other;
    }

    /*! Conversion assignment.  This allows to assign from a proxy ref object. */
    void Assign(const RefType & other)
    {
        this->Data = *other.pData;
        this->SetValid(other.Valid());
        this->SetTimestamp(other.Timestamp());
    }

    /*! Conversion assignment.  This allows to assign from a proxy ref object. */
    inline ThisType & operator=(const RefType &data) {
        this->Assign(data);
        return *this;
    }

    /*! Conversion assignment.  This allows to assign from an object
      of the actual type without explicitly referencing the public
      data member "Data". */
    inline ThisType & operator=(value_type data) {
        this->Data = data;
        this->SetValid(true);
        return *this;
    }

    /*! Cast operator.  This allows to assign to an object of the
      actual type without explicitly referencing the public data
      member "Data". */
    //@{
    inline operator value_type & (void) {
        return this->Data;
    }
    inline operator const value_type & (void) const {
        return this->Data;
    }
    //@}

    /*! Serialization.  Relies on the specialization, if any, of cmnSerializeRaw. */
    inline void SerializeRaw(std::ostream & outputStream) const {
        cmnSerializeRaw(outputStream, this->Data);
    }

    /*! DeSerialization.  Relies on the specialization, if any, of cmnDeSerializeRaw. */
    inline void DeSerializeRaw(std::istream & inputStream) {
        cmnDeSerializeRaw(inputStream, this->Data);
    }

    /*! To stream method.  Uses the default << operator as defined for
        the actual type. */
    inline virtual void ToStream(std::ostream & outputStream) const {
        BaseType::ToStream(outputStream);
        outputStream << " Value: " << this->Data;
    }

    /*! To stream raw data. */
    inline virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                                    bool headerOnly = false, const std::string & headerPrefix = "") const {
        if (headerOnly) {
            BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
            outputStream << delimiter << headerPrefix << "-data";
        } else {
            BaseType::ToStreamRaw(outputStream, delimiter);
            outputStream << delimiter << this->Data;
        }
    }
};


template <class _elementType>
class mtsGenericObjectProxyRef: public mtsGenericObjectProxyBase<_elementType>
{
    /*! Default constructor.  Should not get called!! */
    inline mtsGenericObjectProxyRef(void) : pData(0)
    {}

public:
    typedef mtsGenericObjectProxyRef<_elementType> ThisType;
    typedef mtsGenericObjectProxyBase<_elementType> BaseType;
    typedef typename mtsGenericObjectProxyBase<_elementType>::value_type value_type;
    typedef typename mtsGenericObjectProxyBase<_elementType>::DeRefType DeRefType;
    typedef typename mtsGenericObjectProxyBase<_elementType>::RefType RefType;

    value_type *pData;   // Could instead use: value_type &Data

    /*! Copy constructor. */
    inline mtsGenericObjectProxyRef(const ThisType & other) :
        BaseType(other), pData(other.pData)
    {}

    /*! Conversion constructor.  This allows to construct the proxy
      object using an object of the actual type. */
    // PK: should figure out a way to avoid the const_cast.
    inline mtsGenericObjectProxyRef(const value_type & data):
        pData(const_cast<value_type*>(&data))
    {
        this->SetValid(true);
    }

    inline ~mtsGenericObjectProxyRef(void) {}

    /*! Assignment from same type (RefType and ThisType are the same). */
    void Assign(const RefType & other) { *this = other; }

    /*! Conversion assignment.  This allows to assign from a proxy ref object. */
    void Assign(const DeRefType & other) {
        *this->pData = other.Data;
        this->SetValid(other.Valid());
        this->SetTimestamp(other.Timestamp());
    }

    /*! Conversion assignment.  This allows to assign from a dereferenced object. */
    inline ThisType & operator=(const DeRefType &data) {
        this->Assign(data);
        return *this;
    }

    /*! Conversion assignment.  This allows to assign from an object
      of the actual type without explicitly referencing the public
      data member "Data". */
    inline ThisType & operator=(value_type data) {
        *this->pData = data;
        this->SetValid(true);
        return *this;
    }

    /*! Cast operator.  This allows to assign to an object of the
      actual type without explicitly referencing the public data
      member "Data". */
    //@{
    inline operator value_type & (void) {
        return *this->pData;
    }
    inline operator const value_type & (void) const {
        return *this->pData;
    }
    //@}

    /*! Serialization.  Relies on the specialization, if any, of cmnSerializeRaw. */
    inline void SerializeRaw(std::ostream & outputStream) const {
        cmnSerializeRaw(outputStream, *this->pData);
    }

    /*! DeSerialization.  Relies on the specialization, if any, of cmnDeSerializeRaw. */
    inline void DeSerializeRaw(std::istream & inputStream) {
        cmnDeSerializeRaw(inputStream, *this->pData);
    }

    /*! To stream method.  Uses the default << operator as defined for
        the actual type. */
    inline virtual void ToStream(std::ostream & outputStream) const {
        BaseType::ToStream(outputStream);
        outputStream << " Value(ref): " << *this->pData;
    }

    /*! To stream raw data. */
    inline virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                                    bool headerOnly = false, const std::string & headerPrefix = "") const {
        if (headerOnly) {
            BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
            outputStream << delimiter << headerPrefix << "-data(ref)";
        } else {
            BaseType::ToStreamRaw(outputStream, delimiter);
            outputStream << delimiter << *this->pData;
        }
    }
};


#ifndef SWIG
/*! Following are some template helpers to deal with the conditional wrapping and unwrapping of types. */

template<typename T, bool>
class mtsGenericTypesImpl
{
    // T is not derived from mtsGenericObject, so wrap it
public:
    typedef mtsGenericObjectProxyBase<T>  FinalBaseType;
    typedef mtsGenericObjectProxy<T>      FinalType;
    typedef mtsGenericObjectProxyRef<T>   FinalRefType;
    static FinalRefType *ConditionalWrap(T &obj) { return new FinalRefType(obj); }
    template <typename C> static int CallAfterConditionalWrap(C* cmd, T &obj)
        { FinalRefType arg(obj); return static_cast<int>(cmd->Execute(arg)); }
    static bool IsEqual(const T &obj1, const mtsGenericObject &obj2) {
        const FinalRefType *p2 = dynamic_cast<const FinalRefType *>(&obj2);
        return (p2?(&obj1 == p2->pData):false); }
    static void ConditionalFree(const FinalRefType *obj) { delete obj;}
    static void Copy(const FinalType &from, FinalBaseType &to) { to.Assign(from); }
};

template<typename T>
class mtsGenericTypesImpl<T, true>
{
    // T is derived from mtsGenericObject
public:
    typedef T FinalBaseType;
    typedef T FinalType;
    typedef T FinalRefType;
    static FinalRefType *ConditionalWrap(T &obj) { return &obj; }
    template <typename C> static int CallAfterConditionalWrap(C* cmd, T &obj)
        { return static_cast<int>(cmd->Execute(obj)); }
    static bool IsEqual(const T &obj1, const mtsGenericObject &obj2) { return &obj1 == &obj2; }
    static void ConditionalFree(const FinalRefType *) {}
    static void Copy(const FinalType &from, FinalBaseType &to) { to = from; }
};

template<typename T>
class mtsGenericTypes
{
    typedef mtsGenericTypesImpl<T, cmnIsDerivedFrom<T, mtsGenericObject>::YES> impl;
public:
    typedef typename mtsGenericTypesImpl<T, cmnIsDerivedFrom<T, mtsGenericObject>::YES>::FinalBaseType FinalBaseType;
    typedef typename mtsGenericTypesImpl<T, cmnIsDerivedFrom<T, mtsGenericObject>::YES>::FinalType     FinalType;
    typedef typename mtsGenericTypesImpl<T, cmnIsDerivedFrom<T, mtsGenericObject>::YES>::FinalRefType  FinalRefType;
    static FinalRefType *ConditionalWrap(T &obj) { return impl::ConditionalWrap(obj); }
    // PK TODO: The return type should be mtsCommandBase::ReturnType, but getting this to compile was problematic.
    template <typename C> static int CallAfterConditionalWrap(C* cmd, T &obj)
        { return impl::CallAfterConditionalWrap(cmd, obj); }
    static bool IsEqual(const T &obj1, const mtsGenericObject &obj2) { return impl::IsEqual(obj1, obj2); }
    static void ConditionalFree(const FinalRefType *obj) { impl::ConditionalFree(obj); }
    // PK TODO: Copy can probably be eliminated if the assignment operators are properly defined.
    static void Copy(const FinalType &from, FinalBaseType &to) { impl::Copy(from,to); }
};

template<typename T, bool>
class mtsGenericTypesUnwrapImpl
{
    // T is not a proxy type
public:
    typedef T RefType;
};

template<typename T>
class mtsGenericTypesUnwrapImpl<T, true>
{
    // T is a proxy ref type
public:
    typedef typename T::RefType RefType;
};

template<typename T>
class mtsGenericTypesUnwrap
{
    typedef mtsGenericTypesUnwrapImpl<T, cmnIsDerivedFromTemplated<T, mtsGenericObjectProxy >::YES> impl;
public:
    typedef typename mtsGenericTypesUnwrapImpl<T, cmnIsDerivedFromTemplated<T, mtsGenericObjectProxy >::YES>::RefType RefType;
};
#endif

/* Some basic types defined here for now, could move somewhere else. */
typedef mtsGenericObjectProxy<double> mtsDouble;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsDouble);

typedef mtsGenericObjectProxy<long> mtsLong;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsLong);

typedef mtsGenericObjectProxy<unsigned long> mtsULong;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsULong);

typedef mtsGenericObjectProxy<int> mtsInt;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsInt);

typedef mtsGenericObjectProxy<unsigned int> mtsUInt;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsUInt);

typedef mtsGenericObjectProxy<short> mtsShort;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsShort);

typedef mtsGenericObjectProxy<unsigned short> mtsUShort;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsUShort);

typedef mtsGenericObjectProxy<char> mtsChar;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsChar);

typedef mtsGenericObjectProxy<unsigned char> mtsUChar;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsUChar);

typedef mtsGenericObjectProxy<bool> mtsBool;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsBool);

typedef mtsGenericObjectProxy<std::string> mtsStdString;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsStdString);

#endif
