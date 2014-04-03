/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2011

  (C) Copyright 2006-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlSampleCameraGeometry_h
#define _svlSampleCameraGeometry_h

#include <cisstStereoVision/svlSample.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstVector/vctTransformationTypes.h>
#include <ostream>

// Always include last!
#include <cisstStereoVision/svlExport.h>


class CISST_EXPORT svlSampleCameraGeometry : public svlSample
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    struct Intrinsics
    {
        friend CISST_EXPORT std::ostream & operator << (std::ostream & stream, const Intrinsics & objref);

        vctDouble2 fc;
        vctDouble2 cc;
        double     a;
        vctDouble7 kc;
    };

    struct Extrinsics
    {
        friend CISST_EXPORT std::ostream & operator << (std::ostream & stream, const Extrinsics & objref);

        vctDoubleRodRot3 om;
        vctDouble3       T;
        vctDoubleFrm4x4  frame;
    };

public:
    friend CISST_EXPORT std::ostream & operator << (std::ostream & stream, const svlSampleCameraGeometry & objref);

    svlSampleCameraGeometry();
    svlSampleCameraGeometry(const svlSampleCameraGeometry & other);
    svlSampleCameraGeometry & operator= (const svlSampleCameraGeometry & other);

    svlSample* GetNewInstance() const;
    svlStreamType GetType() const;
    int SetSize(const svlSample* sample);
    int SetSize(const svlSample& sample);
    int CopyOf(const svlSample* sample);
    int CopyOf(const svlSample& sample);
    bool IsInitialized() const;
    unsigned char* GetUCharPointer();
    const unsigned char* GetUCharPointer() const;
    unsigned int GetDataSize() const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);

    svlSampleCameraGeometry(const std::string & calibration_filepath);

    unsigned int GetCameraCount() const;

    int LoadCalibration(const std::string & filepath);

    void SetIntrinsics(const Intrinsics & intrinsics, const unsigned int cam_id = 0);
    void SetIntrinsics(const vctDouble2 fc, const vctDouble2 cc, const double a, const vctDouble7 kc, const unsigned int cam_id = 0);
    void SetIntrinsics(const double fcx, const double fcy,
                       const double ccx, const double ccy,
                       const double a,
                       const double kc0, const double kc1, const double kc2, const double kc3, const double kc4, const double kc5, const double kc6,
                       const unsigned int cam_id = 0);
    void SetExtrinsics(const Extrinsics & extrinsics, const unsigned int cam_id = 0);
    void SetExtrinsics(const vctDouble3 om, const vctDouble3 T, const unsigned int cam_id = 0);
    void SetExtrinsics(const double om0, const double om1, const double om2,
                       const double T0, const double T1, const double T2,
                       const unsigned int cam_id = 0);

    int GetIntrinsics(Intrinsics & intrinsics, const unsigned int cam_id = 0) const;
    Intrinsics GetIntrinsics(const unsigned int cam_id = 0) const;
    const Intrinsics* GetIntrinsicsPtr(const unsigned int cam_id = 0) const;
    int GetIntrinsics(double& fcx, double& fcy,
                      double& ccx, double& ccy,
                      double& a,
                      double& kc0, double& kc1, double& kc2, double& kc3, double& kc4, double& kc5, double& kc6,
                      const unsigned int cam_id = 0);
    int GetExtrinsics(Extrinsics & extrinsics, const unsigned int cam_id = 0) const;
    Extrinsics GetExtrinsics(const unsigned int cam_id = 0) const;
    const Extrinsics* GetExtrinsicsPtr(const unsigned int cam_id = 0) const;
    int GetExtrinsics(double& om0, double& om1, double& om2,
                      double& T0, double& T1, double& T2,
                      const unsigned int cam_id = 0);

    void SetPerspective(const double focallength, const unsigned int width, const unsigned int height, const unsigned int cam_id = 0);

    int GetPosition(vctDouble3 & position, const unsigned int cam_id = 0) const;
    int GetAxis(vctDouble3 & axis, const unsigned int cam_id = 0) const;
    int GetViewUp(vctDouble3 & viewup, const unsigned int cam_id = 0) const;
    int GetPositionAxisViewUp(vctDouble3 & position, vctDouble3 & axis, vctDouble3 & viewup, const unsigned int cam_id = 0) const;
    double GetViewAngleHorizontal(double imagewidth, const unsigned int cam_id = 0) const;
    double GetViewAngleVertical(double imageheight, const unsigned int cam_id = 0) const;

    int IsCameraPerspective(const unsigned int cam_id) const;
    int IsCameraPairRectified(const unsigned int cam_id1, const unsigned int cam_id2) const;

    int SetWorldToCenter();
    int RotateWorldAboutY(double degrees);
    int RotateWorldAboutZ(double degrees);

    void Wrld2Cam(const unsigned int cam_id, vctDouble2 & point2D, const vctDouble3 & point3D);
    vctDouble2 Wrld2Cam(const unsigned int cam_id, const vctDouble3 & point3D);

    template<class _ValueType>
    void Cam2Wrld(vctFixedSizeVector<_ValueType, 3>& point3D,
                  const unsigned int cam_id1,
                  const vctFixedSizeVector<_ValueType, 2>& point2D_1,
                  const unsigned int cam_id2,
                  const vctFixedSizeVector<_ValueType, 2>& point2D_2);

    template<class _ValueType>
    vctFixedSizeVector<_ValueType, 3> Cam2Wrld(const unsigned int cam_id1,
                                               const vctFixedSizeVector<_ValueType, 2>& point2D_1,
                                               const unsigned int cam_id2,
                                               const vctFixedSizeVector<_ValueType, 2>& point2D_2);

    void Empty();

private:
    vctDynamicVector<Intrinsics> IntrinsicVector;
    vctDynamicVector<Extrinsics> ExtrinsicVector;
};

CMN_DECLARE_SERVICES_INSTANTIATION_EXPORT(svlSampleCameraGeometry)

CISST_EXPORT std::ostream & operator << (std::ostream & stream, const svlSampleCameraGeometry::Intrinsics & objref);
CISST_EXPORT std::ostream & operator << (std::ostream & stream, const svlSampleCameraGeometry::Extrinsics & objref);
CISST_EXPORT std::ostream & operator << (std::ostream & stream, const svlSampleCameraGeometry & objref);

#endif // _svlSampleCameraGeometry_h

