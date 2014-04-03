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


#ifndef _svlWebFileObject_h
#define _svlWebFileObject_h

#include <cisstCommon.h>
#include <cisstOSAbstraction/osaCriticalSection.h>
#include <cisstStereoVision/svlWebObjectBase.h>

#include <string>

// Always the last cisst include
#include <cisstCommon/cmnExport.h>


class svlWebFileObject : public svlWebObjectBase
{
friend class svlWebPublisher;

private:
    svlWebFileObject(const std::string & name);
    virtual ~svlWebFileObject();
    virtual svlWebFileObject* clone() const;
    
    bool PrepareContents();
    virtual void Read(char*& data, int& datasize);
    
    int FileSize;
    char* Buffer;
    int BufferSize;
};

#endif // _svlWebFileObject_h

