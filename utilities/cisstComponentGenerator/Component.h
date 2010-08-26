/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: Component.h 1030 2010-06-11 jkriss1 $ */

#include <string>
#include <deque>
#ifndef COMPONENT
#define COMPONENT

using std::string; 

/** Command object
*	holds information for all types of commands
*/
struct Command {
		string type, name, func, arg1, arg2;

		Command::Command() {} ;
		Command::Command(string t, string f, string n, string a1, string a2) 
			{ type = t; func = f; name = n; arg1 = a1; arg2 = a2; };
	};


/** Function object
*	holds information for all types of functions
*/
	struct Function {
		string type, name;

		Function::Function() {} ;
		Function::Function(string t, string n) { type = t; name = n; };
	};


/** Event object
*	holds information for all types of events
*/
	struct Event {
		string type, function, name, arg1, arg2;

		Event::Event() {} ;
		Event::Event(string t, string f, string n, string a1, string a2) { type = t; function = f; name = n; arg1 = a1; arg2 = a2; };
	};

	
/** Event handler object
*	holds information for all types of event handlers
*/
	struct EventHandler {
		string type, name, function, arg;

		EventHandler::EventHandler() {} ;
		EventHandler::EventHandler(string t, string f, string n, string a) { type = t; function = f; name = n; arg = a; };
	};


/** State Table Data object
*	holds information for state table variables
*/
	struct STData {
		string type, name;

		STData::STData() { type = ""; name = ""; };
		STData::STData(string t, string n) { type = t; name = n; };
	};
	
/** Provided interface object
*	has a deque of commands and a deque of events for the interface
*/
	struct InterfaceProv {
		string name;
		std::deque<Command> commands;
		std::deque<Event> events;

		InterfaceProv::InterfaceProv() {};
		InterfaceProv::InterfaceProv(string n) { name = n; } ;
	};

/** Required interface object
*	has a deque of functions and a deque of event handlers for the interface
*	type is the name of the object that is declared in the _init file
*/
	struct InterfaceReq {
		string name, type;
		std::deque<Function> functions;
		std::deque<EventHandler> handlers;

		InterfaceReq::InterfaceReq() {};
		InterfaceReq::InterfaceReq(string n) { name = n; type = ""; } ;
		void InterfaceReq::setObjName(string t) { type = t; } ;
	};


/** State table object
*	has a deque of the variables in the state table
*/
	struct StateTable {
		string name;
		std::deque<STData> data;
	};

class Component{

public:
	
	Component::Component() ;

	
/** sets the type of the component ex. mtsTaskPeriodic
*/
	inline void Component::setType(string t) {type = t;} ;

/** sets the name of the component
*/
	inline void Component::setName(string n) {name = n;} ;

/** returns the name of the component
*/
	inline string Component::getName() { return name; } ;
	
/** adds a variable to the state table
*/
	inline void Component::addStateTableData(const string type, const string n) { 
		table.data.push_back(STData(type, n)); } ;

/** adds an empty provided interface to the component
*/
	inline void Component::addInterfaceProvided(const string n) {
		provInterfaces.push_back(InterfaceProv(n)); } ;

/** adds an empty required interface to the component
*/
	inline void Component::addInterfaceRequired(const string n) {
		reqInterfaces.push_back(InterfaceReq(n)); } ;

/** sets the name of the object created for the required interface
*	always sets it for the last added interface
*/
	inline void Component::nameInterfaceRequired(const string n) {
		reqInterfaces.back().setObjName(n); } ;

/** adds a command to the last added provided interface
*/
	inline void Component::addCommandToInterface(const string t, const string f, const string n, const string a1 = "void", const string a2 = "void")
	{ provInterfaces.back().commands.push_back(Command(t, f, n, a1, a2)); };

/** adds a function to the last added required interface
*/
	inline void Component::addFunctionToInterface(const string t, const string n) {
		reqInterfaces.back().functions.push_back(Function(t, n)); };

/** adds an event to the last added provided interface
*/
	inline void Component::addEventToInterface(const string t, const string f, const string n, const string a1 = "void", const string a2 = "void") {
		provInterfaces.back().events.push_back(Event(t, f, n, a1, a2)); };

/** adds an event handler to the last added required interface
*/
	inline void Component::addEventHandlerToInterface(const string t, const string f, const string n, const string a = "void") {
		reqInterfaces.back().handlers.push_back(EventHandler(t, f, n, a)); };
	
	/********** Accessor functions *********************/
	
	inline int Component::stateTableSize() { return table.data.size(); };

	inline int Component::getNumProvInt() { return provInterfaces.size(); };

	inline int Component::getNumReqInt() { return reqInterfaces.size(); };

	inline std::deque<InterfaceProv> * Component::getProvInt() { return &provInterfaces; };

	inline std::deque<InterfaceReq> * Component::getReqInt() { return &reqInterfaces; };

	/********** Code Generator Functions ****************/
	
	string Component::writeStateTableData() ;

	string Component::writeProvInterface() ;

	string Component::writeReqInterface() ;

	/** Generates the _init.cpp file for the current Component object
	*	takes the full name of the output file as an argument
	*/
	void Component::generateCode(string filename) ;

private:
	string type, name;
	std::deque<InterfaceProv> provInterfaces;
	std::deque<InterfaceReq> reqInterfaces;
	StateTable table;
};

#endif