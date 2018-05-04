/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Min Yang Jung
  Created on: 2007-04-08

  (C) Copyright 2007-2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


/*!
  \file
  \brief Declaration of cmnDeSerializer and functions cmnDeSerializeRaw
*/
#pragma once

#ifndef _cmnDeSerializer_h
#define _cmnDeSerializer_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnTypeTraits.h>
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegister.h>
#include <cisstCommon/cmnThrow.h>
#include <cisstCommon/cmnAssert.h>

#include <string>
#include <fstream>
#include <map>
#include <cstddef>

#include <cisstCommon/cmnExport.h>


// Implementation class to provide deserialization when _elementType is not derived from cmnGenericObject
template <class _elementType, bool>
class cmnDeSerializeRawImpl
{
public:
    static void DeSerializeRaw(std::istream & inputStream, _elementType & data) CISST_THROW(std::runtime_error)
    {
        inputStream.read(reinterpret_cast<char *>(&data), sizeof(_elementType));
        if (inputStream.fail()) {
            std::string message("cmnDeSerializeRaw(_elementType): Error occured with std::istream::read, _elementType = ");
            message += typeid(_elementType).name();
            cmnThrow(message);
        }
    }
};

// Implementation class to provide deserialization when _elementType is derived from cmnGenericObject
template <class _elementType>
class cmnDeSerializeRawImpl<_elementType, true>
{
public:
    static void DeSerializeRaw(std::istream & inputStream, _elementType & data) CISST_THROW(std::runtime_error)
    {
        data.DeSerializeRaw(inputStream);
    }
};

/*! De-serialization helper function for a basic type.  If the type is derived
    from cmnGenericObject, it just calls the DeSerializeRaw member function.
    Otherwise, it performs a cast to char pointer (<code>char *</code>) using
    <code>reinterpret_cast</code> and then replaces the data with the
    result of <code>read</code> from the input stream.  If the read
    operation fails, an exception is thrown
    (<code>std::runtime_error</code>). */

template <class _elementType>
inline void cmnDeSerializeRaw(std::istream & inputStream, _elementType & data)
    CISST_THROW(std::runtime_error)
{
    typedef cmnDeSerializeRawImpl<_elementType, cmnIsDerivedFrom<_elementType, cmnGenericObject>::IS_DERIVED> impl;
    impl::DeSerializeRaw(inputStream, data);
}

/*! De-serialization helper function for STL size object.  This
  function converts a serialized size (unsigned long long int) to the
  host size_t object.  This operation is required for all size_t as
  the writer/reader can be both 32 bits or 64 bits programs.

  This function should be use to implement the DeSerializeRaw method of
  classes derived from cmnGenericObject. */
inline void cmnDeSerializeSizeRaw(std::istream & inputStream, size_t & data)
    CISST_THROW(std::runtime_error)
{
    unsigned long long int dataToRead;
    cmnDeSerializeRaw(inputStream, dataToRead);
    if (dataToRead <= cmnTypeTraits<size_t>::MaxPositiveValue()) {
        data = static_cast<size_t>(dataToRead);
    } else {
        cmnThrow("cmnDeSerializeSizeRaw: received size greater than maximum supported on this configuration");
    }
}


/*! De-serialization helper function for an STL string.  This function
  first de-serializes the string size, resize the string and then
  replaces its content with the result of <code>read</code> from the
  input stream.  If the read operation fails, an exception is thrown
  (<code>std::runtime_error</code>). */
inline void cmnDeSerializeRaw(std::istream & inputStream, std::string & data)
    CISST_THROW(std::runtime_error)
{
    std::string::size_type size = 0;
    cmnDeSerializeSizeRaw(inputStream, size);
    data.resize(size);
    // The following seems to work fine, but may not be valid for all implementations of std::string
    // because it is not guaranteed that c_str(), or data(), returns a pointer to the internal buffer;
    // it could create a copy and then return a pointer to the copy.
    //
    // inputStream.read(const_cast<char *>(data.c_str()), size * sizeof(std::string::value_type));
    //
    // So, even though it is less efficient, the data is first deserialized to a buffer and then
    // assigned to the string.
    std::string::value_type *buffer = new std::string::value_type[size];
    inputStream.read(static_cast<char *>(buffer), size * sizeof(std::string::value_type));
    data.assign(buffer, size);
    delete [] buffer;
    if (inputStream.fail()) {
        cmnThrow("cmnDeSerializeRaw(std::string): Error occured with std::istream::read");
    }
}


/*! De-serialization helper function for an STL vector.  This function
  first de-serializes the vector size, resizes the vector and then
  alls cmnDeSerializeRaw for each element of the vector.  This assumes that
  cmnDeSerializeRaw is defined for the element type.
  If the read operation fails, an exception is thrown
  (<code>std::runtime_error</code>). */
template <class _elementType>
inline void cmnDeSerializeRaw(std::istream & inputStream, std::vector<_elementType> & data)
    CISST_THROW(std::runtime_error)
{
    typename std::vector<_elementType>::size_type size = 0;
    cmnDeSerializeSizeRaw(inputStream, size);
    data.resize(size);
    for (size_t i = 0; i < size; i++) {
        cmnDeSerializeRaw(inputStream, data[i]);
        if (inputStream.fail()) {
            cmnThrow("cmnDeSerializeRaw(std::vector<_elementType>): Error occured with std::istream::read");
        }
    }
}


/*!
  \brief De-serialization utility class.
  \ingroup cisstCommon

  This class allows to deserialize objects previously serialized using
  cmnSerializer.

  As each object serialized is identified by a unique class identifer
  defined at runtime (address of the object cmnClassServices), this
  class maintains a table of equivalence between the remote class
  identifiers and local class identifiers.  This class can be used to
  read both the class information and objects in any order as long as
  the class information for a specific object has been received before
  the object itself.

  \sa cmnSerializer cmnGenericObject
*/
class CISST_EXPORT cmnDeSerializer: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    /*! Type used to identify objects over the network.  It uses the
      services pointer but as the sender or receiver could be a 32
      or 64 bits OS, we use a data type that can handle both. */
    typedef long long int TypeId;

    /*! Constructor.

      \param inputStream any stream derived from
      <code>std::istream</code>.  The output stream must be created
      with the open flag <code>std::istream::binary</code>.
     */
    cmnDeSerializer(std::istream & inputStream);

    /*! Destructor */
    ~cmnDeSerializer();

    /*! Reset the list of types already received. */
    void Reset(void);

    /*! De-serialize an object from the input stream.  This method
      will create an object dynamically therefore the class of the
      object serialized must have been registered using the flag
      #CMN_DYNAMIC_CREATION (see cmnClassServices and
      cmnClassRegister).  This also requires to provide a default
      constructor for the given class.

      This method will deserialize one object only.  If the stream
      contains some class identifiers (pair of class name, remote
      class identifer) it will read them all until an object is found.

      \param serializeObject If true, object content is serialized.
             If false, only class services is serialized and object
             content is not serialized. True by default.

      \note As this method relies on cmnDeSerializeRaw, it might throw
      an exception.
    */
    cmnGenericObject * DeSerialize(const bool serializeObject = true);


    /*! De-serialize an object from the input stream.  This method
      will replace the object content by the data read from the input
      stream.  There is no dynamic creation involved.

      This method will deserialize one object only.  If the stream
      contains some class identifiers (pair of class name, remote
      class identifer) it will read them all until an object is found.

      \note As this method relies on cmnDeSerializeRaw, it might throw
      an exception.
    */
    template <class _elementType>
    inline void DeSerialize(_elementType & object, const bool serializeObject = true) {
        // get object services
        TypeId typeId;
        cmnDeSerializeRaw(this->InputStream, typeId);
        if (typeId == 0) {
            this->DeSerializeServices();
            // read again to deserialize coming object
            if (serializeObject) {
                this->DeSerialize(object);
            }
        } else {
            const const_iterator end = ServicesContainer.end();
            const const_iterator iterator = ServicesContainer.find(typeId);
            if (iterator == end) {
                cmnThrow("DeSerialize: Can't find corresponding class information");
            } else {
                cmnClassServicesBase * servicesPointerLocal = iterator->second;
                if (servicesPointerLocal != object.Services()) {
                    CMN_LOG_CLASS_RUN_ERROR << "DeSerialize: Object types don't match, local class = "
                        << servicesPointerLocal->GetName() << ", object class = " << object.Services()->GetName()
                        << std::endl;
                    cmnThrow("DeSerialize: Object types don't match");
                } else {
                    if (serializeObject) {
                        object.DeSerializeRaw(this->InputStream);
                    }
                }
            }
        }
    }


 private:
    /*! This method will read the class name and the remote type
        identifier from the input stream.

        It will then try to locate the local type identifier based on
        the class name by doing an STL string comparaison.  If the
        class doesn't exist on the de-serialization side, an exception
        is thrown (<code>std::runtime_error</code>).

        Once the local type identifier is found, this method stores
        the pair in a map.  If there is already an entry for the given
        remote identifier, a warning message is sent using
        #CMN_LOG_CLASS.
     */
    void DeSerializeServices(void) CISST_THROW(std::runtime_error);

    std::istream & InputStream;

    typedef std::map<TypeId, cmnClassServicesBase *> ServicesContainerType;
    typedef ServicesContainerType::value_type EntryType;

    typedef ServicesContainerType::const_iterator const_iterator;
    typedef ServicesContainerType::iterator iterator;

    ServicesContainerType ServicesContainer;
};


// Add services instantiation
CMN_DECLARE_SERVICES_INSTANTIATION(cmnDeSerializer)


#endif // _cmnDeSerialize_h

