/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2010-02-26

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _GCMUITask_h
#define _GCMUITask_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstMultiTask/mtsComponentViewer.h>
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstOSAbstraction/osaGetTime.h>
#include <cisstOSAbstraction/osaMutex.h>
#include "GlobalComponentManagerUI.h"
#include "multiplot_cisst.h"

#include <iostream>

#define MAX_CHANNEL_COUNT  12

// Basic UI feature test
//#define BASIC_PLOTTING_TEST

// FLTK Fl_Browser format code definition
#define FLTK_COLOR_BLACK   0
#define FLTK_COLOR_RED     1
#define FLTK_COLOR_GREEN   2
#define FLTK_COLOR_YELLOW  3
#define FLTK_COLOR_BLUE    4
#define FLTK_COLOR_MAGENTA 5
#define FLTK_COLOR_CYAN    6
#define FLTK_COLOR_WHITE   255

#define FLTK_STYLE_BOLD      'b'
#define FLTK_STYLE_ITALIC    'i'
#define FLTK_STYLE_UNDERLINE 'u'
#define FLTK_STYLE_STRIKE    '-'
#define FLTK_STYLE_LARGE     'l' /* 24pt */
#define FLTK_STYLE_MEDIUM    'm' /* 18pt */
#define FLTK_STYLE_SMALL     's' /* 11pt */
#define FLTK_STYLE_CENTER    'c' /* 11pt */

class GCMUITask: public mtsTaskPeriodic {

public:
    //typedef std::vector<std::string> StringVector;
    typedef stdStringVec StringVector;

    //-------------------------------------------------------------------------
    //  Component Inspector
    //-------------------------------------------------------------------------
    /*! Typedef to remember the last index selected by user (used by component
        inspector) */
    typedef struct {
    public:
        int Process;
        int Component;
        int ProvidedInterface;
        int Command;
        int EventGenerator;
        int RequiredInterface;
        int Function;
        int EventHandler;

        void Reset() {
            Process = -1;
            Component = -1;
            ProvidedInterface = -1;
            Command = -1;
            EventGenerator = -1;
            RequiredInterface = -1;
            Function = -1;
            EventHandler = -1;
        }
    } SelectedIndexType;

    //-------------------------------------------------------------------------
    //  Data Visualizer
    //-------------------------------------------------------------------------
    /*! Typedef of an element that contains information about a signal(value) in
        a parameter  */
    typedef struct {
        bool Show;
        bool AutoScale;
        double Min;
        double Max;
        double Offset;
    } SignalState;

    /*! Typedef of an element that contains information about a signal that user
        picks up for visualization */
    class SignalSelected {
    public:
        /*! scalar index in arguments of read command */
        size_t Index;
        /*! index in the 'selected signal(s)' browser */
        unsigned int PlotIndex;
        /*! index assigned internally by multiplot */
        size_t MultiplotIndex;
        /*! signal color index associated with this signal */
        unsigned int SignalColor;

        std::string ProcessName;
        std::string ComponentName;
        std::string InterfaceName;
        std::string CommandName;
        std::string ArgumentName;
        std::string SignalName;
        SignalState State;

        double Timeout;
        double LastTimeFetched;

        void Refresh() {
            LastTimeFetched = osaGetTime(); // current tick in sec (double)
        }

        // Set timer timeout in sec
        void SetTimeout(const double timeout) {
            Timeout = osaGetTime() + timeout;
        }

        bool IsExpired() const {
            return (osaGetTime() > Timeout);
        }

        void ToStream(void) {
            std::cout << "Signal name     : " << SignalName << std::endl;
            std::cout << "scalar index    : " << Index << std::endl;
            std::cout << "plot index      : " << PlotIndex<< std::endl;
            std::cout << "Multiplot index : " << MultiplotIndex << std::endl;
            std::cout << "Signal Color    : " << SignalColor << std::endl << std::endl;
        }
    };

    // Typedef of per-process time origin information (the time when the first sample is fetched)
    //typedef std::map<std::string, double> TimeOriginMapType;

    // Typedef of a list to contain predefined signal color.
    typedef std::list<unsigned int> SignalColorSetType;

protected:
    // Time server object
    const osaTimeServer * TimeServer;

    // Component Viewer
    mtsComponentViewer * ComponentViewer;

    // Remember time origin (the time when the first sample is fetched) per process
    //TimeOriginMapType TimeOriginMap;
    double TimeOrigin;

    // Global Component Manager instance that this UI attaches to
    mtsManagerGlobal & GlobalComponentManager;

    // Remember the last index selected by user (used by component inspector)
    SelectedIndexType LastIndexClicked;
    SelectedIndexType CurrentIndexClicked;

    // List of signals which are currently being plotted
    std::list<SignalSelected*> SignalsBeingPlotted;
    // Mutex for data structure above (MJ: not necessary for FLTK
    osaMutex MutexSignal;

    // X-axis scaling factor
    int XAxisScaleFactor;

    // FLTK UI resources
    Fl_Progress * ProgressBars[MAX_CHANNEL_COUNT];
    MULTIPLOT * GraphPane;

    // Signal color set. When a signal is added, it pops available color from this
    // list and it is plotted using the color.  When a signal is removed, the
    // color that the signal was using is returned to this list for recycle.
    SignalColorSetType SignalColorSet;

    // System-wide logger
    void Log(const mtsLogMessage & log);

    // User interface generated by FTLK/fluid
public:
    GlobalComponentManagerUI UI;

    //-------------------------------------------------------------------------
    //  Update UI Check
    //-------------------------------------------------------------------------
    // Update overall UI
    void UpdateUI(void);
    // Check user's input from the 'Component Inspector' tab
    void CheckComponentInspectorInput(void);
    // Check user's input from the 'Logger' tab
    void CheckLoggerInput(void);
    // Check user's input from the 'Data Visualizer' tab
    void CheckDataVisualizerInput(void);

    //-------------------------------------------------------------------------
    //  Component Inspector
    //-------------------------------------------------------------------------
    // Populate browsers
    void PopulateComponents(const std::string & processName);
    void PopulateProvidedInterfaces(const std::string & processName,
                                    const std::string & componentName);
    void PopulateCommands(const std::string & processName,
                          const std::string & componentName,
                          const std::string & providedInterfaceName);
    void PopulateEventGenerators(const std::string & processName,
                                 const std::string & componentName,
                                 const std::string & providedInterfaceName);
    void PopulateRequiredInterfaces(const std::string & processName,
                                    const std::string & componentName);
    void PopulateFunctions(const std::string & processName,
                           const std::string & componentName,
                           const std::string & requiredInterfaceName);
    void PopulateEventHandlers(const std::string & processName,
                               const std::string & componentName,
                               const std::string & requiredInterfaceName);

    // Show detailed information
    void ShowCommandDescription(const std::string & processName,
                                const std::string & componentName,
                                const std::string & providedInterfaceName,
                                const std::string & commandName);
    void ShowEventGeneratorDescription(const std::string & processName,
                                       const std::string & componentName,
                                       const std::string & providedInterfaceName,
                                       const std::string & eventGeneratorName);
    void ShowFunctionDescription(const std::string & processName,
                                 const std::string & componentName,
                                 const std::string & requiredInterfaceName,
                                 const std::string & functionName);
    void ShowEventHandlerDescription(const std::string & processName,
                                     const std::string & componentName,
                                     const std::string & requiredInterfaceName,
                                     const std::string & eventHandlerName);

    // UI Event Handler
    void OnButtonRefreshClicked(void);

    //-------------------------------------------------------------------------
    //  Data Visualizer
    //-------------------------------------------------------------------------
    /*! Get detailed information on a selected read command */
    void GetArgumentInformation(const std::string & processName,
                                const std::string & componentName,
                                const std::string & providedInterfaceName,
                                const std::string & commandName,
                                std::string & argumentName,
                                std::vector<std::string> & argumentParameterNames);

    /*! Add the signal that a user selected to visualization list */
    void VisualizeSignal(SignalSelected & newSignal);

    /*! Fetch/sample current values that user has chosen to visualize */
    void FetchCurrentValues(void);

    /*! Set time origin (the time when the first sample is fetched) per process */
    void SetTimeOrigin(const double firstTimeStamp);

    /*! Get time origin */
    double GetTimeOrigin(void);

    /*! UI Event Handler */
    //
    // Global Control
    //
    // Scale Buttons
    void OnButtonYScaleUpClicked(void);
    void OnButtonYScaleDownClicked(void);
    void OnButtonXScaleUpClicked(void);
    void OnButtonXScaleDownClicked(void);
    // Offset Buttons
    void OnButtonGlobalOffsetIncreaseClicked(void);
    void OnButtonGlobalOffsetDecreaseClicked(void);
    void OnSpinnerGlobalOffsetControlClicked(void);
    // Hold Button
    void OnButtonHoldClicked(void);
    // Capture Button
    void OnButtonCaptureClicked(void) {}
    //
    // Per Signal Control
    //
    void OnBrowserSelectedSignalsClicked(void);
    // Remove/Remove all buttons
    void OnButtonRemoveClicked(void);
    void OnButtonRemoveAllClicked(void);
    // Hide button
    void OnButtonHideClicked(void);
    // Autoscale button
    void OnButtonAutoScaleClicked(void);
    // Offset buttons
    void OnButtonSignalOffsetIncreaseClicked(void);
    void OnButtonSignalOffsetDecreaseClicked(void);
    void OnSpinnerSignalOffsetControlClicked(void);

    /*! Reset UI */
    void ResetDataVisualizerUI(void);

    /*! Draw graph */
#ifdef BASIC_PLOTTING_TEST
    void PlotGraph(void);
#endif

#ifdef THIS_NEEDS_TO_BE_UPDATED_TO_USE_A_PROPER_COMMAND
    void DrawGraph(const mtsManagerLocalInterface::SetOfValues & values, const SignalSelected& signal);
#endif

    // Update output windows for current min/max values
    void UpdateMinMaxUI(void);
    // Update Hide/Show button
    void UpdateButtonHide(const bool isShow = true);

    // Get unit value for global offset
    float GetGlobalOffsetDelta();
    // Get unit value for per-signal offset
    float GetSignalOffsetDelta();

    //-------------------------------------------------------------------------
    //  Utilities
    //-------------------------------------------------------------------------
    std::string StripOffFormatCharacters(const std::string & text);

    void AddLineToBrowser(Fl_Browser * browser,
                          const char * line,
                          const int fg = FLTK_COLOR_BLACK,
                          const int bg = FLTK_COLOR_WHITE,
                          const char style = '0');
    void AddLineToBrowser(Fl_Check_Browser * checkBrowser, const char * line, bool checked = false);
    void AddLineToDescription(Fl_Output * output, const char * msg);

    SignalSelected * GetCurrentSignal(void) const;

public:
    /*! Constructor and destructor */
    GCMUITask(const std::string & taskName, const double period, mtsManagerGlobal& globalComponentManager);
    ~GCMUITask();

    void Configure(const std::string & CMN_UNUSED(filename) = "");
    void Startup(void);
    void Run(void);
    void Cleanup(void) {};

    bool UIOpened(void) const;
};

#endif // _GCMUITask_h
