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


#include "cisstStereoVision/svlWebObjectBase.h"


CMN_IMPLEMENT_SERVICES(svlWebObjectBase)

svlWebObjectBase::svlWebObjectBase(const std::string & name) :
    cmnGenericObject(),
    Readable(false),
    Writable(false),
    Name(name),
    Temporary(false)
{
}

svlWebObjectBase::~svlWebObjectBase()
{
}

void svlWebObjectBase::Read(char*& CMN_UNUSED(data), int& CMN_UNUSED(datasize))
{
}

void svlWebObjectBase::Write(char* CMN_UNUSED(data), int CMN_UNUSED(datasize))
{
}

void svlWebObjectBase::Lock()
{
    CS.Enter();
}

void svlWebObjectBase::Unlock()
{
    CS.Leave();
}

const std::string & svlWebObjectBase::GetName()
{
    return Name;
}

void svlWebObjectBase::SetContentType(const std::string & content_type)
{
    ContentType = content_type;
}

const std::string & svlWebObjectBase::GetContentType()
{
    return ContentType;
}

bool svlWebObjectBase::IsReadable()
{
    return Readable;
}

bool svlWebObjectBase::IsWritable()
{
    return Writable;
}

bool svlWebObjectBase::IsTemporary()
{
    return Temporary;
}

