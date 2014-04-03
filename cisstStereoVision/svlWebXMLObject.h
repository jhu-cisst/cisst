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


#ifndef _svlWebXMLObject_h
#define _svlWebXMLObject_h

#include <cisstCommon.h>
#include <cisstOSAbstraction/osaCriticalSection.h>
#include <cisstStereoVision/svlWebObjectBase.h>

#include <map>
#include <string>

// Always the last cisst include
#include <cisstCommon/cmnExport.h>


class CISST_EXPORT svlWebXMLObject : public svlWebObjectBase
{
    typedef struct __NodeData {
        int modified;
        std::string value;
    } _NodeData;
    typedef std::pair<std::string, _NodeData> _NodePair;
    typedef std::map<std::string, _NodeData> _NodeMap;

public:
    svlWebXMLObject(const std::string & name, bool readable, bool writable);
    virtual ~svlWebXMLObject();
    virtual svlWebXMLObject* clone() const;

    virtual void Read(char*& data, int& datasize);
    virtual void Write(char* data, int datasize);

    virtual int AddNode(const std::string & name, const std::string & value);
    virtual int RemoveNode(const std::string & name);
    virtual int SetNode(const std::string & name, const std::string & value);
    virtual int GetNode(const std::string & name, std::string & value);
    virtual const std::string & GetNode(const std::string & name);

private:
    svlWebXMLObject();

    void Serialize();
    void Deserialize();
    int parseDocTree(void *node, unsigned int depth = -1);
    void CheckCacheSize(int size);

    _NodeMap Nodes;
    std::string XML;
    std::string InvalidNode;
    char* Cache;
    int CacheSize;
};

#endif // _svlWebXMLObject_h

