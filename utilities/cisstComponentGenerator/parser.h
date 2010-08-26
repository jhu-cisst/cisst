/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: parser.h 1030 2010-06-11 jkriss1 $ */


#ifndef _parser_h
#define _parser_h

#include <string>
#include "Component.h"

enum VALID_MACRO {  NEG = -1, DECLARE_COMPONENT, 
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
 FUNCTION_VOID,
 FUNCTION_WRITE,
 FUNCTION_READ,
 FUNCTION_QUALIFIEDREAD,
 EVENT_HANDLER_WRITE,
 EVENT_HANDLER_VOID,
 INTERFACE_REQUIRED_END,
 MAX
};

class parser {
	
private:
	// a string that holds the text of the file
	std::string line;
	
	//name of the file to be written
	std::string filename;
	//full name of the file being parsed
	std::string fullName;
	
	// an object that holds the information for the new component
	Component * component;
	// the current state
	int state;
	
	/******  Functions used to parse input file *********/

	VALID_MACRO parser::getMacro(int startValue) ;
	
	int parser::changeState(VALID_MACRO m) ;
	
	int parser::runState(VALID_MACRO m) ;
	
	int parser::parseComponent() ;

	int parser::parseStateTableData() ;

	int parser::parseStateTableDataLine() ;

	int parser::parseInterfaceProvided() ;

	int parser::parseInterfaceRequired() ;

	int parser::parseInterfaceRequiredEnd() ;

	int parser::parseCommandVoid() ;

	int parser::parseCommandRead() ;

	int parser::parseCommandReadState() ;

	int parser::parseCommandWrite() ;

	int parser::parseCommandQualifiedRead() ;

	int parser::parseFunction() ;

	int parser::parseEventVoid() ;

	int parser::parseEventWrite() ;

	int parser::parseEventHandlerWrite() ;

	int parser::parseEventHandlerVoid() ;

	
public:
	/** Constructor
	*	takes the name of the file to be parsed as an argument
	*/
	parser::parser(string filename);
	
	int parser::parseFile() ;

	/** Generates code for the component that has been parsed
	*	appends _init.cpp to the name of the input file
	*/
	void parser::GenerateCode() ;

	inline Component * parser::getComponent() { return component; } ;

};

#endif