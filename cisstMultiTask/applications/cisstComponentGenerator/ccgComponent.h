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

#include <string>
#include <iostream>
#include <deque>

#ifndef _ccgComponent_h
#define _ccgComponent_h

/** Command object
*    holds information for all types of commands
*/
struct ccgCommand {
    std::string Type, Name, Func, Arg1, Arg2;

    ccgCommand() {}
    ccgCommand(const std::string & t, const std::string  & f,
               const std::string & n, const std::string  & a1,
               const std::string & a2) {
        Type = t; Func = f; Name = n; Arg1 = a1; Arg2 = a2;
    }
};

/** Function object
*    holds information for all types of functions
*/
struct ccgFunction {
    std::string Type, Name;

    ccgFunction() {}
    ccgFunction(const std::string & t, const std::string & n) {
        Type = t; Name = n;
    }
};

/** Event object
 *    holds information for all types of events
 */
struct ccgEvent {
    std::string Type, Function, Name, Arg1, Arg2;

    ccgEvent() {}
    ccgEvent(const std::string & t, const std::string & f,
             const std::string & n, const std::string & a1,
             const std::string & a2) {
        Type = t; Function = f; Name = n; Arg1 = a1; Arg2 = a2;
    }
};

/** Event handler object
 *    holds information for all types of event handlers
 */
struct ccgEventHandler {
    std::string Type, Name, Function, Arg;

    ccgEventHandler() {};
    ccgEventHandler(const std::string & t, const std::string & f,
                    const std::string & n, const std::string & a) {
        Type = t; Function = f; Name = n; Arg = a;
    }
};

/** State Table Data object
 *    holds information for state table variables
 */
struct ccgStateTableData {
    std::string Type, Name;

    ccgStateTableData() {
        Type = ""; Name = "";
    }
    ccgStateTableData(const std::string & t, const std::string & n) {
        Type = t; Name = n;
    }
};

/** Provided interface object
 *    has a deque of commands and a deque of events for the interface
 */
struct ccgInterfaceProvided {
    std::string Name;
    std::deque<ccgCommand> Commands;
    std::deque<ccgEvent> Events;

    ccgInterfaceProvided() {}
    ccgInterfaceProvided(const std::string & n) {
        Name = n;
    }
};

/** Required interface object
*    has a deque of functions and a deque of event handlers for the interface
*    type is the name of the object that is declared in the _init file
*/
struct ccgInterfaceRequired {
    std::string Name, Type;
    std::deque<ccgFunction> Functions;
    std::deque<ccgEventHandler> Handlers;

    ccgInterfaceRequired() {}
    ccgInterfaceRequired(const std::string & n) {
        Name = n; Type = "";
    }
    void SetObjName(const std::string & t) {
        Type = t;
    }
};

/** State table object
*    has a deque of the variables in the state table
*/
struct ccgStateTable {
    std::string Name;
    std::deque<ccgStateTableData> Data;
};

class ccgComponent{

public:

    ccgComponent() ;


/** sets the type of the component ex. mtsTaskPeriodic
*/
    inline void SetType(const std::string & t) {Type = t;}

/** sets the name of the component
*/
    inline void SetName(const std::string & n) {Name = n;}

/** returns the name of the component
*/
    inline const std::string & GetName(void) const { return Name; }

/** adds a variable to the state table
*/
    inline void AddStateTableData(const std::string & type, const std::string & n) {
        StateTable.Data.push_back(ccgStateTableData(type, n));
    }

/** adds an empty provided interface to the component
*/
    inline void AddInterfaceProvided(const std::string & n) {
        InterfacesProvided.push_back(ccgInterfaceProvided(n));
    }

/** adds an empty required interface to the component
*/
    inline void AddInterfaceRequired(const std::string & n) {
        InterfacesRequired.push_back(ccgInterfaceRequired(n));
    }

/** sets the name of the object created for the required interface
*    always sets it for the last added interface
*/
    inline void NameInterfaceRequired(const std::string & n) {
        InterfacesRequired.back().SetObjName(n);
    }

/** adds a command to the last added provided interface
*/
    inline void AddCommandToInterface(const std::string & t, const std::string & f,
                                      const std::string & n, const std::string & a1 = "void",
                                      const std::string & a2 = "void") {
        InterfacesProvided.back().Commands.push_back(ccgCommand(t, f, n, a1, a2));
    }

/** adds a function to the last added required interface
*/
    inline void AddFunctionToInterface(const std::string & t, const std::string & n) {
        InterfacesRequired.back().Functions.push_back(ccgFunction(t, n));
    }

/** adds an event to the last added provided interface
*/
    inline void AddEventToInterface(const std::string & t, const std::string & f,
                                    const std::string & n, const std::string & a1 = "void",
                                    const std::string & a2 = "void") {
        InterfacesProvided.back().Events.push_back(ccgEvent(t, f, n, a1, a2));
    }

/** adds an event handler to the last added required interface
*/
    inline void AddEventHandlerToInterface(const std::string & t, const std::string & f,
                                           const std::string & n, const std::string a = "void") {
        InterfacesRequired.back().Handlers.push_back(ccgEventHandler(t, f, n, a));
    }

    /********** Accessor functions *********************/

    inline size_t GetStateTableSize(void) const {
        return StateTable.Data.size();
    }

    inline size_t GetNumberOfInterfacesProvided(void) const {
        return InterfacesProvided.size();
    }

    inline size_t GetNumberOfInterfacesRequired(void) const {
        return InterfacesRequired.size();
    }

    inline std::deque<ccgInterfaceProvided> * GetInterfacesProvided(void) {
        return &InterfacesProvided;
    }

    inline std::deque<ccgInterfaceRequired> * GetInterfacesRequired(void) {
        return &InterfacesRequired;
    }

    /********** Code Generator Functions ****************/

    std::string WriteStateTableData(void);

    std::string WriteInterfaceProvided(void);

    std::string WriteInterfaceRequired(void);

    /** Generates the _init.cpp file for the current Component object
    *    takes the full name of the output file as an argument
    */
    void GenerateCode(std::string filename);

private:
    std::string Type, Name;
    std::deque<ccgInterfaceProvided> InterfacesProvided;
    std::deque<ccgInterfaceRequired> InterfacesRequired;
    ccgStateTable StateTable;
};

#endif // _ccgComponent_h
