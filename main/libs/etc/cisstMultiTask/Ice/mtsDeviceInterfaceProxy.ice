/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsDeviceInterfaceProxy.ice 2009-03-16 mjung5 $
  
  Author(s):  Min Yang Jung
  Created on: 2009-04-24
  
  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

//
// This Slice file defines the communication between a provided interface
// and a required interfaces. 
// A provided interfaces act as a server while a required interface does 
// as a client.
//

#ifndef _mtsDeviceInterfaceProxy_ICE_h
#define _mtsDeviceInterfaceProxy_ICE_h

#include <Ice/Identity.ice>

module mtsDeviceInterfaceProxy
{
	//-----------------------------------------------------------------------------
	//	Command and Event Object Definition
	//-----------------------------------------------------------------------------
	struct CommandVoidInfo {
		string Name;
	};
	
	struct CommandWriteInfo { 
		string Name;
        string ArgumentPrototypeSerialized;
	};
	
	struct CommandReadInfo { 
		string Name;
        string ArgumentPrototypeSerialized;
	};
	
	struct CommandQualifiedReadInfo { 
		string Name;
        string Argument1PrototypeSerialized;
        string Argument2PrototypeSerialized;
	};
	
	struct EventVoidInfo { 
		string Name;
	};
	
	struct EventWriteInfo {
		string Name;
        string ArgumentPrototypeSerialized;
	};

	sequence<CommandVoidInfo>          CommandVoidSequence;
	sequence<CommandWriteInfo>         CommandWriteSequence;
	sequence<CommandReadInfo>          CommandReadSequence;
	sequence<CommandQualifiedReadInfo> CommandQualifiedReadSequence;
    sequence<EventVoidInfo>            EventVoidSequence;
    sequence<EventWriteInfo>           EventWriteSequence;

    //-----------------------------------------------------------------------------
	//	Provided Interface Related Definition
	//-----------------------------------------------------------------------------	
	// Data structure definition
	struct ProvidedInterfaceInfo {
		// Interface name
		string InterfaceName;
		
		// Commands
		CommandVoidSequence          CommandsVoid;
		CommandWriteSequence         CommandsWrite;
		CommandReadSequence          CommandsRead;
		CommandQualifiedReadSequence CommandsQualifiedRead;
        
        // Events
		EventVoidSequence            EventsVoid;
		EventWriteSequence           EventsWrite;
	};

    //-----------------------------------------------------------------------------
	//	Function Proxy Related Definition
	//-----------------------------------------------------------------------------	
    
    //
    // TODO: Change the name of FunctionProxyInfo => ProxyElementInfo (?)
    //
    // The information about the function proxies.
    struct FunctionProxyInfo {
        string Name;
        // This id is set as the pointer to the function proxy at server side.
        // Note that this is valid only for 32-bit OS. Under 64-bit machine, this
        // should be changed so as to be able to handle 64-bit address space.
        long FunctionProxyId;
    };

    sequence<FunctionProxyInfo> FunctionProxySequence;

    struct FunctionProxySet {
        // A name of the server task proxy. This is used as a key to find a server 
        // task proxy at client side.
        string ServerTaskProxyName;
        
        // A name of the provided interface proxy that has command proxies at client side.
        string ProvidedInterfaceProxyName;

        // Set of pointers to the function proxies.
        FunctionProxySequence FunctionVoidProxies;
        FunctionProxySequence FunctionWriteProxies;
        FunctionProxySequence FunctionReadProxies;
        FunctionProxySequence FunctionQualifiedReadProxies;
    };

    struct EventGeneratorProxyElement {
        string Name;
        // This id is set as the pointer to the function proxy at client side.
        // Note that this is valid only for 32-bit OS. Under 64-bit machine, this
        // should be changed so as to be able to handle 64-bit address space.
        long ProxyId;
    };
    sequence<EventGeneratorProxyElement> EventGeneratorProxySequence;

    // Used by GetListOfEventHandlerRegistered()
    struct ListsOfEventGeneratorsRegistered {
        EventGeneratorProxySequence EventGeneratorVoidProxies;
        EventGeneratorProxySequence EventGeneratorWriteProxies;
    };

	//-----------------------------------------------------------------------------
	// Interface for Required Interface (Proxy Client)
	//-----------------------------------------------------------------------------
	interface DeviceInterfaceClient
	{
        void ExecuteEventVoid(long CommandId);
        void ExecuteEventWriteSerialized(long CommandId, string argument);
	};

	//-----------------------------------------------------------------------------
	// Interface for Provided Interface (Proxy Server)
	//-----------------------------------------------------------------------------
	interface DeviceInterfaceServer
	{
		/*! Replacement for OnConnect event. */
		void AddClient(Ice::Identity ident);

        /*! This is called by a client when it terminates. This allows a server to
            shutdown (or close) safely and cleanly. */
        void Shutdown();

        /*! Get the information on the provided interface which will be used to 
            create a provided interface proxy at client side. */
        ["cpp:const"] idempotent 
        bool GetProvidedInterfaceInfo(string providedInterfaceName,
                                      out ProvidedInterfaceInfo info);

        /*! Send the information on the required interface that will be used to
            create a required interface proxy at the server side. Then, the server will
            create client proxies (e.g. client task proxy, required interface proxy)
            using this information.
            This information includes the serialized argument prototypes of the event 
            handlers' at the client side.
            Return false if any proxy object creation process failed. */
        bool CreateClientProxies(string userTaskName, string requiredInterfaceName,
			                     string resourceTaskName, string providedInterfaceName);

        /*! Call mtsTaskManager::Connect() at server side. */
        bool ConnectServerSide(string userTaskName, string requiredInterfaceName,
			                   string resourceTaskName, string providedInterfaceName);

        /*! Update CommandId. This updates the command id of command proxies
            at client side, which is a critical step regarding thread 
            synchronization. */
        ["cpp:const"] idempotent
        void GetCommandId(string clientTaskProxyName, out FunctionProxySet functionProxies);

        /*! Update event handler proxy objects' commandId field. This replaces default 
            value (zero) with the pointers to actual event handler object 
            (instance of either mtsFunctionVoid or mtsFunctionWrite type).
            When the server task receives the return value with updated 'functionProxies'
            object, it has to do the following two things.
            1) Iterating the list of actual event handler objects registered (used) by
            the client, the server task has to enable corresponding events.            
            2) Update event handler proxy objects' commandId field (required interface
            proxy contains all the information about event handler proxy objects). 
        */
        ["cpp:const"] idempotent
        bool UpdateEventHandlerId(string clientTaskProxyName, 
            ListsOfEventGeneratorsRegistered eventGeneratorProxies);


		/*! Execute command objects across networks. */
		// Here 'int' type is used instead of 'unsigned int' because SLICE does not
		// support unsigned type.
		// (see http://zeroc.com/doc/Ice-3.3.1/manual/Slice.5.8.html)
		// (Also see http://www.zeroc.com/doc/Ice-3.3.1/manual/Cpp.7.6.html for
		// Mapping for simple built-in types)
		void ExecuteCommandVoid(long CommandId);
        void ExecuteCommandWriteSerialized(long CommandId, string argument);
        void ExecuteCommandReadSerialized(long CommandId, out string argument);
        void ExecuteCommandQualifiedReadSerialized(long CommandId, string argument1, out string argument2);
	};

};

#endif // _mtsDeviceInterfaceProxy_ICE_h
