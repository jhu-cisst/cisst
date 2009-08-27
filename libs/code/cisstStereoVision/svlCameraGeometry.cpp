/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $
  
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

#include <cisstStereoVision/svlCameraGeometry.h>
#include <cisstStereoVision/svlStreamDefs.h>
#include <iostream>

/*************************************/
/*** svlCameraGeometry class ******/
/*************************************/

svlCameraGeometry::svlCameraGeometry()
{
}

svlCameraGeometry::~svlCameraGeometry()
{
}

void svlCameraGeometry::SetIntrinsics(const svlCameraGeometry::Intrinsics & intrinsics, const unsigned int cam_id)
{
    SetIntrinsics(intrinsics.fc, intrinsics.cc, intrinsics.a, intrinsics.kc, cam_id);
}

void svlCameraGeometry::SetIntrinsics(const double fc[2], const double cc[2], const double a, const double kc[5], const unsigned int cam_id)
{
    SetIntrinsics(fc[0], fc[1], cc[0], cc[1], a, kc[0], kc[1], kc[2], kc[3], kc[4], cam_id);
}

void svlCameraGeometry::SetIntrinsics(const double fcx, const double fcy,
                                         const double ccx, const double ccy,
                                         const double a,
                                         const double kc0, const double kc1, const double kc2, const double kc3, const double kc4,
                                         const unsigned int cam_id)
{
    if (cam_id >= IntrinsicParams.size()) IntrinsicParams.resize(cam_id + 1);
    IntrinsicParams[cam_id].fc[0] = fcx;
    IntrinsicParams[cam_id].fc[1] = fcy;
    IntrinsicParams[cam_id].cc[0] = ccx;
    IntrinsicParams[cam_id].cc[1] = ccy;
    IntrinsicParams[cam_id].a     = a;
    IntrinsicParams[cam_id].kc[0] = kc0;
    IntrinsicParams[cam_id].kc[1] = kc1;
    IntrinsicParams[cam_id].kc[2] = kc2;
    IntrinsicParams[cam_id].kc[3] = kc3;
    IntrinsicParams[cam_id].kc[4] = kc4;
    if (cam_id == 0 && ExtrinsicParams.size() == 0) SetExtrinsics(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0);
}

void svlCameraGeometry::SetExtrinsics(const svlCameraGeometry::Extrinsics & extrinsics, const unsigned int cam_id)
{
    double om[3] = {extrinsics.om[0], extrinsics.om[1], extrinsics.om[2]};
    double T[3]  = {extrinsics.T[0],  extrinsics.T[1],  extrinsics.T[2] };
    SetExtrinsics(om, T, cam_id);
}

void svlCameraGeometry::SetExtrinsics(const double om[3], const double T[3], const unsigned int cam_id)
{
    SetExtrinsics(om[0], om[1], om[2], T[0], T[1], T[2], cam_id);
}

void svlCameraGeometry::SetExtrinsics(const double om0, const double om1, const double om2,
                                         const double T0, const double T1, const double T2,
                                         const unsigned int cam_id)
{
    if (cam_id >= ExtrinsicParams.size()) ExtrinsicParams.resize(cam_id + 1);
    ExtrinsicParams[cam_id].om[0] = om0;
    ExtrinsicParams[cam_id].om[1] = om1;
    ExtrinsicParams[cam_id].om[2] = om2;
    ExtrinsicParams[cam_id].T[0]  = T0;
    ExtrinsicParams[cam_id].T[1]  = T1;
    ExtrinsicParams[cam_id].T[2]  = T2;
    // Provided for convenience
    ExtrinsicParams[cam_id].frame.From(ExtrinsicParams[cam_id].om, ExtrinsicParams[cam_id].T);
}

void svlCameraGeometry::SetPerspective(const double focallength, const unsigned int cam_id)
{
    SetIntrinsics(focallength, focallength, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, cam_id);
}

int svlCameraGeometry::LoadCalibration(const std::string & filepath)
{
    // Currently, only stereo calibration files created
    // by the Camera Calibration Toolbox for Matlab are supported
    // http://www.vision.caltech.edu/bouguetj/calib_doc/

    Empty();

    bool success = true;
    double fc[2], cc[2], a, kc[5], om[3], T[3];

    int pos;
    char chstr[2048];
    std::string str;
    std::ifstream fin1, fin2;

    // reading whole file into string buffer
    fin1.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        fin1.open(filepath.c_str());
        pos = 0;
        memset(chstr, 0, 2048);
        while (1) { // will quit with exception
            pos += fin1.getline(chstr + pos, 2048 - pos).gcount();
            chstr[pos - 1] = ' ';
            pos ++; chstr[pos - 1] = ' '; // fix for CRLF
        }
    } catch (std::ifstream::failure e) {
    }
    fin1.close();

    // re-reading file and parsing values
    fin2.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        fin2.open(filepath.c_str());
        str.assign(chstr);

        ///////////////////////////
        // LEFT CAMERA INTRINSIC

        // left focal length
        pos = str.find("fc_left = [ ");
        if (pos >= 0) {
            fin2.seekg(pos + 12);
            fin2 >> fc[0] >> fc[1];
        }
        else success = false;

        // left principal point
        pos = str.find("cc_left = [ ");
        if (pos >= 0) {
            fin2.seekg(pos + 12);
            fin2 >> cc[0] >> cc[1];
        }
        else success = false;

        // left skew
        pos = str.find("alpha_c_left = [ ");
        if (pos >= 0) {
            fin2.seekg(pos + 17);
            fin2 >> a;
        }
        else success = false;

        // left radial distortion
        pos = str.find("kc_left = [ ");
        if (pos >= 0) {
            fin2.seekg(pos + 12);
            fin2 >> kc[0] >> kc[1] >> kc[2] >> kc[3] >> kc[4];
        }
        else success = false;

        if (success == true) SetIntrinsics(fc, cc, a, kc, SVL_LEFT);

        ///////////////////////////
        // RIGHT CAMERA INTRINSIC

        // right focal length
        pos = str.find("fc_right = [ ");
        if (pos >= 0) {
            fin2.seekg(pos + 13);
            fin2 >> fc[0] >> fc[1];
        }
        else success = false;

        // right principal point
        pos = str.find("cc_right = [ ");
        if (pos >= 0) {
            fin2.seekg(pos + 13);
            fin2 >> cc[0] >> cc[1];
        }
        else success = false;

        // right skew
        pos = str.find("alpha_c_right = [ ");
        if (pos >= 0) {
            fin2.seekg(pos + 18);
            fin2 >> a;
        }
        else success = false;

        // right radial distortion
        pos = str.find("kc_right = [ ");
        if (pos >= 0) {
            fin2.seekg(pos + 13);
            fin2 >> kc[0] >> kc[1] >> kc[2] >> kc[3] >> kc[4];
        }
        else success = false;

        if (success == true) SetIntrinsics(fc, cc, a, kc, SVL_RIGHT);

        ///////////////////////////
        // STEREO EXTRINSIC

        // Initialize LEFT extrinsics to zero
        SetExtrinsics(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, SVL_LEFT);

        // rotation between cameras
        pos = str.find(" om = [ ");
        if (pos >= 0) {
            fin2.seekg(pos + 8);
            fin2 >> om[0] >> om[1] >> om[2];
        }
        else success = false;

        // translation between cameras
        pos = str.find(" T = [ ");
        if (pos >= 0) {
            fin2.seekg(pos + 7);
            fin2 >> T[0] >> T[1] >> T[2];
        }
        else success = false;

        if (success == true) SetExtrinsics(om, T, SVL_RIGHT);
    } catch (std::ifstream::failure e) {
    }
    fin2.close();

    return success;
}

int svlCameraGeometry::GetIntrinsics(svlCameraGeometry::Intrinsics & intrinsics, const unsigned int cam_id)
{
    if (cam_id >= IntrinsicParams.size()) return SVL_FAIL;
    intrinsics = IntrinsicParams[cam_id];
    return SVL_OK;
}

svlCameraGeometry::Intrinsics svlCameraGeometry::GetIntrinsics(const unsigned int cam_id)
{
    Intrinsics intrinsics;
    memset(&intrinsics, 0, sizeof(Intrinsics));
    GetIntrinsics(intrinsics, cam_id);
    return intrinsics;
}

int svlCameraGeometry::GetExtrinsics(svlCameraGeometry::Extrinsics & extrinsics, const unsigned int cam_id)
{
    if (cam_id >= ExtrinsicParams.size()) return SVL_FAIL;
    extrinsics = ExtrinsicParams[cam_id];
    return SVL_OK;
}

svlCameraGeometry::Extrinsics svlCameraGeometry::GetExtrinsics(const unsigned int cam_id)
{
    Extrinsics extrinsics;
    memset(&extrinsics, 0, sizeof(Extrinsics));
    GetExtrinsics(extrinsics, cam_id);
    return extrinsics;
}

int svlCameraGeometry::GetPosition(vctDouble3 & position, const unsigned int cam_id)
{
    if (cam_id >= ExtrinsicParams.size()) return SVL_FAIL;
    position = ExtrinsicParams[cam_id].T;
    return SVL_OK;
}

int svlCameraGeometry::GetAxis(vctDouble3 & axis, const unsigned int cam_id)
{
    if (cam_id >= ExtrinsicParams.size()) return SVL_FAIL;
    axis[0] = ExtrinsicParams[cam_id].frame.Element(0, 2);
    axis[1] = ExtrinsicParams[cam_id].frame.Element(1, 2);
    axis[2] = ExtrinsicParams[cam_id].frame.Element(2, 2);
    return SVL_OK;
}

int svlCameraGeometry::GetViewUp(vctDouble3 & viewup, const unsigned int cam_id)
{
    if (cam_id >= ExtrinsicParams.size()) return SVL_FAIL;
    viewup[0] = ExtrinsicParams[cam_id].frame.Element(0, 1);
    viewup[1] = ExtrinsicParams[cam_id].frame.Element(1, 1);
    viewup[2] = ExtrinsicParams[cam_id].frame.Element(2, 1);
    return SVL_OK;
}

int svlCameraGeometry::GetPositionAxisViewUp(vctDouble3 & position, vctDouble3 & axis, vctDouble3 & viewup, const unsigned int cam_id)
{
    if (cam_id >= ExtrinsicParams.size()) return SVL_FAIL;
    GetPosition(position, cam_id);
    GetAxis(axis, cam_id);
    GetViewUp(viewup, cam_id);
    return SVL_OK;
}

double svlCameraGeometry::GetViewAngleHorizontal(double imagewidth, const unsigned int cam_id)
{
    if (cam_id >= IntrinsicParams.size()) return -1.0;
    return (atan2(imagewidth / 2.0, IntrinsicParams[cam_id].fc[0]) * 2.0) * 180.0 / 3.1415926535898;
}

double svlCameraGeometry::GetViewAngleVertical(double imageheight, const unsigned int cam_id)
{
    if (cam_id >= IntrinsicParams.size()) return -1.0;
    return (atan2(imageheight / 2.0, IntrinsicParams[cam_id].fc[1]) * 2.0) * 180.0 / 3.1415926535898;
}

int svlCameraGeometry::IsCameraPerspective(const unsigned int cam_id)
{
    if (cam_id >= IntrinsicParams.size()) return SVL_FAIL;
    if (IntrinsicParams[cam_id].fc[0] == IntrinsicParams[cam_id].fc[1] && // vertical and horizontal focal lengths are equal
        IntrinsicParams[cam_id].a     == 0.0 &&                       // no skew
        IntrinsicParams[cam_id].kc[0] == 0.0 &&                       // no radial distortion
        IntrinsicParams[cam_id].kc[1] == 0.0 &&
        IntrinsicParams[cam_id].kc[2] == 0.0 &&
        IntrinsicParams[cam_id].kc[3] == 0.0 &&
        IntrinsicParams[cam_id].kc[4] == 0.0) return SVL_YES;
    return SVL_NO;
}

int svlCameraGeometry::IsCameraPairRectified(const unsigned int cam_id1, const unsigned int cam_id2)
{
    if (cam_id1 >= IntrinsicParams.size() ||
        cam_id2 >= IntrinsicParams.size() ||
        cam_id1 >= ExtrinsicParams.size() ||
        cam_id2 >= ExtrinsicParams.size() ||
        cam_id1 == cam_id2) return SVL_FAIL;

    // Both needs to be perspective
    if (IsCameraPerspective(cam_id1) != SVL_YES ||
        IsCameraPerspective(cam_id2) != SVL_YES) return SVL_NO;

    // Cameras need to have the same focal length and the same vertical principal point position
    if (IntrinsicParams[cam_id1].fc[0] != IntrinsicParams[cam_id2].fc[0] ||
        IntrinsicParams[cam_id1].cc[1] != IntrinsicParams[cam_id2].cc[1]) return SVL_NO;

    // Currently, supporting only rectified pairs with the left camera being centered
    if (ExtrinsicParams[cam_id1].om[0] == 0.0 &&
        ExtrinsicParams[cam_id1].om[1] == 0.0 &&
        ExtrinsicParams[cam_id1].om[2] == 0.0 &&
        ExtrinsicParams[cam_id1].T[0]  == 0.0 &&
        ExtrinsicParams[cam_id1].T[1]  == 0.0 &&
        ExtrinsicParams[cam_id1].T[2]  == 0.0 &&
        ExtrinsicParams[cam_id2].om[0] == 0.0 &&
        ExtrinsicParams[cam_id2].om[1] == 0.0 &&
        ExtrinsicParams[cam_id2].om[2] == 0.0 &&
        ExtrinsicParams[cam_id2].T[1]  == 0.0 &&
        ExtrinsicParams[cam_id2].T[2]  == 0.0) return SVL_YES;

    return SVL_NO;
}

int svlCameraGeometry::SetWorldToCenter()
{
    unsigned int i;
    const unsigned int camcount = ExtrinsicParams.size();
    if (camcount == 0) return SVL_FAIL;
    if (camcount == 1) return SVL_OK;

    // Compute world to camera transformation
    vctDoubleRodRot3 w2c_om;
    vctDouble3 w2c_T;
    vctDoubleFrm4x4 w2c_frame, temp_frame;
    vctDoubleRot3 w2c_R;
    vctDouble3 vec;

    w2c_om.Zeros();
    vec.Zeros();
    for (i = 0; i < camcount; i ++) {
        w2c_om -= ExtrinsicParams[i].om;
        vec -= ExtrinsicParams[i].T;
    }
    w2c_om /= camcount;
    vec /= camcount;

    w2c_R.From(w2c_om);
    w2c_T.ProductOf(w2c_R, vec);
    w2c_frame.From(w2c_R, w2c_T);

    // Apply transformation to cameras
    for (i = 0; i < camcount; i ++) {
        ExtrinsicParams[i].om += w2c_om;
        ExtrinsicParams[i].T += w2c_T;
        temp_frame.ProductOf(w2c_frame, ExtrinsicParams[i].frame);
        ExtrinsicParams[i].frame = temp_frame;
    }

    return SVL_OK;
}

void svlCameraGeometry::Wrld2Cam(const unsigned int cam_id, vctDouble2 & point2D, const vctDouble3 & point3D)
{
    point2D = Wrld2Cam(cam_id, point3D);
}

vctDouble2 svlCameraGeometry::Wrld2Cam(const unsigned int cam_id, const vctDouble3 & point3D)
{
    vctDouble2 result;
    result.SetAll(0.0);
    return result;
}

void svlCameraGeometry::Cam2Wrld(vctDouble3 & point3D,
                                 const unsigned int cam_id1, const vctDouble2 & point2D_1,
                                 const unsigned int cam_id2, const vctDouble2 & point2D_2)
{
    point3D = Cam2Wrld(cam_id1, point2D_1, cam_id2, point2D_2);
}

vctDouble3 svlCameraGeometry::Cam2Wrld(const unsigned int cam_id1, const vctDouble2 & point2D_1,
                                       const unsigned int cam_id2, const vctDouble2 & point2D_2)
{
    vctDouble3 result;
    result.SetAll(0.0);
    return result;
}

void svlCameraGeometry::Empty()
{
    IntrinsicParams.SetSize(0);
    ExtrinsicParams.SetSize(0);
}
