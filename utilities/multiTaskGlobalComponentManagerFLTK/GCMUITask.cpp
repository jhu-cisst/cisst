/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: GCMUITask.cpp 952 2009-11-10 00:06:14Z auneri1 $

  Author(s):  Min Yang Jung
  Created on: 2010-02-26

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "GCMUITask.h"
#include "PopupBrowser.h"

#include <cisstCommon/cmnStrings.h>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu.H>
#include <FL/Fl_Color_Chooser.H>

/////////////////////////////////////////////////////////////////////////////
//
// TODO:
// - Replace autorefresh/refresh buttons with callback mechanism
// - Clear SignalsBeingPlotted when a process disconnects 
// - If a command being visualized is removed, visualization UI should be reset 
//   and wait for a user to choose a new command to visualize.
// - Clear the current oscilloscope screen and refresh it so that a newly selected 
//   signal can be visualized.
//
// Improvements:
// - (two types of) offset controller -> associated with the channel / for global
// - freeze (+export to file) / trigger / hold
// - higher quality oscilloscope -> support stop & zoom => requires more and more data 
//   to be collected???

// FLTK color code definition
#define RED          255.0f/255.0f, 0.0f,          0.0f
#define GREEN        0.0f,          255.0f/255.0f, 0.0f
#define YELLOW       255.0f/255.0f, 255.0f/255.0f, 0.0f
#define BLUE         0.0f,          0.0f,          255.0f/255.0f
#define FUCHSIA      255.0f/255.0f, 0.0f,          255.0f/255.0f
#define AQUA         0.0f,          255.0f/255.0f, 255.0f/255.0f
#define GRAY         192.0f/255.0f, 192.0f/255.0f, 192.0f/255.0f
#define DARK_RED     128.0f/255.0f, 0.0f,          0.0f
#define DARK_GREEN   0.0f,          128.0f/255.0f, 0.0f
#define DARK_YELLOW  128.0f/255.0f, 128.0f/255.0f, 0.0f 
#define LIGHT_PURPLE 153.0f/255.0f, 153.0f/255.0f, 204.0f/255.0f
#define DARK_PURPLE  128.0f/255.0f, 0.0f,          128.0f/255.0f
#define DARK_AQUA    0.0f,          128.0f/255.0f, 128.0f/255.0f
#define DARK_GRAY    128.0f/255.0f, 128.0f/255.0f, 128.0f/255.0f

#define MAX_ARGUMENT_PARAMETER_COUNT 12
#define STRING_CANCEL "Cancel"

#define ENABLE_UI( _uiName )  UI._uiName->activate();
#define DISABLE_UI( _uiName ) UI._uiName->deactivate();

// GCMUITask object
GCMUITask * GCMUI;


//-------------------------------------------------------------------------
//  Callback Functions
//-------------------------------------------------------------------------
// Callback invoked when menu item selected
void callbackSignalSelect(Fl_Widget * w, void * v) 
{
    if (!v) return;
    
    GCMUITask::SignalSelected * signal = reinterpret_cast<GCMUITask::SignalSelected *>(v);
    
    // If 'cancel' is selected
    if (signal->SignalName == STRING_CANCEL) {
        return;
    }

    // Get actual command name
    const std::string actualCommandName = signal->CommandName.substr(3, signal->CommandName.size() - 2);

    std::stringstream ss;
    ss << "The following signal is added for visualization:\n";
    ss << "   Signal \"" << signal->SignalName << "\" of command \"" << actualCommandName << "\"";
    fl_choice(ss.str().c_str(), "Confirm", NULL, NULL);
    std::cout << ss.str() << std::endl;

    // Switch the current focus to the data visualization tab
    GCMUI->UI.TabControl->value(GCMUI->UI.DataVisualizer);
    // Start collecting data for visualization
    GCMUI->VisualizeSignal(*signal);

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
    unsigned int i = signalNames.size();
    memset(&popupMenus[i], 0, sizeof(Fl_Menu_Item));
    popupMenus[i].text = STRING_CANCEL;
    popupMenus[i].shortcut(FL_ALT + 'c');
    popupMenus[i].callback(callbackSignalSelect);
    popupMenus[i].user_data((void*) i);
    popupMenus[i].flags = 0;
    popupMenus[i].labeltype(FL_NORMAL_LABEL);
    popupMenus[i].labelfont(FL_HELVETICA);
    popupMenus[i].labelsize(12);
    // Add end mark
    memset(&popupMenus[signalNames.size() + 1], 0, sizeof(Fl_Menu_Item));

    const Fl_Menu_Item * m = popupMenus->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
    if (m) {
        GCMUITask::SignalSelected * signal = new GCMUITask::SignalSelected;
        signal->Index = (int) m->user_data();
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
    GlobalComponentManager(globalComponentManager)
{
    GCMUI = this;
    TimeOrigin = 0.0;

    TimeServer = &mtsManagerLocal::GetInstance()->GetTimeServer();
}

void GCMUITask::Configure(const std::string & CMN_UNUSED(filename))
{
    //
    // Component Inspector
    //
    LastIndexClicked.Reset();
    CurrentIndexClicked.Reset();

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

    // HostIP
    Fl_Text_Buffer * buf = new Fl_Text_Buffer();
    UI.TextDisplayHostIP->buffer(buf);
    
    StringVector ipAddresses;
    GlobalComponentManager.GetIPAddress(ipAddresses);
    
    std::string ipString;
    if (ipAddresses.size() == 0) {
        ipString += "Failed to retrieve IP address";
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

    GraphPane = UI.GraphPane;
    GraphPane->set_scrolling(100);
    GraphPane->SetAutoScale(false);
    GraphPane->set_grid(MP_LINEAR_GRID, MP_LINEAR_GRID, true);
    GraphPane->set_grid_color(GRAY);

    for (int i=0; i<=12; ++i) {
        GraphPane->set_pointsize(i, 1.0);
        GraphPane->set_linewidth(i, 1.0);
    }

    LastUpdateTime = clock();

    ResetDataVisualizerUI();
}

void GCMUITask::Startup(void) 
{
    // make the UI visible
    UI.show(0, NULL);
}

void GCMUITask::Run(void)
{
#if 0
    if (clock() - LastUpdateTime > 20) {
        LastUpdateTime = clock();
        PlotGraph();
    }
#endif

    // Check user's input on the 'Component Inspector' tab
    CheckComponentInspectorInput();

    // Check user's input on the 'Data Visualizer' tab
    CheckDataVisualizerInput();

    // Fetch/sample current values that user has chosen to visualize.
    FetchCurrentValues();

    // TODO: Replace autorefresh/refresh buttons with callback mechanism
    if (UI.ButtonAutoRefresh->value() == 0) {
        goto ReturnWithUpdate;
    }

    // Refresh immediately
    if (UI.ButtonRefreshClicked) {
        OnButtonRefreshClicked();
        UI.ButtonRefreshClicked = false;
        return;
    }

    // Auto refresh period: 5 secs
    static int cnt = 0;
    if (++cnt < 20 * 5) {
        goto ReturnWithUpdate;
    } else {
        cnt = 0;
    }

    UpdateUI();

ReturnWithUpdate:
    if (Fl::check() == 0) {
        Kill();
    }
}

/*
#define BASIC_PLOTTING_TEST
//#define SIGNAL_CONTROL_TEST
void GCMUITask::PlotGraph(void)
{
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
    GraphPane->add(3, PLOT_POINT((float)x, value * 1.3f,  BLUE));
    GraphPane->add(4, PLOT_POINT((float)x, value * 1.4f,  FUCHSIA));
    GraphPane->add(5, PLOT_POINT((float)x, value * 1.5f,  AQUA));
    GraphPane->add(6, PLOT_POINT((float)x, value * 1.6f,  DARK_RED));
    GraphPane->add(7, PLOT_POINT((float)x, value * 1.7f,  DARK_GREEN));
    GraphPane->add(8, PLOT_POINT((float)x, value * 1.8f,  DARK_YELLOW));
    GraphPane->add(9, PLOT_POINT((float)x, value * 1.9f,  LIGHT_PURPLE));
    GraphPane->add(10, PLOT_POINT((float)x, value * 2.0f, DARK_PURPLE));
    GraphPane->add(11, PLOT_POINT((float)x, value * 2.1f, DARK_AQUA));

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
//*/

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

    // Periodically fetch process list from GCM
    StringVector names;
    GlobalComponentManager.GetNamesOfProcesses(names);
    for (StringVector::size_type i = 0; i < names.size(); ++i) {
        AddLineToBrowser(UI.BrowserProcesses, names[i].c_str());
    }

    LastIndexClicked.Reset();
}

void GCMUITask::AddLineToBrowser(Fl_Browser * browser, const char * line, const int fg, const int bg, const char style)
{
    char buf[100] = "";

    if (style == '0') {
        sprintf(buf, "@B%d@C%d@.%s", bg, fg, line);
    } else {
        sprintf(buf, "@B%d@C%d@%c@.%s", bg, fg, style, line);
    }
    browser->add(buf);
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
        if (mtsManagerLocal::IsProxyComponent(componentName)) {
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
    GlobalComponentManager.GetNamesOfProvidedInterfaces(processName, componentName, names);

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
    GlobalComponentManager.GetNamesOfRequiredInterfaces(processName, componentName, names);

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
    SET_CALLBACK_FUNCTION(ButtonAllSignalOffsetIncrease);
    SET_CALLBACK_FUNCTION(ButtonAllSignalOffsetDecrease);
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
}

void GCMUITask::ResetDataVisualizerUI(void)
{
    // TODO: smmy
    //ResetBrowserVisualizeSignals();

    for (int i = 0; i < MAX_CHANNEL_COUNT; ++i) {
        ProgressBars[i]->value(0.0);
    }

    UI.OutputProcessName->value("");
    UI.OutputComponentName->value("");
    UI.OutputInterfaceName->value("");
    UI.OutputArgumentName->value("");

    UI.OutputMaxValue->value("0.0");
    UI.OutputMinValue->value("0.0");
}

void GCMUITask::OnBrowserSelectedSignalsClicked(void)
{
    SignalSelected * signal = GetCurrentSignal();
    if (!signal) {
        return;
    }

    // Update UI
    /*
    char buf[10] = "";
    cmn_snprintf(buf, sizeof(buf), "%d", static_cast<int>(signal->State.Min) + 1);
    UI.OutputMinValue->value(buf);
    cmn_snprintf(buf, sizeof(buf), "%d", static_cast<int>(signal->State.Max) + 1);
    UI.OutputMaxValue->value(buf);
    */

    const std::string actualCommandName = signal->CommandName.substr(3, signal->CommandName.size() - 2);

    UI.OutputProcessName->value(signal->ProcessName.c_str());
    UI.OutputComponentName->value(signal->ComponentName.c_str());
    UI.OutputInterfaceName->value(signal->InterfaceName.c_str());
    UI.OutputCommandName->value(actualCommandName.c_str());
    UI.OutputArgumentName->value(signal->ArgumentName.c_str());

    UpdateButtonHide(signal->State.Show);
}

void GCMUITask::OnButtonRefreshClicked(void)
{
    UpdateUI();
}

void GCMUITask::VisualizeSignal(SignalSelected & newSignal)
{
    // Initialize signal states
    newSignal.State.AutoScale = true;
    newSignal.State.Show = true;
    newSignal.State.Min = -1.0;
    newSignal.State.Max = 1.0;
    newSignal.State.Offset = 0.0;

    // Add to UI
    AddLineToBrowser(UI.BrowserSelectedSignals, newSignal.SignalName.c_str());

    SignalSelected * newSignalCopy = new SignalSelected(newSignal);
    SignalsBeingPlotted.push_back(newSignalCopy);

    const int lastIndex = UI.BrowserSelectedSignals->size();
    UI.BrowserSelectedSignals->data(lastIndex, (void*)newSignalCopy);

    ProgressBars[lastIndex - 1]->value(1.0);
    newSignalCopy->PlotIndex = lastIndex - 1;

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
    // Update global UI
    ENABLE_UI(ButtonXScaleUp);
    ENABLE_UI(ButtonXScaleDown);
    ENABLE_UI(ButtonYScaleUp);
    ENABLE_UI(ButtonYScaleDown);
    ENABLE_UI(ButtonAllSignalOffsetIncrease);
    ENABLE_UI(ButtonAllSignalOffsetDecrease);
    ENABLE_UI(ButtonHold);
    //ENABLE_UI(ButtonCapture);

    //UI.OutputYOffset->value("0.0");

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
    GCMUITask::SignalSelected * signal = GetCurrentSignal();
    if (!signal) {
        return;
    }

    // TODO
}

void GCMUITask::OnButtonRemoveClicked(void)
{
    GCMUITask::SignalSelected * signal = GetCurrentSignal();
    if (!signal) {
        return;
    }

    // TODO
    /*
    // If there is only one signal being plotted
    if (UI.BrowserSelectedSignals->size() == 1) {
        OnButtonRemoveAllClicked();
        return;
    }

    // Get the index of the selected signal
    const int index = UI.BrowserSelectedSignals->value();
    // Remove the signal
    for (size_t i = 0; i < SignalsBeingPlotted.size(); ++i) {
        if (SignalsBeingPlotted[i]->PlotIndex == index) {
        }
    }
    UI.BrowserSelectedSignals->remove(index);

    // Set the active signal as the first signal in the list
    UI.BrowserSelectedSignals->value(1);
    */
}

void GCMUITask::OnButtonYScaleUpClicked(void)
{
    GraphPane->AdjustYScale(2.0);

    GraphPane->SetAutoScale(false);

    UpdateMinMaxUI();
}

void GCMUITask::OnButtonYScaleDownClicked(void)
{
    GraphPane->AdjustYScale(0.5);

    GraphPane->SetAutoScale(false);

    UpdateMinMaxUI();
}

void GCMUITask::OnButtonXScaleUpClicked(void)
{
    if (XAxisScaleFactor <= 60) {
        return;
    }

    XAxisScaleFactor -= 30;
    GraphPane->set_scrolling(XAxisScaleFactor);
    GraphPane->clear(true);
}

void GCMUITask::OnButtonXScaleDownClicked(void)
{
    if (XAxisScaleFactor >= 240) {
        return;
    }

    XAxisScaleFactor += 30;
    GraphPane->set_scrolling(XAxisScaleFactor);
    GraphPane->clear(true);
}

void GCMUITask::OnButtonAllSignalOffsetIncreaseClicked(void)
{
    const float delta = 1.0;

    GraphPane->SetAutoScale(false);
    GraphPane->SetYOffset(GraphPane->GetYOffset() + delta);

    // Update offset value output window
    char buf[10] = "";
    cmn_snprintf(buf, sizeof(buf), "%2.2f", GraphPane->GetYOffset());
    UI.OutputYOffset->value(buf);

    UpdateMinMaxUI();
}

void GCMUITask::OnButtonAllSignalOffsetDecreaseClicked(void)
{
    const float delta = -1.0;

    GraphPane->SetAutoScale(false);
    GraphPane->SetYOffset(GraphPane->GetYOffset() + delta);

    // Update offset value output window
    char buf[10] = "";
    cmn_snprintf(buf, sizeof(buf), "%2.2f", GraphPane->GetYOffset());
    UI.OutputYOffset->value(buf);

    UpdateMinMaxUI();
}

void GCMUITask::OnButtonHoldClicked(void)
{
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
        // Update global UI
        DISABLE_UI(ButtonXScaleUp);
        DISABLE_UI(ButtonXScaleDown);
        DISABLE_UI(ButtonYScaleUp);
        DISABLE_UI(ButtonYScaleDown);
        DISABLE_UI(ButtonAllSignalOffsetIncrease);
        DISABLE_UI(ButtonAllSignalOffsetDecrease);
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
        // Update global UI
        ENABLE_UI(ButtonXScaleUp);
        ENABLE_UI(ButtonXScaleDown);
        ENABLE_UI(ButtonYScaleUp);
        ENABLE_UI(ButtonYScaleDown);
        ENABLE_UI(ButtonAllSignalOffsetIncrease);
        ENABLE_UI(ButtonAllSignalOffsetDecrease);
        //ENABLE_UI(ButtonCapture);
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

    GraphPane->SetAutoScale(true);
    GraphPane->SetYOffset(0.0f);

    // Update offset value output window
    char buf[10] = "";
    cmn_snprintf(buf, sizeof(buf), "%2.2f", GraphPane->GetYOffset());
    UI.OutputYOffset->value(buf);

    UpdateMinMaxUI();
}

void GCMUITask::OnButtonHideClicked(void)
{
    GCMUITask::SignalSelected * signal = GetCurrentSignal();
    if (!signal) {
        return;
    }

    // If no signal is being plotted, disable global offset buttons
    bool isAnySignalShown = false;
    for (size_t i = 0; i < SignalsBeingPlotted.size(); ++i) {
        isAnySignalShown |= SignalsBeingPlotted[i]->State.Show;
    }
    if (!isAnySignalShown) {
        UI.ButtonAllSignalOffsetIncrease->deactivate();
        UI.ButtonAllSignalOffsetDecrease->deactivate();
    } else {
        UI.ButtonAllSignalOffsetIncrease->activate();
        UI.ButtonAllSignalOffsetDecrease->activate();
    }

    signal->State.Show = !signal->State.Show;

    GraphPane->ShowSignal(signal->PlotIndex, signal->State.Show);

    UpdateButtonHide(signal->State.Show);
}

void GCMUITask::OnButtonSignalOffsetIncreaseClicked(void)
{
    // TODO
    GCMUITask::SignalSelected * signal = GetCurrentSignal();
    if (!signal) {
        return;
    } else {
        if (!signal->State.Show) {
            return;
        }
    }
}

void GCMUITask::OnButtonSignalOffsetDecreaseClicked(void)
{
    // TODO
    GCMUITask::SignalSelected * signal = GetCurrentSignal();
    if (!signal) {
        return;
    } else {
        if (!signal->State.Show) {
            return;
        }
    }
}

void GCMUITask::FetchCurrentValues(void)
{
    if (SignalsBeingPlotted.size() == 0) return;

    SignalSelected * signal;
    for (unsigned int i = 0; i < SignalsBeingPlotted.size(); ++i) {
        // Check timeout. Fetch new values only if timer expires.
        if (SignalsBeingPlotted[i]->IsExpired()) {
            signal = SignalsBeingPlotted[i];

            // Fetch new values
            mtsManagerLocalInterface::SetOfValues values; 
            GlobalComponentManager.GetValuesOfCommand(
                signal->ProcessName, signal->ComponentName, signal->InterfaceName, 
                signal->CommandName, signal->Index, values);

            if (values.size() > 0) {
                // Draw graph                
                const double relativeTime = TimeServer->AbsoluteToRelative(values[0][0].Timestamp);
                SetTimeOrigin(signal->ProcessName, relativeTime);
                DrawGraph(values, signal->PlotIndex, GetTimeOrigin(signal->ProcessName));

                // Set timer
                signal->Refresh();
                signal->SetTimeout(20 * cmn_ms);
            }
        }
    }

    GraphPane->redraw();
}

void GCMUITask::SetTimeOrigin(const std::string & processName, const double firstTimeStamp)
{
    /*
    TimeOriginMapType::const_iterator it = TimeOriginMap.find(processName);
    if (it == TimeOriginMap.end()) {
        TimeOriginMap.insert(std::make_pair(processName, firstTimeStamp));
    }
    */
    if (TimeOrigin == 0.0) {
        TimeOrigin = firstTimeStamp;
    }
}

double GCMUITask::GetTimeOrigin(const std::string & processName)
{
    /*
    TimeOriginMapType::const_iterator it = TimeOriginMap.find(processName);

    return (it == TimeOriginMap.end() ? 0.0 : it->second);
    */
    return TimeOrigin;
}

void GCMUITask::DrawGraph(const mtsManagerLocalInterface::SetOfValues & values, const int plotIndex, const double timeOrigin)
{
    double value;
    double relativeTime;
    float x, y;

    for (unsigned int j = 0; j < values.size(); ++j) {
        for (unsigned int i = 0; i < values[j].size(); ++i) {
            value = values[j][i].Value;
            relativeTime = TimeServer->AbsoluteToRelative(values[j][i].Timestamp) - timeOrigin;

            x = static_cast<float>(relativeTime);
            y = static_cast<float>(value);

            if (plotIndex == 0)  GraphPane->add( 0, PLOT_POINT(x, y, RED));
            if (plotIndex == 1)  GraphPane->add( 1, PLOT_POINT(x, y, GREEN));
            if (plotIndex == 2)  GraphPane->add( 2, PLOT_POINT(x, y, YELLOW));
            if (plotIndex == 3)  GraphPane->add( 3, PLOT_POINT(x, y, BLUE));
            if (plotIndex == 4)  GraphPane->add( 4, PLOT_POINT(x, y, FUCHSIA));
            if (plotIndex == 5)  GraphPane->add( 5, PLOT_POINT(x, y, AQUA));
            if (plotIndex == 6)  GraphPane->add( 6, PLOT_POINT(x, y, DARK_RED));
            if (plotIndex == 7)  GraphPane->add( 7, PLOT_POINT(x, y, DARK_GREEN));
            if (plotIndex == 8)  GraphPane->add( 8, PLOT_POINT(x, y, DARK_YELLOW));
            if (plotIndex == 9)  GraphPane->add( 9, PLOT_POINT(x, y, LIGHT_PURPLE));
            if (plotIndex == 10) GraphPane->add(10, PLOT_POINT(x, y, DARK_PURPLE));
            if (plotIndex == 11) GraphPane->add(11, PLOT_POINT(x, y, DARK_AQUA));
        }
    }

    UpdateMinMaxUI();
}

void GCMUITask::UpdateMinMaxUI(void)
{
    float Ymin, Ymax;
    
    bool isAnySignalShown = false;
    for (size_t i = 0; i < SignalsBeingPlotted.size(); ++i) {
        isAnySignalShown |= SignalsBeingPlotted[i]->State.Show;
    }
    // If all signals are hidden
    if (!isAnySignalShown) {
        Ymin = 0.0f;
        Ymax = 0.0f;
    } else {
        Ymin = GraphPane->GetYMin();
        Ymax = GraphPane->GetYMax();
    }

    // Show min/max Y values
    char buf[10] = "";
    cmn_snprintf(buf, sizeof(buf), "%2.2f", Ymin);
    UI.OutputMinValue->value(buf);
    cmn_snprintf(buf, sizeof(buf), "%2.2f", Ymax);
    UI.OutputMaxValue->value(buf);
}

void GCMUITask::UpdateButtonHide(const bool isShow)
{
    if (isShow) {
        UI.ButtonHide->label("Hide");
    } else {
        UI.ButtonHide->label("Show");
    }
}

void GCMUITask::GetArgumentInformation(const std::string & processName, 
                                       const std::string & componentName, 
                                       const std::string & providedInterfaceName, 
                                       const std::string & commandName,
                                       std::string & argumentName,
                                       std::vector<std::string> & argumentParameterNames)
{
    GlobalComponentManager.GetArgumentInformation(
        processName, componentName, providedInterfaceName, commandName, argumentName, argumentParameterNames);
}

GCMUITask::SignalSelected * GCMUITask::GetCurrentSignal(void) const
{
    if (UI.BrowserSelectedSignals->size() == 0 || UI.BrowserSelectedSignals->value() == 0) {
        return 0;
    }

    const int currentIndex = UI.BrowserSelectedSignals->value();
    return reinterpret_cast<SignalSelected *>(UI.BrowserSelectedSignals->data(currentIndex));
}
