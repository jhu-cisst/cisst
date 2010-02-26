/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009

  (C) Copyright 2006-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlCameraGeometry_h
#define _svlCameraGeometry_h

#include <ostream>
#include <cisstVector.h>

#include <cisstStereoVision/svlExport.h>

class CISST_EXPORT svlCameraGeometry
{
public:
    typedef struct _Intrinsics {
        double fc[2];
        double cc[2];
        double a;
        double kc[5];
        friend std::ostream & operator << (std::ostream & stream, const _Intrinsics & objref);
    } Intrinsics;

    typedef struct _Extrinsics {
        vctDoubleRodRot3 om;
        vctDouble3       T;
        vctDoubleFrm4x4  frame;
        friend std::ostream & operator << (std::ostream & stream, const _Extrinsics & objref);
    } Extrinsics;

public:
    svlCameraGeometry();
    virtual ~svlCameraGeometry();

    friend CISST_EXPORT std::ostream & operator << (std::ostream & stream, const svlCameraGeometry & objref);

    void SetIntrinsics(const Intrinsics & intrinsics, const unsigned int cam_id = 0);
    void SetIntrinsics(const double fc[2], const double cc[2], const double a, const double kc[5], const unsigned int cam_id = 0);
    void SetIntrinsics(const double fcx, const double fcy,
                       const double ccx, const double ccy,
                       const double a,
                       const double kc0, const double kc1, const double kc2, const double kc3, const double kc4,
                       const unsigned int cam_id = 0);
    void SetExtrinsics(const Extrinsics & extrinsics, const unsigned int cam_id = 0);
    void SetExtrinsics(const double om[3], const double T[3], const unsigned int cam_id = 0);
    void SetExtrinsics(const double om0, const double om1, const double om2,
                       const double T0, const double T1, const double T2,
                       const unsigned int cam_id = 0);
    void SetPerspective(const double focallength, const unsigned int cam_id = 0);
    int LoadCalibration(const std::string & filepath);

    int GetIntrinsics(Intrinsics & intrinsics, const unsigned int cam_id = 0) const;
    Intrinsics GetIntrinsics(const unsigned int cam_id = 0) const;
    int GetExtrinsics(Extrinsics & extrinsics, const unsigned int cam_id = 0) const;
    Extrinsics GetExtrinsics(const unsigned int cam_id = 0) const;

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

    void Wrld2Cam(const unsigned int cam_id, vctDouble2 & point2D, const vctDouble3 & point3D);
    vctDouble2 Wrld2Cam(const unsigned int cam_id, const vctDouble3 & point3D);

    void Cam2Wrld(vctDouble3 & point3D,
                  const unsigned int cam_id1, const vctDouble2 & point2D_1,
                  const unsigned int cam_id2, const vctDouble2 & point2D_2);
    vctDouble3 Cam2Wrld(const unsigned int cam_id1, const vctDouble2 & point2D_1,
                        const unsigned int cam_id2, const vctDouble2 & point2D_2);

    void Empty();

private:
    vctDynamicVector<Intrinsics> IntrinsicParams;
    vctDynamicVector<Extrinsics> ExtrinsicParams;
};

#endif // _svlCameraGeometry_h

