/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3BehaviorBase.h,v 1.15 2009/02/24 02:43:13 anton Exp $

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

#ifndef _ui3BehaviorBase_h
#define _ui3BehaviorBase_h

#include <cisstMultiTask/mtsTaskContinuous.h>
#include <cisstMultiTask/mtsFunctionReadOrWrite.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstParameterTypes/prmEventButton.h>


#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3InputDeviceBase.h>
#include <cisst3DUserInterface/ui3MenuBar.h>

/*!
 Provides a default behavior and common interface for derived behavior classes.
*/
class ui3BehaviorBase: public mtsTaskContinuous
{

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);

    friend class ui3Manager;

private:
    void Run(void);

public:
    /*!
      Typedef for pointer to member function for button callbacks
    */
    typedef void(*ControlCallbackType)(void);

    /*!
      Enumerated behavior states
    */
    typedef enum {
        Foreground,
        Background,
        Idle
    } StateType;
    
public:
    /*!
     Constructor: called when instantiating behaviors
    */
    ui3BehaviorBase(const std::string & name,
                    ui3Manager * manager);

    /*!
     Destructor
    */
    virtual ~ui3BehaviorBase();

    /*!
     Returns the user interface state of the behavior.

     \return                Behavior state
    */
    StateType GetState(void) const;

    /*!
     Returns a pointer to the main user interface manager object.

     \return                Pointer to the main user interface manager
    */
    virtual ui3Manager * GetManager(void);

    /*!
     Returns a pointer to the 3D scene manager object.

     \return                Pointer to the 3D scene manager
    */
    virtual ui3SceneManager * GetSceneManager(void);

    /*!
     This method shall perform all the necessary resource initialization and memory
     allocation that is to be done before running the behavior.
     Menu bar buttons should be created in this method.
     The resources initialized and memory allocated in this function shall be released
     in the ui3BehaviorBase::Release method.

     \return                Success flag: true=success, false=error
    */
    virtual void Startup(void) = 0;

    /*!
     This method shall release all resources initialized and memory allocated in the
     ui3BehaviorBase::Initialize method.
    */
    virtual void Cleanup(void) = 0;

    /*!
     This method is called once for every video frame when the user interface
     manager is in master-as-mouse mode and the behavior is the active behavior
     (is in foreground).
     It shall perform interface related actions in the behavior.
     \note
     This method should handle only user interface related actions that do not block
     the calling thread for a long period of time. Implement all computationally
     intensive tasks on an asynchronous thread/task instead.

     \return                Success flag: true=success, false=error
    */
    virtual bool RunForeground(void) = 0;

    /*!
     This method is called once for every video frame when the user interface
     manager is in master-as-mouse mode and the behavior is not active (is in
     background).
     It shall perform interface related actions in the behavior.
     \note
     This method should handle only user interface related actions that do not block
     the calling thread for a long period of time. Implement all computationally
     intensive tasks on an asynchronous thread/task instead.

     \return                Success flag: true=success, false=error
    */
    virtual bool RunBackground(void) = 0;

    /*!
     This method is called once for every video frame when the user interface
     manager is not in master-as-mouse mode (is in tele-operated mode). In this
     state no user inputs will be dispatched to the behavior.
     \note
     This method should handle only user interface related actions that do not block
     the calling thread for a long period of time. Implement all computationally
     intensive tasks on an asynchronous thread/task instead.

     \return                Success flag: true=success, false=error
    */
    virtual bool RunNoInput(void) = 0;

    /*!
     Tis method shall oad the parameters of the behavior from the specified file.
     It is called automatically by ui3Manager::LoadConfiguration.

     \param configfile      Configuration file
     \return                Success flag: true=success, false=error
    */
    virtual void Configure(const std::string & configFile);

    /*!
     This method shall save the parameters of the behavior to the specified file.
     It is called automatically by ui3Manager::SaveConfiguration.

     \param configfile      Configuration file
     \return                Success flag: true=success, false=error
    */
    virtual bool SaveConfiguration(const std::string & configFile);


    virtual ui3VisibleObject * GetVisibleObject(void) = 0;

protected:
    /*!
     Stores user interface buttons.
    */
    ui3MenuBar * MenuBar;
    /*!
     Maps control handles to callback methods.
    */
    std::map<ui3Handle, ControlCallbackType> CallbackMap;

    /*!
     Assigns a callback method for the specified control handle.

     \param ctrlhandle          Handle of the control (button) on the menu bar
     \param method              Specifies the vertical screen coordinate
    */
    virtual void RegisterCallback(ui3Handle ctrlhandle, ControlCallbackType method);

    /*!
     Removes a callback method association for the specified control handle.

     \param ctrlhandle          Handle of the control (button) on the menu bar
    */
    virtual void UnregisterCallback(ui3Handle ctrlhandle);

    /*!
     Enables input action callbacks or the specified input device.

     \param inputid             Input device identifier
    */
    virtual void SubscribeInputCallback(unsigned int inputid);

    /*!
     Disables input action callbacks for the specified input device.

     \param inputid             Input device identifier
    */
    virtual void UnsubscribeInputCallback(unsigned int inputid);

    /*!
     This method is called automatically by ui3BehaviorBase::DispatchGUIEvents
     if any input actions occur that the application previously subscribed for
     using the ui3BehaviorBase::SubscribeInputCallback method.

     \param inputid             Input device identifier
     \param action              Action identifier
    */
    virtual void OnInputAction(unsigned int inputid, ui3InputDeviceBase::InputAction action);

    /*!
     Automatically interprets relevant UI events and calls registered callback
     methods if needed.
    */
    virtual void DispatchGUIEvents(void);

    /*! Method called when this behavior becomes active, i.e. the user selected it from the previous menu */
    void SetStateForeground(void);
    void SetStateBackground(void);

protected:

    void AddMenuBar(bool isManager);

    void SetState(const StateType & newState);

    /*!
     State variable for storing the current UI state of the behavior.
    */
    StateType State;
    /*!
     Pointer to the main user interface manager object.
     This member is set by the UI manager upon creating the behavior.
    */
    ui3Manager * Manager;
};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3BehaviorBase);


#endif // _ui3BehaviorBase_h
