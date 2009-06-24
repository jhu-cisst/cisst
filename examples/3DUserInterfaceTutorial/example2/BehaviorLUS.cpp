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
        warningtextActor(0),
        warning_text(0),
        warningtextMapper(0),
        textXform(0),

        Position(position)
        {}

        inline ~BehaviorLUSBackground()
        {
        }

        inline bool CreateVTKObjects(void) {

            //set up the stippled plane funtions

            cubeSource = vtkCubeSource::New();
            cubeSource -> SetBounds(0,160,0,130,0,0);

            mapOutline = vtkPolyDataMapper::New();

            mapOutline->SetInputConnection(cubeSource->GetOutputPort());

            cubePlane = CSOpenGLStippleActor::New();

            cubePlane->SetMapper(mapOutline);
            cubePlane->GetProperty()->SetColor(1,1,1);
           // cubePlane -> SetStipplePattern(1);

//set up the outline funtions

            outlineSource = vtkOutlineSource::New();
            outlineSource -> SetBounds(0,70,0,70,0,0);

//            vtkPolyDataMapper *mapOutline = vtkPolyDataMapper::New();

            mapOutline->SetInputConnection(outlineSource->GetOutputPort());

            outline = CSOpenGLStippleActor::New();

            outline->SetMapper(mapOutline);
            outline->GetProperty()->SetColor(1,1,1);
            //outline -> SetStipplePattern(1);

            //setting the position of the outline 

            //outline -> SetScale(SCALE);


           // std::cout << "adding text" << std::endl;

            warning_text = vtkVectorText::New();
            warning_text->SetText("Warning Text");


            warningtextMapper = vtkPolyDataMapper::New();
            warningtextMapper->SetInputConnection( warning_text->GetOutputPort() );

            warningtextActor = vtkFollower::New();
            warningtextActor->SetMapper( warningtextMapper );
            warningtextActor->GetProperty()->SetColor(1, 165.0/255, 79.0/255);
            //warningtextActor-> VisibilityOff();
            warningtextActor-> SetScale(3);


//setting the positon of the insertion limit warning 
//should be along the top of the outline, above the probe 

//             CreateVTKObject(textXform, vtkMatrix4x4);
//             textXform->SetElement(0, 3, 0); //need to figure out where would be best 
//             textXform->SetElement(1, 3, -45/(warningtextActor-> GetScale())[0]);
//             textXform->SetElement(2, 3, DEPTH/(warningtextActor-> GetScale())[0]); //need to make this the same depth as the outline 
// 
//             warningtextActor -> SetUserMatrix(textXform);


        this->Assembly->AddPart(this->outline);
        this->Assembly->AddPart(this->warningtextActor);
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
            backgroundposition.Translation() = vct3(0.0,0.0,-200.0);
            this->SetTransformation(backgroundposition);
            break;
        default:
            backgroundposition.Translation() = vct3(0.0,0.0,-200.0);
            this->SetTransformation(backgroundposition);
            break;
    }
                    
    
    }


    protected:
       vtkVectorText           *warning_text;

        vtkOutlineSource        *outlineSource;
        vtkCubeSource           *cubeSource;

        vtkPolyDataMapper       *mapOutline;
        vtkPolyDataMapper       *cubePlaneMapper, *warningtextMapper;

        CSOpenGLStippleActor    *outline, *cubePlane;

        vtkFollower             *warningtextActor;
        vtkMatrix4x4            *outlineXform, *textXform;


        vtkProperty             *property;
        vctFrm3 Position;  // initial position

};

CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSBackground);
CMN_IMPLEMENT_SERVICES(BehaviorLUSBackground);





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
    Backgrounds(0)
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
    this->VisibleList->Add(this->ProbeList);
    this->VisibleList->Add(this->BackgroundList);
    
    this->ProbeHead = new BehaviorLUSProbeHead(manager, this->Position);
    this->ProbeJoint1 = new BehaviorLUSProbeJoint(manager, this->Position);
    this->ProbeJoint2 = new BehaviorLUSProbeJoint(manager, this->Position);
    this->ProbeJoint3 = new BehaviorLUSProbeJoint(manager, this->Position);
    this->ProbeShaft = new BehaviorLUSProbeShaft(manager, this->Position);
    this->Backgrounds = new BehaviorLUSBackground(manager, this->Position);
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
    this->ProbeListJoint1->Add(this->AxesJoint1);
    this->ProbeListJoint2->Add(this->ProbeJoint2);
    this->ProbeListJoint2->Add(this->AxesJoint2);
    this->ProbeListJoint3->Add(this->ProbeJoint3);
    this->ProbeListJoint3->Add(this->AxesJoint3);
    this->ProbeListShaft->Add(this->ProbeShaft);
    this->ProbeListShaft->Add(this->AxesShaft);
    this->BackgroundList->Add(this->Backgrounds);
    
    this->ProbeList->Add(ProbeListJoint1);
    this->ProbeListJoint1 -> Add(ProbeListJoint2);
    this->ProbeListJoint2 ->Add(ProbeListJoint3);
    this->ProbeListJoint3 ->Add(ProbeListShaft);
    
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


void BehaviorLUS::Startup(void)
{
    this->Slave1 = this->Manager->GetSlaveArm("Slave1");

    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    CMN_ASSERT(taskManager);
    mtsDevice * daVinci = taskManager->GetTask("daVinci");
    CMN_ASSERT(daVinci);
    mtsProvidedInterface * providedInterface = daVinci->GetProvidedInterface("PSM1");
    CMN_ASSERT(providedInterface);
    mtsCommandReadBase * command = providedInterface->GetCommandRead("GetPositionJoint");
    CMN_ASSERT(command);
    GetJointPositionSlave.Bind(command);


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
    //this->Slave1Position.Position().Translation().Add(this->Offset);
    this->ProbeList->SetTransformation(this->Slave1Position.Position());
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
    
    this->GetJointPositionSlave(this->JointsSlave);
    std::cout << JointsSlave.Position() << std::endl;

    // .Positions() returns oject of type vctDynamicVector of doubles
    // for translations you might have a meter to mm conversion to do


    this->Slave1->GetCartesianPosition(this->Slave1Position);
//    this->Slave1Position.Position().Translation().Add(this->Offset);
    vctFrm3 tmp;
    tmp.Rotation() =vctMatRot3(this->Slave1Position.Position().Rotation()) * vctMatRot3(vctAxAnRot3(vctDouble3(0.0,0.0,1.0), cmnPI_4 ));
    tmp.Translation() = vctDouble3(-10.0,-80.0,-300.0);
    this->ProbeList ->SetTransformation(tmp);
 //   this->ProbeList ->SetPosition(vctDouble3(30.0, -40.0, -300.0));
    this->BackgroundList -> SetPosition(vctDouble3(-10.0,-80.0,-300.0));
//    this->ImagePlane->SetTransformation(this->Slave1Position.Position());
    this->SetJoints(JointsSlave.Position()[5],JointsSlave.Position()[4],JointsSlave.Position()[2],JointsSlave.Position()[3]);
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
    this->SetProbeColor(1.0,0.0,0.0);
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
    double yaw = A2; //(A2*180/_PI);

    double total = fabs(pitch) + fabs(yaw);
//    cout << "Probe wrist pitch: " << pitch << endl;
//    cout << "Probe wrist yaw:   " << yaw << endl;
//    cout << "total angle:       " << total << endl;
//    cout << "insertion:         " << insertion << endl;
//    cout << "roll:              " << roll << endl;



    ProbeHead -> Hide();

    //set up first joint position
    vctFrm3 j1pos;
    j1pos.Rotation() = vctMatRot3(vctAxAnRot3(Xaxis, -pitch)) * vctMatRot3(vctAxAnRot3 (Xaxis, cmnPI_2)) * vctMatRot3(vctAxAnRot3(Yaxis, cmnPI_2));
    j1pos.Translation() = vctDouble3(0.0, 3.0*SCALE, -12.0*SCALE);

//    this->ProbeJoint1->SetColor(1.0, 0.0, 0.0);
    ProbeListJoint1->SetTransformation(j1pos);

    //set up second joint
    vctFrm3 j2pos;
    j2pos.Rotation() = vctMatRot3(vctAxAnRot3(Xaxis, -yaw));
    j2pos.Translation() = vctDouble3(0.0,-7.0*SCALE,0.0);
    ProbeListJoint2 -> SetTransformation(j2pos);
    
        //set up second joint
    vctFrm3 j3pos;
    j3pos.Rotation() = vctMatRot3(vctAxAnRot3(Xaxis, -yaw));
    j3pos.Translation() = vctDouble3(0.0,-7.0*SCALE,0.0);
    ProbeListJoint3 -> SetTransformation(j3pos);
    
    vctFrm3 shaftpos;
    shaftpos.Rotation() = vctMatRot3(vctAxAnRot3(Zaxis, -pitch));
    shaftpos.Translation() = vctDouble3(0.0, -7.0*SCALE, 0.0);
    ProbeListShaft -> SetTransformation(shaftpos);
#if 0
    
    //set up second joint position
    ProbeJoint2 -> SetTransformation(this -> ProbeJoint1 ->Position);
    vctFrm3 joint2Position;
    vctFrm3 joint2angle;
    vctAxAnRot3 joint2Rot(Yaxis, yaw);
    joint2Position.Translation() = vct3(0.0,0.0, - 7.0);
    ProbeJoint2 ->SetTransformation(joint2Position);
    
#endif
}


void BehaviorLUS::SetProbeColor(double r, double g, double b)
{
    this->ProbeHead ->SetColor(r,g,b);
    this->ProbeJoint1 ->SetColor(r,g,b);
    this->ProbeJoint2 ->SetColor(r,g,b);
    this->ProbeJoint3 ->SetColor(r,g,b);
    this->ProbeShaft ->SetColor(r,g,b);

}

/*
 void  LUSWidget::SetUserMatrix(vtkMatrix4x4 *tform, float A1, float A2, float insertion, float roll)
{
    //convert the pitch and yaw from radians into degrees 
    double pitch = (A1*180/_PI);
//            pitch = pitch - 3;
    double yaw = (A2*180/_PI);
//            yaw = yaw - 3;


    double total = fabs(pitch) + fabs(yaw);
//    cout << "Probe wrist pitch: " << pitch << endl;
//    cout << "Probe wrist yaw:   " << yaw << endl;
//    cout << "total angle:       " << total << endl;
//    cout << "insertion:         " << insertion << endl;
//    cout << "roll:              " << roll << endl;

    double s[3];
    LUSActors[0] -> GetScale(s);

// cout << "scale of partActor:    " << s[0] << "  "<< s[1] <<"  " << s[2] <<"  "<< endl; 


    t1 -> Identity();
    t2 -> Identity();
    t3 -> Identity();
    t4 -> Identity();
//     probeXform -> Identity();
    // 
//     probeXform -> RotateX(180);
//     probeXform -> RotateY(90);
//     probeXform -> Translate(-35*.4, 12*.4, -8*.4);
    // 
//     LUSActors[0] -> SetUserTransform(probeXform);

    //incoming matrix is the transform of the probe and thus the probe Actor pitch and yaw are the joint angles
    //setting the transform for the probe actor 
    LUSActors[0] -> SetUserMatrix(tform);

    //set up first joint
    t1 -> SetMatrix(LUSActors[0]->GetUserMatrix());
    t1 -> Translate(50*s[0],3*s[0] , 0);
    t1 -> RotateZ(90);
    t1 -> RotateZ(-pitch);
    LUSActors[1]-> SetUserTransform(t1);

    //set up second joint
    t2 -> Identity();
    t2 -> SetMatrix(LUSActors[1]->GetUserMatrix());
    t2 -> RotateX(yaw);
    t2 -> Translate(0, -7*s[0], 0);
    LUSActors[2] -> SetUserTransform(t2);

    //set up third joint
    t3 -> Identity();
    t3 -> SetMatrix(LUSActors[2]->GetUserMatrix());
    t3 -> RotateX(yaw);
    t3 -> Translate(0, -7*s[0], 0);
    LUSActors[3] -> SetUserTransform(t3);

    //set up shaft joint
    t4 -> Identity();
    t4 -> SetMatrix(LUSActors[3]->GetUserMatrix());
    t4 -> RotateZ(-pitch);
    t4 -> Translate(0, -20*s[0], 0);
    LUSActors[4] -> SetUserTransform(t4);

    //LUSActors[5] -> SetUserMatrix(tform);
   // textAct -> SetUserMatrix (tform);
    //textAct -> VisibilityOn();

   // measuretextActor -> VisibilityOn();
   // measuretextActor -> SetUserMatrix (tform);

    if (insertion < 0.165)
    {
        //LUSActors[numActs -1 ] -> GetProperty() ->  SetColor(1, 0, 0);
        LUSActors[numActs -1 ] -> GetProperty() ->  SetColor(163./255, 214./255, 1);
        warningtextActor -> VisibilityOn();
        warning_text->SetText("STOP Wrist in Cannula");
    }
    else if(total >= 50 || (fabs(pitch) > 40 && fabs(yaw) < 9) || (fabs(yaw) > 40 && fabs(pitch) < 9) ) //turn red 
    {
        ChangeColor( 1, 165.0/255, 79.0/255 );   //1, 120.0/255, 65.0/255 );
//         textAct -> VisibilityOn();
//         textAct -> SetInput("Joint Limit Reached");
        warningtextActor -> VisibilityOn();
        warning_text->SetText("Joint Limit Reached");
    }
    else if(insertion > 0.29)
    {
        LUSActors[numActs -1 ] -> GetProperty() ->  SetColor(1, 0, 0);
//         textAct -> VisibilityOn();
//         textAct -> SetInput("Insertion Limit Reached");
        warningtextActor -> VisibilityOn();
        warning_text->SetText("Insertion Limit Reached");
    }
    else if (roll > 4.5 || roll < -4.5)
    {
        ChangeColor( 1, 165.0/255, 79.0/255 );
        warningtextActor -> VisibilityOn();
        warning_text->SetText("Roll limit Reached");
    }
    else
    {
        ChangeColor( 127./255, 255./255, 212./255 );
     //   textAct -> VisibilityOff();
        LUSActors[numActs -1 ] -> GetProperty() -> SetColor( 192.0/255 , 192.0/255 , 192.0/255);
        warningtextActor -> VisibilityOff();
    }


    if( total < 3 ) //turn blue 
    {
        ChangeColor(159.0/255, 182.0/255, 205.0/255) ;
    }

// delete pitch;
// delete yaw;
// delete total;
// delete [] s;

}
*/


