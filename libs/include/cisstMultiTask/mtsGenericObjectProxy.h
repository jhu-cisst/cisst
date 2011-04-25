/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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
template <class _elementType> class mtsGenericObjectProxyBase;
template <class _elementType> class mtsGenericObjectProxy;
template <class _elementType> class mtsGenericObjectProxyRef;

#ifndef SWIG
// Class services specialization for proxy objects.  We assume that we always want dynamic creation.
// The specialization is that if the dynamic_cast to the Proxy type fails, we also try to dynamic_cast
// to the ProxyRef type.
template <typename _elementType>
class cmnConditionalObjectFactoryCopy<true, mtsGenericObjectProxy<_elementType> > {
public:
    typedef mtsGenericObjectProxy<_elementType> value_type;
    typedef mtsGenericObjectProxyRef<_elementType> value_reftype;
    typedef value_type * pointer;

    inline static cmnGenericObject * Create(const cmnGenericObject & other) {
        const value_type * otherPointer = dynamic_cast<const value_type *>(&other);
        if (otherPointer)
            return new value_type(*otherPointer);
        const value_reftype * otherRefPointer = dynamic_cast<const value_reftype *>(&other);
        if (otherRefPointer)
            return new value_type(otherRefPointer->GetData());
        return 0;
    }

    inline static bool Create(cmnGenericObject * existing, const cmnGenericObject & other) {
        const value_type * otherPointer = dynamic_cast<const value_type *>(&other);
        if (otherPointer) {
            new(existing) value_type(*otherPointer);
            return true;
        }
        const value_reftype * otherRefPointer = dynamic_cast<const value_reftype *>(&other);
        if (otherRefPointer) {
            new(existing) value_type(otherRefPointer->GetData());
            return true;
        }
        return false;
    }

    inline static cmnGenericObject * CreateArray(size_t size, const cmnGenericObject & other) {
        const value_type * otherPointer = dynamic_cast<const value_type *>(&other);
        pointer data, dummy;
        size_t index;
        if (otherPointer) {
            data = static_cast<pointer>(::operator new(sizeof(value_type) * size));
            for (index = 0; index < size; index++) {
                dummy = new(&(data[index])) value_type(*otherPointer); // placement new with copy constructor
            }
            return data;
        }
        const value_reftype * otherRefPointer = dynamic_cast<const value_reftype *>(&other);
        if (otherRefPointer) {
            data = static_cast<pointer>(::operator new(sizeof(value_type) * size));
            for (index = 0; index < size; index++) {
                dummy = new(&(data[index])) value_type(otherRefPointer->GetData()); // placement new with copy constructor
            }
            return data;
        }
        return 0;
    }

    inline static bool CopyConstructorAvailable(void) { return true; }
};

template<typename _class, typename _elementType>
class cmnConditionalObjectFactoryOneArg<CMN_DYNAMIC_CREATION_ONEARG, _class, mtsGenericObjectProxy<_elementType> >
{
public:
    typedef _class value_type;
    typedef mtsGenericObjectProxy<_elementType> argTypeWrapped;

    /*! Specialization of create when dynamic create is enabled. */
    inline static _class * Create(const cmnGenericObject & arg) {
        const mtsGenericObject *mts = dynamic_cast<const mtsGenericObject *>(&arg);
        if (mts) return new value_type(*mtsGenericTypes<_elementType>::CastArg(*mts));
        CMN_LOG_INIT_WARNING << "cmnConditionalObjectFactoryOneArg::Create for proxy could not create object" << std::endl;
        return 0;
    }

    inline static bool OneArgConstructorAvailable(void) { return true; }

    inline static const cmnClassServicesBase *GetConstructorArgServices(void) {
        return argTypeWrapped::ClassServices();
    }
};

/*!  Specialization of cmnConditionalObjectFactoryOneArg with enabled
     dynamic creation. Requires default constructor and SetName method.
     This emulates a constructor with an std::string parameter and is provided
     for backward compatibility.
*/
template<typename _class>
class cmnConditionalObjectFactoryOneArg<CMN_DYNAMIC_CREATION_SETNAME, _class, mtsGenericObjectProxy<std::string> >
{
public:
    typedef _class value_type;
    typedef mtsGenericObjectProxy<std::string> argTypeWrapped;

    /*! Specialization of create when dynamic creation is enabled. */
    inline static _class * Create(const cmnGenericObject & arg) {
        const mtsGenericObject *mts = dynamic_cast<const mtsGenericObject *>(&arg);
        if (mts) {
            const std::string *name = mtsGenericTypes<std::string>::CastArg(*mts);
            if (name) {
                _class *obj = new value_type;
                obj->SetName(*name);
                return obj;
            }
            CMN_LOG_INIT_WARNING << "cmnConditionalObjectFactoryOneArg::Create for string proxy could not cast to string" << std::endl;
            return 0;
        }
        CMN_LOG_INIT_WARNING << "cmnConditionalObjectFactoryOneArg::Create for string proxy could not create object" << std::endl;
        return 0;
    }

    inline static bool OneArgConstructorAvailable(void) { return true; }

    inline static const cmnClassServicesBase *GetConstructorArgServices(void) {
        return argTypeWrapped::ClassServices();
    }
};

template<typename _elementType>
class cmnConditionalObjectDestructor<true, mtsGenericObjectProxy<_elementType> >
{
    typedef mtsGenericObjectProxy<_elementType> value_type;
    typedef mtsGenericObjectProxyRef<_elementType> value_reftype;
    typedef value_type * pointer;
    typedef cmnGenericObject * generic_pointer;
public:
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

    inline static bool Delete(cmnGenericObject * existing) {
        value_type * existingPointer = dynamic_cast<value_type *>(existing);
        if (existingPointer) {
            existingPointer->~value_type();
            return true;
        }
        const value_reftype * existingRefPointer = dynamic_cast<const value_reftype *>(existing);
        if (existingRefPointer) {
            existingRefPointer->~value_reftype();
            return true;
        }
        return false;
    }
};
#endif  // !SWIG

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
    enum {InitialLoD = CMN_LOG_ALLOW_DEFAULT};
    static cmnClassServicesBase * ClassServices(void) { return DeRefType::ClassServices(); }
    virtual const cmnClassServicesBase * Services(void) const
    {
        return DeRefType::ClassServices();
    }

    /*! Default constructor.  The data member is initialized using its
        default constructor. */
    inline mtsGenericObjectProxyBase(void) : BaseType()
    {}

    /*! Copy constructor. */
    inline mtsGenericObjectProxyBase(const ThisType & other) :
        BaseType(other)
    {}

    inline ~mtsGenericObjectProxyBase(void) {}

    /*! Return pointer to data */
    virtual value_type& GetData(void) = 0;
    virtual const value_type& GetData(void) const = 0;

    /*! Conversion assignment. */
    ThisType & operator=(const ThisType &data) {
        this->Assign(data);
        return *this;
    }

    virtual void Assign(const ThisType &other) {
        this->GetData() = other.GetData();
        this->SetValid(other.Valid());
        this->SetTimestamp(other.Timestamp());
    }

};


template <class _elementType>
class mtsGenericObjectProxy: public mtsGenericObjectProxyBase<_elementType>
{
    CMN_DECLARE_SERVICES_EXPORT_ALWAYS(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef mtsGenericObjectProxy<_elementType> ThisType;
    typedef mtsGenericObjectProxyBase<_elementType> BaseType;
    typedef typename mtsGenericObjectProxyBase<_elementType>::value_type value_type;
    typedef typename mtsGenericObjectProxyBase<_elementType>::DeRefType DeRefType;
    typedef typename mtsGenericObjectProxyBase<_elementType>::RefType RefType;
    value_type Data;

    /*! Default constructor.  The data member is initialized using its
        default constructor. */
    inline mtsGenericObjectProxy(void):
        BaseType(),
        Data()
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

    /*! Return pointer to data */
    value_type& GetData(void) { return Data; }
    const value_type& GetData(void) const { return Data; }

    /*! Conversion assignment, from base type (i.e., Proxy or ProxyRef) to Proxy. */
    ThisType & operator=(const BaseType &data) {
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
        mtsGenericObject::SerializeRaw(outputStream);
        cmnSerializeRaw(outputStream, this->Data);
    }

    /*! DeSerialization.  Relies on the specialization, if any, of cmnDeSerializeRaw. */
    inline void DeSerializeRaw(std::istream & inputStream) {
        mtsGenericObject::DeSerializeRaw(inputStream);
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
    inline mtsGenericObjectProxyRef(void)
    {}

public:
    typedef mtsGenericObjectProxyRef<_elementType> ThisType;
    typedef mtsGenericObjectProxyBase<_elementType> BaseType;
    typedef typename mtsGenericObjectProxyBase<_elementType>::value_type value_type;
    typedef typename mtsGenericObjectProxyBase<_elementType>::DeRefType DeRefType;
    typedef typename mtsGenericObjectProxyBase<_elementType>::RefType RefType;

    value_type &rData;

    /*! Copy constructor. */
    inline mtsGenericObjectProxyRef(const ThisType & other) :
        BaseType(other), rData(other.rData)
    {}

    /*! Conversion constructor.  This allows to construct the proxy
      object using an object of the actual type. */
    inline mtsGenericObjectProxyRef(value_type & data): rData(data)
    {
        this->SetValid(true);
    }
    // The const_cast can be eliminated by defining an mtsGenericObjectProxyConstRef,
    // or by specializing mtsGenericObjectProxyRef for const objects,
    // e.g., template <typename _elementType>
    //       class mtsGenericObjectProxyRef<const _elementType>
    inline mtsGenericObjectProxyRef(const value_type & data):
        rData(*const_cast<value_type*>(&data))
    {
        this->SetValid(true);
    }

    inline ~mtsGenericObjectProxyRef(void) {}

    /*! Return pointer to data */
    value_type& GetData(void) { return rData; }
    const value_type& GetData(void) const { return rData; }

    /*! Conversion assignment, from base type (i.e., Proxy or ProxyRef) to ProxyRef. */
    ThisType & operator=(const BaseType &data) {
        this->Assign(data);
        return *this;
    }

    /*! Conversion assignment.  This allows to assign from an object
      of the actual type without explicitly referencing the public
      data member "Data". */
    inline ThisType & operator=(value_type data) {
        this->rData = data;
        this->SetValid(true);
        return *this;
    }

    /*! Cast operator.  This allows to assign to an object of the
      actual type without explicitly referencing the public data
      member "Data". */
    //@{
    inline operator value_type & (void) {
        return this->rData;
    }
    inline operator const value_type & (void) const {
        return this->rData;
    }
    //@}

    /*! Serialization.  Relies on the specialization, if any, of cmnSerializeRaw. */
    inline void SerializeRaw(std::ostream & outputStream) const {
        mtsGenericObject::SerializeRaw(outputStream);
        cmnSerializeRaw(outputStream, this->rData);
    }

    /*! DeSerialization.  Relies on the specialization, if any, of cmnDeSerializeRaw. */
    inline void DeSerializeRaw(std::istream & inputStream) {
        mtsGenericObject::DeSerializeRaw(inputStream);
        cmnDeSerializeRaw(inputStream, this->rData);
    }

    /*! To stream method.  Uses the default << operator as defined for
        the actual type. */
    inline virtual void ToStream(std::ostream & outputStream) const {
        BaseType::ToStream(outputStream);
        outputStream << " Value(ref): " << this->rData;
    }

    /*! To stream raw data. */
    inline virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                                    bool headerOnly = false, const std::string & headerPrefix = "") const {
        if (headerOnly) {
            BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
            outputStream << delimiter << headerPrefix << "-data(ref)";
        } else {
            BaseType::ToStreamRaw(outputStream, delimiter);
            outputStream << delimiter << this->rData;
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
    static bool IsEqual(const T &obj1, const mtsGenericObject &obj2) {
        const FinalRefType *p2 = dynamic_cast<const FinalRefType *>(&obj2);
        return (p2?(&obj1 == &p2->rData):false); }
    static void ConditionalFree(const FinalRefType *obj) { delete obj;}
    static mtsGenericObject* ConditionalCreate(const T &arg, const std::string &) 
    {
        return new FinalType(arg);
    }

    static T* CastArg(mtsGenericObject &arg) {
        FinalBaseType *tmp = dynamic_cast<FinalBaseType *>(&arg);
        if (!tmp) {
            CMN_LOG_INIT_ERROR << "CastArg could not cast from " << typeid(arg).name()
                               << " to " << typeid(FinalBaseType).name() << std::endl;
            return 0;
        }
        return &(tmp->GetData());
    }
    static const T* CastArg(const mtsGenericObject &arg) {
        const FinalBaseType *tmp = dynamic_cast<const FinalBaseType *>(&arg);
        if (!tmp) {
            CMN_LOG_INIT_ERROR << "CastArg could not cast from const " << typeid(arg).name()
                               << " to const " << typeid(FinalBaseType).name() << std::endl;
            return 0;
        }
        return &(tmp->GetData());
    }
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
    static bool IsEqual(const T &obj1, const mtsGenericObject &obj2) { return &obj1 == &obj2; }
    static void ConditionalFree(const FinalRefType *) {}
    static mtsGenericObject* ConditionalCreate(const T &arg, const std::string &name) { 
        if (typeid(T) != *arg.Services()->TypeInfoPointer()) {
            CMN_LOG_INIT_ERROR << "ConditionalCreate: argument prototype is wrong type for command \"" << name << "\" (expected \""
                               << typeid(T).name() << "\", got \"" 
                               << arg.Services()->TypeInfoPointer()->name() << "\")" << std::endl;
        }
        return new FinalType(arg);
    }

    static T* CastArg(mtsGenericObject &arg) {
        T* temp = dynamic_cast<T * >(&arg);
        if (!temp)
            CMN_LOG_INIT_ERROR << "CastArg for mts, could not cast from " << arg.Services()->GetName()
                               << " to " << T::ClassServices()->GetName() << std::endl;
        return temp;
    }

    static const T* CastArg(const mtsGenericObject &arg) {
        const T* temp = dynamic_cast<const T * >(&arg);
        if (!temp)
            CMN_LOG_INIT_ERROR << "CastArg for mts, could not cast from const " << arg.Services()->GetName()
                               <<" to " << T::ClassServices()->GetName() << std::endl;
        return temp;
    }
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
    static bool IsEqual(const T &obj1, const mtsGenericObject &obj2) { return impl::IsEqual(obj1, obj2); }
    static void ConditionalFree(const FinalRefType *obj) { impl::ConditionalFree(obj); }
    static mtsGenericObject* ConditionalCreate(const T &arg, const std::string &name) { 
        mtsGenericObject *tmp = impl::ConditionalCreate(arg, name);
        if (!tmp)
            CMN_LOG_INIT_ERROR << "ConditionalCreate returning NULL for " << name << " (maybe you should use CMN_DECLARE_SERVICES with CMN_DYNAMIC_CREATION)" << std::endl;
        return tmp;
    }

    static T* CastArg(mtsGenericObject &arg) { 
        return impl::CastArg(arg);
    }
    static const T* CastArg(const mtsGenericObject &arg) {
        return impl::CastArg(arg);
    }
};

template<typename T, bool>
class mtsGenericTypesUnwrapImpl
{
    // T is not a proxy type
public:
    typedef T RefType;
    typedef T BaseType;
};

template<typename T>
class mtsGenericTypesUnwrapImpl<T, true>
{
    // T is a proxy type
public:
    typedef typename T::RefType RefType;
    typedef typename T::BaseType BaseType;
};

template<typename T>
class mtsGenericTypesUnwrap
{
    typedef mtsGenericTypesUnwrapImpl<T, cmnIsDerivedFromTemplated<T, mtsGenericObjectProxyBase >::YES> impl;
public:
    typedef typename mtsGenericTypesUnwrapImpl<T, cmnIsDerivedFromTemplated<T, mtsGenericObjectProxyBase >::YES>::RefType RefType;
    typedef typename mtsGenericTypesUnwrapImpl<T, cmnIsDerivedFromTemplated<T, mtsGenericObjectProxyBase >::YES>::BaseType BaseType;
};

// Some macros for creating class services

#define CMN_IMPLEMENT_SERVICES_ONEARG(className, argType)     \
        CMN_IMPLEMENT_SERVICES_INTERNAL(className, mtsGenericTypes<argType>::FinalType)

#define CMN_IMPLEMENT_SERVICES_TEMPLATED_ONEARG(className, argType)     \
        CMN_IMPLEMENT_SERVICES_TEMPLATED_INTERNAL(className, mtsGenericTypes<argType>::FinalType)

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

typedef std::vector<std::string> stdStringVec;
// Add Proxy to name to distinguish this from mtsVector<std::string>
typedef mtsGenericObjectProxy<stdStringVec> mtsStdStringVecProxy;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsStdStringVecProxy);

// Define stream out operator for stdStringVec
inline std::ostream & operator << (std::ostream & output,
                            const stdStringVec & object) {
    output << "[";
    for (size_t i = 0; i < object.size(); i++) {
        output << object[i];
        if (i < object.size()-1)
            output << ", ";
    }
    output << "]";
    return output;
}


#endif
