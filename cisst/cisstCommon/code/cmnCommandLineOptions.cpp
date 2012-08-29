/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2012-08-27

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnCommandLineOptions.h>

CMN_IMPLEMENT_SERVICES(cmnCommandLineOptions);


cmnCommandLineOptions::OptionNoValue::OptionNoValue(const std::string & shortOption, const std::string & longOption,
                                                    const std::string & description, RequiredType required):
    Short(shortOption),
    Long(longOption),
    Description(description),
    Required(required),
    Set(false)
{
}


bool cmnCommandLineOptions::OptionNoValue::SetValue(char * CMN_UNUSED(value))
{
    this->Set = true;
    return true;
}


cmnCommandLineOptions::OptionOneValueBase::OptionOneValueBase(const std::string & shortOption, const std::string & longOption,
                                                              const std::string & description, RequiredType required):
    cmnCommandLineOptions::OptionNoValue(shortOption, longOption, description, required)
{
}


cmnCommandLineOptions::cmnCommandLineOptions(void)
{
}


bool cmnCommandLineOptions::AddOptionNoValue(const std::string & shortOption, const std::string & longOption,
                                             const std::string & description, cmnCommandLineOptions::RequiredType required)
{
    if (this->GetShortNoDash(shortOption)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddOption: option \"-" << shortOption << "\" already defined" << std::endl;
        return false;
    }
    OptionNoValue * option = new OptionNoValue(shortOption, longOption, description, required);;
    this->Options.push_back(option);
    return true;
}


bool cmnCommandLineOptions::Parse(int argc, char * argv[], std::string & errorMessage)
{
    // first, extract program name
    this->ProgramName = argv[0];
    --argc;
    ++argv;
    const size_t size = this->ProgramName.size();
    size_t index;
    for (index = 0; index < size; index++) {
        if (this->ProgramName[index] == '\\') {
            this->ProgramName[index]= '/';
        }
    }

    // dummy case
    if (this->Options.empty()) {
        CMN_LOG_CLASS_INIT_ERROR << "Parse: no option added, need to use cmnCommandLineOptions::Add before Parse (programmer error!)" << std::endl;
        return false;
    }

    // parse options
    OptionNoValue * optionNoValue;
    while (argc != 0) {
        optionNoValue = this->Get(argv[0]);
        if (!optionNoValue) {
            errorMessage = "Unknown option: ";
            errorMessage.append(argv[0]);
            return false;
        }
        --argc; // parsed one parameter
        ++argv;
        if (dynamic_cast<OptionOneValueBase *>(optionNoValue)) {
            if (argc == 0) {
                errorMessage = "Not enough command line parameters while parsing value for option --" + optionNoValue->Long;
                return false;
            }
            if (optionNoValue->Set) {
                errorMessage = "Option --" + optionNoValue->Long + " already set";
                return false;
            }
            if (!optionNoValue->SetValue(argv[0])) {
                errorMessage = "Unable to parse/convert value for option --" + optionNoValue->Long + " (found \"" + argv[0] + "\")";
                return false;
            }
            ++argv;
            --argc;
        }
    }

    // make sure all required options are set
    const OptionsType::const_iterator end = this->Options.end();
    OptionsType::const_iterator iter = this->Options.begin();
    for (; iter != end; ++iter) {
        optionNoValue = *iter;
        if ((optionNoValue->Required == REQUIRED) && !(optionNoValue->Set)) {
            errorMessage = "Option --" + optionNoValue->Long + " required but not set";
            return false;
        }
    }

    return true;
}


void cmnCommandLineOptions::PrintUsage(std::ostream & outputStream)
{
    if (this->ProgramName != "") {
        outputStream << this->ProgramName << ":" << std::endl;
    }
    const OptionsType::const_iterator end = this->Options.end();
    OptionsType::const_iterator iter = this->Options.begin();
    OptionNoValue * option;
    std::string value;
    for (; iter != end; ++iter) {
        option = *iter;
        if (dynamic_cast<const OptionOneValueBase *>(option)) {
            value = " <value>";
        } else {
            value = "";
        }
        outputStream << " -" << option->Short << value << ", --" << option->Long << value << " : " << option->Description
                     << ((option->Required == REQUIRED) ? " (required)" : " (optional)") << std::endl;
    }
}


cmnCommandLineOptions::OptionNoValue * cmnCommandLineOptions::GetShortNoDash(const std::string & shortOption)
{
    const OptionsType::const_iterator end = this->Options.end();
    OptionsType::const_iterator iter = this->Options.begin();
    for (; iter != end; ++iter) {
        if ((*iter)->Short == shortOption) {
            return *iter;
        }
    }
    return 0;
}


cmnCommandLineOptions::OptionNoValue * cmnCommandLineOptions::Get(const std::string & option)
{
    const OptionsType::const_iterator end = this->Options.end();
    OptionsType::const_iterator iter = this->Options.begin();
    std::string shortOption;
    std::string longOption;
    for (; iter != end; ++iter) {
        shortOption = "-";
        shortOption.append((*iter)->Short);
        longOption = "--";
        longOption.append((*iter)->Long);
        if ((shortOption == option) || (longOption == option)) {
            return *iter;
        }
    }
    return 0;
}
