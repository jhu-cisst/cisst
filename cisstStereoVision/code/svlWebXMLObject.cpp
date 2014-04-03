/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 
 Author(s): Balazs Vagvolgyi
 Created on: 2009-11-04
 
 (C) Copyright 2005-2009 Johns Hopkins University (JHU), All Rights
 Reserved.
 
 --- begin cisst license - do not edit ---
 
 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.
 
 --- end cisst license ---
 
 */


#include "cisstStereoVision/svlWebXMLObject.h"

#include <iostream>
#include <sstream>
#include <libxml/parser.h>
#include <libxml/tree.h>


svlWebXMLObject::svlWebXMLObject(const std::string & name, bool readable, bool writable) :
    svlWebObjectBase(name),
    InvalidNode("Invalid node"),
    Cache(0),
    CacheSize(0)
{
    Readable = readable;
    Writable = writable;
    SetContentType("text/xml");

    LIBXML_TEST_VERSION
    xmlInitParser();
}

svlWebXMLObject::~svlWebXMLObject()
{
    xmlCleanupParser();
    if (Cache) delete [] Cache;
}

svlWebXMLObject* svlWebXMLObject::clone() const
{
    return new svlWebXMLObject(*this);
}

void svlWebXMLObject::Read(char*& data, int& datasize)
{
    int size;

    Lock();
        Serialize();
        size = XML.length();
        CheckCacheSize(size);
        memcpy(Cache, XML.c_str(), size);
        data = Cache;
        datasize = size;
    Unlock();
}

void svlWebXMLObject::Write(char* data, int datasize)
{
    if (!data || datasize < 1) return;

    Lock();
        XML.assign(data, datasize);
        Deserialize();
    Unlock();
}

int svlWebXMLObject::AddNode(const std::string & name, const std::string & value)
{
    int ret;
    _NodeData data;
    data.modified = 1;
    data.value = value;
    
    Lock();
        ret = Nodes.insert(_NodePair(name, data)).second;
    Unlock();
    
    if (ret == false) return -1;
    return 0;
}

int svlWebXMLObject::RemoveNode(const std::string & name)
{
    int ret;
    
    Lock();
        ret = Nodes.erase(name);
    Unlock();
    
    if (ret == 0) return -1;
    return 0;
}

int svlWebXMLObject::SetNode(const std::string & name, const std::string & value)
{
    int ret = -1;
    
    Lock();
        _NodeMap::iterator iter = Nodes.find(name);
        if (iter != Nodes.end()) {
            iter->second.modified = 1;
            iter->second.value = value;
            ret = 0;
        }
    Unlock();
    
    return ret;
}

int svlWebXMLObject::GetNode(const std::string & name, std::string & value)
{
    int ret = -1;
    
    Lock();
        _NodeMap::iterator iter = Nodes.find(name);
        if (iter != Nodes.end()) {
            value = iter->second.value;
            ret = 0;
        }
    Unlock();
    
    return ret;
}

const std::string & svlWebXMLObject::GetNode(const std::string & name)
{
    int ret = -1;
    
    Lock();
        _NodeMap::iterator iter = Nodes.find(name);
        if (iter != Nodes.end()) ret = 0;
    Unlock();
    
    if (ret == 0) return iter->second.value;
    return InvalidNode;
}

void svlWebXMLObject::Serialize()
{
    std::stringstream strstrm;
    strstrm << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    strstrm << "<" << GetName() << ">\n";
    _NodeMap::iterator iter = Nodes.begin();
    for (; iter != Nodes.end(); iter ++) {
        strstrm << "<" << iter->first << ">" << iter->second.value << "</" << iter->first << ">\n";
    }
    strstrm << "</" << GetName() << ">\n";
    XML = strstrm.str();
}

void svlWebXMLObject::Deserialize()
{
    xmlDocPtr doc = xmlReadMemory(XML.c_str(), XML.length(), "noname.xml", 0, 0);
    if (doc == 0) {
        std::cout << "xmlReadMemory - unable to parse XML document" << std::endl;
        return;
    }
    parseDocTree(xmlDocGetRootElement(doc), 1);
    xmlFreeDoc(doc);
}

int svlWebXMLObject::parseDocTree(void *node, unsigned int depth)
{
    xmlNode *begin_node = reinterpret_cast<xmlNode*>(node);
    xmlNode *cur_node = 0;
    xmlChar *content = 0;
    _NodeMap::iterator iter;
    std::string str;
    
    for (cur_node = begin_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if (xmlLastElementChild(cur_node) == 0) { // leaf node
                // look up corresponding local node
                str.assign((const char*)cur_node->name);
                iter = Nodes.find(str);
                if (iter != Nodes.end()) {
                    if (iter->second.modified) {
                        // if local content has changed:
                        //   reset modified flag and ignore remote content
                        iter->second.modified = 0;
                        continue;
                    }
                    // extract value
                    content = xmlNodeGetContent(cur_node);
                    if (content) {
                        iter->second.value.assign((const char*)content);
                        xmlFree(content);
                    }
                    else {
                        iter->second.value = "";
                    }
                }
            }
            else { // node has child(ren)
                str.assign((const char*)cur_node->name);
                if (GetName().compare(str) != 0) {
                    // don't parse if memory doesn't correspond to this object
                    return -1;
                }
                if (depth != 0) parseDocTree(cur_node->children, depth - 1);
            }
        }
    }
    
    return 0;
}

void svlWebXMLObject::CheckCacheSize(int size)
{
    if (size > CacheSize) {
        CacheSize = size + 1024;
        if (Cache) delete [] Cache;
        Cache = new char[CacheSize];
    }
}

