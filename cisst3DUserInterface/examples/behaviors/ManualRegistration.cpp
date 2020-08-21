/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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
#define IMPORT_FIDUCIALS 1
#define FIDUCIAL_COUNT_MAX 30
// z-axis translation between tool eye and tip (mm) for debakey forceps
#define WRIST_TIP_OFFSET (11.0)

class ManualRegistrationSurfaceVisibleStippleObject: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    enum GeometryType {NONE=0,CUBE,SPHERE,CYLINDER};
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

    inline bool CreateVTKObjectCylinder()
    {
        vtkCylinderSource *source = vtkCylinderSource::New();
        source->SetHeight(5*this->Size);
        source->SetRadius(this->Size);
        SurfaceMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(SurfaceMapper);
        SurfaceMapper->SetInputConnection(source->GetOutputPort());
        SurfaceMapper->SetScalarRange(0,7);
        SurfaceMapper->ScalarVisibilityOff();
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
        switch (Geometry)
        {
        case CUBE:
            return CreateVTKObjectCube();
        break;
        case CYLINDER:
            return CreateVTKObjectCylinder();
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
    typedef std::map<size_t, vctFrm3> vctFrm3MapType;
    size_t PreviousIndex;
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
    size_t Size;
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
    ManualRegistrationSurfaceVisibleStippleObject * tumor;  
    this->Cursor = new ui3VisibleAxes;

#if CUBE_DEMO
    model = new ManualRegistrationSurfaceVisibleStippleObject("",ManualRegistrationSurfaceVisibleStippleObject::CUBE,25);
    tumor = new ManualRegistrationSurfaceVisibleStippleObject("",ManualRegistrationSurfaceVisibleStippleObject::CUBE,5);
#else
    //model = new ManualRegistrationSurfaceVisibleStippleObject("E:/Users/wliu25/MyCommon/data/TORS/TORS_tongue.vtk");
    model = new ManualRegistrationSurfaceVisibleStippleObject("E:/Users/wliu25/MyCommon/data/RedSkull/Red_Skull_CT_TORS_ROI_Resample1.vtk");
    //model = new ManualRegistrationSurfaceVisibleStippleObject("E:/Users/wliu25/MyCommon/data/20120223_TORS_Pig_Phantoms/20120223_TORS_PigTongue_sc4_c191100_ROI_Resample0.6.vtk");
    //model = new ManualRegistrationSurfaceVisibleStippleObject("E:/Users/wliu25/MyCommon/data/20120307_TORS_Pig_Phantoms/T3Final/BoneModel.vtk");

    //tumor = new ManualRegistrationSurfaceVisibleStippleObject("E:/Users/wliu25/MyCommon/data/20120223_TORS_Pig_Phantoms/20120223_TORS_PigTongue_sc4_c191100_Targets.vtk");
    //tumor = new ManualRegistrationSurfaceVisibleStippleObject("E:/Users/wliu25/MyCommon/data/20120307_TORS_Pig_Phantoms/T3Final/TargetPlanning.vtk");
#endif

    VisibleObjects[MODEL] = model;
    VisibleObjects[TUMOR] = tumor;

    for (ManualRegistrationType::iterator iter = VisibleObjects.begin();
        iter != VisibleObjects.end();
        iter++) {
            VisibleListVirtual->Add(iter->second);
    }

    VisibleListECMRCM->Add(VisibleListVirtual);
    VisibleListECMRCM->Add(VisibleListReal);
    VisibleListECM->Add(VisibleListECMRCM);
    VisibleList->Add(VisibleListECM);
    VisibleList->Add(this->Cursor);

    // Initialize boolean flags
    this->BooleanFlags[DEBUG] = true;
    this->BooleanFlags[VISIBLE] = true;
    this->BooleanFlags[PREVIOUS_MAM] = false;
    this->BooleanFlags[CAMERA_PRESSED] = false;
    this->BooleanFlags[CLUTCH_PRESSED] = false;
    this->BooleanFlags[UPDATE_FIDUCIALS] = false;
    ResetButtonEvents();

    this->VisibleToggle = ALL;
    this->FiducialToggle = MODEL;
    this->MeanTRE = 0.0;
    this->MaxTRE = -1.0;
    this->MeanTREProjection = 0.0;
    this->MaxTREProjection = -1.0;
    this->TREFiducialCount = 0;
    char file[50];
    sprintf(file,"tre%d.txt");
    TRE = fopen(file,"w");
    sprintf(file,"treProjection%d.txt");
    TREProjection = fopen(file,"w");
    sprintf(file,"treTriangulation%d.txt");
    TRETriangulation = fopen(file,"w");

    this->WristToTip.Translation().Assign(vctDouble3(0.0, 0.0, WRIST_TIP_OFFSET));
}


ManualRegistration::~ManualRegistration()
{
    fclose(TRE);
    fclose(TREProjection);
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
    bool debugLocal = false;
    // get current position in UI3
    ManualRegistrationType::iterator foundModel;
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel == VisibleObjects.end()) {
        return;
    }
    //previous positions saved in ECMRCM
    (foundModel->second)->PreviousPositions[(foundModel->second)->PreviousPositions.size()+1] = (foundModel->second)->GetAbsoluteTransformation();
    (foundModel->second)->PreviousIndex = (foundModel->second)->PreviousPositions.size();
    if (this->BooleanFlags[DEBUG] && debugLocal) {
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

    //hack
    fprintf(TRE,"meanTRE, maxTRE\n");
    fprintf(TRE,"%f, %f\n",MeanTRE/TREFiducialCount,MaxTRE);
    fprintf(TREProjection,"meanTREProjection, maxTREProjection\n"); 
    fprintf(TREProjection,"%f, %f\n",MeanTREProjection/(2*TREFiducialCount),MaxTREProjection);
    fclose(TRE);
    fclose(TREProjection);
    fclose(TRETriangulation);
}

void ManualRegistration::ToggleFiducials()
{
    switch(this->FiducialToggle)
    {
    case(MODEL):
        this->FiducialToggle = FIDUCIALS_REAL;
        this->BooleanFlags[UPDATE_FIDUCIALS] = true;
        break;
    case(FIDUCIALS_REAL):
        this->FiducialToggle = TARGETS_REAL;
        this->BooleanFlags[UPDATE_FIDUCIALS] = true;
        break;
    case(TARGETS_REAL):
        this->FiducialToggle = CALIBRATION_REAL;
        this->BooleanFlags[UPDATE_FIDUCIALS] = true;
        break;
    case(CALIBRATION_REAL):
        this->FiducialToggle = MODEL;
        this->BooleanFlags[UPDATE_FIDUCIALS] = false;
        break;
    default:
        this->FiducialToggle = MODEL;
        this->BooleanFlags[UPDATE_FIDUCIALS] = false;
        break;
    }

    if (this->BooleanFlags[DEBUG]) {
        std::cout << "Toggling Fiducial: " << this->FiducialToggle << std::endl;
    }
}

void ManualRegistration::ToggleVisibility(void)
{
    bool localDebug = false;
    ManualRegistrationType::iterator foundTumor;
    foundTumor = VisibleObjects.find(TUMOR);

    switch(this->VisibleToggle)
    {
    case(ALL):
        this->VisibleToggle = NO_FIDUCIALS;
        break;
    case(NO_FIDUCIALS):
        this->VisibleToggle = MODEL;
        break;
    case(MODEL):
        if(foundTumor != VisibleObjects.end())
            this->VisibleToggle = TUMOR;
        else
            this->VisibleToggle = TARGETS_REAL;
        break;
    case(TUMOR):
        this->VisibleToggle = TARGETS_REAL;
        break;
    case(TARGETS_REAL):
        this->VisibleToggle = FIDUCIALS_REAL;
        break;
    case(FIDUCIALS_REAL):
        this->VisibleToggle = CALIBRATION_REAL;
        break;
    default:
        this->VisibleToggle = ALL;
        break;
    }
    if (this->BooleanFlags[DEBUG] && localDebug) {
        std::cout << "Toggling Visible: " << this->VisibleToggle << std::endl;
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
    this->MenuBar->AddClickButton("ToggleFiducials",
        2,
        "map.png",
        &ManualRegistration::ToggleFiducials,
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
    prmPositionCartesianGet positionLeft, positionRight, position;;
    this->GetPrimaryMasterPosition(positionRight);
    this->GetSecondaryMasterPosition(positionLeft);
    ManualRegistrationSurfaceVisibleStippleObject* closestFiducial = NULL;
    VisibleObjectType type = ALL;
    bool addCalibrationVirtual = false;

    switch(this->FiducialToggle)
    {
    case(TARGETS_REAL):
        if(!this->BooleanFlags[BOTH_BUTTON_PRESSED] && this->BooleanFlags[RIGHT_BUTTON] && !this->BooleanFlags[LEFT_BUTTON]) {
            type = TARGETS_REAL;
            position = positionRight;
        }else if(!this->BooleanFlags[BOTH_BUTTON_PRESSED] && !this->BooleanFlags[RIGHT_BUTTON] && this->BooleanFlags[LEFT_BUTTON])
        {
            type = TARGETS_VIRTUAL;
            position = positionLeft;
        }
        break;
    case(FIDUCIALS_REAL):
        if(!this->BooleanFlags[BOTH_BUTTON_PRESSED] && this->BooleanFlags[RIGHT_BUTTON] && !this->BooleanFlags[LEFT_BUTTON]) {
            type = FIDUCIALS_REAL;
            position = positionRight;
        }else if(!this->BooleanFlags[BOTH_BUTTON_PRESSED] && !this->BooleanFlags[RIGHT_BUTTON] && this->BooleanFlags[LEFT_BUTTON])
        {
            type = FIDUCIALS_VIRTUAL;
            position = positionLeft;
        }
        break;
    case(CALIBRATION_REAL):
        if(!this->BooleanFlags[BOTH_BUTTON_PRESSED] && this->BooleanFlags[RIGHT_BUTTON] && !this->BooleanFlags[LEFT_BUTTON]) {
            type = CALIBRATION_REAL;
            position = positionRight;
            addCalibrationVirtual = true;
        //For now we are not calibrating left, use for delete virtuals
        }else if(!this->BooleanFlags[BOTH_BUTTON_PRESSED] && !this->BooleanFlags[RIGHT_BUTTON] && this->BooleanFlags[LEFT_BUTTON])
        {
            type = CALIBRATION_VIRTUAL;
            position = positionLeft;
            addCalibrationVirtual = false;
        }
        break;
    default:
        //std::cerr << "Doing nothing to update fiducial of this type: " << type << std::endl;
        return;        
    }

    int index;
    closestFiducial = FindClosestFiducial(position.Position(),type,index);
    if(closestFiducial != NULL && closestFiducial->Valid)
    {
        closestFiducial->Valid = false;
        if(this->FiducialToggle == CALIBRATION_REAL)
        {
            std::cout << "Trying to invalidate virual calibration at " << index << std::endl;
            ManualRegistrationType::iterator foundObject = VisibleObjectsVirtualCalibration.find(index);
            if(foundObject != VisibleObjectsVirtualCalibration.end())
                (foundObject->second)->Valid = false;
        }
        ResetButtonEvents();
        UpdateVisibleList();
    }else
    {
        AddFiducial(position.Position(),type);
        if(this->FiducialToggle == CALIBRATION_REAL && addCalibrationVirtual)
        {
            AddFiducial(GetCurrentCartesianPositionSlave(),CALIBRATION_VIRTUAL);
        }
        //std::cerr << "MaM position: " << position.Position().Translation() << " slave: " << GetCurrentCartesianPositionSlave().Translation() << std::endl;
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

    // prepare to drop marker if clutch and right MTM are pressed
    if ((this->BooleanFlags[CLUTCH_PRESSED] && this->BooleanFlags[RIGHT_BUTTON] && !this->BooleanFlags[RIGHT_BUTTON_RELEASED])
    &&((this->FiducialToggle == TARGETS_REAL || this->FiducialToggle == FIDUCIALS_REAL) || this->FiducialToggle == CALIBRATION_REAL))
    {
        //Add fiducial
        std::cerr << "Add marker with slave" << std::endl;
        if(this->FiducialToggle == CALIBRATION_REAL)
            AddFiducial(GetCurrentCartesianPositionSlave(),CALIBRATION_VIRTUAL);
        else
            AddFiducial(this->WristCalibration*GetCurrentCartesianPositionSlave()*this->WristToTip,this->FiducialToggle);
    }

    // update cursor
    this->Cursor->SetTransformation(this->WristCalibration*GetCurrentCartesianPositionSlave()*this->WristToTip);

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
    ManualRegistrationType::iterator foundObject;

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
#if CUBE_DEMO
    // VTK meshes harded coded start location at (0,0,-200)
    modelHomePosition.Translation().Assign(0.0,0.0,-200.0);
#else
    // VTK meshes harded coded start location at (0,0,-200)
    //modelHomePosition.Translation().Assign(100.0,100.0,-200.0);
    modelHomePosition.Translation().Assign(0.0,0.0,-200.0);
#endif
    this->VisibleListVirtual->SetTransformation(this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse()*modelHomePosition);

    // setup VTK model
    foundObject = VisibleObjects.find(MODEL);
    if (foundObject != VisibleObjects.end()) {
        (foundObject->second)->SetColor(1.0, 0.49, 0.25);
        (foundObject->second)->HomePositionUI3 = modelHomePosition;
        (foundObject->second)->SetStipplePercentage(50);
        (foundObject->second)->SetOpacity(0.7);
    }
    foundObject = VisibleObjects.find(TUMOR);
    if (foundObject != VisibleObjects.end()) {
        (foundObject->second)->SetColor(1.0, 0.0, 0.25);
        (foundObject->second)->HomePositionUI3 = modelHomePosition;
        (foundObject->second)->SetStipplePercentage(50);
        (foundObject->second)->SetOpacity(0.7);
    }

    for (ManualRegistrationType::iterator iter = VisibleObjects.begin();
        iter != VisibleObjects.end();
        iter++) {
            (iter->second)->Visible = true;
            (iter->second)->Valid = true;
            (iter->second)->Show();
    }

#if IMPORT_FIDUCIALS
#if CUBE_DEMO
    ImportFiducialFile("E:/Users/wliu25/MyCommon/data/TORS/CubeCTFids.fcsv", FIDUCIALS_VIRTUAL);
    ImportFiducialFile("E:/Users/wliu25/MyCommon/data/TORS/CubeCTTargets.fcsv", TARGETS_VIRTUAL);
#else
    //ImportFiducialFile("E:/Users/wliu25/MyCommon/data/TORS/TORSPhantomFiducialList.fcsv",FIDUCIALS_VIRTUAL);
    ImportFiducialFile("E:/Users/wliu25/MyCommon/data/RedSkull/TORSRegistrationFiducials.fcsv", FIDUCIALS_VIRTUAL);
    ImportFiducialFile("E:/Users/wliu25/MyCommon/data/RedSkull/TORSTargetFiducials.fcsv", TARGETS_VIRTUAL);
    //ImportFiducialFile("E:/Users/wliu25/MyCommon/data/20120223_TORS_Pig_Phantoms/20120223_TORS_PigTongue_sc4_c191100_ROI_Fiducials.fcsv", FIDUCIALS_VIRTUAL);
    //ImportFiducialFile("E:/Users/wliu25/MyCommon/data/20120307_TORS_Pig_Phantoms/T3Final/fiducials.fcsv", FIDUCIALS_VIRTUAL);
#endif
    ImportFiducialFile("E:/Users/wliu25/MyCommon/data/TORS/CubeCalibrationReal.fcsv", CALIBRATION_REAL);
#endif
    UpdateVisibleList();
}


void ManualRegistration::Startup(void) {

    // To get the joint values, we need to access the component directly
    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
    CMN_ASSERT(componentManager);
    mtsComponent * daVinci = componentManager->GetComponent("daVinci");
    CMN_ASSERT(daVinci);

    // get PSM1 interface
    mtsInterfaceProvided * interfaceProvided = daVinci->GetInterfaceProvided("PSM1");
    CMN_ASSERT(interfaceProvided);
    mtsCommandRead * command = interfaceProvided->GetCommandRead("measured_cp");
    CMN_ASSERT(command);
    GetCartesianPositionSlave.Bind(command);

    // get slave interface
    interfaceProvided = daVinci->GetInterfaceProvided("ECM1");
    CMN_ASSERT(interfaceProvided);
    command = interfaceProvided->GetCommandRead("GetPositionJoint");
    CMN_ASSERT(command);
    GetJointPositionECM.Bind(command);

    // get clutch interface
    interfaceProvided = daVinci->GetInterfaceProvided("Clutch");
    CMN_ASSERT(interfaceProvided);
    mtsCommandWrite<ManualRegistration, prmEventButton> * clutchCallbackCommand =
        new mtsCommandWrite<ManualRegistration, prmEventButton>(&ManualRegistration::MasterClutchPedalCallback, this, "Button", prmEventButton());
    CMN_ASSERT(clutchCallbackCommand);
    interfaceProvided->AddObserver("Button", clutchCallbackCommand);

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

vctFrm3 ManualRegistration::GetCurrentCartesianPositionSlave(void)
{
    // raw cartesian position from slave daVinci, no ui3 correction
    prmPositionCartesianGet slavePosition;
    GetCartesianPositionSlave(slavePosition);

    // Find first virtual object, i.e. Model
    ManualRegistrationType::iterator foundModel;
    foundModel = VisibleObjects.find(MODEL);
    vctFrm3 staticECMtoUI3, currentECMtoECMRCM, currentECMRCMtoUI3;
    staticECMtoUI3.Rotation().From(vctAxAnRot3(vctDouble3(0.0,1.0,0.0), cmnPI));
    currentECMtoECMRCM = GetCurrentECMtoECMRCM();
    currentECMRCMtoUI3 = staticECMtoUI3 * currentECMtoECMRCM.Inverse();     

    return staticECMtoUI3 * slavePosition.Position();
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

/*!
Function callback triggered by pressing the master cluch pedal
Changes the state of the behavior and allows some other features to become active
*/
void ManualRegistration::MasterClutchPedalCallback(const prmEventButton & payload)
{
    if (payload.Type() == prmEventButton::PRESSED) {
        this->BooleanFlags[CLUTCH_PRESSED] = true;
        if (this->BooleanFlags[DEBUG])
            std::cout << "Clutch pressed" << std::endl;
    } else {
        this->BooleanFlags[CLUTCH_PRESSED] = false;
    }
}

bool ManualRegistration::ImportFiducialFile(const std::string & inputFile, VisibleObjectType type)
{
    double count=0.0;
    vct3 positionFromFile;
    vctFrm3 fiducialPositionUI3, modelPositionUI3, altPositionUI3;
    altPositionUI3.Translation().Assign(0.0,0.0,-127.0);
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
		std::cerr << cmnData<std::vector<std::string> >::HumanReadable(token) << std::endl;
        if (token.at(0).compare(0,1,"#")) {
            if (token.size() < 4)
                return false;
            //assume fiducials are given wrt to model
            positionFromFile =  vct3(strtod(token.at(1).c_str(), NULL),strtod(token.at(2).c_str(), NULL),strtod(token.at(3).c_str(), NULL));
            fiducialPositionUI3.Translation().Assign((foundModel->second)->GetAbsoluteTransformation() * positionFromFile);
            //Add random rotation
            if(type == CALIBRATION_REAL)
            {
                fiducialPositionUI3.Rotation().From(vctAxAnRot3(vctDouble3(1.0,0.0,0.0), cmnPI/(count+1)));
            }
            AddFiducial(fiducialPositionUI3, type);
            ////HACK TO TEST REGISTRATION WITH STATIC REAL FIDUCIALS - REMOVE!!!
            //fiducialPositionUI3.Translation().Assign(altPositionUI3 * positionFromFile);
            //AddFiducial(fiducialPositionUI3, false);
            count++;
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
    vctFrm3 displacement, currentUI3toECMRCM;
    vctDynamicVector<vct3> fiducialsVirtual;
    vctDynamicVector<vct3> fiducialsReal;
    currentUI3toECMRCM = this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse();

    // Register fiducials
    GetFiducials(fiducialsVirtual, fiducialsReal,FIDUCIALS_REAL, ECMRCM);
    double * fre = new double[3*fiducialsVirtual.size()];
    memset(fre, 0, 3*fiducialsVirtual.size() * sizeof(double));

    bool valid = nmrRegistrationRigid(fiducialsVirtual, fiducialsReal, displacement,fre);
    if (valid) {
        // apply transformation in ECMRCM
        this->VisibleListVirtual->SetTransformation(displacement * this->VisibleListVirtual->GetTransformation());
        std::cerr << "Registered using # " << fiducialsReal.size() << " fiducials with fre: "<< fre[0] << std::endl;
    } else {
        std::cerr << "ERROR:ManualRegistration::Register() with # " << fiducialsReal.size() << " real, " << fiducialsVirtual.size()<< " virtual, see log" << std::endl;
    }  

    // Calibrate
    GetFiducials(fiducialsVirtual, fiducialsReal,CALIBRATION_REAL,UI3);
    fre = new double[3*fiducialsVirtual.size()];
    memset(fre, 0, 3*fiducialsVirtual.size() * sizeof(double));
    valid = nmrRegistrationRigid(fiducialsVirtual, fiducialsReal, displacement,fre);
    if (valid) {
        // save calibration transformation
        this->WristCalibration = displacement;
        std::cerr << "Calibrated wrist using # " << fiducialsReal.size() << " fiducials with fre: "<< fre[0] << std::endl;
    } else {
        std::cerr << "ERROR:ManualRegistration::Register() with # " << fiducialsReal.size() << " real, " << fiducialsVirtual.size()<< " virtual, see log" << std::endl;
    }  

    ComputeTRE();
}

void ManualRegistration::ComputeTRE()
{
    //Get fiducials
    vctDynamicVector<vct3> targetsVirtual, targetsReal, targetsRealCamera;
    GetFiducials(targetsVirtual,targetsReal,TARGETS_REAL,ECMRCM);
    targetsRealCamera.resize(targetsReal.size());


    //Error checking
    if(targetsVirtual.size() <= 0 || (targetsVirtual.size() != targetsReal.size()))// ||(targetsVirtual.size() != targetsReal.size()/2)))
    {
        std::cerr << "ERROR: ComputeTRE(): virtual#" << targetsVirtual.size() << " real#: " << targetsReal.size() << std::endl;
        return;
    }

    //Get camera parameters
    vctDynamicVector<vctDouble3> error, projectionErrorLeft, projectionErrorRight, errorTriangulation;
    double mTRE=0, maxTRE=-1, mTREProjection=0, maxTREProjection=-1, mTRETriangulation=0, maxTRETriangulation=-1;
    error.resize(targetsVirtual.size());
    errorTriangulation.resize(targetsVirtual.size());
    projectionErrorLeft.resize(targetsVirtual.size());
    projectionErrorRight.resize(targetsVirtual.size());
    vctDoubleMatRot3 intrinsicsLeft, intrinsicsRight;
    double fcx,fcy,ccx,ccy,a,kc0,kc1,kc2,kc3,kc4;
    int result;
    ui3VTKRenderer * rendererLeft = this->Manager->GetRenderer(SVL_LEFT);
    svlCameraGeometry cameraGeometry = rendererLeft->GetCameraGeometry(); //same geometry stores both left and right, pass in camid
    vctDoubleFrm4x4 extrinsicsLeft(cameraGeometry.GetExtrinsics(SVL_LEFT).frame);
    vctDoubleFrm4x4 extrinsicsRight(cameraGeometry.GetExtrinsics(SVL_RIGHT).frame);
    result = cameraGeometry.GetIntrinsics(fcx,fcy,ccx,ccy,a,kc0,kc1,kc2,kc3,kc4,SVL_LEFT);
    if(result != SVL_OK)
        std::cerr << "ERROR: ComputeTRE, GetIntrinsics(SVL_LEFT)" << std::endl;
    intrinsicsLeft = vctDoubleMatRot3::Identity();
    intrinsicsLeft.Assign(fcx,a,ccx,0.0,fcy,ccy,0.0,0.0,1.0);
    result = cameraGeometry.GetIntrinsics(fcx,fcy,ccx,ccy,a,kc0,kc1,kc2,kc3,kc4,SVL_RIGHT);
    intrinsicsRight = vctDoubleMatRot3::Identity();
    intrinsicsRight.Assign(fcx,a,ccx,0.0,fcy,ccy,0.0,0.0,1.0);
    if(result != SVL_OK)
        std::cerr << "ERROR: ComputeTRE, GetIntrinsics(SVL_RIGHT)" << std::endl;

    //camera pose (R,t)
    //intrinsic k
    //PtImage = virtual
    //ray r
    //r = R*inv(k)*PtImage-t
    vctDouble3 r, projection;
    for(int i=0;i<(int)targetsVirtual.size();i++)
    {
        //3d point error
        //targetsRealCamera[i] = vct3((targetsReal[j].X()+targetsReal[j+1].X())/2,(targetsReal[j].Y()+targetsReal[j+1].Y())/2,(targetsReal[j].Z()+targetsReal[j+1].Z())/2);
        error[i] = targetsReal[i]-targetsVirtual[i];
        double errorL2Norm = sqrt(error[i].NormSquare());
        mTRE += errorL2Norm;
        fprintf(TRE,"%f, %f, %f\n",error[i].X(),error[i].Y(),error[i].Z());
        if(errorL2Norm > maxTRE)
            maxTRE = errorL2Norm;

        //project on camera left
        //RayRayIntersect(translationLeft,targetsReal[j], translationRight, targetsReal[j+1], pointA, pointB);
        //targetsRealCamera[i] = vct3((pointA.X()+pointB.X())/2,(pointA.Y()+pointB.Y())/2,(pointA.Z()+pointB.Z())/2);
        //std::cerr << "triangulation: " << targetsRealCamera[i]<< std::endl;
        //errorTriangulation[i] = targetsRealCamera[i]-targetsVirtual[i];
        //double errorTriangulationL2Norm = sqrt(errorTriangulation[i].NormSquare());
        //mTRETriangulation += errorTriangulationL2Norm;
        //fprintf(TRETriangulation,"%f, %f, %f\n",errorTriangulation[i].X(),errorTriangulation[i].Y(),errorTriangulation[i].Z());
        //if(errorTriangulationL2Norm > maxTRETriangulation)
        //   maxTRETriangulation = errorTriangulationL2Norm;
        //c=cbct=virtual,p=translation,q=real; q-p = r
        //l = np.dot((c-p), (q-p)) / np.dot((q-p), (q-p))
        //return p + (l * (q-p))      
        r = targetsReal[i]-extrinsicsLeft.Translation();
        projection = extrinsicsLeft.Translation() + (((targetsVirtual[i]-extrinsicsLeft.Translation()).DotProduct(r))/(r.DotProduct(r)))*r;
        projectionErrorLeft[i] = targetsVirtual[i]-projection;
        fprintf(TREProjection,"%f, %f, %f\n",projectionErrorLeft[i].X(),projectionErrorLeft[i].Y(),projectionErrorLeft[i].Z());
        double projectionErrorL2Norm = sqrt(projectionErrorLeft[i].NormSquare());
        mTREProjection += projectionErrorL2Norm;
        if(projectionErrorL2Norm > maxTREProjection)
            maxTREProjection = projectionErrorL2Norm; 

        if(this->BooleanFlags[DEBUG])
        {
            std::cerr << "virtual: " << i << " " << targetsVirtual[i] << std::endl;
            //std::cerr << "real: " << i << " " << targetsReal[i]<< std::endl;
            //std::cerr << "projection: " << i << " " << projection<< std::endl;
            std::cerr << "error: " << i << " " << error[i] << std::endl;
            //std::cerr << "projection error: " << i << " " << projectionError[i] << std::endl;
            //PtCBCT = real
            //distance d
            //d=abs|PtCBCT-(t+[(r.(PtCBCT-t))/(r.r))]*r)|
            //std::cerr << "error L1Norm: " << i << " " << error[i].L1Norm()<< std::endl;
            std::cerr << "error L2Norm: " << i << " " << errorL2Norm << std::endl;
            //std::cerr << "error L2NormTriangulation: " << i << " " << errorTriangulationL2Norm << std::endl;
            //std::cerr << "projection error L1Norm: " << i << " " << projectionErrorLeft[i].L1Norm()<< std::endl;
            std::cerr << "projection error left L2Norm: " << i << " " << projectionErrorL2Norm<< std::endl;
        }

        r = targetsReal[i]-extrinsicsRight.Translation();
        projection = extrinsicsRight.Translation() + (((targetsVirtual[i]-extrinsicsRight.Translation()).DotProduct(r))/(r.DotProduct(r)))*r;
        projectionErrorRight[i] = targetsVirtual[i]-projection;
        fprintf(TREProjection,"%f, %f, %f\n",projectionErrorRight[i].X(),projectionErrorRight[i].Y(),projectionErrorRight[i].Z());
        projectionErrorL2Norm = sqrt(projectionErrorRight[i].NormSquare());
        mTREProjection += projectionErrorL2Norm;
        if(projectionErrorL2Norm > maxTREProjection)
            maxTREProjection = projectionErrorL2Norm;

        if(this->BooleanFlags[DEBUG])
        {
            //std::cerr << "virtual: " << i << " " << targetsVirtual[i]<< std::endl;
            //std::cerr << "real: " << i << " " << targetsReal[i]<< std::endl;
            //std::cerr << "projection: " << i << " " << projection<< std::endl;
            //std::cerr << "error: " << i << " " << error[i] << std::endl;
            //std::cerr << "projection error: " << i << " " << projectionError[i] << std::endl;
            //PtCBCT = real
            //distance d
            //d=abs|PtCBCT-(t+[(r.(PtCBCT-t))/(r.r))]*r)|
            //std::cerr << "projection error L1Norm: " << i << " " << projectionErrorRight[i].L1Norm()<< std::endl;
            std::cerr << "projection error right L2Norm: " << i << " " << projectionErrorL2Norm << std::endl;
        }
    }
    std::cerr << "===========================Before All=========================="<<std::endl;
    std::cerr << "MeanTRE: " << MeanTRE/TREFiducialCount << " maxTRE: " << MaxTRE << " count " << TREFiducialCount << std::endl;
    std::cerr << "MeanTRETriangulation: " << MeanTRETriangulation/TREFiducialCount << " maxTRE: " << MaxTRETriangulation << std::endl;
    std::cerr << "MeanTREProjection: " << MeanTREProjection/(2*TREFiducialCount) << " maxTREProjection: " << MaxTREProjection << std::endl;
    std::cerr << "====================================================="<<std::endl;

    //update mean TRE
    this->MeanTRE += mTRE;
    this->MeanTREProjection += mTREProjection;
    this->MeanTRETriangulation += mTRETriangulation;
    this->TREFiducialCount += targetsVirtual.size();
    if(maxTRE > this->MaxTRE)
        this->MaxTRE = maxTRE;
    if(maxTREProjection > this->MaxTREProjection)
        this->MaxTREProjection = maxTREProjection;
    if(maxTRETriangulation > this->MaxTREProjection)
        this->MaxTRETriangulation = maxTRETriangulation;

    std::cerr << "========================After local============================="<<std::endl;
    std::cerr << "mTRE: " << mTRE/targetsVirtual.size() << " maxTRE: " << maxTRE << std::endl;
    std::cerr << "mTRETriangulation: " << mTRETriangulation/targetsVirtual.size() << " maxTRE: " << maxTRETriangulation << std::endl;
    std::cerr << "mTREProjection: " << mTREProjection/(2*targetsVirtual.size()) << " maxTREProjection: " << maxTREProjection << std::endl;
    std::cerr << "======================After All==============================="<<std::endl;
    std::cerr << "MeanTRE: " << MeanTRE/TREFiducialCount << " maxTRE: " << MaxTRE << " count " << TREFiducialCount << std::endl;
    std::cerr << "MeanTRETriangulation: " << MeanTRETriangulation/TREFiducialCount << " maxTRE: " << MaxTRETriangulation << std::endl;
    std::cerr << "MeanTREProjection: " << MeanTREProjection/(2*TREFiducialCount) << " maxTREProjection: " << MaxTREProjection << std::endl;
}

/*
Calculate the line segment PaPb that is the shortest route between
two rays P1P2 and P3P4.
Pa = P1 + mua (P2 - P1)
Pb = P3 + mub (P4 - P3)
Return FALSE if no solution exists.
*/
bool ManualRegistration::RayRayIntersect(vctDouble3 p1,vctDouble3 p2,vctDouble3 p3,vctDouble3 p4,vctDouble3 &pa,vctDouble3 &pb)
{
    vctDouble3 p13,p43,p21;
    double d1343,d4321,d1321,d4343,d2121;
    double numer,denom;
    double mua, mub;
    double eps = 0.0000001;

    p13.X() = p1.X() - p3.X();
    p13.Y() = p1.Y() - p3.Y();
    p13.Z() = p1.Z() - p3.Z();
    p43.X() = p4.X() - p3.X();
    p43.Y() = p4.Y() - p3.Y();
    p43.Z() = p4.Z() - p3.Z();
    if (fabs(p43.X()) < eps && fabs(p43.Y()) < eps && fabs(p43.Z()) < eps)
        return false;
    p21.X() = p2.X() - p1.X();
    p21.Y() = p2.Y() - p1.Y();
    p21.Z() = p2.Z() - p1.Z();
    if (fabs(p21.X()) < eps && fabs(p21.Y()) < eps && fabs(p21.Z()) < eps)
        return false;

    d1343 = p13.X() * p43.X() + p13.Y() * p43.Y() + p13.Z() * p43.Z();
    d4321 = p43.X() * p21.X() + p43.Y() * p21.Y() + p43.Z() * p21.Z();
    d1321 = p13.X() * p21.X() + p13.Y() * p21.Y() + p13.Z() * p21.Z();
    d4343 = p43.X() * p43.X() + p43.Y() * p43.Y() + p43.Z() * p43.Z();
    d2121 = p21.X() * p21.X() + p21.Y() * p21.Y() + p21.Z() * p21.Z();

    denom = d2121 * d4343 - d4321 * d4321;
    if (fabs(denom) < eps)
        return false;
    numer = d1343 * d4321 - d1321 * d4343;

    mua = numer / denom;
    mub = (d1343 + d4321 * mua) / d4343;

    pa.X() = p1.X() + mua * p21.X();
    pa.Y() = p1.Y() + mua * p21.Y();
    pa.Z() = p1.Z() + mua * p21.Z();
    pb.X() = p3.X() + mub * p43.X();
    pb.Y() = p3.Y() + mub * p43.Y();
    pb.Z() = p3.Z() + mub * p43.Z();

    return true;
}

void ManualRegistration::GetFiducials(vctDynamicVector<vct3>& fiducialsVirtual, vctDynamicVector<vct3>& fiducialsReal, VisibleObjectType type, Frame frame)
{
    int fiducialIndex = 0;
    bool debugLocal = false;
    ManualRegistrationType localVisibleObjectsVirtual,localVisibleObjectsReal;

    switch(type)
    {
    case(TARGETS_REAL):
        localVisibleObjectsReal = VisibleObjectsRealTargets;
        localVisibleObjectsVirtual = VisibleObjectsVirtualTargets;
        break;
    case(FIDUCIALS_REAL):
        localVisibleObjectsReal = VisibleObjectsRealFiducials;
        localVisibleObjectsVirtual = VisibleObjectsVirtualFiducials;
        break;
    case(CALIBRATION_REAL):
        localVisibleObjectsReal = VisibleObjectsRealCalibration;
        localVisibleObjectsVirtual = VisibleObjectsVirtualCalibration;
        break;
    default:
        //std::cerr << "ERROR: Cannot find fiducial of this type: " << type << std::endl;
        return;        
    }

    //real
    for (ManualRegistrationType::iterator iter = localVisibleObjectsReal.begin();
        iter != localVisibleObjectsReal.end() && fiducialsReal.size() < FIDUCIAL_COUNT_MAX;
        iter++) {
            if((iter->second)->Valid)
            {
                fiducialsReal.resize(fiducialIndex + 1);
                switch(frame)
                {
                case(ECMRCM):
                    fiducialsReal[fiducialIndex] = this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse() *(iter->second)->GetAbsoluteTransformation().Translation();
                    break;
                case(UI3):
                    fiducialsReal[fiducialIndex] = (iter->second)->GetAbsoluteTransformation().Translation();
                    break;
                default:
                    fiducialsReal[fiducialIndex] = this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse() *(iter->second)->GetAbsoluteTransformation().Translation();
                    break;
                }
                fiducialIndex++;
                if (this->BooleanFlags[DEBUG] && debugLocal)
                    std::cerr << "Getting real fiducial " << fiducialIndex << " at abs positionUI3 " << ((iter->second)->GetAbsoluteTransformation()).Translation()
                    << " relative position: " << ((iter->second)->GetTransformation()).Translation()
                    << " returning " << fiducialsReal[fiducialIndex] << std::endl;
            }
    }

    fiducialIndex = 0;

    //virtual
    for (ManualRegistrationType::iterator iter = localVisibleObjectsVirtual.begin();
        iter != localVisibleObjectsVirtual.end() && fiducialsVirtual.size() < FIDUCIAL_COUNT_MAX;
        iter++) {
            //for fiducials returns no more than # of real fiducials - size must equal for nmrRegistrationRigid
            if((iter->second)->Valid && (fiducialsVirtual.size() < fiducialsReal.size()))
            {
                fiducialsVirtual.resize(fiducialIndex + 1);
                switch(frame)
                {
                case(ECMRCM):
                    fiducialsVirtual[fiducialIndex] = this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse() *(iter->second)->GetAbsoluteTransformation().Translation();
                    break;
                case(UI3):
                    fiducialsVirtual[fiducialIndex] = (iter->second)->GetAbsoluteTransformation().Translation();
                    break;
                default:
                    fiducialsVirtual[fiducialIndex] = this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse() *(iter->second)->GetAbsoluteTransformation().Translation();
                    break;
                }
                fiducialIndex++;
                if (this->BooleanFlags[DEBUG] && debugLocal)
                    std::cerr << "Getting virtual fiducial " << fiducialIndex << " at abs positionUI3 " << ((iter->second)->GetAbsoluteTransformation()).Translation()
                    << " relative position: " << ((iter->second)->GetTransformation()).Translation()
                    << " returning " << fiducialsVirtual[fiducialIndex] << std::endl;
            }
    }
}

void ManualRegistration::AddFiducial(vctFrm3 positionUI3, VisibleObjectType type)
{
    size_t fiducialIndex;
    ManualRegistrationType::iterator foundModel;

    // Find first virtual object, i.e. Model
    foundModel = VisibleObjects.find(MODEL);
    if (foundModel == VisibleObjects.end())
        return;

    // Get fiducial index
    switch(type)
    {
    case(TARGETS_REAL):
        fiducialIndex = VisibleObjectsRealTargets.size(); 
        break;
    case(TARGETS_VIRTUAL):
        fiducialIndex = VisibleObjectsVirtualTargets.size(); 
        break;
    case(FIDUCIALS_REAL):
        fiducialIndex = VisibleObjectsRealFiducials.size(); 
        break;
    case(FIDUCIALS_VIRTUAL):
        fiducialIndex = VisibleObjectsVirtualFiducials.size();
        break;
    case(CALIBRATION_REAL):
        fiducialIndex = VisibleObjectsRealCalibration.size();
        break;
    case(CALIBRATION_VIRTUAL):
        fiducialIndex = VisibleObjectsVirtualCalibration.size();
        break;
    default:
        //std::cerr << "ERROR: Cannot add fiducial of this type: " << type << std::endl;
        return;        
    }

    // create new visibleObject for fiducial
    ManualRegistrationSurfaceVisibleStippleObject * newFiducial;
    char buffer[33];
    sprintf(buffer, "%d", fiducialIndex);

    // add visibleObject to visibleList and visibleObjects
    switch(type)
    {
    case(TARGETS_REAL):
        newFiducial = new ManualRegistrationSurfaceVisibleStippleObject(buffer,ManualRegistrationSurfaceVisibleStippleObject::SPHERE,2);
        VisibleObjectsRealTargets[fiducialIndex] = newFiducial;
        this->VisibleListReal->Add(newFiducial);
        break;
    case(TARGETS_VIRTUAL):
        newFiducial = new ManualRegistrationSurfaceVisibleStippleObject(buffer,ManualRegistrationSurfaceVisibleStippleObject::SPHERE,2);
        VisibleObjectsVirtualTargets[fiducialIndex] = newFiducial;
        this->VisibleListVirtual->Add(newFiducial);
        break;
    case(FIDUCIALS_REAL):
        newFiducial = new ManualRegistrationSurfaceVisibleStippleObject(buffer,ManualRegistrationSurfaceVisibleStippleObject::SPHERE,2);
        VisibleObjectsRealFiducials[fiducialIndex] = newFiducial;
        this->VisibleListReal->Add(newFiducial);
        break;
    case(FIDUCIALS_VIRTUAL):
        newFiducial = new ManualRegistrationSurfaceVisibleStippleObject(buffer,ManualRegistrationSurfaceVisibleStippleObject::SPHERE,2);
        VisibleObjectsVirtualFiducials[fiducialIndex] = newFiducial;
        this->VisibleListVirtual->Add(newFiducial);
        break;
    case(CALIBRATION_REAL):
        newFiducial = new ManualRegistrationSurfaceVisibleStippleObject(buffer,ManualRegistrationSurfaceVisibleStippleObject::CYLINDER,1);
        VisibleObjectsRealCalibration[fiducialIndex] = newFiducial;
        this->VisibleListReal->Add(newFiducial);
        break;
    case(CALIBRATION_VIRTUAL):
        newFiducial = new ManualRegistrationSurfaceVisibleStippleObject(buffer,ManualRegistrationSurfaceVisibleStippleObject::CYLINDER,1);
        VisibleObjectsVirtualCalibration[fiducialIndex] = newFiducial;
        this->VisibleListVirtual->Add(newFiducial);
        break;
    default:
        return;        
    }

    newFiducial->WaitForCreation();
    newFiducial->SetStipplePercentage(50);
    newFiducial->SetOpacity(0.7);
    newFiducial->Show();
    newFiducial->Visible = true;
    newFiducial->Valid = true;

    switch(type)
    {
    case(TARGETS_REAL):
        newFiducial->SetColor(0.0,1.0,0.0);
        // set position wrt visibleListECMRCM
        newFiducial->SetTransformation(this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse()*positionUI3);
        CMN_LOG_CLASS_RUN_VERBOSE << "PrimaryMasterButtonCallback: added real target: " << fiducialIndex << " "
            << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
        if (this->BooleanFlags[DEBUG])
            std::cerr << "Adding real target " << fiducialIndex << " at positionUI3 " << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
        break;
    case(TARGETS_VIRTUAL):
        newFiducial->SetColor(0.0,0.0,1.0);
        // set position wrt model
        newFiducial->SetTransformation((foundModel->second)->GetAbsoluteTransformation().Inverse()*positionUI3);
        CMN_LOG_CLASS_RUN_VERBOSE << "PrimaryMasterButtonCallback: added virtual target: " << fiducialIndex << " "
            << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
        if (this->BooleanFlags[DEBUG])
            std::cerr << "Adding virtual target " << fiducialIndex << " at positionUI3 " << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
        break;
    case(FIDUCIALS_REAL):
        newFiducial->SetColor(0.75,1.0,0.75);
        // set position wrt visibleListECMRCM
        newFiducial->SetTransformation(this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse()*positionUI3);
        CMN_LOG_CLASS_RUN_VERBOSE << "PrimaryMasterButtonCallback: added real fiducial: " << fiducialIndex << " "
            << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
        if (this->BooleanFlags[DEBUG])
            std::cerr << "Adding real fiducial " << fiducialIndex << " at positionUI3 " << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
        break;
    case(FIDUCIALS_VIRTUAL):
        newFiducial->SetColor(0.75,0.75,1.0);
        // set position wrt model
        newFiducial->SetTransformation((foundModel->second)->GetAbsoluteTransformation().Inverse()*positionUI3);
        CMN_LOG_CLASS_RUN_VERBOSE << "PrimaryMasterButtonCallback: added virtual fiducial: " << fiducialIndex << " "
            << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
        if (this->BooleanFlags[DEBUG])
            std::cerr << "Adding virtual fiducial " << fiducialIndex << " at positionUI3 " << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
        break;
    case(CALIBRATION_REAL):
        newFiducial->SetColor(1.0,0.0,0.0);
        // set position wrt visibleListECMRCM
        newFiducial->SetTransformation(this->VisibleListECMRCM->GetAbsoluteTransformation().Inverse()*positionUI3);
        CMN_LOG_CLASS_RUN_VERBOSE << "PrimaryMasterButtonCallback: added calibration real: " << fiducialIndex << " "
            << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
        if (this->BooleanFlags[DEBUG])
            std::cerr << "Adding calibration real " << fiducialIndex << " at positionUI3 " << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
        break;
    case(CALIBRATION_VIRTUAL):
        newFiducial->SetColor(1.0,0.75,0.75);
        // set position wrt model
        newFiducial->SetTransformation((foundModel->second)->GetAbsoluteTransformation().Inverse()*positionUI3);
        CMN_LOG_CLASS_RUN_VERBOSE << "PrimaryMasterButtonCallback: added calibration virtual : " << fiducialIndex << " "
            << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
        if (this->BooleanFlags[DEBUG])
            std::cerr << "Adding calibration virtual " << fiducialIndex << " at positionUI3 " << newFiducial->GetAbsoluteTransformation().Translation() << std::endl;
        break;

    default:
        std::cerr << "ERROR: Cannot add fiducial of this type: " << type << std::endl;
        return;        
    }

    ResetButtonEvents();
    UpdateVisibleList();
    return;   
}

/*!
find the closest marker to the cursor
*/

ManualRegistrationSurfaceVisibleStippleObject* ManualRegistration::FindClosestFiducial(vctFrm3 positionUI3, VisibleObjectType type, int& index)
{
    vctFrm3 pos;
    double closestDist = cmnTypeTraits<double>::MaxPositiveValue();
    vctDouble3 dist;
    double abs;
    int currentCount = 0;
    ManualRegistrationSurfaceVisibleStippleObject* closestFiducial = NULL;
    ManualRegistrationType localVisibleObjects;

    switch(type)
    {
    case(TARGETS_REAL):
        localVisibleObjects = VisibleObjectsRealTargets;
        break;
    case(TARGETS_VIRTUAL):
        localVisibleObjects = VisibleObjectsVirtualTargets;
        break;
    case(FIDUCIALS_REAL):
        localVisibleObjects = VisibleObjectsRealFiducials;
        break;
    case(FIDUCIALS_VIRTUAL):
        localVisibleObjects = VisibleObjectsVirtualFiducials;
        break;
    case(CALIBRATION_REAL):
        localVisibleObjects = VisibleObjectsRealCalibration;
        break;
   case(CALIBRATION_VIRTUAL):
        localVisibleObjects = VisibleObjectsVirtualCalibration;
        break;
    default:
        //std::cerr << "ERROR: Cannot find fiducial of this type: " << type << std::endl;
        return NULL;        
    }

    index = -1;
    for (ManualRegistrationType::iterator iter = localVisibleObjects.begin();
        iter != localVisibleObjects.end();
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

    if(closestDist > 5.0)
    {
        closestFiducial = NULL;
    }
    else{
        if(this->BooleanFlags[DEBUG])
            std::cerr << "Found existing marker at index: " << currentCount << std::endl;
        index = currentCount;
    }

    return closestFiducial;

}

void ManualRegistration::UpdateVisibleList()
{
    for (ManualRegistrationType::iterator iter = VisibleObjects.begin();
        iter != VisibleObjects.end();
        iter++) {
            if (this->BooleanFlags[VISIBLE] && (iter->second)->Valid && (iter->second)->Visible &&
                (VisibleToggle == ALL || (VisibleToggle == NO_FIDUCIALS) ||VisibleToggle == iter->first)) {
                    (iter->second)->Show();
            } else {
                (iter->second)->Hide();
            }
    }

    if(VisibleToggle == ALL)
    {
        this->Cursor->Show();
    }else
    {
        this->Cursor->Hide();
    }

    for (ManualRegistrationType::iterator iter = VisibleObjectsVirtualFiducials.begin();
        iter != VisibleObjectsVirtualFiducials.end();
        iter++) {
            if (this->BooleanFlags[VISIBLE] && (iter->second)->Valid && (iter->second)->Visible && 
                (VisibleToggle == ALL || VisibleToggle == FIDUCIALS_REAL || VisibleToggle == FIDUCIALS_VIRTUAL)) {
                    (iter->second)->Show();
            } else {
                (iter->second)->Hide();
            }
    }
    for (ManualRegistrationType::iterator iter = VisibleObjectsRealFiducials.begin();
        iter != VisibleObjectsRealFiducials.end();
        iter++) {
            if (this->BooleanFlags[VISIBLE] && (iter->second)->Valid && (iter->second)->Visible&& 
                (VisibleToggle == ALL || VisibleToggle == FIDUCIALS_REAL || VisibleToggle == FIDUCIALS_VIRTUAL)) 
            {
                (iter->second)->Show();
            } else {
                (iter->second)->Hide();
            }
    }
    for (ManualRegistrationType::iterator iter = VisibleObjectsVirtualTargets.begin();
        iter != VisibleObjectsVirtualTargets.end();
        iter++) {
            if (this->BooleanFlags[VISIBLE] && (iter->second)->Valid && (iter->second)->Visible && 
                (VisibleToggle == ALL || VisibleToggle == TARGETS_REAL || VisibleToggle == TARGETS_VIRTUAL)) {
                    (iter->second)->Show();
            } else {
                (iter->second)->Hide();
            }
    }
    for (ManualRegistrationType::iterator iter = VisibleObjectsRealTargets.begin();
        iter != VisibleObjectsRealTargets.end();
        iter++) {
            if (this->BooleanFlags[VISIBLE] && (iter->second)->Valid && (iter->second)->Visible&& 
                (VisibleToggle == ALL || VisibleToggle == TARGETS_REAL || VisibleToggle == TARGETS_VIRTUAL)) 
            {
                (iter->second)->Show();
            } else {
                (iter->second)->Hide();
            }
    }

    for (ManualRegistrationType::iterator iter = VisibleObjectsVirtualCalibration.begin();
        iter != VisibleObjectsVirtualCalibration.end();
        iter++) {
            if (this->BooleanFlags[VISIBLE] && (iter->second)->Valid && (iter->second)->Visible&& 
                (VisibleToggle == ALL || VisibleToggle == CALIBRATION_REAL || VisibleToggle == CALIBRATION_VIRTUAL)) 
            {
                (iter->second)->Show();
            } else {
                (iter->second)->Hide();
            }
    }

    for (ManualRegistrationType::iterator iter = VisibleObjectsRealCalibration.begin();
        iter != VisibleObjectsRealCalibration.end();
        iter++) {
            if (this->BooleanFlags[VISIBLE] && (iter->second)->Valid && (iter->second)->Visible&& 
                (VisibleToggle == ALL || VisibleToggle == CALIBRATION_REAL || VisibleToggle == CALIBRATION_VIRTUAL)) 
            {
                (iter->second)->Show();
            } else {
                (iter->second)->Hide();
            }
    }
}
