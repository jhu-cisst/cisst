/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: parser.cpp 1030 2010-06-11 jkriss1 $ */

#include "parser.h"
#include <string>
#include <iostream>
#include <fstream>

using std::cout; using std::endl;

static const std::string macroNames[] =  { "DECLARE_COMPONENT", 
 "STATE_TABLE_BEGIN",
 "STATE_TABLE_DATA",
 "STATE_TABLE_END",
 "INTERFACE_PROVIDED_BEGIN",
 "INTERFACE_PROVIDED_END",
 "COMMAND_VOID",
 "COMMAND_WRITE",
 "COMMAND_READ",
 "COMMAND_STATEREAD",
 "COMMAND_QUALIFIEDREAD",
 "EVENT_VOID",
 "EVENT_WRITE",
 "INTERFACE_REQUIRED_BEGIN",
 "FUNCTION_VOID",
 "FUNCTION_WRITE",
 "FUNCTION_READ",
 "FUNCTION_QUALIFIEDREAD",
 "EVENT_HANDLER_WRITE",
 "EVENT_HANDLER_VOID",
 "INTERFACE_REQUIRED_END"
};

	parser::parser(string fullname){
		line = "";
		state = 0;
		fullName = fullname;
		
		filename = fullname.substr(0, fullname.rfind('.')) + "_init.cpp";
		component = new Component();
	}	
	
	/* state machine as in cisstBuilder wiki page https://trac.lcsr.jhu.edu/cisst/wiki/Private/cisstBuilder
		  	Valid States
		   	0. Idle (no keywords found)
     		1. Processing MTS_COMPONENT
     		2. Processing MTS_STATE_TABLE
     		3. Processing MTS_INTERFACE_PROVIDED
     		4. Processing MTS_INTERFACE_REQUIRED
     		
     		Valid Transitions
     		0 --> 1  found MTS_COMPONENT, parsed name of current component (e.g., sineTask)
     		1 --> 2  found MTS_STATE_TABLE_BEGIN, start processing state table entries
     		2 --> 1  found MTS_STATE_TABLE_END
     		1 --> 3  found MTS_INTERFACE_PROVIDED_BEGIN, start processing commands
     		3 --> 1  found MTS_INTERFACE_PROVIDED_END
     		1 --> 4  found MTS_INTERFACE_REQUIRED_BEGIN, start processing functions
     		4 --> 1  found MTS_INTERFACE_REQUIRED_END
		 */
	
	//input function, reads in file one line at a time and passes it to the other parsing functions
	int parser::parseFile()
	{		
		std::string temp = "";
		VALID_MACRO macro = NEG;

		std::ifstream fin;
		fin.open(fullName.c_str());

		while(getline(fin, temp))
		{
			line = temp;
				//cout << line << endl;
			if((macro = getMacro(0)) != NEG || state == 2)
			{
				//cout << line << endl;
				if(changeState(macro))
				{
					//cout << "state is " << state << endl;
					runState(macro);
					//if(!runState(macro)) {
					//	cout << "Error: Format Error in " << macroNames[macro] << endl;					
					//	return 0;
					//}
				}
				else{
					cout << "Error: Illegal State Change!" << endl;
					return 0;//illegal state change
				}
			}
		}			 

		if(state != 1) {
			cout << "Unexpected EOF!" << endl;
			fin.close();
			return 0; //reached end of file without ending declarations
		}

		fin.close();
		return 1;
	}
	
	VALID_MACRO parser::getMacro(int startValue)
	{
	
		for(VALID_MACRO macro = DECLARE_COMPONENT; macro < MAX; macro = (VALID_MACRO) (macro + 1))
		{
			int temp;
			if((temp = line.find(macroNames[macro] ,startValue)) != -1) 
			{
				line = line.substr(temp);
				return macro;
			}
		}
		return NEG;
	}
	
	int parser::changeState(VALID_MACRO m)
	{
		switch(state){
		case 0: 
			if(m == DECLARE_COMPONENT)
				state = 1;
			else
				return 0;
			break;
		case 1:
			switch(m) {
			case STATE_TABLE_BEGIN:
				state = 2;
				break;
			case INTERFACE_PROVIDED_BEGIN:
				state = 3;
				break;
			case INTERFACE_REQUIRED_BEGIN:
				state = 4;
				break;
			default:
				return 0;
			}
			break;
		case 2:
			if(m == STATE_TABLE_END) {
				state = 1;
				break;
			}
			else if(m == STATE_TABLE_DATA || m == NEG)
				state = 2;
			else
				return 0;
			break;
		case 3:
			switch(m) {
				case INTERFACE_PROVIDED_END:
					state = 1;
					break;
				case COMMAND_VOID:
				case COMMAND_WRITE:
				case COMMAND_READ:
				case COMMAND_STATEREAD:
				case COMMAND_QUALIFIEDREAD:
				case EVENT_VOID:
				case EVENT_WRITE:
					break;
				default:
					return 0;
			}
			break;
		case 4:
			switch(m) {
			case INTERFACE_REQUIRED_END:
			case FUNCTION_VOID:
			case FUNCTION_WRITE:
			case FUNCTION_READ:
			case FUNCTION_QUALIFIEDREAD:
			case EVENT_HANDLER_WRITE:
			case EVENT_HANDLER_VOID:
				break;
			default:
				//cout << m << endl;
				return 0;
		}
			break;
		default:
			return 0;
		}
		return 1;
	}
	
	int parser::runState(VALID_MACRO m)
	{
		switch (state) {
		case 0:
			return 1;
		case 1:
			//parse component name and type from line
			return parseComponent();
		case 2:
			//parse data types and names for state table
			//may need to add additional state table options
			if(m == STATE_TABLE_END)
				return 1;
			else if(m == STATE_TABLE_DATA)
				return parseStateTableData();
			else if(m == NEG)
				return parseStateTableDataLine();
			break;
		case 3:
			switch(m) {
			case INTERFACE_PROVIDED_BEGIN:
				return parseInterfaceProvided();
			case COMMAND_VOID:
				return parseCommandVoid(); 
			case COMMAND_WRITE:
				return parseCommandWrite();
			case COMMAND_READ:
				return parseCommandRead();
			case COMMAND_STATEREAD:
				return parseCommandReadState();
			case COMMAND_QUALIFIEDREAD:
				return parseCommandQualifiedRead();
			case EVENT_VOID:
				return parseEventVoid();
			case EVENT_WRITE:
				return parseEventWrite();
			default:
				return 0;
			}
		case 4:
			switch (m) {
			case INTERFACE_REQUIRED_BEGIN:
				return parseInterfaceRequired();
			case FUNCTION_VOID:
			case FUNCTION_WRITE:
			case FUNCTION_READ:
			case FUNCTION_QUALIFIEDREAD:
				return parseFunction();
			case EVENT_HANDLER_WRITE:
				return parseEventHandlerWrite();
			case EVENT_HANDLER_VOID:
				return parseEventHandlerVoid();
			case INTERFACE_REQUIRED_END:
				state = 1;
				return parseInterfaceRequiredEnd();
			default:
				return 0;
			}
		}
		return 1;
	}
	
	int parser::parseComponent()
	{
		int startin = line.find("(");
		int endin = line.find(",");
		std::string Cname, Ctype;

		if(startin == line.npos || endin == line.npos)
			return 0;

		Cname = line.substr(startin+1, endin - startin-1);
		Ctype = line.substr(endin+2, line.find(")") - endin-2);

		component->setType(Ctype);
		component->setName(Cname);

		
		//cout << Cname << " " << Ctype << endl;

		return 1;
	}

	int parser::parseStateTableData()
	{
		std::string dtype, dname;
		int startin = line.find('(')+1;
		int endin = line.find(',');
		int namestart = line.find(' ', endin)+1;

		dtype = line.substr(startin, endin - startin);
		dname = line.substr(namestart, line.find(')') - namestart);

		component->addStateTableData(dtype, dname);

		//cout << dtype << " " << dname << endl;
		return 1;
	}

	int parser::parseStateTableDataLine()
	{
		std::string dtype, dname;
		int typestart = line.find_first_not_of("\t ");

		if(line.at(line.find_last_not_of("\t ")) != ';')
			return 0;

		int typeend = line.find(' ', typestart);
		if(typeend == line.npos)
			return 0;

		int namestart = line.find_first_not_of(" \t*", typeend);
		if(namestart == line.npos)
			return 0;
		
		int nameend = line.find_last_not_of(" ;\t\n")+1;
		if(nameend == line.npos)
			return 0;

		dtype = line.substr(typestart, typeend - typestart);
		dname = line.substr(namestart, nameend - namestart);

		component->addStateTableData(dtype, dname);

		//cout << "Data: " << dtype << " " << dname << endl;
		
		return 1;
	}

	int parser::parseCommandVoid()
	{
		int funcstart = line.find('(')+1;
		int funcend = line.find(',', funcstart);
		int namestart = line.find('\"', funcend);
		std::string Cfunc, Cname, Ctype;

		Ctype = "Void";
		Cfunc = line.substr(funcstart, funcend - funcstart);
		Cname = line.substr(namestart, line.find(')')-namestart);

		component->addCommandToInterface(Ctype, Cfunc, Cname);

		//cout << Cname << " " << Cfunc << endl;
		return 1;
	}

	int parser::parseCommandWrite()
	{
		int funcstart = line.find('(')+1;
		int funcend = line.find(',', funcstart);
		int namestart = line.find('\"', funcend);
		int nameend = line.find('\"', namestart+1)+1;
		int argstart = line.find(' ', nameend);
		std::string Cfunc, Cname, Carg, Ctype;

		Ctype = "Write";
		Cfunc = line.substr(funcstart, funcend - funcstart);
		Cname = line.substr(namestart, nameend - namestart);
		if(argstart != line.npos)
			Carg = line.substr(argstart, line.find(')', argstart) - argstart);
		else
			Carg = "void";

		//cout << Ctype << " " << Cname << " " << Cfunc << " " << Carg << endl;
		component->addCommandToInterface(Ctype, Cfunc, Cname, Carg);

		return 1;
	}

	int parser::parseCommandRead()
	{
		int funcstart = line.find('(')+1;
		int funcend = line.find(',', funcstart);
		int argstart = line.find(' ', funcend)+1;
		int argend = line.find(',', argstart);
		int namestart = line.find('\"', argend);
		int nameend = line.find('\"', namestart+1)+1;
		int arg2start = line.find(' ', nameend);
		std::string Ctype, Cfunc, Cname, Carg, Carg2;

		Ctype = "Read";
		Cfunc = line.substr(funcstart, funcend - funcstart);
		Carg = line.substr(argstart, argend - argstart);
		Cname = line.substr(namestart, nameend - namestart);
		if(arg2start != line.npos)
			Carg2 = line.substr(arg2start+1, line.find(')', arg2start) - arg2start-1);
		else 
			Carg2 = "void";

		component->addCommandToInterface(Ctype, Cfunc, Cname, Carg, Carg2);

		
		//cout << Cname << " " << Ctype << " " << Carg << " " << Carg2 << endl;

		return 1;
	}

	int parser::parseCommandReadState()
	{
		int varstart = line.find('(')+1;
		int varend = line.find(',', varstart);
		int namestart = line.find('\"', varend);
		int nameend = line.find(')', namestart);
		std::string Ctype, Cvar, Cname;

		Ctype = "ReadState";
		Cvar = line.substr(varstart, varend - varstart);
		Cname = line.substr(namestart, nameend - namestart);
		
		component->addCommandToInterface(Ctype, Cvar, Cname);

		//cout << Cname << " " << Ctype << endl;
		return 1;
	}

	int parser::parseCommandQualifiedRead()
	{
		int startin = line.find('\"');
		int endin = line.find('\"', startin+1)+1;
		int funcstart = line.find(' ', endin)+1;
		int funcend = line.find(',', funcstart);
		int argstart = line.find(' ', funcend)+1;
		int argend = line.find(',', argstart);
		int arg2start = line.find(',', argend)+1;
		std::string Ctype, Cfunc, Cname, Carg1, Carg2;

		Ctype = "QualifiedRead";
		Cname = line.substr(startin, endin - startin);
		Cfunc = line.substr(funcstart, funcend - funcstart);
		Carg1 = line.substr(argstart, argend - argstart);
		Carg2 = line.substr(arg2start, line.find(')', arg2start) - arg2start);

		
		component->addCommandToInterface(Ctype, Cfunc, Cname, Carg1, Carg2);

		//cout << Cname << " " << Ctype << " " << Carg1 << " " << Carg2 << endl;
		return 1;
	}

	int parser::parseInterfaceProvided()
	{
		int startin = line.find('\"');
		int endin = line.find('\"', startin+1)+1;
		std::string name;

		name = line.substr(startin, endin - startin);

		component->addInterfaceProvided(name);

		//cout << name << endl;

		return 1;
	}

	int parser::parseInterfaceRequired()
	{
		int startin = line.find('(')+1;
		int endin = line.find(')', startin);
		std::string name = "\"";

		name += line.substr(startin, endin - startin) + "\"";
		
		component->addInterfaceRequired(name);
		
		//cout << name << endl;
		return 1;
	}

	int parser::parseInterfaceRequiredEnd()
	{
		int startin = line.find('(')+1;
		int endin = line.find(')', startin);
		std::string name;

		name = line.substr(startin, endin - startin);
		
		component->nameInterfaceRequired(name);
		
		//cout << name << endl;
		return 1;
	}

	int parser::parseFunction()
	{
		int namestart = line.find('\"');
		int nameend = line.find('\"', namestart+1)+1;
		int funcstart = line.find(' ', nameend)+1;
		std::string name, func;
		
		name = line.substr(namestart, nameend - namestart);
		func = line.substr(funcstart, line.find(')', funcstart) - funcstart);
		
		//cout << name << " " << func << endl;
		component->addFunctionToInterface(func, name);

		return 1;
	}

	int parser::parseEventVoid()
	{
		int funcstart = line.find('(')+1;
		int funcend = line.find(',', funcstart);
		int namestart = line.find('\"', funcend);
		int nameend = line.find(')', namestart);
		std::string Efunc, Ename, Etype;

		Etype = "Void";
		Efunc = line.substr(funcstart, funcend - funcstart);
		Ename = line.substr(namestart, nameend - namestart);

		component->addEventToInterface(Etype, Efunc, Ename);

		//cout << Ename << " " << Efunc << endl;

		return 1;
	}

	int parser::parseEventWrite()
	{
		int funcstart = line.find('(')+1;
		int funcend = line.find(',', funcstart);
		int namestart = line.find('\"', funcend);
		int nameend = line.find('\"', namestart+1)+1;
		int argstart = line.find(' ', nameend);
		std::string Efunc, Ename, Earg, Etype;

		Etype = "Write";
		Efunc = line.substr(funcstart, funcend - funcstart);
		Ename = line.substr(namestart, nameend - namestart);
		Earg = line.substr(argstart, line.find(')', argstart) - argstart);

		component->addEventToInterface(Etype, Efunc, Ename, Earg);
		return 1;
	}

	int parser::parseEventHandlerWrite()
	{
		int funcstart = line.find('(')+1;
		int funcend = line.find(',', funcstart);
		int namestart = line.find('\"', funcend);
		int nameend = line.find('\"', namestart+1)+1;
		int argstart = line.find(' ', nameend);
		std::string Efunc, Ename, Earg, Etype;

		Etype = "Write";
		Efunc = line.substr(funcstart, funcend - funcstart);
		Ename = line.substr(namestart, nameend - namestart);
		if(argstart != line.npos)
			Earg = line.substr(argstart, line.find(')', argstart) - argstart);
		else
			Earg = "void";

		component->addEventHandlerToInterface(Etype, Efunc, Ename, Earg);
		return 1;
	}

	int parser::parseEventHandlerVoid()
	{
		int funcstart = line.find('(')+1;
		int funcend = line.find(',', funcstart);
		int namestart = line.find('\"', funcend);
		std::string Efunc, Ename, Etype;

		Etype = "Void";
		Efunc = line.substr(funcstart, funcend - funcstart);
		Ename = line.substr(namestart, line.find(')') - namestart);

		component->addEventHandlerToInterface(Etype, Efunc, Ename);
		return 1;
	}

	void parser::GenerateCode()
	{
		component->generateCode(filename);
	}


