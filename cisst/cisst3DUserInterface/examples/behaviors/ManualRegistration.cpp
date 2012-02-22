/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
$Id$

Author(s):  Anton Deguet, Simon DiMaio
Created on: 2009-09-13

(C) Copyright 2009 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstParameterTypes/prmPositionCartesianSet.h>
#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisst3DUserInterface/ui3Widget3D.h>
#include <cisst3DUserInterface/ui3Manager.h>
#include <cisst3DUserInterface/ui3SlaveArm.h> // bad, ui3 should not have slave arm to start with (adeguet1)

#include "ManualRegistration.h"

#define CUBE_DEMO 1
#define IMPORT_VIRTUAL_FIDUCIALS 1

class ManualRegistrationSurfaceVisibleStippleObject: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    enum GeometryType {NONE=0,CUBE=1,SPHERE=2};
    inline ManualRegistrationSurfaceVisibleStippleObject(const std::string & inputFile, const GeometryType & geometry=NONE, bool hasOutline = false):
        ui3VisibleObject(),
        Visible(true),
        InputFile(inputFile),
        SurfaceReader(0),
        SurfaceMapper(0),
        SurfaceActor(0),
        HasOutline(hasOutline),
        OutlineData(0),
        OutlineMapper(0),
        OutlineActor(0),
        Geometry(geometry)
    {
    }

    inline ~ManualRegistrationSurfaceVisibleStippleObject()
    {
        if (this->SurfaceActor) {
            this->SurfaceActor->Delete();
            this->SurfaceActor = 0;
        }
        if (this->SurfaceMapper) {
            this->SurfaceMapper->Delete();
            this->SurfaceMapper = 0;
        }
        if (this->SurfaceReader) {
            this->SurfaceReader->Delete();
            this->SurfaceReader = 0;
        }
        if (this->OutlineData) {
            this->OutlineData->Delete();
            this->OutlineData = 0;
        }
        if (this->OutlineMapper) {
            this->OutlineMapper->Delete();
            this->OutlineMapper = 0;
        }
        if (this->OutlineActor) {
            this->OutlineActor->Delete();
            this->OutlineActor = 0;
        }
    }

    inline bool CreateVTKObjectCube()
    {
        vtkCubeSource *source = vtkCubeSource::New();
        source->SetBounds(-25,25,-25,25,-25,25);
        SurfaceMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(SurfaceMapper);
        SurfaceMapper->SetInputConnection(source->GetOutputPort());
        SurfaceMapper->SetScalarRange(0,7);
        SurfaceMapper->ScalarVisibilityOff();
        SurfaceMapper->ImmediateModeRenderingOn();
        SurfaceActor = ui3VTKStippleActor::New();
        CMN_ASSERT(SurfaceActor);
        SurfaceActor->SetMapper(SurfaceMapper);

        // Add the actor
        this->AddPart(this->SurfaceActor);
        return true;
    }

    inline bool CreateVTKObjectSphere(void) {
        vtkSphereSource *source = vtkSphereSource::New();
        CMN_ASSERT(source);
        source->SetRadius(5.0);

        vtkTextActor3D *textActor = vtkTextActor3D::New();
        textActor->GetTextProperty()->SetFontSize(5);
        textActor->SetInput(InputFile.c_str());

        SurfaceMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(SurfaceMapper);
        SurfaceMapper->SetInputConnection(source->GetOutputPort());
        SurfaceMapper->ImmediateModeRenderingOn();

        SurfaceActor = ui3VTKStippleActor::New();
        CMN_ASSERT(SurfaceActor);
        SurfaceActor->SetMapper(SurfaceMapper);

        // Add the actor
        this->AddPart(SurfaceActor);
        this->AddPart(textActor);
        return true;
    }

    inline bool CreateVTKObjectFromFile()
    {
        SurfaceReader = vtkPolyDataReader::New();
        CMN_ASSERT(SurfaceReader);
        CMN_LOG_CLASS_INIT_VERBOSE << "Loading file \"" << InputFile << "\"" << std::endl;
        SurfaceReader->SetFileName(InputFile.c_str());
        CMN_LOG_CLASS_INIT_VERBOSE << "File \"" << InputFile << "\" loaded" << std::endl;
        SurfaceReader->Update();

        SurfaceMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(SurfaceMapper);
        SurfaceMapper->SetInputConnection(SurfaceReader->GetOutputPort());
        SurfaceMapper->ScalarVisibilityOff();
        SurfaceMapper->ImmediateModeRenderingOn();

        SurfaceActor = ui3VTKStippleActor::New();
        CMN_ASSERT(SurfaceActor);
        SurfaceActor->SetMapper(SurfaceMapper);
        // SurfaceActor->GetProperty()->SetSpecular(.3);
        // SurfaceActor->GetProperty()->SetSpecularPower(20);
        // Create a frame for the data volume.
        if (HasOutline) {
            OutlineData = vtkOutlineFilter::New();
            CMN_ASSERT(OutlineData);
            OutlineData->SetInputConnection(SurfaceReader->GetOutputPort());
            OutlineMapper = vtkPolyDataMapper::New();
            CMN_ASSERT(OutlineMapper);
            OutlineMapper->SetInputConnection(OutlineData->GetOutputPort());
            OutlineMapper->ImmediateModeRenderingOn();
            OutlineActor = vtkActor::New();
            CMN_ASSERT(OutlineActor);
            OutlineActor->SetMapper(OutlineMapper);
            OutlineActor->GetProperty()->SetColor(1,1,1);

            // Scale the actors.
            //OutlineActor->SetScale(0.05);
            this->AddPart(this->OutlineActor);
        }

        // Set Opacity/Transparency with vtk
        //SurfaceActor->GetProperty()->SetOpacity(1.0);

        // Scale the actors.
        //SurfaceActor->SetScale(0.10);

        // Add the actor
        this->AddPart(this->SurfaceActor);
        return true;
    }

    inline bool CreateVTKObjects(void) {

        // Create surface actor/mapper
        switch (Geometry){
        case CUBE:
            return CreateVTKObjectCube();
            break;
        case SPHERE:
            return CreateVTKObjectSphere();
            break;
        default:
            return CreateVTKObjectFromFile();
            break;
        }

        return true;
    }

    inline bool UpdateVTKObjects(void) {
        return true;
    }

    inline void SetColor(double r, double g, double b) {
        SurfaceActor->GetProperty()->SetDiffuseColor(r, g, b);
    }

    inline void SetOpacity(double opacity) {
        SurfaceActor->GetProperty()->SetOpacity(opacity);
    }

    inline void SetStipplePattern(int mode) {
        while (!this->Created()) {
            osaSleep(0.1 * cmn_s);
        }
        SurfaceActor->SetStipplePattern(mode);
    }

    inline void SetStipplePercentage(int percentage) {
        while (!this->Created()) {
            osaSleep(0.1 * cmn_s);
        }
        SurfaceActor->SetStipplePercentage(percentage);
    }

    vctDouble3 GetCenter(void) {
        vctDouble3 center;
        if (HasOutline) {
            center.Assign(OutlineActor->GetCenter());
        }
        return center;
    }

    vctFrm3 PositionECMRCM;//warning: this is not always updated
    bool Visible;
    vctFrm3 HomePositionUI3;
    typedef std::map<int,vctFrm3> vctFrm3MapType;
    int PreviousIndex;
    vctFrm3MapType PreviousPositions;

protected:
    std::string InputFile;
    vtkPolyDataReader * SurfaceReader;
    vtkPolyDataMapper * SurfaceMapper;
    ui3VTKStippleActor * SurfaceActor;
    bool HasOutline;
    vtkOutlineFilter * OutlineData;
    vtkPolyDataMapper * OutlineMapper;
    vtkActor  * OutlineActor;

    GeometryType Geometry;
};

CMN_DECLARE_SERVICES_INSTANTIATION(ManualRegistrationSurfaceVisibleStippleObject);
CMN_IMPLEMENT_SERVICES(ManualRegistrationSurfaceVisibleStippleObject);

ManualRegistration::ManualRegistration(const std::string & name):
    ui3BehaviorBase(std::string("ManualRegistration::") + name, 0),
    VisibleList(0),
    VisibleListVirtual(0),
    VisibleListReal(0)
{
    //Rotate by pi in y to be aligned to console
    ECMtoUI3.Rotation().From(vctAxAnRot3(vctDouble3(0.0,1.0,0.0), cmnPI));
    UI3toECM = ECMtoUI3.Inverse();
    VisibleList = new ui3VisibleList("ManualRegistration");
    VisibleListVirtual = new ui3VisibleList("ManualRegistrationVirtual");
    VisibleListReal = new ui3VisibleList("ManualRegistrationReal");

    ManualRegistrationSurfaceVisibleStippleObject * model;
#if CUBE_DEMO
    model = new ManualRegistrationSurfaceVisibleStippleObject("",ManualRegistrationSurfaceVisibleStippleObject::CUBE);
#else
    model = new ManualRegistrationSurfaceVisibleStippleObject("E:/Users/wliu25/MyCommon/data/TORS_tongue.vtk");
#endif
    VisibleObjects[MODEL] = model;

    for (ManualRegistrationType::iterator iter = VisibleObjects.begin();
         iter != VisibleObjects.end();
         iter++) {
        VisibleListVirtual->Add(iter->second);
    }

    VisibleList->Add(VisibleListVirtual);
    VisibleList->Add(VisibleListReal);

    // Initialize all flags to false
    this->BooleanFlags[DEBUG] = true;
    this->BooleanFlags[VISIBLE] = true;
    this->BooleanFlags[PREVIOUS_MAM] = false;
    this->BooleanFlags[RIGHT_BUTTON] = false;
    this->BooleanFlags[LEFT_BUTTON] = false;
    this->BooleanFlags[CAMERA_PRESSED] = false;
    this->BooleanFlags[BOTH_BUTTON_PRESSED] = false;
    this->BooleanFlags[ADD_FIDUCIALS] = false;
    this->BooleanFlags[RIGHT_BUTTON_RELEASED] = false;
    this->BooleanFlags[LEFT_BUTTON_RELEASED] = false;
}


ManualRegistration::~ManualRegistration()
{
}


void ManualRegistration::PositionDepth(void)
{
    ManualRegistrationType::iterator foundModel;
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel == VisibleObjects.end()) {
        return;
    }

    std::cout << "PositionDepth" << std::endl;
    // compute depth of model
    vctFrm3 positionUI3 = ECMtoUI3 * ECMtoECMRCM.Inverse() * (foundModel->second)->PositionECMRCM;
    prmPositionCartesianGet currentPosition;
    prmPositionCartesianSet newPosition;

    mtsExecutionResult result;
    result = GetPrimaryMasterPosition(currentPosition);
    //if (!result.IsOK()) {
    std::cerr << "PositionDepth, GetPrimaryMasterPosition: " << result << std::endl;
    //}
    newPosition.Goal().Assign(currentPosition.Position());
    newPosition.Goal().Translation().Z() = positionUI3.Translation().Z();
    result = SetPrimaryMasterPosition(newPosition);
    //if (!result.IsOK()) {
    std::cerr << "PositionDepth, SetPrimaryMasterPosition: " << result << std::endl;
    //}

    result = GetSecondaryMasterPosition(currentPosition);
    if (!result.IsOK()) {
        std::cerr << "PositionDepth, GetPrimaryMasterPosition: " << result << std::endl;
    }
    newPosition.Goal().Assign(currentPosition.Position());
    newPosition.Goal().Translation().Z() = positionUI3.Translation().Z();
    result = SetSecondaryMasterPosition(newPosition);
    if (!result.IsOK()) {
        std::cerr << "PositionDepth, SetPrimaryMasterPosition: " << result << std::endl;
    }
}

void ManualRegistration::UpdatePreviousPosition()
{
    // get current position in UI3
    vctFrm3 previousPositionUI3;
    ManualRegistrationType::iterator foundModel;
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel == VisibleObjects.end()) {
        return;
    }
    previousPositionUI3 = ECMtoUI3 * ECMtoECMRCM.Inverse() * (foundModel->second)->PositionECMRCM;
    (foundModel->second)->PreviousPositions[(foundModel->second)->PreviousPositions.size()+1] = previousPositionUI3;
    (foundModel->second)->PreviousIndex = (foundModel->second)->PreviousPositions.size();
    if (this->BooleanFlags[DEBUG]) {
        std::cout << "Previous Index:" << (foundModel->second)->PreviousIndex << std::endl;
    }
}

void ManualRegistration::PositionBack(void)
{
    ManualRegistrationType::iterator foundModel;
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel == VisibleObjects.end())
        return;
    ManualRegistrationSurfaceVisibleStippleObject::vctFrm3MapType::iterator foundPosition;
    foundPosition = (foundModel->second)->PreviousPositions.find((foundModel->second)->PreviousIndex);
    if (foundPosition != (foundModel->second)->PreviousPositions.end()) {
        UpdateECMtoECMRCM();
        foundModel->second->PositionECMRCM = ECMtoECMRCM * UI3toECM * (foundModel->second)->PreviousPositions[(foundModel->second)->PreviousIndex];
        UpdateVisibleList(false);
        if (this->BooleanFlags[DEBUG]) {
            std::cout << "Setting back to index:" << (foundModel->second)->PreviousIndex << std::endl;
        }
        (foundModel->second)->PreviousIndex--;
        if ((foundModel->second)->PreviousIndex < 0) {
            (foundModel->second)->PreviousIndex = 0;
        }
    }
    UpdateVisibleList(false);
}


void ManualRegistration::PositionHome(void)
{
    UpdateECMtoECMRCM();
    ManualRegistrationType::iterator foundModel;
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel == VisibleObjects.end()) {
        return;
    }
    (foundModel->second)->PositionECMRCM = ECMtoECMRCM * UI3toECM * (foundModel->second)->HomePositionUI3;
    UpdateVisibleList(false);
}

void ManualRegistration::ToggleAddFiducials()
{
    if (this->BooleanFlags[ADD_FIDUCIALS]) {
        if (this->BooleanFlags[DEBUG]) {
            std::cerr << "Toggle Adding fiducials: Off" << std::endl;
        }
        this->BooleanFlags[ADD_FIDUCIALS] = false;
    }
    else {
        if (this->BooleanFlags[DEBUG]) {
            std::cerr << "Toggle Adding fiducials: On" << std::endl;
        }
        this->BooleanFlags[ADD_FIDUCIALS] = true;
    }
}

void ManualRegistration::ToggleVisibility(void)
{
    if (this->BooleanFlags[VISIBLE]) {
        this->BooleanFlags[VISIBLE] = false;
        std::cout << "Toggling Visible - OFF" << std::endl;
    } else {
        this->BooleanFlags[VISIBLE] = true;
        std::cout << "Toggling Visible - ON" << std::endl;
    }
    UpdateVisibleList(false);
}


void ManualRegistration::ConfigureMenuBar(void)
{
    this->MenuBar->AddClickButton("PositionBack",
                                  0,
                                  "undo.png",
                                  &ManualRegistration::PositionBack,
                                  this);
    this->MenuBar->AddClickButton("PositionHome",
                                  1,
                                  "triangle.png",
                                  &ManualRegistration::PositionHome,
                                  this);
    this->MenuBar->AddClickButton("ToggleAddFiducials",
                                  2,
                                  "map.png",
                                  &ManualRegistration::ToggleAddFiducials,
                                  this);
    this->MenuBar->AddClickButton("Register",
                                  3,
                                  "move.png",
                                  &ManualRegistration::Register,
                                  this);
    this->MenuBar->AddClickButton("ToggleVisibility",
                                  4,
                                  "sphere.png",
                                  &ManualRegistration::ToggleVisibility,
                                  this);
    this->MenuBar->AddClickButton("PositionDepth",
                                  5,
                                  "iconify-top-left.png",
                                  &ManualRegistration::PositionDepth,
                                  this);
}


bool ManualRegistration::RunForeground(void)
{
    if (this->Manager->MastersAsMice() != this->BooleanFlags[PREVIOUS_MAM]) {
        this->BooleanFlags[PREVIOUS_MAM] = this->Manager->MastersAsMice();
        this->BooleanFlags[BOTH_BUTTON_PRESSED] = false;
        this->BooleanFlags[LEFT_BUTTON] = false;
        this->BooleanFlags[RIGHT_BUTTON] = false;
        this->BooleanFlags[LEFT_BUTTON_RELEASED] = false;
        this->BooleanFlags[RIGHT_BUTTON_RELEASED] = false;
    }

    // detect transition, should that be handled as an event?
    // State is used by multiple threads ...
    if (this->State != this->PreviousState) {
        std::cerr << "Entering RunForeground" << std::endl;
        this->PreviousState = this->State;
        this->BooleanFlags[LEFT_BUTTON_RELEASED] = false;
        this->BooleanFlags[RIGHT_BUTTON_RELEASED] = false;
    }

    // detect active mice
    prmPositionCartesianGet positionLeft, positionRight;
    this->GetPrimaryMasterPosition(positionRight);
    this->GetSecondaryMasterPosition(positionLeft);

    if (this->BooleanFlags[ADD_FIDUCIALS]) {
        if (this->BooleanFlags[LEFT_BUTTON] && this->BooleanFlags[LEFT_BUTTON_RELEASED])
            AddFiducial(positionLeft.Position(),false);
        else if (this->BooleanFlags[RIGHT_BUTTON] && this->BooleanFlags[RIGHT_BUTTON_RELEASED])
            AddFiducial(positionRight.Position(),true);
    } else if (this->BooleanFlags[LEFT_BUTTON] || this->BooleanFlags[RIGHT_BUTTON]) {
        FollowMaster();
    }

    return true;
}


void ManualRegistration::FollowMaster(void)
{
    prmPositionCartesianGet positionLeft, positionRight;
    vctFrm3 displacementUI3, displacementUI3T, displacementUI3R;
    vctFrm3 currentTransformation;
    ManualRegistrationType::iterator foundModel;
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel == VisibleObjects.end()) {
        return;
    }

    this->GetPrimaryMasterPosition(positionRight);
    this->GetSecondaryMasterPosition(positionLeft);

    // get current position in UI3
    vctFrm3 positionUI3, positionUI3New;
    positionUI3 = ECMtoUI3 * ECMtoECMRCM.Inverse() * (foundModel->second)->PositionECMRCM;

    //vtk picking NOT USED
    //Manager->RequestPick(&PickSignal, 0, "RightEyeView", positionRight.Position().Translation());

    if (!this->BooleanFlags[BOTH_BUTTON_PRESSED]
        && this->BooleanFlags[RIGHT_BUTTON]
        && !this->BooleanFlags[LEFT_BUTTON]) {
        //translation only using right
        displacementUI3.Translation() = positionRight.Position().Translation() - InitialMasterRight;
    }
    else if (!this->BooleanFlags[BOTH_BUTTON_PRESSED]
             && !this->BooleanFlags[RIGHT_BUTTON]
             && this->BooleanFlags[LEFT_BUTTON]) {
        //translation only using left
        displacementUI3.Translation() = positionLeft.Position().Translation() - InitialMasterLeft;
    } else if (this->BooleanFlags[BOTH_BUTTON_PRESSED]) {
        //rotation using both
        vctDouble3 axis;
        axis.Assign(1.0,0.0,0.0);
        double angle;
        angle = 0.0;
        double object_displacement[3], object_rotation[4];
        vctDouble3 translation, translationInWorld;
        vctDouble3 axisInWorld;

        vctFrm3 modelToHandleCenter;
        modelToHandleCenter.Translation().SumOf(InitialMasterRight,InitialMasterLeft);
        modelToHandleCenter.Translation().Divide(2.0);
        modelToHandleCenter.Translation().Subtract(positionUI3.Translation());

        ComputeTransform(InitialMasterRight.Pointer(),
                         InitialMasterLeft.Pointer(),
                         positionRight.Position().Translation().Pointer(),
                         positionLeft.Position().Translation().Pointer(),
                         object_displacement, object_rotation);

        // Set the Translation.
        translation.Assign(object_displacement);
        positionUI3.Rotation().ApplyInverseTo(translation, translationInWorld);
        displacementUI3T.Translation()= translationInWorld;

        // Set the Rotation.
        angle = object_rotation[0];
        axis.Assign(object_rotation+1);
        positionUI3.Rotation().ApplyInverseTo(axis, axisInWorld);
        displacementUI3R.Rotation().From(vctAxAnRot3(axisInWorld, angle));

        displacementUI3 = displacementUI3T * modelToHandleCenter.Inverse() * displacementUI3R * modelToHandleCenter;

    }
    // apply delta in UI3
    positionUI3New = positionUI3 * displacementUI3;

    // go back to ECMRCM
    (foundModel->second)->PositionECMRCM = ECMtoECMRCM * UI3toECM * positionUI3New;

    InitialMasterRight = positionRight.Position().Translation();
    InitialMasterLeft = positionLeft.Position().Translation();

    // this will transform to UI3
    UpdateVisibleList();

}


/*!
  Compute the object transform from the motion of two grabbed control points.
  @param pointa               Right control position.
  @param pointb               Left control position.
  @param point1               Right cursor pos.
  @param point2               Left cursor pos.
  @param object_displacement  [dx, dy, dz]
  @param object_rotation      [angle, axis_x, axis_y, axis_z]
*/
void ManualRegistration::ComputeTransform(double pointa[3], double pointb[3],
                                          double point1[3], double point2[3],
                                          double object_displacement[3],
                                          double object_rotation[4])
{
    double v1[3], v2[3], v1norm, v2norm, wnorm;
    double w[3], angle, dotarg;

    //cout << "pointa: " << pointa[0] << " " << pointa[1] << " " << pointa[2] << endl;
    //cout << "pointb: " << pointb[0] << " " << pointb[1] << " " << pointb[2] << endl;

    // v1 = ((pb-pa)/norm(pb-pa))
    v1[0] = pointb[0]-pointa[0];
    v1[1] = pointb[1]-pointa[1];
    v1[2] = pointb[2]-pointa[2];
    v1norm = sqrt(v1[0]*v1[0]+v1[1]*v1[1]+v1[2]*v1[2]);
    if (v1norm>cmnTypeTraits<double>::Tolerance()) {
        v1[0] /= v1norm;
        v1[1] /= v1norm;
        v1[2] /= v1norm;
    }

    // v2 = ((p2-p1)/norm(p2-p1))
    v2[0] = point2[0]-point1[0];
    v2[1] = point2[1]-point1[1];
    v2[2] = point2[2]-point1[2];
    v2norm = sqrt(v2[0]*v2[0]+v2[1]*v2[1]+v2[2]*v2[2]);
    if (v2norm>cmnTypeTraits<double>::Tolerance())
        {
            v2[0] /= v2norm;
            v2[1] /= v2norm;
            v2[2] /= v2norm;
        }

    // w = (v1 x v2)/norm(v1 x v2)
    w[0] = v1[1]*v2[2] - v1[2]*v2[1];
    w[1] = v1[2]*v2[0] - v1[0]*v2[2];
    w[2] = v1[0]*v2[1] - v1[1]*v2[0];
    wnorm = sqrt(w[0]*w[0]+w[1]*w[1]+w[2]*w[2]);
    if (wnorm> cmnTypeTraits<double>::Tolerance()) {
        w[0] /= wnorm;
        w[1] /= wnorm;
        w[2] /= wnorm;
    }
    else {
        w[0] = 1.0;
        w[1] = w[2] = 0.0;
    }

    // theta = arccos(v1.v2/(norm(v1)*norm(v2))
    dotarg = v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2];
    if (dotarg>-1.0 && dotarg<1.0) {
        angle = acos(dotarg);
    } else {
        angle = 0.0;
    }
    //if (CMN_ISNAN(angle)) angle=0.0;

    // std::cout << "v1: " << v1[0] << " " << v1[1] << " " << v1[2] << std::endl;
    // std::cout << "v2: " << v2[0] << " " << v2[1] << " " << v2[2] << std::endl;
    // std::cout << "w: " << w[0] << " " << w[1] << " " << w[2] << " angle: " << angle*180.0/cmnPI << std::endl;

    // Set object pose updates.
    object_displacement[0] = (point1[0]+point2[0])/2 - (pointa[0]+pointb[0])/2;
    object_displacement[1] = (point1[1]+point2[1])/2 - (pointa[1]+pointb[1])/2;
    object_displacement[2] = (point1[2]+point2[2])/2 - (pointa[2]+pointb[2])/2;

    object_rotation[0] = angle;
    object_rotation[1] = w[0];
    object_rotation[2] = w[1];
    object_rotation[3] = w[2];
}


bool ManualRegistration::RunBackground(void)
{
    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
    }
    return true;
}

bool ManualRegistration::RunNoInput(void)
{
    if (this->Manager->MastersAsMice() != this->BooleanFlags[PREVIOUS_MAM]) {
        this->BooleanFlags[PREVIOUS_MAM] = this->Manager->MastersAsMice();
    }

#if 1

    this->GetJointPositionECM(this->JointsECM);

    //check if the objects should be updated
    if (this->BooleanFlags[CAMERA_PRESSED]) {
        //Update when the camera is clutched
        UpdateECMtoECMRCM();
        this->UpdateVisibleList(true);
    }
#endif
    return true;
}


void ManualRegistration::UpdateVisibleList(bool updateAll)
{
    vctFrm3 ECMRCMtoUI3 = ECMtoUI3 * ECMtoECMRCM.Inverse();
    ManualRegistrationType::iterator foundModel;
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel == VisibleObjects.end())
        return;

    if (updateAll) {
        VisibleListReal->SetTransformation(ECMRCMtoUI3 * this->PositionECMRCM);
        VisibleListVirtual->SetTransformation(ECMRCMtoUI3 * (foundModel->second)->PositionECMRCM);
        (foundModel->second)->PositionECMRCM = ECMRCMtoUI3.Inverse()*(foundModel->second)->GetAbsoluteTransformation();
    }
    else {
        VisibleListVirtual->SetTransformation(ECMRCMtoUI3 * (foundModel->second)->PositionECMRCM);
    }

    for (ManualRegistrationType::iterator iter = VisibleObjects.begin();
         iter != VisibleObjects.end();
         iter++) {
        (iter->second)->PositionECMRCM = ECMRCMtoUI3.Inverse()*(iter->second)->GetAbsoluteTransformation();
        if (this->BooleanFlags[VISIBLE] && (iter->second)->Visible) {
            (iter->second)->Show();
        } else {
            (iter->second)->Hide();
        }
    }

    for (ManualRegistrationType::iterator iter = VisibleObjectsVirtualFiducials.begin();
         iter != VisibleObjectsVirtualFiducials.end();
         iter++) {
        (iter->second)->PositionECMRCM = ECMRCMtoUI3.Inverse()*(iter->second)->GetAbsoluteTransformation();
        if (this->BooleanFlags[VISIBLE] && (iter->second)->Visible) {
            (iter->second)->Show();
        } else {
            (iter->second)->Hide();
        }
    }
    for (ManualRegistrationType::iterator iter = VisibleObjectsRealFiducials.begin();
         iter != VisibleObjectsRealFiducials.end();
         iter++) {
        (iter->second)->PositionECMRCM = ECMRCMtoUI3.Inverse()*(iter->second)->GetAbsoluteTransformation();
        if (this->BooleanFlags[VISIBLE] && (iter->second)->Visible) {
            (iter->second)->Show();
        } else {
            (iter->second)->Hide();
        }
    }
}


void ManualRegistration::OnQuit(void)
{
}


void ManualRegistration::OnStart(void)
{
    vctFrm3 homePositionUI3, modelHomePositionUI3;
    modelHomePositionUI3.Translation().Assign(0.0,0.0,-200.0);

    UpdateECMtoECMRCM();
    ManualRegistrationType::iterator foundModel;
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel != VisibleObjects.end()) {
        (foundModel->second)->PositionECMRCM = ECMtoECMRCM * UI3toECM * modelHomePositionUI3;
        (foundModel->second)->HomePositionUI3 = modelHomePositionUI3;
    }
    homePositionUI3.Translation().Assign(0.0,0.0,0.0);
    this->PositionECMRCM = ECMtoECMRCM * UI3toECM * homePositionUI3;

    foundModel = VisibleObjects.find(MODEL);
    if (foundModel != VisibleObjects.end()) {
        foundModel->second->SetColor(1.0, 0.49, 0.25);
    }

    for (ManualRegistrationType::iterator iter = VisibleObjects.begin();
         iter != VisibleObjects.end();
         iter++) {
        (iter->second)->SetStipplePercentage(50);
        (iter->second)->SetOpacity(0.7);
    }

#if IMPORT_VIRTUAL_FIDUCIALS
    bool valid = ImportFiducialFile("E:/Users/wliu25/MyCommon/data/TORS/CTFids.fcsv");
#endif

    UpdateVisibleList(false);

    std::cout << "OnStart Finished" << std::endl;
}


void ManualRegistration::Startup(void) {

    // To get the joint values, we need to access the component directly
    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
    CMN_ASSERT(componentManager);
    mtsComponent * daVinci = componentManager->GetComponent("daVinci");
    CMN_ASSERT(daVinci);

    // get slave interface
    mtsInterfaceProvided * interfaceProvided = daVinci->GetInterfaceProvided("ECM1");
    CMN_ASSERT(interfaceProvided);
    mtsCommandRead * command = interfaceProvided->GetCommandRead("GetPositionJoint");
    CMN_ASSERT(command);
    GetJointPositionECM.Bind(command);

    //get camera control interface
    interfaceProvided = daVinci->GetInterfaceProvided("Camera");
    CMN_ASSERT(interfaceProvided);
    mtsCommandWrite<ManualRegistration, prmEventButton> * cameraCallbackCommand =
        new mtsCommandWrite<ManualRegistration, prmEventButton>(&ManualRegistration::CameraControlPedalCallback, this,
                                                                "Button", prmEventButton());
    CMN_ASSERT(cameraCallbackCommand);
    interfaceProvided->AddObserver("Button", cameraCallbackCommand);

}

void ManualRegistration::PrimaryMasterButtonCallback(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::PRESSED) {
        this->BooleanFlags[RIGHT_BUTTON] = true;
        UpdatePreviousPosition();
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->BooleanFlags[RIGHT_BUTTON] = false;
        this->BooleanFlags[RIGHT_BUTTON_RELEASED] = true;
        //CMN_LOG_CLASS_VERY_VERBOSE << "Primary master button pressed, following ended" << std::endl;
    }
    UpdateFollowing();
}


void ManualRegistration::SecondaryMasterButtonCallback(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::PRESSED) {
        this->BooleanFlags[LEFT_BUTTON] = true;
        UpdatePreviousPosition();
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->BooleanFlags[LEFT_BUTTON] = false;
        this->BooleanFlags[LEFT_BUTTON_RELEASED] = true;
        //CMN_LOG_CLASS_VERY_VERBOSE << "Primary master button pressed, following ended" << std::endl;
    }
    UpdateFollowing();
}


void ManualRegistration::UpdateFollowing(void)
{
    prmPositionCartesianGet position;
    if (this->BooleanFlags[RIGHT_BUTTON]) {
        this->GetPrimaryMasterPosition(position);
        InitialMasterRight = position.Position().Translation();
    }

    if (this->BooleanFlags[LEFT_BUTTON]) {
        this->GetSecondaryMasterPosition(position);
        InitialMasterLeft = position.Position().Translation();
    }

    if (this->BooleanFlags[RIGHT_BUTTON] && this->BooleanFlags[LEFT_BUTTON]) {
        this->BooleanFlags[BOTH_BUTTON_PRESSED] = true;
    } else if (this->BooleanFlags[BOTH_BUTTON_PRESSED]){
        this->BooleanFlags[BOTH_BUTTON_PRESSED] = false;
    }

}


/*!

  Returns the current position of the center of the tool in the frame of the camera Remote center of motion
  @return the frame of the tool wrt to the ECM RCM

*/
void ManualRegistration::UpdateECMtoECMRCM(void)
{
    vctDouble3 Xaxis;
    Xaxis.Assign(1.0,0.0,0.0);
    vctDouble3 Yaxis;
    Yaxis.Assign(0.0,1.0,0.0);
    vctDouble3 Zaxis;
    Zaxis.Assign(0.0,0.0,1.0);

    // get joint values for ECM
    mtsExecutionResult result = this->GetJointPositionECM(this->JointsECM);
    if (!result.IsOK()) {
        std::cout << "GetECMtoECMRCM(): ERROR" << result << std::endl;
    }
    // [0] = outer yaw
    // [1] = outer pitch
    // [2] = scope insertion
    // [3] = scope roll

    double yaw0 = JointsECM.Position()[0];
    double pitch1 = JointsECM.Position()[1];
    double insert2 = JointsECM.Position()[2]*1000.0;//convert to mm
    double roll3 = JointsECM.Position()[3];
    double angle = 30.0*cmnPI/180.0;

    //create frame for yaw
    vctFrm3 yawFrame0;
    yawFrame0.Rotation() = vctMatRot3(vctAxAnRot3(Yaxis, yaw0));

    //create frame for pitch
    vctFrm3 pitchFrame1;
    pitchFrame1.Rotation() = vctMatRot3(vctAxAnRot3(Xaxis, -pitch1));  // we don't have any logical explanation

    //create frame for insertion
    vctFrm3 insertFrame2;
    insertFrame2.Translation() = vctDouble3(0.0, 0.0, insert2);

    //create frame for the roll
    vctFrm3 rollFrame3;
    rollFrame3.Rotation() = vctMatRot3(vctAxAnRot3(Zaxis, roll3));

    vctFrm3 T_to_horiz;
    T_to_horiz.Rotation() = vctMatRot3(vctAxAnRot3(Xaxis, angle));

    ECMtoECMRCM = yawFrame0 * pitchFrame1 * insertFrame2 * rollFrame3;

}

/*!
  Function callback triggered by pressing the camera control pedal
  Changes the state of the behavior and allows some other features to become active
*/

void ManualRegistration::CameraControlPedalCallback(const prmEventButton & payload)
{
    if (payload.Type() == prmEventButton::PRESSED) {
        this->BooleanFlags[CAMERA_PRESSED] = true;
        if (this->BooleanFlags[DEBUG])
            std::cout << "Camera pressed" << std::endl;
    } else {
        this->BooleanFlags[CAMERA_PRESSED] = false;
    }
}

ui3VisibleObject* ManualRegistration::GetVisibleObjectAtIndex(int index)
{
    ManualRegistrationType::iterator found = VisibleObjects.find(index);
    if (found != VisibleObjects.end())
        return found->second;
    else
        return NULL;
}

bool ManualRegistration::ImportFiducialFile(const std::string & inputFile)
{
    vct3 positionFromFile;
    vctFrm3 fiducialPositionUI3, modelPositionUI3, altPositionUI3;
    altPositionUI3.Translation().Assign(0.0,0.0,-300.0);
    std::string tempLine = "aaaa";
    std::vector <std::string> token;
    ManualRegistrationType::iterator foundModel;
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel == VisibleObjects.end())
        return false;
    modelPositionUI3 = ECMtoUI3 * ECMtoECMRCM.Inverse() * (foundModel->second)->PositionECMRCM;

    if (this->BooleanFlags[DEBUG]) {
        std::cerr << "Importing fiducials from: " << inputFile << std::endl;
    }
    std::ifstream inf(inputFile.c_str());
    while(1) {
        tempLine = "aaaa";
        std::vector <std::string> token;
        std::getline(inf, tempLine);
        Tokenize(tempLine, token, ",");
        if (inf.eof() || token.size() <= 0)
            break;
        std::cerr << token << std::endl;
        if (token.at(0).compare(0,1,"#")) {
            if (token.size() < 4)
                return false;
            positionFromFile =  vct3(strtod(token.at(1).c_str(), NULL),strtod(token.at(2).c_str(), NULL),strtod(token.at(3).c_str(), NULL));
            fiducialPositionUI3.Translation().Assign(modelPositionUI3 * positionFromFile);
            AddFiducial(fiducialPositionUI3, true);
            ////COMPLETE HACK TO TEST REGISTRATION WITH STATIC REAL FIDUCIALS - REMOVE ASAP!!!
            fiducialPositionUI3.Translation().Assign(altPositionUI3 * positionFromFile);
            AddFiducial(fiducialPositionUI3, false);
        }
        token.clear();
    }
    return true;
}


void ManualRegistration::Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


void ManualRegistration::Register(void)
{
    double fre;
    vctFrm3 displacementUI3, modelPositionUI3, positionUI3, modelPositionUI3New;
    vctDoubleFrm3 fiducialAbsolutePosition;
    ManualRegistrationType::iterator foundModel;
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel == VisibleObjects.end())
        return;

    // get current position in UI3
    positionUI3 = ECMtoUI3 * ECMtoECMRCM.Inverse() * this->PositionECMRCM;
    modelPositionUI3 = ECMtoUI3 * ECMtoECMRCM.Inverse() * (foundModel->second)->PositionECMRCM;
    vctDynamicVector<vct3> virtualFiducials;
    vctDynamicVector<vct3> realFiducials;
    int fiducialIndex = 0;

    for (ManualRegistrationType::iterator iter = VisibleObjectsVirtualFiducials.begin();
         iter != VisibleObjectsVirtualFiducials.end();
         iter++) {
        virtualFiducials.resize(fiducialIndex + 1);
        virtualFiducials[fiducialIndex] = (iter->second)->GetAbsoluteTransformation().Translation();//(modelPositionUI3*(iter->second)->GetTransformation()).Translation();
        if (this->BooleanFlags[DEBUG])
            std::cerr << "Register virtual fiducial " << fiducialIndex << " at abs positionUI3 " << ((iter->second)->GetAbsoluteTransformation()).Translation()
                      << " position: " << ((iter->second)->GetTransformation()).Translation()
                      << " using " << virtualFiducials[fiducialIndex] << std::endl;
        fiducialIndex++;
    }
    fiducialIndex = 0;
    for (ManualRegistrationType::iterator iter = VisibleObjectsRealFiducials.begin();
         iter != VisibleObjectsRealFiducials.end();
         iter++) {
        realFiducials.resize(fiducialIndex + 1);
        realFiducials[fiducialIndex] = (iter->second)->GetAbsoluteTransformation().Translation();//(positionUI3*(iter->second)->GetTransformation()).Translation();
        if (this->BooleanFlags[DEBUG])
            std::cerr << "Register real fiducial " << fiducialIndex << " at abs positionUI3 " << ((iter->second)->GetAbsoluteTransformation()).Translation()
                      << " position: " << ((iter->second)->GetTransformation()).Translation()
                      << " using " << realFiducials[fiducialIndex] << std::endl;
        fiducialIndex++;
    }
    bool valid = nmrRegistrationRigid(virtualFiducials, realFiducials, displacementUI3,&fre);
    if (valid) {
        UpdateECMtoECMRCM();
        // apply delta in UI3
        modelPositionUI3New = modelPositionUI3 * displacementUI3;
        // go back to ECMRCM
        (foundModel->second)->PositionECMRCM = ECMtoECMRCM * UI3toECM * modelPositionUI3New;
        UpdateVisibleList(false);
        std::cerr << "Registered using # " << realFiducials.size() << " fiducials with fre: "<< fre << std::endl;
    } else {
        std::cerr << "ERROR:ManualRegistration::Register() error, see log" << std::endl;
    }
}


void ManualRegistration::AddFiducial(vctFrm3 positionUI3, bool virtualFlag)
{
    int fiducialIndex;
    ManualRegistrationType::iterator foundModel;

    // Find first virtual object, i.e. Model
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel == VisibleObjects.end())
        return;

    // Get fiducial index
    if (virtualFlag) {
        fiducialIndex = VisibleObjectsVirtualFiducials.size();
    } else {
        fiducialIndex = VisibleObjectsRealFiducials.size();
    }

    // create new visibleObject for fiducial
    ManualRegistrationSurfaceVisibleStippleObject * newFiducial;
    char buffer[33];

    sprintf(buffer, "%d", fiducialIndex);
    // itoa(fiducialIndex, buffer, 10);
    newFiducial = new ManualRegistrationSurfaceVisibleStippleObject(buffer,ManualRegistrationSurfaceVisibleStippleObject::SPHERE);

    // add visibleObject to visibleList and visibleObjects
    if (virtualFlag) {
        VisibleObjectsVirtualFiducials[fiducialIndex] = newFiducial;
        this->VisibleListVirtual->Add(newFiducial);
    } else {
        VisibleObjectsRealFiducials[fiducialIndex] = newFiducial;
        this->VisibleListReal->Add(newFiducial);
    }
    newFiducial->WaitForCreation();
    newFiducial->SetStipplePercentage(50);
    newFiducial->SetOpacity(0.7);
    newFiducial->Show();
    newFiducial->Visible = true;
    newFiducial->PositionECMRCM = ECMtoECMRCM * UI3toECM * positionUI3;

    if (virtualFlag) {
        newFiducial->SetColor(0.0,0.0,1.0);
        // set position wrt first virtualObject, i.e. Model
        newFiducial->SetPosition((foundModel->second)->PositionECMRCM.Inverse()*newFiducial->PositionECMRCM.Translation());
        CMN_LOG_CLASS_RUN_VERBOSE << "PrimaryMasterButtonCallback: added virtual fiducial: " << fiducialIndex << " "
                                  << newFiducial->PositionECMRCM.Translation() << std::endl;
        if (this->BooleanFlags[DEBUG])
            std::cerr << "Adding virtual fiducial " << fiducialIndex << " at positionUI3 " << (ECMtoUI3 * ECMtoECMRCM.Inverse() * newFiducial->PositionECMRCM).Translation() << std::endl;
    } else {
        newFiducial->SetColor(1.0,0.0,0.0);
        // set position wrt visibleList
        newFiducial->SetPosition(this->PositionECMRCM.Inverse()*newFiducial->PositionECMRCM.Translation());
        CMN_LOG_CLASS_RUN_VERBOSE << "PrimaryMasterButtonCallback: added virtual fiducial: " << fiducialIndex << " "
                                  << newFiducial->PositionECMRCM.Translation() << std::endl;
        if (this->BooleanFlags[DEBUG])
            std::cerr << "Adding real fiducial " << fiducialIndex << " at positionUI3 " << (ECMtoUI3 * ECMtoECMRCM.Inverse()*newFiducial->PositionECMRCM).Translation() << std::endl;
    }

    this->BooleanFlags[RIGHT_BUTTON_RELEASED] = false;
    this->BooleanFlags[LEFT_BUTTON_RELEASED] = false;
    this->UpdateVisibleList(false);
    return;
}
