/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3Manager.h,v 1.13 2009/02/24 14:58:26 anton Exp $

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

#ifndef _ui3Manager_h
#define _ui3Manager_h

#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3BehaviorBase.h>
#include <cisst3DUserInterface/ui3SceneManager.h>
#include <cisst3DUserInterface/ui3VTKRenderer.h>
#include <cisst3DUserInterface/ui3VideoSourceBase.h>
#include <cisst3DUserInterface/ui3Cursor.h>

/*!
 Provides a default implementation for the main user interface manager.
*/
class ui3Manager: public ui3BehaviorBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
    
    friend class ui3BehaviorBase;
public:
    /*!
     Typedef for list of behaviors
    */
    typedef std::list<ui3BehaviorBase*> BehaviorList;

    /*!
     Enumerated display modes
    */
    enum DisplayMode {
        Mono,
        StereoDualScreen,
        StereoVerticalSplit,
        StereoHorizontalSplit,
        StereoWindowed
    };

    /*!
     Constructor
    */
    ui3Manager(const std::string & name = "ui3Manager");

    /*!
     Destructor
    */
    virtual ~ui3Manager();

    /*!
     Configures video input devices that capture the live camera images from
     the optional cameras and loads the camera calibration file(s).
     If there is no live video stream to be rendered in the background of the
     scene, the call of this function should be omitted.
     
     \param calibfile       Camera calibration file path
     \return                Success flag: true=success, false=error
    */
    // more parameters needed for setting up the input device
    virtual bool SetupVideoSource(const std::string& calibfile);

    /*!
     Configures the connection between the UI manager and the input device(s) .
     This call can be omitted if no input devices are used.

     \param configfile      Input device configuration file
     \return                Success flag: true=success, false=error
    */
    virtual bool SetupRightMaster(mtsDevice * device, const std::string & providedInterface,
                                  vctFrm3 & transformation, double scale = 1.0);

    virtual bool SetupRightMasterButton(mtsDevice * device, const std::string & providedInterface);

    virtual bool SetupLeftMaster(mtsDevice * device, const std::string & providedInterface,
                                 vctFrm3 & transformation, double scale = 1.0);

    virtual bool SetupLeftMasterButton(mtsDevice * device, const std::string & providedInterface);

    /*!
     Configures the image display.

     \param width           Display window width
     \param height          Display window height
     \param mode            Display mode: mono mode or one of the stereo modes
     \return                Success flag: true=success, false=error
    */
    virtual bool SetupDisplay(unsigned int width, unsigned int height, DisplayMode mode);

    /*! Returns a pointer to the main user interface manager object,
     i.e. this object.
     \return Pointer to the main user interface manager
    */
    virtual ui3Manager * GetUIManager(void);

    /*!
     Returns a pointer to the 3D scene manager object.
     \return Pointer to the 3D scene manager
    */
    virtual ui3SceneManager * GetSceneManager(void);

    /*!
     Loads the parameters of the user interface from the specified file and calls
     the ui3BehaviorBase::LoadConfiguration member of each registered behavior.

     \param configfile      Configuration file
     \return                Success flag: true=success, false=error
    */
    virtual void Configure(const std::string & configFile);

    /*!
     Saves the parameters of the user interface to the specified file and calls
     the ui3BehaviorBase::SaveConfiguration members of each registered behavior.

     \param configfile      Configuration file
     \return                Success flag: true=success, false=error
    */
    virtual bool SaveConfiguration(const std::string & configFile) const;

    /*!
     Returns a flag signaling whether the application is in
     interface/interaction/input/master-as-mouse mode.
     
     \return                User interface/interaction/input/master-as-mouse mode flag
    */
    bool IsInUIMode(void) const;

    /*!
     Returns the current position of the pointer.

     \param inputid         Input device identifier
     \return                3D Cartesian coordinates of the pointer in camera frame
    */
    vct3 GetPointerPosition(unsigned int inputId) const;

    /*!
     Returns the current position of the cursor.

     \note
     The cursor is usually rendered at the same position as the pointer, however
     the UI manager or the behavior are allowed to remap cursor rendering coordinates
     for special scenarios.
     For the sake of simplicity, when in tele-operated mode, theis function returns
     the pointer positions although the cursor will not be rendered by default.

     \param inputid         Input device identifier
     \return                3D Cartesian coordinates of the cursor in camera frame
    */
    vct3 GetCursorPosition(unsigned int inputId) const;

    /*!
     Adds a behavior to the behavior list. The corresponding icon file has to
     contain images for all behavior states on a single bitmap.
     The method also sets the ui3BehaviorBase::Manager and the
     ui3BehaviorBase::SceneManager members of the added behavior.

     \param behavior        Reference to the behavior object
     \param description     Textual description, tool tip
     \param position        Defines the position of the behavior on the menu bar.
                            If positive, the behavior will be inserted to the specified
                            position on the menu bar.
                            If -1, the behavior will be added to the end of the menu bar.
     \param iconfile        Image file storing all the behavior states on a singe bitmap.
     \return                Unique handle assigned to the behavior
    */
    virtual ui3Handle AddBehavior(ui3BehaviorBase * behavior,
                                  unsigned int position,
                                  const std::string & iconfile);

    /*!
     Initializes all registered behaviors, starts the user interface thread,
     and executes the main loop.
     It returns on error or after the main loop has successfully been started.
     The running user interface loop can be terminated by calling the
     the ui3Manager::Stop method.

     \return                Success flag: true=success, false=error
    */
    virtual void Startup(void);

    /*!
     Terminates the running user interface main loop started earlier by calling the
     ui3Manager::Start method. It first stops the executing thread then releases
     the registered behaviors.
    */
    // virtual void Stop(void);

    /*!
     Called by the main user interface thread. Should not be called directly by
     the application.
     This method releases all resources allocated by the ui3Manager::Initialize
     function and it calls the ui3BehaviorBase::Initialize function
     of each registered behavior.
    */
    virtual void Cleanup(void);

    virtual void Run(void);

    /*!
     Called by the main user interface thread when the user interface manager is
     in master-as-mouse mode and the main menu for selecting behaviors is active
     (so that no behaviors are in foreground).
     Should not be called directly by the application.
     This method performs user interface related actions for each rendered
     frame and it calls the ui3BehaviorBase::RunForeground function of each
     registered behavior.

     \return                Success flag: true=success, false=error
    */
    virtual bool RunForeground(void);

    /*!
     Called by the main user interface thread when the user interface manager is
     in master-as-mouse mode and one of the behaviors is in foreground.
     Should not be called directly by the application.
     This method performs user interface related actions for each rendered
     frame and it calls the ui3BehaviorBase::Run function of each registered
     behavior.

     \return                Success flag: true=success, false=error
    */
    virtual bool RunBackground(void);

    /*!
     Called by the main user interface thread when the user interface manager is
     not in master-as-mouse mode (is in tele-operated mode). In this state no user
     inputs will be dispatched to the behavior.
     Should not be called directly by the application.
     This method performs user interface related actions for each rendered
     frame and it calls the ui3BehaviorBase::Run function of each registered
     behavior.

     \return                Success flag: true=success, false=error
    */
    virtual bool RunNoInput(void);


private:

    inline ui3VisibleObject * GetVisibleObject(void) {
        CMN_LOG_CLASS(5) << "GetVisibleObject: this method should never be called" << std::endl;
        return 0;
    }

    inline void ConfigureMenuBar(void) {
        CMN_LOG_CLASS(5) << "ConfigureMenuBar: this method should never be called" << std::endl;
    }

    void SetActiveBehavior(ui3BehaviorBase * newBehavior);

    /*!
     Flag signaling whether the application is in user
     interface/interaction/input/master-as-mouse mode.
    */
    bool UIMode;

    /*!
     Flag signalling whether the user interface loop has been successfully initialized.
    */
    bool Initialized;

    /*!
     Flag signalling whether the user interface loop is running.
    */
    bool Running;

    ui3BehaviorBase * ActiveBehavior;

    /*!
     Linked list of behaviors.
    */
    BehaviorList Behaviors;

    /*!
     Scene manager object that maintains the consistency and thread safety of 3D scene.
    */
    ui3SceneManager * SceneManager;
    
    /*!
     Input device interface module. (???)
    */
    ui3InputDeviceBase InputDevice;

    /*!
     VTK 3D graphics renderer module. (???)
    */
    ui3VTKRenderer * Renderer;

    /*!
     Video capture module. (???)
    */
    ui3VideoSourceBase VideoSource;

    /*! Keep a pointer on singleton task manager to make it easier to access */
    mtsTaskManager * TaskManager;

    // functions which will be bound to commands
    mtsFunctionRead RightMasterGetCartesianPosition;
    mtsFunctionRead LeftMasterGetCartesianPosition;
    
    // event handlers
    void RightMasterButtonEventHandler(const prmEventButton & buttonEvent);
    void LeftMasterButtonEventHandler(const prmEventButton & buttonEvent);

    // cursors
    ui3Cursor * RightCursor;
    ui3Cursor * LeftCursor;

    // button state, might be a better implementation for this (Anton)
    bool RightButtonPressed, RightButtonReleased;
    bool LeftButtonPressed, LeftButtonReleased;

    // transformation between inputs and scen
    vctFrm3 RightTransform;
    vctFrm3 LeftTransform;

    // scale
    double RightScale;
    double LeftScale;
};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3Manager)


#endif // _ui3Manager_h
