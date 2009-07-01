/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: BehaviorLUS.cpp 309 2009-05-05 01:26:24Z adeguet1 $

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-05-23

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaThreadedLogFile.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstDevices/devSensableHD.h>

#include "BehaviorLUS.h"

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkConeSource.h>
#include <vtkSphereSource.h>
#include "vtkCylinderSource.h" //this and beyond were added for the LUS Widget
#include "vtkSTLReader.h"
#include "vtkTransform.h"
#include "vtkOutlineSource.h"
#include "vtkSphereSource.h"
#include "vtkTextSource.h"
#include "vtkTextMapper.h"
#include "vtkTextActor3D.h"
#include "CSOpenGLStippleActor.h"
#include "vtkVectorText.h"
#include "vtkFollower.h"
#include "vtkTextActor.h"
#include "vtkCubeSource.h"
#include "CSOpenGLStippleActor.h"
#include "vtkMatrix4x4.h"
#include "vtkTextSource.h"
#include "vtkAxesActor.h"

#define SCALE .5

class BehaviorLUSProbeHead: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
public:
      inline BehaviorLUSProbeHead(ui3Manager * manager, vctFrm3 position):
        ui3VisibleObject(manager),
                         

        SphereSource(0),
        SphereMapper(0),
        SphereActor(0),
                         
        reader(0),
        partMapper(0),
        probeActorS(0),

        

        Position(position)
    {}

    inline bool CreateVTKObjects(void) {

            CMN_LOG_CLASS_INIT_VERBOSE << "begin creation of VTK objects." << std::endl;

            reader = vtkSTLReader::New();
            reader->SetFileName ("probeEnd.STL"); 


            partMapper = vtkPolyDataMapper::New();
            partMapper->SetInputConnection( reader->GetOutputPort() );


            probeActorS = CSOpenGLStippleActor::New();
            CMN_ASSERT(probeActorS);
            probeActorS -> SetMapper(partMapper);


            probeActorS -> GetProperty() -> EdgeVisibilityOff();
            probeActorS -> SetScale(SCALE);
            //probeActorS -> SetOrigin(0,0,0);
            //probeActorS -> SetStipplePattern(1);

//             probeActorS -> RotateX(180);





//==========================================================================================

// 
            this->SphereSource = vtkSphereSource::New();
            CMN_ASSERT(this->SphereSource);
            this->SphereSource->SetRadius(1.0);

            this->SphereMapper = vtkPolyDataMapper::New();
            CMN_ASSERT(this->SphereMapper);
            this->SphereMapper->SetInputConnection(this->SphereSource->GetOutputPort());

            this->SphereActor = vtkActor::New();
            CMN_ASSERT(this->SphereActor);
            this->SphereActor->SetMapper(this->SphereMapper);


//             this->SphereActor->GetProperty()->SetColor(1.0, 0.0, 0.0);


           // this->Assembly->AddPart(this->SphereActor);




//===========================================================================================



        CMN_LOG_CLASS_INIT_VERBOSE << "stop place" << std::endl;
        this->Assembly->AddPart(this->probeActorS);
        CMN_LOG_CLASS_INIT_VERBOSE << "added part to assembly" << std::endl;

        this->SetTransformation(this->Position);



        return true;
    }
    
    void SetColor(double r, double g, double b) {
            this->probeActorS->GetProperty()->SetColor(r, g, b);
    }

protected:
//========================================================================================
    vtkSphereSource * SphereSource;
    vtkPolyDataMapper * SphereMapper;
    vtkActor * SphereActor;
//=======================================================================================

    vtkSTLReader            *reader;
    vtkSphereSource         *sphere;
    CSOpenGLStippleActor    *probeActorS;
    vtkPolyDataMapper       *partMapper;




    vctFrm3 Position; // initial position
};


CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSProbeHead);
CMN_IMPLEMENT_SERVICES(BehaviorLUSProbeHead);

class BehaviorLUSProbeJoint: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    public:
    inline BehaviorLUSProbeJoint(ui3Manager * manager, vctFrm3 position):
    ui3VisibleObject(manager),
    jCylinder(0),
    jointMapper(0),
    joint(0),

    Position(position)
    {}

    inline ~BehaviorLUSProbeJoint()
    {
       
    }

    inline bool CreateVTKObjects(void) {

        CMN_LOG_CLASS_INIT_VERBOSE << "joint1 set up" << endl;

        jCylinder = vtkCylinderSource::New();
        jCylinder->SetHeight( 2.5 );
        jCylinder->SetRadius( 7 );
        jCylinder->SetResolution( 25 );

        jointMapper = vtkPolyDataMapper::New();
        jointMapper->SetInputConnection( jCylinder->GetOutputPort() );


        joint = CSOpenGLStippleActor::New();
        joint->SetMapper( jointMapper);
            //joint1 -> SetStipplePattern(2);
        joint->SetScale(SCALE);

        this->Assembly->AddPart(this->joint);
        this->SetTransformation(this->Position);
        return true;
    }
    
    void SetColor(double r, double g, double b) {
        this->joint->GetProperty()->SetColor(r, g, b);
    }


    protected:
        vtkCylinderSource *jCylinder;
        vtkPolyDataMapper *jointMapper;
        CSOpenGLStippleActor *joint;
    public:
        vctFrm3 Position; // initial position

};

CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSProbeJoint);
CMN_IMPLEMENT_SERVICES(BehaviorLUSProbeJoint);

class BehaviorLUSProbeShaft: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    public:
    inline BehaviorLUSProbeShaft(ui3Manager * manager, vctFrm3 position):
        ui3VisibleObject(manager),
        shaftSource(0),
        shaftMapper(0),
        shaftActor(0),

        Position(position)
        {}

        inline ~BehaviorLUSProbeShaft()
        {

        }

        inline bool CreateVTKObjects(void) {

            cout << "shaft set up" << endl;
            //set up shaft of probe
            shaftSource = vtkCylinderSource::New();
            shaftSource->SetHeight( 30 );
            shaftSource->SetRadius( 7 );
            shaftSource->SetResolution( 25 );
            shaftSource->SetCenter( 0.0, -15.0,0.0 );

            shaftMapper = vtkPolyDataMapper::New();
            shaftMapper->SetInputConnection( shaftSource->GetOutputPort() );

            //vtkActor *shaftActor = vtkActor::New();
            shaftActor = CSOpenGLStippleActor::New();
            shaftActor->SetMapper( shaftMapper );
            shaftActor->SetScale(SCALE);
            //shaftActor-> SetStipplePattern(2);


            this->Assembly->AddPart(this->shaftActor);
            this->SetTransformation(this->Position);
        return true;
        }

        void SetColor(double r, double g, double b) {
            this->shaftActor->GetProperty()->SetColor(r, g, b);
        }

    protected:
        vtkCylinderSource *shaftSource;
        vtkPolyDataMapper *shaftMapper;
        CSOpenGLStippleActor *shaftActor;
    public:
        vctFrm3 Position;  // initial position

};

CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSProbeShaft);
CMN_IMPLEMENT_SERVICES(BehaviorLUSProbeShaft);


//==================================================================================================================
class BehaviorLUSText: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    public:
    inline BehaviorLUSText(ui3Manager * manager, vctFrm3 position):
        ui3VisibleObject(manager),
        warningtextActor(0),
        warning_text(0),
        warningtextMapper(0),
       // textXform(0),

       Position(position)
        {}

        inline ~BehaviorLUSText()
        {
        }
        
        inline bool CreateVTKObjects(void) {
                      // std::cout << "adding text" << std::endl;

            warning_text = vtkVectorText::New();
            warning_text->SetText(" ");


            warningtextMapper = vtkPolyDataMapper::New();
            warningtextMapper->SetInputConnection( warning_text->GetOutputPort() );

            warningtextActor = vtkFollower::New();
            warningtextActor->SetMapper( warningtextMapper );
            warningtextActor->GetProperty()->SetColor(1, 165.0/255, 79.0/255);
            //warningtextActor-> VisibilityOff();
            warningtextActor-> SetScale(5);

            this->Assembly->AddPart(this->warningtextActor);

            this->SetTransformation(this->Position);
        }

        inline void SetText(char* txt)
        {
            this->warning_text->SetText(txt);
        }

        inline void SetColor(double r, double g, double b)
        {
            this->warningtextActor->GetProperty()->SetColor(r,g,b);
        }


    protected:
        vtkVectorText           *warning_text;
        vtkFollower             *warningtextActor;
        vtkPolyDataMapper       *warningtextMapper;
        vtkProperty             *property;
        vctFrm3 Position;  // initial position
        //vctMatrix4x4            *textXform;
};

CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSText);
CMN_IMPLEMENT_SERVICES(BehaviorLUSText);
        //======================================================================================================
class BehaviorLUSBackground: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    public:
    inline BehaviorLUSBackground(ui3Manager * manager, vctFrm3 position):
        ui3VisibleObject(manager),
        outlineSource(0),
        cubeSource(0),
        cubePlaneMapper(0),
        cubePlane(0),
        mapOutline(0),
        outline(0),
        outlineXform(0),

        Position(position)
        {}

        inline ~BehaviorLUSBackground()
        {
        }

        inline bool CreateVTKObjects(void) {

//set up the stippled plane funtions

            cubeSource = vtkCubeSource::New();
            cubeSource -> SetBounds(0,50,0,40,0,0);

            vtkPolyDataMapper *cubePlaneMapper = vtkPolyDataMapper::New();

            cubePlaneMapper->SetInputConnection(cubeSource->GetOutputPort());

            cubePlane = CSOpenGLStippleActor::New();

            cubePlane->SetMapper(cubePlaneMapper);
            cubePlane->GetProperty()->SetColor(1,1,1);
           // cubePlane -> SetStipplePattern(1);

//set up the outline funtions

            outlineSource = vtkOutlineSource::New();
            outlineSource -> SetBounds(0,70,0,60,0,0);

            vtkPolyDataMapper *mapOutline = vtkPolyDataMapper::New();

            mapOutline->SetInputConnection(outlineSource->GetOutputPort());

            outline = CSOpenGLStippleActor::New();

            outline->SetMapper(mapOutline);
            outline->GetProperty()->SetColor(1,1,1);
            //outline -> SetStipplePattern(1);

        this->Assembly->AddPart(this->cubePlane);
        this->Assembly->AddPart(this->outline);


        int p = 1;
       // this->GetBackgroundPosition(p);
        this->SetTransformation(this->Position);
        return true;
    }
    
    
    
    inline void GetBackgroundPosition(int mode)
    {
    vctFrm3 backgroundposition;
    switch(mode)
    {
        case 1:
            
            break;
        default:
            backgroundposition.Translation() = vct3(0.0,0.0,-200.0);
            this->SetTransformation(backgroundposition);
            break;
    }
                    
    
    }


    protected:
 

        vtkOutlineSource        *outlineSource;
        vtkCubeSource           *cubeSource;

        vtkPolyDataMapper       *mapOutline;
        vtkPolyDataMapper       *cubePlaneMapper;

        CSOpenGLStippleActor    *outline, *cubePlane;

        vtkMatrix4x4            *outlineXform;
        vctFrm3 Position;  // initial position

};

CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSBackground);
CMN_IMPLEMENT_SERVICES(BehaviorLUSBackground);


//============================================================================================================================================
class BehaviorLUSMarker: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    public:
    inline BehaviorLUSMarker(ui3Manager * manager, vctFrm3 position):
    ui3VisibleObject(manager),
    jCylinder(0),
    jointMapper(0),
    joint(0),

    Position(position)
    {}

    inline ~BehaviorLUSMarker()
    {
       
    }

    inline bool CreateVTKObjects(void) {

        CMN_LOG_CLASS_INIT_VERBOSE << "Marker set up" << endl;

        jCylinder = vtkCylinderSource::New();
        jCylinder->SetHeight( 6 );
        jCylinder->SetRadius( 2 );
        jCylinder->SetResolution( 25 );

        jointMapper = vtkPolyDataMapper::New();
        jointMapper->SetInputConnection( jCylinder->GetOutputPort() );


        joint = vtkActor::New();
        joint->SetMapper( jointMapper);
            //joint1 -> SetStipplePattern(2);
        joint->SetScale(SCALE);

        this->Assembly->AddPart(this->joint);
        this->SetTransformation(this->Position);
        return true;
    }
    
    void SetColor(double r, double g, double b) {
        this->joint->GetProperty()->SetColor(r, g, b);
    }


    protected:
        vtkCylinderSource *jCylinder;
        vtkPolyDataMapper *jointMapper;
        vtkActor *joint;
    public:
        vctFrm3 Position; // initial position

};

CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSMarker);
CMN_IMPLEMENT_SERVICES(BehaviorLUSMarker);


//============================================================================================================================================





BehaviorLUS::BehaviorLUS(const std::string & name, ui3Manager * manager):
    ui3BehaviorBase(std::string("BehaviorLUS::") + name, 0),
    Ticker(0),
    Following(false),
    MapEnabled(false),
    ImagePlane(0),
    VisibleList(0),
    ProbeHead(0),
    ProbeJoint1(0),
    ProbeJoint2(0),
    ProbeJoint3(0),
    ProbeShaft(0),
    Backgrounds(0),
    WarningText(0),
    MeasureText(0)
{
    // add video source interfaces
    AddStream(svlTypeImageRGB, "USVideo");

    this->VisibleList = new ui3VisibleList(manager);
    
    this->ProbeList = new ui3VisibleList(manager);
    this->ProbeListJoint1 = new ui3VisibleList(manager);
    this->ProbeListJoint2 = new ui3VisibleList(manager);
    this->ProbeListJoint3 = new ui3VisibleList(manager);
    this->ProbeListShaft = new ui3VisibleList(manager);
    this->BackgroundList = new ui3VisibleList(manager);
    this->TextList = new ui3VisibleList(manager);
    this->CursorList = new ui3VisibleList(manager);
    this->MarkerList = new ui3VisibleList(manager);
    
    this->VisibleList->Add(this->ProbeList);
    this->VisibleList->Add(this->BackgroundList);
    this->VisibleList->Add(this->TextList);
    this->VisibleList->Add(this->MarkerList);
    this->VisibleList->Add(this->CursorList);
    
    
    this->ProbeHead = new BehaviorLUSProbeHead(manager, this->Position);
    this->ProbeJoint1 = new BehaviorLUSProbeJoint(manager, this->Position);
    this->ProbeJoint2 = new BehaviorLUSProbeJoint(manager, this->Position);
    this->ProbeJoint3 = new BehaviorLUSProbeJoint(manager, this->Position);
    this->ProbeShaft = new BehaviorLUSProbeShaft(manager, this->Position);
    this->Backgrounds = new BehaviorLUSBackground(manager, this->Position);
    this->BackgroundMap = new BehaviorLUSBackground(manager, this->Position);
    this->WarningText = new BehaviorLUSText(manager, this->Position);
    this->MeasureText = new BehaviorLUSText(manager, this->Position);
    this->Cursor = new BehaviorLUSMarker(manager, this->Position);
    this->ProbeAxes = new ui3VisibleAxes(manager);
    this->AxesJoint1 = new ui3VisibleAxes(manager);
    //AxesJoint1->SetSize(15);
    this->AxesJoint2 = new ui3VisibleAxes(manager);
    //AxesJoint2->SetSize(20);
    this->AxesJoint3 = new ui3VisibleAxes(manager);
    //AxesJoint3->SetSize(25);
    this->AxesShaft = new ui3VisibleAxes(manager);
    //AxesShaft->SetSize(30);
    
    this->ProbeList->Add(this->ProbeHead);
    this->ProbeList->Add(this->ProbeAxes);
    this->ProbeListJoint1->Add(this->ProbeJoint1);
    //this->ProbeListJoint1->Add(this->AxesJoint1);
    this->ProbeListJoint2->Add(this->ProbeJoint2);
    //this->ProbeListJoint2->Add(this->AxesJoint2);
    this->ProbeListJoint3->Add(this->ProbeJoint3);
    //this->ProbeListJoint3->Add(this->AxesJoint3);
    this->ProbeListShaft->Add(this->ProbeShaft);
    //this->ProbeListShaft->Add(this->AxesShaft);
    this->BackgroundList->Add(this->Backgrounds);
    this->BackgroundList->Add(this->BackgroundMap);
    this->TextList->Add(this->WarningText);
    this->TextList->Add(this->MeasureText);
    
    this->ProbeList->Add(ProbeListJoint1);
    this->ProbeListJoint1 -> Add(ProbeListJoint2);
    this->ProbeListJoint2 ->Add(ProbeListJoint3);
    this->ProbeListJoint3 ->Add(ProbeListShaft);

    this->CursorList-> Add(Cursor);

    this->VisibleList->SetTransformation(vctFrm3::Identity());



    this->Offset.SetAll(0.0);

}


BehaviorLUS::~BehaviorLUS()
{
}

void BehaviorLUS::ConfigureMenuBar()
{
    this->MenuBar->AddClickButton("FirstButton",
                                  1,
                                  "empty.png",
                                  &BehaviorLUS::FirstButtonCallback,
                                  this);

    this->MenuBar->AddClickButton("EnableMapButton",
                                 2,
                                 "circle.png",
                                 &BehaviorLUS::EnableMapButtonCallback,
                                 this);

}

void BehaviorLUS::mtm_right_button_callback(const prmEventButton & payload)
{
    CMN_LOG_RUN_VERBOSE << "EVENT: MTM Right Button:" << payload << endl;
}

void BehaviorLUS::Startup(void)
{
    this->Slave1 = this->Manager->GetSlaveArm("Slave1");
    this->ECM1 = this->Manager->GetSlaveArm("ECM1");
    this->RMaster = this->Manager->GetMasterArm("MTMR");

    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    CMN_ASSERT(taskManager);
    mtsDevice * daVinci = taskManager->GetTask("daVinci");
    CMN_ASSERT(daVinci);
    mtsProvidedInterface * providedInterface = daVinci->GetProvidedInterface("PSM1");
    CMN_ASSERT(providedInterface);
    mtsCommandReadBase * command = providedInterface->GetCommandRead("GetPositionJoint");
    CMN_ASSERT(command);
    GetJointPositionSlave.Bind(command);
    
    mtsProvidedInterface * providedInterfaceECM = daVinci->GetProvidedInterface("ECM1");
    CMN_ASSERT(providedInterfaceECM);
    mtsCommandReadBase * commandECM = providedInterfaceECM->GetCommandRead("GetPositionJoint");
    CMN_ASSERT(commandECM);
    GetJointPositionECM.Bind(commandECM);


    RightMTMOpen = true;
    prevRightMTMOpen = RightMTMOpen;

    if (!this->Slave1) {
        CMN_LOG_CLASS_INIT_ERROR << "Startup: this behavior requires a slave arm ..." << std::endl;
    }
	
    CMN_LOG_CLASS_INIT_VERBOSE << "starting up BehaviorLUS" << std::endl;

    // Adding US image plane
    ImagePlane = new ui3ImagePlane(this->GetManager());
    CMN_ASSERT(ImagePlane);
    // Get bitmap dimensions from pipeline.
    // The pipeline has to be already initialized to get the required info.
    ImagePlane->SetBitmapSize(GetStreamWidth("USVideo"), GetStreamHeight("USVideo"));

    // Set plane size (dimensions are already in millimeters)
    ImagePlane->SetPhysicalSize(40.0*.65, 50.0*.65);

    // Change pivot position to move plane to the right location.
    // The pivot point will remain in the origin, only the plane moves.
    ImagePlane->SetPhysicalPositionRelativeToPivot(vct3(0.0, 0.0, 0.0));
    

    vctDouble3 Yaxis;
    Yaxis.Assign(0.0,1.0,0.0);
    vctAxAnRot3 imageRot(Yaxis, cmnPI_2);
    vctFrm3 planePosition;
    planePosition.Rotation() = vctMatRot3(imageRot);
    planePosition.Translation() = vct3(0.0, 0.0, 30.0); //=================================================================================================================
    ImagePlane->SetTransformation(planePosition);



    this->ImagePlane->Lock();
    this->ProbeList->Add(this->ImagePlane);
    this->ImagePlane->Unlock();
    
    MeasurePoint1.Assign(0.0,0.0,0.0);

    this->PreviousSlavePosition.Assign(this->Slave1Position.Position().Translation());
    this->CursorOffset.SetAll(0.0);
    

}


void BehaviorLUS::Cleanup(void)
{
    // menu bar will release itself upon destruction
}


bool BehaviorLUS::RunForeground()
{
    this->Ticker++;

    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->VisibleList->Show();
        this->Backgrounds->Show();

    }

    // detect transition, should that be handled as an event?
    // State is used by multiple threads ...
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->VisibleList->Show();
        this->Backgrounds->Show();
    }

    // running in foreground GUI mode
    prmPositionCartesianGet position;

    // compute offset
    this->GetPrimaryMasterPosition(position);
    if (this->Following) {
        vctDouble3 deltaCursor;
        deltaCursor.DifferenceOf(position.Position().Translation(),
                                 this->PreviousCursorPosition);
        this->Offset.Add(deltaCursor);
    }
    this->PreviousCursorPosition.Assign(position.Position().Translation());

    // apply to object
    this->Slave1->GetCartesianPosition(this->Slave1Position);
    //this->Slave1Position.Position().Translation().Add(this->Offset);
    this->ProbeList->SetTransformation(this->Slave1Position.Position());
    this->CursorList->SetTransformation(this->Slave1Position.Position());
    this->SetJoints(0.0,0.0,0.0,0.0);


/*
    this->ProbeHead->SetTransformation(this->Slave1Position.Position());
    this->ImagePlane->SetTransformation(this->Slave1Position.Position());*/


    return true;
}

bool BehaviorLUS::RunBackground()
{
    this->Ticker++;

    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->VisibleList->Show();
        this->Backgrounds ->Show();
    }

    this->Slave1->GetCartesianPosition(this->Slave1Position);
    this->Slave1Position.Position().Translation().Add(this->Offset);
    this->ProbeList->SetTransformation(this->Slave1Position.Position());
    this->CursorList->SetTransformation(this->Slave1Position.Position());
 //   this->SetJoints(0.0,0.0,0.0,0.0);

//     this->ProbeHead->SetTransformation(this->Slave1Position.Position());
//     this->ImagePlane->SetTransformation(this->Slave1Position.Position());

    return true;
}

bool BehaviorLUS::RunNoInput()
{
    this->Ticker++;
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->VisibleList->Show();
        this->BackgroundList->Show();
    }

    vctDynamicVector<double> vec = RMaster -> GetMasterJointPosition();

    //std::cout << "mast joint 7 " << vec <<std::endl;

    RightMTMOpen = isRightMTMOpen(vec[7]);

    this->GetJointPositionSlave(this->JointsSlave);
    this->GetJointPositionECM(this->JointsECM);

    // .Positions() returns oject of type vctDynamicVector of doubles
    // for translations you might have a meter to mm conversion to do


    this->Slave1->GetCartesianPosition(this->Slave1Position);
    this->ECM1->GetCartesianPosition(this->ECM1Position);

    std::cout << "emc position: " << ECM1Position.Position()<< std::endl;
    std::cout << "ecm joints: " << JointsECM.Position() << std::endl;
//    this->Slave1Position.Position().Translation().Add(this->Offset);
    vctFrm3 tmp;
    tmp.Rotation() =vctMatRot3(this->Slave1Position.Position().Rotation()) * vctMatRot3(vctAxAnRot3(vctDouble3(0.0,0.0,1.0), cmnPI_4 ));
    tmp.Translation() = vctDouble3(25.0,-50.0,-300.0);
    this->ProbeList ->SetTransformation(tmp);
 //   this->ProbeList ->SetPosition(vctDouble3(30.0, -40.0, -300.0));
    this->Backgrounds -> SetPosition(vctDouble3(-10.0,-80.0,-300.0));
    this->BackgroundMap->SetPosition(vctDouble3(-10.0,-120.0,-300.0));
    this->TextList -> SetPosition(vctDouble3(-10.0,-90.0,-300.0));
    
    this-> MeasureText -> SetColor(1,1,1);
    this-> MeasureText -> SetPosition(vctDouble3(0.0, 10, 0.0));


//    this->ImagePlane->SetTransformation(this->Slave1Position.Position());
    //void BehaviorLUS::SetJoints(double pitch, double yaw, double insertion, double roll)
    this->SetJoints(JointsSlave.Position()[4],JointsSlave.Position()[5],JointsSlave.Position()[2],JointsSlave.Position()[3]);
    

    if (!RightMTMOpen)
    {
        cout<< "getMeasurement()" << endl;
        this-> GetMeasurement(this->Slave1Position.Position().Translation());
    }
    else {
        MeasurementActive = false;
        this->SetText(MeasureText, " ");
    }
    
    if(MapEnabled)
    {
        this->CursorList->Show();
        this->UpdateMap(ECM1Position, vec[2]);
    }
    else {this->CursorList->Hide();}

}

void BehaviorLUS::Configure(const std::string & CMN_UNUSED(configFile))
{
    // load settings
}

bool BehaviorLUS::SaveConfiguration(const std::string & CMN_UNUSED(configFile))
{
    // save settings
    return true;
}

void BehaviorLUS::FirstButtonCallback()
{
    CMN_LOG_CLASS_RUN_DEBUG << "Behavior \"" << this->GetName() << "\" Button 1 pressed" << std::endl;
//     this->SetProbeColor(1.0,0.0,0.0);

}

void BehaviorLUS::EnableMapButtonCallback()
{
    if (this -> MapEnabled == true)
    {
        this->MapEnabled = false;
    }
    else this->MapEnabled = true;
    
    CMN_LOG_CLASS_RUN_DEBUG << "Behavior \"" << this->GetName() << "\" Enable map Button pressed" << std::endl;
    CMN_LOG_CLASS_RUN_DEBUG << "Behavior \"" << this->GetName() << "\" Enable map " << this->MapEnabled << std::endl;
}

void BehaviorLUS::PrimaryMasterButtonCallback(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::PRESSED) {
        this->Following = true;
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->Following = false;
    }
}


void BehaviorLUS::OnStreamSample(svlSample* sample, int streamindex)
{
    if (State == Foreground) {
        ImagePlane->SetImage(dynamic_cast<svlSampleImageBase*>(sample), streamindex);
    }
}



/*--------------------------------------------------------------------------------------*/


/*!
    setting the user matrix for the probeActor and then orienting the others accordingly
    the joint angles 
@param tform the transformation of the probe actor from MTM 
@param A1 picth angle for joint 1 and 4
@param A2 yaw angle for joints 2 and 3
@param insertion the insertion 'joint'
@param roll the roll joint value
 */

void BehaviorLUS::SetJoints(double A1, double A2, double insertion, double roll)
{
    vctDouble3 Xaxis;
    Xaxis.Assign(1.0,0.0,0.0);
    vctDouble3 Yaxis;
    Yaxis.Assign(0.0,1.0,0.0);
    vctDouble3 Zaxis;
    Zaxis.Assign(0.0,0.0,1.0);


    vctFrm3 probePosition;
    vctAxAnRot3 probeRot(Xaxis, cmnPI);
    probePosition.Rotation() = vctMatRot3(probeRot);
    probePosition.Translation() = vct3(-8.0*SCALE,12.0*SCALE, 70.0*SCALE);  //-8,12, 70
    this->ProbeHead->SetTransformation(probePosition);
    


    
        //convert the pitch and yaw from radians into degrees 
    double pitch = A1; //(A1*180/_PI);
    double yaw = -A2; //(A2*180/_PI);

    double total = fabs(pitch) + fabs(yaw);
//    cout << "Probe wrist pitch: " << pitch << endl;
//    cout << "Probe wrist yaw:   " << yaw << endl;
//    cout << "total angle:       " << total << endl;
//    cout << "insertion:         " << insertion << endl;
//    cout << "roll:              " << roll << endl;



    ProbeHead -> Hide();

    //set up first joint position
    vctFrm3 j1pos;
    j1pos.Rotation() = vctMatRot3(vctAxAnRot3(Xaxis, pitch)) * vctMatRot3(vctAxAnRot3 (Xaxis, cmnPI_2)) * vctMatRot3(vctAxAnRot3(Yaxis, cmnPI_2));
    j1pos.Translation() = vctDouble3(0.0, 3.0*SCALE, -12.0*SCALE);

//    this->ProbeJoint1->SetColor(1.0, 0.0, 0.0);
    ProbeListJoint1->SetTransformation(j1pos);

    //set up second joint
    vctFrm3 j2pos;
    j2pos.Rotation() = vctMatRot3(vctAxAnRot3(Xaxis, yaw));
    j2pos.Translation() = vctDouble3(0.0,-7.0*SCALE,0.0);
    ProbeListJoint2 -> SetTransformation(j2pos);
    
        //set up second joint
    vctFrm3 j3pos;
    j3pos.Rotation() = vctMatRot3(vctAxAnRot3(Xaxis, yaw));
    j3pos.Translation() = vctDouble3(0.0,-7.0*SCALE,0.0);
    ProbeListJoint3 -> SetTransformation(j3pos);
    
    vctFrm3 shaftpos;
    shaftpos.Rotation() = vctMatRot3(vctAxAnRot3(Zaxis, pitch));
    shaftpos.Translation() = vctDouble3(0.0, -7.0*SCALE, 0.0);
    ProbeListShaft -> SetTransformation(shaftpos);
    
    CheckLimits(pitch, yaw, insertion, roll);
}


void BehaviorLUS::SetProbeColor(double r, double g, double b)
{
    this->ProbeHead ->SetColor(r,g,b);
    this->ProbeJoint1 ->SetColor(r,g,b);
    this->ProbeJoint2 ->SetColor(r,g,b);
    this->ProbeJoint3 ->SetColor(r,g,b);
    this->ProbeShaft ->SetColor(r,g,b);

}

void BehaviorLUS::SetText(BehaviorLUSText *obj, char* txt)
{
    obj-> SetText(txt);
}

void BehaviorLUS::CheckLimits(double p, double y, double i, double r)
{
        //convert the pitch and yaw from radians into degrees 
    double pitch = (p*180/cmnPI);
    double yaw = (y*180/cmnPI);
    double insertion = i;
    double roll = r;
    
    double total = fabs(pitch) + fabs(yaw);
    
   // std::cout << "total: " << total << std::endl;
    if (insertion < 0.165)
    {
        SetProbeColor(1, 0.0/255, 0.0/255 );
        SetText(WarningText, "STOP: Wrist in cannula");
    }
    else if(total >= 50 || (fabs(pitch) > 40 && fabs(yaw) < 9) || (fabs(yaw) > 40 && fabs(pitch) < 9) ) //turn red 
    {
        SetProbeColor( 1, 165.0/255, 79.0/255 );   //1, 120.0/255, 65.0/255 );
        SetText(WarningText, "Joint Limit Reached");
    }
    else if(insertion > 0.28)
    {
        SetProbeColor(1, 165.0/255, 79.0/255 );
//         textAct -> VisibilityOn();
//         textAct -> SetInput("Insertion Limit Reached");
        SetText(WarningText, "Insertion Limit Reached");
    }
    else if (roll > 4.5 || roll < -4.5)
    {
        SetProbeColor( 1, 165.0/255, 79.0/255 );
        SetText(WarningText, "Roll limit Reached");
    }
    else
    {
        SetProbeColor( 1.0,1.0,1.0);//127./255, 255./255, 212./255 );
        SetText(WarningText, " ");
    }

    if( total < 6 ) //turn blue 
    {
        SetProbeColor(159.0/255, 182.0/255, 205.0/255) ;
    }

}

bool BehaviorLUS::isRightMTMOpen(double grip)
{
    if(grip > .1)
        return true;
    if(grip < .1)
        return false;
}

void BehaviorLUS::GetMeasurement(vctDouble3 pos)
{
    char    measure_string[100];
    
    if(!MeasurementActive)
    {
        MeasurementActive = true;
        //            memcpy(measure_point1, psm_pos, sizeof(float)*3);
        MeasurePoint1.FastCopyOf(pos);
        std::cout<< "MeasurePoint1 from if statement: " << MeasurePoint1<< std::endl;
    }
    else{
 //       std::cout<< "start calcs" << std::endl;
        vctDouble3 diff;
        diff.DifferenceOf(MeasurePoint1,pos);
        
        double AbsVal;
        AbsVal=diff.Norm();
//         std::cout<< "cout line" << std::endl;
//         std::cout<< "MeasurePoint1: " << MeasurePoint1<< std::endl;
//         std::cout<< "pos: " << pos<< std::endl;
//     std::cout<< "Absval: " << AbsVal<< std::endl;
    
 //   sprintf(measure_string,"%4.1fmm",measure_dist);
    sprintf(measure_string,"%4.1fmm", AbsVal);
    this->SetText(MeasureText, measure_string);
    }

}

void BehaviorLUS::UpdateMap(prmPositionCartesianGet ecmPosition, double insertion )
{
    double scale = .2;

    vctFrm3 cursorPos;
    cursorPos.Rotation() = ecmPosition.Position().Rotation();
    cursorPos.Translation() = ecmPosition.Position().Translation();

    prmPositionCartesianGet position;
    //translate into ECM frame


        //take the difference between the cursor position and the ecm tip position
        //move the cursor to the ecm tip

 //   CursorList->SetTransformation(cursorPos);
    
    //move the cursor based on the insertion depth to the ecm rcm
    vctFrm3 toECM_RCM;
    toECM_RCM.Translation() = vctDouble3(0.0,0.0,insertion*1000);
    toECM_RCM.Rotation().SetAll(0.0);
//    CursorList -> SetTransformation(toECM_RCM);

    //translate the cursor back to a normal depth
    vctFrm3 toScreen;
    toScreen.Translation() = vctDouble3(0.0,0.0,-100.0);
    toScreen.Rotation().SetAll(0.0);
//    CursorList->SetTransformation(toScreen);


    //apply offset due to slave movement
    // compute offset
    
    vctDouble3 offset;
    vctDouble3 deltaCursor, deltaSlave;
    vctFrm3 finalFrm;
    this->Slave1->GetCartesianPosition(position);
    //translate slave position to patient coordinates
    deltaSlave.DifferenceOf(ecmPosition.Position().Translation(), position.Position().Translation());
    
    position.Position().Rotation() = ecmPosition.Position().Rotation();
    position.Position().Translation() = -deltaSlave + toScreen.Translation() + toECM_RCM.Translation();
    
    
    deltaCursor.DifferenceOf(position.Position().Translation(), this->PreviousSlavePosition);
//    CursorOffset.Add(deltaCursor);
    finalFrm.Rotation().SetAll(0.0);// = position.Position().Rotation();
    finalFrm.Translation() = deltaCursor;//.Multiply(scale);

//    this->CursorList->SetTransformation(finalFrm*toScreen*toECM_RCM*cursorPos);
    //this->CursorList->SetTransformation(cursorPos*toScreen);//*toECM_RCM*toScreen*finalFrm);
    //this->CursorList->SetTransformation(cursorPos);
    this->CursorOffset += deltaCursor;
    this->CursorList->SetOrientation(ecmPosition.Position().Rotation());
    this->CursorList->SetPosition(ecmPosition.Position().Translation() + toScreen.Translation() + toECM_RCM.Translation() + CursorOffset);

    this->PreviousSlavePosition.Assign(position.Position().Translation());

    // apply to object
  //  cursorPos.Translation().Add(offset);
  //  this->CursorList->SetTransformation(cursorPos);

    //this->CursorList->SetOrientation(this->Slave1Position.Position().Rotation());
    //this->CursorList->SetPosition(vctDouble3(0.0, 0.0, -300.0));
    
}
//     this->Slave1->GetCartesianPosition(this->Slave1Position);
//     this->Slave1Position.Position().Translation().Add(this->Offset);
//     this->VisibleObject->SetTransformation(this->Slave1Position.Position());

