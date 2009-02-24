/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: example1.cpp,v 1.13 2009/02/24 14:58:26 anton Exp $

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

#include "example1.h"

#define RIGHT_ARM       0
#define LEFT_ARM        1

/**********************************************************/
/* main() application entry point                         */
/**********************************************************/

int main()
{
    // log configuration
    cmnLogger::SetLoD(10);
	cmnLogger::GetMultiplexer()->AddChannel(std::cout, 10);
    // add a log per thread
    osaThreadedLogFile threadedLog("example1-");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, 10);
    // specify a higher, more verbose log level for these classes
    cmnClassRegister::SetLoD("ui3BehaviorBase", 10);
    cmnClassRegister::SetLoD("ui3Manager", 10);
    cmnClassRegister::SetLoD("mtsTaskInterface", 10);
    cmnClassRegister::SetLoD("mtsTaskManager", 10);

    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    devSensableHD * sensable = new devSensableHD("Omni", "Omni1", "Omni2" /* name in driver, see Preferences in Sensable Driver */);
    taskManager->AddTask(sensable);

    ui3Manager guiManager;

    CExampleBehavior behavior("Example1");
    CExampleBehavior behavior2("Example2");

    guiManager.AddBehavior(&behavior,       // behavior reference
                           0,             // position in the menu bar: default
                           "dvLUS_icon_128.bmp");            // icon file: no texture

    guiManager.AddBehavior(&behavior2,       // behavior reference
                           2,             // position in the menu bar: default
                           "dvViewer_icon_128.bmp");            // icon file: no texture

    guiManager.Configure("config.xml");
    // setup main user interface
    guiManager.SetupVideoSource("camera_calib.txt");
    vctFrm3 transform;
    transform.Translation().Assign(+30.0, 0.0, -150.0); // recenter Omni's depth
    guiManager.SetupRightMaster(sensable, "Omni1", transform, 0.5 /* scale factor */);
    guiManager.SetupRightMasterButton(sensable, "Omni1Button1"); 
    transform.Translation().Assign(-30.0, 0.0, -150.0); // recenter Omni's depth
    guiManager.SetupLeftMaster(sensable, "Omni2", transform, 0.5 /* scale factor */);
    guiManager.SetupLeftMasterButton(sensable, "Omni2Button1"); 

    guiManager.SetupDisplay(640, 480, ui3Manager::StereoWindowed);
    // guiManager.SetFrequency(30.0 / 1.001);
    
    // setup behavior

    // TODO something like:
    //      behavior.FooSetup()

    // following should be replaced by a utility function or method of ui3Manager 
    taskManager->CreateAll();
    taskManager->StartAll();
    // replace by exit condition created by ui3Manager
    osaSleep(100.0 * cmn_s);
    taskManager->KillAll();

    guiManager.SaveConfiguration("config.xml");
    return 0;
}


/**********************************************************/
/* CExampleBehavior class implementation                  */
/**********************************************************/

CExampleBehavior::CExampleBehavior(const std::string & name):
    ui3BehaviorBase(std::string("CExampleBehavior:") + name, 0)
{
}

CExampleBehavior::~CExampleBehavior()
{
}

void CExampleBehavior::ConfigureMenuBar()
{
    this->MenuBar->AddClickButton("FirstButton",
                                  1,
                                  "",
                                  &CExampleBehavior::FirstButtonCallback,
                                  this);
}

void CExampleBehavior::FirstButtonCallback()
{
    std::cerr << "Behavior \"" << this->GetName() << "\" Button 1 pressed" << std::endl;
}

void CExampleBehavior::Startup(void)
{
    // Creating buttons on menu bar
    /*hButton1 = MenuBar.AddClickButton("Button 1", -1, "");
    hButton2 = MenuBar.AddClickButton("Button 2", -1, "");
    hSpacer  = MenuBar.AddSpacer(-1);
    hButton3 = MenuBar.AddCheckButton("Button 3", -1, "", true);
*/
    // Assign callbacks to buttons
    // Doesn't work this way... will need to be moved to cisstMultiTask
//    RegisterCallback(hButton1, &CExampleBehavior::OnButton2);
//    RegisterCallback(hButton2, &CExampleBehavior::OnButton2);
//    RegisterCallback(hButton3, &CExampleBehavior::OnButton3);

    // Subscribe for input actions
    SubscribeInputCallback(RIGHT_ARM);

#if 0
    // Construct a new VTK actor object
    h3DModel = GetSceneManager()->CreateActor();

    // Request actor object
    vtkActor* actor = GetSceneManager()->GetActor(h3DModel);

        // Initialize actor
        // TO DO something like:
        //      actor->Load("3dmodel.vtk");

    // Release actor object
    GetSceneManager()->ReleaseActor(h3DModel);
#endif
}

void CExampleBehavior::Cleanup(void)
{
    // menu bar will release itself upon destruction
}

bool CExampleBehavior::RunForeground()
{
    // running in foreground GUI mode
    // menu bar is visible

    // automatically interpret relevant UI events
    // and call registered callback methods if needed
    DispatchGUIEvents();

    // do processing:
    vct3 ptrpos = GetManager()->GetPointerPosition(RIGHT_ARM);
#if 0
    // Request actor object
    vtkActor* actor = GetSceneManager()->GetActor(h3DModel);

        // Manipulate actor in a thread safe manner
        // TO DO something like:
        //      actor->Move(ptrpos);

    // Release actor object
    GetSceneManager()->ReleaseActor(h3DModel);
#endif
    return true;
}

bool CExampleBehavior::RunBackground()
{
    // running in background GUI mode
    // menu bar is hidden

    // do whatever processing...
    // TO DO

    return true;
}

bool CExampleBehavior::RunNoInput()
{
    // running in tele-operated mode
    // menu bar is hidden

    // do whatever processing...
    // TO DO

    return true;
}

void CExampleBehavior::Configure(const std::string & configFile)
{
    // load settings
}

bool CExampleBehavior::SaveConfiguration(const std::string & configFile)
{
    // save settings
    return true;
}

void CExampleBehavior::OnButton2()
{
    // button click event
}

void CExampleBehavior::OnButton3()
{
    // check state change
    bool ischecked = MenuBar->GetCheck(hButton3);
}

void CExampleBehavior::OnInputAction(unsigned int inputid, ui3InputDeviceBase::InputAction action)
{
    vct3 ptrpos = GetManager()->GetPointerPosition(inputid);
    vct3 curpos = GetManager()->GetCursorPosition(inputid);

    switch (action) {
        case INPUT_CLICK:
        break;

        case INPUT_DBCLICK:
        break;

        case INPUT_PRESSED:
        break;

        case INPUT_RELEASED:
        break;

        case INPUT_2NDCLICK:
        break;

        case INPUT_2NDDBCLICK:
        break;

        case INPUT_2NDPRESSED:
        break;

        case INPUT_2NDRELEASED:
        break;

        default:
        break;
    }
}

// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: example1.cpp,v $
//  Revision 1.13  2009/02/24 14:58:26  anton
//  Testing two master arms
//
//  Revision 1.12  2009/02/24 02:44:36  anton
//  Example modified to use different interfaces for arm and buttons (recently added to Omni wrappers)
//
//  Revision 1.11  2009/02/23 16:55:06  anton
//  Work in progress to support VTK scene locks.  Compiles, doesn't run.
//
//  Revision 1.10  2009/02/17 04:14:30  anton
//  New VTK framwork almost working.
//
//  Revision 1.9  2009/02/16 22:36:59  anton
//  Major update for better VTK support, doesn't work yet
//
//  Revision 1.8  2009/02/12 02:49:02  anton
//  Working menus and buttons.  Messy.
//
//  Revision 1.7  2009/02/10 15:57:17  anton
//  Removed CVS log at end of file, added scale and transformation between master arm and cursor
//
//  Revision 1.6  2009/02/05 04:51:33  anton
//  ui3: work in progress, events for buttons work, VTK doesn't crash (on Windows) and displays cursor.
//
//  Revision 1.5  2009/02/03 21:09:06  anton
//  ui3 example and lib: work in progress, now binds to devices, get button event and some preliminary VTK code
//
//  Revision 1.4  2009/02/02 23:21:42  anton
//  Work in progress, work on connecting master arm to ui3Manager
//
//  Revision 1.3  2009/02/02 21:42:49  anton
//  ui3: updated code to compile
//
//  Revision 1.2  2008/08/19 20:05:09  anton
//  cisst3DUserInterface: Port to cisstMultiTask, compiles, does not run!
//
//  Revision 1.1  2008/06/18 22:54:49  vagvoba
//  cisst3DUserInterface: example1 added (CMakeLists.txt, example1.h, example1.cpp)
//
//
// ****************************************************************************
