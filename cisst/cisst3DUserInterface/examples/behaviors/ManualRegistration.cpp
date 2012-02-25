/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
$Id$

Author(s):  Wen P. Liu, Anton Deguet
Created on: 2012-01-27

(C) Copyright 2012 Johns Hopkins University (JHU), All Rights
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
#define FIDUCIAL_COUNT_MAX 30

class ManualRegistrationSurfaceVisibleStippleObject: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    enum GeometryType {NONE=0,CUBE=1,SPHERE=2};
    inline ManualRegistrationSurfaceVisibleStippleObject(const std::string & inputFile, const GeometryType & geometry=NONE, int size = 0, bool hasOutline = false):
        ui3VisibleObject(),
        Visible(true),
        Valid(true),
        InputFile(inputFile),
        SurfaceReader(0),
        SurfaceMapper(0),
        SurfaceActor(0),
        HasOutline(hasOutline),
        OutlineData(0),
        OutlineMapper(0),
        OutlineActor(0),
        Geometry(geometry),
        Size(size)
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
        source->SetBounds(-1*this->Size,this->Size,-1*this->Size,this->Size,-1*this->Size,this->Size);
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
        source->SetRadius(this->Size);

        SurfaceMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(SurfaceMapper);
        SurfaceMapper->SetInputConnection(source->GetOutputPort());
        SurfaceMapper->ImmediateModeRenderingOn();

        SurfaceActor = ui3VTKStippleActor::New();
        CMN_ASSERT(SurfaceActor);
        SurfaceActor->SetMapper(SurfaceMapper);
        
	    // Create a vector text
	    vtkVectorText* vecText = vtkVectorText::New();
	    vecText->SetText(InputFile.c_str());

	    vtkLinearExtrusionFilter* extrude = vtkLinearExtrusionFilter::New();
        extrude->SetInputConnection( vecText->GetOutputPort());
	    extrude->SetExtrusionTypeToNormalExtrusion();
	    extrude->SetVector(0, 0, 1 );
	    extrude->SetScaleFactor (0.5);

	    vtkPolyDataMapper* txtMapper = vtkPolyDataMapper::New();
	    txtMapper->SetInputConnection( extrude->GetOutputPort());
        ui3VTKStippleActor * textActor = ui3VTKStippleActor::New();
        CMN_ASSERT(textActor);
	    textActor->SetMapper(txtMapper);

        // Add the actor(s)
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

    bool Valid;
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
    int Size;
};

CMN_DECLARE_SERVICES_INSTANTIATION(ManualRegistrationSurfaceVisibleStippleObject);
CMN_IMPLEMENT_SERVICES(ManualRegistrationSurfaceVisibleStippleObject);

ManualRegistration::ManualRegistration(const std::string & name):
    ui3BehaviorBase(std::string("ManualRegistration::") + name, 0),
    VisibleList(0),
    VisibleListECM(0),
    VisibleListECMRCM(0),
    VisibleListVirtual(0),
    VisibleListReal(0)
{
    VisibleList = new ui3VisibleList("ManualRegistration");
    VisibleListECM = new ui3VisibleList("ManualRegistrationECM");
    VisibleListECMRCM = new ui3VisibleList("ManualRegistrationECMRCM");
    VisibleListVirtual = new ui3VisibleList("ManualRegistrationVirtual");
    VisibleListReal = new ui3VisibleList("ManualRegistrationReal");

    ManualRegistrationSurfaceVisibleStippleObject * model;
#if CUBE_DEMO
    model = new ManualRegistrationSurfaceVisibleStippleObject("",ManualRegistrationSurfaceVisibleStippleObject::CUBE,25);
#else
    model = new ManualRegistrationSurfaceVisibleStippleObject("E:/Users/wliu25/MyCommon/data/TORS/TORS_tongue.vtk");
#endif

    VisibleObjects[MODEL] = model;

    for (ManualRegistrationType::iterator iter = VisibleObjects.begin();
         iter != VisibleObjects.end();
         iter++) {
        VisibleListVirtual->Add(iter->second);
    }

    VisibleListECMRCM->Add(VisibleListVirtual);
    VisibleListECMRCM->Add(VisibleListReal);
    VisibleListECM->Add(VisibleListECMRCM);
    VisibleList->Add(VisibleListECM);

    // Initialize boolean flags
    this->BooleanFlags[DEBUG] = true;
    this->BooleanFlags[VISIBLE] = true;
    this->BooleanFlags[PREVIOUS_MAM] = false;
    this->BooleanFlags[CAMERA_PRESSED] = false;
    this->BooleanFlags[UPDATE_FIDUCIALS] = false;
    ResetButtonEvents();
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
    vctFrm3 positionUI3 = (foundModel->second)->GetAbsoluteTransformation();
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
    ManualRegistrationType::iterator foundModel;
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel == VisibleObjects.end()) {
        return;
    }
    //previous positions saved in ECMRCM
    (foundModel->second)->PreviousPositions[(foundModel->second)->PreviousPositions.size()+1] = (foundModel->second)->GetAbsoluteTransformation();
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
        this->VisibleListVirtual->SetTransformation(this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse() * (foundModel->second)->PreviousPositions[(foundModel->second)->PreviousIndex]);
        if (this->BooleanFlags[DEBUG]) {
            std::cout << "Setting back to index:" << (foundModel->second)->PreviousIndex << std::endl;
        }
        (foundModel->second)->PreviousIndex--;
        if ((foundModel->second)->PreviousIndex < 0) {
            (foundModel->second)->PreviousIndex = 0;
        }
    }
}


void ManualRegistration::PositionHome(void)
{
    ManualRegistrationType::iterator foundModel;
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel != VisibleObjects.end())
        this->VisibleListVirtual->SetTransformation(this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse()*(foundModel->second)->HomePositionUI3);
}

void ManualRegistration::ToggleUpdateFiducials()
{
    if (this->BooleanFlags[UPDATE_FIDUCIALS]) {
        if (this->BooleanFlags[DEBUG]) {
            std::cerr << "Toggle update fiducials: Off" << std::endl;
        }
        this->BooleanFlags[UPDATE_FIDUCIALS] = false;
    }
    else {
        if (this->BooleanFlags[DEBUG]) {
            std::cerr << "Toggle update fiducials: On" << std::endl;
        }
        this->BooleanFlags[UPDATE_FIDUCIALS] = true;
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

   UpdateVisibleList();
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
    this->MenuBar->AddClickButton("ToggleUpdateFiducials",
                                  2,
                                  "map.png",
                                  &ManualRegistration::ToggleUpdateFiducials,
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

void ManualRegistration::UpdateFiducials(void)
{
    prmPositionCartesianGet positionLeft, positionRight;
    this->GetPrimaryMasterPosition(positionRight);
    this->GetSecondaryMasterPosition(positionLeft);
    ManualRegistrationSurfaceVisibleStippleObject* closestFiducial = NULL;

    if (!this->BooleanFlags[BOTH_BUTTON_PRESSED]
        && this->BooleanFlags[RIGHT_BUTTON]
        && !this->BooleanFlags[LEFT_BUTTON]) {
            closestFiducial = FindClosestFiducial(positionRight.Position(),false);
        if(closestFiducial != NULL)
        {
            closestFiducial->Valid = false;
            ResetButtonEvents();
            UpdateVisibleList();
        }else
        {
            AddFiducial(positionRight.Position(),false);
        }
    }
    else if (!this->BooleanFlags[BOTH_BUTTON_PRESSED]
             && !this->BooleanFlags[RIGHT_BUTTON]
             && this->BooleanFlags[LEFT_BUTTON]) {
            closestFiducial = FindClosestFiducial(positionLeft.Position(),true);
        if(closestFiducial != NULL)
        {
            closestFiducial->Valid = false;
            ResetButtonEvents();
            UpdateVisibleList();
        }
        else
        {
            AddFiducial(positionLeft.Position(),true);
        }
    } 
}

void ManualRegistration::FollowMaster(void)
{
    prmPositionCartesianGet positionLeft, positionRight;
    vctFrm3 displacementUI3, displacementUI3T, displacementUI3R;
    vctFrm3 visibleListVirtualPositionUI3, visibleListVirtualPositionUI3New;
    vctFrm3 displacementECMRCM, displacementECMRCMT, displacementECMRCMR;
    
    this->GetPrimaryMasterPosition(positionRight);
    this->GetSecondaryMasterPosition(positionLeft);

    vctFrm3 currentUI3toECMRCM = this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse();
    vctDouble3 initialMasterRightECMRCM = currentUI3toECMRCM * InitialMasterRight;
    vctDouble3 initialMasterLeftECMRCM = currentUI3toECMRCM * InitialMasterLeft;
    vctDouble3 positionRightECMRCM = currentUI3toECMRCM * positionRight.Position().Translation();
    vctDouble3 positionLeftECMRCM = currentUI3toECMRCM * positionLeft.Position().Translation();

    if (!this->BooleanFlags[BOTH_BUTTON_PRESSED]
        && this->BooleanFlags[RIGHT_BUTTON]
        && !this->BooleanFlags[LEFT_BUTTON]) {
        //translation only using right
        displacementECMRCM.Translation() = positionRightECMRCM - initialMasterRightECMRCM;
    }
    else if (!this->BooleanFlags[BOTH_BUTTON_PRESSED]
             && !this->BooleanFlags[RIGHT_BUTTON]
             && this->BooleanFlags[LEFT_BUTTON]) {
        //translation only using left
        displacementECMRCM.Translation() = positionLeftECMRCM - initialMasterLeftECMRCM;
    } else if (this->BooleanFlags[BOTH_BUTTON_PRESSED]) {

        //rotation using both
        vctDouble3 axis;
        axis.Assign(1.0,0.0,0.0);
        double angle;
        angle = 0.0;
        double object_displacement[3], object_rotation[4];
        vctDouble3 translation, translationInWorld;
        vctDouble3 axisInWorld;

        vctFrm3 handleCenterECMRCM;
        handleCenterECMRCM.Translation().SumOf(initialMasterRightECMRCM, initialMasterLeftECMRCM);
        handleCenterECMRCM.Translation().Divide(2.0);

        ComputeTransform(initialMasterRightECMRCM.Pointer(),
                         initialMasterLeftECMRCM.Pointer(),
                         positionRightECMRCM.Pointer(),
                         positionLeftECMRCM.Pointer(),
                         object_displacement, object_rotation);

        // Set the Translation.
        translation.Assign(object_displacement);
        // visibleListVirtualPositionUI3.Rotation().ApplyInverseTo(translation, translationInWorld);
        //displacementUI3T.Translation()= translation /*InWorld*/;
        displacementECMRCMT.Translation()= translation;
        // hard coded scale
        translation *= 0.1;

        // Set the Rotation.
        angle = object_rotation[0];
        // hard coded scale
        angle *= 0.5;
        axis.Assign(object_rotation+1);
        // visibleListVirtualPositionUI3.Rotation().ApplyInverseTo(axis, axisInWorld);
        //displacementUI3R.Rotation().From(vctAxAnRot3(axis /*InWorld*/, angle));
        displacementECMRCMR.Rotation().From(vctAxAnRot3(axis, angle));

        // so we apply rotation on center of handles
        displacementECMRCM = displacementECMRCMT * handleCenterECMRCM * displacementECMRCMR * handleCenterECMRCM.Inverse();

    }

    // save cursor positions
    InitialMasterRight = positionRight.Position().Translation();
    InitialMasterLeft = positionLeft.Position().Translation();

    // apply transformation in ECMRCM
    this->VisibleListVirtual->SetTransformation(displacementECMRCM * this->VisibleListVirtual->GetTransformation());
    //if(this->BooleanFlags[DEBUG])
    //    std::cerr << "VisibleListVirtual " << this->VisibleListVirtual->GetAbsoluteTransformation().Translation() << " rel " << this->VisibleListVirtual->GetTransformation().Translation() << std::endl;

}


/*!
  Compute the object transform from the motion of two grabbed control points.
  @param pointa               Right control position.
  @param pointb               Left control position.
  @param point1               Right cursor pos.
  @param point2               Left cursor pos.
  @param object_displacement  [dx, dy, dz]
  @param object_rotation      [angle, axis_x, axis_y, axis_z]
  Author(s):  Simon DiMaio
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

bool ManualRegistration::RunForeground(void)
{
    if (this->Manager->MastersAsMice() != this->BooleanFlags[PREVIOUS_MAM]) {
        this->BooleanFlags[PREVIOUS_MAM] = this->Manager->MastersAsMice();
        ResetButtonEvents();
    }

    // detect transition, should that be handled as an event?
    // State is used by multiple threads ...
    if (this->State != this->PreviousState) {
        std::cerr << "Entering RunForeground" << std::endl;
        this->PreviousState = this->State;
    }

    // detect active mice
    if (this->BooleanFlags[UPDATE_FIDUCIALS]) {
        UpdateFiducials();
    } else if (this->BooleanFlags[LEFT_BUTTON] || this->BooleanFlags[RIGHT_BUTTON]) {
        FollowMaster();
    }

    return true;
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
        ResetButtonEvents();
    }

    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
    }

    //check if the objects should be updated
    if (this->BooleanFlags[CAMERA_PRESSED]) {
        UpdateCameraPressed();
    }

    return true;
}


void ManualRegistration::UpdateCameraPressed()
{
    vctFrm3 currentECMtoECMRCM = GetCurrentECMtoECMRCM();
    this->VisibleListECMRCM->SetTransformation(currentECMtoECMRCM);
    //if(this->BooleanFlags[DEBUG])
    //    std::cerr << "VisibleListECMRCM " << this->VisibleListECMRCM->GetAbsoluteTransformation().Translation() << " rel " << this->VisibleListECMRCM->GetTransformation().Translation() << std::endl;
}


void ManualRegistration::OnQuit(void)
{
}


void ManualRegistration::OnStart(void)
{
    vctFrm3 homePosition, modelHomePosition, currentECMtoECMRCM, staticECMtoUI3;
    ManualRegistrationType::iterator foundModel;

    // VirtualList - Set root transformation at origin
    homePosition.Translation().Assign(0.0,0.0,0.0);
    this->VisibleList->SetTransformation(homePosition);

    // VisibleListECM - Rotate by pi in y to be aligned to console
    staticECMtoUI3.Rotation().From(vctAxAnRot3(vctDouble3(0.0,1.0,0.0), cmnPI));
    this->VisibleListECM->SetTransformation(staticECMtoUI3.Inverse());

    // VisibleListECMRCM
    currentECMtoECMRCM = GetCurrentECMtoECMRCM();
    this->VisibleListECMRCM->SetTransformation(currentECMtoECMRCM);
 
    // VisibleListVirtual
    // VTK meshes harded coded start location at (0,0,-200)
    modelHomePosition.Translation().Assign(0.0,0.0,-200.0);
    this->VisibleListVirtual->SetTransformation(this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse()*modelHomePosition);

    // setup VTK model
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel != VisibleObjects.end()) {
        (foundModel->second)->SetColor(1.0, 0.49, 0.25);
        (foundModel->second)->HomePositionUI3 = modelHomePosition;
    }

    for (ManualRegistrationType::iterator iter = VisibleObjects.begin();
         iter != VisibleObjects.end();
         iter++) {
        (iter->second)->SetStipplePercentage(50);
        (iter->second)->SetOpacity(0.7);
        (iter->second)->Visible = true;
        (iter->second)->Valid = true;
        (iter->second)->Show();
    }

#if CUBE_DEMO && IMPORT_VIRTUAL_FIDUCIALS
    bool valid = ImportFiducialFile("E:/Users/wliu25/MyCommon/data/TORS/CTFids.fcsv");
#endif
    UpdateVisibleList();
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
        this->BooleanFlags[RIGHT_BUTTON_RELEASED] = false;
        UpdatePreviousPosition();
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->BooleanFlags[RIGHT_BUTTON] = false;
        this->BooleanFlags[RIGHT_BUTTON_RELEASED] = true;
    }
    UpdateButtonEvents();
}


void ManualRegistration::SecondaryMasterButtonCallback(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::PRESSED) {
        this->BooleanFlags[LEFT_BUTTON] = true;
        this->BooleanFlags[LEFT_BUTTON_RELEASED] = false;
        UpdatePreviousPosition();
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->BooleanFlags[LEFT_BUTTON] = false;
        this->BooleanFlags[LEFT_BUTTON_RELEASED] = true;
    }
    UpdateButtonEvents();
}


void ManualRegistration::UpdateButtonEvents(void)
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
vctFrm3 ManualRegistration::GetCurrentECMtoECMRCM(void)
{
    prmPositionJointGet jointsECM;
    vctFrm3 currentECMRCMtoECM;

    vctDouble3 Xaxis;
    Xaxis.Assign(1.0,0.0,0.0);
    vctDouble3 Yaxis;
    Yaxis.Assign(0.0,1.0,0.0);
    vctDouble3 Zaxis;
    Zaxis.Assign(0.0,0.0,1.0);

    // get joint values for ECM
    mtsExecutionResult result = this->GetJointPositionECM(jointsECM);
    if (!result.IsOK()) {
        std::cout << "GetECMtoECMRCM(): ERROR" << result << std::endl;
    }
    // [0] = outer yaw
    // [1] = outer pitch
    // [2] = scope insertion
    // [3] = scope roll

    double yaw0 = jointsECM.Position()[0];
    double pitch1 = jointsECM.Position()[1];
    double insert2 = jointsECM.Position()[2]*1000.0;//convert to mm
    double roll3 = jointsECM.Position()[3];
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

    currentECMRCMtoECM = yawFrame0 * pitchFrame1 * insertFrame2 * rollFrame3;
    return currentECMRCMtoECM.Inverse();

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
            //assume fiducials are given wrt to model
            positionFromFile =  vct3(strtod(token.at(1).c_str(), NULL),strtod(token.at(2).c_str(), NULL),strtod(token.at(3).c_str(), NULL));
            fiducialPositionUI3.Translation().Assign((foundModel->second)->GetAbsoluteTransformation() * positionFromFile);
            AddFiducial(fiducialPositionUI3, true);
            ////HACK TO TEST REGISTRATION WITH STATIC REAL FIDUCIALS - REMOVE!!!
            //fiducialPositionUI3.Translation().Assign(altPositionUI3 * positionFromFile);
            //AddFiducial(fiducialPositionUI3, false);
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
    vctFrm3 displacementECMRCM, currentUI3toECMRCM;
    ManualRegistrationType::iterator foundModel;
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel == VisibleObjects.end())
        return;

    // get current of ECMRCM
    vctDynamicVector<vct3> virtualFiducials;
    vctDynamicVector<vct3> realFiducials;
    int fiducialIndex = 0;

    for (ManualRegistrationType::iterator iter = VisibleObjectsVirtualFiducials.begin();
         iter != VisibleObjectsVirtualFiducials.end();
         iter++) {
        if((iter->second)->Valid)
        {
            virtualFiducials.resize(fiducialIndex + 1);
            virtualFiducials[fiducialIndex] = this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse() *(iter->second)->GetAbsoluteTransformation().Translation();
            if (this->BooleanFlags[DEBUG])
                std::cerr << "Register virtual fiducial " << fiducialIndex << " at abs positionUI3 " << ((iter->second)->GetAbsoluteTransformation()).Translation()
                          << " relative position: " << ((iter->second)->GetTransformation()).Translation()
                          << " using " << virtualFiducials[fiducialIndex] << std::endl;
            fiducialIndex++;
        }
    }
    fiducialIndex = 0;
    for (ManualRegistrationType::iterator iter = VisibleObjectsRealFiducials.begin();
         iter != VisibleObjectsRealFiducials.end();
         iter++) {
        if((iter->second)->Valid)
        {
            realFiducials.resize(fiducialIndex + 1);
            realFiducials[fiducialIndex] = this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse() *(iter->second)->GetAbsoluteTransformation().Translation();
            if (this->BooleanFlags[DEBUG])
                std::cerr << "Register real fiducial " << fiducialIndex << " at abs positionUI3 " << ((iter->second)->GetAbsoluteTransformation()).Translation()
                          << " relative position: " << ((iter->second)->GetTransformation()).Translation()
                          << " using " << realFiducials[fiducialIndex] << std::endl;
            fiducialIndex++;
        }
    }
    bool valid = nmrRegistrationRigid(virtualFiducials, realFiducials, displacementECMRCM,&fre);
    if (valid) {
        // apply transformation in ECMRCM
        this->VisibleListVirtual->SetTransformation(displacementECMRCM * this->VisibleListVirtual->GetTransformation());
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
    newFiducial = new ManualRegistrationSurfaceVisibleStippleObject(buffer,ManualRegistrationSurfaceVisibleStippleObject::SPHERE,3);

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
    newFiducial->Valid = true;

    if (virtualFlag) {
        newFiducial->SetColor(0.0,0.0,1.0);
        // set position wrt model
        newFiducial->SetTransformation((foundModel->second)->GetAbsoluteTransformation().Inverse()*positionUI3);
        CMN_LOG_CLASS_RUN_VERBOSE << "PrimaryMasterButtonCallback: added virtual fiducial: " << fiducialIndex << " "
                                  << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
        if (this->BooleanFlags[DEBUG])
            std::cerr << "Adding virtual fiducial " << fiducialIndex << " at positionUI3 " << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
    } else {
        newFiducial->SetColor(1.0,0.0,0.0);
        // set position wrt visibleListECMRCM
        newFiducial->SetTransformation(this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse()*positionUI3);
        CMN_LOG_CLASS_RUN_VERBOSE << "PrimaryMasterButtonCallback: added virtual fiducial: " << fiducialIndex << " "
                                  << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
        if (this->BooleanFlags[DEBUG])
            std::cerr << "Adding real fiducial " << fiducialIndex << " at positionUI3 " << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
    }

    ResetButtonEvents();
    UpdateVisibleList();
    return;   
}

/*!
find the closest marker to the cursor
*/

ManualRegistrationSurfaceVisibleStippleObject* ManualRegistration::FindClosestFiducial(vctFrm3 positionUI3, bool virtualFlag)
{
    vctFrm3 pos;
    double closestDist = cmnTypeTraits<double>::MaxPositiveValue();
    vctDouble3 dist;
    double abs;
    int currentCount = 0;
    ManualRegistrationSurfaceVisibleStippleObject* closestFiducial = NULL;

    if(virtualFlag)
    {
        for (ManualRegistrationType::iterator iter = VisibleObjectsVirtualFiducials.begin();
            iter != VisibleObjectsVirtualFiducials.end();
            iter++) {
                dist.DifferenceOf(positionUI3.Translation(), (iter->second)->GetAbsoluteTransformation().Translation());
                abs = dist.Norm();
                if(abs < closestDist)
                {
                    currentCount++;
                    closestDist = abs;
                    closestFiducial = (iter->second);
                }
        }
    }else{
        for (ManualRegistrationType::iterator iter = VisibleObjectsRealFiducials.begin();
            iter != VisibleObjectsRealFiducials.end();
            iter++) {
                dist.DifferenceOf(positionUI3.Translation(), (iter->second)->GetAbsoluteTransformation().Translation());
                abs = dist.Norm();
                if(abs < closestDist)
                {
                    currentCount++;
                    closestDist = abs;
                    closestFiducial = (iter->second);
                }
        }
    }

    //if there is one close to the cursor, turn it red
    //return value is that markers count
    //for(iter2 = Markers.begin(); iter2 !=end; iter2++)
    //{
    //    if(closestDist < 2.0 && (*iter2)->count == currentCount)
    //    {
    //        (*iter2)->VisibleObject->SetColor(255.0/255.0, 0.0/255.0, 51.0/255.0);
    //        returnValue = currentCount;
    //    }else{
    //         //otherwise, all the markers should be green, return an invalid number
    //        (*iter2)->VisibleObject->SetColor(153.0/255.0, 255.0/255.0, 153.0/255.0);
    //    }
    //}

    if(closestDist > 2.0)
    {
        closestFiducial = NULL;
    }
    else{
        if(this->BooleanFlags[DEBUG])
        std::cerr << "Found existing marker at index: " << currentCount << std::endl;
    }

    return closestFiducial;

}

void ManualRegistration::UpdateVisibleList()
{
    for (ManualRegistrationType::iterator iter = VisibleObjects.begin();
         iter != VisibleObjects.end();
         iter++) {
        if (this->BooleanFlags[VISIBLE] && (iter->second)->Valid && (iter->second)->Visible) {
            (iter->second)->Show();
        } else {
            (iter->second)->Hide();
        }
    }

    for (ManualRegistrationType::iterator iter = VisibleObjectsVirtualFiducials.begin();
         iter != VisibleObjectsVirtualFiducials.end();
         iter++) {
        if (this->BooleanFlags[VISIBLE] && (iter->second)->Valid && (iter->second)->Visible) {
            (iter->second)->Show();
        } else {
            (iter->second)->Hide();
        }
    }
    for (ManualRegistrationType::iterator iter = VisibleObjectsRealFiducials.begin();
         iter != VisibleObjectsRealFiducials.end();
         iter++) {
        if (this->BooleanFlags[VISIBLE] && (iter->second)->Valid && (iter->second)->Visible) {
            (iter->second)->Show();
        } else {
            (iter->second)->Hide();
        }
    }
}