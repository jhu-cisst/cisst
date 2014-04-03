/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2010

  (C) Copyright 2006-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision/svlTypes.h>


/***************************/
/*** svlSampleText class ***/
/***************************/

CMN_IMPLEMENT_SERVICES(svlSampleText)

svlSampleText::svlSampleText() :
    svlSample()
{
}

svlSampleText::svlSampleText(const svlSampleText & other) :
    svlSample(other)
{
    CopyOf(other);
}

svlSampleText & svlSampleText::operator= (const svlSampleText & other)
{
    CopyOf(other);
    return *this;
}

svlSample* svlSampleText::GetNewInstance() const
{
    return new svlSampleText;
}

svlStreamType svlSampleText::GetType() const
{
    return svlTypeText;
}

int svlSampleText::SetSize(const svlSample* sample)
{
    const svlSampleText* text = dynamic_cast<const svlSampleText*>(sample);
    if (text == 0) return SVL_FAIL;

    String.resize(text->GetSize());

    return SVL_OK;
}

int svlSampleText::SetSize(const svlSample& sample)
{
    const svlSampleText* text = dynamic_cast<const svlSampleText*>(&sample);
    if (text == 0) return SVL_FAIL;
    
    String.resize(text->GetSize());
    
    return SVL_OK;
}

int svlSampleText::CopyOf(const svlSample* sample)
{
    const svlSampleText* text = dynamic_cast<const svlSampleText*>(sample);
    if (text == 0) return SVL_FAIL;
    
    String.assign(text->GetStringRef());
    
    return SVL_OK;
}

int svlSampleText::CopyOf(const svlSample& sample)
{
    const svlSampleText* text = dynamic_cast<const svlSampleText*>(&sample);
    if (text == 0) return SVL_FAIL;

    String.assign(text->GetStringRef());

    return SVL_OK;
}

bool svlSampleText::IsInitialized() const
{
    return true;
}

unsigned char* svlSampleText::GetUCharPointer()
{
    return reinterpret_cast<unsigned char*>(const_cast<char*>(String.c_str()));
}

const unsigned char* svlSampleText::GetUCharPointer() const
{
    return reinterpret_cast<const unsigned char*>(String.c_str());
}

unsigned int svlSampleText::GetDataSize() const
{
    return static_cast<unsigned int>(String.size());
}

void svlSampleText::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);

    cmnSerializeRaw(outputStream, GetType());
    cmnSerializeRaw(outputStream, GetTimestamp());
    cmnSerializeRaw(outputStream, String);
}

void svlSampleText::DeSerializeRaw(std::istream & inputStream)
{
    mtsGenericObject::DeSerializeRaw(inputStream);

    int type = -1;
    double timestamp;
    cmnDeSerializeRaw(inputStream, type);
    if (type != GetType()) {
        CMN_LOG_CLASS_RUN_ERROR << "Deserialized sample type mismatch " << std::endl;
        return;
    }
    cmnDeSerializeRaw(inputStream, timestamp);
    SetTimestamp(timestamp);
    cmnDeSerializeRaw(inputStream, String);
}

svlSampleText::svlSampleText(const std::string & text) :
    svlSample(),
    String(text)
{
}

void svlSampleText::SetText(const std::string & text)
{
    String = text;
}

std::string & svlSampleText::GetStringRef()
{
    return String;
}

const std::string & svlSampleText::GetStringRef() const
{
    return String;
}

char* svlSampleText::GetCharPointer()
{
    return const_cast<char*>(String.c_str());
}

const char* svlSampleText::GetCharPointer() const
{
    return String.c_str();
}

unsigned int svlSampleText::GetSize() const
{
    return static_cast<unsigned int>(String.size());
}

unsigned int svlSampleText::GetLength() const
{
    return static_cast<unsigned int>(String.size());
}

