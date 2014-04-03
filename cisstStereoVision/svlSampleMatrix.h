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

#ifndef _svlSampleMatrix_h
#define _svlSampleMatrix_h

#include <cisstStereoVision/svlSample.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

// Forward declarations
class svlSampleImage;


class CISST_EXPORT svlSampleMatrix : public svlSample
{
public:
    svlSampleMatrix();
    svlSampleMatrix(const svlSampleMatrix & other);
    virtual ~svlSampleMatrix();

    virtual svlSample* GetNewInstance() const = 0;
    virtual svlStreamType GetType() const = 0;
    virtual int SetSize(const svlSample* sample) = 0;
    virtual int SetSize(const svlSample& sample) = 0;
    virtual int CopyOf(const svlSample* sample) = 0;
    virtual int CopyOf(const svlSample& sample) = 0;
    virtual bool IsInitialized() const = 0;
    virtual unsigned char* GetUCharPointer() = 0;
    virtual const unsigned char* GetUCharPointer() const = 0;
    virtual unsigned int GetDataSize() const = 0;
    virtual void SerializeRaw(std::ostream & outputStream) const = 0;
    virtual void DeSerializeRaw(std::istream & inputStream) = 0;

    virtual unsigned char* GetUCharPointer(const unsigned int col, const unsigned int row) = 0;
    virtual const unsigned char* GetUCharPointer(const unsigned int col, const unsigned int row) const = 0;
    virtual void SetSize(const unsigned int cols, const unsigned int rows) = 0;
    virtual void Resize(const unsigned int cols, const unsigned int rows) = 0;
    virtual unsigned int GetCols() const = 0;
    virtual unsigned int GetRows() const = 0;
    virtual unsigned int GetColStride() const = 0;
    virtual unsigned int GetRowStride() const = 0;
    virtual const std::string Str(const unsigned int width, const unsigned int precision, const int errorvalue) const = 0;
    virtual const std::string Str(const unsigned int width, const unsigned int precision, const unsigned int errorvalue) const = 0;
    virtual const std::string Str(const unsigned int width, const unsigned int precision, const double errorvalue) const = 0;

    template <class _TypeIn>
    int ImportData(_TypeIn *input, const unsigned int size);
    int ImportMatrix(const svlSampleMatrix* matrix);
    int ImportImage(const svlSampleImage* image);
    int ImportSample(const svlSample* sample);

protected:
    template <class _TypeIn, class _TypeOut>
    int ConvertData(_TypeIn *input, const unsigned int size);
};


#endif // _svlSampleMatrix_h

