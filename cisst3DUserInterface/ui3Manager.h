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

#ifndef _ui3Manager_h
#define _ui3Manager_h

#include <cisstCommon/cmnNamedMap.h>

#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3BehaviorBase.h>
#include <cisst3DUserInterface/ui3MasterArm.h>
#include <cisstStereoVision/svlCameraGeometry.h>

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

// forward declaration, to be moved to cisstStereoVision
class svlRenderTargetBase;


class ui3ManagerCVTKRendererProc
{
public:
    ui3ManagerCVTKRendererProc(void);

    void * Proc(ui3Manager * baseref);

    osaThreadSignal ThreadReadySignal;
    bool KillThread;
    bool ThreadKilled;
};


/*!
  Provides a default implementation for the main user interface manager.
*/
class CISST_EXPORT ui3Manager: public ui3BehaviorBase
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class ui3ManagerCVTKRendererProc;
    friend class ui3BehaviorBase;
    friend class ui3MasterArm;
    friend class ui3SlaveArm;

 public:
    /*!
      Typedef for list of behaviors
    */
    typedef std::list<ui3BehaviorBase *> BehaviorList;

    typedef cmnNamedMap<ui3MasterArm> MasterArmList;

    typedef cmnNamedMap<ui3SlaveArm> SlaveArmList;

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
    //    virtual bool SetupVideoSource(const std::string& calibfile);

    /*!
      Configures the connection between the UI manager and the input device(s) .
      This call can be omitted if no input devices are used.

      \param configfile      Input device configuration file
      \return                Success flag: true=success, false=error
    */
    virtual bool SetupMaM(const std::string & mamDevice, const std::string & mamInterface);
    virtual bool SetupMaM(mtsDevice * mamDevice, const std::string & mamInterface);

    /*!
      Adds a render window to the UI Manager.
    */
    virtual bool AddRenderer(unsigned int width, unsigned int height,
                             double zoom, bool borderless, int x, int y,
                             svlCameraGeometry & camgeometry, unsigned int camid,
                             const std::string & renderername);

    /*!
      Assigns an external render target for the renderer.
    */
    virtual bool SetRenderTargetToRenderer(const std::string & renderername, svlRenderTargetBase* rendertarget);

    /*!
      Assigns a video backgrond to a render window.
    */
    virtual bool AddVideoBackgroundToRenderer(const std::string & renderername, const std::string & streamname, unsigned int videochannel = 0);

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
    bool AddBehavior(ui3BehaviorBase * behavior,
                     unsigned int position,
                     const std::string & iconfile);

    bool AddMasterArm(ui3MasterArm * arm);

    bool AddSlaveArm(ui3SlaveArm * arm);

    ui3SlaveArm * GetSlaveArm(const std::string & armName);

    ui3MasterArm * GetMasterArm(const std::string & armName);

    void ConnectAll(void);

    /*!
      Initializes all registered behaviors, starts the user interface thread,
      and executes the main loop.
      It returns on error or after the main loop has successfully been started.
      The running user interface loop can be terminated by calling the
      the ui3Manager::Stop method.
    */
    virtual void Startup(void);

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

    virtual void RequestPick(osaThreadSignal * pickSignal, vtkPropPicker * picker,
                             const std::string & rendererName, const vct3 & pickerPoint);


 protected:

    typedef struct tagRendererStruct {
        unsigned int width;
        unsigned int height;
        double zoom;
        bool borderless;
        int windowposx;
        int windowposy;
        svlCameraGeometry camgeometry;
        unsigned int camid;
        std::string name;
        ui3VTKRenderer* renderer;
        svlRenderTargetBase* rendertarget;
        int streamindex;
        unsigned int streamchannel;
        ui3ImagePlane* imageplane;
    } _RendererStruct;

 private:

    inline ui3VisibleObject * GetVisibleObject(void) {
        CMN_LOG_CLASS_RUN_ERROR << "GetVisibleObject: this method should never be called" << std::endl;
        return 0;
    }

    inline void ConfigureMenuBar(void) {
        CMN_LOG_CLASS_RUN_ERROR << "ConfigureMenuBar: this method should never be called" << std::endl;
    }

    void SetActiveBehavior(ui3BehaviorBase * newBehavior);

    void DispatchButtonEvent(const ui3MasterArm::RoleType & armRole, const prmEventButton & buttonEvent);

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
      Map of std::string, master arms
    */
    MasterArmList MasterArms;

    /*!
      Map of std::string, slave arms
    */
    SlaveArmList SlaveArms;

    /*!
      Scene manager object that maintains the consistency and thread safety of 3D scene.
    */
    ui3SceneManager * SceneManager;

    /*!
      3D graphics renderer modules.
    */
    vctDynamicVector<_RendererStruct*> Renderers;

    /*!
      3D graphics renderer procedure class.
    */
    ui3ManagerCVTKRendererProc RendererProc;

    /*!
      3D graphics renderer thread.
    */
    osaThread* RendererThread;

    /*!
      Method to create VTK scenes.
    */
    bool SetupRenderers();

    /*!
      Method to release VTK scenes.
    */
    void ReleaseRenderers();

    /*!
      Background video stream event callback.
    */
    void OnStreamSample(svlSample* sample, int streamindex);

    /*! Keep a pointer on singleton task manager to make it easier to access */
    mtsComponentManager * ComponentManager;

    // event handlers
    void MaMModeEventHandler(const prmEventButton & payload);

    void RecenterMasterCursors(const vctDouble3 & lowerCorner, const vctDouble3 & upperCorner);

    // hide/show all objects controlled by the ui3Manager
    void HideAll(void);
    void ShowAll(void);

    // MaM (MastersAsMice) mode
    bool MaM;
    bool IsOverMenu;
    bool HasMaMDevice;

    osaThreadSignal * PickSignal;
    vtkPropPicker * Picker;
    std::string PickRendererName;
    vct3 PickPosition;
    bool PickRequested;

 public:

    inline bool MastersAsMice(void) const {
        return this->MaM;
    }
    
    ui3VTKRenderer * GetRenderer(unsigned int camid);

 private:


};





CMN_DECLARE_SERVICES_INSTANTIATION(ui3Manager)


#endif // _ui3Manager_h
