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
#include <cisstMultiTask/mtsComponentViewer.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

void mtsComponentViewer::WriteString(osaPipeExec & pipe, const std::string & s)
{
    pipe.Write(s, s.length());
}

CMN_IMPLEMENT_SERVICES(mtsComponentViewer)

mtsComponentViewer::mtsComponentViewer(const std::string & name, double periodicityInSeconds) :
    mtsTaskPeriodic(name, periodicityInSeconds),
    UDrawPipeConnected(false)
{
    // MJ TEMP
#if 0
    // Extend internal required interface (to Manager Component) to include event handlers
    mtsInterfaceRequired *required = AddInterfaceRequired(
        mtsManagerComponentBase::InterfaceNames::InterfaceInternalRequired);
    if (required) {
        InternalCommands.SetInterfaceRequired(required);
        InternalCommands.AddComponentEventHandler(&mtsComponentViewer::AddComponent, this);
        InternalCommands.AddConnectionEventHandler(&mtsComponentViewer::AddConnection, this);
    }
#endif

    mtsInterfaceRequired * required = EnableDynamicComponentManagement();
    if (required) {
        ManagerComponentServices->AddComponentEventHandler(&mtsComponentViewer::AddComponent, this);
        ManagerComponentServices->AddConnectionEventHandler(&mtsComponentViewer::AddConnection, this);
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
    CMN_LOG_CLASS_INIT_VERBOSE << "Startup called" << std::endl;
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
#if 0
    // Could also periodically check for connection to JGraph-based program.
    if (!JGrahSocketConnected) {
       ConnectToJGraph();
       if (JGraphSocketConnected) {
           CMN_LOG_CLASS_INIT_VERBOSE << "Run: Sending all info" << std::endl;
           SendAllInfo();
       }
    }
#endif
    ProcessQueuedCommands();
    ProcessQueuedEvents();
}

void mtsComponentViewer::Cleanup(void)
{
    if (UDrawPipeConnected) {
        UDrawPipe.Close();
        UDrawPipeConnected = false;
    }
}

//*************************************** Event Handlers ******************************************************

void mtsComponentViewer::AddComponent(const mtsDescriptionComponent &componentInfo)
{
    if (UDrawPipeConnected) {
        std::string buffer = GetComponentInUDrawGraphFormat(componentInfo.ProcessName, componentInfo.ComponentName);
        if (buffer != "") {
            CMN_LOG_CLASS_INIT_VERBOSE << "Sending " << buffer << std::endl;
            UDrawPipe.Write(buffer, static_cast<int>(buffer.length()));
            buffer = UDrawPipe.ReadUntil(256, '\n');
            if (buffer != "")
                CMN_LOG_CLASS_INIT_VERBOSE << "Received response from UDraw(Graph): " << buffer << std::endl;
        }
    }
}

void mtsComponentViewer::AddConnection(const mtsDescriptionConnection &connection)
{
    if (UDrawPipeConnected) {
        char IDString[256];
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
        UDrawPipe.Write(message, static_cast<int>(message.length()));
        std::string response = UDrawPipe.ReadUntil(256, '\n');
        if (response != "")
            CMN_LOG_CLASS_INIT_VERBOSE << "Received response from UDraw(Graph): " << response << std::endl;
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
        std::string response = UDrawPipe.ReadUntil(256, '\n');
        if (response != "") {
            CMN_LOG_CLASS_INIT_VERBOSE << "Received response from UDraw(Graph): " << response << std::endl;
            WriteString(UDrawPipe, "drag_and_drop(dragging_on)\n");
            response = UDrawPipe.ReadUntil(256, '\n');
            if (response != "")
               CMN_LOG_CLASS_INIT_VERBOSE << "Received response from UDraw(Graph), drag_and_drop: " << response << std::endl;
            WriteString(UDrawPipe, "graph(new([]))\n");
            response = UDrawPipe.ReadUntil(256, '\n');
            if (response != "")
               CMN_LOG_CLASS_INIT_VERBOSE << "Received response from UDraw(Graph), new: " << response << std::endl;
        }
        CMN_LOG_CLASS_INIT_VERBOSE << "Connected to UDraw(Graph)" << std::endl;
    }
    return UDrawPipeConnected;
}

bool mtsComponentViewer::IsProxyComponent(const std::string & componentName) const
{
    // PK: Need to fix this to be more robust
    return (componentName.find("Proxy", componentName.length()-5) != std::string::npos);
}

void mtsComponentViewer::SendAllInfo(void)
{
    // Now, send all existing components and connections
    std::vector<std::string> processList;
    std::vector<std::string> componentList;
    size_t i, j;  // could use iterators instead
    ManagerComponentServices->RequestGetNamesOfProcesses(processList);
    for (i = 0; i < processList.size(); i++) {
        componentList.clear();
        ManagerComponentServices->RequestGetNamesOfComponents(processList[i], componentList);
        for (j = 0; j < componentList.size(); j++) {
            // Ignore proxy components
            if (!IsProxyComponent(componentList[j])) {
                mtsDescriptionComponent arg;
                arg.ProcessName = processList[i];
                arg.ComponentName = componentList[j];
                this->AddComponent(arg);
            }
        }
        std::vector<mtsDescriptionConnection> connectionList;
        ManagerComponentServices->RequestGetListOfConnections(connectionList);
        for (i = 0; i < connectionList.size(); i++)
            this->AddConnection(connectionList[i]);
    }
    if (UDrawPipeConnected) {
        WriteString(UDrawPipe, "menu(layout(improve_all))\n");
        std::string response = UDrawPipe.ReadUntil(256, '\n');
        if (response != "")
           CMN_LOG_CLASS_INIT_VERBOSE << "Received response from UDraw(Graph), improve_all: " << response << std::endl;
    }
}

std::string mtsComponentViewer::GetComponentInGraphFormat(const std::string &processName,
                                                     const std::string &componentName) const
{
    size_t i;
    std::vector<std::string> requiredList;
    std::vector<std::string> providedList;
    ManagerComponentServices->RequestGetNamesOfInterfaces(processName, componentName, requiredList, providedList);
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
                                                          const std::string &componentName) const
{
#if 0
    // Enable this to ignore components that don't have any interfaces
    std::vector<std::string> requiredList;
    std::vector<std::string> providedList;
    ManagerComponentServices->RequestGetNamesOfInterfaces(processName, componentName, requiredList, providedList);
    if ((requiredList.size() == 0) && (providedList.size() == 0))
        return "";
#endif
    std::string buffer("graph(update([new_node(\"");
    buffer.append(processName + ":" + componentName);
    buffer.append("\",\"B\",[a(\"OBJECT\",\""); 
    buffer.append(componentName);
    buffer.append("\"), a(\"INFO\", \"");
    buffer.append(processName + ":" + componentName);
    buffer.append("\")])],[]))\n");
    return buffer;
}
