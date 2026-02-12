/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Peter Kazanzides
  Created on: 2010-09-07

  (C) Copyright 2010-2016 Johns Hopkins University (JHU), All Rights Reserved.

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

void mtsComponentViewer::WriteString(osaPipeExec & pipe, const std::string & s, double CMN_UNUSED(timeoutInSec))
{
    WaitingForResponse = true;
    pipe.Write(s, s.length());
}

mtsComponentViewer::mtsComponentViewer(const std::string & name) :
    mtsTaskFromSignal(name),
    UDrawPipeConnected(false),
    UDrawResponse(""),
    ConnectionStarted(false),
    WaitingForResponse(false)
{
    SetInitializationDelay(30.0);  // Allow up to 30 seconds for it to start
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
}

void mtsComponentViewer::Run(void)
{
    if (!UDrawPipeConnected) {
       ConnectToUDrawGraph();
       if (UDrawPipeConnected) {
           CMN_LOG_CLASS_INIT_VERBOSE << "Run: Sending all info" << std::endl;
           // Should flush all events before calling SendAllInfo
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
        {
            if (connection.Server.InterfaceName != mtsManagerComponentBase::InterfaceNames::InterfaceInternalProvided) {
                std::string buffer = GetConnectionInUDrawGraphFormat(connection);
                CMN_LOG_CLASS_RUN_VERBOSE << "Sending " << buffer << std::endl;
                WriteString(UDrawPipe, buffer);
            }
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
        if (WaitingForResponse) {
            CMN_LOG_CLASS_RUN_ERROR << "ConnectToUDrawGraph: Failed to receive response" << std::endl;
        }

        /// Now initialize the system
        WriteString(UDrawPipe, "window(title(\"CISST Component Viewer\"))\n");
        WriteString(UDrawPipe, "app_menu(create_menus([menu_entry(\"redraw\", \"Redraw graph\"), "
                                                      "blank, "
                                                      "menu_entry(\"connectStart\", \"Connect Start...\"), "
                                                      "menu_entry(\"connectFinish\", \"Connect Finish\"), "
                                                      "menu_entry(\"connectCancel\", \"Connect Cancel\")]))\n");
        // Activate menu items (context-sensitive)
        ActivateMenuItems();

        // Default node rules (nr) could be removed -- not currently used
        WriteString(UDrawPipe, "visual(new_rules([er(\"CONNECTION\", [m([menu_entry(\"disconnect\", \"Disconnect\")])]), "
                                                 "nr(\"USER\", [m([menu_entry(\"start\", \"Start\"), "
                                                                  "menu_entry(\"stop\", \"Stop\")])])]))\n");
        WriteString(UDrawPipe, "drag_and_drop(dragging_on)\n");
        // Following is an example of how to create a TCL/TK window; in this case, a window with a single button "Connect"
        // that returns the message "connectPressed".
        // WriteString(UDrawPipe, "tcl(eval(\"button .b -text Connect -command {uDrawGraph tcl_answer connectPressed} ; pack .b\"))\n"); 
        CMN_LOG_CLASS_INIT_VERBOSE << "Connected to UDraw(Graph)" << std::endl;
    }
    return UDrawPipeConnected;
}

void mtsComponentViewer::ActivateMenuItems(void)
{
    std::string buffer("app_menu(activate_menus([\"redraw\", ");
    if (ConnectionStarted) {
        if ((ConnectionRequest.Client.ProcessName != "") && (ConnectionRequest.Server.ProcessName != ""))
            buffer.append("\"connectFinish\", ");
        buffer.append("\"connectCancel\"");
    }
    else
        buffer.append("\"connectStart\"");
    buffer.append("]))\n");
    WriteString(UDrawPipe, buffer);
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
            if (GetState() == mtsComponentState::ACTIVE)
                ReadyToRead.Wait(3.0);  // 3 second timeout
            else {
                CMN_LOG_CLASS_INIT_WARNING << "ReadFromUDrawGraph: waiting for ComponentViewer to become active"
                                           << ", current state = " 
                                           << GetState().HumanReadable() << std::endl;
                ReadyToRead.Wait();    // no timeout
            }
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

bool mtsComponentViewer::ParseProcessAndComponent(const std::string &input, std::string &processName,
                                                  std::string &componentName)
{
    // Get process:component from arg1. Assumes that there is only one ':' in the string.
    size_t pos = input.find(':');
    if (pos != std::string::npos) {
        processName = input.substr(0, pos);
        componentName = input.substr(pos+1);
    }
    return (pos != std::string::npos);
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
            // Remove leading and trailing quotes
            args = args.substr(1, args.size()-2);
            if (args == "redraw") {
                CMN_LOG_CLASS_RUN_VERBOSE << "Redrawing graph" << std::endl;
                SendAllInfo();
            }
            else if (args == "connectStart") {
                ConnectionRequest.Init();
                ConnectionStarted = true;
                ActivateMenuItems();
                WriteString(UDrawPipe, "window(show_status(\"Starting connection: choose required and provided interfaces\"))\n");
                CMN_LOG_CLASS_RUN_VERBOSE << "Starting connection -- choose required and provided interfaces" << std::endl;
            }
            else if (args == "connectFinish") {
                WriteString(UDrawPipe, "window(show_status(\"\"))\n");
                CMN_LOG_CLASS_RUN_VERBOSE << "Attempting connection: " << ConnectionRequest << std::endl;
                ManagerComponentServices->Connect(ConnectionRequest);
                ChangeComponentBorder(ConnectionRequest.Client.ProcessName,
                                      ConnectionRequest.Client.ComponentName, BORDER_SINGLE);
                ChangeComponentBorder(ConnectionRequest.Server.ProcessName,
                                      ConnectionRequest.Server.ComponentName, BORDER_SINGLE);
                ConnectionStarted = false;
                ConnectionRequest.Init();
                ActivateMenuItems();
            }
            else if (args == "connectCancel") {
                WriteString(UDrawPipe, "window(show_status(\"\"))\n");
                ConnectionStarted = false;
                ChangeComponentBorder(ConnectionRequest.Client.ProcessName,
                                      ConnectionRequest.Client.ComponentName, BORDER_SINGLE);
                ChangeComponentBorder(ConnectionRequest.Server.ProcessName,
                                      ConnectionRequest.Server.ComponentName, BORDER_SINGLE);
                ConnectionRequest.Init();
                ActivateMenuItems();
                CMN_LOG_CLASS_RUN_VERBOSE << "Canceling connection request" << std::endl;
            }
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
            std::string processName;
            std::string componentName;
            if (ParseProcessAndComponent(arg1, processName, componentName)) {
                if (arg2 == "start") {
                    CMN_LOG_CLASS_RUN_WARNING << "Starting component " << arg1 << std::endl;
                    ManagerComponentServices->ComponentStart(processName, componentName);
                }
                else if (arg2 == "stop") {
                    CMN_LOG_CLASS_RUN_WARNING << "Stopping component " << arg1 << std::endl;
                    ManagerComponentServices->ComponentStop(processName, componentName);
                }
                else if (arg2.compare(0, 9, "Required:") == 0) {
                    if (ConnectionStarted) {
                        ChangeComponentBorder(ConnectionRequest.Client.ProcessName,
                                              ConnectionRequest.Client.ComponentName, BORDER_SINGLE);
                        ConnectionRequest.Client.ProcessName = processName;
                        ConnectionRequest.Client.ComponentName = componentName;
                        ConnectionRequest.Client.InterfaceName = arg2.substr(9);
                        ChangeComponentBorder(ConnectionRequest.Client.ProcessName,
                                              ConnectionRequest.Client.ComponentName, BORDER_DOUBLE);
                        CMN_LOG_CLASS_RUN_VERBOSE << "Selected required interface "
                                                  << processName << ":"
                                                  << componentName << ":"
                                                  << ConnectionRequest.Client.InterfaceName << std::endl;
                        if (ConnectionRequest.Server.ProcessName != "")
                            ActivateMenuItems();
                    }
                }
                else if (arg2.compare(0, 9, "Provided:") == 0) {
                    if (ConnectionStarted) {
                        ChangeComponentBorder(ConnectionRequest.Server.ProcessName,
                                              ConnectionRequest.Server.ComponentName, BORDER_SINGLE);
                        ConnectionRequest.Server.ProcessName = processName;
                        ConnectionRequest.Server.ComponentName = componentName;
                        ConnectionRequest.Server.InterfaceName = arg2.substr(9);
                        ChangeComponentBorder(ConnectionRequest.Server.ProcessName,
                                              ConnectionRequest.Server.ComponentName, BORDER_DOUBLE);
                        CMN_LOG_CLASS_RUN_VERBOSE << "Selected provided interface "
                                                  << processName << ":"
                                                  << componentName << ":"
                                                  << ConnectionRequest.Server.InterfaceName << std::endl;
                        if (ConnectionRequest.Client.ProcessName != "")
                            ActivateMenuItems();
                    }
                }
                else
                    CMN_LOG_CLASS_RUN_WARNING << "Unhandled popup_selection_node: " << args << std::endl;
            }
            else
                CMN_LOG_CLASS_RUN_ERROR << "Could not parse component name from " << arg1 << std::endl;
        }
        else if (command == "node_selections_labels") {
            // Node (component) is selected -- no action implemented
            CMN_LOG_CLASS_RUN_VERBOSE << "Selected component(s): " << args << std::endl;
        }
        else if (command == "edge_selection_label") {
            // Edge (connection) is selected -- no action implemented
            CMN_LOG_CLASS_RUN_VERBOSE << "Selected connection: " << args << std::endl;
        }
        else if (command == "create_edge") {
            // Drag & drop -- not implemented (need a way to select interfaces)
            CMN_LOG_CLASS_RUN_WARNING << "Create connection: " << args << std::endl;
        }
        else {
            // Other actions not implemented
            CMN_LOG_CLASS_RUN_VERBOSE << "Response: " << UDrawResponse << std::endl;
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
    if (ConnectionStarted) {
        ChangeComponentBorder(ConnectionRequest.Client.ProcessName,
                              ConnectionRequest.Client.ComponentName, BORDER_DOUBLE);
        ChangeComponentBorder(ConnectionRequest.Server.ProcessName,
                              ConnectionRequest.Server.ComponentName, BORDER_DOUBLE);
    }
    WriteString(UDrawPipe, "menu(layout(improve_all))\n");
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

std::string mtsComponentViewer::GetConnectionInUDrawGraphFormat(const mtsDescriptionConnection &connection) const
{
    bool swapped = false;
    // If the MCS is the server, swap so that it becomes the first node (parent).  This ensures that it is displayed
    // as the top level node. Similarly, if the MCC is the server, swap unless the MCS is the client.
    if (mtsManagerComponentBase::IsManagerComponentServer(connection.Server.ComponentName) ||
         (mtsManagerComponentBase::IsManagerComponentClient(connection.Server.ComponentName) &&
          !mtsManagerComponentBase::IsManagerComponentServer(connection.Client.ComponentName)))
        swapped = true;
    bool SVLio = false;
    // PK TEMP: Check for SVL input/output. In the future, this should be an attribute in the "connection"
    // structure received from the GCM.
    if ((connection.Client.InterfaceName == "input") && (connection.Server.InterfaceName == "output"))
        SVLio = true;
    char IDString[20];
    std::string message("graph(update([],[new_edge(\"");
    sprintf(IDString, "%d", connection.ConnectionID);
    message.append(IDString);
    message.append("\", \"CONNECTION\", [a(\"OBJECT\", \"");
    message.append(IDString);
    message.append("\"), a(\"INFO\", \"");
    if (SVLio)
        message.append(connection.Server.InterfaceName + "<->" + connection.Client.InterfaceName);
    else
        message.append(connection.Client.InterfaceName + "<->" + connection.Server.InterfaceName);
    message.append("\")");
    if (swapped)
        message.append(", a(\"_DIR\", \"first\")");
    if (SVLio)
        message.append(", a(\"EDGEPATTERN\", \"double;solid;5;0\"), a(\"HEAD\", \"arrow\")");
    message.append("], \"");
    if (swapped || SVLio)
        message.append(connection.Server.ProcessName + ":" + connection.Server.ComponentName);
    else
        message.append(connection.Client.ProcessName + ":" + connection.Client.ComponentName);
    message.append("\", \"");
    if (swapped || SVLio)
        message.append(connection.Client.ProcessName + ":" + connection.Client.ComponentName);
    else
        message.append(connection.Server.ProcessName + ":" + connection.Server.ComponentName);
    message.append("\")]))\n");
    return message;
}

void mtsComponentViewer::ChangeComponentBorder(const std::string &processName, const std::string &componentName,
                                               BorderType border)
{
    if ((UDrawPipeConnected) && (processName != "") && (componentName != "")) {
        std::string buffer("graph(change_attr([node(\"");
        buffer.append(processName + ":" + componentName);
        buffer.append("\", [a(\"BORDER\", \"");
        if (border == BORDER_DOUBLE)
            buffer.append("double");
        else if (border == BORDER_NONE)
            buffer.append("none");
        else
            buffer.append("");  // default is single
        buffer.append("\")])]))\n");
        CMN_LOG_CLASS_RUN_VERBOSE << "Sending " << buffer << std::endl;
        WriteString(UDrawPipe, buffer);
    }
}
