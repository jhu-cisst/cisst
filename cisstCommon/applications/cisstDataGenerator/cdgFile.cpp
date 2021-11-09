/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010-2020 Johns Hopkins University (JHU), All Rights Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---

*/

#include "cdgFile.h"

size_t cmnQueryReplace(std::string & userString, const std::string & queryString, const std::string & replaceString)
{
    size_t counter = 0;
    size_t position = 0;
    while ((position = userString.find(queryString, position)) != std::string::npos) {
        userString.replace(position, queryString.length(), replaceString);
        position += replaceString.length();
        counter++;
    }
    return counter;
}


void cdgFile::SetHeader(const std::string & filename)
{
    Header = filename;
    HeaderGuard = filename;
    cmnQueryReplace(HeaderGuard, "/", "_");
    cmnQueryReplace(HeaderGuard, ".", "_");
    cmnQueryReplace(HeaderGuard, "\\", "_");
}


bool cdgFile::ParseFile(std::ifstream & input, const std::string & filename)
{
    Filename = filename;

    bool errorFound = false; // any error found, stop parsing and return false
    std::string errorMessage;
    std::stringstream parsedOutput; // for debug
    int parsedOutputIndentCounter = 0;
    std::string parsedOutputIndent;
    size_t lineNumber = 1; // counter for debug messages
    char currentChar, previousChar = ' '; // we read character by character, used to detect //
    std::string word, temp; // each word is created by concatenating characters
    bool wordFinished, lineFinished; // start real processing or upgrade line count
    bool semiColumn, curlyOpen, curlyClose; // special characters
    std::string keyword, value; // parses "keyword <value .....>;"
    bool curlyOpenExpected = false;
    size_t curlyCounter = 0; // for inline code, need to balance {};
    // global initialization
    cdgScope::Stack scopes; // scope being parsed
    this->Global = new cdgGlobal(lineNumber);
    scopes.push_back(this->Global);
    keyword.clear();
    value.clear();

    // parsing character by character
    while (input.get(currentChar) && !errorFound) {
        semiColumn = false;
        curlyOpen = false;
        curlyClose = false;
        lineFinished = false;

        // find curly brackets
        switch (currentChar) {
        case '/':
            if (previousChar == '/') {
                word.resize(word.size() - 1); // we had added previous '/' to a word
                wordFinished = true;
                getline(input, temp); // read and ignore comments until end of line
                lineFinished = true;
            } else {
                word = word + currentChar;
                wordFinished = false;
            }
            break;
        case '\n':
            lineFinished = true;
            wordFinished = true;
            break;
        case ' ':
        case '\t':
        case '\r':
            wordFinished = true;
            break;
        case ';':
            semiColumn = true;
            wordFinished = true;
            break;
        case '{':
            curlyOpen = true;
            wordFinished = true;
            break;
        case '}':
            curlyClose = true;
            wordFinished = true;
            break;
        default:
            word = word + currentChar;
            wordFinished = false;
        }

        // parsed output for debug
        if (wordFinished && !word.empty()) {
            parsedOutput << '\'' << word << '\'';
        }
        if (curlyOpen) {
            parsedOutput << '[';
            parsedOutputIndentCounter++;
            parsedOutputIndent.resize(parsedOutputIndentCounter * 4, ' ');
        }
        if (curlyClose) {
            parsedOutput << ']';
            parsedOutputIndentCounter--;
            if (parsedOutputIndentCounter < 0) {
                parsedOutputIndentCounter = 0;
            }
            parsedOutputIndent.resize(parsedOutputIndentCounter * 4, ' ');
        }
        if (semiColumn) {
            parsedOutput << '|';
        }

        // process based on flags and scope if there is anything to do
        if (curlyOpen || curlyClose || semiColumn || (wordFinished && !word.empty())) {
            if (curlyOpenExpected) {
                if (!curlyOpen) {
                    std::cerr << filename << ":" << lineNumber << ": error: \"{\" expected, not \""
                              << word << "\"" << std::endl;
                    errorFound = true;
                } else {
                    curlyOpenExpected = false;
                }
            } else {
                switch (scopes.back()->GetScope()) {
                case cdgScope::CDG_GLOBAL:
                case cdgScope::CDG_CLASS:
                case cdgScope::CDG_BASECLASS:
                case cdgScope::CDG_MEMBER:
                case cdgScope::CDG_ENUM:
                case cdgScope::CDG_ENUMVALUE:
                case cdgScope::CDG_TYPEDEF:
                    // first define the current step, either find new keyword or new scope
                    if (keyword.empty()) {
                        value.clear();
                        if (scopes.back()->HasField(word)) {
                            keyword = word;
                        } else if (scopes.back()->HasSubScope(word, scopes, lineNumber)) {
                            keyword.clear();
                            if (!curlyOpen) {
                                curlyOpenExpected = true;
                            }
                        } else if (!word.empty()) {
                            std::cerr << filename << ":" << lineNumber << ": error: unexpected keyword \""
                                      << word << "\" in " << std::endl;
                            scopes.back()->DisplaySyntax(std::cerr, 0, false);
                            std::cerr << std::endl;
                            errorFound = true;
                        }
                        if (curlyClose) {
                            if (!scopes.back()->IsValid(errorMessage)) {
                                std::cerr << filename << ":" << lineNumber << ": error: " << errorMessage
                                          << " in scope \"" << scopes.back()->GetScopeName() << "\"" << std::endl;
                                errorFound = true;
                            }
                            scopes.pop_back();
                            if (!scopes.back()) {
                                std::cerr << filename << ":" << lineNumber << ": error: found one too many closing \"}\""
                                          << std::endl;
                                errorFound = true;
                            }
                        }
                    } else {
                        value += word + " ";
                        if (semiColumn) {
                            // we are done for this set keyword/value
                            RemoveTrailingSpaces(value);
                            errorMessage.clear();
                            if (!scopes.back()->SetFieldValue(keyword, value, errorMessage)) {
                                std::cerr << filename << ":" << lineNumber << ": error: " << errorMessage
                                          << " in scope \"" << scopes.back()->GetScopeName() << "\"" << std::endl;
                                errorFound = true;
                            }
                            keyword.clear();
                        }
                    }
                    word.clear();
                    break;


                case cdgScope::CDG_CODE:
                default:
                    std::cerr << filename << ":" << lineNumber << ": error: invalid scope ("
                              << scopes.back()->GetScopeName() << ")" << std::endl;
                    errorFound = true;
                    break;
                } // end of switch scope

                // if we found some code to be parsed
                if (scopes.back()->GetScope() == cdgScope::CDG_CODE) {
                    if (curlyOpenExpected) {
                        lineNumber++;
                        curlyCounter = 0;
                    } else {
                        curlyCounter = 1;
                    }
                    value.clear();
                    while (input.get(currentChar) && (curlyOpenExpected || (curlyCounter > 0))) {
                        switch (currentChar) {
                        case '\n':
                            lineNumber++;
                            value += currentChar;
                            break;
                        case '{':
                            curlyCounter++;
                            if (curlyOpenExpected) {
                                curlyOpenExpected = false;
                            } else {
                                value += currentChar;
                            }
                            break;
                        case '}':
                            curlyCounter--;
                            if (curlyCounter == 0) {
                                errorMessage.clear();
                                if (!scopes.back()->SetFieldValue(keyword, value, errorMessage)) {
                                    std::cerr << filename << ":" << lineNumber << ": error: " << errorMessage
                                              << " in scope \"" << scopes.back()->GetScopeName() << "\"" << std::endl;
                                    errorFound = true;
                                }
                                scopes.pop_back();
                                keyword.clear();
                            }
                            value += currentChar;
                            break;
                        default:
                            value += currentChar;
                            break;
                        }
                        parsedOutput << currentChar;
                    }
                    if (curlyOpenExpected) {
                        std::cerr << filename << ":" << lineNumber << ": error: missing opening \"}\""
                                  << " for \"" << keyword << "\" in scope \""
                                  << scopes.back()->GetScopeName() << "\""  << std::endl;
                        errorFound = true;
                    }
                } // end of inline code parsing
            }
        }

        // general settings for next iteration
        if (wordFinished) {
            previousChar = ' ';
        } else {
            previousChar = currentChar;
        }
        if (lineFinished) {
            lineNumber++;
            lineFinished = false;
            parsedOutput << '\n' << lineNumber << ": " << parsedOutputIndent;
        }
    } // while loop for character

    // some sanity checks
    if (scopes.back()->GetScope() != cdgScope::CDG_GLOBAL) {
        std::cerr << filename << ":" << lineNumber << ": error: invalid scope ("
                  << scopes.back()->GetScopeName() << ")" << std::endl;
        errorFound = true;
    }
    if (curlyCounter != 0) {
        std::cerr << filename << ":" << lineNumber << ": error: missing closing \"}\"" << std::endl;
        errorFound = true;
    }

    // provides a parsed view which might help debug
    if (errorFound) {
        std::cerr << parsedOutput.str() << std::endl;
    } else {
        // fill in default values
        scopes.back()->FillInDefaults();
    }

    return !errorFound;
}


bool cdgFile::Validate(std::string & errorMessage)
{
    return this->Global->ValidateRecursion(errorMessage);
}


void cdgFile::RemoveTrailingSpaces(std::string & value)
{
    while (value[(value.length() - 1)] == ' ') {
        value.resize(value.length() - 1);
    }
}


void cdgFile::GenerateMessage(std::ostream & outputStream) const
{
    outputStream << "// file automatically generated, do not modify" << std::endl
                 << "// cisst version: " << CISST_VERSION << std::endl
                 << "// source file: " << Filename << std::endl << std::endl;
}


void cdgFile::GenerateHeader(std::ostream & outputStream) const
{
    GenerateMessage(outputStream);

    outputStream << "#pragma once" << std::endl
                 << "#ifndef _" << HeaderGuard << std::endl
                 << "#define _" << HeaderGuard << std::endl << std::endl;

    outputStream << "#include <cisstCommon/cmnDataFunctions.h>" << std::endl
                 << "#include <cisstCommon/cmnDataFunctionsEnumMacros.h>" << std::endl
                 << "#if CISST_HAS_JSON" << std::endl
                 << "#include <cisstCommon/cmnDataFunctionsJSON.h>" << std::endl
                 << "#endif // CISST_HAS_JSON" << std::endl
                 << std::endl;

    this->Global->GenerateIncludes(outputStream);
    this->Global->GenerateHeader(outputStream);

    outputStream << std::endl << "#endif // _" << HeaderGuard << std::endl;
}


void cdgFile::GenerateCode(std::ostream & outputStream) const
{
    GenerateMessage(outputStream);

    outputStream << "#include <" << Header << ">" << std::endl
                 << "#include <cisstCommon/cmnDataFunctionsMacros.h>" << std::endl
                 << "#include <cisstCommon/cmnDataFunctionsEnumMacros.h>" << std::endl
                 << "#include <cisstCommon/cmnDataFunctionsString.h>" << std::endl
                 << "#include <cisstCommon/cmnDataFunctionsVector.h>" << std::endl
                 << "#include <cisstCommon/cmnDataFunctionsList.h>" << std::endl
                 << std::endl;
    this->Global->GenerateCode(outputStream);
}


void cdgFile::DisplaySyntax(std::ostream & outputStream) const
{
    cdgGlobal * global = new cdgGlobal(0);
    outputStream << "File syntax:" << std::endl;
    global->DisplaySyntax(outputStream, 0, true, true); // no offset, recursive and hide top scope
    delete global;
}
