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


/**********************************/
/*** svlSampleTransform3D class ***/
/**********************************/

CMN_IMPLEMENT_SERVICES(svlSampleTransform3D)

svlSampleTransform3D::svlSampleTransform3D() :
    svlSample()
{
}

svlSampleTransform3D::svlSampleTransform3D(const svlSampleTransform3D & other) :
    svlSample(other)
{
    CopyOf(other);
}

svlSampleTransform3D & svlSampleTransform3D::operator= (const svlSampleTransform3D & other)
{
    CopyOf(other);
    return *this;
}

svlSample* svlSampleTransform3D::GetNewInstance() const
{
    return new svlSampleTransform3D;
}

svlStreamType svlSampleTransform3D::GetType() const
{
    return svlTypeTransform3D;
}

int svlSampleTransform3D::SetSize(const svlSample* sample)
{
    if (!sample || sample->GetType() != svlTypeTransform3D) return SVL_FAIL;
    return SVL_OK;
}

int svlSampleTransform3D::SetSize(const svlSample& sample)
{
    if (sample.GetType() != svlTypeTransform3D) return SVL_FAIL;
    return SVL_OK;
}

int svlSampleTransform3D::CopyOf(const svlSample* sample)
{
    if (!sample || sample->GetType() != svlTypeTransform3D) return SVL_FAIL;
    
    const svlSampleTransform3D* samplexform = dynamic_cast<const svlSampleTransform3D*>(sample);
    memcpy(GetUCharPointer(), samplexform->GetUCharPointer(), GetDataSize());
    SetTimestamp(sample->GetTimestamp());
    
    return SVL_OK;
}

int svlSampleTransform3D::CopyOf(const svlSample& sample)
{
    if (sample.GetType() != svlTypeTransform3D) return SVL_FAIL;

    const svlSampleTransform3D* samplexform = dynamic_cast<const svlSampleTransform3D*>(&sample);
    memcpy(GetUCharPointer(), samplexform->GetUCharPointer(), GetDataSize());
    SetTimestamp(sample.GetTimestamp());

    return SVL_OK;
}

bool svlSampleTransform3D::IsInitialized() const
{
    return true;
}

unsigned char* svlSampleTransform3D::GetUCharPointer()
{
    return reinterpret_cast<unsigned char*>(Matrix.Pointer());
}

const unsigned char* svlSampleTransform3D::GetUCharPointer() const
{
    return reinterpret_cast<const unsigned char*>(Matrix.Pointer());
}

unsigned int svlSampleTransform3D::GetDataSize() const
{
    return static_cast<unsigned int>(Matrix.size() * sizeof(double));
}

void svlSampleTransform3D::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);

    cmnSerializeRaw(outputStream, GetType());
    cmnSerializeRaw(outputStream, GetTimestamp());
    cmnSerializeRaw(outputStream, Matrix);
}

void svlSampleTransform3D::DeSerializeRaw(std::istream & inputStream)
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
    cmnDeSerializeRaw(inputStream, Matrix);
}

svlSampleTransform3D::svlSampleTransform3D(const vct4x4 & matrix) :
    svlSample()
{
    Matrix.Assign(matrix);
}

svlSampleTransform3D::svlSampleTransform3D(const vctFrm4x4 & frame) :
    svlSample()
{
    Matrix.Assign(frame);
}

svlSampleTransform3D::svlSampleTransform3D(const vctRot3 & rotation, const vct3 & translation) :
    svlSample()
{
    Matrix.Element(0, 0) = rotation.Element(0, 0);
        Matrix.Element(0, 1) = rotation.Element(0, 1);
            Matrix.Element(0, 2) = rotation.Element(0, 2);
    Matrix.Element(1, 0) = rotation.Element(1, 0);
        Matrix.Element(1, 1) = rotation.Element(1, 1);
            Matrix.Element(1, 2) = rotation.Element(1, 2);
    Matrix.Element(2, 0) = rotation.Element(2, 0);
        Matrix.Element(2, 1) = rotation.Element(2, 1);
            Matrix.Element(2, 2) = rotation.Element(2, 2);
    Matrix.Element(0, 3) = translation.X();
        Matrix.Element(1, 3) = translation.Y();
            Matrix.Element(2, 3) = translation.Z();
    Matrix.Element(3, 0) = 0.0;
        Matrix.Element(3, 1) = 0.0;
            Matrix.Element(3, 2) = 0.0;
                Matrix.Element(3, 3) = 1.0;
}

vct4x4 & svlSampleTransform3D::GetMatrixRef()
{
    return Matrix;
}

const vct4x4 & svlSampleTransform3D::GetMatrixRef() const
{
    return Matrix;
}

vctFrm4x4 svlSampleTransform3D::GetFrame() const
{
    return vctFrm4x4(Matrix);
}

vctRot3 svlSampleTransform3D::GetRotation() const
{
    vctRot3 rotation;

    rotation.Element(0, 0) = Matrix.Element(0, 0);
        rotation.Element(0, 1) = Matrix.Element(0, 1);
            rotation.Element(0, 2) = Matrix.Element(0, 2);
    rotation.Element(1, 0) = Matrix.Element(1, 0);
        rotation.Element(1, 1) = Matrix.Element(1, 1);
            rotation.Element(1, 2) = Matrix.Element(1, 2);
    rotation.Element(2, 0) = Matrix.Element(2, 0);
        rotation.Element(2, 1) = Matrix.Element(2, 1);
            rotation.Element(2, 2) = Matrix.Element(2, 2);

    return rotation;
}

vct3 svlSampleTransform3D::GetTranslation() const
{
    vct3 translation;

    translation.X() = Matrix.Element(0, 3);
        translation.Y() = Matrix.Element(1, 3);
            translation.Z() = Matrix.Element(2, 3);

    return translation;
}

double* svlSampleTransform3D::GetDoublePointer()
{
    return Matrix.Pointer();
}

const double* svlSampleTransform3D::GetDoublePointer() const
{
    return Matrix.Pointer();
}

void svlSampleTransform3D::Identity()
{
    Matrix = vct4x4::Eye();
}

