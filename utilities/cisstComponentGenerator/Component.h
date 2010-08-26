/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: Component.h 1030 2010-06-11 jkriss1 $ */

#include <string>
#include <deque>

#ifndef COMPONENT
#define COMPONENT

/** Command object
*	holds information for all types of commands
*/
struct Command {
    std::string type, name, func, arg1, arg2;

    Command() {} ;
    Command(std::string t, std::string f, std::string n, std::string a1, std::string a2) 
    { type = t; func = f; name = n; arg1 = a1; arg2 = a2; };
};


/** Function object
*	holds information for all types of functions
*/
	struct Function {
        std::string type, name;

		Function() {} ;
		Function(std::string t, std::string n) { type = t; name = n; };
	};


/** Event object
*	holds information for all types of events
*/
	struct Event {
        std::string type, function, name, arg1, arg2;

		Event() {} ;
		Event(std::string t, std::string f, std::string n, std::string a1, std::string a2) { type = t; function = f; name = n; arg1 = a1; arg2 = a2; };
	};

	
/** Event handler object
*	holds information for all types of event handlers
*/
	struct EventHandler {
		std::string type, name, function, arg;

		EventHandler() {} ;
		EventHandler(std::string t, std::string f, std::string n, std::string a) { type = t; function = f; name = n; arg = a; };
	};


/** State Table Data object
*	holds information for state table variables
*/
	struct STData {
		std::string type, name;

		STData() { type = ""; name = ""; };
		STData(std::string t, std::string n) { type = t; name = n; };
	};
	
/** Provided interface object
*	has a deque of commands and a deque of events for the interface
*/
	struct InterfaceProv {
		std::string name;
		std::deque<Command> commands;
		std::deque<Event> events;

		InterfaceProv() {};
		InterfaceProv(std::string n) { name = n; } ;
	};

/** Required interface object
*	has a deque of functions and a deque of event handlers for the interface
*	type is the name of the object that is declared in the _init file
*/
	struct InterfaceReq {
		std::string name, type;
		std::deque<Function> functions;
		std::deque<EventHandler> handlers;

		InterfaceReq() {};
		InterfaceReq(std::string n) { name = n; type = ""; } ;
		void setObjName(std::string t) { type = t; } ;
	};


/** State table object
*	has a deque of the variables in the state table
*/
	struct StateTable {
		std::string name;
		std::deque<STData> data;
	};

class Component{

public:
	
	Component() ;

	
/** sets the type of the component ex. mtsTaskPeriodic
*/
	inline void setType(std::string t) {type = t;} ;

/** sets the name of the component
*/
	inline void setName(std::string n) {name = n;} ;

/** returns the name of the component
*/
	inline std::string getName() { return name; } ;
	
/** adds a variable to the state table
*/
	inline void addStateTableData(const std::string type, const std::string n) { 
		table.data.push_back(STData(type, n)); } ;

/** adds an empty provided interface to the component
*/
	inline void addInterfaceProvided(const std::string n) {
		provInterfaces.push_back(InterfaceProv(n)); } ;

/** adds an empty required interface to the component
*/
	inline void addInterfaceRequired(const std::string n) {
		reqInterfaces.push_back(InterfaceReq(n)); } ;

/** sets the name of the object created for the required interface
*	always sets it for the last added interface
*/
	inline void nameInterfaceRequired(const std::string n) {
		reqInterfaces.back().setObjName(n); } ;

/** adds a command to the last added provided interface
*/
	inline void addCommandToInterface(const std::string t, const std::string f, const std::string n, const std::string a1 = "void", const std::string a2 = "void")
	{ provInterfaces.back().commands.push_back(Command(t, f, n, a1, a2)); };

/** adds a function to the last added required interface
*/
	inline void addFunctionToInterface(const std::string t, const std::string n) {
		reqInterfaces.back().functions.push_back(Function(t, n)); };

/** adds an event to the last added provided interface
*/
	inline void addEventToInterface(const std::string t, const std::string f, const std::string n, const std::string a1 = "void", const std::string a2 = "void") {
		provInterfaces.back().events.push_back(Event(t, f, n, a1, a2)); };

/** adds an event handler to the last added required interface
*/
	inline void addEventHandlerToInterface(const std::string t, const std::string f, const std::string n, const std::string a = "void") {
		reqInterfaces.back().handlers.push_back(EventHandler(t, f, n, a)); };
	
	/********** Accessor functions *********************/
	
	inline int stateTableSize() { return table.data.size(); };

	inline int getNumProvInt() { return provInterfaces.size(); };

	inline int getNumReqInt() { return reqInterfaces.size(); };

	inline std::deque<InterfaceProv> * getProvInt() { return &provInterfaces; };

	inline std::deque<InterfaceReq> * getReqInt() { return &reqInterfaces; };

	/********** Code Generator Functions ****************/
	
	std::string writeStateTableData() ;

	std::string writeProvInterface() ;

	std::string writeReqInterface() ;

	/** Generates the _init.cpp file for the current Component object
	*	takes the full name of the output file as an argument
	*/
	void generateCode(std::string filename) ;

private:
	std::string type, name;
	std::deque<InterfaceProv> provInterfaces;
	std::deque<InterfaceReq> reqInterfaces;
	StateTable table;
};

#endif
