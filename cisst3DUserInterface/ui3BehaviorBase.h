/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on: 2008-05-23

  (C) Copyright 2008-2012 Johns Hopkins University (JHU), All Rights
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
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsStateData.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstParameterTypes/prmEventButton.h>
#include <cisstStereoVision/svlFilterBase.h>

#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3InputDeviceBase.h>
#include <cisst3DUserInterface/ui3MenuBar.h>

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

/*!
  Provides a default behavior and common interface for derived behavior classes.
*/
class CISST_EXPORT ui3BehaviorBase: public mtsTaskContinuous
{

    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class ui3Manager;
    friend class ui3MasterArm;
    friend class ui3VideoInterfaceFilter;

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
        Foreground, // running, receives GUI events
        Background, // running, doesn't receive events
        Idle // not running
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

    virtual void ConfigureMenuBar(void) = 0;

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

    /*! Method called when this behavior becomes active, i.e. the user selected it from the previous menu */
    void SetStateForeground(void);
    void SetStateBackground(void);
    void SetStateIdle(void);

    virtual void OnQuit(void) {};
    virtual void OnStart(void) {};

 public:
    /*!
      If there are any video sources connected to the behavior, this method is
      called by SVL pipelines every time a new stream sample arrives.
      Attention:
      This method is heavily multithreaded. It is used by all asynchronous
      streams attached to the behavior. It may be called from several
      independent stream threads simultaneously.
    */
    virtual void OnStreamSample(svlSample* sample, size_t streamindex);

    /*!
      Adds a new SVL source interface to the behavior and
      returns the stream index for the new interface.
    */
    size_t AddStream(svlStreamType type, const std::string & streamname);

    /*!
      Returns image width if the specified stream is of image type.
      Otherwise or if the SVL pipeline is not yet initialized,
      the return value is 0.
    */
    unsigned int GetStreamWidth(const size_t streamindex, unsigned int channel = 0);
    unsigned int GetStreamWidth(const std::string & streamname, unsigned int channel = 0);

    /*!
      Returns image height if the specified stream is of image type.
      Otherwise or if the SVL pipeline is not yet initialized,
      the return value is 0.
    */
    unsigned int GetStreamHeight(const int streamindex, unsigned int channel = 0);
    unsigned int GetStreamHeight(const std::string & streamname, unsigned int channel = 0);

    /*!
      Returns the index of the first stream named as the specified string.
      If no matching stream is found, the return value is negative.
    */
    int GetStreamIndexFromName(const std::string & streamname);

    vctDynamicVector<ui3VideoInterfaceFilter*> Streams;
    vctDynamicVector<std::string> StreamNames;

    /*!
      Returns a pointer to the filter that interfaces the behavior
      with StereoVision pipelines. Using the its pointer, the filter
      can be connected to a pipeline.
    */
    svlFilterBase * GetStreamSamplerFilter(const std::string & streamname);


    // list of ui3 widgets
    typedef std::list<ui3Widget3D *> Widget3DList;
    Widget3DList Widget3Ds;
    void AddWidget3D(ui3Widget3D * widget3D);

    // list of selectables objects, includes handles for ui3Widgets
    typedef std::list<ui3Selectable *> SelectableList;
    SelectableList Selectables;
    void AddSelectable(ui3Selectable * selectable);


    void AddMenuBar(bool isManager = false);

    void SetState(const StateType & newState);

    virtual void PrimaryMasterButtonCallback(const prmEventButton & event);
    virtual void SecondaryMasterButtonCallback(const prmEventButton & event);
    virtual void SetStateIdleCallback(void);
    virtual void SetStateForegroundCallback(void);
    virtual void SetStateBackgroundCallback(void);

    /*!
      State variable for storing the current UI state of the behavior.
    */
    StateType State;
    /*!
      Pointer to the main user interface manager object.
      This member is set by the UI manager upon creating the behavior.
    */
    ui3Manager * Manager;

    mtsFunctionRead GetPrimaryMasterPosition, GetSecondaryMasterPosition;
    mtsFunctionWrite SetPrimaryMasterPosition, SetSecondaryMasterPosition;

 private:
    /*! Event triggers, used by ui3Manager only */
    mtsFunctionWrite PrimaryMasterButtonEvent, SecondaryMasterButtonEvent;
};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3BehaviorBase);


#endif // _ui3BehaviorBase_h
