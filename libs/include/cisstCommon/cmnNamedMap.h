/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnNamedMap.h 271 2009-04-23 03:14:21Z adeguet1 $

  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2008-11-15

  (C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _cmnNamedMap_h
#define _cmnNamedMap_h

#include <map>
#include <vector>
#include <string>

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnLogger.h>

/*!
  \file
  \brief Declaration of cmnNamedMap
 */


// forward declaration
class cmnClassServicesBase;

/*!
  \ingroup cisstCommon

  cmnNamedMap provides a map (lookup table), where the key is a string
  and the value (item) is specified by the template parameter.  The
  map actually contains a pointer to the value.  In cisstMultiTask,
  this class is used to maintain the lists of interfaces per
  task/device, as well as the lists of commands per interface.

  The cmnNamedMap class is a small "wrapper" around std::map that was
  created to factorize some code that would otherwise be duplicated.
*/
template <class _elementType>
class cmnNamedMap {

public:
    /*! Type of the actual map */
    typedef std::map<std::string, _elementType *> MapType;

    typedef typename MapType::iterator iterator;
    typedef typename MapType::const_iterator const_iterator;
    typedef typename MapType::reverse_iterator reverse_iterator;
    typedef typename MapType::const_reverse_iterator const_reverse_iterator;

protected:
    MapType Map;
    std::string MapName;
    const cmnClassServicesBase * OwnerServices;

    inline const cmnClassServicesBase * Services(void) const {
        return this->OwnerServices;
    }

public:
    /*! Default constructor, initialize the internal map and set the
      map name to "undefined" */
    cmnNamedMap(void):
        Map(),
        MapName("undefined"),
        OwnerServices(0)
    {}

    /*! Constructor with a map name.  The map name is useful for all
      human readable log messages as well as string streaming
      (e.g. std::cout) */
    cmnNamedMap(const std::string & mapName):
        Map(),
        MapName(mapName),
        OwnerServices(0)
    {}

    /*! Constructor with a map name and a pointer a cmnGenericObject
      (in most cases, use "*this").  The map name is useful for all
      human readable log messages as well as string streaming
      (e.g. std::cout).  The cmnGenericObject level of detail will be
      used to filter the messages. */
    cmnNamedMap(const std::string & mapName, const cmnGenericObject & owner):
        Map(),
        MapName(mapName),
        OwnerServices(owner.Services())
    {}

    /*! Destructor.  Relies on std::map destructor. */
    ~cmnNamedMap() {
        //DeleteAll();
    }

    /*! Set the map owner */
    inline void SetOwner(const cmnGenericObject & owner)
    {
        this->OwnerServices = owner.Services();
    }

    /*! Add an item to the internal map.  The log level of details is
      used to determine the lod of a message if the item can not be
      added. */ 
    bool AddItem(const std::string & name,
                 _elementType * item,
                 cmnLogLoD lod = CMN_LOG_LOD_RUN_ERROR);

    /*! Get an item by name */
    _elementType * GetItem(const std::string & name, cmnLogLoD lod = CMN_LOG_LOD_RUN_ERROR) const;

    /*! Remove an item from the internal map.  The log level of
      details is used to determine the lod of a message if the item
      can not be removed. */ 
    bool RemoveItem(const std::string & name, cmnLogLoD lod = CMN_LOG_LOD_RUN_ERROR);

    /*! List of names used, i.e. list of keys in the map */
    //@{
    void GetNames(std::vector<std::string> & placeHolder) const;
    std::vector<std::string> GetNames(void) const;
    //@}

    /*! Type of void method in class _elementType */
    typedef void (_elementType::*VoidMethodPointer)(void);

    /*! Execute a method on all objects contained in the map */
    void ForEachVoid(VoidMethodPointer method);

    /*! Reference on the internal map */
    //@{
    const MapType & GetMap(void) const {
        return this->Map;
    }
    MapType & GetMap(void) {
        return this->Map;
    }
    //@}

    /*! Print the content of the map as well as its name */
    void ToStream(std::ostream & outputStream) const;

    // void Cleanup(void) { ForEachVoid(&_elementType::Cleanup); }  // needed?
  
    /*! Delete all content while preserving keys, i.e. the map
      contains pairs of names and null pointers. */
    void DeleteAll(void);

    /*! STL compatible size */
    inline unsigned int size(void) const {
        return this->Map.size();
    }

    inline bool empty(void) const {
        return this->Map.empty();
    }

    inline void clear(void) {
        this->Map.clear();
    }

    inline iterator begin(void) {
        return this->Map.begin();
    }

    inline const_iterator begin(void) const {
        return this->Map.begin();
    }

    inline iterator end(void) {
        return this->Map.end();
    }

    inline const_iterator end(void) const {
        return this->Map.end();
    }
};



template <class _elementType>
bool cmnNamedMap<_elementType>::AddItem(const std::string & name, _elementType *item, cmnLogLoD lod)
{
    // check if this name already exists
    const typename MapType::const_iterator iterator = Map.find(name);
    if (iterator != Map.end()) {
        if (this->Services()) {
            CMN_LOG_CLASS(lod) << "AddItem: map \"" << MapName << "\" already contains an item named \""
                               << name << "\"." << std::endl;
        } else {
            CMN_LOG(lod) << "class mtsMap: AddItem: map \"" << MapName << "\" already contains an item named \""
                         << name << "\"." << std::endl;
        }
    return false;
    }
    Map[name] = item;
    return true;
}

template <class _elementType>
_elementType * cmnNamedMap<_elementType>::GetItem(const std::string & itemName, cmnLogLoD lod) const {
    const typename MapType::const_iterator iter = Map.find(itemName);
    if (iter != Map.end()) {
        return iter->second;
    } else {
        if (this->Services()) {
            CMN_LOG_CLASS(lod) << "GetItem: can't find \"" << itemName << "\" in map \"" << MapName << "\"" << std::endl;
        } else {
            CMN_LOG(lod) << "class mtsMap: GetItem: can't find \"" << itemName << "\" in map \"" << MapName << "\"" << std::endl;
        }
        return 0;
    }
}

template <class _elementType>
bool cmnNamedMap<_elementType>::RemoveItem(const std::string & itemName, cmnLogLoD lod)
{
    // check if this name already exists
    const typename MapType::iterator iterator = Map.find(itemName);
    if (iterator == Map.end()) {
        if (this->Services()) {
            CMN_LOG_CLASS(lod) << "RemoveItem: can't find \"" << itemName << "\" in map \"" << MapName << "\"" << std::endl;
        } else {
            CMN_LOG(lod) << "class mtsMap: RemoveItem: can't find \"" << itemName << "\" in map \"" << MapName << "\"" << std::endl;
        }
        return false;
    }
    Map.erase(iterator);
    return true;
}


template <class _elementType>
void cmnNamedMap<_elementType>::GetNames(std::vector<std::string> & placeHolder) const {
    placeHolder.clear();
    typename MapType::const_iterator iter = Map.begin();
    const typename MapType::const_iterator end = Map.end();
    for (;
         iter != end;
         ++iter) {
        placeHolder.push_back(iter->first);
    }
}


template <class _elementType>
std::vector<std::string> cmnNamedMap<_elementType>::GetNames(void) const {
    std::vector<std::string> names;
    GetNames(names);
    return names;
}


template <class _elementType>
void cmnNamedMap<_elementType>::ForEachVoid(VoidMethodPointer method)
{
    typename MapType::iterator iter;
    const typename MapType::iterator end = Map.end();
    for (iter = Map.begin(); iter != end; iter++) {
        (iter->second->*method)();
    }
}


template <class _elementType>
void cmnNamedMap<_elementType>::ToStream(std::ostream & outputStream) const
{
    unsigned int counter = 0;
    typename MapType::const_iterator iter = Map.begin();
    const typename MapType::const_iterator end = Map.end();
    for (;
         iter != end;
         ++iter, ++counter) {
        outputStream << "- " << MapName << "[" << counter << "] (\""
                     << iter->first << "\"): " << *(iter->second) << std::endl;
    }
}


template <class _elementType>
void cmnNamedMap<_elementType>::DeleteAll(void) {
    typename MapType::iterator iterator = Map.begin();
    const typename MapType::iterator end = Map.end();
    for (;
         iterator != end;
         ++iterator) {
        delete iterator->second;
    }
    Map.clear();
}

#endif // _cmnNamedMap_h

