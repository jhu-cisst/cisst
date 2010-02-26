/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-12-19

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsInterfaceCommon_h
#define _mtsInterfaceCommon_h

#include <string>
#include <vector>

class mtsInterfaceCommon {

public:
    //-------------------------------------------------------------------------
    //  Structures for Provided Interface Description
    //-------------------------------------------------------------------------
    /*! Command object definition */
	struct CommandVoidElement {
        std::string Name;
	};

	struct CommandWriteElement {
		std::string Name;
        std::string ArgumentPrototypeSerialized;
	};

	struct CommandReadElement {
		std::string Name;
        std::string ArgumentPrototypeSerialized;
	};

	struct CommandQualifiedReadElement {
		std::string Name;
        std::string Argument1PrototypeSerialized;
        std::string Argument2PrototypeSerialized;
	};

    /* Event object definition */
	struct EventVoidElement {
		std::string Name;
	};

	struct EventWriteElement {
        std::string Name;
        std::string ArgumentPrototypeSerialized;
	};

    typedef std::vector<CommandVoidElement>          CommandVoidVector;
	typedef std::vector<CommandWriteElement>         CommandWriteVector;
	typedef std::vector<CommandReadElement>          CommandReadVector;
	typedef std::vector<CommandQualifiedReadElement> CommandQualifiedReadVector;
    typedef std::vector<EventVoidElement>            EventVoidVector;
    typedef std::vector<EventWriteElement>           EventWriteVector;

	class ProvidedInterfaceDescription {
    public:
		// Interface name
        std::string ProvidedInterfaceName;

		// Commands
		CommandVoidVector          CommandsVoid;
		CommandWriteVector         CommandsWrite;
		CommandReadVector          CommandsRead;
		CommandQualifiedReadVector CommandsQualifiedRead;

        // Events
		EventVoidVector  EventsVoid;
		EventWriteVector EventsWrite;
	};

    //-------------------------------------------------------------------------
    //  Structure for Required Interface Description
    //-------------------------------------------------------------------------
    typedef std::vector<std::string> CommandPointerNames;
    typedef CommandVoidVector  EventHandlerVoidVector;
    typedef CommandWriteVector EventHandlerWriteVector;

    class RequiredInterfaceDescription {
    public:
        // Interface name
        std::string RequiredInterfaceName;

        // Functions (i.e., command pointers)
        CommandPointerNames FunctionVoidNames;
        CommandPointerNames FunctionWriteNames;
        CommandPointerNames FunctionReadNames;
        CommandPointerNames FunctionQualifiedReadNames;

        // Event handlers
        EventHandlerVoidVector  EventHandlersVoid;
		EventHandlerWriteVector EventHandlersWrite;
    };
};

/* Typedefs for easy access to internal classes */
#define TYPEDEF( _class ) typedef mtsInterfaceCommon::_class _class;

TYPEDEF(ProvidedInterfaceDescription);
TYPEDEF(RequiredInterfaceDescription);
TYPEDEF(CommandVoidElement);
TYPEDEF(CommandWriteElement);
TYPEDEF(CommandReadElement);
TYPEDEF(CommandQualifiedReadElement);
TYPEDEF(EventVoidElement);
TYPEDEF(EventWriteElement);

TYPEDEF(CommandVoidVector);
TYPEDEF(CommandWriteVector);
TYPEDEF(CommandReadVector);
TYPEDEF(CommandQualifiedReadVector);
TYPEDEF(EventVoidVector);
TYPEDEF(EventWriteVector);

TYPEDEF(CommandPointerNames);
TYPEDEF(EventHandlerVoidVector);
TYPEDEF(EventHandlerWriteVector);

#endif // _mtsInterfaceCommon_h
