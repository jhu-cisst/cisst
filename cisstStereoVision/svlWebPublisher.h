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


#ifndef _svlWebPublisher_h
#define _svlWebPublisher_h

#include <cisstCommon.h>
#include <cisstOSAbstraction/osaCriticalSection.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaThreadSignal.h>

#include <map>
#include <string>

// Always the last cisst include
#include <cisstCommon/cmnExport.h>


class svlWebObjectBase;

class CISST_EXPORT svlWebPublisher : public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    typedef std::pair<std::string, svlWebObjectBase*> _ObjectPair;
    typedef std::map<std::string, svlWebObjectBase*> _ObjectMap;

public:
    enum RequestType { GET, POST };

    svlWebPublisher(unsigned int port, bool fileserver);
    ~svlWebPublisher();

    unsigned int GetPort();
    bool IsFileServer();

    int Start();
    void Stop();

    int AddObject(svlWebObjectBase* object);
    int RemoveObject(svlWebObjectBase* object);
    int RemoveObject(const std::string & objectname);

    svlWebObjectBase* GetObject(const std::string & objectname, RequestType request);
    void ReleaseObject(svlWebObjectBase* object);

    void* Proc(int param);
private:
    svlWebPublisher();

    _ObjectMap Objects;
    osaCriticalSection CS;
    unsigned int Port;
    bool FileServer;
    RequestType Request;

    osaThread* ServerThread;
    osaThreadSignal ServerSignal;
    bool ServerInitSuccess;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlWebPublisher)

#endif // _svlWebPublisher_h

