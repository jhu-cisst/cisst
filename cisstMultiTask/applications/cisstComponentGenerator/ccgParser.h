/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Jonathan Kriss, Anton Deguet
  Created on: 2010

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#ifndef _ccgParser_h
#define _ccgParser_h


#include <string>
#include <ccgComponent.h>

#include <cisstCommon/cmnGenericObject.h>

enum ccgMacro {NEG_MACRO = -1,
               DECLARE_COMPONENT,
               STATE_TABLE_BEGIN,
               STATE_TABLE_DATA,
               STATE_TABLE_END,
               INTERFACE_PROVIDED_BEGIN,
               INTERFACE_PROVIDED_END,
               COMMAND_VOID,
               COMMAND_WRITE,
               COMMAND_READ,
               COMMAND_STATEREAD,
               COMMAND_QUALIFIEDREAD,
               EVENT_VOID,
               EVENT_WRITE,
               INTERFACE_REQUIRED_BEGIN,
               INTERFACE_REQUIRED_END,
               FUNCTION_VOID,
               FUNCTION_WRITE,
               FUNCTION_READ,
               FUNCTION_QUALIFIEDREAD,
               EVENT_HANDLER_WRITE,
               EVENT_HANDLER_VOID,
               EVENT_HANDLER_NONQUEUED_VOID,
               MAX_MACRO
};


enum ccgState {CCG_IDLE = 0,
               CCG_PROCESSING_MTS_COMPONENT,
               CCG_PROCESSING_MTS_STATE_TABLE,
               CCG_PROCESSING_MTS_INTERFACE_PROVIDED,
               CCG_PROCESSING_MTS_INTERFACE_REQUIRED
};


class ccgParser: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

private:
    // a string that holds the text of the file
    std::string Line;
    size_t LineNumber;

    // full name of the file being parsed
    std::string FullName;

    // an object that holds the information for the new component
    ccgComponent * Component;

    // the current state
    ccgState State;

    /******  Functions used to parse input file *********/

    ccgMacro GetMacro(int startValue);

    int ChangeState(ccgMacro macro);

    int RunState(ccgMacro macro);

    int ParseComponent(void);

    int ParseStateTableData(void);

    int ParseStateTableDataLine(void);

    int ParseInterfaceProvided(void);

    int ParseInterfaceRequired(void);

    int ParseInterfaceRequiredEnd(void);

    int ParseCommandVoid(void);

    int ParseCommandRead(void);

    int ParseCommandReadState(void);

    int ParseCommandWrite(void);

    int ParseCommandQualifiedRead(void);

    int ParseFunction(void);

    int ParseEventVoid(void);

    int ParseEventWrite(void);

    int ParseEventHandlerWrite(void);

    int ParseEventHandlerVoid(void);

    int ParseEventHandlerVoidNonQueued(void);


public:
    /** Constructor
    *    takes the name of the file to be parsed as an argument
    */
    ccgParser(const std::string & filename);

    int ParseFile(void);

    /** Generates code for the component that has been parsed
    */
    void GenerateCode(const std::string & filename);

    inline ccgComponent * GetComponent(void) {
        return Component;
    }

};

CMN_DECLARE_SERVICES_INSTANTIATION(ccgParser);

#endif // _ccgParser_h
