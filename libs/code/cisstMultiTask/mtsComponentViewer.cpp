/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2010-09-07

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights Reserved.

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
#include <cisstMultiTask/mtsManagerGlobal.h>
#include <cisstMultiTask/mtsManagerComponentBase.h>

void mtsComponentViewer::WriteString(osaPipeExec & pipe, const std::string & s, double timeoutInSec)
{
    WaitingForResponse = true;
    pipe.Write(s, s.length());
}

CMN_IMPLEMENT_SERVICES(mtsComponentViewer)

mtsComponentViewer::mtsComponentViewer(const std::string & name) :
    mtsTaskFromSignal(name),
    UDrawPipeConnected(false),
    UDrawResponse(""),
    ShowProxies(false),
    WaitingForResponse(false)
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
    if (UDrawPipeConnected)
        ProcessResponse();
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
#if CISST_MTS_HAS_ICE
        // Ignore proxy components, unless ShowProxies is true
        if (ShowProxies || !mtsManagerGlobal::IsProxyComponent(componentInfo.ComponentName))
#endif
        {
            mtsComponentState componentState = ManagerComponentServices->ComponentGetState(componentInfo);
            std::string buffer = GetComponentInUDrawGraphFormat(componentInfo.ProcessName, componentInfo.ComponentName, componentState);
            CMN_LOG_CLASS_RUN_VERBOSE << "Sending " << buffer << std::endl;
            WriteString(UDrawPipe, buffer);
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
#if CISST_MTS_HAS_ICE
        if (ShowProxies || (!mtsManagerGlobal::IsProxyComponent(connection.Client.ComponentName) &&
                            !mtsManagerGlobal::IsProxyComponent(connection.Server.ComponentName)))
#endif
        {
            char IDString[20];
            std::string message("graph(update([],[new_edge(\"");
            sprintf(IDString, "%d", connection.ConnectionID);
            message.append(IDString);
            message.append("\", \"CONNECTION\", [a(\"OBJECT\", \"");
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
        UDrawResponse = "";
        // Create reader thread.
        ReaderThread.Create<mtsComponentViewer, int>(this, &mtsComponentViewer::ReadFromUDrawGraph, 0);
        // Wait for initial OK
        for (int i = 0; (i < 100) && (UDrawResponse == ""); i++)
            osaSleep(0.01);
        ProcessResponse();
        if (WaitingForResponse)
            CMN_LOG_CLASS_RUN_ERROR << "Failed to receive response" << std::endl;
        /// Now initialize the system
#if CISST_MTS_HAS_ICE
        WriteString(UDrawPipe, "app_menu(create_menus([menu_entry(\"redraw\", \"Redraw graph\"), "
                                                      "menu_entry(\"showproxies\", \"Show proxies\"), "
                                                      "menu_entry(\"hideproxies\", \"Hide proxies\")]))\n");
        WriteString(UDrawPipe, "app_menu(activate_menus([\"redraw\", \"showproxies\"]))\n");
#else
        WriteString(UDrawPipe, "app_menu(create_menus([menu_entry(\"redraw\", \"Redraw graph\")]))\n");
        WriteString(UDrawPipe, "app_menu(activate_menus([\"redraw\"]))\n");
#endif
        // Default node rules (nr) could be removed -- not currently used
        WriteString(UDrawPipe, "visual(new_rules([er(\"CONNECTION\", [m([menu_entry(\"disconnect\", \"Disconnect\")])]), "
                                                 "nr(\"USER\", [m([menu_entry(\"start\", \"Start\"), "
                                                                  "menu_entry(\"stop\", \"Stop\")])])]))\n");
        WriteString(UDrawPipe, "drag_and_drop(dragging_on)\n");
        CMN_LOG_CLASS_INIT_VERBOSE << "Connected to UDraw(Graph)" << std::endl;
    }
    return UDrawPipeConnected;
}

void *mtsComponentViewer::ReadFromUDrawGraph(int)
{
    ReaderThreadFinished = false;
    while (!ReaderThreadFinished) {
        if (UDrawResponse == "") {
            // Blocking read from pipe
            UDrawResponse = UDrawPipe.ReadUntil(256, '\n');
            // Wake up Component Viewer (mtsTaskFromSignal) to process response
            PostCommandQueuedMethod();
            // Wait until component viewer signals that it is ready for new data
            ReadyToRead.Wait(3.0);  // 3 second timeout
        }
        else {
            CMN_LOG_CLASS_INIT_WARNING << "ReadFromUDrawGraph: not ready to read new responses" << std::endl;
            // Wake up Component Viewer just in case
            PostCommandQueuedMethod();
        }
    }
    return 0;
}

// This function does not handle embedded quotes (i.e., \")
void mtsComponentViewer::ParseArgs(const std::string &input, std::string &arg1, std::string &arg2)
{
    size_t pos = input.find("\"");
    if (pos == std::string::npos) return;
    size_t epos = input.find("\"", pos+1);    
    if (epos == std::string::npos) return;
    arg1 = input.substr(pos+1, epos-pos-1);
    pos = input.find("\"", epos+1);
    if (pos == std::string::npos) return;
    epos = input.find("\"", pos+1);    
    if (epos == std::string::npos) return;
    arg2 = input.substr(pos+1, epos-pos-1);
}

void mtsComponentViewer::ProcessResponse(void)
{
    if (UDrawResponse != "") {
        // Parse command and args (if present).
        std::string command;
        std::string args;
        size_t pos = UDrawResponse.find('(');
        if (pos != std::string::npos) {
            command = UDrawResponse.substr(0,pos);
            size_t epos = UDrawResponse.rfind(')');
            if (epos != std::string::npos)
                args = UDrawResponse.substr(pos+1, epos-pos-1);
        }
        else {
            pos = UDrawResponse.find_first_of("\r\n");
            if (pos != std::string::npos)
                command = UDrawResponse.substr(0,pos);
        }

        if (command == "ok")
            WaitingForResponse = false;
        else if (command == "communication_error") {
            WaitingForResponse = false;
            CMN_LOG_CLASS_RUN_WARNING << "UDrawGraph error: " << args << std::endl;
        }
        else if (command == "quit") {
            CMN_LOG_CLASS_RUN_WARNING << "Received quit command from UDrawGraph" << std::endl;
            ReaderThreadFinished = true;
            Kill();
            // mtsTaskFromSignal::Kill should wake up thread (no need to call PostCommandQueuedMethod)
        }
        else if (command == "menu_selection") {
            if (args == "\"redraw\"") {
                CMN_LOG_CLASS_RUN_VERBOSE << "Redrawing graph" << std::endl;
                SendAllInfo();
            }
#if CISST_MTS_HAS_ICE
            else if (args == "\"showproxies\"") {
                CMN_LOG_CLASS_RUN_VERBOSE << "Redrawing graph, showing proxies" << std::endl;
                ShowProxies = true;
                SendAllInfo();
                WriteString(UDrawPipe, "app_menu(activate_menus([\"redraw\", \"hideproxies\"]))\n");
            }
            else if (args == "\"hideproxies\"") {
                CMN_LOG_CLASS_RUN_VERBOSE << "Redrawing graph, hiding proxies" << std::endl;
                ShowProxies = false;
                SendAllInfo();
                WriteString(UDrawPipe, "app_menu(activate_menus([\"redraw\", \"showproxies\"]))\n");
            }
#endif
        }
        else if (command == "popup_selection_edge") {
            std::string arg1, arg2;
            ParseArgs(args, arg1, arg2);
            if (arg2 == "disconnect") {
                int connectionID;
                if (sscanf(arg1.c_str(), "%d", &connectionID) == 1) {
                    CMN_LOG_CLASS_RUN_VERBOSE << "Disconnecting connection ID " << connectionID << std::endl;
                    ManagerComponentServices->Disconnect(static_cast<ConnectionIDType>(connectionID));
                }
                else
                    CMN_LOG_CLASS_RUN_ERROR << "Could not parse connection ID from: " << arg1 << std::endl;
            }
        }
        else if (command == "popup_selection_node") {
            std::string arg1, arg2;
            ParseArgs(args, arg1, arg2);
            // Get process:component from arg1. Assumes that there is only one ':' in the string.
            std::string processName;
            std::string componentName;
            size_t pos = arg1.find(':');
            if (pos != std::string::npos) {
                processName = arg1.substr(0, pos);
                componentName = arg1.substr(pos+1);
                if (arg2 == "start") {
                    CMN_LOG_CLASS_RUN_WARNING << "Starting component " << arg1 << std::endl;
                    ManagerComponentServices->ComponentStart(processName, componentName);
                }
                else if (arg2 == "stop") {
                    CMN_LOG_CLASS_RUN_WARNING << "Stopping component " << arg1 << std::endl;
                    ManagerComponentServices->ComponentStop(processName, componentName);
                }
                else
                    CMN_LOG_CLASS_RUN_WARNING << "Selected: " << args << std::endl;
            }
            else
                CMN_LOG_CLASS_RUN_ERROR << "Could not parse component name from " << arg1 << std::endl;
        }
        UDrawResponse = "";
        ReadyToRead.Raise();
    }
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
#if CISST_MTS_HAS_ICE
            // Ignore proxy components, unless ShowProxies is true
            if (ShowProxies || !mtsManagerGlobal::IsProxyComponent(componentList[j]))
#endif
            {
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

static void MakeInterfaceList(std::string &buffer, const std::string &type, const std::vector<std::string> &list)
{
    size_t i;
    if (list.size() > 0) {
        if (buffer[buffer.size()-1] != '[')
            buffer.append(", ");
        buffer.append("submenu_entry(\"");
        buffer.append(type);
        buffer.append("\", \"");
        buffer.append(type);
        buffer.append("\", [");
        for (i = 0; i < list.size(); i++) {
            buffer.append("menu_entry(\"");
            buffer.append(type+":"+list[i]);
            buffer.append("\", \"");
            buffer.append(list[i]);
            buffer.append("\")");
            if (i < list.size()-1)
                buffer.append(", ");
        }
        buffer.append("])");
    }
}

std::string mtsComponentViewer::GetComponentInUDrawGraphFormat(const std::string &processName,
                                const std::string &componentName, const mtsComponentState &componentState) const
{
    std::string buffer("graph(update([new_node(\"");
    buffer.append(processName + ":" + componentName);
    buffer.append("\",\"");
    std::string componentType("USER");
    if (mtsManagerComponentBase::IsManagerComponentServer(componentName) ||
        mtsManagerComponentBase::IsManagerComponentClient(componentName))
        componentType = "SYSTEM";
#if CISST_MTS_HAS_ICE
    else if (ShowProxies && mtsManagerGlobal::IsProxyComponent(componentName))
        componentType = "PROXY";
#endif
    buffer.append(componentType);
    buffer.append("\",[a(\"OBJECT\",\""); 
    buffer.append(componentName);
    buffer.append("\"), a(\"INFO\", \"");
    buffer.append(processName + ":" + componentName);
    buffer.append("\"), ");
    buffer.append(GetStateInUDrawGraphFormat(componentState));
    std::vector<std::string> requiredList, providedList;
    ManagerComponentServices->GetNamesOfInterfaces(processName, componentName, requiredList, providedList);
    if ((componentType == "USER") || (requiredList.size() > 0) || (providedList.size() > 0)) {
        buffer.append(", m([");
        // Only allow Start/Stop of user components (but not this component)
        if ((componentType == "USER") && (componentName != GetName()))
            buffer.append("menu_entry(\"start\", \"Start\"), menu_entry(\"stop\", \"Stop\"), blank");
        MakeInterfaceList(buffer, "Required", requiredList);
        MakeInterfaceList(buffer, "Provided", providedList);
        buffer.append("])");
    }
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
