/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2010-01-12

  (C) Copyright 2010-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

//
// This Slice file defines an interface between a provided interface and a
// required interfaces.
//

#ifndef _mtsComponentInterfaceProxy_ICE_h
#define _mtsComponentInterfaceProxy_ICE_h

#include <Ice/Identity.ice>

module mtsComponentInterfaceProxy
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

	struct CommandVoidReturnInfo {
		string Name;
        string ResultPrototypeSerialized;
	};

	struct CommandWriteReturnInfo {
		string Name;
        string ArgumentPrototypeSerialized;
        string ResultPrototypeSerialized;
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
	sequence<CommandVoidReturnInfo>    CommandVoidReturnSequence;
	sequence<CommandWriteReturnInfo>   CommandWriteReturnSequence;
    sequence<EventVoidInfo>            EventVoidSequence;
    sequence<EventWriteInfo>           EventWriteSequence;

    //-----------------------------------------------------------------------------
	//	Function Proxy Related Definition
	//-----------------------------------------------------------------------------
    // The information about the function proxies.
    struct FunctionProxyInfo {
        string Name;
        // This id is set as a pointer to a function proxy at server side.
        // Note that type 'long' in slice is converted to ::Ice::Long which is
        // big enough to handle 64-bit numbers.
        long FunctionProxyId;
    };

    sequence<FunctionProxyInfo> FunctionProxySequence;

    struct FunctionProxyPointerSet {
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
        FunctionProxySequence FunctionVoidReturnProxies;
        FunctionProxySequence FunctionWriteReturnProxies;
    };

    //
    //  Definitions for Event Generator Proxy Pointers
    //
    struct EventGeneratorProxyElement {
        string Name;
        // This ID is set as a pointer to a event generator proxy pointer at client side.
        // Note that type 'long' in slice is converted to ::Ice::Long which is
        // big enough to handle 64-bit numbers.
        long EventGeneratorProxyId;
    };
    sequence<EventGeneratorProxyElement> EventGeneratorProxySequence;

    struct EventGeneratorProxyPointerSet {
        EventGeneratorProxySequence EventGeneratorVoidProxies;
        EventGeneratorProxySequence EventGeneratorWriteProxies;
    };

	//-----------------------------------------------------------------------------
	// Interface for Required Interface (Proxy Client)
	//-----------------------------------------------------------------------------
	interface ComponentInterfaceClient
	{
        /*! Methods for testing */
        void TestMessageFromServerToClient(string str);

        /*! Fetch function proxy pointers from a required interface proxy at
            server side. */
        ["cpp:const"] idempotent
        bool FetchFunctionProxyPointers(string requiredInterfaceName, out FunctionProxyPointerSet functionProxyPointers);

        /*! Execute command across a network */
		void ExecuteCommandVoid(long commandID, bool blocking, out byte executionResult);
        void ExecuteCommandWriteSerialized(long commandID, string argument, bool blocking, out byte executionResult);
        void ExecuteCommandReadSerialized(long commandID, out string argument, out byte executionResult);
        void ExecuteCommandQualifiedReadSerialized(long commandID, string argumentIn, out string argumentOut, out byte executionResult);
        void ExecuteCommandVoidReturnSerialized(long commandID, long resultAddress, out string result, out byte executionResult);
        void ExecuteCommandWriteReturnSerialized(long commandID, string argumentIn, long resultAddress, out string result, out byte executionResult);
	};

	//-----------------------------------------------------------------------------
	// Interface for Provided Interface (Proxy Server)
	//-----------------------------------------------------------------------------
	interface ComponentInterfaceServer
	{
        /*! Methods for testing */
        void TestMessageFromClientToServer(string str);

		/*! Called by a proxy client when it connects to a proxy server */
		bool AddClient(string connectingProxyName, int providedInterfaceProxyInstanceID, Ice::Identity ident);

        /*! Periodically called not to lose connection */
        void Refresh();

        /*! This is called by a client when it terminates. This allows a server to
            shutdown (or close) connections safely and cleanly. */
        void Shutdown();

        /*! Fetch pointers of event generator proxies from a provided interface
            proxy at server side. */
        ["cpp:const"] idempotent
        bool FetchEventGeneratorProxyPointers(
            string clientComponentName, string requiredInterfaceName,
            out EventGeneratorProxyPointerSet eventGeneratorProxyPointers);

        /*! Execute events across a network */
        void ExecuteEventVoid(long CommandID);
        void ExecuteEventWriteSerialized(long CommandID, string argument);
        void ExecuteEventReturnSerialized(long CommandID, long resultAddress, string result);
	};
};

#endif // _mtsComponentInterfaceProxy_ICE_h
