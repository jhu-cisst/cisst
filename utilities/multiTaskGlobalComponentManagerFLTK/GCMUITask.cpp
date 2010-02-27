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
    buf = new Fl_Text_Buffer();
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

    if (UI.ButtonRefresh->value() == 0) {
        goto UpdateUIandReturn;
    }
    
    // Auto refresh period: 5 secs
    static int cnt = 0;
    if (++cnt < 20 * 5) {
        goto UpdateUIandReturn;
    } else {
        cnt = 0;
    }

    UpdateUI();

UpdateUIandReturn:
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

            LastIndexClicked.Reset();

            // Get all component names of the selected process
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

            LastIndexClicked.ProvidedInterface = -1;
            LastIndexClicked.Command = -1;
            LastIndexClicked.EventGenerator = -1;
            LastIndexClicked.RequiredInterface = -1;
            LastIndexClicked.Function = -1;
            LastIndexClicked.EventHandler = -1;

            // Get all interface names of the selected component
            const std::string processName = StripOffFormatCharacters(UI.BrowserProcesses->text(CurrentIndexClicked.Process));

            // Check if component is a proxy one. If yes, strip format control characters from it.
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

            LastIndexClicked.Command = -1;
            LastIndexClicked.EventGenerator = -1;

            // Get all interface names of the selected component
            const std::string processName = StripOffFormatCharacters(UI.BrowserProcesses->text(CurrentIndexClicked.Process));
            const std::string componentName = StripOffFormatCharacters(UI.BrowserComponents->text(CurrentIndexClicked.Component));
            const std::string providedInterfaceName = StripOffFormatCharacters(UI.BrowserProvidedInterfaces->text(CurrentIndexClicked.ProvidedInterface));
            
            PopulateCommands(processName, componentName, providedInterfaceName);
            PopulateEventGenerators(processName, componentName, providedInterfaceName);

            LastIndexClicked.ProvidedInterface = CurrentIndexClicked.ProvidedInterface;

            return;
        }
    }

    // Check if an user clicked required interface browser
    if (CurrentIndexClicked.RequiredInterface) {
        if (LastIndexClicked.RequiredInterface != CurrentIndexClicked.RequiredInterface) {
            // clear all subsequent browsers
            UI.BrowserFunctions->clear();
            UI.BrowserEventHandlers->clear();

            LastIndexClicked.Function = -1;
            LastIndexClicked.EventHandler = -1;

            // Get all interface names of the selected component
            const std::string processName = StripOffFormatCharacters(UI.BrowserProcesses->text(CurrentIndexClicked.Process));
            const std::string componentName = StripOffFormatCharacters(UI.BrowserComponents->text(CurrentIndexClicked.Component));
            const std::string requiredInterfaceName = StripOffFormatCharacters(UI.BrowserRequiredInterfaces->text(CurrentIndexClicked.RequiredInterface));
            
            PopulateFunctions(processName, componentName, requiredInterfaceName);
            PopulateEventHandlers(processName, componentName, requiredInterfaceName);

            LastIndexClicked.RequiredInterface = CurrentIndexClicked.RequiredInterface;

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
