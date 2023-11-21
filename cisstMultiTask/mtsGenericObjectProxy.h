/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Ankur Kapoor, Anton Deguet, Peter Kazanzides
  Created on: 2006-05-05

  (C) Copyright 2006-2023 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsGenericObjectProxy_h
#define _mtsGenericObjectProxy_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>
#include <cisstCommon/cmnTypeTraits.h>

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstCommon/cmnDataFunctionsString.h>
#include <cisstCommon/cmnDataFunctionsVector.h>
#include <cisstCommon/cmnDataFunctionsList.h>

#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctDataFunctionsFixedSizeVector.h>

#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsGenericObject.h>

// Always include last!
#include <cisstMultiTask/mtsExport.h>

typedef std::vector<std::string> stdStringVec;
typedef std::vector<double> stdDoubleVec;
typedef std::vector<char> stdCharVec;
typedef std::vector<vct3> stdVct3Vec;

typedef std::list<std::string> stdStringList;

// Forward declarations
template <class _elementType> class mtsGenericObjectProxyBase;
template <class _elementType> class mtsGenericObjectProxy;
template <class _elementType> class mtsGenericObjectProxyRef;
template <typename _elementType> class mtsGenericTypes;

template <typename _elementType, bool>
class cmnDataProxy
{
    // _elementType does not have cmnData<_elementType> static methods, so use the "old" way
public:
    static void CISST_DEPRECATED ToStream(std::ostream & outputStream, const _elementType & data) {
        outputStream << data;
    }
    static void CISST_DEPRECATED ToStreamRaw(std::ostream & outputStream, const char CMN_UNUSED(delimiter), const _elementType & data) {
        outputStream << data;
    }
    static bool CISST_DEPRECATED FromStreamRaw(std::istream & CMN_UNUSED(inputStream), const char CMN_UNUSED(delimiter), _elementType & CMN_UNUSED(data)) {
        // Could try the "stream in" operator
        return false;
    }
};

template <typename _elementType>
class cmnDataProxy<_elementType, true>
{
    // _elementType has the cmnData<_elementType> static methods
public:
    static void ToStream(std::ostream & outputStream, const _elementType & data) {
        outputStream << cmnData<_elementType>::HumanReadable(data);
    }
    static void ToStreamRaw(std::ostream & outputStream, const char delimiter, const _elementType & data) {
        cmnData<_elementType>::SerializeText(data, outputStream, delimiter);
    }
    static bool FromStreamRaw(std::istream & inputStream, const char delimiter, _elementType & data) {
        try {
            cmnDataDeSerializeTextDelimiter(inputStream, delimiter, "mtsGenericObjectProxy");
            cmnData<_elementType>::DeSerializeText(data, inputStream, delimiter);
        } catch (...) {
            return false;
        }
        if (inputStream.fail()) {
            inputStream.clear();
            return false;
        }
        return true;
    }
};

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
        // If they already point to the same memory, just return
        if (existing == &other)
            return true;
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
        pointer data;
        size_t index;
        if (otherPointer) {
            data = static_cast<pointer>(::operator new(sizeof(value_type) * size));
            for (index = 0; index < size; index++) {
                new(&(data[index])) value_type(*otherPointer); // placement new with copy constructor
            }
            return data;
        }
        const value_reftype * otherRefPointer = dynamic_cast<const value_reftype *>(&other);
        if (otherRefPointer) {
            data = static_cast<pointer>(::operator new(sizeof(value_type) * size));
            for (index = 0; index < size; index++) {
                new(&(data[index])) value_type(otherRefPointer->GetData()); // placement new with copy constructor
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
     for backward compatibility. Here _elementType should be std::string.
*/
template<typename _class, typename _elementType>
class cmnConditionalObjectFactoryOneArg<CMN_DYNAMIC_CREATION_SETNAME, _class, mtsGenericObjectProxy<_elementType> >
{
public:
    typedef _class value_type;
    typedef mtsGenericObjectProxy<_elementType> argTypeWrapped;

    /*! Specialization of create when dynamic creation is enabled. */
    inline static _class * Create(const cmnGenericObject & arg) {
        const mtsGenericObject *mts = dynamic_cast<const mtsGenericObject *>(&arg);
        if (mts) {
            const _elementType *name = mtsGenericTypes<_elementType>::CastArg(*mts);
            if (name) {
                _class *obj = new value_type;
                obj->SetName(*name);
                return obj;
            }
            CMN_LOG_INIT_WARNING << "cmnConditionalObjectFactoryOneArg::Create for string proxy could not get string" << std::endl;
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
    virtual value_type & GetData(void) = 0;
    virtual const value_type & GetData(void) const = 0;

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

    /*! Assignment operator */
    inline ThisType & operator = (const ThisType & other) {
        BaseType::operator = (other);
        Data = other.Data;
        return *this;
    }

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
        outputStream << " Value: ";
        cmnDataProxy<value_type, cmnData<value_type>::IS_SPECIALIZED>::ToStream(outputStream, this->Data);
    }

    /*! To stream raw data. */
    inline virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                                    bool headerOnly = false, const std::string & headerPrefix = "") const {
        if (headerOnly) {
            BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
            outputStream << delimiter << headerPrefix << "-data";
        } else {
            BaseType::ToStreamRaw(outputStream, delimiter);
            outputStream << delimiter;
            cmnDataProxy<value_type, cmnData<value_type>::IS_SPECIALIZED>::ToStreamRaw(outputStream, delimiter, this->Data);
        }
    }

    /*! From stream raw. */
    inline virtual bool FromStreamRaw(std::istream & inputStream, const char delimiter = ' ') {
        BaseType::FromStreamRaw(inputStream, delimiter);
        return cmnDataProxy<value_type, cmnData<value_type>::IS_SPECIALIZED>::FromStreamRaw(inputStream, delimiter, this->Data);
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
        outputStream << " Value(ref): ";
        cmnDataProxy<value_type, cmnData<value_type>::IS_SPECIALIZED>::ToStream(outputStream, this->rData);
    }

    /*! To stream raw data. */
    inline virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                                    bool headerOnly = false, const std::string & headerPrefix = "") const {
        if (headerOnly) {
            BaseType::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
            outputStream << delimiter << headerPrefix << "-data(ref)";
        } else {
            BaseType::ToStreamRaw(outputStream, delimiter);
            outputStream << delimiter;
            cmnDataProxy<value_type, cmnData<value_type>::IS_SPECIALIZED>::ToStreamRaw(outputStream, delimiter, this->rData);
        }
    }

    /*! From stream raw. */
    inline virtual bool FromStreamRaw(std::istream & inputStream, const char delimiter = ' ') {
        BaseType::FromStreamRaw(inputStream, delimiter);
        return cmnDataProxy<value_type, cmnData<value_type>::IS_SPECIALIZED>::FromStreamRaw(inputStream, delimiter, this->rData);
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
        if (!temp) {
            CMN_LOG_INIT_ERROR << "CastArg for mts, could not cast from " << arg.Services()->GetName()
                               << " to " << T::ClassServices()->GetName() << std::endl;
        }
        return temp;
    }

    static const T* CastArg(const mtsGenericObject &arg) {
        const T* temp = dynamic_cast<const T * >(&arg);
        if (!temp) {
            CMN_LOG_INIT_ERROR << "CastArg for mts, could not cast from const " << arg.Services()->GetName()
                               <<" to " << T::ClassServices()->GetName() << std::endl;
        }
        return temp;
    }
};

template<typename T>
class mtsGenericTypes
{
    typedef mtsGenericTypesImpl<T, cmnIsDerivedFrom<T, mtsGenericObject>::IS_DERIVED> impl;
public:
    typedef typename mtsGenericTypesImpl<T, cmnIsDerivedFrom<T, mtsGenericObject>::IS_DERIVED>::FinalBaseType FinalBaseType;
    typedef typename mtsGenericTypesImpl<T, cmnIsDerivedFrom<T, mtsGenericObject>::IS_DERIVED>::FinalType     FinalType;
    typedef typename mtsGenericTypesImpl<T, cmnIsDerivedFrom<T, mtsGenericObject>::IS_DERIVED>::FinalRefType  FinalRefType;
    static FinalRefType *ConditionalWrap(T &obj) { return impl::ConditionalWrap(obj); }
    static bool IsEqual(const T &obj1, const mtsGenericObject &obj2) { return impl::IsEqual(obj1, obj2); }
    static void ConditionalFree(const FinalRefType *obj) { impl::ConditionalFree(obj); }
    static mtsGenericObject* ConditionalCreate(const T &arg, const std::string &name) {
        mtsGenericObject *tmp = impl::ConditionalCreate(arg, name);
        if (!tmp) {
            CMN_LOG_INIT_ERROR << "ConditionalCreate returning NULL for " << name
                               << " (maybe you should use CMN_DECLARE_SERVICES with CMN_DYNAMIC_CREATION)" << std::endl;
        }
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
    typedef mtsGenericTypesUnwrapImpl<T, cmnIsDerivedFromTemplated<T, mtsGenericObjectProxyBase >::IS_DERIVED> impl;
public:
    typedef typename mtsGenericTypesUnwrapImpl<T, cmnIsDerivedFromTemplated<T, mtsGenericObjectProxyBase >::IS_DERIVED>::RefType RefType;
    typedef typename mtsGenericTypesUnwrapImpl<T, cmnIsDerivedFromTemplated<T, mtsGenericObjectProxyBase >::IS_DERIVED>::BaseType BaseType;
};

// Some macros for creating class services

#define CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(className, parentName, argType) \
            CMN_IS_DERIVED_FROM_ASSERT(className, parentName) \
            CMN_IMPLEMENT_SERVICES_INTERNAL(className, parentName::ClassServices(), mtsGenericTypes<argType>::FinalType)

#define CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG_TEMPLATED(className, parentName, argType) \
            CMN_IS_DERIVED_FROM_ASSERT(className, parentName) \
            CMN_IMPLEMENT_SERVICES_TEMPLATED_INTERNAL(className, parentName::ClassServices(), mtsGenericTypes<argType>::FinalType)
#endif

/* Some basic types defined here for now, could move somewhere else. */
typedef mtsGenericObjectProxy<double> mtsDouble;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsDouble);

typedef mtsGenericObjectProxy<float> mtsFloat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsFloat);

typedef mtsGenericObjectProxy<long> mtsLong;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsLong);

typedef mtsGenericObjectProxy<long long> mtsLongLong;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsLongLong);

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

#if (CISST_OS == CISST_WINDOWS)
typedef mtsGenericObjectProxy<unsigned __int64> mtsUInt64;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsUInt64);
#endif

typedef mtsGenericObjectProxy<std::string> mtsStdString;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsStdString);

// Add Proxy to name to distinguish this from mtsVector<std::string>
typedef mtsGenericObjectProxy<stdStringVec> mtsStdStringVecProxy;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsStdStringVecProxy);

typedef mtsGenericObjectProxy<stdDoubleVec> mtsStdDoubleVecProxy;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsStdDoubleVecProxy);

typedef mtsGenericObjectProxy<stdCharVec> mtsStdCharVecProxy;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsStdCharVecProxy);

typedef mtsGenericObjectProxy<stdVct3Vec> mtsStdVct3VecProxy;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsStdVct3VecProxy);

typedef mtsGenericObjectProxy<stdStringList> mtsStdStringListProxy;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsStdStringListProxy);

// Now, define proxies for cisstVector classes (see also
// mtsFixedSizeVectorTypes.h, which uses multiple inheritance,
// rather than proxies).
typedef mtsGenericObjectProxy<vct1> mtsVct1;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVct1)
typedef mtsGenericObjectProxy<vct2> mtsVct2;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVct2)
typedef mtsGenericObjectProxy<vct3> mtsVct3;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVct3)
typedef mtsGenericObjectProxy<vct4> mtsVct4;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVct4)
typedef mtsGenericObjectProxy<vct5> mtsVct5;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVct5)
typedef mtsGenericObjectProxy<vct6> mtsVct6;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVct6)
typedef mtsGenericObjectProxy<vct7> mtsVct7;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVct7)
typedef mtsGenericObjectProxy<vct8> mtsVct8;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVct8)
typedef mtsGenericObjectProxy<vct9> mtsVct9;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVct9)

typedef mtsGenericObjectProxy<vctFloat1> mtsVctFloat1;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctFloat1)
typedef mtsGenericObjectProxy<vctFloat2> mtsVctFloat2;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctFloat2)
typedef mtsGenericObjectProxy<vctFloat3> mtsVctFloat3;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctFloat3)
typedef mtsGenericObjectProxy<vctFloat4> mtsVctFloat4;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctFloat4)
typedef mtsGenericObjectProxy<vctFloat5> mtsVctFloat5;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctFloat5)
typedef mtsGenericObjectProxy<vctFloat6> mtsVctFloat6;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctFloat6)
typedef mtsGenericObjectProxy<vctFloat7> mtsVctFloat7;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctFloat7)
typedef mtsGenericObjectProxy<vctFloat8> mtsVctFloat8;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctFloat8)
typedef mtsGenericObjectProxy<vctFloat9> mtsVctFloat9;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctFloat9)

typedef mtsGenericObjectProxy<vctLong1> mtsVctLong1;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctLong1)
typedef mtsGenericObjectProxy<vctLong2> mtsVctLong2;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctLong2)
typedef mtsGenericObjectProxy<vctLong3> mtsVctLong3;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctLong3)
typedef mtsGenericObjectProxy<vctLong4> mtsVctLong4;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctLong4)
typedef mtsGenericObjectProxy<vctLong5> mtsVctLong5;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctLong5)
typedef mtsGenericObjectProxy<vctLong6> mtsVctLong6;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctLong6)
typedef mtsGenericObjectProxy<vctLong7> mtsVctLong7;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctLong7)
typedef mtsGenericObjectProxy<vctLong8> mtsVctLong8;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctLong8)
typedef mtsGenericObjectProxy<vctLong9> mtsVctLong9;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctLong9)

typedef mtsGenericObjectProxy<vctULong1> mtsVctULong1;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctULong1)
typedef mtsGenericObjectProxy<vctULong2> mtsVctULong2;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctULong2)
typedef mtsGenericObjectProxy<vctULong3> mtsVctULong3;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctULong3)
typedef mtsGenericObjectProxy<vctULong4> mtsVctULong4;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctULong4)
typedef mtsGenericObjectProxy<vctULong5> mtsVctULong5;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctULong5)
typedef mtsGenericObjectProxy<vctULong6> mtsVctULong6;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctULong6)
typedef mtsGenericObjectProxy<vctULong7> mtsVctULong7;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctULong7)
typedef mtsGenericObjectProxy<vctULong8> mtsVctULong8;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctULong8)
typedef mtsGenericObjectProxy<vctULong9> mtsVctULong9;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctULong9)

typedef mtsGenericObjectProxy<vctInt1> mtsVctInt1;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctInt1)
typedef mtsGenericObjectProxy<vctInt2> mtsVctInt2;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctInt2)
typedef mtsGenericObjectProxy<vctInt3> mtsVctInt3;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctInt3)
typedef mtsGenericObjectProxy<vctInt4> mtsVctInt4;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctInt4)
typedef mtsGenericObjectProxy<vctInt5> mtsVctInt5;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctInt5)
typedef mtsGenericObjectProxy<vctInt6> mtsVctInt6;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctInt6)
typedef mtsGenericObjectProxy<vctInt7> mtsVctInt7;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctInt7)
typedef mtsGenericObjectProxy<vctInt8> mtsVctInt8;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctInt8)
typedef mtsGenericObjectProxy<vctInt9> mtsVctInt9;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctInt9)

typedef mtsGenericObjectProxy<vctUInt1> mtsVctUInt1;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUInt1)
typedef mtsGenericObjectProxy<vctUInt2> mtsVctUInt2;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUInt2)
typedef mtsGenericObjectProxy<vctUInt3> mtsVctUInt3;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUInt3)
typedef mtsGenericObjectProxy<vctUInt4> mtsVctUInt4;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUInt4)
typedef mtsGenericObjectProxy<vctUInt5> mtsVctUInt5;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUInt5)
typedef mtsGenericObjectProxy<vctUInt6> mtsVctUInt6;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUInt6)
typedef mtsGenericObjectProxy<vctUInt7> mtsVctUInt7;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUInt7)
typedef mtsGenericObjectProxy<vctUInt8> mtsVctUInt8;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUInt8)
typedef mtsGenericObjectProxy<vctUInt9> mtsVctUInt9;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUInt9)

typedef mtsGenericObjectProxy<vctShort1> mtsVctShort1;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctShort1)
typedef mtsGenericObjectProxy<vctShort2> mtsVctShort2;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctShort2)
typedef mtsGenericObjectProxy<vctShort3> mtsVctShort3;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctShort3)
typedef mtsGenericObjectProxy<vctShort4> mtsVctShort4;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctShort4)
typedef mtsGenericObjectProxy<vctShort5> mtsVctShort5;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctShort5)
typedef mtsGenericObjectProxy<vctShort6> mtsVctShort6;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctShort6)
typedef mtsGenericObjectProxy<vctShort7> mtsVctShort7;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctShort7)
typedef mtsGenericObjectProxy<vctShort8> mtsVctShort8;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctShort8)
typedef mtsGenericObjectProxy<vctShort9> mtsVctShort9;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctShort9)

typedef mtsGenericObjectProxy<vctUShort1> mtsVctUShort1;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUShort1)
typedef mtsGenericObjectProxy<vctUShort2> mtsVctUShort2;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUShort2)
typedef mtsGenericObjectProxy<vctUShort3> mtsVctUShort3;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUShort3)
typedef mtsGenericObjectProxy<vctUShort4> mtsVctUShort4;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUShort4)
typedef mtsGenericObjectProxy<vctUShort5> mtsVctUShort5;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUShort5)
typedef mtsGenericObjectProxy<vctUShort6> mtsVctUShort6;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUShort6)
typedef mtsGenericObjectProxy<vctUShort7> mtsVctUShort7;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUShort7)
typedef mtsGenericObjectProxy<vctUShort8> mtsVctUShort8;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUShort8)
typedef mtsGenericObjectProxy<vctUShort9> mtsVctUShort9;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUShort9)

typedef mtsGenericObjectProxy<vctChar1> mtsVctChar1;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctChar1)
typedef mtsGenericObjectProxy<vctChar2> mtsVctChar2;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctChar2)
typedef mtsGenericObjectProxy<vctChar3> mtsVctChar3;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctChar3)
typedef mtsGenericObjectProxy<vctChar4> mtsVctChar4;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctChar4)
typedef mtsGenericObjectProxy<vctChar5> mtsVctChar5;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctChar5)
typedef mtsGenericObjectProxy<vctChar6> mtsVctChar6;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctChar6)
typedef mtsGenericObjectProxy<vctChar7> mtsVctChar7;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctChar7)
typedef mtsGenericObjectProxy<vctChar8> mtsVctChar8;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctChar8)
typedef mtsGenericObjectProxy<vctChar9> mtsVctChar9;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctChar9)

typedef mtsGenericObjectProxy<vctUChar1> mtsVctUChar1;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUChar1)
typedef mtsGenericObjectProxy<vctUChar2> mtsVctUChar2;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUChar2)
typedef mtsGenericObjectProxy<vctUChar3> mtsVctUChar3;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUChar3)
typedef mtsGenericObjectProxy<vctUChar4> mtsVctUChar4;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUChar4)
typedef mtsGenericObjectProxy<vctUChar5> mtsVctUChar5;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUChar5)
typedef mtsGenericObjectProxy<vctUChar6> mtsVctUChar6;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUChar6)
typedef mtsGenericObjectProxy<vctUChar7> mtsVctUChar7;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUChar7)
typedef mtsGenericObjectProxy<vctUChar8> mtsVctUChar8;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUChar8)
typedef mtsGenericObjectProxy<vctUChar9> mtsVctUChar9;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUChar9)

typedef mtsGenericObjectProxy<vctBool1> mtsVctBool1;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctBool1)
typedef mtsGenericObjectProxy<vctBool2> mtsVctBool2;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctBool2)
typedef mtsGenericObjectProxy<vctBool3> mtsVctBool3;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctBool3)
typedef mtsGenericObjectProxy<vctBool4> mtsVctBool4;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctBool4)
typedef mtsGenericObjectProxy<vctBool5> mtsVctBool5;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctBool5)
typedef mtsGenericObjectProxy<vctBool6> mtsVctBool6;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctBool6)
typedef mtsGenericObjectProxy<vctBool7> mtsVctBool7;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctBool7)
typedef mtsGenericObjectProxy<vctBool8> mtsVctBool8;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctBool8)
typedef mtsGenericObjectProxy<vctBool9> mtsVctBool9;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctBool9)

// Define a few fixed size matrices
#include <cisstVector/vctFixedSizeMatrixTypes.h>
#include <cisstVector/vctDataFunctionsFixedSizeMatrix.h>
typedef mtsGenericObjectProxy<vct2x2> mtsVct2x2;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVct2x2)
typedef mtsGenericObjectProxy<vct3x3> mtsVct3x3;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVct3x3)
typedef mtsGenericObjectProxy<vct4x4> mtsVct4x4;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVct4x4)

// Transformation types (see also mtsTransformationTypes.h,
// which uses multiple inheritance).
#include <cisstVector/vctTransformationTypes.h>
#include <cisstVector/vctDataFunctionsTransformations.h>
typedef mtsGenericObjectProxy<vctMatRot3> mtsVctMatRot3;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctMatRot3)
typedef mtsGenericObjectProxy<vctFrm3> mtsVctFrm3;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctFrm3)
typedef mtsGenericObjectProxy<vctFrm4x4> mtsVctFrm4x4;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctFrm4x4)

// Dynamic vectors (see also mtsVector.h, which uses
// multiple inheritance)
#include <cisstVector/vctDynamicVectorTypes.h>
#include <cisstVector/vctDataFunctionsDynamicVector.h>
typedef mtsGenericObjectProxy<vctDoubleVec> mtsVctDoubleVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctDoubleVec)
typedef mtsGenericObjectProxy<vctFloatVec> mtsVctFloatVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctFloatVec)
typedef mtsGenericObjectProxy<vctIntVec> mtsVctIntVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctIntVec)
typedef mtsGenericObjectProxy<vctUIntVec> mtsVctUIntVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUIntVec)
typedef mtsGenericObjectProxy<vctCharVec> mtsVctCharVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctCharVec)
typedef mtsGenericObjectProxy<vctUCharVec> mtsVctUCharVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUCharVec)
typedef mtsGenericObjectProxy<vctBoolVec> mtsVctBoolVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctBoolVec)
typedef mtsGenericObjectProxy<vctShortVec> mtsVctShortVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctShortVec)
typedef mtsGenericObjectProxy<vctUShortVec> mtsVctUShortVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctUShortVec)
typedef mtsGenericObjectProxy<vctLongVec> mtsVctLongVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctLongVec)
typedef mtsGenericObjectProxy<vctULongVec> mtsVctULongVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctULongVec)

// Dynamic matrices (see also mtsMatrix.h, which uses
// multiple inheritance)
#include <cisstVector/vctDynamicMatrixTypes.h>
#include <cisstVector/vctDataFunctionsDynamicMatrix.h>
typedef mtsGenericObjectProxy<vctDoubleMat> mtsVctDoubleMat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctDoubleMat)
typedef mtsGenericObjectProxy<vctFloatMat> mtsVctFloatMat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctFloatMat)
typedef mtsGenericObjectProxy<vctIntMat> mtsVctIntMat;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsVctIntMat)

#endif
