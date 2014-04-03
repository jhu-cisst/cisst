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


#ifndef _svlWebObjectBase_h
#define _svlWebObjectBase_h

#include <cisstCommon.h>
#include <cisstOSAbstraction/osaCriticalSection.h>
#include <cisstStereoVision/svlWebPublisher.h>

#include <string>

// Always the last cisst include
#include <cisstCommon/cmnExport.h>


class CISST_EXPORT svlWebObjectBase : public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

friend class svlWebPublisher;

public:
    svlWebObjectBase(const std::string & name);
    virtual ~svlWebObjectBase();
    virtual svlWebObjectBase* clone() const = 0;

    virtual void Read(char*& data, int& datasize);
    virtual void Write(char* data, int datasize);
    virtual void Lock();
    virtual void Unlock();
    const std::string & GetName();
    const std::string & GetContentType();
    bool IsReadable();
    bool IsWritable();
    bool IsTemporary();

protected:
    bool Readable;
    bool Writable;

    void SetContentType(const std::string & content_type);

private:
    svlWebObjectBase();

    osaCriticalSection CS;
    std::string Name;
    std::string ContentType;
    bool Temporary;
};

CMN_DECLARE_SERVICES_INSTANTIATION(svlWebObjectBase)

#endif // _svlWebObjectBase_h

