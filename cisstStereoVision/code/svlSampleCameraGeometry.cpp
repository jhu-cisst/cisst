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

#include <cisstStereoVision/svlTypes.h>
#include <sstream>


/*************************************/
/*** svlSampleCameraGeometry class ***/
/*************************************/

CMN_IMPLEMENT_SERVICES(svlSampleCameraGeometry)

svlSampleCameraGeometry::svlSampleCameraGeometry() :
    svlSample()
{
}

svlSampleCameraGeometry::svlSampleCameraGeometry(const svlSampleCameraGeometry & other) :
    svlSample(other)
{
    CopyOf(other);
}

svlSampleCameraGeometry & svlSampleCameraGeometry::operator= (const svlSampleCameraGeometry & other)
{
    CopyOf(other);
    return *this;
}

svlSample* svlSampleCameraGeometry::GetNewInstance() const
{
    return new svlSampleCameraGeometry;
}

svlStreamType svlSampleCameraGeometry::GetType() const
{
    return svlTypeCameraGeometry;
}

int svlSampleCameraGeometry::SetSize(const svlSample* sample)
{
    if (!sample || sample->GetType() != svlTypeCameraGeometry) return SVL_FAIL;
    return SVL_OK;
}

int svlSampleCameraGeometry::SetSize(const svlSample& sample)
{
    if (sample.GetType() != svlTypeCameraGeometry) return SVL_FAIL;
    return SVL_OK;
}

int svlSampleCameraGeometry::CopyOf(const svlSample* sample)
{
    if (!sample || sample->GetType() != svlTypeCameraGeometry) return SVL_FAIL;

    const svlSampleCameraGeometry* sample_camgeo = dynamic_cast<const svlSampleCameraGeometry*>(sample);
    if (!sample_camgeo) return SVL_FAIL;

    IntrinsicVector.SetSize(sample_camgeo->IntrinsicVector.size());
    ExtrinsicVector.SetSize(sample_camgeo->ExtrinsicVector.size());
    IntrinsicVector = sample_camgeo->IntrinsicVector;
    ExtrinsicVector = sample_camgeo->ExtrinsicVector;
    SetTimestamp(sample->GetTimestamp());

    return SVL_OK;
}

int svlSampleCameraGeometry::CopyOf(const svlSample& sample)
{
    if (sample.GetType() != svlTypeCameraGeometry) return SVL_FAIL;

    const svlSampleCameraGeometry* sample_camgeo = dynamic_cast<const svlSampleCameraGeometry*>(&sample);
    if (!sample_camgeo) return SVL_FAIL;

    IntrinsicVector.SetSize(sample_camgeo->IntrinsicVector.size());
    ExtrinsicVector.SetSize(sample_camgeo->ExtrinsicVector.size());
    IntrinsicVector = sample_camgeo->IntrinsicVector;
    ExtrinsicVector = sample_camgeo->ExtrinsicVector;
    SetTimestamp(sample.GetTimestamp());

    return SVL_OK;
}

bool svlSampleCameraGeometry::IsInitialized() const
{
    if (IntrinsicVector.size() > 0 && ExtrinsicVector.size()) return true;
    return false;
}

unsigned char* svlSampleCameraGeometry::GetUCharPointer()
{
    return 0;
}

const unsigned char* svlSampleCameraGeometry::GetUCharPointer() const
{
    return 0;
}

unsigned int svlSampleCameraGeometry::GetDataSize() const
{
    return 0;
}

void svlSampleCameraGeometry::SerializeRaw(std::ostream & outputStream) const
{
    mtsGenericObject::SerializeRaw(outputStream);

    cmnSerializeRaw(outputStream, GetType());
    cmnSerializeRaw(outputStream, GetTimestamp());
    cmnSerializeRaw(outputStream, IntrinsicVector);
    cmnSerializeRaw(outputStream, ExtrinsicVector);
}

void svlSampleCameraGeometry::DeSerializeRaw(std::istream & inputStream)
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
    cmnDeSerializeRaw(inputStream, IntrinsicVector);
    cmnDeSerializeRaw(inputStream, ExtrinsicVector);
}

svlSampleCameraGeometry::svlSampleCameraGeometry(const std::string & calibration_file_path) :
    svlSample()
{
    LoadCalibration(calibration_file_path);
}

unsigned int svlSampleCameraGeometry::GetCameraCount() const
{
    return static_cast<unsigned int>(IntrinsicVector.size());
}

int svlSampleCameraGeometry::LoadCalibration(const std::string & filepath)
{
    // Currently, only stereo calibration files created
    // by the Camera Calibration Toolbox for Matlab are supported
    // http://www.vision.caltech.edu/bouguetj/calib_doc/

    Empty();

    int success = SVL_OK;
    vctDouble2 fc, cc;
    double a;
    vctDouble7 kc;
    vctDouble3 om, T;

    int pos;
    char chstr[2048];
    std::string str;
    std::ifstream fin;

    // reading whole file into string buffer
    fin.exceptions(std::ios_base::eofbit | std::ios_base::failbit | std::ios_base::badbit);
    try {
        fin.open(filepath.c_str());
        memset(chstr, 0, 2048);
        pos = static_cast<int>(fin.read(chstr, 2047).gcount());
    } catch (std::ifstream::failure e) {
    }
    fin.close();

    str.assign(chstr);
    std::istringstream strstrm(str);

    // re-reading file and parsing values
    strstrm.exceptions(std::ios_base::eofbit | std::ios_base::failbit | std::ios_base::badbit);
    try {
        ///////////////////////////
        // LEFT CAMERA INTRINSIC

        // left focal length
        pos = static_cast<int>(str.find("fc_left = [ "));
        if (pos >= 0) {
            strstrm.seekg(pos + 12);
            strstrm >> fc[0] >> fc[1];
        }
        else success = SVL_FAIL;

        // left principal point
        pos = static_cast<int>(str.find("cc_left = [ "));
        if (pos >= 0) {
            strstrm.seekg(pos + 12);
            strstrm >> cc[0] >> cc[1];
        }
        else success = SVL_FAIL;

        // left skew
        pos = static_cast<int>(str.find("alpha_c_left = [ "));
        if (pos >= 0) {
            strstrm.seekg(pos + 17);
            strstrm >> a;
        }
        else success = SVL_FAIL;

        // left radial distortion
        pos = static_cast<int>(str.find("kc_left = [ "));
        if (pos >= 0) {
            strstrm.seekg(pos + 12);
            strstrm >> kc[0] >> kc[1] >> kc[2] >> kc[3] >> kc[4] >> kc[5] >> kc[6];
        }
        else success = SVL_FAIL;

        if (success == SVL_OK) SetIntrinsics(fc, cc, a, kc, SVL_LEFT);

        ///////////////////////////
        // RIGHT CAMERA INTRINSIC

        // right focal length
        pos = static_cast<int>(str.find("fc_right = [ "));
        if (pos >= 0) {
            strstrm.seekg(pos + 13);
            strstrm >> fc[0] >> fc[1];
        }
        else success = SVL_FAIL;

        // right principal point
        pos = static_cast<int>(str.find("cc_right = [ "));
        if (pos >= 0) {
            strstrm.seekg(pos + 13);
            strstrm >> cc[0] >> cc[1];
        }
        else success = SVL_FAIL;

        // right skew
        pos = static_cast<int>(str.find("alpha_c_right = [ "));
        if (pos >= 0) {
            strstrm.seekg(pos + 18);
            strstrm >> a;
        }
        else success = SVL_FAIL;

        // right radial distortion
        pos = static_cast<int>(str.find("kc_right = [ "));
        if (pos >= 0) {
            strstrm.seekg(pos + 13);
            strstrm >> kc[0] >> kc[1] >> kc[2] >> kc[3] >> kc[4] >> kc[5] >> kc[6];
        }
        else success = SVL_FAIL;

        if (success == SVL_OK) SetIntrinsics(fc, cc, a, kc, SVL_RIGHT);

        ///////////////////////////
        // STEREO EXTRINSIC

        // Initialize LEFT extrinsics to zero
        SetExtrinsics(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, SVL_LEFT);

        // rotation between cameras
        pos = static_cast<int>(str.find(" om = [ "));
        if (pos >= 0) {
            strstrm.seekg(pos + 8);
            strstrm >> om[0] >> om[1] >> om[2];
        }
        else success = SVL_FAIL;

        // translation between cameras
        pos = static_cast<int>(str.find(" T = [ "));
        if (pos >= 0) {
            strstrm.seekg(pos + 7);
            strstrm >> T[0] >> T[1] >> T[2];
        }
        else success = SVL_FAIL;

        if (success == SVL_OK) SetExtrinsics(om, T, SVL_RIGHT);
    } catch (std::istringstream::failure e) {
        success = SVL_FAIL;
    }

    return success;
}

void svlSampleCameraGeometry::SetIntrinsics(const svlSampleCameraGeometry::Intrinsics & intrinsics, const unsigned int cam_id)
{
    SetIntrinsics(intrinsics.fc, intrinsics.cc, intrinsics.a, intrinsics.kc, cam_id);
}

void svlSampleCameraGeometry::SetIntrinsics(const vctDouble2 fc, const vctDouble2 cc, const double a, const vctDouble7 kc, const unsigned int cam_id)
{
    SetIntrinsics(fc[0], fc[1], cc[0], cc[1], a, kc[0], kc[1], kc[2], kc[3], kc[4], kc[5], kc[6], cam_id);
}

void svlSampleCameraGeometry::SetIntrinsics(const double fcx, const double fcy,
                                            const double ccx, const double ccy,
                                            const double a,
                                            const double kc0, const double kc1, const double kc2, const double kc3, const double kc4, const double kc5, const double kc6,
                                            const unsigned int cam_id)
{
    if (cam_id >= IntrinsicVector.size()) IntrinsicVector.resize(cam_id + 1);
    IntrinsicVector[cam_id].fc[0] = fcx;
    IntrinsicVector[cam_id].fc[1] = fcy;
    IntrinsicVector[cam_id].cc[0] = ccx;
    IntrinsicVector[cam_id].cc[1] = ccy;
    IntrinsicVector[cam_id].a     = a;
    IntrinsicVector[cam_id].kc[0] = kc0;
    IntrinsicVector[cam_id].kc[1] = kc1;
    IntrinsicVector[cam_id].kc[2] = kc2;
    IntrinsicVector[cam_id].kc[3] = kc3;
    IntrinsicVector[cam_id].kc[4] = kc4;
    IntrinsicVector[cam_id].kc[5] = kc5;
    IntrinsicVector[cam_id].kc[6] = kc6;
    if (cam_id >= ExtrinsicVector.size()) SetExtrinsics(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, cam_id);
}

void svlSampleCameraGeometry::SetExtrinsics(const svlSampleCameraGeometry::Extrinsics & extrinsics, const unsigned int cam_id)
{
    SetExtrinsics(extrinsics.om, extrinsics.T, cam_id);
}

void svlSampleCameraGeometry::SetExtrinsics(const vctDouble3 om, const vctDouble3 T, const unsigned int cam_id)
{
    SetExtrinsics(om[0], om[1], om[2], T[0], T[1], T[2], cam_id);
}

void svlSampleCameraGeometry::SetExtrinsics(const double om0, const double om1, const double om2,
                                            const double T0, const double T1, const double T2,
                                            const unsigned int cam_id)
{
    if (cam_id >= ExtrinsicVector.size()) ExtrinsicVector.resize(cam_id + 1);
    ExtrinsicVector[cam_id].om[0] = om0;
    ExtrinsicVector[cam_id].om[1] = om1;
    ExtrinsicVector[cam_id].om[2] = om2;
    ExtrinsicVector[cam_id].T[0]  = T0;
    ExtrinsicVector[cam_id].T[1]  = T1;
    ExtrinsicVector[cam_id].T[2]  = T2;
    // Provided for convenience
    ExtrinsicVector[cam_id].frame.From(ExtrinsicVector[cam_id].om, ExtrinsicVector[cam_id].T);
}

int svlSampleCameraGeometry::GetIntrinsics(svlSampleCameraGeometry::Intrinsics & intrinsics, const unsigned int cam_id) const
{
    if (cam_id >= IntrinsicVector.size()) return SVL_FAIL;
    intrinsics = IntrinsicVector[cam_id];
    return SVL_OK;
}

svlSampleCameraGeometry::Intrinsics svlSampleCameraGeometry::GetIntrinsics(const unsigned int cam_id) const
{
    Intrinsics intrinsics;
    memset(&intrinsics, 0, sizeof(Intrinsics));
    GetIntrinsics(intrinsics, cam_id);
    return intrinsics;
}

const svlSampleCameraGeometry::Intrinsics* svlSampleCameraGeometry::GetIntrinsicsPtr(const unsigned int cam_id) const
{
    if (cam_id >= IntrinsicVector.size()) return 0;
    return &(IntrinsicVector[cam_id]);
}

int svlSampleCameraGeometry::GetIntrinsics(double& fcx, double& fcy,
                                           double& ccx, double& ccy,
                                           double& a,
                                           double& kc0, double& kc1, double& kc2, double& kc3, double& kc4, double& kc5, double& kc6,
                                           const unsigned int cam_id)
{
    if (cam_id >= IntrinsicVector.size()) return SVL_FAIL;

    fcx = IntrinsicVector[cam_id].fc[0];
    fcy = IntrinsicVector[cam_id].fc[1];
    ccx = IntrinsicVector[cam_id].cc[0];
    ccy = IntrinsicVector[cam_id].cc[1];
    a   = IntrinsicVector[cam_id].a;
    kc0 = IntrinsicVector[cam_id].kc[0];
    kc1 = IntrinsicVector[cam_id].kc[1];
    kc2 = IntrinsicVector[cam_id].kc[2];
    kc3 = IntrinsicVector[cam_id].kc[3];
    kc4 = IntrinsicVector[cam_id].kc[4];
    kc5 = IntrinsicVector[cam_id].kc[5];
    kc6 = IntrinsicVector[cam_id].kc[6];
    
    return SVL_OK;
}

int svlSampleCameraGeometry::GetExtrinsics(svlSampleCameraGeometry::Extrinsics & extrinsics, const unsigned int cam_id) const
{
    if (cam_id >= ExtrinsicVector.size()) return SVL_FAIL;
    extrinsics = ExtrinsicVector[cam_id];
    return SVL_OK;
}

svlSampleCameraGeometry::Extrinsics svlSampleCameraGeometry::GetExtrinsics(const unsigned int cam_id) const
{
    Extrinsics extrinsics;
    memset(&extrinsics, 0, sizeof(Extrinsics));
    GetExtrinsics(extrinsics, cam_id);
    return extrinsics;
}

const svlSampleCameraGeometry::Extrinsics* svlSampleCameraGeometry::GetExtrinsicsPtr(const unsigned int cam_id) const
{
    if (cam_id >= ExtrinsicVector.size()) return 0;
    return &(ExtrinsicVector[cam_id]);
}

int svlSampleCameraGeometry::GetExtrinsics(double& om0, double& om1, double& om2,
                                           double& T0, double& T1, double& T2,
                                           const unsigned int cam_id)
{
    if (cam_id >= ExtrinsicVector.size()) return SVL_FAIL;

    om0 = ExtrinsicVector[cam_id].om[0];
    om1 = ExtrinsicVector[cam_id].om[1];
    om2 = ExtrinsicVector[cam_id].om[2];
    T0  = ExtrinsicVector[cam_id].T[0];
    T1  = ExtrinsicVector[cam_id].T[1];
    T2  = ExtrinsicVector[cam_id].T[2];

    return SVL_OK;
}

void svlSampleCameraGeometry::SetPerspective(const double focallength, const unsigned int width, const unsigned int height, const unsigned int cam_id)
{
    SetIntrinsics(focallength, focallength, width / 2, height / 2, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, cam_id);
}

int svlSampleCameraGeometry::GetPosition(vctDouble3 & position, const unsigned int cam_id) const
{
    if (cam_id >= ExtrinsicVector.size()) return SVL_FAIL;
    position = ExtrinsicVector[cam_id].T;
    return SVL_OK;
}

int svlSampleCameraGeometry::GetAxis(vctDouble3 & axis, const unsigned int cam_id) const
{
    if (cam_id >= ExtrinsicVector.size()) return SVL_FAIL;
    axis[0] = ExtrinsicVector[cam_id].frame.Element(0, 2);
    axis[1] = ExtrinsicVector[cam_id].frame.Element(1, 2);
    axis[2] = ExtrinsicVector[cam_id].frame.Element(2, 2);
    return SVL_OK;
}

int svlSampleCameraGeometry::GetViewUp(vctDouble3 & viewup, const unsigned int cam_id) const
{
    if (cam_id >= ExtrinsicVector.size()) return SVL_FAIL;
    viewup[0] = ExtrinsicVector[cam_id].frame.Element(0, 1);
    viewup[1] = ExtrinsicVector[cam_id].frame.Element(1, 1);
    viewup[2] = ExtrinsicVector[cam_id].frame.Element(2, 1);
    return SVL_OK;
}

int svlSampleCameraGeometry::GetPositionAxisViewUp(vctDouble3 & position, vctDouble3 & axis, vctDouble3 & viewup, const unsigned int cam_id) const
{
    if (cam_id >= ExtrinsicVector.size()) return SVL_FAIL;
    GetPosition(position, cam_id);
    GetAxis(axis, cam_id);
    GetViewUp(viewup, cam_id);
    return SVL_OK;
}

double svlSampleCameraGeometry::GetViewAngleHorizontal(double imagewidth, const unsigned int cam_id) const
{
    if (cam_id >= IntrinsicVector.size()) return -1.0;
    return (atan2(imagewidth / 2.0, IntrinsicVector[cam_id].fc[0]) * 2.0) * 180.0 / 3.1415926535898;
}

double svlSampleCameraGeometry::GetViewAngleVertical(double imageheight, const unsigned int cam_id) const
{
    if (cam_id >= IntrinsicVector.size()) return -1.0;
    return (atan2(imageheight / 2.0, IntrinsicVector[cam_id].fc[1]) * 2.0) * 180.0 / 3.1415926535898;
}

int svlSampleCameraGeometry::IsCameraPerspective(const unsigned int cam_id) const
{
    if (cam_id >= IntrinsicVector.size()) return SVL_FAIL;
    if (IntrinsicVector[cam_id].fc[0] == IntrinsicVector[cam_id].fc[1] && // vertical and horizontal focal lengths are equal
        IntrinsicVector[cam_id].a     == 0.0 &&                       // no skew
        IntrinsicVector[cam_id].kc[0] == 0.0 &&                       // no radial distortion
        IntrinsicVector[cam_id].kc[1] == 0.0 &&
        IntrinsicVector[cam_id].kc[2] == 0.0 &&
        IntrinsicVector[cam_id].kc[3] == 0.0 &&
        IntrinsicVector[cam_id].kc[4] == 0.0 &&
        IntrinsicVector[cam_id].kc[5] == 0.0 &&
        IntrinsicVector[cam_id].kc[6] == 0.0) return SVL_YES;
    return SVL_NO;
}

int svlSampleCameraGeometry::IsCameraPairRectified(const unsigned int cam_id1, const unsigned int cam_id2) const
{
    if (cam_id1 >= IntrinsicVector.size() ||
        cam_id2 >= IntrinsicVector.size() ||
        cam_id1 >= ExtrinsicVector.size() ||
        cam_id2 >= ExtrinsicVector.size() ||
        cam_id1 == cam_id2) return SVL_FAIL;

    // Both needs to be perspective
    if (IsCameraPerspective(cam_id1) != SVL_YES ||
        IsCameraPerspective(cam_id2) != SVL_YES) return SVL_NO;

    // Cameras need to have the same focal length and the same vertical principal point position
    if (IntrinsicVector[cam_id1].fc[0] != IntrinsicVector[cam_id2].fc[0] ||
        IntrinsicVector[cam_id1].cc[1] != IntrinsicVector[cam_id2].cc[1]) return SVL_NO;

    // Currently, supporting only rectified pairs with the left camera being centered
    if (ExtrinsicVector[cam_id1].om[0] == 0.0 &&
        ExtrinsicVector[cam_id1].om[1] == 0.0 &&
        ExtrinsicVector[cam_id1].om[2] == 0.0 &&
        ExtrinsicVector[cam_id1].T[0]  == 0.0 &&
        ExtrinsicVector[cam_id1].T[1]  == 0.0 &&
        ExtrinsicVector[cam_id1].T[2]  == 0.0 &&
        ExtrinsicVector[cam_id2].om[0] == 0.0 &&
        ExtrinsicVector[cam_id2].om[1] == 0.0 &&
        ExtrinsicVector[cam_id2].om[2] == 0.0 &&
        ExtrinsicVector[cam_id2].T[1]  == 0.0 &&
        ExtrinsicVector[cam_id2].T[2]  == 0.0) return SVL_YES;

    return SVL_NO;
}

int svlSampleCameraGeometry::SetWorldToCenter()
{
    unsigned int i;
    const unsigned int camcount = static_cast<unsigned int>(ExtrinsicVector.size());
    if (camcount == 0) return SVL_FAIL;
    if (camcount == 1) return SVL_OK;

    // Compute world to camera transformation
    vctDoubleRodRot3 w2c_om;
    vctDouble3 w2c_T;
    vctDoubleFrm4x4 w2c_frame, temp_frame;
    vctDoubleMatRot3 w2c_R;
    vctDouble3 vec;

    w2c_om.Zeros();
    vec.Zeros();
    for (i = 0; i < camcount; i ++) {
        w2c_om -= ExtrinsicVector[i].om;
        vec -= ExtrinsicVector[i].T;
    }
    w2c_om /= camcount;
    vec /= camcount;

    w2c_R.From(w2c_om);
    w2c_T.ProductOf(w2c_R, vec);
    w2c_frame.From(w2c_R, w2c_T);

    // Apply transformation to cameras
    for (i = 0; i < camcount; i ++) {
        temp_frame.ProductOf(w2c_frame, ExtrinsicVector[i].frame);
        ExtrinsicVector[i].frame = temp_frame;
        w2c_R.FromRaw(temp_frame.Rotation());
        ExtrinsicVector[i].om.From(w2c_R);
        ExtrinsicVector[i].T = temp_frame.Translation();
    }

    return SVL_OK;
}

int svlSampleCameraGeometry::RotateWorldAboutY(double degrees)
{
    const unsigned int camcount = static_cast<unsigned int>(ExtrinsicVector.size());
    if (camcount == 0) return SVL_FAIL;

    // Create axis-angle rotation
    vctDoubleAxAnRot3 aarot(vctDouble3(0.0, 1.0, 0.0), degrees * 3.1415926535898 / 180.0);
    vctDoubleFrm4x4 frame_from_aarot;
    frame_from_aarot.Identity();
    frame_from_aarot.Rotation().From(aarot);

    // Apply transformation to cameras
    vctDoubleFrm4x4 temp_frame;
    vctDoubleMatRot3 w2c_R;
    
    for (unsigned int i = 0; i < camcount; i ++) {
        frame_from_aarot.ApplyTo(ExtrinsicVector[i].frame, temp_frame);
        ExtrinsicVector[i].frame = temp_frame;
        w2c_R.FromRaw(temp_frame.Rotation());
        ExtrinsicVector[i].om.From(w2c_R);
        ExtrinsicVector[i].T = temp_frame.Translation();
    }

    return SVL_OK;
}

int svlSampleCameraGeometry::RotateWorldAboutZ(double degrees)
{
    const unsigned int camcount = static_cast<unsigned int>(ExtrinsicVector.size());
    if (camcount == 0) return SVL_FAIL;

    // Create axis-angle rotation
    vctDoubleAxAnRot3 aarot(vctDouble3(0.0, 0.0, 1.0), degrees * 3.1415926535898 / 180.0);
    vctDoubleFrm4x4 frame_from_aarot;
    frame_from_aarot.Identity();
    frame_from_aarot.Rotation().From(aarot);

    // Apply transformation to cameras
    vctDoubleFrm4x4 temp_frame;
    vctDoubleMatRot3 w2c_R;
    
    for (unsigned int i = 0; i < camcount; i ++) {
        frame_from_aarot.ApplyTo(ExtrinsicVector[i].frame, temp_frame);
        ExtrinsicVector[i].frame = temp_frame;
        w2c_R.FromRaw(temp_frame.Rotation());
        ExtrinsicVector[i].om.From(w2c_R);
        ExtrinsicVector[i].T = temp_frame.Translation();
    }

    return SVL_OK;
}

void svlSampleCameraGeometry::Wrld2Cam(const unsigned int cam_id, vctDouble2 & point2D, const vctDouble3 & point3D)
{
    point2D = Wrld2Cam(cam_id, point3D);
}

vctDouble2 svlSampleCameraGeometry::Wrld2Cam(const unsigned int cam_id, const vctDouble3 & point3D)
{
    vctDouble2 result;
    double z = point3D[2];

    if (z > 0.001) {
        Intrinsics* in = IntrinsicVector.Pointer(cam_id);
        z = in->fc[0] / z;
        result[0] = z * point3D[0] + in->cc[0];
        result[1] = z * point3D[1] + in->cc[1];
    }
    else {
        result[0] = result[1] = -1.0;
    }

    return result;
}

template<class _ValueType>
void svlSampleCameraGeometry::Cam2Wrld(vctFixedSizeVector<_ValueType, 3>& point3D,
                                       const unsigned int cam_id1,
                                       const vctFixedSizeVector<_ValueType, 2>& point2D_1,
                                       const unsigned int cam_id2,
                                       const vctFixedSizeVector<_ValueType, 2>& point2D_2)
{
    Extrinsics* ex2 = ExtrinsicVector.Pointer(cam_id2);
    Intrinsics* in2 = IntrinsicVector.Pointer(cam_id2);
    const _ValueType ppx = static_cast<_ValueType>(in2->cc[0]);
    const _ValueType mindisp = static_cast<_ValueType>(0.01);
    _ValueType disp, ratio;

    double* pfrm = ex2->frame.Pointer();
    const _ValueType axis1 = static_cast<_ValueType>(pfrm[0]);
    const _ValueType axis2 = static_cast<_ValueType>(pfrm[5]);
    const _ValueType axis3 = static_cast<_ValueType>(pfrm[10]);

    disp = (point2D_1.X() - static_cast<_ValueType>(IntrinsicVector[cam_id1].cc[0])) - (point2D_2.X() - ppx);
    if (disp < mindisp) disp = mindisp;

    ratio = static_cast<_ValueType>(ExtrinsicVector[cam_id1].T.X() - ex2->T.X()) / disp;
    if (ratio < 0) ratio = -ratio;

    point3D.X() = axis1 * ratio * (ppx - point2D_2.X());
    point3D.Y() = axis2 * ratio * (static_cast<_ValueType>(in2->cc[1]) - point2D_2.Y());
    point3D.Z() = axis3 * ratio * static_cast<_ValueType>(in2->fc[0]);
}

template void svlSampleCameraGeometry::Cam2Wrld(vctDouble3&, const unsigned int, const vctDouble2&, const unsigned int, const vctDouble2&);
template void svlSampleCameraGeometry::Cam2Wrld(vctFloat3&, const unsigned int, const vctFloat2&, const unsigned int, const vctFloat2&);

template<class _ValueType>
vctFixedSizeVector<_ValueType, 3> svlSampleCameraGeometry::Cam2Wrld(const unsigned int cam_id1,
                                                                    const vctFixedSizeVector<_ValueType, 2>& point2D_1,
                                                                    const unsigned int cam_id2,
                                                                    const vctFixedSizeVector<_ValueType, 2>& point2D_2)
{
    vctFixedSizeVector<_ValueType, 3> point3d;
    Cam2Wrld<_ValueType>(point3d, cam_id1, point2D_1, cam_id2, point2D_2);
    return point3d;
}

template vctDouble3 svlSampleCameraGeometry::Cam2Wrld(const unsigned int, const vctDouble2&, const unsigned int, const vctDouble2&);
template vctFloat3 svlSampleCameraGeometry::Cam2Wrld(const unsigned int, const vctFloat2&, const unsigned int, const vctFloat2&);

void svlSampleCameraGeometry::Empty()
{
    IntrinsicVector.SetSize(0);
    ExtrinsicVector.SetSize(0);
}

/*********************************/
/*** Output Stream definitions ***/
/*********************************/

std::ostream & operator << (std::ostream & stream, const svlSampleCameraGeometry::Intrinsics & objref)
{
    stream.flags(std::ios::fixed | std::ios::right);
    stream.width(6);
    stream << "Focal Length:         fc = [ " << objref.fc[0] << "  " << objref.fc[1] << " ]" << std::endl
           << "Principal point:      cc = [ " << objref.cc[0]  << "  " << objref.cc[1]  << " ]" << std::endl;
    stream.flags(std::ios::showpos | std::ios::fixed | std::ios::right);
    stream.width(7);
    stream << "Skew:            alpha_c = [ " << objref.a  << " ]" << std::endl
           << "Distortion:           kc = [ " << objref.kc[0] << "  " << objref.kc[1] << "  " << objref.kc[2] << "  " << objref.kc[3] << "  " << objref.kc[4] << "  " << objref.kc[5] << "  " << objref.kc[6] <<" |" << std::endl;
    return stream;
}

std::ostream & operator << (std::ostream & stream, const svlSampleCameraGeometry::Extrinsics & objref)
{
    stream.flags(std::ios::showpos | std::ios::fixed | std::ios::right);
    stream.width(7);
    stream << "Rotation vector:      om = [ " << objref.om[0] << "  " << objref.om[1] << "  " << objref.om[2] << " ]" << std::endl
           << "Translation vector:    T = [ " << objref.T[0]  << "  " << objref.T[1]  << "  " << objref.T[2]  << " ]" << std::endl
           << "Frame:                 F = | " << objref.frame.Element(0, 0) << "  " << objref.frame.Element(0, 1) << "  " << objref.frame.Element(0, 2) << "  " << objref.frame.Element(0, 3) << " |" << std::endl
           << "                           | " << objref.frame.Element(1, 0) << "  " << objref.frame.Element(1, 1) << "  " << objref.frame.Element(1, 2) << "  " << objref.frame.Element(1, 3) << " |" << std::endl
           << "                           | " << objref.frame.Element(2, 0) << "  " << objref.frame.Element(2, 1) << "  " << objref.frame.Element(2, 2) << "  " << objref.frame.Element(2, 3) << " |" << std::endl
           << "                           | " << objref.frame.Element(3, 0) << "  " << objref.frame.Element(3, 1) << "  " << objref.frame.Element(3, 2) << "  " << objref.frame.Element(3, 3) << " |" << std::endl;
    return stream;
}

std::ostream & operator << (std::ostream & stream, const svlSampleCameraGeometry & objref)
{
    svlSampleCameraGeometry::Extrinsics extrinsics;
    svlSampleCameraGeometry::Intrinsics intrinsics;
    vctDouble3 position, axis, viewup;
    unsigned int i = 0;
    stream << std::endl;
    while (objref.GetIntrinsics(intrinsics, i) == SVL_OK) {
        stream << "Camera #" << i << " - Intrinsic Parameters:" << std::endl << intrinsics << std::endl;
        i ++;
    }
    i = 0;
    while (objref.GetExtrinsics(extrinsics, i) == SVL_OK) {
        stream << "Camera #" << i << " - Extrinsic Parameters:" << std::endl << extrinsics;
        if (objref.GetPositionAxisViewUp(position, axis, viewup, i) == SVL_OK) {
            stream << "Position:                = [ " << position[0] << "  " << position[1] << "  " << position[2] << " ]" << std::endl
                   << "ViewUp:                  = [ " << viewup[0] << "  " << viewup[1] << "  " << viewup[2] << " ]" << std::endl
                   << "Axis:                    = [ " << axis[0] << "  " << axis[1] << "  " << axis[2] << " ]" << std::endl << std::endl;
        }
        else {
            stream << "Error: invalid parameters" << std::endl << std::endl;
        }
        i ++;
    }
    return stream;
}

