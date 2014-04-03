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

#ifndef _svlSampleMatrixCustom_h
#define _svlSampleMatrixCustom_h

#include <cisstStereoVision/svlSampleMatrix.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>


template <class _ValueType>
class CISST_EXPORT svlSampleMatrixCustom : public svlSampleMatrix
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:

    //////////////////
    // Constructors //
    //////////////////

    svlSampleMatrixCustom() :
        svlSampleMatrix(),
        OwnData(true),
        InvalidElement(0)
    {
        Matrix = new vctDynamicMatrix<_ValueType>;
    }

    svlSampleMatrixCustom(bool owndata) :
        svlSampleMatrix(),
        OwnData(owndata),
        InvalidElement(0)
    {
        if (OwnData) Matrix = new vctDynamicMatrix<_ValueType>;
        else Matrix = 0;
    }

    svlSampleMatrixCustom(const svlSampleMatrixCustom<_ValueType> & other) :
        svlSampleMatrix(other),
        OwnData(true),
        InvalidElement(0)
    {
        Matrix = new vctDynamicMatrix<_ValueType>;
        CopyOf(other);
    }


    ////////////////
    // Destructor //
    ////////////////

    ~svlSampleMatrixCustom()
    {
        if (OwnData) delete Matrix;
    }


    ///////////////
    // Operators //
    ///////////////

    svlSampleMatrixCustom & operator= (const svlSampleMatrixCustom<_ValueType> & other)
    {
        CopyOf(other);
        return *this;
    }


    //////////////////////////////
    // Inherited from svlSample //
    //////////////////////////////

    svlSample* GetNewInstance() const
    {
        return new svlSampleMatrixCustom<_ValueType>;
    }

    svlStreamType GetType() const
    {
             if (IsTypeInt8  <_ValueType>(static_cast<_ValueType>(0))) return svlTypeMatrixInt8;
        else if (IsTypeInt16 <_ValueType>(static_cast<_ValueType>(0))) return svlTypeMatrixInt16;
        else if (IsTypeInt32 <_ValueType>(static_cast<_ValueType>(0))) return svlTypeMatrixInt32;
        else if (IsTypeInt64 <_ValueType>(static_cast<_ValueType>(0))) return svlTypeMatrixInt64;
        else if (IsTypeUInt8 <_ValueType>(static_cast<_ValueType>(0))) return svlTypeMatrixUInt8;
        else if (IsTypeUInt16<_ValueType>(static_cast<_ValueType>(0))) return svlTypeMatrixUInt16;
        else if (IsTypeUInt32<_ValueType>(static_cast<_ValueType>(0))) return svlTypeMatrixUInt32;
        else if (IsTypeUInt64<_ValueType>(static_cast<_ValueType>(0))) return svlTypeMatrixUInt64;
        else if (IsTypeFloat <_ValueType>(static_cast<_ValueType>(0))) return svlTypeMatrixFloat;
        else if (IsTypeDouble<_ValueType>(static_cast<_ValueType>(0))) return svlTypeMatrixDouble;
        return svlTypeInvalid;
    }

    int SetSize(const svlSample* sample)
    {
        const svlSampleMatrix* samplematrix = dynamic_cast<const svlSampleMatrix*>(sample);
        if (samplematrix) {
            SetSize(samplematrix->GetCols(), samplematrix->GetRows());
            return SVL_OK;
        }
        else {
            const svlSampleImage* sampleimage = dynamic_cast<const svlSampleImage*>(sample);
            if (sampleimage) {
                SetSize(sampleimage->GetWidth(), sampleimage->GetHeight());
                return SVL_OK;
            }
        }
        return SVL_FAIL;
    }

    int SetSize(const svlSample& sample)
    {
        const svlSampleMatrix* samplematrix = dynamic_cast<const svlSampleMatrix*>(&sample);
        if (samplematrix) {
            SetSize(samplematrix->GetCols(), samplematrix->GetRows());
            return SVL_OK;
        }
        else {
            const svlSampleImage* sampleimage = dynamic_cast<const svlSampleImage*>(&sample);
            if (sampleimage) {
                SetSize(sampleimage->GetWidth(), sampleimage->GetHeight());
                return SVL_OK;
            }
        }
        return SVL_FAIL;
    }

    int CopyOf(const svlSample* sample)
    {
        if (!sample) return SVL_FAIL;
        if (sample->GetType() != GetType() || SetSize(sample) != SVL_OK) return SVL_FAIL;

        memcpy(GetUCharPointer(), sample->GetUCharPointer(), GetDataSize());
        SetTimestamp(sample->GetTimestamp());

        return SVL_OK;
    }

    int CopyOf(const svlSample& sample)
    {
        if (sample.GetType() != GetType() || SetSize(sample) != SVL_OK) return SVL_FAIL;

        memcpy(GetUCharPointer(), sample.GetUCharPointer(), GetDataSize());
        SetTimestamp(sample.GetTimestamp());

        return SVL_OK;
    }

    bool IsInitialized() const
    {
        if (Matrix == 0 || Matrix->cols() < 1 || Matrix->rows() < 1) return false;
        return true;
    }

    unsigned char* GetUCharPointer()
    {
        return reinterpret_cast<unsigned char*>(GetPointer());
    }

    const unsigned char* GetUCharPointer() const
    {
        return reinterpret_cast<const unsigned char*>(GetPointer());
    }

    unsigned int GetDataSize() const
    {
        return (GetColStride() * GetCols() * GetRows());
    }

    virtual void SerializeRaw(std::ostream & outputStream) const
    {
        mtsGenericObject::SerializeRaw(outputStream);

        if (IsInitialized()) {
            cmnSerializeRaw(outputStream, GetType());
            cmnSerializeRaw(outputStream, GetTimestamp());
            cmnSerializeRaw(outputStream, *Matrix);
        }
    }

    virtual void DeSerializeRaw(std::istream & inputStream)
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
        cmnDeSerializeRaw(inputStream, *Matrix);
    }


    ///////////////////////////////////
    // Inherited from svlSampleMatrix //
    ///////////////////////////////////

    unsigned char* GetUCharPointer(const unsigned int col, const unsigned int row)
    {
        return reinterpret_cast<unsigned char*>(GetPointer(col, row));
    }

    const unsigned char* GetUCharPointer(const unsigned int col, const unsigned int row) const
    {
        return reinterpret_cast<const unsigned char*>(GetPointer(col, row));
    }

    void SetSize(const unsigned int cols, const unsigned int rows)
    {
        if (Matrix && (GetCols() != cols || GetRows() != rows)) Matrix->SetSize(rows, cols);
    }

    void Resize(const unsigned int cols, const unsigned int rows)
    {
        if (Matrix && (GetCols() != cols || GetRows() != rows)) Matrix->resize(rows, cols);
    }

    unsigned int GetCols() const
    {
        if (Matrix) return static_cast<unsigned int>(Matrix->cols());
        return 0;
    }

    unsigned int GetRows() const
    {
        if (Matrix) return static_cast<unsigned int>(Matrix->rows());
        return 0;
    }

    unsigned int GetColStride() const
    {
        if (Matrix) return sizeof(_ValueType);
        return 0;
    }

    unsigned int GetRowStride() const
    {
        if (Matrix) return static_cast<unsigned int>(Matrix->cols() * sizeof(_ValueType));
        return 0;
    }


    ////////////////////////////////////////////
    // svlSampleMatrixCustom specific methods //
    ////////////////////////////////////////////

    int SetMatrix(vctDynamicMatrix<_ValueType>* matrix)
    {
        if (!OwnData) {
            Matrix = matrix;
            return SVL_OK;
        }
        return SVL_FAIL;
    }

    vctDynamicMatrix<_ValueType> & GetDynamicMatrixRef()
    {
        if (Matrix) return *Matrix;
        return InvalidMatrix;
    }

    const vctDynamicMatrix<_ValueType> & GetDynamicMatrixRef() const
    {
        if (Matrix) return *Matrix;
        return InvalidMatrix;
    }

    _ValueType* GetPointer()
    {
        if (Matrix) return Matrix->Pointer();
        return 0;
    }

    const _ValueType* GetPointer() const
    {
        if (Matrix) return Matrix->Pointer();
        return 0;
    }

    _ValueType* GetPointer(const unsigned int col, const unsigned int row)
    {
        if (Matrix) return Matrix->Pointer(row, col);
        return 0;
    }

    const _ValueType* GetPointer(const unsigned int col, const unsigned int row) const
    {
        if (Matrix) return Matrix->Pointer(row, col);
        return 0;
    }

    _ValueType& Element(const unsigned int col, const unsigned int row)
    {
        if (Matrix) return Matrix->Element(row, col);
        return InvalidElement;
    }

    const _ValueType& Element(const unsigned int col, const unsigned int row) const
    {
        if (Matrix) return Matrix->Element(row, col);
        return InvalidElement;
    }

    const std::string Str(const unsigned int width, const unsigned int precision, const int errorvalue) const
    {
        std::stringstream strstr;
        Str(strstr, width, precision, errorvalue);
        return strstr.str();
    }

    const std::string Str(const unsigned int width, const unsigned int precision, const unsigned int errorvalue) const
    {
        std::stringstream strstr;
        Str(strstr, width, precision, errorvalue);
        return strstr.str();
    }

    const std::string Str(const unsigned int width, const unsigned int precision, const double errorvalue) const
    {
        std::stringstream strstr;
        Str(strstr, width, precision, errorvalue);
        return strstr.str();
    }

    template <class _TypeIn>
    void Str(std::stringstream& strstr, const unsigned int width, const unsigned int precision, _TypeIn errorvalue) const
    {
        // Create error value string
        vctDynamicVector<char> errorstr(width + 1, '.');
        errorstr[width] = 0;

        _ValueType val, error = static_cast<_ValueType>(errorvalue);
        const unsigned int cols = GetCols();
        const unsigned int rows = GetRows();
        unsigned int c, r;

        for (r = 0; r < rows; r ++) {
            strstr << std::fixed << std::setprecision(precision);
            for (c = 0; c < cols; c ++) {
                if (c > 0)  strstr << " ";
                val = Element(c, r);
                if (val != error) strstr << std::setw(width) << val;
                else strstr << errorstr.Pointer();
            }
            strstr << std::endl;
        }
    }

private:
    bool OwnData;
    vctDynamicMatrix<_ValueType>* Matrix;
    vctDynamicMatrix<_ValueType> InvalidMatrix;
    _ValueType InvalidElement;
};

#endif // _svlSampleMatrixCustom_h

