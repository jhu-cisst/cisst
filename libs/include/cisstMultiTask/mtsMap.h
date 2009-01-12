/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsMap.h,v 1.3 2008/12/14 06:39:19 pkaz Exp $

  Author(s):  Peter Kazanzides
  Created on: 2008-11-15

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsMap_h
#define _mtsMap_h

#include <map>
#include <vector>
#include <string>

#include <cisstCommon/cmnLogger.h>

/*!
  \file
  \brief Declaration of mtsMap
 */


/*!
  \ingroup cisstMultiTask

  mtsMap provides a map (lookup table), where the key is a string and the value (item) is
  specified by the template parameter.  The map actually contains a pointer to the value.
  This class is used to maintain the lists of interfaces per task/device, as well as the lists
  of commands per interface.

  The mtsMap class is a small "wrapper" around std::map that was created to factorize some code
  that would otherwise be duplicated.
*/

template <class _ItemType>
class mtsMap {
public:
  typedef std::map<std::string, _ItemType *> MapType;
protected:
  MapType Map;
  std::string MapName;
public:
  mtsMap() : Map(), MapName("???") {}
  mtsMap(const std::string & mapName) : Map(), MapName(mapName) {}
  ~mtsMap() {}
  bool AddItem(const std::string & name, _ItemType * item, cmnLogger::LoDType lod = 99);
  _ItemType *GetItem(const std::string & name, cmnLogger::LoDType lod = 99) const;
  std::vector<std::string> GetNames() const;
  const MapType &GetMap() const { return Map; }
  MapType &GetMap() { return Map; }
  void ToStream(std::ostream & outputStream) const;
  void Cleanup(void);  // needed?
  // free all memory
  void DeleteAll(void);
};

template <class _ItemType>
bool mtsMap<_ItemType>::AddItem(const std::string & name, _ItemType *item, cmnLogger::LoDType lod)
{
    // check if this name already exists
    typename MapType::const_iterator iterator = Map.find(name);
    if (iterator != Map.end()) {
        CMN_LOG(lod) << "a " << MapName << " item named " << name
                     << " already exists." << std::endl;
        return false;
    }
    Map[name] = item;
    return true;
}

template <class _ItemType>
_ItemType *mtsMap<_ItemType>::GetItem(const std::string & itemName, cmnLogger::LoDType lod) const {
    typename MapType::const_iterator iter;
    iter = Map.find(itemName);
    if (iter != Map.end())
        return iter->second;
    else {
        CMN_LOG(lod) << "can't find \"" << itemName << "\" in " << MapName << std::endl;
        return 0;
    }
}

template <class _ItemType>
std::vector<std::string> mtsMap<_ItemType>::GetNames(void) const {
    std::vector<std::string> names;
    typename MapType::const_iterator iter;
    for (iter = Map.begin(); iter != Map.end(); ++iter) {
        names.push_back(iter->first);
    }
    return names;
}

template <class _ItemType>
void mtsMap<_ItemType>::ToStream(std::ostream & outputStream) const
{
    unsigned int counter = 0;
    typename MapType::const_iterator iter;
    for (iter = Map.begin();
         iter != Map.end();
         ++iter, ++counter) {
        outputStream << "- " << MapName << "[" << counter << "]: "
                     << *(iter->second) << std::endl;
    }
}


template <class _ItemType>
void mtsMap<_ItemType>::Cleanup(void) {
    typename MapType::iterator iterator = Map.begin();
    const typename MapType::iterator end = Map.end();
    for (;
         iterator != end;
         ++iterator) {
        iterator->second->Cleanup();
    }
}

template <class _ItemType>
void mtsMap<_ItemType>::DeleteAll(void) {
    typename MapType::iterator iterator = Map.begin();
    const typename MapType::iterator end = Map.end();
    for (;
         iterator != end;
         ++iterator) {
        delete iterator->second;
    }
}

#endif // _mtsMap_h

