/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-12-19

  (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsInterfaceCommon_h
#define _mtsInterfaceCommon_h

#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>

#include <string>
#include <vector>

namespace mtsInterfaceCommon {

    //-------------------------------------------------------------------------
    //  Structures for Provided Interface Description
    //-------------------------------------------------------------------------
    // Command object definition
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

	struct CommandVoidReturnElement {
		std::string Name;
        std::string ResultPrototypeSerialized;
	};

	struct CommandWriteReturnElement {
		std::string Name;
        std::string ArgumentPrototypeSerialized;
        std::string ResultPrototypeSerialized;
	};

    // Event object definition
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
	typedef std::vector<CommandVoidReturnElement>    CommandVoidReturnVector;
	typedef std::vector<CommandWriteReturnElement>   CommandWriteReturnVector;
    typedef std::vector<EventVoidElement>            EventVoidVector;
    typedef std::vector<EventWriteElement>           EventWriteVector;

	class InterfaceProvidedDescription {
    public:
		// Interface name
        std::string InterfaceProvidedName;

		// Commands
		CommandVoidVector          CommandsVoid;
		CommandWriteVector         CommandsWrite;
		CommandReadVector          CommandsRead;
		CommandQualifiedReadVector CommandsQualifiedRead;
		CommandVoidReturnVector    CommandsVoidReturn;
		CommandWriteReturnVector   CommandsWriteReturn;

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

    class InterfaceRequiredDescription {
    public:
        // Interface name
        std::string InterfaceRequiredName;

        // Functions (i.e., command pointers)
        CommandPointerNames FunctionVoidNames;
        CommandPointerNames FunctionWriteNames;
        CommandPointerNames FunctionReadNames;
        CommandPointerNames FunctionQualifiedReadNames;
        CommandPointerNames FunctionVoidReturnNames;
        CommandPointerNames FunctionWriteReturnNames;

        // Event handlers
        EventHandlerVoidVector  EventHandlersVoid;
		EventHandlerWriteVector EventHandlersWrite;

        // "IsRequired" attribute
        bool IsRequired; // MTS_OPTIONAL or MTS_REQUIRED (of type mtsRequiredType)
    };

#ifndef SWIG
    // Following functions could be moved to cpp file, if one is created

    inline void cmnSerializeRaw(std::ostream & outputStream, const CommandVoidElement & data)
            throw (std::runtime_error)
    {
    ::cmnSerializeRaw(outputStream, data.Name);
    if (outputStream.fail())
        cmnThrow("cmnSerializeRaw(CommandVoidElement: Error occured with std::ostream::write");
    }

    inline void cmnDeSerializeRaw(std::istream & inputStream, CommandVoidElement & data)
            throw (std::runtime_error)
    {
    ::cmnDeSerializeRaw(inputStream, data.Name);
    if (inputStream.fail())
        cmnThrow("cmnDeSerializeRaw(CommandVoidElement: Error occured with std::istream::read");
    }

    inline void cmnSerializeRaw(std::ostream & outputStream, const CommandWriteElement & data)
            throw (std::runtime_error)
    {
    ::cmnSerializeRaw(outputStream, data.Name);
    ::cmnSerializeRaw(outputStream, data.ArgumentPrototypeSerialized);
    if (outputStream.fail())
        cmnThrow("cmnSerializeRaw(CommandWriteElement: Error occured with std::ostream::write");
    }

    inline void cmnDeSerializeRaw(std::istream & inputStream, CommandWriteElement & data)
            throw (std::runtime_error)
    {
    ::cmnDeSerializeRaw(inputStream, data.Name);
    ::cmnDeSerializeRaw(inputStream, data.ArgumentPrototypeSerialized);
    if (inputStream.fail())
        cmnThrow("cmnDeSerializeRaw(CommandWriteElement: Error occured with std::istream::read");
    }

    inline void cmnSerializeRaw(std::ostream & outputStream, const CommandReadElement & data)
            throw (std::runtime_error)
    {
    ::cmnSerializeRaw(outputStream, data.Name);
    ::cmnSerializeRaw(outputStream, data.ArgumentPrototypeSerialized);
    if (outputStream.fail())
        cmnThrow("cmnSerializeRaw(CommandReadElement: Error occured with std::ostream::write");
    }

    inline void cmnDeSerializeRaw(std::istream & inputStream, CommandReadElement & data)
            throw (std::runtime_error)
    {
    ::cmnDeSerializeRaw(inputStream, data.Name);
    ::cmnDeSerializeRaw(inputStream, data.ArgumentPrototypeSerialized);
    if (inputStream.fail())
        cmnThrow("cmnDeSerializeRaw(CommandReadElement: Error occured with std::istream::read");
    }

    inline void cmnSerializeRaw(std::ostream & outputStream, const CommandQualifiedReadElement & data)
            throw (std::runtime_error)
    {
    ::cmnSerializeRaw(outputStream, data.Name);
    ::cmnSerializeRaw(outputStream, data.Argument1PrototypeSerialized);
    ::cmnSerializeRaw(outputStream, data.Argument2PrototypeSerialized);
    if (outputStream.fail())
        cmnThrow("cmnSerializeRaw(CommandQualifiedReadElement: Error occured with std::ostream::write");
    }

    inline void cmnDeSerializeRaw(std::istream & inputStream, CommandQualifiedReadElement & data)
            throw (std::runtime_error)
    {
    ::cmnDeSerializeRaw(inputStream, data.Name);
    ::cmnDeSerializeRaw(inputStream, data.Argument1PrototypeSerialized);
    ::cmnDeSerializeRaw(inputStream, data.Argument2PrototypeSerialized);
    if (inputStream.fail())
        cmnThrow("cmnDeSerializeRaw(CommandQualifiedReadElement: Error occured with std::istream::read");
    }

    inline void cmnSerializeRaw(std::ostream & outputStream, const CommandVoidReturnElement & data)
            throw (std::runtime_error)
    {
    ::cmnSerializeRaw(outputStream, data.Name);
        ::cmnSerializeRaw(outputStream, data.ResultPrototypeSerialized);
        if (outputStream.fail()) {
            cmnThrow("cmnSerializeRaw(CommandVoidReturnElement: Error occured with std::ostream::write");
        }
    }

    inline void cmnDeSerializeRaw(std::istream & inputStream, CommandVoidReturnElement & data)
        throw (std::runtime_error)
    {
        ::cmnDeSerializeRaw(inputStream, data.Name);
        ::cmnDeSerializeRaw(inputStream, data.ResultPrototypeSerialized);
        if (inputStream.fail()) {
            cmnThrow("cmnDeSerializeRaw(CommandVoidReturnElement: Error occured with std::istream::read");
        }
    }

    inline void cmnSerializeRaw(std::ostream & outputStream, const CommandWriteReturnElement & data)
        throw (std::runtime_error)
    {
        ::cmnSerializeRaw(outputStream, data.Name);
        ::cmnSerializeRaw(outputStream, data.ArgumentPrototypeSerialized);
        ::cmnSerializeRaw(outputStream, data.ResultPrototypeSerialized);
        if (outputStream.fail()) {
            cmnThrow("cmnSerializeRaw(CommandWriteReturnElement: Error occured with std::ostream::write");
        }
    }

    inline void cmnDeSerializeRaw(std::istream & inputStream, CommandWriteReturnElement & data)
        throw (std::runtime_error)
    {
        ::cmnDeSerializeRaw(inputStream, data.Name);
        ::cmnDeSerializeRaw(inputStream, data.ArgumentPrototypeSerialized);
        ::cmnDeSerializeRaw(inputStream, data.ResultPrototypeSerialized);
        if (inputStream.fail()) {
            cmnThrow("cmnDeSerializeRaw(CommandWriteReturnElement: Error occured with std::istream::read");
        }
    }

    inline void cmnSerializeRaw(std::ostream & outputStream, const EventVoidElement & data)
        throw (std::runtime_error)
    {
        ::cmnSerializeRaw(outputStream, data.Name);
    if (outputStream.fail())
        cmnThrow("cmnSerializeRaw(EventVoidElement: Error occured with std::ostream::write");
    }

    inline void cmnDeSerializeRaw(std::istream & inputStream, EventVoidElement & data)
            throw (std::runtime_error)
    {
    ::cmnDeSerializeRaw(inputStream, data.Name);
    if (inputStream.fail())
        cmnThrow("cmnDeSerializeRaw(EventVoidElement: Error occured with std::istream::read");
    }

    inline void cmnSerializeRaw(std::ostream & outputStream, const EventWriteElement & data)
            throw (std::runtime_error)
    {
    ::cmnSerializeRaw(outputStream, data.Name);
    ::cmnSerializeRaw(outputStream, data.ArgumentPrototypeSerialized);
    if (outputStream.fail())
        cmnThrow("cmnSerializeRaw(EventWriteElement: Error occured with std::ostream::write");
    }

    inline void cmnDeSerializeRaw(std::istream & inputStream, EventWriteElement & data)
            throw (std::runtime_error)
    {
    ::cmnDeSerializeRaw(inputStream, data.Name);
    ::cmnDeSerializeRaw(inputStream, data.ArgumentPrototypeSerialized);
    if (inputStream.fail())
        cmnThrow("cmnDeSerializeRaw(EventWriteElement: Error occured with std::istream::read");
    }

    inline std::ostream & operator << (std::ostream & output,
                                   const InterfaceProvidedDescription & description) {
    output << "InterfaceProvided: " << description.InterfaceProvidedName
           << ", Commands Void(" << description.CommandsVoid.size()
           << ") Write(" << description.CommandsWrite.size()
           << ") Read(" << description.CommandsRead.size()
           << ") QualifiedRead(" << description.CommandsQualifiedRead.size()
               << ") VoidReturn(" << description.CommandsVoidReturn.size()
               << ") WriteReturn(" << description.CommandsWriteReturn.size()
           << "), Events Void(" << description.EventsVoid.size()
           << ") Write(" << description.EventsWrite.size() << ")" << std::endl;
    return output;
    }

    inline void cmnSerializeRaw(std::ostream & outputStream, const InterfaceProvidedDescription & description)
            throw (std::runtime_error)
    {
    ::cmnSerializeRaw(outputStream, description.InterfaceProvidedName);
    ::cmnSerializeRaw(outputStream, description.CommandsVoid);
    ::cmnSerializeRaw(outputStream, description.CommandsWrite);
    ::cmnSerializeRaw(outputStream, description.CommandsRead);
    ::cmnSerializeRaw(outputStream, description.CommandsQualifiedRead);
        ::cmnSerializeRaw(outputStream, description.CommandsVoidReturn);
        ::cmnSerializeRaw(outputStream, description.CommandsWriteReturn);
    ::cmnSerializeRaw(outputStream, description.EventsVoid);
    ::cmnSerializeRaw(outputStream, description.EventsWrite);
    if (outputStream.fail())
        cmnThrow("cmnSerializeRaw(InterfaceProvidedDescription: Error occured with std::ostream::write");
    }

    inline void cmnDeSerializeRaw(std::istream & inputStream, InterfaceProvidedDescription & description)
            throw (std::runtime_error)
    {
    ::cmnDeSerializeRaw(inputStream, description.InterfaceProvidedName);
    ::cmnDeSerializeRaw(inputStream, description.CommandsVoid);
    ::cmnDeSerializeRaw(inputStream, description.CommandsWrite);
    ::cmnDeSerializeRaw(inputStream, description.CommandsRead);
    ::cmnDeSerializeRaw(inputStream, description.CommandsQualifiedRead);
        ::cmnDeSerializeRaw(inputStream, description.CommandsVoidReturn);
        ::cmnDeSerializeRaw(inputStream, description.CommandsWriteReturn);
    ::cmnDeSerializeRaw(inputStream, description.EventsVoid);
    ::cmnDeSerializeRaw(inputStream, description.EventsWrite);
    if (inputStream.fail())
        cmnThrow("cmnDeSerializeRaw(InterfaceProvidedDescription: Error occured with std::istream::read");
    }

    inline std::ostream & operator << (std::ostream & output,
                                   const InterfaceRequiredDescription & description) {
    output << "InterfaceRequired: " << description.InterfaceRequiredName
           << ", Functions Void(" << description.FunctionVoidNames.size()
           << ") Write(" << description.FunctionWriteNames.size()
           << ") Read(" << description.FunctionReadNames.size()
           << ") QualifiedRead(" << description.FunctionQualifiedReadNames.size()
               << ") VoidReturn(" << description.FunctionVoidReturnNames.size()
               << ") WriteReturn(" << description.FunctionWriteReturnNames.size()
           << "), Event Handlers Void(" << description.EventHandlersVoid.size()
           << ") Write(" << description.EventHandlersWrite.size() << ")" << std::endl;
    return output;
    }

    inline void cmnSerializeRaw(std::ostream & outputStream, const InterfaceRequiredDescription & description)
            throw (std::runtime_error)
    {
    ::cmnSerializeRaw(outputStream, description.InterfaceRequiredName);
    ::cmnSerializeRaw(outputStream, description.FunctionVoidNames);
    ::cmnSerializeRaw(outputStream, description.FunctionWriteNames);
    ::cmnSerializeRaw(outputStream, description.FunctionReadNames);
    ::cmnSerializeRaw(outputStream, description.FunctionQualifiedReadNames);
        ::cmnSerializeRaw(outputStream, description.FunctionVoidReturnNames);
        ::cmnSerializeRaw(outputStream, description.FunctionWriteReturnNames);
    ::cmnSerializeRaw(outputStream, description.EventHandlersVoid);
    ::cmnSerializeRaw(outputStream, description.EventHandlersWrite);
    ::cmnSerializeRaw(outputStream, description.IsRequired);
    if (outputStream.fail())
        cmnThrow("cmnSerializeRaw(InterfaceRequiredDescription: Error occured with std::ostream::write");
    }

    inline void cmnDeSerializeRaw(std::istream & inputStream, InterfaceRequiredDescription & description)
            throw (std::runtime_error)
    {
    ::cmnDeSerializeRaw(inputStream, description.InterfaceRequiredName);
    ::cmnDeSerializeRaw(inputStream, description.FunctionVoidNames);
    ::cmnDeSerializeRaw(inputStream, description.FunctionWriteNames);
    ::cmnDeSerializeRaw(inputStream, description.FunctionReadNames);
    ::cmnDeSerializeRaw(inputStream, description.FunctionQualifiedReadNames);
        ::cmnDeSerializeRaw(inputStream, description.FunctionVoidReturnNames);
        ::cmnDeSerializeRaw(inputStream, description.FunctionWriteReturnNames);
    ::cmnDeSerializeRaw(inputStream, description.EventHandlersVoid);
    ::cmnDeSerializeRaw(inputStream, description.EventHandlersWrite);
    if (inputStream.fail())
        cmnThrow("cmnDeSerializeRaw(InterfaceRequiredDescription: Error occured with std::istream::read");
    }
#endif // SWIG

}

using namespace mtsInterfaceCommon;


#endif // _mtsInterfaceCommon_h
