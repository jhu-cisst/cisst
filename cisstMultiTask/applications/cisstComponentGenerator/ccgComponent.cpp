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
#include <ccgComponent.h>

#include <iostream>
#include <fstream>


ccgComponent::ccgComponent(void)
{
}


std::string ccgComponent::WriteStateTableData(void)
{
    std::string s = "StateTable.AddData(";
    ccgStateTableData data = StateTable.Data.front();
    StateTable.Data.pop_front();

    s += data.Name + ", \"" + data.Name + "\");";

    return s;
}


std::string ccgComponent::WriteInterfaceProvided(void)
{
    ccgInterfaceProvided interf = InterfacesProvided.front();
    std::string n = interf.Name.substr(1, interf.Name.length()-2);

    std::string s
        = "    interfaceProvided = AddInterfaceProvided(\"" + interf.Name + "\");\n"
        + "    if (!interfaceProvided) {\n"
        + "        CMN_LOG_CLASS_INIT_ERROR << \"failed to add \\\"" + interf.Name + "\\\" to component \\\"\" << this->GetName() << \"\\\"\" << std::endl;\n"
        + "        return false;\n"
        + "    }\n";

    size_t size = interf.Commands.size();
    for (size_t i = 0; i < size; i++) {
        ccgCommand command = interf.Commands.front();

        s += "    localResult = (0 != interfaceProvided->AddCommand";
        if (command.Type == "Void" || command.Type == "Write" || command.Type == "QualifiedRead") {
            s += command.Type + "(&" + Name + "::" + command.Func + ", this, " + command.Name;

            if (command.Type == "Write" && command.Arg1 != "void") {
                s += ", " + command.Arg1;
            }

            s += "));\n";
        } else if (command.Type == "Read") {
            if (command.Arg2 == "void") {
                s += command.Type + "(&" + Name + "::" + command.Func + ", " + command.Arg1 + ", " + command.Name + "));\n";
            } else {
                s += command.Type + "(&" + command.Arg2 + "::" + command.Func + ", " + command.Arg1 + ", " + command.Name + "));\n";
            }
        } else if (command.Type == "ReadState") {
            s += command.Type + "(StateTable, " + command.Func + ", " + command.Name + "));\n";
        }
        s += "    if (!localResult) {\n";
        s += "        CMN_LOG_CLASS_INIT_ERROR << \"failed to add command to interface\\\"\" << interfaceProvided->GetFullName() << \"\\\"\" << std::endl;\n";
        s += "        return false;\n";
        s += "    }\n";

        interf.Commands.pop_front();
    }

    size = interf.Events.size();
    for (size_t i = 0; i < size; i++) {
        ccgEvent e = interf.Events.front();

        s += "    localResult = (0 != interfaceProvided->AddEvent";
        s += e.Type + "(" + interf.Name + "." + e.Function + ", " + e.Name;
        if (e.Type == "Write") {
            s += ", " + e.Arg1 + "));\n";
        } else {
            s += "));\n";
        }
        s += "    if (!localResult) {\n";
        s += "        CMN_LOG_CLASS_INIT_ERROR << \"failed to add event to interface\\\"\" << interfaceProvided->GetFullName() << \"\\\"\" << std::endl;\n";
        s += "        return false;\n";
        s += "    }\n";
        interf.Events.pop_front();
    }

    InterfacesProvided.pop_front();
    return s;
}


std::string ccgComponent::WriteInterfaceRequired(void)
{
    ccgInterfaceRequired interf = InterfacesRequired.front();
    std::string n = interf.Name.substr(1, interf.Name.length() - 2);

    std::string s = "    interfaceRequired = AddInterfaceRequired(" + interf.Name + ");\n";

    size_t size = interf.Functions.size();
    for (size_t i = 0; i < size; i++) {
        ccgFunction function = interf.Functions.front();
        s += "    localResult = interfaceRequired->AddFunction";
        s += "(" + function.Name + ", " + interf.Type + "." + function.Type + ");\n";
        s += "    if (!localResult) {\n";
        s += "        CMN_LOG_CLASS_INIT_ERROR << \"failed to add function to interface\\\"\" << interfaceRequired->GetFullName() << \"\\\"\" << std::endl;\n";
        s += "        return false;\n";
        s += "    }\n";
        interf.Functions.pop_front();
    }

    size = interf.Handlers.size();
    for (size_t i = 0; i < size; i++) {
        ccgEventHandler handler = interf.Handlers.front();
        s += "    localResult = interfaceRequired->AddEventHandler" + handler.Type;
        s += "(&" + Name + "::" + handler.Function + ", this, " + handler.Name;
        if (handler.Arg != "void") {
            s += ", " + handler.Arg;
        }
        s += ");\n";
        s += "    if (!localResult) {\n";
        s += "        CMN_LOG_CLASS_INIT_ERROR << \"failed to add event handler to interface\\\"\" << interfaceRequired->GetFullName() << \"\\\"\" << std::endl;\n";
        s += "        return false;\n";
        s += "    }\n";
        interf.Handlers.pop_front();
    }

    InterfacesRequired.pop_front();
    return s;
}


void ccgComponent::GenerateCode(std::string filename)
{
    std::ofstream file;

    std::cout << filename << std::endl;

    file.open(filename.c_str());

    file << "/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */" << std::endl
         << "/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */" << std::endl
         << "/* File " << GetName() << " generated by cisstComponentGenerator, do not change */" << std::endl
         << std::endl
         << "#include <" << GetName() << ".h>" << std::endl
         << "#include <cisstMultiTask/mtsInterfaceProvided.h>" << std::endl
         << "#include <cisstMultiTask/mtsInterfaceRequired.h>" << std::endl
         << std::endl
         << "bool " << GetName() << "::InitComponent(void)" << std::endl
         << "{" << std::endl
         << "    bool localResult = true;" << std::endl;


    size_t size = GetStateTableSize();
    if (size > 0) {
        file << "    // state table variables" << std::endl;
    }
    for (size_t i = 0; i < size; i++) {
        file << "    " << WriteStateTableData() << std::endl;
    }

    size = GetNumberOfInterfacesProvided();

    if (size > 0) {
        file << "    // provided interfaces" << std::endl
             << "    mtsInterfaceProvided * interfaceProvided;" << std::endl;
    }

    for (size_t i = 0; i < size; i++) {
        file << WriteInterfaceProvided() << std::endl;
    }

    size = GetNumberOfInterfacesRequired();

    if (size > 0) {
        file << "    // required interfaces" << std::endl
             << "    mtsInterfaceRequired * interfaceRequired;" << std::endl;
    }

    for (size_t i = 0; i < size; i++) {
        file << WriteInterfaceRequired() << std::endl;
    }

    file << std::endl << "    return true;" << std::endl << "};" << std::endl;

    file.close();
}
