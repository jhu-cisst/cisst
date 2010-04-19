/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Min Yang Jung
  Created on: 2010-01-20
  
  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
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
        string ClientRequiredInterfaceName;
        // server side (provided interface)
        string ServerProcessName;
        string ServerComponentName;
        string ServerProvidedInterfaceName;
	};

    /*! String vector that contains names of objects (e.g. commands, event generators,
        functions, event handlers, and signals in a parameter) */
    sequence<string> NamesOfCommandsSequence;
    sequence<string> NamesOfEventGeneratorsSequence;
    sequence<string> NamesOfFunctionsSequence;
    sequence<string> NamesOfEventHandlersSequence;


    //
    // Data visualization
    //
    /*! String vector that contains names of signals in read command argument */
    sequence<string> NamesOfSignals;

    /*! osaAbsoluteTime. Please see osaTimeServer.h for original definition */
    struct AbsoluteTime {
        long sec;   // seconds
        long nsec;  // nano-seconds
    };

    /*! List of sampled values of signals */
    struct ValuePair {
        double Value;
        AbsoluteTime Timestamp;
    };
    sequence<ValuePair> Values;

    // A set of Values. To minimize data exchange between LCM and GCM,
    // several Values are packed together when trasmitted. The size of this 
    // sequence is equal to data sampling count.
    sequence<Values> SetOfValues;
    
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
    sequence<EventVoidElement>            EventVoidSequence;
    sequence<EventWriteElement>           EventWriteSequence;

    //-------------------------------------------
	//	Provided Interface
	//-------------------------------------------
	struct ProvidedInterfaceDescription {
		// Interface name
		string ProvidedInterfaceName;
		// Commands
		CommandVoidSequence          CommandsVoid;
		CommandWriteSequence         CommandsWrite;
		CommandReadSequence          CommandsRead;
		CommandQualifiedReadSequence CommandsQualifiedRead;
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

    struct RequiredInterfaceDescription {
        // Interface name
        string RequiredInterfaceName;
        // Functions (i.e., command pointers)
        CommandPointerNames FunctionVoidNames;
        CommandPointerNames FunctionWriteNames;
        CommandPointerNames FunctionReadNames;
        CommandPointerNames FunctionQualifiedReadNames;
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
        bool CreateProvidedInterfaceProxy(string serverComponentProxyName, ProvidedInterfaceDescription providedInterface);
        bool CreateRequiredInterfaceProxy(string clientComponentProxyName, RequiredInterfaceDescription requiredInterface);
        bool RemoveProvidedInterfaceProxy(string clientComponentProxyName, string providedInterfaceProxyName);
        bool RemoveRequiredInterfaceProxy(string serverComponentProxyName, string requiredInterfaceProxyName);

        //  Connection Management
        bool ConnectServerSideInterface(int userId, int providedInterfaceProxyInstanceID, ConnectionStringSet connectionStrings);
        bool ConnectClientSideInterface(int connectionID, ConnectionStringSet connectionStrings);
        int PreAllocateResources(string userName, string serverProcessName, string serverComponentName, string serverProvidedInterfaceName);

        //  Getters
        ["cpp:const"] idempotent
        bool GetProvidedInterfaceDescription(int userId, string serverComponentName, string providedInterfaceName, out ProvidedInterfaceDescription providedInterface);

        ["cpp:const"] idempotent
        bool GetRequiredInterfaceDescription(string componentName, string requiredInterfaceName, out RequiredInterfaceDescription requiredInterface);

        ["cpp:const"] idempotent
        string GetProcessName();

        ["cpp:const"] idempotent
        int GetCurrentInterfaceCount(string componentName);

        // Getters for component inspector
        ["cpp:const"] idempotent
        void GetNamesOfCommands(string componentName, string providedInterfaceName, out NamesOfCommandsSequence names);

        ["cpp:const"] idempotent
        void GetNamesOfEventGenerators(string componentName, string providedInterfaceName, out NamesOfEventGeneratorsSequence names);

        ["cpp:const"] idempotent
        void GetNamesOfFunctions(string componentName, string requiredInterfaceName, out NamesOfFunctionsSequence names);

        ["cpp:const"] idempotent
        void GetNamesOfEventHandlers(string componentName, string requiredInterfaceName, out NamesOfEventHandlersSequence names);

        ["cpp:const"] idempotent
        void GetDescriptionOfCommand(string componentName, string providedInterfaceName, string commandName, out string description);

        ["cpp:const"] idempotent
        void GetDescriptionOfEventGenerator(string componentName, string providedInterfaceName, string eventGeneratorName, out string description);

        ["cpp:const"] idempotent
        void GetDescriptionOfFunction(string componentName, string requiredInterfaceName, string functionName, out string description);

        ["cpp:const"] idempotent
        void GetDescriptionOfEventHandler(string componentName, string requiredInterfaceName, string eventHandlerName, out string description);
        
        // Getters for data visualization
        ["cpp:const"] idempotent
        void GetArgumentInformation(string componentName, string providedInterfaceName, string commandName, out string argumentName, out NamesOfSignals signalNames);

        ["cpp:const"] idempotent
        void GetValuesOfCommand(string componentName, string providedInterfaceName, string commandName, int scalarIndex, out SetOfValues signalValues);
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
        bool AddProvidedInterface(string processName, string componentName, string interfaceName, bool isProxyInterface);
        bool AddRequiredInterface(string processName, string componentName, string interfaceName, bool isProxyInterface);
        bool FindProvidedInterface(string processName, string componentName, string interfaceName);
        ["cpp:const"] idempotent
        bool FindRequiredInterface(string processName, string componentName, string interfaceName);
        bool RemoveProvidedInterface(string processName, string componentName, string interfaceName);
        bool RemoveRequiredInterface(string processName, string componentName, string interfaceName);

        // Connection Management
        int Connect(ConnectionStringSet connectionStrings, out int userId); 
        bool ConnectConfirm(int connectionSessionID);
        bool Disconnect(ConnectionStringSet connectionStrings);

        // Networking
        bool SetProvidedInterfaceProxyAccessInfo(ConnectionStringSet connectionStrings, string endpointInfo);
        bool GetProvidedInterfaceProxyAccessInfo(ConnectionStringSet connectionStrings, out string endpointInfo);
        bool InitiateConnect(int connectionID, ConnectionStringSet connectionStrings);
        bool ConnectServerSideInterfaceRequest(int connectionID, int providedInterfaceProxyInstanceId, ConnectionStringSet connectionStrings);

	};
};

#endif // _mtsManagerProxy_ICE_h

