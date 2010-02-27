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
#include "GCM_UI.h"

CMN_IMPLEMENT_SERVICES(GCMUITask);

typedef std::vector<std::string> StringVector;

GCMUITask::GCMUITask(const std::string & taskName, const double period, 
                     mtsManagerGlobal& globalComponentManager) :
    GlobalComponentManager(globalComponentManager), 
    mtsTaskPeriodic(taskName, period, false, 5000)
{
}

void GCMUITask::Configure(const std::string & CMN_UNUSED(filename))
{
    LastIndexClicked.Reset();
    CurrentIndexClicked.Reset();

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
}

void GCMUITask::Startup(void) 
{
    // make the UI visible
    UI.show(0, NULL);
}

void GCMUITask::Run(void)
{
    // Check if there is any user's input
    CheckUserInput();

    if (UI.ButtonAutoRefresh->value() == 0) {
        goto ReturnWithUpdate;
    }

    // Refresh immediately
    if (UI.ButtonRefreshClicked) {
        UpdateUI();
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

void GCMUITask::CheckUserInput(void)
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