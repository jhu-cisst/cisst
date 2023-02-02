/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2012-08-27

  (C) Copyright 2012-2023 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstCommon/cmnCommandLineOptions.h>

CMN_IMPLEMENT_SERVICES(cmnCommandLineOptions);


cmnCommandLineOptions::OptionBase::OptionBase(const std::string & shortOption, const std::string & longOption,
                                              const std::string & description, RequiredType required):
    Short(shortOption),
    Long(longOption),
    Description(description),
    Required(required),
    Set(false)
{
}


cmnCommandLineOptions::OptionNoValue::OptionNoValue(const std::string & shortOption, const std::string & longOption,
                                                    const std::string & description, RequiredType required):
    cmnCommandLineOptions::OptionBase(shortOption, longOption, description, required)
{
}



bool cmnCommandLineOptions::OptionNoValue::SetValue(const char * CMN_UNUSED(value))
{
    this->Set = true;
    return true;
}


cmnCommandLineOptions::OptionOneValueBase::OptionOneValueBase(const std::string & shortOption, const std::string & longOption,
                                                              const std::string & description, RequiredType required):
    cmnCommandLineOptions::OptionBase(shortOption, longOption, description, required)
{
}


cmnCommandLineOptions::OptionMultipleValuesBase::OptionMultipleValuesBase(const std::string & shortOption, const std::string & longOption,
                                                                          const std::string & description, RequiredType required):
    cmnCommandLineOptions::OptionBase(shortOption, longOption, description, required)
{
}


cmnCommandLineOptions::cmnCommandLineOptions(void)
{
}


cmnCommandLineOptions::~cmnCommandLineOptions()
{
    const OptionsType::iterator end = this->Options.end();
    OptionsType::iterator iter = this->Options.begin();
    for (; iter != end; ++iter) {
        delete *iter;
    }
}


bool cmnCommandLineOptions::AddOptionNoValue(const std::string & shortOption, const std::string & longOption,
                                             const std::string & description, RequiredType required)
{
    std::string cleanedShort, cleanedLong;
    if (!this->ValidOptions(shortOption, longOption, cleanedShort, cleanedLong)) {
        return false;
    }
    OptionNoValue * option = new OptionNoValue(cleanedShort, cleanedLong, description, required);
    this->Options.push_back(option);
    return true;
}


bool cmnCommandLineOptions::Parse(int argc, const char * argv[], std::string & errorMessage)
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

    bool foundOneSquash = false;

    // parse options
    OptionBase * option;
    while (argc != 0) {
        option = this->Get(argv[0]);
        if (!option) {
            errorMessage = "Unknown option: ";
            errorMessage.append(argv[0]);
            return false;
        }
        --argc; // parsed one parameter
        ++argv;

        // if the option is a squash requirement, save that
        if (option->Required == SQUASH_REQUIRED_OPTION) {
            foundOneSquash = true;
        }

        // check that the option has not been already set
        if (option->Set && !(dynamic_cast<OptionMultipleValuesBase *>(option))) {
            errorMessage = "Option --" + option->Long + " already set";
            return false;
        }

        // -- no value option
        if (dynamic_cast<OptionNoValue *>(option)) {
            option->SetValue(0);
        }
        // -- one or multiple values option
        else if (dynamic_cast<OptionOneValueBase *>(option) || dynamic_cast<OptionMultipleValuesBase *>(option)) {
            if (argc == 0) {
                errorMessage = "Not enough command line parameters while parsing value for option --" + option->Long;
                return false;
            }
            if (!option->SetValue(argv[0])) {
                errorMessage = "Unable to parse/convert value for option --" + option->Long + " (found \"" + argv[0] + "\")";
                return false;
            }
            ++argv;
            --argc;
        }
        else {
            errorMessage = "Unknown type of option: internal bug";
            return false;
        }
    }

    // check required if needed
    if (!foundOneSquash) {
        const OptionsType::const_iterator end = this->Options.end();
        OptionsType::const_iterator iter = this->Options.begin();
        for (; iter != end; ++iter) {
            option = *iter;
            if ((option->Required == REQUIRED_OPTION) && !(option->Set)) {
                errorMessage = "Option --" + option->Long + " required but not set";
                return false;
            }
        }
    }

    return true;
}


bool cmnCommandLineOptions::Parse(int argc, char * argv[], std::string & errorMessage)
{
    int index;
    typedef const char * constCharPtr;
    constCharPtr * argv_const = new constCharPtr[argc];
    for (index = 0; index < argc;  index++) {
        argv_const[index] = argv[index];
    }
    bool result = Parse(argc, argv_const, errorMessage);
    delete[] argv_const;
    return result;
}


bool cmnCommandLineOptions::Parse(const std::vector<std::string> & arguments, std::string & errorMessage)
{
    int index;
    typedef const char * constCharPtr;
    int argc = arguments.size();
    constCharPtr * argv_const = new constCharPtr[argc];
    for (index = 0; index < argc;  index++) {
        argv_const[index] = arguments[index].c_str();
    }
    bool result = Parse(argc, argv_const, errorMessage);
    delete[] argv_const;
    return result;
}


bool cmnCommandLineOptions::Parse(int argc, char * argv[],
                                  std::ostream & outputStream)
{
    std::string errorMessage;
    if (!Parse(argc, argv, errorMessage)) {
        outputStream << "Error: " << errorMessage << std::endl;
        PrintUsage(outputStream);
        return false;
    }
    return true;
}


bool cmnCommandLineOptions::Parse(const std::vector<std::string> & arguments,
                                  std::ostream & outputStream)
{
    std::string errorMessage;
    if (!Parse(arguments, errorMessage)) {
        outputStream << "Error: " << errorMessage << std::endl;
        PrintUsage(outputStream);
        return false;
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
    OptionBase * option;
    std::string value;
    for (; iter != end; ++iter) {
        option = *iter;
        if (dynamic_cast<const OptionOneValueBase *>(option)) {
            value = " <value>";
        } else {
            value = "";
        }
        outputStream << " -" << option->Short << value << ", --" << option->Long << value << " : " << option->Description;
        if (option->Required == REQUIRED_OPTION) {
            outputStream << " (required)";
        } else if (option->Required == OPTIONAL_OPTION) {
            outputStream << " (optional)";
        } else if (option->Required == SQUASH_REQUIRED_OPTION) {
            outputStream << " (optional, no other option required when set)";
        }
        outputStream << std::endl;
    }
}


cmnCommandLineOptions::OptionBase * cmnCommandLineOptions::GetShortNoDash(const std::string & shortOption)
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


cmnCommandLineOptions::OptionBase * cmnCommandLineOptions::GetLongNoDashDash(const std::string & longOption)
{
    const OptionsType::const_iterator end = this->Options.end();
    OptionsType::const_iterator iter = this->Options.begin();
    for (; iter != end; ++iter) {
        if ((*iter)->Long == longOption) {
            return *iter;
        }
    }
    return 0;
}


bool cmnCommandLineOptions::ValidOptions(const std::string & shortOption, const std::string & longOption,
                                         std::string & cleanedShort, std::string & cleanedLong)
{
    cleanedShort = shortOption;
    cleanedLong = longOption;
    size_t position;

    position = cleanedShort.find('-');
    if (position == 0) {
        cleanedShort.erase(0, 1);
        CMN_LOG_CLASS_INIT_WARNING << "AddOption: short option provided with a starting \"-\", it should have just been \""
                                   << cleanedShort << "\"" << std::endl;
    }
    position = cleanedLong.find("--");
    if (position == 0) {
        cleanedLong.erase(0, 2);
        CMN_LOG_CLASS_INIT_WARNING << "AddOption: long option provided with a starting \"--\", it should have just been \""
                                   << cleanedLong << "\"" << std::endl;
    }
    if (this->GetShortNoDash(cleanedShort)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddOption: option \"-" << cleanedShort << "\" already defined" << std::endl;
        return false;
    }
    if (this->GetLongNoDashDash(cleanedLong)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddOption: option \"--" << cleanedLong << "\" already defined" << std::endl;
        return false;
    }
    return true;
}


bool cmnCommandLineOptions::IsSet(const std::string & option)
{
    std::string cleanedOption = option;
    // remove up to 2 '-' in front of option name
    if (cleanedOption.find('-') == 0) {
        cleanedOption.erase(0, 1);
    }
    if (cleanedOption.find('-') == 0) {
        cleanedOption.erase(0, 1);
    }
    // try to find the option
    const OptionsType::const_iterator end = this->Options.end();
    OptionsType::const_iterator iter = this->Options.begin();
    for (; iter != end; ++iter) {
        if (((*iter)->Short == cleanedOption) || ((*iter)->Long == cleanedOption)) {
            return (*iter)->Set;
        }
    }
    return false;
}


void cmnCommandLineOptions::PrintParsedArguments(std::string & parsedArguments) const
{
    std::stringstream strstr;
    OptionBase * option;
    const OptionsType::const_iterator end = this->Options.end();
    OptionsType::const_iterator iter = this->Options.begin();
    for (; iter != end; ++iter) {
        option = *iter;
        if (option->Set) {
            strstr << " - " << option->Description << " [" << option->PrintValues() << "]" << std::endl;
        }
    }
    parsedArguments = strstr.str();
}

cmnCommandLineOptions::OptionBase * cmnCommandLineOptions::Get(const std::string & option)
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
