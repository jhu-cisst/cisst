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

#ifndef _svlSample_h
#define _svlSample_h

#include <cisstStereoVision/svlTypes.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlSample : public mtsGenericObject
{
public:
    svlSample();
    svlSample(const svlSample & other);
    svlSample & operator= (const svlSample & other);

    virtual ~svlSample();
    virtual svlSample* GetNewInstance() const = 0;
    virtual svlStreamType GetType() const = 0;
    virtual int SetSize(const svlSample* sample) = 0;
    virtual int SetSize(const svlSample& sample) = 0;
    virtual int CopyOf(const svlSample* sample) = 0;
    virtual int CopyOf(const svlSample& sample) = 0;
    virtual bool IsInitialized() const;
    virtual unsigned char* GetUCharPointer() = 0;
    virtual const unsigned char* GetUCharPointer() const = 0;
    virtual unsigned int GetDataSize() const = 0;
    virtual void SerializeRaw(std::ostream & outputStream) const = 0;
    virtual void DeSerializeRaw(std::istream & inputStream) = 0;

public:
    void SetTimestamp(double ts);
    double GetTimestamp() const;
    static svlSample* GetNewFromType(svlStreamType type);
    void SetEncoder(const std::string & codec, const int parameter);
    void GetEncoder(std::string & codec, int & parameter) const;

private:
    double Timestamp; // [seconds]
    std::string Encoder;
    int EncoderParameter;
};

#endif // _svlSample_h

