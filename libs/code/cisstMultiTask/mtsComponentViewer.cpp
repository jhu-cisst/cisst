/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2010-09-07

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaPipeExec.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsComponentViewer.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

void mtsComponentViewer::WriteString(osaPipeExec & pipe, const std::string & s, double timeoutInSec)
{
    WaitingForResponse = true;
    pipe.Write(s, s.length());
    WaitForResponse(timeoutInSec);
}

CMN_IMPLEMENT_SERVICES(mtsComponentViewer)

mtsComponentViewer::mtsComponentViewer(const std::string & name) :
    mtsTaskFromSignal(name),
    UDrawPipeConnected(false),
    WaitingForResponse(false),
    RedrawGraph(false)
{
    mtsInterfaceRequired * required = EnableDynamicComponentManagement();
    if (required) {
        ManagerComponentServices->AddComponentEventHandler(&mtsComponentViewer::AddComponentHandler, this);
        ManagerComponentServices->ChangeStateEventHandler(&mtsComponentViewer::ChangeStateHandler, this);
        ManagerComponentServices->AddConnectionEventHandler(&mtsComponentViewer::AddConnectionHandler, this);
        ManagerComponentServices->RemoveConnectionEventHandler(&mtsComponentViewer::RemoveConnectionHandler, this);
    } else {
        cmnThrow(std::runtime_error("mtsComponentViewer constructor: failed to enable dynamic component composition"));
    }
}

mtsComponentViewer::~mtsComponentViewer()
{
    this->Cleanup();
}

void mtsComponentViewer::Startup(void)
{
    // Try to connect to UDrawGraph viewer
    if (!UDrawPipeConnected)
        ConnectToUDrawGraph();
    if (UDrawPipeConnected)
        SendAllInfo();
}

void mtsComponentViewer::Run(void)
{
    if (!UDrawPipeConnected) {
       ConnectToUDrawGraph();
       if (UDrawPipeConnected) {
           CMN_LOG_CLASS_INIT_VERBOSE << "Run: Sending all info" << std::endl;
           SendAllInfo();
       }
    }
    ProcessQueuedCommands();
    ProcessQueuedEvents();
    // PK TEMP: following should be replaced by a queued command
    if (RedrawGraph) {
        RedrawGraph = false;
        SendAllInfo();
    }
}

void mtsComponentViewer::Cleanup(void)
{
    // TEMP: do this here because otherwise it doesn't work (maybe CleanupInternal
    // takes too long to change the state)
    ChangeState(mtsComponentState::FINISHED);
    if (UDrawPipeConnected) {
        if (!ReaderThreadFinished) {
            ReaderThreadFinished = true;
            CMN_LOG_CLASS_RUN_VERBOSE << "Waiting for reader thread to finish" << std::endl;
            ReaderThread.Wait();
        }
        ReaderThread.Delete();
        UDrawPipe.Close();
        UDrawPipeConnected = false;
        CMN_LOG_CLASS_RUN_WARNING << "Exited" << std::endl;
    }
}

//*************************************** Event Handlers ******************************************************

void mtsComponentViewer::AddComponentHandler(const mtsDescriptionComponent &componentInfo)
{
    if (UDrawPipeConnected) {
        // Ignore proxy components
        if (!IsProxyComponent(componentInfo.ComponentName)) {
            mtsComponentState componentState = ManagerComponentServices->ComponentGetState(componentInfo);
            std::string buffer = GetComponentInUDrawGraphFormat(componentInfo.ProcessName, componentInfo.ComponentName, componentState);
            if (buffer != "") {
                CMN_LOG_CLASS_RUN_VERBOSE << "Sending " << buffer << std::endl;
                WriteString(UDrawPipe, buffer);
            }
        }
    }
}

void mtsComponentViewer::ChangeStateHandler(const mtsComponentStateChange &componentStateChange)
{
    if (UDrawPipeConnected) {
        std::string buffer("graph(change_attr([node(\"");
        buffer.append(componentStateChange.ProcessName + ":" + componentStateChange.ComponentName);
        buffer.append("\", [");
        buffer.append(GetStateInUDrawGraphFormat(componentStateChange.NewState));
        buffer.append("])]))\n");
        CMN_LOG_CLASS_RUN_VERBOSE << "Sending " << buffer << std::endl;
        WriteString(UDrawPipe, buffer);
    }
}

void mtsComponentViewer::AddConnectionHandler(const mtsDescriptionConnection &connection)
{
    if (UDrawPipeConnected) {
        char IDString[20];
        std::string message("graph(update([],[new_edge(\"");
        sprintf(IDString, "%d", connection.ConnectionID);
        message.append(IDString);
        message.append("\", \"C\", [a(\"OBJECT\", \"");
        message.append(IDString);
        message.append("\"), a(\"INFO\", \"");
        message.append(connection.Client.InterfaceName + "<->" + connection.Server.InterfaceName);
        message.append("\")], \"");
        message.append(connection.Client.ProcessName + ":" + connection.Client.ComponentName);
        message.append("\", \"");
        message.append(connection.Server.ProcessName + ":" + connection.Server.ComponentName);
        message.append("\")]))\n");
        CMN_LOG_CLASS_INIT_VERBOSE << "Sending " << message << std::endl;
        WriteString(UDrawPipe, message);
    }
}

void mtsComponentViewer::RemoveConnectionHandler(const mtsDescriptionConnection &connection)
{
    if (UDrawPipeConnected) {
        char IDString[20];
        std::string message("graph(update([],[delete_edge(\"");
        sprintf(IDString, "%d", connection.ConnectionID);
        message.append(IDString);
        message.append("\")]))\n");
        CMN_LOG_CLASS_INIT_VERBOSE << "Sending " << message << std::endl;
        WriteString(UDrawPipe, message);
    }
}

//********************************* Local (protected) methods ***********************************************

bool mtsComponentViewer::ConnectToUDrawGraph(void)
{
    // Try to connect to UDrawGraph
    std::vector<std::string> arguments;
    arguments.push_back("-pipe");
    UDrawPipeConnected = UDrawPipe.Open("uDrawGraph", arguments, "rw");
    // wait for initial OK
    if (UDrawPipeConnected) {
        WaitingForResponse = true;
        // Create reader thread.
        ReaderThread.Create<mtsComponentViewer, int>(this, &mtsComponentViewer::ReadFromUDrawGraph, 0);
        WaitForResponse(1.0);
        WriteString(UDrawPipe, "app_menu(create_menus([menu_entry(\"redraw\", \"Redraw graph\")]))\n");
        WriteString(UDrawPipe, "app_menu(activate_menus([\"redraw\"]))\n");
        WriteString(UDrawPipe, "drag_and_drop(dragging_on)\n");
        CMN_LOG_CLASS_INIT_VERBOSE << "Connected to UDraw(Graph)" << std::endl;
    }
    return UDrawPipeConnected;
}

void *mtsComponentViewer::ReadFromUDrawGraph(int)
{
    std::string response;
    ReaderThreadFinished = false;
    while (!ReaderThreadFinished) {
        response = UDrawPipe.ReadUntil(256, '\n');
        if (response.compare(0,2,"ok") == 0)
            WaitingForResponse = false;
        else if (response.compare(0, 19, "communication_error") == 0) {
            WaitingForResponse = false;
            CMN_LOG_CLASS_RUN_WARNING << "UDrawGraph error: " << response;
        }
        else if (response.compare(0,4,"quit") == 0) {
            CMN_LOG_CLASS_RUN_WARNING << "Received quit command from UDrawGraph" << std::endl;
            ReaderThreadFinished = true;
            Kill();
            // mtsTaskFromSignal::Kill should wake up thread (no need to call PostCommandQueuedMethod)
        }
        else if (response.compare(0,24,"menu_selection(\"redraw\")") == 0) {
            CMN_LOG_CLASS_RUN_VERBOSE << "Redrawing graph" << std::endl;
            // PK TEMP: Following to be replaced by use of command pattern
            RedrawGraph = true;
            PostCommandQueuedMethod();
        }
    }
    return 0;
}

bool mtsComponentViewer::WaitForResponse(double timeoutInSec) const
{
    // Could instead use osaThreadSignal with timeout
    for (int i = 0; (i < timeoutInSec*1000) && WaitingForResponse; i++)
        osaSleep(0.001);
    if (WaitingForResponse)
        CMN_LOG_CLASS_RUN_ERROR << "Failed to receive response" << std::endl;
    return !WaitingForResponse;
}

bool mtsComponentViewer::IsProxyComponent(const std::string & componentName) const
{
    // PK: Need to fix this to be more robust
    // MJ: This method can be replaced with mtsManagerGlobal::IsProxyComponent()
    return (componentName.find("Proxy", componentName.length()-5) != std::string::npos);
}

void mtsComponentViewer::SendAllInfo(void)
{
    // Clear any existing graph
    WriteString(UDrawPipe, "graph(new([]))\n");
    // Now, send all existing components and connections
    std::vector<std::string> processList;
    std::vector<std::string> componentList;
    size_t i, j;  // could use iterators instead
    processList = ManagerComponentServices->GetNamesOfProcesses();
    for (i = 0; i < processList.size(); i++) {
        componentList = ManagerComponentServices->GetNamesOfComponents(processList[i]);
        for (j = 0; j < componentList.size(); j++) {
            // Ignore proxy components
            if (!IsProxyComponent(componentList[j])) {
                mtsDescriptionComponent arg;
                arg.ProcessName = processList[i];
                arg.ComponentName = componentList[j];
                this->AddComponentHandler(arg);
            }
        }
    }
    std::vector<mtsDescriptionConnection> connectionList;
    connectionList = ManagerComponentServices->GetListOfConnections();
    for (i = 0; i < connectionList.size(); i++)
        this->AddConnectionHandler(connectionList[i]);
    WriteString(UDrawPipe, "menu(layout(improve_all))\n");
}

std::string mtsComponentViewer::GetComponentInGraphFormat(const std::string &processName,
                                                     const std::string &componentName) const
{
    size_t i;
    std::vector<std::string> requiredList;
    std::vector<std::string> providedList;
    ManagerComponentServices->GetNamesOfInterfaces(processName, componentName, requiredList, providedList);
    // For now, ignore components that don't have any interfaces
    if ((requiredList.size() == 0) && (providedList.size() == 0))
        return "";
    std::string buffer;
    buffer = "add taska [[" + processName + ":" + componentName + "],[";
    for (i = 0; i < requiredList.size(); i++) {
        buffer += requiredList[i];
        if (i < requiredList.size()-1)
            buffer += ",";
    }
    buffer += "],[";
    for (i = 0; i < providedList.size(); i++) {
        buffer += providedList[i];
        if (i < providedList.size()-1)
            buffer += ",";
    }
    buffer += "]]\n";
    return buffer;
}

std::string mtsComponentViewer::GetComponentInUDrawGraphFormat(const std::string &processName,
                                const std::string &componentName, const mtsComponentState &componentState) const
{
#if 0
    // Enable this to ignore components that don't have any interfaces
    std::vector<std::string> requiredList;
    std::vector<std::string> providedList;
    ManagerComponentServices->GetNamesOfInterfaces(processName, componentName, requiredList, providedList);
    if ((requiredList.size() == 0) && (providedList.size() == 0))
        return "";
#endif
    std::string buffer("graph(update([new_node(\"");
    buffer.append(processName + ":" + componentName);
    buffer.append("\",\"B\",[a(\"OBJECT\",\""); 
    buffer.append(componentName);
    buffer.append("\"), a(\"INFO\", \"");
    buffer.append(processName + ":" + componentName);
    buffer.append("\"), ");
    buffer.append(GetStateInUDrawGraphFormat(componentState));
    buffer.append("])],[]))\n");
    return buffer;
}

std::string mtsComponentViewer::GetStateInUDrawGraphFormat(const mtsComponentState &componentState) const
{
    std::string buffer("a(\"COLOR\", \"");
    if (componentState == mtsComponentState::READY)
        buffer.append("yellow");
    else if (componentState == mtsComponentState::ACTIVE)
        buffer.append("green");
    else
        buffer.append("red");
    buffer.append("\")");
    return buffer;
}
