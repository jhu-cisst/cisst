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

#define SCALE .25



/*!

This class creates the VTK object from a STL file of the ultrasound probe head
@param manager The ui3Manager responsible for this class
@param position The starting position of the object

*/
class BehaviorLUSProbeHead: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
public:
      inline BehaviorLUSProbeHead(vctFrm3 position):
        ui3VisibleObject(),
        reader(0),
        partMapper(0),
        probeActorS(0),
        Position(position)
    {}

    inline bool CreateVTKObjects(void) {

            CMN_LOG_CLASS_INIT_VERBOSE << "begin creation of VTK objects." << std::endl;

            reader = vtkSTLReader::New();
            CMN_ASSERT(reader);
            reader->SetFileName ("probeEnd.STL"); 


            partMapper = vtkPolyDataMapper::New();
            CMN_ASSERT(partMapper);
            partMapper->SetInputConnection( reader->GetOutputPort() );


            probeActorS = CSOpenGLStippleActor::New();
            CMN_ASSERT(probeActorS);
            probeActorS -> SetMapper(partMapper);


            probeActorS -> GetProperty() -> EdgeVisibilityOff();
            probeActorS -> SetScale(SCALE);

        this->AddPart(this->probeActorS);
        this->SetTransformation(this->Position);

        return true;
    }
    
    void SetColor(double r, double g, double b) {
        if (this->probeActorS && (r+g+b)<= 3) {
            this->probeActorS->GetProperty()->SetColor(r, g, b);
        }
    }

protected:

    vtkSTLReader            *reader;
    vtkSphereSource         *sphere;
    vtkPolyDataMapper       *partMapper;
    CSOpenGLStippleActor    *probeActorS;

    vctFrm3 Position; // initial position
};


CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSProbeHead);
CMN_IMPLEMENT_SERVICES(BehaviorLUSProbeHead);

/*!

This class creates the VTK object that will become the joints of the probe
@param manager The ui3Manager responsible for this class
@param position The starting position of the object

 */

class BehaviorLUSProbeJoint: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    public:
    inline BehaviorLUSProbeJoint(vctFrm3 position):
    ui3VisibleObject(),
    jCylinder(0),
    jointMapper(0),
    joint(0),

    Position(position)
    {}

    inline ~BehaviorLUSProbeJoint()
    {
       
    }

    inline bool CreateVTKObjects(void) {

        jCylinder = vtkCylinderSource::New();
        CMN_ASSERT(jCylinder);
        jCylinder->SetHeight( 2.5 );
        jCylinder->SetRadius( 7 );
        jCylinder->SetResolution( 25 );

        jointMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(jointMapper);
        jointMapper->SetInputConnection( jCylinder->GetOutputPort() );


        joint = CSOpenGLStippleActor::New();
        CMN_ASSERT(joint);
        joint->SetMapper(jointMapper);
            //joint1 -> SetStipplePattern(2);
        joint->SetScale(SCALE);

        this->AddPart(this->joint);
        this->SetTransformation(this->Position);
        return true;
    }
    
    void SetColor(double r, double g, double b) {
        if (this->joint && (r+g+b)<= 3) {
            this->joint->GetProperty()->SetColor(r, g, b);
        }
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

/*!

This class creates the VTK object that will become the shaft of the probe
@param manager The ui3Manager responsible for this class
@param position The starting position of the object

 */

class BehaviorLUSProbeShaft: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    public:
    inline BehaviorLUSProbeShaft(vctFrm3 position):
        ui3VisibleObject(),
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
            CMN_ASSERT(shaftSource);
            shaftSource->SetHeight( 30 );
            shaftSource->SetRadius( 7 );
            shaftSource->SetResolution( 25 );
            shaftSource->SetCenter( 0.0, -15.0,0.0 );

            shaftMapper = vtkPolyDataMapper::New();
            CMN_ASSERT(shaftMapper);
            shaftMapper->SetInputConnection( shaftSource->GetOutputPort() );

            //vtkActor *shaftActor = vtkActor::New();
            shaftActor = CSOpenGLStippleActor::New();
            CMN_ASSERT(shaftActor);
            shaftActor->SetMapper( shaftMapper );
            shaftActor->SetScale(SCALE);
            //shaftActor-> SetStipplePattern(2);


            this->AddPart(this->shaftActor);
            this->SetTransformation(this->Position);
        return true;
        }

        void SetColor(double r, double g, double b) {
            if (this->shaftActor && (r+g+b)<= 3) {
                this->shaftActor->GetProperty()->SetColor(r, g, b);
            }
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
/*!

This class creates the VTK text object that will create text in the scene
@param manager The ui3Manager responsible for this class
@param position The starting position of the object

 */
class BehaviorLUSText: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
public:
    inline BehaviorLUSText(vctFrm3 position):
        ui3VisibleObject(),
        warningtextActor(0),
        warning_text(0),
        warningtextMapper(0),
        Position(position)
    {}
    
    inline ~BehaviorLUSText()
    {}
        
    inline bool CreateVTKObjects(void) {

        warning_text = vtkVectorText::New();
        CMN_ASSERT(warning_text);
        warning_text->SetText(" ");

        warningtextMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(warningtextMapper);
        warningtextMapper->SetInputConnection( warning_text->GetOutputPort() );

        warningtextActor = vtkFollower::New();
        CMN_ASSERT(warningtextActor);
        warningtextActor->SetMapper( warningtextMapper );
        warningtextActor->GetProperty()->SetColor(1, 165.0/255, 79.0/255 );
        //warningtextActor-> VisibilityOff();
        warningtextActor-> SetScale(2.5);

        this->AddPart(this->warningtextActor);

        this->SetTransformation(this->Position);
        return true;
    }

    inline void SetText(const std::string & text)
    {
        if (this->warning_text) {
            this->warning_text->SetText(text.c_str());
        }
    }
    

    inline void SetColor(double r, double g, double b)
    {
        if (this->warningtextActor && (r+g+b)<= 3) {
            this->warningtextActor->GetProperty()->SetColor(r,g,b);
        }
    }
    
    
protected:
    vtkFollower             *warningtextActor;
    vtkVectorText           *warning_text;
    vtkPolyDataMapper       *warningtextMapper;
    vtkProperty             *property;
    vctFrm3 Position;  // initial position
    //vctMatrix4x4            *textXform;
};

CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSText);
CMN_IMPLEMENT_SERVICES(BehaviorLUSText);


//======================================================================================================
/*!

This class creates the VTK objects that will become the background for the map
@param manager The ui3Manager responsible for this class
@param position The starting position of the object

 */
class BehaviorLUSBackground: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    public:
    inline BehaviorLUSBackground(vctFrm3 position):
        ui3VisibleObject(),
        cubeSource(0),
        cubePlaneMapper(0),
        cubePlane(0),

        Position(position)
        {}

        inline ~BehaviorLUSBackground()
        {
        }

        inline bool CreateVTKObjects(void) {

//set up the stippled plane funtions

            cubeSource = vtkCubeSource::New();
            CMN_ASSERT(cubeSource);
            cubeSource->SetXLength(30);
            cubeSource->SetYLength(25);
            cubeSource->SetZLength(1);
            
//            cubeSource -> SetBounds(0,30,0,25,0,0);

            vtkPolyDataMapper *cubePlaneMapper = vtkPolyDataMapper::New();
            CMN_ASSERT(cubePlaneMapper);
            cubePlaneMapper->SetInputConnection(cubeSource->GetOutputPort());

            cubePlane = CSOpenGLStippleActor::New();
            CMN_ASSERT(cubePlane);
            cubePlane->SetMapper(cubePlaneMapper);
            cubePlane->GetProperty()->SetColor(1,1,1);
           // cubePlane -> SetStipplePattern(1);


        this->cubePlane->VisibilityOff();
        this->AddPart(this->cubePlane);

        this->SetTransformation(this->Position);
        return true;
    }


    protected:
 
        vtkCubeSource           *cubeSource;
        vtkPolyDataMapper       *cubePlaneMapper;
        CSOpenGLStippleActor    *cubePlane;
        vctFrm3 Position;  // initial position
};

CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSBackground);
CMN_IMPLEMENT_SERVICES(BehaviorLUSBackground);
//===========================================================================================================================================

/*!

This class creates the VTK object that will become the outline for the probe graphic
@param manager The ui3Manager responsible for this class
@param position The starting position of the object

 */

class BehaviorLUSOutline: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
public:
    inline BehaviorLUSOutline(vctFrm3 position):
        ui3VisibleObject(),
        outlineSource(0),
        mapOutline(0),
        outline(0),
        outlineXform(0),
        
    Position(position)
    {}
    
    inline ~BehaviorLUSOutline()
    {}
    
    inline bool CreateVTKObjects(void) {
        //set up the outline funtions
        
        outlineSource = vtkOutlineSource::New();
        CMN_ASSERT(outlineSource);
        outlineSource -> SetBounds(0,30,0,30,0,0);
        
        vtkPolyDataMapper *mapOutline = vtkPolyDataMapper::New();
        CMN_ASSERT(mapOutline);
        mapOutline->SetInputConnection(outlineSource->GetOutputPort());
        
        outline = CSOpenGLStippleActor::New();
        CMN_ASSERT(outline);
        outline->SetMapper(mapOutline);
        outline->GetProperty()->SetColor(1,1,1);
        //outline -> SetStipplePattern(1);
        
        this->outline->VisibilityOff();
        this->AddPart(this->outline);
        
        
        //int p = 1;
        // this->GetBackgroundPosition(p);
        this->SetTransformation(this->Position);
        return true;
    }
    

protected:

    vtkOutlineSource        *outlineSource;
    vtkPolyDataMapper       *mapOutline;
    CSOpenGLStippleActor    *outline;
    vtkMatrix4x4            *outlineXform;
    vctFrm3 Position;  // initial position
};

CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSOutline);
CMN_IMPLEMENT_SERVICES(BehaviorLUSOutline);



//============================================================================================================================================
/*!

This class creates the VTK object that will become the cursor and markers of the map
@param manager The ui3Manager responsible for this class

 */
class BehaviorLUSMarker: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    public:
    inline BehaviorLUSMarker(void):
    ui3VisibleObject(),
    jCylinder(0),
    jointMapper(0),
    joint(0),
    Position()
    { std::cout << "marker constructor called" << std::endl;
    }

    inline ~BehaviorLUSMarker()
    {
       
    }

    inline bool CreateVTKObjects(void) {

        std::cout << "Marker set up" << endl;

        jCylinder = vtkCylinderSource::New();
        CMN_ASSERT(jCylinder);
        jCylinder->SetHeight( 8 );
        jCylinder->SetRadius( 3 );
        jCylinder->SetResolution( 25 );

        jointMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(jointMapper);
        jointMapper->SetInputConnection( jCylinder->GetOutputPort() );


        joint = vtkActor::New();
        CMN_ASSERT(joint);
        joint->SetMapper( jointMapper);
        //joint1 -> SetStipplePattern(2);
        joint->SetScale(SCALE);

        joint->RotateX(90);
        this->AddPart(this->joint);
        return true;
    }
    
    void SetColor(double r, double g, double b) {
        if (this->joint && (r+g+b)<= 3) {
            this->joint->GetProperty()->SetColor(r, g, b);
        }
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

/*!

The struct to define the marker type that will be used on the map

*/

struct MarkerType
{
    vctFrm3 AbsolutePosition;
    BehaviorLUSMarker * VisibleObject;
    
};

//============================================================================================================================================


/*!

constructor
@param name the name of the class
@param manager The ui3Manager responsible for this class

*/


BehaviorLUS::BehaviorLUS(const std::string & name):
    ui3BehaviorBase(std::string("BehaviorLUS::") + name, 0),
    Ticker(0),
    ImagePlane(0),
    VisibleList(0),
    MarkerList(0),
    ProbeHead(0),
    ProbeJoint1(0),
    ProbeJoint2(0),
    ProbeJoint3(0),
    ProbeShaft(0),
    Backgrounds(0),
    Outline(0),
    WarningText(0),
    MeasureText(0)
{
    // add video source interfaces
    AddStream(svlTypeImageRGB, "USVideo");
std::cout<< "constructor=======================================================================" << std::endl;

    this->camera2map = vtkMatrix4x4::New();

    this->VisibleList = new ui3VisibleList("LUS Main");
    
    this->ProbeList = new ui3VisibleList("Probe");
    this->ProbeListJoint1 = new ui3VisibleList("ProbeJoint1");
    this->ProbeListJoint2 = new ui3VisibleList("ProbeJoint2");
    this->ProbeListJoint3 = new ui3VisibleList("ProbeJoint3");
    this->ProbeListShaft = new ui3VisibleList("ProbeShaft");
    this->BackgroundList = new ui3VisibleList("Background");
    this->TextList = new ui3VisibleList("Text");
    this->MapCursorList = new ui3VisibleList("MapCursor");
    this->MarkerList = new ui3VisibleList("Marker");
    this->AxesList = new ui3VisibleList("Axes");
    
    this->VisibleList->Add(this->ProbeList);
    this->VisibleList->Add(this->BackgroundList);
    this->VisibleList->Add(this->TextList);
    this->VisibleList->Add(this->MarkerList);
    this->VisibleList->Add(this->MapCursorList);
    this->VisibleList->Add(this->AxesList);

    this->ProbeHead = new BehaviorLUSProbeHead(this->Position);
    this->ProbeJoint1 = new BehaviorLUSProbeJoint(this->Position);
    this->ProbeJoint2 = new BehaviorLUSProbeJoint(this->Position);
    this->ProbeJoint3 = new BehaviorLUSProbeJoint(this->Position);
    this->ProbeShaft = new BehaviorLUSProbeShaft(this->Position);
    this->Backgrounds = new BehaviorLUSBackground(this->Position);
    this->Outline = new BehaviorLUSOutline(this->Position);
    this->WarningText = new BehaviorLUSText(this->Position);
    this->MeasureText = new BehaviorLUSText(this->Position);
    this->MapCursor = new BehaviorLUSMarker();
//     this->M1 = new BehaviorLUSMarker();
//     this->M2 = new BehaviorLUSMarker();
//     this->M3 = new BehaviorLUSMarker();
//     this->M4 = new BehaviorLUSMarker();
    this->ProbeAxes = new ui3VisibleAxes();
    this->AxesJoint1 = new ui3VisibleAxes();
    //AxesJoint1->SetSize(15);
    this->AxesJoint2 = new ui3VisibleAxes();
    //AxesJoint2->SetSize(20);
    this->AxesJoint3 = new ui3VisibleAxes();
    //AxesJoint3->SetSize(25);
    this->AxesShaft = new ui3VisibleAxes();
    //AxesShaft->SetSize(30);
    
    this->ProbeList->Add(this->ProbeHead);
    this->ProbeList->Add(this->ProbeAxes);
    this->ProbeListJoint1->Add(this->ProbeJoint1);
   // this->ProbeListJoint1->Add(this->AxesJoint1);
    this->ProbeListJoint2->Add(this->ProbeJoint2);
    //this->ProbeListJoint2->Add(this->AxesJoint2);
    this->ProbeListJoint3->Add(this->ProbeJoint3);
    //this->ProbeListJoint3->Add(this->AxesJoint3);
    this->ProbeListShaft->Add(this->ProbeShaft);
    //this->ProbeListShaft->Add(this->AxesShaft);
    this->BackgroundList->Add(this->Backgrounds);
    this->BackgroundList->Add(this->Outline);
    this->TextList->Add(this->WarningText);
    this->TextList->Add(this->MeasureText);

    
    this->ProbeList->Add(ProbeListJoint1);
    this->ProbeListJoint1->Add(ProbeListJoint2);
    this->ProbeListJoint2->Add(ProbeListJoint3);
    this->ProbeListJoint3->Add(ProbeListShaft);

    this->MapCursorList-> Add(MapCursor);
    this->VisibleList->Add(MapCursor);

 //   this->AxesList->Add(this->AxesJoint1);
  //  this->AxesList->Add(this->AxesJoint2);
    
    for(int i = 0; i<20 ; i++)
    {
        MyMarkers[i] = new BehaviorLUSMarker();
        this->MarkerList->Add(MyMarkers[i]);
    }

    this->MarkerList->Hide();

    
    std::cout << "2" << std::endl;

    // this->VisibleList->SetTransformation(vctFrm3::Identity());

    std::cout << "1" << std::endl;

    this->zero_position[0] = zero_position[1] = 0.0;
    this->Offset.SetAll(0.0);

    ClutchPressed = false;
    MarkerDropped = false;
    setCenter=false;
    Following=false;
    MapEnabled=true;
    ECMRCMtoVTKscale = 1;

    std::cout<< " end constructor=====================================================================================" << std::endl;
}
/*!

destructor

*/

BehaviorLUS::~BehaviorLUS()
{
    this->VisibleList->Hide();
}

/*!

Configures the menu bar specific for this behavior.  Creates the needed buttons in the specified order

*/

void BehaviorLUS::ConfigureMenuBar()
{
    
    std::cout<< "config menu ======================================================================================================" << std::endl;
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
 /*   this->MenuBar->AddClickButton("DropMarkerButton",
                                  3,
                                  "iconify-top-left.png",
                                  &BehaviorLUS::DropMarkerCallback,
                                  this);
    this->MenuBar->AddClickButton("RemoveMarkerButton",
                                  4,
                                  "undo.png",
                                  &BehaviorLUS::RemoveMarkerCallback,
                                  this);
*/
    std::cout<< "end config menu ======================================================================================================" << std::endl;
}


/*!

All the things that only need to be done once

 */


void BehaviorLUS::Startup(void)
{
    this->Slave1 = this->Manager->GetSlaveArm("Slave1");
    this->ECM1 = this->Manager->GetSlaveArm("ECM1");
 //   this->RMaster = this->Manager->GetMasterArm("MTMR");

    // To get the joint values, we need to access the device directly
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    CMN_ASSERT(taskManager);
    mtsDevice * daVinci = taskManager->GetTask("daVinci");
    CMN_ASSERT(daVinci);
    // get PSM1 interface
    mtsProvidedInterface * providedInterface = daVinci->GetProvidedInterface("PSM1");
    CMN_ASSERT(providedInterface);
    mtsCommandReadBase * command = providedInterface->GetCommandRead("GetPositionJoint");
    CMN_ASSERT(command);
    GetJointPositionSlave.Bind(command);
    command = providedInterface->GetCommandRead("GetPositionCartesian");
    CMN_ASSERT(command);
    GetCartesianPositionSlave.Bind(command);
    // get slave interface
    providedInterface = daVinci->GetProvidedInterface("ECM1");
    CMN_ASSERT(providedInterface);
    command = providedInterface->GetCommandRead("GetPositionJoint");
    CMN_ASSERT(command);
    GetJointPositionECM.Bind(command);
    
    // get clutch interface
    providedInterface = daVinci->GetProvidedInterface("MasterClutchPedal");
    CMN_ASSERT(providedInterface);
    mtsCommandWrite<BehaviorLUS, prmEventButton> * clutchCallbackCommand =
        new mtsCommandWrite<BehaviorLUS, prmEventButton>(&BehaviorLUS::MasterClutchPedalCallback, this,
                                                         "MasterClutchPedalCallback", prmEventButton());
    CMN_ASSERT(clutchCallbackCommand);
    providedInterface->AddObserver("Button", clutchCallbackCommand);
    
    //get camera control interface
    providedInterface = daVinci->GetProvidedInterface("CameraControlPedal");
    CMN_ASSERT(providedInterface);
    mtsCommandWrite<BehaviorLUS, prmEventButton> * cameraCallbackCommand =
        new mtsCommandWrite<BehaviorLUS, prmEventButton>(&BehaviorLUS::CameraControlPedalCallback, this,
                                                         "CameraControlPedalCallback", prmEventButton());
    CMN_ASSERT(cameraCallbackCommand);
    providedInterface->AddObserver("Button", cameraCallbackCommand);


    std::cout<< "start up ======================================================================================================" << std::endl;

    //Set the default position of the booleans
    RightMTMOpen = true;
    prevRightMTMOpen = RightMTMOpen;
    LeftMTMOpen = true;

    if (!this->Slave1) {
        CMN_LOG_CLASS_INIT_ERROR << "Startup: this behavior requires a slave arm ..." << std::endl;
    }
	
    CMN_LOG_CLASS_INIT_VERBOSE << "starting up BehaviorLUS" << std::endl;

    // Adding US image plane
    ImagePlane = new ui3ImagePlane();
    CMN_ASSERT(ImagePlane);
    // Get bitmap dimensions from pipeline.
    // The pipeline has to be already initialized to get the required info.

    ImagePlane->SetBitmapSize(GetStreamWidth("USVideo"), GetStreamHeight("USVideo"));

    // Set plane size (dimensions are already in millimeters), miltuplied by a scaling factor to fit probe dimensions
    ImagePlane->SetPhysicalSize(40.0*.35, 50.0*.35);

    // Change pivot position to move plane to the right location.
    // The pivot point will remain in the origin, only the plane moves.
    ImagePlane->SetPhysicalPositionRelativeToPivot(vct3(0.0, 0.0, 0.0));
    


    this->ImagePlane->Lock();
    this->ProbeList->Add(this->ImagePlane);
    this->ImagePlane->Unlock();
    
    MeasurePoint1.Assign(0.0,0.0,0.0);

    this->PreviousSlavePosition.Assign(this->Slave1Position.Position().Translation());
    this->PreviousCursorPosition.Assign(this->CursorPosition);
    this->CursorOffset.SetAll(0.0);
    
    MarkerCount = 0;

}

/*!

not really sure what this is supposed to do

 */

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

    }

    // detect transition, should that be handled as an event?
    // State is used by multiple threads ...
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->VisibleList->Show();
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
    this->ECM1->GetCartesianPosition(this->ECM1Position);
    //this->Slave1Position.Position().Translation().Add(this->Offset);
    this->ProbeList->SetTransformation(this->Slave1Position.Position());
    this->GetJointPositionSlave(this->JointsSlave);
    this->GetJointPositionECM(this->JointsECM);
    this->SetUpScene();
    this->SetJoints(JointsSlave.Position().Element(4),JointsSlave.Position().Element(5),JointsSlave.Position().Element(2),JointsSlave.Position().Element(3));
   // this->MapCursorList->SetTransformation(this->Slave1Position.Position());
 //   this->SetJoints(0.0,0.0,0.0,0.0);
    //this->MarkerList->SetTransformation(this->Slave1Position.Position());



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
    }

    this->Slave1->GetCartesianPosition(this->Slave1Position);
    this->ECM1->GetCartesianPosition(this->ECM1Position);
    this->Slave1Position.Position().Translation().Add(this->Offset);
    this->ProbeList->SetTransformation(this->Slave1Position.Position());
    this->GetJointPositionSlave(this->JointsSlave);
    this->SetUpScene();
    this->SetJoints(JointsSlave.Position().Element(4),JointsSlave.Position().Element(5),JointsSlave.Position().Element(2),JointsSlave.Position().Element(3));
   // this->MapCursorList->SetTransformation(this->Slave1Position.Position());
   // this->MarkerList->SetTransformation(this->Slave1Position.Position());


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
        this->ProbeList->Show();
        std::cout << "probe head visiblity: " << this->ProbeHead->Visible() << " : " << this->ProbeHead->Created()<< std::endl;
        std::cout << "Image plane visiblity" << this->ImagePlane->Visible()<< std::endl;
    }


    this->GetJointPositionSlave(this->JointsSlave);
    this->GetJointPositionECM(this->JointsECM);

    // .Positions() returns oject of type vctDynamicVector of doubles
    // for translations you might have a meter to mm conversion to do

    this->Slave1->GetCartesianPosition(this->Slave1Position);
    this->ECM1->GetCartesianPosition(this->ECM1Position);

    this->SetUpScene();

//    this->ImagePlane->SetTransformation(this->Slave1Position.Position());
    this->SetJoints(JointsSlave.Position()[4],JointsSlave.Position()[5],JointsSlave.Position()[2],JointsSlave.Position()[3]);
    
//measurement tool should be updated if right MTM is closed
    if (!RightMTMOpen && !ClutchPressed)
    {
        //cout<< "getMeasurement()" << endl;
        this-> GetMeasurement();
    }
    else {
        MeasurementActive = false;
        //this->SetText(MeasureText, " ");
    }

//prepare to drop marker if clutch and right MTM are pressed
    if(ClutchPressed & !RightMTMOpen) //ClutchPressed
    {
        this->AddMarker();
    }

//prepare to remove marker if clutch and left MTM are pressed
    if(ClutchPressed & !LeftMTMOpen)
    {
       this->RemoveLastMarker();
    }
    //show the position of the 'tool'
    this->AxesJoint2->SetTransformation(Slave1Position.Position());

//update the map if enabled other wise it should be hidden.
    if(MapEnabled)
    {

        vctDouble3 xAxis, yAxis, zAxis;
        xAxis = Slave1Position.Position().Rotation().Column(0);
        yAxis = Slave1Position.Position().Rotation().Column(1);
        zAxis = Slave1Position.Position().Rotation().Column(2);
        MapCursor->Show();
        Backgrounds->Show();
        MarkerList->Show();

//         this->UpdateMap(this->camera2map,
//           JointsECM.Position().Pointer(), // q_ecm
//           Slave1Position.Position().Translation().Pointer(),
//           xAxis.Pointer(),
//           yAxis.Pointer(),
//           zAxis.Pointer(), 
//           setCenter); // ANTON TO FIX, vec[2]);
        GetCurrentCursorPositionWRTECMRCM();


        if(CameraPressed ||
           (!ClutchPressed && (PreviousSlavePosition == Slave1Position.Position().Translation())))
        {
            if(this->MapCursor->Visible())
                {
                    this->MapCursor->Hide();
                }
            this->UpdateVisibleMap();
        }else{
            if(!this->MapCursor->Visible())
            {
                this->MapCursor->Show();
            }
        }
        
        //std::cout << "markerPos_ECMRCM: " << markerPos_ECMRCM <<std::endl;
        //MapCursor->SetTransformation(markerPos_ECMRCM);

    }
    else {
    Backgrounds->Hide();
    MarkerList->Hide();
    MapCursor->Hide();
    }
    PreviousSlavePosition=Slave1Position.Position().Translation();
    PreviousCursorPosition = CursorPosition;
    return true;
}

/*!

Places certian objects in the correct posisiton is the scene

 */

void BehaviorLUS::SetUpScene(void)
{
    
        //rotate the image plane such that it lines up with the probe
    vctDouble3 Xaxis;
    Xaxis.Assign(1.0,0.0,0.0);
    vctDouble3 Yaxis;
    Yaxis.Assign(0.0,1.0,0.0);
    vctAxAnRot3 imageRot(Yaxis, cmnPI_2);
    vctFrm3 planePosition;
    planePosition.Rotation() = vctMatRot3(imageRot);
    planePosition.Translation() = vctDouble3(0.0, 0.0, 16.0); //===============================================================================
    ImagePlane->SetTransformation(planePosition);
    
    
     //Set the position of the probe in the scene space
    vctFrm3 tmp;
    tmp.Rotation() = vctMatRot3(this->Slave1Position.Position().Rotation()) * vctMatRot3(vctAxAnRot3(vctDouble3(0.0,0.0,1.0), cmnPI_4 ));
    tmp.Translation() = vctDouble3(25.0,-45.0,-220.0); // x, y , z
    this->ProbeList->SetTransformation(tmp);

    //Set the position of the backgrounds and text in scene space
    this->Outline->SetPosition(vctDouble3(8.0,-60.0,-220.0));// x, y, z
    this->Backgrounds->SetPosition(vctDouble3(55.0,-48.0,-220.0)); //y,x,z
    this->TextList->SetPosition(vctDouble3(-25.0,-65.0,-220.0));

    //Set the position and color of the measurement text object
    this->MeasureText->SetColor(0./255, 34./255, 102.0/255);
    this->MeasureText->SetPosition(vctDouble3(0.0, 5, 0.0));
    
    MapCursor->SetColor(159.0/255, 182.0/255, 205.0/255);

}

/*!

?
*/

void BehaviorLUS::Configure(const std::string & CMN_UNUSED(configFile))
{
    // load settings
}

/*!
?
*/

bool BehaviorLUS::SaveConfiguration(const std::string & CMN_UNUSED(configFile))
{
    // save settings
    return true;
}

/*!

Function callback for the first button

 */

void BehaviorLUS::FirstButtonCallback()
{
    CMN_LOG_CLASS_RUN_DEBUG << "Behavior \"" << this->GetName() << "\" Button 1 pressed" << std::endl;
}

/*!

Function callback to toggle whether the map is functioning or not

 */

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

/*!

Function callback triggered by the closing of the right master grip.
This action will cause a marker to be dropped on the map

 */

void BehaviorLUS::PrimaryMasterButtonCallback(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::PRESSED) {
        this->RightMTMOpen = false;
        this->Following = true;
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->RightMTMOpen = true;
        this->MarkerDropped = false;
        this->Following = false;
    }
}

/*!

Function callback triggered by the closing of the left master grip.
This action will cause a marker to be removed from the map

 */
void BehaviorLUS::SecondaryMasterButtonCallback(const prmEventButton & event)
{
    if (event.Type() == prmEventButton::PRESSED) {
        this->LeftMTMOpen = false;
    } else if (event.Type() == prmEventButton::RELEASED) {
        this->LeftMTMOpen = true;
        this->MarkerRemoved = false;
    }
}

/*!

Function callback triggered by pressing the master cluch pedal
Changes the state of the behavior and allows some other features to become active

*/

void BehaviorLUS::MasterClutchPedalCallback(const prmEventButton & payload)
{
    if (payload.Type() == prmEventButton::PRESSED) {
        this->ClutchPressed = true;
        //std::cout << ClutchPressed << std::endl;
    } else {
        this->ClutchPressed = false;
        //std::cout << ClutchPressed << std::endl;
    }
}

/*!

Function callback triggered by pressing the camera control pedal
Changes the state of the behavior and allows some other features to become active

 */

void BehaviorLUS::CameraControlPedalCallback(const prmEventButton & payload)
{
    if (payload.Type() == prmEventButton::PRESSED) {
        this->CameraPressed = true;
    } else {
        this->CameraPressed = false;
    }
}

/*!

?

*/

void BehaviorLUS::OnStreamSample(svlSample* sample, int streamindex)
{
    if (State == Foreground) {
        ImagePlane->SetImage(dynamic_cast<svlSampleImageBase*>(sample), streamindex);
    }
}



/*--------------------------------------------------------------------------------------*/


/*!
Sets orientation of the probe graphic based on the joint angles from the da Vinci api

@param A1 picth angle for joint 1 and 4
@param A2 yaw angle for joints 2 and 3
@param insertion the insertion 'joint'
@param roll the roll joint value
 */

void BehaviorLUS::SetJoints(double A1, double A2, double insertion, double roll)
{
    //create axis of rotation
    vctDouble3 Xaxis;
    Xaxis.Assign(1.0,0.0,0.0);
    vctDouble3 Yaxis;
    Yaxis.Assign(0.0,1.0,0.0);
    vctDouble3 Zaxis;
    Zaxis.Assign(0.0,0.0,1.0);

    //translate and rotate the probe head graphic into camera frame and such that it lines up with the physical tool
    vctFrm3 probePosition;
    vctAxAnRot3 probeRot(Xaxis, cmnPI);
    probePosition.Rotation() = vctMatRot3(probeRot);
    probePosition.Translation() = vct3(-8.0*SCALE,12.0*SCALE, 70.0*SCALE);  //-8,12, 70
    this->ProbeHead->SetTransformation(probePosition);
    
    //pitch and yaw are in radians
    double pitch = A1;
    double yaw = -A2;

    // double total = fabs(pitch) + fabs(yaw);
    //    cout << "Probe wrist pitch: " << pitch << endl;
    //    cout << "Probe wrist yaw:   " << yaw << endl;
    //    cout << "total angle:       " << total << endl;
    //    cout << "insertion:         " << insertion << endl;
    //    cout << "roll:              " << roll << endl;

    //set up first joint position, needs to be lined up with probe head and rotated properly
    //pitch joint
    vctFrm3 j1pos;
    j1pos.Rotation() = vctMatRot3(vctAxAnRot3(Xaxis, pitch)) * vctMatRot3(vctAxAnRot3 (Xaxis, cmnPI_2)) * vctMatRot3(vctAxAnRot3(Yaxis, cmnPI_2));
    j1pos.Translation() = vctDouble3(0.0, 3.0*SCALE, -12.0*SCALE);
    ProbeListJoint1->SetTransformation(j1pos);

    //set up second joint, yaw joint
    vctFrm3 j2pos;
    j2pos.Rotation() = vctMatRot3(vctAxAnRot3(Xaxis, yaw));
    //should be 7 mm gap between joints
    j2pos.Translation() = vctDouble3(0.0,-7.0*SCALE,0.0);
    ProbeListJoint2 -> SetTransformation(j2pos);

    //set up third joint, yaw joint
    vctFrm3 j3pos;
    j3pos.Rotation() = vctMatRot3(vctAxAnRot3(Xaxis, yaw));
    j3pos.Translation() = vctDouble3(0.0,-7.0*SCALE,0.0);
    ProbeListJoint3 -> SetTransformation(j3pos);

    //set up the shaft of the probe, pitch joint
    vctFrm3 shaftpos;
    shaftpos.Rotation() = vctMatRot3(vctAxAnRot3(Zaxis, pitch));
    shaftpos.Translation() = vctDouble3(0.0, -7.0*SCALE, 0.0);
    ProbeListShaft -> SetTransformation(shaftpos);
    
    //checks the limits and changes the color and text
    CheckLimits(pitch, yaw, insertion, roll);
}

/*!

Changes the color of the probe graphic
@param r the red color must be between 0 and 1
@param g the green color must be between 0 and 1
@param b the blue color must be between 0 and 1

*/
void BehaviorLUS::SetProbeColor(double r, double g, double b)
{

    this->ProbeHead ->SetColor(r, g, b);
    this->ProbeJoint1->SetColor(r, g, b);
    this->ProbeJoint2->SetColor(r, g, b);
    this->ProbeJoint3->SetColor(r, g, b);
    this->ProbeShaft->SetColor(r, g, b);

}

/*!

Sets the text of the text objects
@param obj the text object in which the text is being set or changed
@param text the text

*/

void BehaviorLUS::SetText(BehaviorLUSText * obj, const std::string & text)
{
    obj->SetText(text);
}

/*!

Checks if the joint limits are close to reaching there maximum limits
changes the text and color of the probe accordingly 
@param p the pitch joint value
@param y the yaw joint value
@param i the insertion joint value
@param r the roll joint value

 */

void BehaviorLUS::CheckLimits(double p, double y, double i, double r)
{
        //convert the pitch and yaw from radians into degrees 
    double pitch = (p*180/cmnPI);
    double yaw = (y*180/cmnPI);
    double insertion = i;
    double roll = r;

    double total = fabs(pitch) + fabs(yaw);

    //check each limit in order of importance
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
        SetText(WarningText, "Insertion Limit Reached");
    }
    else if (roll > 4.5 || roll < -4.5)
    {
        SetProbeColor( 1, 165.0/255, 79.0/255 );
        SetText(WarningText, "Roll limit Reached");
    }
    else
    {
        //default color and text
        SetProbeColor( 1.0,1.0,1.0);//127./255, 255./255, 212./255 );
        SetText(WarningText, " ");
    }

    if( total < 6 ) //turn blue if the probe is straight 
    {
        SetProbeColor(159.0/255, 182.0/255, 205.0/255) ;
    }

}

/*!

Displays the absolute 3D distance the center of the probe has moved since being activated

*/

void BehaviorLUS::GetMeasurement()
{

    char    measure_string[100];
    vctFrm3 frame, correctionFrame;
    vctDouble3 correction(0.0, 0.0, 30.0); // 30 mm along probe

    correctionFrame.Translation() = correction;
    frame.ProductOf(Slave1Position.Position(), correctionFrame); //Moves the point from the control point of the probe to the center

    if (!MeasurementActive)
    {
        MeasurementActive = true;
        //saves the first point
        MeasurePoint1.Assign(frame.Translation());
        std::cout<< "MeasurePoint1 from if statement: " << MeasurePoint1<< std::endl;
    } else {
        //calculates the distance maoved
        vctDouble3 diff;
        diff.DifferenceOf(MeasurePoint1, frame.Translation());

        double AbsVal = diff.Norm();

        //displays the distance in mm 
        sprintf(measure_string,"%4.1fmm", AbsVal);
        this->SetText(MeasureText, measure_string);
    }
}

/*!
    Transform the probe position. Return the cursor position in map coordinates.

    @param P_psmtip_ecmframe    Result: position of PSM tip in ECM frame (aka hframe). -- double 4
    @param q_ecm                ecm joint vector -- 4 floats
    @param P_psmtip_cam         Position of PSM tip in camera frame. -- 3 floats, translation
    @param x,y,z axis 		columns  of the PSM rotation matrix 

    @return 
 */
// [0] = outer yaw
// [1] = outer pitch
// [2] = scope insertion
// [3] = scope roll

void BehaviorLUS::UpdateMap(vtkMatrix4x4 * Camera2ECM,
          double *q_ecm,  // q_ecm
          double *P_psmtip_cam,
          double *xaxis, double *yaxis, double *zaxis,
          bool & setCenter)
{
    double *P_psmtip_ecmframe;
    double  ecm_rcm_to_tip_displacement[4]={0.0};
//    vtkMatrix4x4  *T_to_horiz, *T_yaw, *T_pitch, *T_roll;
//    CreateVTKObject(T_to_horiz, vtkMatrix4x4);
    vtkMatrix4x4 *T_to_horiz = vtkMatrix4x4::New();
    vtkMatrix4x4 *T_yaw = vtkMatrix4x4::New();
    vtkMatrix4x4 *T_pitch = vtkMatrix4x4::New();
    vtkMatrix4x4 *T_roll = vtkMatrix4x4::New();

    double  xaxis_ecm[4], yaxis_ecm[4], zaxis_ecm[4];

    double  P_tmp1[4], P_tmp2[4];//, withZoffset[3];
    double  angle   = 30;  //angle for S system
 //   double  Zoffset = 50;
 //   int     i;
    // float *P_psmtip_ecmframe[4];


    // Get ECM insertion dispacement in millimetres.
    ecm_rcm_to_tip_displacement[2] = q_ecm[2]*1000.0;

    SetTransform ( T_to_horiz,   1.0, 0.0, 0.0, 0.0,
                                 0.0, cos(angle*cmnPI/180), -sin(angle*cmnPI/180), 0.0,
                                 0.0, sin(angle*cmnPI/180),  cos(angle*cmnPI/180), 0.0);

    SetTransform(T_yaw, cos(q_ecm[0]), 0.0, sin(q_ecm[0]), 0.0,
                        0.0,           1.0, 0.0,           0.0,
                        -sin(q_ecm[0]),0.0, cos(q_ecm[0]),0.0);

    SetTransform(T_pitch,   1.0, 0.0,            0.0,             0.0,
                            0.0, cos(-q_ecm[1]), -sin(-q_ecm[1]), 0.0,
                            0.0, sin(-q_ecm[1]),  cos(-q_ecm[1]), 0.0);

    SetTransform(T_roll, cos(q_ecm[3]), -sin(q_ecm[3]), 0.0, 0.0,
                         sin(q_ecm[3]),  cos(q_ecm[3]), 0.0, 0.0,
                         0.0,            0.0,           1.0, 0.0);



    // Transform PSM TIP coordinates from control point to center of probe.
    P_psmtip_cam[0] += xaxis[2]*40.0;
    P_psmtip_cam[1] += yaxis[2]*40.0;
    P_psmtip_cam[2] += zaxis[2]*40.0;

//         // Transform PSM1 TIP positions into the "nominal" ECM RCM frame
    vectorSum(P_psmtip_cam, ecm_rcm_to_tip_displacement, P_tmp1);
    T_roll -> MultiplyPoint(P_tmp1, P_tmp2);
    T_pitch -> MultiplyPoint(P_tmp2, P_tmp1);
    T_yaw -> MultiplyPoint(P_tmp1, P_tmp2);
    T_to_horiz -> MultiplyPoint(P_tmp2, P_psmtip_ecmframe);
//cout << "1536" << endl;
//         // Transform xaxis positions into the "nominal" ECM RCM frame
            //vectorSum(xaxis, ecm_rcm_to_tip_displacement, P_tmp1);
    P_tmp1[0] = xaxis[0]; //xaxis[0];
    P_tmp1[1] = yaxis[0]; //xaxis[1];
    P_tmp1[2] = zaxis[0]; //xaxis[2];
    P_tmp1[3] = 1.0;
    T_roll -> MultiplyPoint(P_tmp1, P_tmp2);
    T_pitch -> MultiplyPoint(P_tmp2, P_tmp1);
    T_yaw -> MultiplyPoint(P_tmp1, P_tmp2);
//cout << "1542" << endl;
    T_to_horiz -> MultiplyPoint(P_tmp2, xaxis_ecm);

//         // Transform yaxis positions into the "nominal" ECM RCM frame
            //vectorSum(yaxis, ecm_rcm_to_tip_displacement, P_tmp1);
    P_tmp1[0] = xaxis[1]; //yaxis[0];
    P_tmp1[1] = yaxis[1]; //yaxis[1];
    P_tmp1[2] = zaxis[1]; //yaxis[2];
    P_tmp1[3] = 1.0;
    T_roll -> MultiplyPoint(P_tmp1, P_tmp2);
    T_pitch -> MultiplyPoint(P_tmp2, P_tmp1);
    T_yaw -> MultiplyPoint(P_tmp1, P_tmp2);
    T_to_horiz -> MultiplyPoint(P_tmp2, yaxis_ecm);

//         // Transform zaxis positions into the "nominal" ECM RCM frame
            //vectorSum(zaxis, ecm_rcm_to_tip_displacement, P_tmp1);
    P_tmp1[0] = xaxis[2]; //zaxis[0];
    P_tmp1[1] = yaxis[2]; //zaxis[1];
    P_tmp1[2] = zaxis[2]; //zaxis[2];
    P_tmp1[3] = 1.0;
    T_roll -> MultiplyPoint(P_tmp1, P_tmp2);
    T_pitch -> MultiplyPoint(P_tmp2, P_tmp1);
    T_yaw -> MultiplyPoint(P_tmp1, P_tmp2);
    T_to_horiz -> MultiplyPoint(P_tmp2, zaxis_ecm);



        // Column for x-axis
    Camera2ECM->SetElement(0, 0, xaxis[2]);
    Camera2ECM->SetElement(1, 0, -yaxis[2]);
    Camera2ECM->SetElement(2, 0, zaxis[2]);

        // Column for y-axis
    Camera2ECM->SetElement(0, 1, -xaxis[0]);
    Camera2ECM->SetElement(1, 1, yaxis[0]);
    Camera2ECM->SetElement(2, 1, -zaxis[0]);

        // Column for z-axis
    Camera2ECM->SetElement(0, 2, xaxis[1]);
    Camera2ECM->SetElement(1, 2, -yaxis[1]);
    Camera2ECM->SetElement(2, 2, zaxis[1]);
        



        // Reset this position to be the center of the map.
        // TO DO: existing marker coordinates are not updated.
    if(setCenter==true)
    {
        zero_position[0] = P_psmtip_ecmframe[0];
        zero_position[1] = P_psmtip_ecmframe[2];
    }

//this works!
    Camera2ECM -> SetElement(0, 3, (-(P_psmtip_ecmframe[0]-zero_position[0])/5.0)+ 40.5); //x stays the same
    Camera2ECM -> SetElement(1, 3, (+(P_psmtip_ecmframe[2]-zero_position[1])/5.0)- 43.0); //z of ECM becomes y
    Camera2ECM -> SetElement(2, 3, DEPTH);//P_psmtip_ecmframe[1]); //y of ECM becomes z, should be at fixed depth of 60 mm

  //  cursorActor -> SetUserMatrix(Camera2ECM);

    Camera2ECM->Print(std::cout);
    //MapCursor->SetVTKMatrix(Camera2ECM);
    //std::cout << "MapCursor" << MapCursor->GetTransformation() << std::endl;
 //   CursorPos = GetCurrentCursorPositionWRTECMRCM();
}

/*!

Adds a marker to the list of markers 
the position of the marker is the position of the cursor at the time that it is dropped
 */

void BehaviorLUS::AddMarker(void)
{
    if(MarkerDropped == false)
        {

            BehaviorLUSMarker * newMarkerVisible;
            newMarkerVisible = MyMarkers[MarkerCount];
            newMarkerVisible->SetColor(153.0/255.0, 255.0/255.0, 153.0/255.0); 
            if(MarkerCount < 20)
            {
                MarkerType * newMarker = new MarkerType;
                // create a visible object for each marker
                // newMarkerVisible->CreateVTKObjects();
                newMarkerVisible->Show();
                std::cout<< "newMarkerVisible: " << newMarkerVisible->Visible() << std::endl;
                newMarker->VisibleObject = newMarkerVisible;
                // set the position of the marker based on current cursor position
                newMarker->AbsolutePosition = GetCurrentCursorPositionWRTECMRCM();
                newMarkerVisible->SetTransformation(newMarker->AbsolutePosition);
                std::cout << "GetCurrentCursorPositionWRTECMRCM()" << newMarker->AbsolutePosition << std::endl;
                // add the marker to the list
                this->Markers.push_back(newMarker); //need to delete them too
                this->MarkerList->Add(newMarkerVisible);
                // update the list (updates bounding box and position of all markers
                this->UpdateVisibleMap();
                std::cout << "AddMarker has been called " << MapCursor->GetTransformation() << std::endl;

                MarkerCount++;
            }
            MarkerDropped = true;
        }
}


/*!

Removes the last marker from the list

*/
void BehaviorLUS::RemoveLastMarker(void)
{
    if(MarkerRemoved ==false)
    {
        if(MarkerCount > 0)
        {
            MarkerCount--;
            MyMarkers[MarkerCount]->Hide();
           // Markers.pop_back();
            std::cout << "marker removed" << std::endl;
            //this->MarkerList->RemoveLast();
        }else{
            std::cout<< "There are no more markers to remove" << std::endl;
        }
        std::cout << "Marker Count: " << MarkerCount << std::endl;
        MarkerRemoved = true;
    }
}

/*!

returns the current position of the center of the tool in the camera frame
@return the frame of the tool

*/
vctFrm3 BehaviorLUS::GetCurrentCursorPositionWRTECM(void)
{

    vctFrm3 frame, correctionFrame;

    vctDouble3 correction(0.0, 0.0, 30.0); // 30 mm along probe

    correctionFrame.Translation() = correction;
    frame.ProductOf(Slave1Position.Position(), correctionFrame);

    return frame;
}

/*!

Returns the current position of the center of the tool in the frame of the camera Remote center of motion
@return the frame of the tool wrt to the ECM RCM

 */

vctFrm3 BehaviorLUS::GetCurrentCursorPositionWRTECMRCM(void)
{
    vctDouble3 Xaxis;
    Xaxis.Assign(1.0,0.0,0.0);
    vctDouble3 Yaxis;
    Yaxis.Assign(0.0,1.0,0.0);
    vctDouble3 Zaxis;
    Zaxis.Assign(0.0,0.0,1.0);

    // get joint values for ECM
    GetJointPositionECM(JointsECM);
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
    yawFrame0.Rotation() = vctMatRot3( vctAxAnRot3(Yaxis, yaw0 ) );

    //create frame for pitch
    vctFrm3 pitchFrame1;
    pitchFrame1.Rotation() = vctMatRot3( vctAxAnRot3(Xaxis, -pitch1) );  // we don't have any logical explanation 

    //create frame for insertion
    vctFrm3 insertFrame2;
    insertFrame2.Translation() = vctDouble3(0.0, 0.0, insert2);

    //create frame for the roll
    vctFrm3 rollFrame3;
    rollFrame3.Rotation() = vctMatRot3( vctAxAnRot3(Zaxis, roll3) );

    vctFrm3 T_to_horiz;
    T_to_horiz.Rotation() = vctMatRot3( vctAxAnRot3(Xaxis, angle) );

    //create frame for the current probe center in the camera frame
    vctFrm3 fixedPoint;
    fixedPoint.Translation()= GetCurrentCursorPositionWRTECM().Translation();
 
    // raw cartesian position from slave daVinci, no ui3 correction
    prmPositionCartesianGet slavePosition;
    GetCartesianPositionSlave(slavePosition);

//     std::cout<< "Slave1Position" << Slave1Position<< std::endl;
//     std::cout<< "slavePosition" << slavePosition<< std::endl;
//     std::cout<< "slavePosition inverse" << slavePosition.Position().Inverse()<< std::endl;
//     
    vctFrm3 ProbeOffset;
    ProbeOffset.Translation().Assign(vctDouble3(0.0, 0.0, 30.0));
    
//    fixedPoint.Assign(slavePosition.Position());
//    fixedPoint.Translation().Assign(slavePosition.Position().Translation());

    
 //   fixedPoint.Translation().


    vctFrm3 finalFrame;

    ECMtoECMRCM = yawFrame0 * pitchFrame1 * insertFrame2 * rollFrame3;
    vctFrm3 imdtframe;
    // imdtframe = yawFrame0 * pitchFrame1 * insertFrame2 * rollFrame3 * fixedpoint; // workinf fixed point !!!
    imdtframe = ECMtoECMRCM * slavePosition.Position()* ProbeOffset; //* GetCurrentCursorPositionWRTECM(); // working fixed point !!!
    finalFrame = imdtframe;//.Inverse(); // .InverseSelf();

    vctFrm3 cursorVTK;
    
    ECMRCMtoVTK.ApplyTo(finalFrame, cursorVTK);// cursorVTK = ECMRCMtoVTK * finalframe

    vctDouble3 t1;
    t1 = (cursorVTK.Translation() - CenterRotatedTranslated) * ECMRCMtoVTKscale;
    t1 += CenterRotatedTranslated;
    cursorVTK.Translation().Assign(t1);
    MapCursor->SetTransformation(cursorVTK);

    return finalFrame;
}



/*

// temparary placeholder for data conversion double to float
vctFloat3 psmtip = Slave1Position.Position().Translation();
vctFloat3 xAxis, yAxis, zAxis;
xAxis = Slave1.Position().Rotation().Column(0);

vtkMatrix4x4 *camera2map = vtkMatrix4x4::New(); //should be in constructor

UpdateMap(camera2map
          ECMJoints.Position().Pointer(), // q_ecm
          psmtip.Pointer(),
          xAxis.Pointer(),
          yAxis.Pointer(),
          zAxis.Pointer())
{

delete all vtkobjects at the end of this function
}
*/

// this method should be called after:
// -1- a marker is added (not required with removed)
// -1- the camera is clutch is release and the teleoperation resumes
/*!

Updated the map, uses a re-sizing algorithm each time that a new marker is dropped

*/
void BehaviorLUS::UpdateVisibleMap(void)
{
    vctDouble3 corner1, corner2, center, centerRotated;
    MarkersType::iterator iter = Markers.begin();
    const MarkersType::iterator end = Markers.end();
    vctDouble3 currentOrigin;

    vctDouble3 scaleCompTrans(0.0);
    
    // iterate through all elements to build a bounding box
    if (iter != end)
    {
        // initialize the bounding box corners using the first element
        currentOrigin = (*iter)->AbsolutePosition.Translation();
        corner1.Assign(currentOrigin);
        corner2.Assign(currentOrigin);
        iter++;
        // update with all remaining elements
        for (; iter != end; iter++)
        {
            currentOrigin = (*iter)->AbsolutePosition.Translation();
            corner1.ElementwiseMinOf(corner1, currentOrigin);
            corner2.ElementwiseMaxOf(corner2, currentOrigin);
        }
        // computer center of bounding box
        center.SumOf(corner1, corner2);
        center.Divide(2.0);

        vctDynamicVector<vctDouble3> corners, cornersRotated;
        corners.SetSize(8);
        cornersRotated.SetSize(8);

        corners[0].Assign(corner1[0], corner1[1], corner1[2]);
        corners[1].Assign(corner1[0], corner1[1], corner2[2]);
        corners[2].Assign(corner1[0], corner2[1], corner1[2]);
        corners[3].Assign(corner1[0], corner2[1], corner2[2]);

        corners[4].Assign(corner2[0], corner1[1], corner1[2]);
        corners[5].Assign(corner2[0], corner1[1], corner2[2]);
        corners[6].Assign(corner2[0], corner2[1], corner1[2]);
        corners[7].Assign(corner2[0], corner2[1], corner2[2]);

        vctFrm3 ECMtoVTK;
        ECMtoVTK.Rotation().From( vctAxAnRot3(vctDouble3(0.0,1.0,0.0), cmnPI) );
        
        vctFrm3 ECMtoECMRCMInverse = ECMtoECMRCM.Inverse();
        
        vctFrm3 temp;
        temp = ECMtoVTK * ECMtoECMRCMInverse;
        vctDouble3 corner1Rotated, corner2Rotated;
        for(int i = 0; i<8; i++)
        {
            temp.ApplyTo(corners[i], cornersRotated[i]);
            if(i == 0)
            {
                corner1Rotated = cornersRotated[0];
                corner2Rotated = cornersRotated[0];
            }else{
                corner1Rotated.ElementwiseMinOf(corner1Rotated, cornersRotated[i]);
                corner2Rotated.ElementwiseMaxOf(corner2Rotated, cornersRotated[i]);
            }
        }
        centerRotated.SumOf(corner1Rotated, corner2Rotated);
        centerRotated.Divide(2.0);
        double h,w, ratioH, ratioW;
        h = corner2Rotated.Y() - corner1Rotated.Y();
        w = corner2Rotated.X() - corner1Rotated.X();
        //std::cout << "h: " << h << " w: " << w << std::endl;
        ratioH = h/(25.0/1.25);
        ratioW = w/(30.0/1.25);
        if(ratioH > ratioW)
        {
            ratioW = ratioH;//ratioW is now biggest
        }
        if(ratioW >1)
        {
            ECMRCMtoVTKscale = 1.0/ratioW;
            //std::cout << "ECMRCMtoVTKscale" << ECMRCMtoVTKscale << std::endl;
        }
       
        // computer the transformation to be applied to all absolute coordinates
        // to be display in the SAW coordinate system
        vctDouble3 centerInSAW(this->Backgrounds->GetVTKProp()->GetCenter());// center of the map background
        vctDouble3 offsetUp;
       offsetUp.X() = centerInSAW.X() - centerRotated.X();
       offsetUp.Y() = centerInSAW.Y() - centerRotated.Y();
       offsetUp.Z() = centerInSAW.Z() - corner1Rotated.Z() + 4.0;
       offsetUp.Z() -= ((corner2Rotated.Z() - corner1Rotated.Z()) / 2.0) * (1.0 - ECMRCMtoVTKscale); 
//         offsetUp.X() = centerInSAW.X() - centerInSAW.X()*(corner1Rotated.Z() + 4.0)/centerInSAW.Z();
//         offsetUp.Y() = centerInSAW.Y() - centerInSAW.Y()*(corner1Rotated.Z() + 4.0)/centerInSAW.Z();
        
        
 //       centerInSAW.Add(offsetUp);
//         vctDouble3 BBcenterInSAW = ECMtoVTK * ECMtoECMRCMInverse * center;
// 
//         vctFrm3 VTKrecenter;
//         VTKrecenter.Translation().Assign(centerInSAW - BBcenterInSAW);
// 
//         ECMRCMtoVTK =VTKrecenter * ECMtoVTK * ECMtoECMRCMInverse;
            vctFrm3 VTKrecenter;

            //std::cout << "offsetUp: " << offsetUp << std::endl;
            VTKrecenter.Translation().Assign(offsetUp);
            ECMRCMtoVTK =VTKrecenter * ECMtoVTK * ECMtoECMRCMInverse;
            
            CenterRotatedTranslated = VTKrecenter * centerRotated;
    }

 //   std::cout << "Bouding box: [" << corner1 << "] [" << corner2 << "]" << std::endl;
    
    // apply the transformation to all absolute coordinates

    vctFrm3 positionInSAW;

#if 1
    for (iter = Markers.begin(); iter != end; iter++)
    {
        vctDouble3 t1;
        ECMRCMtoVTK.ApplyTo((*iter)->AbsolutePosition, positionInSAW);//positionInSAW = ECMRCMtoVTK * Absolutepositon
        t1 = (positionInSAW.Translation() - CenterRotatedTranslated) * ECMRCMtoVTKscale;
        t1 += CenterRotatedTranslated;
        positionInSAW.Translation().Assign(t1);

        (*iter)->VisibleObject->SetTransformation(positionInSAW);
        //compute vtk bounding box
        //pull cloud forward
        //apply scale to whole thing
       // std::cout << "Marker at: " << positionInSAW.Translation() << std::endl;
    }
#endif
 //   this->MarkerList->SetScale(this->ECMRCMtoVTKscale);
 //   this->MarkerList->SetTransformation(this->ECMRCMtoVTK);
}

/*!

copies the given elements into a vtkMatrix4x4
@param mat the resulting matrix
@param elements the elements of the matrix, designated by their row and column 

*/

void BehaviorLUS::SetTransform(vtkMatrix4x4 *mat, double e11, double e12, double e13, double e14,
                                     double e21, double e22, double e23, double e24,
                                     double e31, double e32, double e33, double e34)
{
    double elements[16] = { e11,  e12,  e13,  e14,
        e21,  e22,  e23,  e24,
        e31,  e32,  e33,  e34,
        0.0,  0.0,  0.0,  1.0};

        mat -> DeepCopy(elements);
}


/*!
    Vector sum (4x1).
    @param  A       4x1 vector.
    @param  B       4x1 vector.
    @param  Result  The vector sum. 3x1 vector.
    @returns        Pointer to Result=A+B.
 */
void BehaviorLUS::vectorSum(double A[4], double B[4], double Result[4])
{
    Result[0] = A[0] + B[0];
    Result[1] = A[1] + B[1];
    Result[2] = A[2] + B[2];
    Result[3] = 1.0;
}

//         switch(MarkerCount)
//         {
//             case 0:
//                     M1->SetTransformation(MapCursor->GetTransformation());
//                     newMarkerVisible= M1;
//                     std::cout<< "M1 placed" <<std::endl;
//                     break;
//             case 1: 
//                     M2->SetTransformation(MapCursor->GetTransformation());
//                     newMarkerVisible= M2;
//                     break;
//             case 2: 
//                     M3->SetTransformation(MapCursor->GetTransformation());
//                     newMarkerVisible= M3;
//                     break;
//             case 3:
//                     M4->SetTransformation(MapCursor->GetTransformation());
//                     newMarkerVisible= M4;
//                     break;
//             default: std::cout << "there are no more markers to drop" << std::endl;
//         }
