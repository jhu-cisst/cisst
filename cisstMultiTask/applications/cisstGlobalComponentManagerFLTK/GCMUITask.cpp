/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2010-02-26

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "GCMUITask.h"
#include "PopupBrowser.h"

#include <cisstCommon/cmnStrings.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Input_Choice.H>

#include <iostream>
#include <fstream>

static std::ofstream SystemLogFile;

// macro to create an FLTK critical section with lock, unlock and awake
#define FLTK_CRITICAL_SECTION Fl::lock(); for (bool firstRun = true; firstRun; firstRun = false, Fl::unlock(), Fl::awake())

/////////////////////////////////////////////////////////////////////////////
//
// TODO:
// - Replace autorefresh/refresh buttons with callback mechanism
// - Clear SignalsBeingPlotted when a process disconnects 
//
// Improvements:
// - freeze (+export to file) / trigger / hold
// - higher quality oscilloscope -> support stop & zoom => requires more and more data 
//   to be collected???

#define MAX_ARGUMENT_PARAMETER_COUNT 12
#define STRING_CANCEL "Cancel"
#define OFFSET_MAX_VALUE 100000.0f

#define ENABLE_UI( _uiName )  UI._uiName->activate();
#define DISABLE_UI( _uiName ) UI._uiName->deactivate();

// GCMUITask object
GCMUITask * GCMUI;

//-------------------------------------------------------------------------
//  Callback Functions
//-------------------------------------------------------------------------
// Callback invoked when menu item selected
void callbackSignalSelect(Fl_Widget * CMN_UNUSED(w), void * v) 
{
    if (!v) return;
    
    GCMUITask::SignalSelected * signal = reinterpret_cast<GCMUITask::SignalSelected *>(v);
    
    // If 'cancel' is selected
    if (signal->SignalName == STRING_CANCEL) {
        return;
    }

    // Get actual command name
    const std::string actualCommandName = signal->CommandName.substr(3, signal->CommandName.size() - 2);

    // Check if a new signal can be added (the max number of signals that can be
    // plotted simultaneously is limited to MAX_ARGUMENT_PARAMETER_COUNT)
    std::stringstream ss;
    if (GCMUI->UI.BrowserSelectedSignals->size() == MAX_ARGUMENT_PARAMETER_COUNT) {
        ss << "Cannot add more signal. Try again after removing one or more signal";
        fl_choice(ss.str().c_str(), "OK", NULL, NULL);
        std::cout << ss.str() << std::endl;
        return;
    }

    ss << "The following signal is added for visualization:\n";
    ss << "   Signal \"" << signal->SignalName << "\" of command \"" << actualCommandName << "\"";
    fl_choice(ss.str().c_str(), "Confirm", NULL, NULL);
    std::cout << ss.str() << std::endl;

    // Switch the current focus to the data visualization tab
    GCMUI->UI.TabControl->value(GCMUI->UI.DataVisualizer);
    // Start collecting data for visualization
    FLTK_CRITICAL_SECTION {
        GCMUI->VisualizeSignal(*signal);
    }

    delete signal;
}

void callbackPopupSignalSelectionMenu(Fl_Widget * w, void *userdata)
{
    const size_t idxClicked = reinterpret_cast<size_t>(userdata);
    if (idxClicked == 0) {
        return;
    }

    const std::string processName = GCMUI->StripOffFormatCharacters(
        GCMUI->UI.BrowserProcesses->text(GCMUI->UI.BrowserProcesses->value()));
    const std::string componentName = GCMUI->StripOffFormatCharacters(
        GCMUI->UI.BrowserComponents->text(GCMUI->UI.BrowserComponents->value()));
    const std::string interfaceName = GCMUI->StripOffFormatCharacters(
        GCMUI->UI.BrowserProvidedInterfaces->text(GCMUI->UI.BrowserProvidedInterfaces->value()));
    const std::string commandName = GCMUI->StripOffFormatCharacters(
        GCMUI->UI.BrowserCommands->text(idxClicked));

    // Allow only read commands to be selected
    const char commandType = *commandName.c_str();
    if (commandType != 'R') {
        return;
    }

    // Get argument information from LCM
    std::string argumentName;
    std::vector<std::string> signalNames;
    GCMUI->GetArgumentInformation(processName, componentName, interfaceName, commandName, argumentName, signalNames);
    if (signalNames.size() == 0) {
        return;
    }

    // Popup signal selection menu
    // http://www.fltk.org/doc-1.0/Fl_Menu_Item.html#Fl_Menu_Item.popup
    Fl_Menu_Item * popupMenus = new Fl_Menu_Item[signalNames.size() + 2];
    for (unsigned int i = 0; i < signalNames.size(); ++i) {
        memset(&popupMenus[i], 0, sizeof(Fl_Menu_Item));
        popupMenus[i].text = signalNames[i].c_str();
        /*
        if (i < 9) {
            popupMenus[i].shortcut(FL_ALT + '1' + i);
        } else {
            popupMenus[i].shortcut(0);
        }
        */
        popupMenus[i].shortcut(0);
        popupMenus[i].callback(callbackSignalSelect);
        popupMenus[i].user_data((void*) i);
        popupMenus[i].flags = (i == signalNames.size() - 1 ? FL_MENU_DIVIDER : 0);
        popupMenus[i].labeltype(FL_NORMAL_LABEL);
        popupMenus[i].labelfont(FL_COURIER);
        popupMenus[i].labelsize(12);
    }
    // Add cancel menu at the end
    unsigned int j = signalNames.size();
    memset(&popupMenus[j], 0, sizeof(Fl_Menu_Item));
    popupMenus[j].text = STRING_CANCEL;
    popupMenus[j].shortcut(FL_ALT + 'c');
    popupMenus[j].callback(callbackSignalSelect);
    popupMenus[j].user_data((void*) j);
    popupMenus[j].flags = 0;
    popupMenus[j].labeltype(FL_NORMAL_LABEL);
    popupMenus[j].labelfont(FL_HELVETICA);
    popupMenus[j].labelsize(12);
    // Add end mark
    memset(&popupMenus[signalNames.size() + 1], 0, sizeof(Fl_Menu_Item));

    const Fl_Menu_Item * m = popupMenus->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
    if (m) {
        GCMUITask::SignalSelected * signal = new GCMUITask::SignalSelected;
        signal->Index = (int) reinterpret_cast<size_t>(m->user_data());
        signal->ProcessName = processName;
        signal->ComponentName = componentName;
        signal->InterfaceName = interfaceName;
        signal->CommandName = commandName;
        signal->ArgumentName = argumentName;
        signal->SignalName = (signal->Index == signalNames.size() ? STRING_CANCEL : signalNames[signal->Index]); // If cancel is selected

        m->do_callback(w, (void*)signal);
    }

    delete [] popupMenus;
}

void callbackVisualize(Fl_Widget * w, void *userdata)
{
    PopupBrowser * browser = (PopupBrowser*) userdata;
    if (browser->value() == 0) {
        return;
    }

    const int idxClicked = browser->value();
    const std::string commandName = GCMUI->StripOffFormatCharacters(browser->text(idxClicked));
    std::cout << "Visualize command: (" << idxClicked << ") " << commandName << std::endl;

    callbackPopupSignalSelectionMenu(w, (void*)idxClicked);
}

//-------------------------------------------------------------------------
//  GCMUITask
//-------------------------------------------------------------------------
GCMUITask::GCMUITask(const std::string & taskName, const double period, 
                     mtsManagerGlobal& globalComponentManager) :
    mtsTaskPeriodic(taskName, period, false, 5000),
    ComponentViewer(0),
    GlobalComponentManager(globalComponentManager)
{
    GCMUI = this;
    TimeOrigin = 0.0;

    TimeServer = &mtsManagerLocal::GetInstance()->GetTimeServer();

    // To receive system-wide logs from MCS
    mtsInterfaceRequired * required = AddInterfaceRequired(
        mtsManagerComponentBase::InterfaceNames::InterfaceSystemLoggerRequired);
    if (required) {
        required->AddEventHandlerWrite(&GCMUITask::Log, this, 
                                       mtsManagerComponentBase::EventNames::PrintLog);
    }

    EnableDynamicComponentManagement();

    // File log
    SystemLogFile.open("cisstSystemLog.txt");
}

GCMUITask::~GCMUITask()
{
    delete UI.Log->buffer();

    if (SystemLogFile.is_open())
        SystemLogFile.close();
}

void GCMUITask::Configure(const std::string & CMN_UNUSED(filename))
{
    // make the UI visible
    UI.show(0, NULL);
	UI.Opened = true;
}

void GCMUITask::Startup(void) 
{
    //
    // Component Inspector
    //
    LastIndexClicked.Reset();
    CurrentIndexClicked.Reset();

    // HostIP
    Fl_Text_Buffer * buf = new Fl_Text_Buffer();
    FLTK_CRITICAL_SECTION {
        UI.TextDisplayHostIP->buffer(buf);
    }
    
    StringVector ipAddresses;
    GlobalComponentManager.GetIPAddress(ipAddresses);
    
    std::string ipString;
    if (ipAddresses.size() == 0) {
        ipString += "Failed to retrieve IP address";
        CMN_LOG_CLASS_INIT_ERROR << "GCMUITask: No IP address detected on this machine" << std::endl;
        cmnThrow(std::runtime_error("No IP address detected on this machine"));
    } else {
        ipString += ipAddresses[0];
        for (StringVector::size_type i = 1; i < ipAddresses.size(); ++i) {
            ipString += "; ";
            ipString += ipAddresses[i];
        }
    }

    buf->text(ipString.c_str());

    //
    // Data Visualizer
    //
    XAxisScaleFactor = 150;

    FLTK_CRITICAL_SECTION {
        GraphPane = UI.GraphPane;
        GraphPane->set_scrolling(100);
        GraphPane->SetAutoScale(false);
        GraphPane->set_grid(MP_LINEAR_GRID, MP_LINEAR_GRID, true);
        GraphPane->set_grid_color(GRAY);
        //GraphPane->set_grid_color(DARK_GRAY);
        //GraphPane->set_bg_color(1.0f, 1.0f, 1.0f);

        for (int i=0; i< MAX_ARGUMENT_PARAMETER_COUNT; ++i) {
            GraphPane->set_pointsize(i, 1.0);
            GraphPane->set_linewidth(i, 1.0);
        }

        // Populate available signal color set
        SignalColorSet.push_back(1);
        SignalColorSet.push_back(2);
        SignalColorSet.push_back(3);
        SignalColorSet.push_back(4);
        SignalColorSet.push_back(5);
        SignalColorSet.push_back(6);
        SignalColorSet.push_back(9);
        SignalColorSet.push_back(10);
        SignalColorSet.push_back(11);
        SignalColorSet.push_back(12);
        SignalColorSet.push_back(13);
        SignalColorSet.push_back(14);

        // Setup for progress bars
        ProgressBars[0] = UI.Progress1;
        ProgressBars[1] = UI.Progress2;
        ProgressBars[2] = UI.Progress3;
        ProgressBars[3] = UI.Progress4;
        ProgressBars[4] = UI.Progress5;
        ProgressBars[5] = UI.Progress6;
        ProgressBars[6] = UI.Progress7;
        ProgressBars[7] = UI.Progress8;
        ProgressBars[8] = UI.Progress9;
        ProgressBars[9] = UI.Progress10;
        ProgressBars[10] = UI.Progress11;
        ProgressBars[11] = UI.Progress12;
        for (int i = 0; i < MAX_CHANNEL_COUNT; ++i) {
            ProgressBars[i]->maximum(1.0);
        }

        // Offset controller
        UI.SpinnerGlobalOffsetControl->range(0.0, (double)OFFSET_MAX_VALUE);
        UI.SpinnerSignalOffsetControl->range(0.0, (double)OFFSET_MAX_VALUE);

        ResetDataVisualizerUI();

        // Logger
        UI.Log->textsize(13);
        buf = new Fl_Text_Buffer();
        UI.Log->buffer(buf);
        buf->text("");
    }
}

void GCMUITask::Run(void)
{
    if (!UIOpened()) 
        return;

#ifdef BASIC_PLOTTING_TEST
    PlotGraph();
#endif

    static int count = 0;
    if (++count == 20) { // 1 sec
        // TODO: Replace autorefresh/refresh buttons with callback/event mechanism
        FLTK_CRITICAL_SECTION {
            if (UI.ButtonAutoRefresh->value() != 0) {
                UpdateUI();
            }
            count = 0;
        }
        return;
    }

    FLTK_CRITICAL_SECTION {
        // Check user's input from the 'Component Inspector' tab
        CheckComponentInspectorInput();
        // Check user's input from the 'Logger' tab
        CheckLoggerInput();
        // Check user's input from the 'Data Visualizer' tab
        CheckDataVisualizerInput();

        // Fetch/sample current values that user has chosen to visualize.
        FetchCurrentValues();

        // Refresh immediately
        if (UI.ButtonRefreshClicked) {
            OnButtonRefreshClicked();
            UI.ButtonRefreshClicked = false;
        }

        // Connect to Component Viewer
        if (UI.ButtonComponentViewerClicked) {
            // Create and start Component Viewer
            if (!ComponentViewer) {
                ComponentViewer = new mtsComponentViewer("ComponentViewer");
                mtsManagerLocal *managerLocal = mtsManagerLocal::GetInstance();
                managerLocal->AddComponent(ComponentViewer);
                osaSleep(0.2);
                ComponentViewer->Create();
                osaSleep(0.2);
                ComponentViewer->Start();
            }
            UI.ButtonComponentViewerClicked = false;
        }

#if 0
        // TEMP: for now, don't allow Component Viewer to be recreated because
        // it causes an unhandled exception.  Perhaps RemoveComponent does not work.
        if (ComponentViewer && ComponentViewer->IsTerminated()) {
            mtsManagerLocal *managerLocal = mtsManagerLocal::GetInstance();
            managerLocal->RemoveComponent(ComponentViewer);
            delete ComponentViewer;
            ComponentViewer = 0;
        }
#endif
    }

    ProcessQueuedEvents();
    //ProcessQueuedCommands();
}

#ifdef BASIC_PLOTTING_TEST
void GCMUITask::PlotGraph(void)
{
    FLTK_CRITICAL_SECTION {
        // Show min/max Y values
        const float Ymin = GraphPane->GetYMin();
        const float Ymax = GraphPane->GetYMax();

        char buf[100] = "";
        sprintf(buf, "%.2f", Ymax); UI.OutputMaxValue->value(buf);
        sprintf(buf, "%.2f", Ymin); UI.OutputMinValue->value(buf);

#ifdef BASIC_PLOTTING_TEST
        static unsigned int x = 0;

        for (int i=0; i<=12; ++i) {
            if (i <= 7) {
                GraphPane->set_pointsize(i, 2.0);
                GraphPane->set_linewidth(i, 1.0);
            } else {
                GraphPane->set_pointsize(i, 2.0);
                GraphPane->set_linewidth(i, 1.0);
            }
        }

        float value = sin(x/6.0f);

        GraphPane->add(0, PLOT_POINT((float)x, value * 1.0f,  RED));
        GraphPane->add(1, PLOT_POINT((float)x, value * 1.1f,  GREEN));
        GraphPane->add(2, PLOT_POINT((float)x, value * 1.2f,  YELLOW));
        /*
           GraphPane->add(3, PLOT_POINT((float)x, value * 1.3f,  BLUE));
           GraphPane->add(4, PLOT_POINT((float)x, value * 1.4f,  FUCHSIA));
           GraphPane->add(5, PLOT_POINT((float)x, value * 1.5f,  AQUA));
           GraphPane->add(6, PLOT_POINT((float)x, value * 1.6f,  DARK_RED));
           GraphPane->add(7, PLOT_POINT((float)x, value * 1.7f,  DARK_GREEN));
           GraphPane->add(8, PLOT_POINT((float)x, value * 1.8f,  DARK_YELLOW));
           GraphPane->add(9, PLOT_POINT((float)x, value * 1.9f,  LIGHT_PURPLE));
           GraphPane->add(10, PLOT_POINT((float)x, value * 2.0f, DARK_PURPLE));
           GraphPane->add(11, PLOT_POINT((float)x, value * 2.1f, DARK_AQUA));
           */

        ++x;
#endif

#ifdef SIGNAL_CONTROL_TEST
        static unsigned int x = 0;

        GraphPane->set_pointsize(1, 3.0);
        GraphPane->set_linewidth(1, 1.0);
        GraphPane->set_pointsize(2, 1.0);
        GraphPane->set_linewidth(2, 1.0);

        float value1 = sin(x/6.0f) * 10.0f;
        float value2 = cos(x/6.0f + M_PI/2) * 7.0f;

        GraphPane->add(0, PLOT_POINT((float)x, value1, YELLOW));
        GraphPane->add(1, PLOT_POINT((float)x, value2, RED));

        ++x;
#endif

        GraphPane->redraw();
    }
}
#endif

void GCMUITask::CheckComponentInspectorInput(void)
{
    CurrentIndexClicked.Process           = UI.BrowserProcesses->value();
    CurrentIndexClicked.Component         = UI.BrowserComponents->value();
    CurrentIndexClicked.ProvidedInterface = UI.BrowserProvidedInterfaces->value();
    CurrentIndexClicked.Command           = UI.BrowserCommands->value();
    CurrentIndexClicked.EventGenerator    = UI.BrowserEventGenerators->value();
    CurrentIndexClicked.RequiredInterface = UI.BrowserRequiredInterfaces->value();
    CurrentIndexClicked.Function          = UI.BrowserFunctions->value();
    CurrentIndexClicked.EventHandler      = UI.BrowserEventHandlers->value();

    // Check if an user clicked process browser
    if (CurrentIndexClicked.Process) {
        if (LastIndexClicked.Process != CurrentIndexClicked.Process) {
            // clear all subsequent browsers
            UI.BrowserComponents->clear();
            UI.BrowserProvidedInterfaces->clear();
            UI.BrowserCommands->clear();
            UI.BrowserEventGenerators->clear();
            UI.BrowserRequiredInterfaces->clear();
            UI.BrowserFunctions->clear();
            UI.BrowserEventHandlers->clear();
            UI.OutputCommandDescription->value("");
            UI.OutputEventGeneratorDescription->value("");
            UI.OutputFunctionDescription->value("");
            UI.OutputEventHandlerDescription->value("");

            LastIndexClicked.Reset();

            // Get a name of the selected process
            // If the component is a proxy. strip format control characters from it.
            const std::string processName = StripOffFormatCharacters(UI.BrowserProcesses->text(CurrentIndexClicked.Process));

            PopulateComponents(processName);

            LastIndexClicked.Process = CurrentIndexClicked.Process;

            return;
        }
    }

    // Check if an user clicked component browser
    if (CurrentIndexClicked.Component) {
        if (LastIndexClicked.Component != CurrentIndexClicked.Component) {
            // clear all subsequent browsers
            UI.BrowserProvidedInterfaces->clear();
            UI.BrowserCommands->clear();
            UI.BrowserEventGenerators->clear();
            UI.BrowserRequiredInterfaces->clear();
            UI.BrowserFunctions->clear();
            UI.BrowserEventHandlers->clear();
            UI.OutputCommandDescription->value("");
            UI.OutputEventGeneratorDescription->value("");
            UI.OutputFunctionDescription->value("");
            UI.OutputEventHandlerDescription->value("");

            LastIndexClicked.ProvidedInterface = -1;
            LastIndexClicked.Command = -1;
            LastIndexClicked.EventGenerator = -1;
            LastIndexClicked.RequiredInterface = -1;
            LastIndexClicked.Function = -1;
            LastIndexClicked.EventHandler = -1;

            // Get a name of the selected process and component
            const std::string processName = StripOffFormatCharacters(UI.BrowserProcesses->text(CurrentIndexClicked.Process));
            const std::string componentName = StripOffFormatCharacters(UI.BrowserComponents->text(CurrentIndexClicked.Component));
            
            PopulateProvidedInterfaces(processName, componentName);
            PopulateRequiredInterfaces(processName, componentName);

            LastIndexClicked.Component = CurrentIndexClicked.Component;
            
            return;
        }
    }

    // Check if an user clicked provided interface browser
    if (CurrentIndexClicked.ProvidedInterface) {
        if (LastIndexClicked.ProvidedInterface != CurrentIndexClicked.ProvidedInterface) {
            // clear all subsequent browsers
            UI.BrowserCommands->clear();
            UI.BrowserEventGenerators->clear();
            UI.OutputCommandDescription->value("");
            UI.OutputEventGeneratorDescription->value("");

            LastIndexClicked.Command = -1;
            LastIndexClicked.EventGenerator = -1;

            // Get a name of the selected process, component, and provided interface
            const std::string processName = StripOffFormatCharacters(UI.BrowserProcesses->text(CurrentIndexClicked.Process));
            const std::string componentName = StripOffFormatCharacters(UI.BrowserComponents->text(CurrentIndexClicked.Component));
            const std::string providedInterfaceName = StripOffFormatCharacters(UI.BrowserProvidedInterfaces->text(CurrentIndexClicked.ProvidedInterface));
            
            PopulateCommands(processName, componentName, providedInterfaceName);
            PopulateEventGenerators(processName, componentName, providedInterfaceName);

            LastIndexClicked.ProvidedInterface = CurrentIndexClicked.ProvidedInterface;

            return;
        }
    }

    // Check if an user clicked command browser
    if (CurrentIndexClicked.Command) {
        if (LastIndexClicked.Command != CurrentIndexClicked.Command) {
            // clear related widget
            UI.OutputCommandDescription->value("");

            // Get a name of the selected process, component, provided interface, and command
            const std::string processName = StripOffFormatCharacters(UI.BrowserProcesses->text(CurrentIndexClicked.Process));
            const std::string componentName = StripOffFormatCharacters(UI.BrowserComponents->text(CurrentIndexClicked.Component));
            const std::string providedInterfaceName = StripOffFormatCharacters(UI.BrowserProvidedInterfaces->text(CurrentIndexClicked.ProvidedInterface));
            const std::string commandName = StripOffFormatCharacters(UI.BrowserCommands->text(CurrentIndexClicked.Command));
            
            ShowCommandDescription(processName, componentName, providedInterfaceName, commandName);

            LastIndexClicked.Command = CurrentIndexClicked.Command;

            return;
        }
    }

    // Check if an user clicked event generator browser
    if (CurrentIndexClicked.EventGenerator) {
        if (LastIndexClicked.EventGenerator != CurrentIndexClicked.EventGenerator) {
            // clear related widget
            UI.OutputEventGeneratorDescription->value("");

            // Get a name of the selected process, component, provided interface, and command
            const std::string processName = StripOffFormatCharacters(UI.BrowserProcesses->text(CurrentIndexClicked.Process));
            const std::string componentName = StripOffFormatCharacters(UI.BrowserComponents->text(CurrentIndexClicked.Component));
            const std::string providedInterfaceName = StripOffFormatCharacters(UI.BrowserProvidedInterfaces->text(CurrentIndexClicked.ProvidedInterface));
            const std::string eventGeneratorName = StripOffFormatCharacters(UI.BrowserEventGenerators->text(CurrentIndexClicked.EventGenerator));
            
            ShowEventGeneratorDescription(processName, componentName, providedInterfaceName, eventGeneratorName);

            LastIndexClicked.EventGenerator = CurrentIndexClicked.EventGenerator;

            return;
        }
    }

    // Check if an user clicked required interface browser
    if (CurrentIndexClicked.RequiredInterface) {
        if (LastIndexClicked.RequiredInterface != CurrentIndexClicked.RequiredInterface) {
            // clear all subsequent browsers
            UI.BrowserFunctions->clear();
            UI.BrowserEventHandlers->clear();
            UI.OutputFunctionDescription->value("");
            UI.OutputEventHandlerDescription->value("");
            LastIndexClicked.Function = -1;
            LastIndexClicked.EventHandler = -1;

            // Get a name of the selected process, component, and required interface
            const std::string processName = StripOffFormatCharacters(UI.BrowserProcesses->text(CurrentIndexClicked.Process));
            const std::string componentName = StripOffFormatCharacters(UI.BrowserComponents->text(CurrentIndexClicked.Component));
            const std::string requiredInterfaceName = StripOffFormatCharacters(UI.BrowserRequiredInterfaces->text(CurrentIndexClicked.RequiredInterface));
            
            PopulateFunctions(processName, componentName, requiredInterfaceName);
            PopulateEventHandlers(processName, componentName, requiredInterfaceName);

            LastIndexClicked.RequiredInterface = CurrentIndexClicked.RequiredInterface;

            return;
        }
    }

    // Check if an user clicked function browser
    if (CurrentIndexClicked.Function) {
        if (LastIndexClicked.Function != CurrentIndexClicked.Function) {
            // clear related widget
            UI.OutputFunctionDescription->value("");

            // Get a name of the selected process, component, provided interface, and command
            const std::string processName = StripOffFormatCharacters(UI.BrowserProcesses->text(CurrentIndexClicked.Process));
            const std::string componentName = StripOffFormatCharacters(UI.BrowserComponents->text(CurrentIndexClicked.Component));
            const std::string requiredInterfaceName = StripOffFormatCharacters(UI.BrowserRequiredInterfaces->text(CurrentIndexClicked.RequiredInterface));
            const std::string functionName = StripOffFormatCharacters(UI.BrowserFunctions->text(CurrentIndexClicked.Function));
            
            ShowFunctionDescription(processName, componentName, requiredInterfaceName, functionName);

            LastIndexClicked.Function = CurrentIndexClicked.Function;

            return;
        }
    }

    // Check if an user clicked event handler browser
    if (CurrentIndexClicked.EventHandler) {
        if (LastIndexClicked.EventHandler != CurrentIndexClicked.EventHandler) {
            // clear related widget
            UI.OutputEventHandlerDescription->value("");

            // Get a name of the selected process, component, provided interface, and command
            const std::string processName = StripOffFormatCharacters(UI.BrowserProcesses->text(CurrentIndexClicked.Process));
            const std::string componentName = StripOffFormatCharacters(UI.BrowserComponents->text(CurrentIndexClicked.Component));
            const std::string requiredInterfaceName = StripOffFormatCharacters(UI.BrowserRequiredInterfaces->text(CurrentIndexClicked.RequiredInterface));
            const std::string eventHandlerName = StripOffFormatCharacters(UI.BrowserEventHandlers->text(CurrentIndexClicked.EventHandler));
            
            ShowEventHandlerDescription(processName, componentName, requiredInterfaceName, eventHandlerName);

            LastIndexClicked.EventHandler = CurrentIndexClicked.EventHandler;

            return;
        }
    }
}

// Handle click events from Logger tab
void GCMUITask::CheckLoggerInput(void)
{
    if (UI.BrowserProcessLogClicked) {
        UI.BrowserProcessLogClicked = false;
        // Get checked state
        const int index = UI.BrowserProcessLog->value();
        if (index != 0) {
            int checked = UI.BrowserProcessLog->checked(index);
            const std::string processName = UI.BrowserProcessLog->text(index);
            stdStringVec processNames;
            processNames.push_back(processName);
            if (checked == 1) {
                ManagerComponentServices->EnableLogForwarding(processNames);
                CMN_LOG_CLASS_RUN_VERBOSE << "Enabling system-wide logging for process \"" << processName << "\"" << std::endl;
            } else {
                ManagerComponentServices->DisableLogForwarding(processNames);
                CMN_LOG_CLASS_RUN_VERBOSE << "Disabling system-wide logging for process \"" << processName << "\"" << std::endl;
            }
            return;
        }
    }

    if (UI.ButtonLogForwardEnableAllClicked) {
        UI.BrowserProcessLog->check_all();
        UI.ButtonLogForwardEnableAllClicked = false;
        ManagerComponentServices->EnableLogForwarding();
        CMN_LOG_CLASS_RUN_VERBOSE << "Enabling system-wide logging for all processes" << std::endl;
        return;
    }

    if (UI.ButtonLogForwardDisableAllClicked) {
        UI.BrowserProcessLog->check_none();
        UI.ButtonLogForwardDisableAllClicked = false;
        ManagerComponentServices->DisableLogForwarding();
        CMN_LOG_CLASS_RUN_VERBOSE << "Disabling system-wide logging for all processes" << std::endl;
        return;
    }
}

void GCMUITask::UpdateUI(void)
{
    UI.BrowserProcesses->clear();
    UI.BrowserComponents->clear();
    UI.BrowserProvidedInterfaces->clear();
    UI.BrowserCommands->clear();
    UI.BrowserEventGenerators->clear();
    UI.BrowserRequiredInterfaces->clear();
    UI.BrowserFunctions->clear();
    UI.BrowserEventHandlers->clear();

    UI.OutputCommandDescription->value("");
    UI.OutputEventGeneratorDescription->value("");
    UI.OutputFunctionDescription->value("");
    UI.OutputEventHandlerDescription->value("");

    UI.BrowserProcessLog->clear();

    // Periodically fetch active process list from GCM
    StringVector names;
    GlobalComponentManager.GetNamesOfProcesses(names);

    // Populate process list UI of the Component Viewer tab
    for (StringVector::size_type i = 0; i < names.size(); ++i) {
        AddLineToBrowser(UI.BrowserProcesses, names[i].c_str());
    }

    // Populate process list UI of the Logger tab
    stdCharVec logForwardStates;
    ManagerComponentServices->GetLogForwardingStates(names, logForwardStates);

    for (stdCharVec::size_type i = 0; i < logForwardStates.size(); ++i) {
        AddLineToBrowser(UI.BrowserProcessLog, names[i].c_str(), logForwardStates[i]);
    }
   
    LastIndexClicked.Reset();
}

void GCMUITask::AddLineToBrowser(Fl_Browser * browser, const char * line, const int fg, const int bg, const char style)
{
    char buf[50] = "";

    std::stringstream ss;
    if (style == '0') {
        ss << "@B" << bg << "@C" << fg << "@.";
        strncpy(buf, line, sizeof(buf) - 15);
        ss << buf;
    } else {
        ss << "@B" << bg << "@C" << fg << "@" << style << "@.";
        strncpy(buf, line, sizeof(buf) - 15);
        ss << buf;
    }
    browser->add(ss.str().c_str());
}

void GCMUITask::AddLineToBrowser(Fl_Check_Browser * browser, const char * line, bool checked)
{
    browser->add(line, (checked? 1 : 0));
}

void GCMUITask::AddLineToDescription(Fl_Output * output, const char * msg)
{
    char *cat = new char[strlen(output->value()) + strlen(msg) + 1];
    strcpy(cat, output->value());
    strcat(cat, msg);
    output->value(cat);
    delete [] cat;
}

std::string GCMUITask::StripOffFormatCharacters(const std::string & text)
{
    std::string ret;

    const std::string delim = "@.";
    size_t pos = text.find(delim);
    if (pos != std::string::npos) {
        ret = text.substr(pos + 2, text.size() - (pos + 2));
    } else {
        ret = text;
    }

    return ret;
}

void GCMUITask::PopulateComponents(const std::string & processName)
{
    UI.BrowserComponents->clear();

    // Get all names of components in the process
    StringVector names;
    GlobalComponentManager.GetNamesOfComponents(processName, names);

    std::string componentName;
    for (StringVector::size_type i = 0; i < names.size(); ++i) {
        componentName = names[i];
        if (mtsManagerGlobal::IsProxyComponent(componentName)) {
            AddLineToBrowser(UI.BrowserComponents, componentName.c_str(), FLTK_COLOR_BLUE, FLTK_COLOR_WHITE, FLTK_STYLE_ITALIC);
        } else {
            AddLineToBrowser(UI.BrowserComponents, componentName.c_str());
        }
    }
}

void GCMUITask::PopulateProvidedInterfaces(const std::string & processName, const std::string & componentName)
{
    UI.BrowserProvidedInterfaces->clear();

    // Get all names of provided interfaces in the component
    StringVector names;
    GlobalComponentManager.GetNamesOfInterfacesProvidedOrOutput(processName, componentName, names);

    for (StringVector::size_type i = 0; i < names.size(); ++i) {
        AddLineToBrowser(UI.BrowserProvidedInterfaces, names[i].c_str());
    }
}

void GCMUITask::PopulateCommands(const std::string & processName, const std::string & componentName, const std::string & providedInterfaceName)
{
    UI.BrowserCommands->clear();

    // Get all names of provided interfaces in the component
    StringVector names;
    GlobalComponentManager.GetNamesOfCommands(processName, componentName, providedInterfaceName, names);

    for (StringVector::size_type i = 0; i < names.size(); ++i) {
        AddLineToBrowser(UI.BrowserCommands, names[i].c_str());
    }
}

void GCMUITask::PopulateEventGenerators(const std::string & processName, const std::string & componentName, const std::string & providedInterfaceName)
{
    UI.BrowserEventGenerators->clear();

    // Get all names of provided interfaces in the component
    StringVector names;
    GlobalComponentManager.GetNamesOfEventGenerators(processName, componentName, providedInterfaceName, names);

    for (StringVector::size_type i = 0; i < names.size(); ++i) {
        AddLineToBrowser(UI.BrowserEventGenerators, names[i].c_str());
    }
}

void GCMUITask::PopulateRequiredInterfaces(const std::string & processName, const std::string & componentName)
{
    UI.BrowserRequiredInterfaces->clear();

    // Get all names of provided interfaces in the component
    StringVector names;
    GlobalComponentManager.GetNamesOfInterfacesRequiredOrInput(processName, componentName, names);

    for (StringVector::size_type i = 0; i < names.size(); ++i) {
        AddLineToBrowser(UI.BrowserRequiredInterfaces, names[i].c_str());
    }
}

void GCMUITask::PopulateFunctions(const std::string & processName, const std::string & componentName, const std::string & requiredInterfaceName)
{
    UI.BrowserFunctions->clear();

    // Get all names of provided interfaces in the component
    StringVector names;
    GlobalComponentManager.GetNamesOfFunctions(processName, componentName, requiredInterfaceName, names);

    for (StringVector::size_type i = 0; i < names.size(); ++i) {
        AddLineToBrowser(UI.BrowserFunctions, names[i].c_str());
    }
}

void GCMUITask::PopulateEventHandlers(const std::string & processName, const std::string & componentName, const std::string & requiredInterfaceName)
{
    UI.BrowserEventHandlers->clear();

    // Get all names of provided interfaces in the component
    StringVector names;
    GlobalComponentManager.GetNamesOfEventHandlers(processName, componentName, requiredInterfaceName, names);

    for (StringVector::size_type i = 0; i < names.size(); ++i) {
        AddLineToBrowser(UI.BrowserEventHandlers, names[i].c_str());
    }
}

void GCMUITask::ShowCommandDescription(const std::string & processName, 
                                       const std::string & componentName, 
                                       const std::string & providedInterfaceName, 
                                       const std::string & commandName)
{
    char buf[100] = "";
    sprintf(buf, "Command: %s\n", commandName.substr(3, commandName.size() - 2).c_str());
    UI.OutputCommandDescription->value(buf);
    
    std::string description;
    GlobalComponentManager.GetDescriptionOfCommand(processName, componentName, providedInterfaceName, commandName, description);

    AddLineToDescription(UI.OutputCommandDescription, description.c_str());
}

void GCMUITask::ShowEventGeneratorDescription(const std::string & processName, 
                                              const std::string & componentName, 
                                              const std::string & providedInterfaceName, 
                                              const std::string & eventGeneratorName)
{
    char buf[100] = "";
    sprintf(buf, "Event generator: %s\n", eventGeneratorName.substr(3, eventGeneratorName.size() - 2).c_str());
    UI.OutputEventGeneratorDescription->value(buf);

    std::string description;
    GlobalComponentManager.GetDescriptionOfEventGenerator(processName, componentName, providedInterfaceName, eventGeneratorName, description);

    AddLineToDescription(UI.OutputEventGeneratorDescription, description.c_str());
}

void GCMUITask::ShowFunctionDescription(const std::string & processName, 
                                        const std::string & componentName, 
                                        const std::string & requiredInterfaceName, 
                                        const std::string & functionName)
{
    char buf[100] = "";
    sprintf(buf, "Function: %s\n", functionName.substr(3, functionName.size() - 2).c_str());
    UI.OutputFunctionDescription->value(buf);

    std::string description;
    GlobalComponentManager.GetDescriptionOfFunction(processName, componentName, requiredInterfaceName, functionName, description);

    AddLineToDescription(UI.OutputFunctionDescription, description.c_str());
}

void GCMUITask::ShowEventHandlerDescription(const std::string & processName, 
                                            const std::string & componentName, 
                                            const std::string & requiredInterfaceName, 
                                            const std::string & eventHandlerName)
{
    char buf[100] = "";
    sprintf(buf, "Event handler: %s\n", eventHandlerName.substr(3, eventHandlerName.size() - 2).c_str());
    UI.OutputEventHandlerDescription->value(buf);

    std::string description;
    GlobalComponentManager.GetDescriptionOfEventHandler(processName, componentName, requiredInterfaceName, eventHandlerName, description);

    AddLineToDescription(UI.OutputEventHandlerDescription, description.c_str());
}

void GCMUITask::CheckDataVisualizerInput(void)
{
#define SET_CALLBACK_FUNCTION( _uiName )\
    if (UI._uiName##Clicked) {\
        On##_uiName##Clicked();\
        UI._uiName##Clicked = false;\
        return;\
    }

    //
    // Component Inspector
    //
    
    // NOP

    //
    // Data Visualizer - Global Control
    // 
    SET_CALLBACK_FUNCTION(BrowserSelectedSignals);
    // Scale Buttons
    SET_CALLBACK_FUNCTION(ButtonYScaleUp);
    SET_CALLBACK_FUNCTION(ButtonYScaleDown);
    SET_CALLBACK_FUNCTION(ButtonXScaleUp);
    SET_CALLBACK_FUNCTION(ButtonXScaleDown);
    // Offset Buttons
    SET_CALLBACK_FUNCTION(ButtonGlobalOffsetIncrease);
    SET_CALLBACK_FUNCTION(ButtonGlobalOffsetDecrease);
    SET_CALLBACK_FUNCTION(SpinnerGlobalOffsetControl);

    // Hold Button
    SET_CALLBACK_FUNCTION(ButtonHold);
    // Capture Button
    SET_CALLBACK_FUNCTION(ButtonCapture);
    //
    // Data Visualizer - Per Signal Control
    //
    // Remove/Remove all buttons
    SET_CALLBACK_FUNCTION(ButtonRemove);
    SET_CALLBACK_FUNCTION(ButtonRemoveAll);
    // Hide button
    SET_CALLBACK_FUNCTION(ButtonHide);
    // Autoscale button
    SET_CALLBACK_FUNCTION(ButtonAutoScale);
    // Offset buttons
    SET_CALLBACK_FUNCTION(ButtonSignalOffsetIncrease);
    SET_CALLBACK_FUNCTION(ButtonSignalOffsetDecrease);
    SET_CALLBACK_FUNCTION(SpinnerSignalOffsetControl);
}

void GCMUITask::ResetDataVisualizerUI(void)
{
    for (int i = 0; i < MAX_CHANNEL_COUNT; ++i) {
        ProgressBars[i]->value(0.0);
    }

    UI.OutputProcessName->value("");
    UI.OutputComponentName->value("");
    UI.OutputInterfaceName->value("");
    UI.OutputCommandName->value("");
    UI.OutputArgumentName->value("");

    DISABLE_UI(BrowserSelectedSignals);
    DISABLE_UI(OutputProcessName);
    DISABLE_UI(OutputComponentName);
    DISABLE_UI(OutputInterfaceName);
    DISABLE_UI(OutputCommandName);
    DISABLE_UI(OutputArgumentName);

    DISABLE_UI(ButtonRemove);
    DISABLE_UI(ButtonRemoveAll);
    DISABLE_UI(ButtonHide);
    DISABLE_UI(ButtonAutoscale);
    DISABLE_UI(ButtonSignalOffsetIncrease);
    DISABLE_UI(ButtonSignalOffsetDecrease);
    DISABLE_UI(SpinnerSignalOffsetControl);

    DISABLE_UI(ButtonXScaleUp);
    DISABLE_UI(ButtonXScaleDown);
    DISABLE_UI(ButtonYScaleUp);
    DISABLE_UI(ButtonYScaleDown);
    DISABLE_UI(ButtonGlobalOffsetIncrease);
    DISABLE_UI(ButtonGlobalOffsetDecrease);
    DISABLE_UI(SpinnerGlobalOffsetControl);
    
    DISABLE_UI(ButtonHold);
    DISABLE_UI(ButtonCapture);
    
    UI.OutputMaxValue->value("0.0");
    UI.OutputMinValue->value("0.0");
    UI.OutputYOffset->value("0.0");
    UI.OutputYOffsetSignal->value("0.0");

    GraphPane->redraw();
}

void GCMUITask::OnBrowserSelectedSignalsClicked(void)
{
    SignalSelected * signal = GetCurrentSignal();
    if (!signal) {
        return;
    }

    const std::string actualCommandName = signal->CommandName.substr(3, signal->CommandName.size() - 2);

    UI.OutputProcessName->value(signal->ProcessName.c_str());
    UI.OutputComponentName->value(signal->ComponentName.c_str());
    UI.OutputInterfaceName->value(signal->InterfaceName.c_str());
    UI.OutputCommandName->value(actualCommandName.c_str());
    UI.OutputArgumentName->value(signal->ArgumentName.c_str());

    UpdateButtonHide(signal->State.Show);

    // Update offset value output window
    char buf[10] = "";
    cmn_snprintf(buf, sizeof(buf), "%2.2f", GraphPane->GetYOffsetSignal(signal->MultiplotIndex));
    UI.OutputYOffsetSignal->value(buf);

    //signal->ToStream();
}

void GCMUITask::OnButtonRefreshClicked(void)
{
    FLTK_CRITICAL_SECTION {
        UpdateUI();
    }
}

void GCMUITask::VisualizeSignal(SignalSelected & newSignal)
{
    // Initialize signal states
    newSignal.State.AutoScale = true;
    newSignal.State.Show = true;
    newSignal.State.Min = -1.0;
    newSignal.State.Max = 1.0;
    newSignal.State.Offset = 0.0;

    // Add to signal browser
    AddLineToBrowser(UI.BrowserSelectedSignals, newSignal.SignalName.c_str());

    // Assign multiplot's internal index to newSignalCopy object
    SignalSelected * newSignalCopy = new SignalSelected(newSignal);

    const int lastIndex = UI.BrowserSelectedSignals->size();
    UI.BrowserSelectedSignals->data(lastIndex, (void*)newSignalCopy);

    newSignalCopy->MultiplotIndex = GraphPane->GetSignalCount();
    newSignalCopy->PlotIndex = lastIndex;

    // Determine signal color
    unsigned int signalColor = *(SignalColorSet.begin());
    ProgressBars[lastIndex - 1]->selection_color(signalColor);
    ProgressBars[lastIndex - 1]->value(1.0);
    SignalColorSet.pop_front();
    newSignalCopy->SignalColor = signalColor;

    // Add to being-plotted-signal list
    MutexSignal.Lock();
    SignalsBeingPlotted.push_back(newSignalCopy);
    MutexSignal.Unlock();

    // Initialize UI: enable UI components, fill in initial values
    // Update per-signal UI
    ENABLE_UI(BrowserSelectedSignals);
    ENABLE_UI(OutputProcessName);
    ENABLE_UI(OutputComponentName);
    ENABLE_UI(OutputInterfaceName);
    ENABLE_UI(OutputCommandName);
    ENABLE_UI(OutputArgumentName);
    ENABLE_UI(ButtonRemove);
    ENABLE_UI(ButtonRemoveAll);
    ENABLE_UI(ButtonHide);
    ENABLE_UI(ButtonAutoscale);
    ENABLE_UI(ButtonSignalOffsetIncrease);
    ENABLE_UI(ButtonSignalOffsetDecrease);
    ENABLE_UI(SpinnerSignalOffsetControl);
    // Update global UI
    ENABLE_UI(ButtonXScaleUp);
    ENABLE_UI(ButtonXScaleDown);
    ENABLE_UI(ButtonYScaleUp);
    ENABLE_UI(ButtonYScaleDown);
    ENABLE_UI(ButtonGlobalOffsetIncrease);
    ENABLE_UI(ButtonGlobalOffsetDecrease);
    ENABLE_UI(SpinnerGlobalOffsetControl);
    ENABLE_UI(ButtonHold);
    //ENABLE_UI(ButtonCapture);

    // Clear all previous history
    GraphPane->clear(true);

    // Fetch new values immediately
    newSignalCopy->SetTimeout(0);

    // If the first element is added
    UI.BrowserSelectedSignals->value(1);
    OnBrowserSelectedSignalsClicked();
}

void GCMUITask::OnButtonRemoveAllClicked(void)
{
    // Before removing all the signal registered, confirm again
    std::stringstream ss;
    ss << "Are you sure to remove all the signals registered?";
    int ret = fl_choice(ss.str().c_str(), "Cancel", "No", "Yes");
    if (ret == 0) { // Cancel
        return;
    } else if (ret == 1) { // No
        return;
    } else if (ret == 2) { // Yes
        while (UI.BrowserSelectedSignals->size() != 0) {
            //UI.BrowserSelectedSignals->value(1);
            UI.BrowserSelectedSignals->value(UI.BrowserSelectedSignals->size());
            OnButtonRemoveClicked();
        }
    }
}

void GCMUITask::OnButtonRemoveClicked(void)
{
    GCMUITask::SignalSelected * signal = GetCurrentSignal();
    if (!signal) {
        return;
    }

    // Get signal data element
    //SignalSelected * data = reinterpret_cast<SignalSelected *>(UI.BrowserSelectedSignals->data(signal->PlotIndex));
    const unsigned int plotIndex = signal->PlotIndex;
    const unsigned int multiplotIndex = signal->MultiplotIndex;
    //signal->ToStream();

    // Remove signal from signal management list and rearrange PlotIndex values
    // of the other active signals'.
    bool found = false;
    int newPlotIndex = 1;

    MutexSignal.Lock();
    std::list<SignalSelected*>::iterator it = SignalsBeingPlotted.begin();
    for (; it != SignalsBeingPlotted.end(); ++it) {
        if ((*it)->PlotIndex == plotIndex) {
            // Update UI and return signal color back to color pool
            SignalColorSet.push_front((*it)->SignalColor);
            ProgressBars[(*it)->PlotIndex - 1]->value(0.0);

            it = SignalsBeingPlotted.erase(it);
            found = true;
        }
        
        // If the last signal is deleted
        if (it == SignalsBeingPlotted.end()) {
            break;
        }

        // Update PlotIndex if necessary
        if (!found) {
            newPlotIndex++;
        } else {
            ProgressBars[(*it)->PlotIndex - 1]->value(0.0);
            // Update plot index
            (*it)->PlotIndex = newPlotIndex++;
            // Update signal color label
            ProgressBars[(*it)->PlotIndex - 1]->selection_color((*it)->SignalColor);
            ProgressBars[(*it)->PlotIndex - 1]->value(1.0);
        }
    }

    MutexSignal.Unlock();

    // Update signal selection UI
    UI.BrowserSelectedSignals->remove(plotIndex);

    // MJ: Don't release memory here; Otherwise, it crashes.
    // FLTK seems to manage (deallocate) data pointer internally.
    //delete data;

    // Remove signal from multi_plot's internal data structure as well.
    GraphPane->remove(multiplotIndex);

    // If the last signal is removed
    if (UI.BrowserSelectedSignals->size() == 0) {
        GraphPane->clear();
        FLTK_CRITICAL_SECTION {
            ResetDataVisualizerUI();
        }
    } 
    // Set the first signal to be active
    else {
        FLTK_CRITICAL_SECTION {
            UI.BrowserSelectedSignals->value(1);
            OnBrowserSelectedSignalsClicked();
        }
    }
}

void GCMUITask::OnButtonYScaleUpClicked(void)
{
    FLTK_CRITICAL_SECTION {
        GraphPane->AdjustYScale(2.0);

        GraphPane->SetAutoScale(false);
        UI.ButtonAutoscale->value(0);

        UpdateMinMaxUI();
    }
}

void GCMUITask::OnButtonYScaleDownClicked(void)
{
    FLTK_CRITICAL_SECTION {
        GraphPane->AdjustYScale(0.5);

        GraphPane->SetAutoScale(false);
        UI.ButtonAutoscale->value(0);

        UpdateMinMaxUI();
    }
}

void GCMUITask::OnButtonXScaleUpClicked(void)
{
    if (XAxisScaleFactor <= 60) {
        return;
    }

    FLTK_CRITICAL_SECTION {
        XAxisScaleFactor -= 30;
        GraphPane->set_scrolling(XAxisScaleFactor);
        GraphPane->clear(true);
    }
}

void GCMUITask::OnButtonXScaleDownClicked(void)
{
    if (XAxisScaleFactor >= 240) {
        return;
    }

    FLTK_CRITICAL_SECTION {
        XAxisScaleFactor += 30;
        GraphPane->set_scrolling(XAxisScaleFactor);
        GraphPane->clear(true);
    }
}

void GCMUITask::OnButtonGlobalOffsetIncreaseClicked(void)
{
    FLTK_CRITICAL_SECTION {
        const float delta = GetGlobalOffsetDelta();

        GraphPane->SetAutoScale(false);
        GraphPane->SetYOffsetGlobal(GraphPane->GetYOffsetGlobal() + delta);

        // Update offset value output window
        char buf[10] = "";
        cmn_snprintf(buf, sizeof(buf), "%2.2f", GraphPane->GetYOffsetGlobal());
        UI.OutputYOffset->value(buf);

        UpdateMinMaxUI();
    }
}

void GCMUITask::OnButtonGlobalOffsetDecreaseClicked(void)
{
    FLTK_CRITICAL_SECTION {
        const float delta = -GetGlobalOffsetDelta();

        GraphPane->SetAutoScale(false);
        GraphPane->SetYOffsetGlobal(GraphPane->GetYOffsetGlobal() + delta);

        // Update offset value output window
        char buf[10] = "";
        cmn_snprintf(buf, sizeof(buf), "%2.2f", GraphPane->GetYOffsetGlobal());
        UI.OutputYOffset->value(buf);

        UpdateMinMaxUI();
    }
}

void GCMUITask::OnSpinnerGlobalOffsetControlClicked(void)
{
    // NOP for now
}

void GCMUITask::OnButtonHoldClicked(void)
{
    FLTK_CRITICAL_SECTION {
        GraphPane->SetHoldDrawing(!GraphPane->GetHoldDrawing());

        if (GraphPane->GetHoldDrawing()) {
            UI.ButtonHold->label("Resume");
            // Disable all the other control buttons
            DISABLE_UI(BrowserSelectedSignals);
            DISABLE_UI(OutputProcessName);
            DISABLE_UI(OutputComponentName);
            DISABLE_UI(OutputInterfaceName);
            DISABLE_UI(OutputCommandName);
            DISABLE_UI(OutputArgumentName);
            DISABLE_UI(ButtonRemove);
            DISABLE_UI(ButtonRemoveAll);
            DISABLE_UI(ButtonHide);
            DISABLE_UI(ButtonAutoscale);
            DISABLE_UI(ButtonSignalOffsetIncrease);
            DISABLE_UI(ButtonSignalOffsetDecrease);
            DISABLE_UI(SpinnerSignalOffsetControl);
            // Update global UI
            DISABLE_UI(ButtonXScaleUp);
            DISABLE_UI(ButtonXScaleDown);
            DISABLE_UI(ButtonYScaleUp);
            DISABLE_UI(ButtonYScaleDown);
            DISABLE_UI(ButtonGlobalOffsetIncrease);
            DISABLE_UI(ButtonGlobalOffsetDecrease);
            DISABLE_UI(SpinnerGlobalOffsetControl);
            //DISABLE_UI(ButtonCapture);
        } else {
            UI.ButtonHold->label("Hold");
            // Enable all the other control buttons
            ENABLE_UI(BrowserSelectedSignals);
            ENABLE_UI(OutputProcessName);
            ENABLE_UI(OutputComponentName);
            ENABLE_UI(OutputInterfaceName);
            ENABLE_UI(OutputCommandName);
            ENABLE_UI(OutputArgumentName);
            ENABLE_UI(ButtonRemove);
            ENABLE_UI(ButtonRemoveAll);
            ENABLE_UI(ButtonHide);
            ENABLE_UI(ButtonAutoscale);
            ENABLE_UI(ButtonSignalOffsetIncrease);
            ENABLE_UI(ButtonSignalOffsetDecrease);
            ENABLE_UI(SpinnerSignalOffsetControl);
            // Update global UI
            ENABLE_UI(ButtonXScaleUp);
            ENABLE_UI(ButtonXScaleDown);
            ENABLE_UI(ButtonYScaleUp);
            ENABLE_UI(ButtonYScaleDown);
            ENABLE_UI(ButtonGlobalOffsetIncrease);
            ENABLE_UI(ButtonGlobalOffsetDecrease);
            ENABLE_UI(SpinnerGlobalOffsetControl);
            //ENABLE_UI(ButtonCapture);
        }
    }
}

void GCMUITask::OnButtonAutoScaleClicked(void)
{
    GCMUITask::SignalSelected * signal = GetCurrentSignal();
    if (!signal) {
        return;
    } else {
        if (!signal->State.Show) {
            return;
        }
    }

    char buf[10] = "";
    FLTK_CRITICAL_SECTION {
        if (UI.ButtonAutoscale->value() == 0) {
            GraphPane->SetAutoScale(false);
            UI.ButtonAutoscale->value(0);
        } else {
            GraphPane->SetAutoScale(true);
            UI.ButtonAutoscale->value(1);
            GraphPane->SetYOffsetGlobal(0.0f);

            // Update offset value output window
            cmn_snprintf(buf, sizeof(buf), "%2.2f", GraphPane->GetYOffsetGlobal());
            UI.OutputYOffset->value(buf);
        }

        // Reset per-signal offset when autoscale operates
        GraphPane->SetYOffsetSignal(signal->MultiplotIndex, 0.0f);
        cmn_snprintf(buf, sizeof(buf), "%2.2f", GraphPane->GetYOffsetSignal(signal->MultiplotIndex));

        UpdateMinMaxUI();
    }
}

void GCMUITask::OnButtonHideClicked(void)
{
    GCMUITask::SignalSelected * signal = GetCurrentSignal();
    if (!signal) {
        return;
    }

    signal->State.Show = !signal->State.Show;

    FLTK_CRITICAL_SECTION {
        GraphPane->ShowSignal(signal->MultiplotIndex, signal->State.Show);

        UpdateButtonHide(signal->State.Show);

        // If no signal is being plotted, disable global offset buttons
        bool isAnySignalShown = false;
        std::list<SignalSelected*>::const_iterator it = SignalsBeingPlotted.begin();
        for (; it != SignalsBeingPlotted.end(); ++it) {
            isAnySignalShown |= (*it)->State.Show;
        }
        if (!isAnySignalShown) {
            UI.ButtonGlobalOffsetIncrease->deactivate();
            UI.ButtonGlobalOffsetDecrease->deactivate();
            return;
        } else {
            UI.ButtonGlobalOffsetIncrease->activate();
            UI.ButtonGlobalOffsetDecrease->activate();
        }
    }
}

void GCMUITask::OnButtonSignalOffsetIncreaseClicked(void)
{
    GCMUITask::SignalSelected * signal = GetCurrentSignal();
    if (!signal) {
        return;
    } else {
        if (!signal->State.Show) {
            return;
        }

        const float delta = GetSignalOffsetDelta();
        const size_t index = signal->MultiplotIndex;

        GraphPane->SetAutoScale(false);
        GraphPane->SetYOffsetSignal(index, GraphPane->GetYOffsetSignal(index) + delta);

        // Update offset value output window
        char buf[10] = "";
        cmn_snprintf(buf, sizeof(buf), "%2.2f", GraphPane->GetYOffsetSignal(index));
        FLTK_CRITICAL_SECTION {
            UI.OutputYOffsetSignal->value(buf);

            UpdateMinMaxUI();
        }
    }
}

void GCMUITask::OnButtonSignalOffsetDecreaseClicked(void)
{
    GCMUITask::SignalSelected * signal = GetCurrentSignal();
    if (!signal) {
        return;
    } else {
        if (!signal->State.Show) {
            return;
        }

        const float delta = -GetSignalOffsetDelta();
        const size_t index = signal->MultiplotIndex;

        GraphPane->SetAutoScale(false);
        GraphPane->SetYOffsetSignal(index, GraphPane->GetYOffsetSignal(index) + delta);

        // Update offset value output window
        char buf[10] = "";
        cmn_snprintf(buf, sizeof(buf), "%2.2f", GraphPane->GetYOffsetSignal(index));
        FLTK_CRITICAL_SECTION {
            UI.OutputYOffsetSignal->value(buf);

            UpdateMinMaxUI();
        }
    }
}

void GCMUITask::OnSpinnerSignalOffsetControlClicked(void)
{
    // NOP (for now)
}

void GCMUITask::FetchCurrentValues(void)
{
#ifdef THIS_NEEDS_TO_BE_UPDATED_TO_USE_A_PROPER_COMMAND
    if (SignalsBeingPlotted.size() == 0) return;

    SignalSelected * signal;

    MutexSignal.Lock();
    std::list<SignalSelected*>::const_iterator it = SignalsBeingPlotted.begin();
    for (; it != SignalsBeingPlotted.end(); ++it) {
        // Check timeout. Fetch new values only if timer expires.
        if (!(*it)->IsExpired()) {
            continue;
        }

        // Fetch new values
        signal = *it;
        values.clear();
        GlobalComponentManager.GetValuesOfCommand(
            signal->ProcessName, signal->ComponentName, signal->InterfaceName, 
            signal->CommandName, signal->Index, values);
#if 0
        // test code
        static int a = 0;
        mtsManagerLocalInterface::ValuePair value;
        mtsManagerLocalInterface::Values valueSet;
        value.Value = sin((double) (a++)) * 2.0;
        TimeServer->RelativeToAbsolute(osaGetTime(), value.Timestamp);
        valueSet.push_back(value);
        values.push_back(valueSet);
#endif

        if (values.size() > 0) {
            // Draw graph                
            const double relativeTime = TimeServer->AbsoluteToRelative(values[0][0].Timestamp);
            SetTimeOrigin(relativeTime);
            FLTK_CRITICAL_SECTION {
                DrawGraph(values, *signal);
            }

            // Set timer
            signal->Refresh();
            signal->SetTimeout(20 * cmn_ms);
        }
    }
    MutexSignal.Unlock();

    FLTK_CRITICAL_SECTION {
        GraphPane->redraw();
    }
#endif // THIS_NEEDS_TO_BE_UPDATED_TO_USE_A_PROPER_COMMAND
}

void GCMUITask::SetTimeOrigin(const double firstTimeStamp)
{
    if (TimeOrigin == 0.0) {
        TimeOrigin = firstTimeStamp;
    }
}

double GCMUITask::GetTimeOrigin()
{
    return TimeOrigin;
}

#ifdef THIS_NEEDS_TO_BE_UPDATED_TO_USE_A_PROPER_COMMAND
void GCMUITask::DrawGraph(const mtsManagerLocalInterface::SetOfValues & values, const SignalSelected& signal)
{
    double value;
    double relativeTime;
    double timeOrigin = GetTimeOrigin();

    float x, y;

    if (values.size() == 0) {
        std::cout << "SIZE 0" << std::endl;
        return;
    }

    for (unsigned int j = 0; j < values.size(); ++j) {
        for (unsigned int i = 0; i < values[j].size(); ++i) {
            value = values[j][i].Value;
            relativeTime = TimeServer->AbsoluteToRelative(values[j][i].Timestamp) - timeOrigin;

            x = static_cast<float>(relativeTime);
            y = static_cast<float>(value);

            GraphPane->add(signal.MultiplotIndex, PLOT_POINT(x, y, signal.SignalColor));
        }
    }

    UpdateMinMaxUI();
}
#endif // THIS_NEEDS_TO_BE_UPDATED_TO_USE_A_PROPER_COMMAND

void GCMUITask::UpdateMinMaxUI(void)
{
    float Ymin, Ymax;
    
    bool isAnySignalShown = false;
    std::list<SignalSelected*>::const_iterator it = SignalsBeingPlotted.begin();
    for (; it != SignalsBeingPlotted.end(); ++it) {
        isAnySignalShown |= (*it)->State.Show;
    }
    // If all signals are hidden
    if (!isAnySignalShown) {
        Ymin = 0.0f;
        Ymax = 0.0f;
    } else {
        Ymin = GraphPane->GetYMin();
        Ymax = GraphPane->GetYMax();
    }

    if (Ymin > Ymax) {
        Ymin = Ymax = 0.0f;
    }

    // Show min/max Y values
    char buf[10] = "";
    cmn_snprintf(buf, sizeof(buf), "%2.2f", Ymin);
    FLTK_CRITICAL_SECTION {
        UI.OutputMinValue->value(buf);
        cmn_snprintf(buf, sizeof(buf), "%2.2f", Ymax);
        UI.OutputMaxValue->value(buf);
    }
}

void GCMUITask::UpdateButtonHide(const bool isShow)
{
    FLTK_CRITICAL_SECTION {
        if (isShow) {
            UI.ButtonHide->label("Hide");
        } else {
            UI.ButtonHide->label("Show");
        }
    }
}

float GCMUITask::GetGlobalOffsetDelta()
{
    float offset = (float) UI.SpinnerGlobalOffsetControl->value();
    if (offset < 0) {
        UI.SpinnerGlobalOffsetControl->value(1.0f);
        return 1.0f;
    } else if (offset >= OFFSET_MAX_VALUE) {
        UI.SpinnerGlobalOffsetControl->value(OFFSET_MAX_VALUE);
        return OFFSET_MAX_VALUE;
    }

    return offset;
}

float GCMUITask::GetSignalOffsetDelta()
{
    float offset = (float) UI.SpinnerSignalOffsetControl->value();
    if (offset < 0) {
        UI.SpinnerSignalOffsetControl->value(1.0f);
        return 1.0f;
    } else if (offset >= OFFSET_MAX_VALUE) {
        UI.SpinnerSignalOffsetControl->value(OFFSET_MAX_VALUE);
        return OFFSET_MAX_VALUE;
    }

    return offset;
}

void GCMUITask::GetArgumentInformation(const std::string & processName, 
                                       const std::string & componentName, 
                                       const std::string & providedInterfaceName, 
                                       const std::string & commandName,
                                       std::string & argumentName,
                                       std::vector<std::string> & argumentParameterNames)
{
#ifdef THIS_NEEDS_TO_BE_UPDATED_TO_USE_A_PROPER_COMMAND
    GlobalComponentManager.GetArgumentInformation(
        processName, componentName, providedInterfaceName, commandName, argumentName, argumentParameterNames);
#endif
}

GCMUITask::SignalSelected * GCMUITask::GetCurrentSignal(void) const
{
    if (UI.BrowserSelectedSignals->size() == 0 || UI.BrowserSelectedSignals->value() == 0) {
        return 0;
    }

    const int currentIndex = UI.BrowserSelectedSignals->value();
    return reinterpret_cast<SignalSelected *>(UI.BrowserSelectedSignals->data(currentIndex));
}

void GCMUITask::Log(const mtsLogMessage & log)
{
    std::string s(log.Message, log.Length);

    FLTK_CRITICAL_SECTION {
        UI.Log->move_down();
        UI.Log->insert(s.c_str());
        UI.Log->show_insert_position();
    }

    if (SystemLogFile.is_open())
        SystemLogFile << s << std::flush;
}

bool GCMUITask::UIOpened(void) const
{
    return UI.Opened;
}
