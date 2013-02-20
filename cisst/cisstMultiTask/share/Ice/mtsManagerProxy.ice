/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2010-01-20

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

//
// This Slice file defines an interface between the global component manager
// and local component managers
//

#ifndef _mtsManagerProxy_ICE_h
#define _mtsManagerProxy_ICE_h

#include <Ice/Identity.ice>

module mtsManagerProxy
{
	//-----------------------------------------------------------------------------
	//	Structure Definitions
	//-----------------------------------------------------------------------------
    /*! Set of string arguments used to connect and disconnect two interfaces */
	struct ConnectionStringSet {
        // connect request process name
        string RequestProcessName;
        // client side (required interface)
		string ClientProcessName;
        string ClientComponentName;
        string ClientInterfaceName;
        // server side (provided interface)
        string ServerProcessName;
        string ServerComponentName;
        string ServerInterfaceName;
	};

    /*! String vector that contains names of objects (e.g. commands, event generators,
        functions, event handlers, and signals in a parameter) */
    sequence<string> NamesOfCommandsSequence;
    sequence<string> NamesOfEventGeneratorsSequence;
    sequence<string> NamesOfFunctionsSequence;
    sequence<string> NamesOfEventHandlersSequence;

    //-------------------------------------------
    //  Command and Event Objects
    //-------------------------------------------
    struct CommandVoidElement {
		string Name;
	};
	struct CommandWriteElement {
		string Name;
        string ArgumentPrototypeSerialized;
	};
	struct CommandReadElement {
		string Name;
        string ArgumentPrototypeSerialized;
	};
	struct CommandQualifiedReadElement {
		string Name;
        string Argument1PrototypeSerialized;
        string Argument2PrototypeSerialized;
	};
	struct CommandVoidReturnElement {
		string Name;
        string ResultPrototypeSerialized;
	};
	struct CommandWriteReturnElement {
		string Name;
        string ArgumentPrototypeSerialized;
        string ResultPrototypeSerialized;
	};
	struct EventVoidElement {
		string Name;
	};
	struct EventWriteElement {
		string Name;
        string ArgumentPrototypeSerialized;
	};

	sequence<CommandVoidElement>          CommandVoidSequence;
	sequence<CommandWriteElement>         CommandWriteSequence;
	sequence<CommandReadElement>          CommandReadSequence;
	sequence<CommandQualifiedReadElement> CommandQualifiedReadSequence;
	sequence<CommandVoidReturnElement>    CommandVoidReturnSequence;
	sequence<CommandWriteReturnElement>   CommandWriteReturnSequence;
    sequence<EventVoidElement>            EventVoidSequence;
    sequence<EventWriteElement>           EventWriteSequence;

    //-------------------------------------------
	//	Provided Interface
	//-------------------------------------------
	struct InterfaceProvidedDescription {
		// Interface name
		string InterfaceName;
		// Commands
		CommandVoidSequence          CommandsVoid;
		CommandWriteSequence         CommandsWrite;
		CommandReadSequence          CommandsRead;
		CommandQualifiedReadSequence CommandsQualifiedRead;
		CommandVoidReturnSequence    CommandsVoidReturn;
		CommandWriteReturnSequence   CommandsWriteReturn;
        // Events
		EventVoidSequence            EventsVoid;
		EventWriteSequence           EventsWrite;
	};

    //--------------------------------------------
    //  Required Interface
    //--------------------------------------------
    sequence<string> CommandPointerNames;
    sequence<CommandVoidElement> EventHandlerVoidSequence;
    sequence<CommandWriteElement> EventHandlerWriteSequence;

    struct InterfaceRequiredDescription {
        // Interface name
        string InterfaceName;
        // Is required
        bool IsRequired;
        // Functions (i.e., command pointers)
        CommandPointerNames FunctionVoidNames;
        CommandPointerNames FunctionWriteNames;
        CommandPointerNames FunctionReadNames;
        CommandPointerNames FunctionQualifiedReadNames;
        CommandPointerNames FunctionVoidReturnNames;
        CommandPointerNames FunctionWriteReturnNames;
        // Event handlers
        EventHandlerVoidSequence  EventHandlersVoid;
		EventHandlerWriteSequence EventHandlersWrite;
    };

	interface ManagerClient
	{
        //-------------------------------------------------
        //  Test Method
        //-------------------------------------------------
        // Methods for testing
        void TestMessageFromServerToClient(string str);

        //-----------------------------------------------------------------------------
	    // Interface for Local Component Manager (Proxy Client)
        //
        // This interface implements APIs defined in mtsManagerLocalInterface.h
        // See mtsManagerLocalInterface.h for detailed comments
	    //-----------------------------------------------------------------------------
        //  Proxy Object Control (Creation, Removal)
        bool CreateComponentProxy(string componentProxyName);
        bool RemoveComponentProxy(string componentProxyName);
        bool CreateInterfaceProvidedProxy(string serverComponentProxyName, InterfaceProvidedDescription interfaceProvided);
        bool CreateInterfaceRequiredProxy(string clientComponentProxyName, InterfaceRequiredDescription interfaceRequired);
        bool RemoveInterfaceProvidedProxy(string clientComponentProxyName, string interfaceProvidedProxyName);
        bool RemoveInterfaceRequiredProxy(string serverComponentProxyName, string interfaceRequiredProxyName);

        //  Connection Management
        bool ConnectServerSideInterface(int connectionID, ConnectionStringSet connectionStrings);
        bool ConnectClientSideInterface(int connectionID, ConnectionStringSet connectionStrings);

        //  Getters
        ["cpp:const"] idempotent
        bool GetInterfaceProvidedDescription(string serverComponentName, string interfaceProvidedName, out InterfaceProvidedDescription interfaceProvided);

        ["cpp:const"] idempotent
        bool GetInterfaceRequiredDescription(string componentName, string interfaceRequiredName, out InterfaceRequiredDescription interfaceRequired);

        ["cpp:const"] idempotent
        string GetProcessName();

        // Getters for component inspector
        ["cpp:const"] idempotent
        void GetNamesOfCommands(string componentName, string interfaceProvidedName, out NamesOfCommandsSequence names);

        ["cpp:const"] idempotent
        void GetNamesOfEventGenerators(string componentName, string interfaceProvidedName, out NamesOfEventGeneratorsSequence names);

        ["cpp:const"] idempotent
        void GetNamesOfFunctions(string componentName, string interfaceRequiredName, out NamesOfFunctionsSequence names);

        ["cpp:const"] idempotent
        void GetNamesOfEventHandlers(string componentName, string interfaceRequiredName, out NamesOfEventHandlersSequence names);

        ["cpp:const"] idempotent
        void GetDescriptionOfCommand(string componentName, string interfaceProvidedName, string commandName, out string description);

        ["cpp:const"] idempotent
        void GetDescriptionOfEventGenerator(string componentName, string interfaceProvidedName, string eventGeneratorName, out string description);

        ["cpp:const"] idempotent
        void GetDescriptionOfFunction(string componentName, string interfaceRequiredName, string functionName, out string description);

        ["cpp:const"] idempotent
        void GetDescriptionOfEventHandler(string componentName, string interfaceRequiredName, string eventHandlerName, out string description);
	};

	interface ManagerServer
	{
        //-------------------------------------------------
        //  Test Method and Connection Management
        //-------------------------------------------------
        // Methods for testing
        void TestMessageFromClientToServer(string str);

	    // Called by a proxy client when it connects to a proxy server
		bool AddClient(string connectingProcessName, Ice::Identity ident);

        // Periodically called not to lose connection
        void Refresh();

        // This is called by a client when it terminates. This allows a server to
        // shutdown (or close) connections safely and cleanly
        void Shutdown();

        //-----------------------------------------------------------------------------
	    // Interface for Global Component Manager (Proxy Server)
        //
        // This interface implements APIs defined in mtsManagerGlobalInterface.h
        // See mtsManagerGlobalInterface.h for detailed comments
	    //-----------------------------------------------------------------------------
        // Process Management
        bool AddProcess(string processName);
        ["cpp:const"] idempotent
        bool FindProcess(string processName);
        bool RemoveProcess(string processName);

        // Component Management
        bool AddComponent(string processName, string componentName);
        ["cpp:const"] idempotent
        bool FindComponent(string processName, string componentName);
        bool RemoveComponent(string processName, string componentName);

        // Interface Management
        bool AddInterfaceProvidedOrOutput(string processName, string componentName, string interfaceName);
        bool AddInterfaceRequiredOrInput(string processName, string componentName, string interfaceName);
        bool FindInterfaceProvidedOrOutput(string processName, string componentName, string interfaceName);
        ["cpp:const"] idempotent
        bool FindInterfaceRequiredOrInput(string processName, string componentName, string interfaceName);
        bool RemoveInterfaceProvidedOrOutput(string processName, string componentName, string interfaceName);
        bool RemoveInterfaceRequiredOrInput(string processName, string componentName, string interfaceName);

        // Connection Management
        int Connect(ConnectionStringSet connectionStrings);
        bool ConnectConfirm(int connectionID);
        bool DisconnectWithID(int connectionID);
        bool Disconnect(ConnectionStringSet connectionStrings);

        // Networking
        bool SetInterfaceProvidedProxyAccessInfo(int connectionID, string endpointInfo);
        bool GetInterfaceProvidedProxyAccessInfo(string clientProcessName, string serverProcessName, string serverComponentName, string serverInterfaceProvidedName, out string endpointInfo);
        bool GetInterfaceProvidedProxyAccessInfoWithID(int connectionID, out string endpointInfo);
        bool InitiateConnect(int connectionID);
        bool ConnectServerSideInterfaceRequest(int connectionID);
	};
};

#endif // _mtsManagerProxy_ICE_h

