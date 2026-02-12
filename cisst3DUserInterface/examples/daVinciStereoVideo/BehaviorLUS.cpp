/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

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

#include <cisst3DUserInterface/ui3VTKStippleActor.h>
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
#include "vtkVectorText.h"
#include "vtkFollower.h"
#include "vtkTextActor.h"
#include "vtkCubeSource.h"
#include "vtkMatrix4x4.h"
#include "vtkAxesActor.h"
#include "string.h"

#define SCALE .25



/*!

This class creates the VTK object from a STL file of the ultrasound probe head
@param position The starting position of the object
@param name name of the object

*/
#if 1
class BehaviorLUSProbeHead: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
      inline BehaviorLUSProbeHead(vctFrm3 position, const std::string & name = "ProbeHead"):
        ui3VisibleObject(name),
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
            partMapper->ImmediateModeRenderingOn();


            probeActorS = ui3VTKStippleActor::New();
            CMN_ASSERT(probeActorS);
            probeActorS -> SetMapper(partMapper);


            probeActorS -> GetProperty() -> EdgeVisibilityOff();
            probeActorS -> SetScale(SCALE);

        this->AddPart(this->probeActorS);
        this->SetTransformation(this->Position);

        return true;
    }
    
    inline bool UpdateVTKObjects(void) {
        return false;
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
    ui3VTKStippleActor    *probeActorS;

    vctFrm3 Position; // initial position
};


CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSProbeHead);
CMN_IMPLEMENT_SERVICES(BehaviorLUSProbeHead);

#endif

/*!

This class creates the VTK object that will become the joints of the probe
@param position The starting position of the object
@param name name of the object

 */

class BehaviorLUSProbeJoint: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    public:
    inline BehaviorLUSProbeJoint(vctFrm3 position,const std::string & name = "ProbeJoint"):
    ui3VisibleObject(name),
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
        jointMapper->ImmediateModeRenderingOn();


        joint = ui3VTKStippleActor::New();
        CMN_ASSERT(joint);
        joint->SetMapper(jointMapper);
            //joint1 -> SetStipplePattern(2);
        joint->SetScale(SCALE);

        this->AddPart(this->joint);
        this->SetTransformation(this->Position);
        return true;
    }
    
    inline bool UpdateVTKObjects(void) {
        return false;
    }

    void SetColor(double r, double g, double b) {
        if (this->joint && (r+g+b)<= 3) {
            this->joint->GetProperty()->SetColor(r, g, b);
        }
    }


    protected:
        vtkCylinderSource *jCylinder;
        vtkPolyDataMapper *jointMapper;
        ui3VTKStippleActor *joint;
    public:
        vctFrm3 Position; // initial position

};

CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSProbeJoint);
CMN_IMPLEMENT_SERVICES(BehaviorLUSProbeJoint);

/*!

This class creates the VTK object that will become the shaft of the probe
@param position The starting position of the object
@param name name of the object

 */

class BehaviorLUSProbeShaft: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    public:
    inline BehaviorLUSProbeShaft(vctFrm3 position, const std::string & name = "Probe Shaft"):
        ui3VisibleObject(name),
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
            shaftMapper->ImmediateModeRenderingOn();

            //vtkActor *shaftActor = vtkActor::New();
            shaftActor = ui3VTKStippleActor::New();
            CMN_ASSERT(shaftActor);
            shaftActor->SetMapper( shaftMapper );
            shaftActor->SetScale(SCALE);
            //shaftActor-> SetStipplePattern(2);


            this->AddPart(this->shaftActor);
            this->SetTransformation(this->Position);
        return true;
        }

    inline bool UpdateVTKObjects(void) {
        return false;
    }

        void SetColor(double r, double g, double b) {
            if (this->shaftActor && (r+g+b)<= 3) {
                this->shaftActor->GetProperty()->SetColor(r, g, b);
            }
        }

    protected:
        vtkCylinderSource *shaftSource;
        vtkPolyDataMapper *shaftMapper;
        ui3VTKStippleActor *shaftActor;
    public:
        vctFrm3 Position;  // initial position

};

CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSProbeShaft);
CMN_IMPLEMENT_SERVICES(BehaviorLUSProbeShaft);


//==================================================================================================================
/*!

This class creates the VTK text object that will create text in the scene
@param position The starting position of the object
@param name name of the object

 */
class BehaviorLUSText: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    inline BehaviorLUSText(vctFrm3 position, const std::string & name = "Text"):
        ui3VisibleObject(name),
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
        warningtextMapper->ImmediateModeRenderingOn();

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

    inline bool UpdateVTKObjects(void) {
        return false;
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
@param position The starting position of the object
@param name name of the object

 */
class BehaviorLUSBackground: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    public:
    inline BehaviorLUSBackground(vctFrm3 position, const std::string & name = "Background"):
        ui3VisibleObject(name),
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
            cubePlaneMapper->ImmediateModeRenderingOn();

            cubePlane = ui3VTKStippleActor::New();
            CMN_ASSERT(cubePlane);
            cubePlane->SetMapper(cubePlaneMapper);
            cubePlane->GetProperty()->SetColor(1,1,1);
           // cubePlane -> SetStipplePattern(1);


        this->cubePlane->VisibilityOff();
        this->AddPart(this->cubePlane);

        this->SetTransformation(this->Position);
        return true;
    }

    inline bool UpdateVTKObjects(void) {
        return false;
    }

    protected:
 
        vtkCubeSource           *cubeSource;
        vtkPolyDataMapper       *cubePlaneMapper;
        ui3VTKStippleActor    *cubePlane;
        vctFrm3 Position;  // initial position
};

CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSBackground);
CMN_IMPLEMENT_SERVICES(BehaviorLUSBackground);
//===========================================================================================================================================

/*!

This class creates the VTK object that will become the outline for the probe graphic
@param position The starting position of the object
@param name name of the object

 */

class BehaviorLUSOutline: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
public:
    inline BehaviorLUSOutline(vctFrm3 position, const std::string & name = "Outline"):
        ui3VisibleObject(name),
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
        mapOutline->ImmediateModeRenderingOn();

        outline = ui3VTKStippleActor::New();
        CMN_ASSERT(outline);
        outline->SetMapper(mapOutline);
        outline->GetProperty()->SetColor(1,1,1);

        this->outline->VisibilityOff();
        this->AddPart(this->outline);

        this->SetTransformation(this->Position);
        return true;
    }
    
    inline bool UpdateVTKObjects(void) {
        return false;
    }

protected:

    vtkOutlineSource        *outlineSource;
    vtkPolyDataMapper       *mapOutline;
    ui3VTKStippleActor    *outline;
    vtkMatrix4x4            *outlineXform;
    vctFrm3 Position;  // initial position
};

CMN_DECLARE_SERVICES_INSTANTIATION(BehaviorLUSOutline);
CMN_IMPLEMENT_SERVICES(BehaviorLUSOutline);


/*!

This class creates the VTK object that will become the cursor and markers of the map
@param name name of the object

 */
class BehaviorLUSMarker: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    public:
    inline BehaviorLUSMarker(const std::string & name):
    ui3VisibleObject(name),
    jCylinder(0),
    jointMapper(0),
    joint(0),
    Position()
    { std::cout << "marker constructor called: " <<name << std::endl;
    }

    inline ~BehaviorLUSMarker()
    {
       
    }

    inline bool CreateVTKObjects(void) {

        //std::cout << "Marker set up: "<< endl;

        jCylinder = vtkCylinderSource::New();
        CMN_ASSERT(jCylinder);
        jCylinder->SetHeight( 8 );
        jCylinder->SetRadius( 3 );
        jCylinder->SetResolution( 25 );

        jointMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(jointMapper);
        jointMapper->SetInputConnection( jCylinder->GetOutputPort() );
        jointMapper->ImmediateModeRenderingOn();


        joint = vtkActor::New();
        CMN_ASSERT(joint);
        joint->SetMapper( jointMapper);
        //joint1 -> SetStipplePattern(2);
        joint->SetScale(SCALE);

        joint->RotateX(90);
        this->AddPart(this->joint);
        return true;
    }
    
    inline bool UpdateVTKObjects(void) {
        return false;
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

    //create main list for the behavior
    this->VisibleList = new ui3VisibleList("LUS Main");

    //create subordinate lists
    this->ProbeList = new ui3VisibleList("Probe");
    this->ProbeListJoint1 = new ui3VisibleList("ProbeJoint1");
    this->ProbeListJoint2 = new ui3VisibleList("ProbeJoint2");
    this->ProbeListJoint3 = new ui3VisibleList("ProbeJoint3");
    this->ProbeListShaft = new ui3VisibleList("ProbeShaft");
    this->BackgroundList = new ui3VisibleList("Background");
    this->TextList = new ui3VisibleList("Text");
    this->MarkerList = new ui3VisibleList("Marker");
    
    //add sub list to behavior list
    this->VisibleList->Add(this->ProbeList);
    this->VisibleList->Add(this->BackgroundList);
    this->VisibleList->Add(this->TextList);
    this->VisibleList->Add(this->MarkerList);

    //create visible objects
    this->ProbeHead = new BehaviorLUSProbeHead(this->Position, "ProbeHeadSTL");
    this->ProbeJoint1 = new BehaviorLUSProbeJoint(this->Position);
    this->ProbeJoint2 = new BehaviorLUSProbeJoint(this->Position);
    this->ProbeJoint3 = new BehaviorLUSProbeJoint(this->Position);
    this->ProbeShaft = new BehaviorLUSProbeShaft(this->Position);
    this->Backgrounds = new BehaviorLUSBackground(this->Position);
    this->Outline = new BehaviorLUSOutline(this->Position);
    this->WarningText = new BehaviorLUSText(this->Position);
    this->MeasureText = new BehaviorLUSText(this->Position);
    this->MapCursor = new BehaviorLUSMarker("MapCursor");

    //add them to thier lists
    this->ProbeList->Add(this->ProbeHead);
    this->ProbeListJoint1->Add(this->ProbeJoint1);
    this->ProbeListJoint2->Add(this->ProbeJoint2);
    this->ProbeListJoint3->Add(this->ProbeJoint3);
    this->ProbeListShaft->Add(this->ProbeShaft);
    this->BackgroundList->Add(this->Backgrounds);
    this->BackgroundList->Add(this->Outline);
    this->TextList->Add(this->WarningText);
    this->TextList->Add(this->MeasureText);

    this->ProbeList->Add(ProbeListJoint1);
    this->ProbeListJoint1->Add(ProbeListJoint2);
    this->ProbeListJoint2->Add(ProbeListJoint3);
    this->ProbeListJoint3->Add(ProbeListShaft);

    this->VisibleList->Add(MapCursor);

    //create map cursors NOTE: this is a problem
    for(int i = 0; i<20 ; i++)
    {

        MyMarkers[i] = new BehaviorLUSMarker("marker");
        this->MarkerList->Add(MyMarkers[i]);
    }

    this->MarkerList->Hide();

    this->Offset.SetAll(0.0);

    ClutchPressed = false;
    MarkerDropped = false;
    Following=false;
    MapEnabled=false;
    ECMRCMtoVTKscale = 1;

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
//create the buttons that will be used in the behavior, each button has a callback function
    this->MenuBar->AddClickButton("FirstButton",
                                  1,
                                  "empty.png",
                                  &BehaviorLUS::FirstButtonCallback,
                                  this);

    this->MenuBar->AddClickButton("EnableMapButton",
                                 2,
                                 "map.png",
                                 &BehaviorLUS::EnableMapButtonCallback,
                                 this);
    this->MenuBar->AddClickButton("ReSetMapButton",
                                  3,
                                  "undo.png",
                                  &BehaviorLUS::ReSetMapButtonCallback,
                                  this);

}


/*!

Configures some of the objects within the scene.  This function only needs to be called once.

 */


void BehaviorLUS::Startup(void)
{
    this->Slave1 = this->Manager->GetSlaveArm("Slave1");
    this->ECM1 = this->Manager->GetSlaveArm("ECM1");

    // To get the joint values, we need to access the device directly
    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
    CMN_ASSERT(componentManager);
    mtsComponent * daVinci = componentManager->GetComponent("daVinci");
    CMN_ASSERT(daVinci);
    // get PSM1 interface
    mtsInterfaceProvided * interfaceProvided = daVinci->GetInterfaceProvided("PSM1");
    CMN_ASSERT(interfaceProvided);
    mtsCommandRead * command = interfaceProvided->GetCommandRead("GetPositionJoint");
    CMN_ASSERT(command);
    GetJointPositionSlave.Bind(command);
    command = interfaceProvided->GetCommandRead("measured_cp");
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
    mtsCommandWrite<BehaviorLUS, prmEventButton> * clutchCallbackCommand =
        new mtsCommandWrite<BehaviorLUS, prmEventButton>(&BehaviorLUS::MasterClutchPedalCallback, this,
                                                         "MasterClutchPedalCallback", prmEventButton());
    CMN_ASSERT(clutchCallbackCommand);
    interfaceProvided->AddObserver("Button", clutchCallbackCommand);
    
    //get camera control interface
    interfaceProvided = daVinci->GetInterfaceProvided("Camera");
    CMN_ASSERT(interfaceProvided);
    mtsCommandWrite<BehaviorLUS, prmEventButton> * cameraCallbackCommand =
        new mtsCommandWrite<BehaviorLUS, prmEventButton>(&BehaviorLUS::CameraControlPedalCallback, this,
                                                         "CameraControlPedalCallback", prmEventButton());
    CMN_ASSERT(cameraCallbackCommand);
    interfaceProvided->AddObserver("Button", cameraCallbackCommand);


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
    this->CursorOffset.SetAll(0.0);

    MarkerCount = 0;
    
    //=====================================
    //repeated below
    



}

/*!

not really sure what this is supposed to do

 */

void BehaviorLUS::Cleanup(void)
{
    // menu bar will release itself upon destruction
}
/*!

Preform when in MaM mode and behavior is active

*/

bool BehaviorLUS::RunForeground()
{
    this->Ticker++;

    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->VisibleList->Hide();

    }

    // detect transition, should that be handled as an event?
    // State is used by multiple threads ...
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->VisibleList->Hide();
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
    this->ProbeList->SetTransformation(this->Slave1Position.Position());
    this->GetJointPositionSlave(this->JointsSlave);
    this->GetJointPositionECM(this->JointsECM);
    this->SetUpScene();
    this->SetJoints(JointsSlave.Position().Element(4),JointsSlave.Position().Element(5),JointsSlave.Position().Element(2),JointsSlave.Position().Element(3));


    return true;
}

/*!

Preform when in MaM mode and behavior is not active

 */

bool BehaviorLUS::RunBackground()
{
    this->Ticker++;

    // detect transition
    if (this->State != this->PreviousState) {
        this->PreviousState = this->State;
        this->VisibleList->Hide();
    }

    this->Slave1->GetCartesianPosition(this->Slave1Position);
    this->ECM1->GetCartesianPosition(this->ECM1Position);
    this->Slave1Position.Position().Translation().Add(this->Offset);
    this->ProbeList->SetTransformation(this->Slave1Position.Position());
    this->GetJointPositionSlave(this->JointsSlave);
    this->SetUpScene();
    this->SetJoints(JointsSlave.Position().Element(4),JointsSlave.Position().Element(5),JointsSlave.Position().Element(2),JointsSlave.Position().Element(3));

    return true;
}

/*!

Preform when not in MaM and the behavior is active

 */

bool BehaviorLUS::RunNoInput()
{
    this->Ticker++;
    if (this->Manager->MastersAsMice() != this->PreviousMaM) {
        this->PreviousMaM = this->Manager->MastersAsMice();
        this->VisibleList->Show();
        this->ProbeList->Show();
        this->SetUpScene();
    }


    this->GetJointPositionSlave(this->JointsSlave);
    this->GetJointPositionECM(this->JointsECM);

    // .Positions() returns object of type vctDynamicVector of doubles
    // for translations you might have a meter to mm conversion to do

    this->Slave1->GetCartesianPosition(this->Slave1Position);
    this->ECM1->GetCartesianPosition(this->ECM1Position);

    this->SetJoints(JointsSlave.Position()[4],JointsSlave.Position()[5],JointsSlave.Position()[2],JointsSlave.Position()[3]);

    //measurement tool should be updated if right MTM is closed
    if (!RightMTMOpen && !ClutchPressed)
    {
        this-> GetMeasurement();
    }
    else {
        MeasurementActive = false;
        //can either leave the last measurement on the screen or let it dispear
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

    //update the map if enabled other wise it should be hidden.
    if(MapEnabled)
    {

        MapCursor->Show();
        Backgrounds->Show();
        MarkerList->Show();

        UpdateCursorPosition();

        //this is true when the slave position is being updated 
        if(CameraPressed ||
           (!ClutchPressed && (PreviousSlavePosition == Slave1Position.Position().Translation())))
        {
            if(this->MapCursor->Visible())
                {
                    //if the cursor is visible then hide
                    this->MapCursor->Hide();
                }
            //Update the visible map position when the camera is clutched
            this->UpdateVisibleMap();
        }else{
            if(!this->MapCursor->Visible())
            {
                this->MapCursor->Show();
            }
        }
    }
    else {
        //hide the map if it is not enabled
        Backgrounds->Hide();
        MarkerList->Hide();
        MapCursor->Hide();
    }
    PreviousSlavePosition=Slave1Position.Position().Translation();
    return true;
}

/*!

Places certian objects in the correct posisiton is the scene

 */

void BehaviorLUS::SetUpScene(void)
{
   //Set the position of the probe in the scene space
    vctFrm3 tmp;
    tmp.Rotation() = vctMatRot3(this->Slave1Position.Position().Rotation()) * vctMatRot3(vctAxAnRot3(vctDouble3(0.0,0.0,1.0), cmnPI_4 ));
    tmp.Translation() = vctDouble3(25.0,-45.0,-220.0); // x, y , z
    this->ProbeList->WaitForCreation();
    this->ProbeList->SetTransformation(tmp);

    
    //rotate the image plane such that it lines up with the probe
    vctDouble3 Xaxis;
    Xaxis.Assign(1.0,0.0,0.0);
    vctDouble3 Yaxis;
    Yaxis.Assign(0.0,1.0,0.0);
    vctAxAnRot3 imageRot(Yaxis, cmnPI_2);
    vctFrm3 planePosition;
    planePosition.Rotation() = vctMatRot3(imageRot);
    planePosition.Translation() = vctDouble3(0.0, 0.0, 16.0); //===============================================================================
    ImagePlane->WaitForCreation();
    ImagePlane->SetTransformation(planePosition);

    //Set the position of the backgrounds and text in scene space
    this->Outline->WaitForCreation();
    this->Outline->SetPosition(vctDouble3(8.0,-60.0,-220.0));// x, y, z
    this->Backgrounds->WaitForCreation();
    this->Backgrounds->SetPosition(vctDouble3(55.0,-48.0,-220.0)); //y,x,z
    this->TextList->WaitForCreation();
    this->TextList->SetPosition(vctDouble3(-25.0,-65.0,-220.0));

    //Set the position and color of the measurement text object
    this->MeasureText->WaitForCreation();
    this->MeasureText->SetColor(0./255, 34./255, 102.0/255);
    this->MeasureText->SetPosition(vctDouble3(0.0, 5, 0.0));
    
    this->MapCursor->WaitForCreation();
    this->MapCursor->SetColor(159.0/255, 182.0/255, 205.0/255);

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

Function callback for the first button, currently this button does nothing

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
    if (this->MapEnabled == true)
    {
        this->MapEnabled = false;
    }
    else{
        this->MapEnabled = true;
    }
    
    CMN_LOG_CLASS_RUN_DEBUG << "Behavior \"" << this->GetName() << "\" Enable map Button pressed" << std::endl;
    CMN_LOG_CLASS_RUN_DEBUG << "Behavior \"" << this->GetName() << "\" Enable map " << this->MapEnabled << std::endl;
}
/*!

Function Callback to reset the map.  Returns the map to the original starting conditons

*/

void BehaviorLUS::ReSetMapButtonCallback()
{
    Markers.clear();
    for(int i = 0; i<20; i++)
    {
        MyMarkers[i]->Hide();
    }
    CMN_LOG_CLASS_RUN_DEBUG << "Behavior \"" << this->GetName() << "\" ReSetMap Button has been pressed" << std::endl;
    UpdateVisibleMap();
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
    } else {
        this->ClutchPressed = false;
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

void BehaviorLUS::OnStreamSample(svlSample * sample, int streamindex)
{
    if (State == Foreground) {
        ImagePlane->SetImage(dynamic_cast<svlSampleImage *>(sample), streamindex);
    }
}


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

    //Set the position of the probe in the scene space
    vctFrm3 tmp;
    tmp.Rotation() = vctMatRot3(this->Slave1Position.Position().Rotation()) * vctMatRot3(vctAxAnRot3(vctDouble3(0.0,0.0,1.0), cmnPI_4 ));
    tmp.Translation() = vctDouble3(25.0,-45.0,-220.0); // x, y , z
    this->ProbeList->WaitForCreation();
    this->ProbeList->SetTransformation(tmp);

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
    vctFrm3 correctionFrame;

    //returns the center point of the probe
    correctionFrame = GetCurrentCursorPositionWRTECM();

    if (!MeasurementActive)
    {
        MeasurementActive = true;
        //saves the first point
        MeasurePoint1.Assign(correctionFrame.Translation());
        std::cout<< "MeasurePoint1 from if statement: " << MeasurePoint1<< std::endl;
    } else {
        //calculates the distance maoved
        vctDouble3 diff;
        diff.DifferenceOf(MeasurePoint1, correctionFrame.Translation());

        //finds the absolute distance moved
        double AbsVal = diff.Norm();

        //displays the distance in mm 
        sprintf(measure_string,"%4.1fmm", AbsVal);
        this->SetText(MeasureText, measure_string);
    }
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
    //create axes for later use
    vctDouble3 Xaxis;
    Xaxis.Assign(1.0,0.0,0.0);
    vctDouble3 Yaxis;
    Yaxis.Assign(0.0,1.0,0.0);
    vctDouble3 Zaxis;
    Zaxis.Assign(0.0,0.0,1.0);

    // get joint values for ECM
    this->GetJointPositionECM(this->JointsECM);
    // [0] = outer yaw
    // [1] = outer pitch
    // [2] = scope insertion
    // [3] = scope roll

    double yaw0 = JointsECM.Position()[0];
    double pitch1 = JointsECM.Position()[1];
    double insert2 = JointsECM.Position()[2]*1000.0;//convert to mm
    double roll3 = JointsECM.Position()[3];
 
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
 
    // raw cartesian position from slave daVinci, no ui3 correction
    prmPositionCartesianGet slavePosition;
    GetCartesianPositionSlave(slavePosition);

    //move the control point to center of probe NOTE: could use function.
    vctFrm3 ProbeOffset;
    ProbeOffset.Translation().Assign(vctDouble3(0.0, 0.0, 30.0));



    vctFrm3 finalFrame;

    //undo camera movement to get in EMC RCM frame
    ECMtoECMRCM = yawFrame0 * pitchFrame1 * insertFrame2 * rollFrame3;

    //find position of the slave in this new frame
    finalFrame = ECMtoECMRCM * slavePosition.Position()* ProbeOffset;

    return finalFrame;
}

/*!
updates the map cursor position, by converting the absolute frame into to the vtk frame

*/ 

void BehaviorLUS::UpdateCursorPosition(void)
{
    //get 'absolute' cordinates of the slave
    vctFrm3 finalFrame;
    finalFrame = GetCurrentCursorPositionWRTECMRCM();

    //convert absolute frame to a display frame in VTK
    //ECMRCMtoVTK is updated in function UpdateVisibleMap
    vctFrm3 cursorVTK;
    ECMRCMtoVTK.ApplyTo(finalFrame, cursorVTK);// cursorVTK = ECMRCMtoVTK * finalframe

    //make sure the cursor is displayed correctly with repect to the centering of the map and other markers
    //CenterRotatedTranslated and ECMRCMtoVTKscale are updated in function UpdateVisibleMap
    vctDouble3 t1;
    t1 = (cursorVTK.Translation() - CenterRotatedTranslated) * ECMRCMtoVTKscale;
    t1 += CenterRotatedTranslated;
    cursorVTK.Translation().Assign(t1);
    MapCursor->SetTransformation(cursorVTK);
}

/*!

// this method should be called after:
// -1- a marker is added (not required with removed)
// -1- the camera is clutch is release and the teleoperation resumes


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

        //create rotation into VTK frame for display
        vctFrm3 ECMtoVTK;
        ECMtoVTK.Rotation().From( vctAxAnRot3(vctDouble3(0.0,1.0,0.0), cmnPI) );

        //inverse kinimatics of the camera
        vctFrm3 ECMtoECMRCMInverse = ECMtoECMRCM.Inverse();

        //rotate entire bounding box into VTK frame
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
        //find scale to keep markers in the specified background
        double h,w, ratioH, ratioW;
        h = corner2Rotated.Y() - corner1Rotated.Y();
        w = corner2Rotated.X() - corner1Rotated.X();
        ratioH = h/(25.0/1.25);
        ratioW = w/(30.0/1.25);
        if(ratioH > ratioW)
        {
            ratioW = ratioH;//ratioW is now biggest
        }
        if(ratioW >1)
        {
            //scale factor
            ECMRCMtoVTKscale = 1.0/ratioW;
        }
        ECMRCMtoVTKscale = 0.14; // NOTE: hard coded for now for a known workspace

        // computer the transformation to be applied to all absolute coordinates
        // to be display in the SAW coordinate system
        vctDouble3 centerInSAW(this->Backgrounds->GetVTKProp()->GetCenter());// center of the map background
        vctDouble3 offsetUp;
        offsetUp.X() = centerInSAW.X() - centerRotated.X();
        offsetUp.Y() = centerInSAW.Y() - centerRotated.Y();
        offsetUp.Z() = centerInSAW.Z() - corner1Rotated.Z() + 4.0;
        offsetUp.Z() -= ((corner2Rotated.Z() - corner1Rotated.Z()) / 2.0) * (1.0 - ECMRCMtoVTKscale); 

        vctFrm3 VTKrecenter;

        VTKrecenter.Translation().Assign(offsetUp);
        ECMRCMtoVTK =VTKrecenter * ECMtoVTK * ECMtoECMRCMInverse;

        CenterRotatedTranslated = VTKrecenter * centerRotated;
    }

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
    }
#endif
}


