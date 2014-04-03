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

#include "ccgParser.h"
#include <string>
#include <iostream>
#include <fstream>

CMN_IMPLEMENT_SERVICES(ccgParser);

#define CCG_PARSER_LOG "line: " << LineNumber << ", state: " << State

static const std::string MacroNames[] =  {
    "DECLARE_COMPONENT",
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
    "INTERFACE_REQUIRED_END",
    "FUNCTION_VOID",
    "FUNCTION_WRITE",
    "FUNCTION_READ",
    "FUNCTION_QUALIFIEDREAD",
    "EVENT_HANDLER_WRITE",
    "EVENT_HANDLER_VOID",
    "EVENT_HANDLER_NONQUEUED_VOID"
};

ccgParser::ccgParser(const std::string & fullname)
{
    Line = "";
    LineNumber = 0;
    State = CCG_IDLE;
    FullName = fullname;
    Component = new ccgComponent();
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
int ccgParser::ParseFile(void)
{
    std::string temp = "";
    ccgMacro macro = NEG_MACRO;

    std::ifstream fin;
    fin.open(FullName.c_str());
    if (!fin) {
        CMN_LOG_CLASS_INIT_ERROR << "ParseFile: unable to open file " << FullName << std::endl;
        return 0;
    }

    while (getline(fin, temp)) {
        Line = temp;
        LineNumber++;
        CMN_LOG_CLASS_RUN_VERBOSE << "ParseFile: " << Line << ", " << CCG_PARSER_LOG << std::endl;
        if ((macro = GetMacro(0)) != NEG_MACRO || State == CCG_PROCESSING_MTS_STATE_TABLE) {
            if (ChangeState(macro)) {
                CMN_LOG_CLASS_RUN_VERBOSE << "ParseFile: " << CCG_PARSER_LOG << std::endl;
                try {
                    RunState(macro);
                } catch (...) {
                    std::cerr << FullName << ":" << LineNumber << ":error: internal parser error" << std::endl;
                }
            } else {
                std::cerr << FullName << ":" << LineNumber << ":error: invalid state change" << std::endl;
                CMN_LOG_CLASS_RUN_ERROR << "ParseFile: invalid state change, " << CCG_PARSER_LOG << std::endl;
                return 0;//illegal state change
            }
        }
    }

    if (State != CCG_PROCESSING_MTS_COMPONENT) {
        std::cerr << FullName << ":" << LineNumber << ":error: unexpected EOF" << std::endl;
        CMN_LOG_CLASS_RUN_ERROR << "ParseFile: unexpected EOF, " << CCG_PARSER_LOG << std::endl;
        fin.close();
        return 0; //reached end of file without ending declarations
    }

    fin.close();
    return 1;
}


ccgMacro ccgParser::GetMacro(int startValue)
{
    for (ccgMacro macro = DECLARE_COMPONENT;
         macro < MAX_MACRO;
         macro = (ccgMacro) (macro + 1)) {
        size_t temp;
        if ((temp = Line.find(MacroNames[macro], startValue)) != -1) {
            Line = Line.substr(temp);
            return macro;
        }
    }
    return NEG_MACRO;
}


int ccgParser::ChangeState(ccgMacro macro)
{
    switch (State) {
    case CCG_IDLE:
        if (macro == DECLARE_COMPONENT)
            State = CCG_PROCESSING_MTS_COMPONENT;
        else
            return 0;
        break;
    case CCG_PROCESSING_MTS_COMPONENT:
        switch (macro) {
        case STATE_TABLE_BEGIN:
            State = CCG_PROCESSING_MTS_STATE_TABLE;
            break;
        case INTERFACE_PROVIDED_BEGIN:
            State = CCG_PROCESSING_MTS_INTERFACE_PROVIDED;
            break;
        case INTERFACE_REQUIRED_BEGIN:
            State = CCG_PROCESSING_MTS_INTERFACE_REQUIRED;
            break;
        default:
            return 0;
        }
        break;
    case CCG_PROCESSING_MTS_STATE_TABLE:
        if (macro == STATE_TABLE_END) {
            State = CCG_PROCESSING_MTS_COMPONENT;
            break;
        } else if (macro == STATE_TABLE_DATA || macro == NEG_MACRO)
            State = CCG_PROCESSING_MTS_STATE_TABLE;
        else
            return 0;
        break;
    case CCG_PROCESSING_MTS_INTERFACE_PROVIDED:
        switch (macro) {
        case INTERFACE_PROVIDED_END:
            State = CCG_PROCESSING_MTS_COMPONENT;
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
    case CCG_PROCESSING_MTS_INTERFACE_REQUIRED:
        switch (macro) {
        case INTERFACE_REQUIRED_END:
        case FUNCTION_VOID:
        case FUNCTION_WRITE:
        case FUNCTION_READ:
        case FUNCTION_QUALIFIEDREAD:
        case EVENT_HANDLER_WRITE:
        case EVENT_HANDLER_VOID:
        case EVENT_HANDLER_NONQUEUED_VOID:
            break;
        default:
            //std::cout << m << std::endl;
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 1;
}


int ccgParser::RunState(ccgMacro macro)
{
    switch (State) {
    case CCG_IDLE:
        return 1;
    case CCG_PROCESSING_MTS_COMPONENT:
        //parse component name and type from line
        return ParseComponent();
    case CCG_PROCESSING_MTS_STATE_TABLE:
        //parse data types and names for state table
        //may need to add additional state table options
        if (macro == STATE_TABLE_END)
            return 1;
        else if (macro == STATE_TABLE_DATA)
            return ParseStateTableData();
        else if (macro == NEG_MACRO)
            return ParseStateTableDataLine();
        break;
    case CCG_PROCESSING_MTS_INTERFACE_PROVIDED:
        switch (macro) {
        case INTERFACE_PROVIDED_BEGIN:
            return ParseInterfaceProvided();
        case COMMAND_VOID:
            return ParseCommandVoid();
        case COMMAND_WRITE:
            return ParseCommandWrite();
        case COMMAND_READ:
            return ParseCommandRead();
        case COMMAND_STATEREAD:
            return ParseCommandReadState();
        case COMMAND_QUALIFIEDREAD:
            return ParseCommandQualifiedRead();
        case EVENT_VOID:
            return ParseEventVoid();
        case EVENT_WRITE:
            return ParseEventWrite();
        default:
            return 0;
        }
    case CCG_PROCESSING_MTS_INTERFACE_REQUIRED:
        switch (macro) {
        case INTERFACE_REQUIRED_BEGIN:
            return ParseInterfaceRequired();
        case FUNCTION_VOID:
        case FUNCTION_WRITE:
        case FUNCTION_READ:
        case FUNCTION_QUALIFIEDREAD:
            return ParseFunction();
        case EVENT_HANDLER_WRITE:
            return ParseEventHandlerWrite();
        case EVENT_HANDLER_VOID:
            return ParseEventHandlerVoid();
        case EVENT_HANDLER_NONQUEUED_VOID:
            return ParseEventHandlerVoidNonQueued();
        case INTERFACE_REQUIRED_END:
            State = CCG_PROCESSING_MTS_COMPONENT;
            return ParseInterfaceRequiredEnd();
        default:
            return 0;
        }
    }
    return 1;
}


int ccgParser::ParseComponent(void)
{
    size_t startin = Line.find("(");
    size_t endin = Line.find(",");
    std::string Cname, Ctype;

    if (startin == Line.npos || endin == Line.npos)
        return 0;

    Cname = Line.substr(startin+1, endin - startin-1);
    Ctype = Line.substr(endin+2, Line.find(")") - endin-2);

    Component->SetType(Ctype);
    Component->SetName(Cname);

    return 1;
}


int ccgParser::ParseStateTableData(void)
{
    std::string dtype, dname;
    size_t startin = Line.find('(') + 1;
    size_t endin = Line.find(',');
    size_t namestart = Line.find(' ', endin) + 1;

    dtype = Line.substr(startin, endin - startin);
    dname = Line.substr(namestart, Line.find(')') - namestart);

    Component->AddStateTableData(dtype, dname);

    return 1;
}


int ccgParser::ParseStateTableDataLine(void)
{
    std::string dtype, dname;
    size_t typestart = Line.find_first_not_of("\t ");

    if (Line.at(Line.find_last_not_of("\t ")) != ';')
        return 0;

    size_t typeend = Line.find(' ', typestart);
    if (typeend == Line.npos)
        return 0;

    size_t namestart = Line.find_first_not_of(" \t*", typeend);
    if (namestart == Line.npos)
        return 0;

    size_t nameend = Line.find_last_not_of(" ;\t\n")+1;
    if (nameend == Line.npos)
        return 0;

    dtype = Line.substr(typestart, typeend - typestart);
    dname = Line.substr(namestart, nameend - namestart);

    Component->AddStateTableData(dtype, dname);

    //std::cout << "Data: " << dtype << " " << dname << std::endl;
    return 1;
}


int ccgParser::ParseCommandVoid(void)
{
    size_t funcstart = Line.find('(')+1;
    size_t funcend = Line.find(',', funcstart);
    size_t namestart = Line.find('\"', funcend);
    std::string Cfunc, Cname, Ctype;

    Ctype = "Void";
    Cfunc = Line.substr(funcstart, funcend - funcstart);
    Cname = Line.substr(namestart, Line.find(')')-namestart);

    Component->AddCommandToInterface(Ctype, Cfunc, Cname);

    //std::cout << Cname << " " << Cfunc << std::endl;
    return 1;
}


int ccgParser::ParseCommandWrite(void)
{
    size_t funcstart = Line.find('(')+1;
    size_t funcend = Line.find(',', funcstart);
    size_t namestart = Line.find('\"', funcend);
    size_t nameend = Line.find('\"', namestart+1)+1;
    size_t argstart = Line.find(' ', nameend);
    std::string Cfunc, Cname, Carg, Ctype;

    Ctype = "Write";
    Cfunc = Line.substr(funcstart, funcend - funcstart);
    Cname = Line.substr(namestart, nameend - namestart);
    if (argstart != Line.npos)
        Carg = Line.substr(argstart, Line.find_last_of(')') - argstart);
    else
        Carg = "void";

    //std::cout << Ctype << " " << Cname << " " << Cfunc << " " << Carg << std::endl;
    Component->AddCommandToInterface(Ctype, Cfunc, Cname, Carg);

    return 1;
}


int ccgParser::ParseCommandRead(void)
{
    size_t funcstart = Line.find('(')+1;
    size_t funcend = Line.find(',', funcstart);
    size_t argstart = Line.find(' ', funcend)+1;
    size_t argend = Line.find(',', argstart);
    size_t namestart = Line.find('\"', argend);
    size_t nameend = Line.find('\"', namestart+1)+1;
    size_t arg2start = Line.find(' ', nameend);
    std::string Ctype, Cfunc, Cname, Carg, Carg2;

    Ctype = "Read";
    Cfunc = Line.substr(funcstart, funcend - funcstart);
    Carg = Line.substr(argstart, argend - argstart);
    Cname = Line.substr(namestart, nameend - namestart);
    if (arg2start != Line.npos)
        Carg2 = Line.substr(arg2start+1, Line.find(')', arg2start) - arg2start-1);
    else
        Carg2 = "void";

    Component->AddCommandToInterface(Ctype, Cfunc, Cname, Carg, Carg2);
    //std::cout << Cname << " " << Ctype << " " << Carg << " " << Carg2 << std::endl;

    return 1;
}


int ccgParser::ParseCommandReadState(void)
{
    size_t varstart = Line.find('(')+1;
    size_t varend = Line.find(',', varstart);
    size_t namestart = Line.find('\"', varend);
    size_t nameend = Line.find(')', namestart);
    std::string Ctype, Cvar, Cname;

    Ctype = "ReadState";
    Cvar = Line.substr(varstart, varend - varstart);
    Cname = Line.substr(namestart, nameend - namestart);

    Component->AddCommandToInterface(Ctype, Cvar, Cname);
    //std::cout << Cname << " " << Ctype << std::endl;
    return 1;
}


int ccgParser::ParseCommandQualifiedRead(void)
{
    size_t startin = Line.find('\"');
    size_t endin = Line.find('\"', startin+1)+1;
    size_t funcstart = Line.find(' ', endin)+1;
    size_t funcend = Line.find(',', funcstart);
    size_t argstart = Line.find(' ', funcend)+1;
    size_t argend = Line.find(',', argstart);
    size_t arg2start = Line.find(',', argend)+1;
    std::string Ctype, Cfunc, Cname, Carg1, Carg2;

    Ctype = "QualifiedRead";
    Cname = Line.substr(startin, endin - startin);
    Cfunc = Line.substr(funcstart, funcend - funcstart);
    Carg1 = Line.substr(argstart, argend - argstart);
    Carg2 = Line.substr(arg2start, Line.find(')', arg2start) - arg2start);

    Component->AddCommandToInterface(Ctype, Cfunc, Cname, Carg1, Carg2);
    //std::cout << Cname << " " << Ctype << " " << Carg1 << " " << Carg2 << std::endl;
    return 1;
}


int ccgParser::ParseInterfaceProvided(void)
{
    size_t startin = Line.find('(')+1;
    size_t endin = Line.find(')', startin);
    std::string name;

    name = Line.substr(startin, endin - startin);

    Component->AddInterfaceProvided(name);
    //std::cout << name << std::endl;
    return 1;
}


int ccgParser::ParseInterfaceRequired(void)
{
    size_t startin = Line.find('(')+1;
    size_t endin = Line.find(')', startin);
    std::string name = "\"";

    name += Line.substr(startin, endin - startin) + "\"";

    Component->AddInterfaceRequired(name);
    //std::cout << name << std::endl;
    return 1;
}


int ccgParser::ParseInterfaceRequiredEnd(void)
{
    size_t startin = Line.find('(')+1;
    size_t endin = Line.find(')', startin);
    std::string name;

    name = Line.substr(startin, endin - startin);

    Component->NameInterfaceRequired(name);
    //std::cout << name << std::endl;
    return 1;
}


int ccgParser::ParseFunction(void)
{
    size_t funcstart = Line.find('(')+1;
    size_t funcend = Line.find(',', funcstart);
    size_t namestart = Line.find('\"', funcend);
    std::string func, name;

    func = Line.substr(funcstart, funcend - funcstart);
    name = Line.substr(namestart, Line.find(')')-namestart);

    // std::cout << name << " " << func << std::endl;
    Component->AddFunctionToInterface(func, name);
    return 1;
}


int ccgParser::ParseEventVoid(void)
{
    size_t funcstart = Line.find('(')+1;
    size_t funcend = Line.find(',', funcstart);
    size_t namestart = Line.find('\"', funcend);
    size_t nameend = Line.find(')', namestart);
    std::string Efunc, Ename, Etype;

    Etype = "Void";
    Efunc = Line.substr(funcstart, funcend - funcstart);
    Ename = Line.substr(namestart, nameend - namestart);

    Component->AddEventToInterface(Etype, Efunc, Ename);
    //std::cout << Ename << " " << Efunc << std::endl;
    return 1;
}


int ccgParser::ParseEventWrite(void)
{
    size_t funcstart = Line.find('(')+1;
    size_t funcend = Line.find(',', funcstart);
    size_t namestart = Line.find('\"', funcend);
    size_t nameend = Line.find('\"', namestart+1)+1;
    size_t argstart = Line.find(' ', nameend);
    std::string Efunc, Ename, Earg, Etype;

    Etype = "Write";
    Efunc = Line.substr(funcstart, funcend - funcstart);
    Ename = Line.substr(namestart, nameend - namestart);
    Earg = Line.substr(argstart, Line.find(')', argstart) - argstart);

    Component->AddEventToInterface(Etype, Efunc, Ename, Earg);
    return 1;
}


int ccgParser::ParseEventHandlerWrite(void)
{
    size_t funcstart = Line.find('(')+1;
    size_t funcend = Line.find(',', funcstart);
    size_t namestart = Line.find('\"', funcend);
    size_t nameend = Line.find('\"', namestart+1)+1;
    size_t argstart = Line.find(' ', nameend);
    std::string Efunc, Ename, Earg, Etype;

    Etype = "Write";
    Efunc = Line.substr(funcstart, funcend - funcstart);
    Ename = Line.substr(namestart, nameend - namestart);
    if (argstart != Line.npos) {
        Earg = Line.substr(argstart, Line.find(')', argstart) - argstart);
    } else {
        Earg = "void";
    }

    Component->AddEventHandlerToInterface(Etype, Efunc, Ename, Earg);
    return 1;
}


int ccgParser::ParseEventHandlerVoid(void)
{
    size_t funcstart = Line.find('(')+1;
    size_t funcend = Line.find(',', funcstart);
    size_t namestart = Line.find('\"', funcend);
    std::string Efunc, Ename, Etype;

    Etype = "Void";
    Efunc = Line.substr(funcstart, funcend - funcstart);
    Ename = Line.substr(namestart, Line.find(')') - namestart);

    Component->AddEventHandlerToInterface(Etype, Efunc, Ename);
    return 1;
}


int ccgParser::ParseEventHandlerVoidNonQueued(void)
{
    size_t funcstart = Line.find('(')+1;
    size_t funcend = Line.find(',', funcstart);
    size_t namestart = Line.find('\"', funcend);
    std::string Efunc, Ename, Etype, Epolicy;

    Etype = "Void";
    Efunc = Line.substr(funcstart, funcend - funcstart);
    Ename = Line.substr(namestart, Line.find(')') - namestart);
    Epolicy = "MTS_EVENT_NOT_QUEUED";

    Component->AddEventHandlerToInterface(Etype, Efunc, Ename, Epolicy);
    return 1;
}


void ccgParser::GenerateCode(const std::string & filename)
{
    Component->GenerateCode(filename);
}
