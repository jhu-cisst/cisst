/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---

*/

#include "cdgFile.h"

enum cdgScopeType {CDG_GLOBAL,
                   CDG_CLASS,
                   CDG_TYPEDEF,
                   CDG_MEMBER,
                   CDG_CODE};

const std::string cdgScopeNames[] = {"global",
                                     "class",
                                     "member",
                                     "code"};

bool cdgFile::ParseFile(std::ifstream & input, const std::string & filename)
{
    bool errorFound = false; // any error found, stop parsing and return false
    std::string errorMessage;
    std::stringstream parsedOutput; // for debug
    size_t lineNumber = 1; // counter for debug messages
    char currentChar, previousChar = ' '; // we read character by character, used to detect //
    std::string word, temp; // each word is created by concatenating characters
    bool wordFinished, lineFinished; // start real processing or upgrade line count
    bool semiColumn, curlyOpen, curlyClose; // special characters
    std::list<cdgScopeType> scopes; // scope being parsed
    std::string keyword, value; // parses "keyword <value .....>;"
    bool curlyOpenExpected = false;
    size_t curlyCounter = 0; // for code snippets, need to balance {};
    // global initialization
    scopes.push_back(CDG_GLOBAL);
    keyword.clear();
    value.clear();

    // objects created
    cdgClass * currentClass;
    cdgTypedef * currentTypedef;
    cdgMember * currentMember;

    // parsing character by character
    while (input.get(currentChar) && !errorFound) {
        semiColumn = false;
        curlyOpen = false;
        curlyClose = false;

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
        }
        if (curlyClose) {
            parsedOutput << ']';
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
                switch (scopes.back()) {
                case CDG_GLOBAL:
                    // first define the current step, either find new keyword or new scope
                    if (keyword.empty()) {
                        value.clear();
                        if (word == "include") {
                            keyword = "include";
                        } else if (word == "class") {
                            keyword.clear();
                            scopes.push_back(CDG_CLASS);
                            currentClass = new cdgClass;
                            Classes.push_back(currentClass);
                            if (!curlyOpen) {
                                curlyOpenExpected = true;
                            }
                        } else {
                            std::cerr << filename << ":" << lineNumber << ": error: unexpected keyword \""
                                      << word << "\" in scope \"" << cdgScopeNames[scopes.back()]
                                      << "\"" << std::endl;
                            errorFound = true;
                        }
                    } else {
                        value += word + " ";
                        if (semiColumn) {
                            // we are done for this set keyword/value
                            RemoveTrailingSpaces(value);
                            this->Includes.push_back(value);
                            keyword.clear();
                        }
                    }
                    word.clear();
                    break;


                case CDG_CLASS:
                    // first define the current step, either find new keyword or new scope
                    if (keyword.empty()) {
                        value.clear();
                        if (word == "name") {
                            keyword = "name";
                        } else if (word == "default-log-lod") {
                            keyword = "default-log-lod";
                        } else if (word == "typedef") {
                            keyword.clear();
                            scopes.push_back(CDG_TYPEDEF);
                            currentTypedef = new cdgTypedef;
                            currentClass->Typedefs.push_back(currentTypedef);
                            if (!curlyOpen) {
                                curlyOpenExpected = true;
                            }
                        } else if (word == "member") {
                            keyword.clear();
                            scopes.push_back(CDG_MEMBER);
                            currentMember = new cdgMember;
                            currentClass->Members.push_back(currentMember);
                            if (!curlyOpen) {
                                curlyOpenExpected = true;
                            }
                        } else if ((word == "header-snippet")
                                   || (word == "code-snippet")) {
                            keyword = word;
                            scopes.push_back(CDG_CODE);
                            if (!curlyOpen) {
                                curlyOpenExpected = true;
                            }
                        } else if (!word.empty()) {
                            std::cerr << filename << ":" << lineNumber << ": error: unexpected keyword \""
                                      << word << "\" in scope \"" << cdgScopeNames[scopes.back()]
                                      << "\"" << std::endl;
                            errorFound = true;
                        }
                        if (curlyClose) {
                            scopes.pop_back();
                        }
                    } else {
                        value += word + " ";
                        if (semiColumn) {
                            // we are done for this set keyword/value
                            RemoveTrailingSpaces(value);
                            if (keyword == "name") {
                                currentClass->Name = value;
                            } else if (keyword == "default-log-lod") {
                                currentClass->DefaultLogLoD = value;
                            } else {
                                std::cerr << filename << ":" << lineNumber << ": error: unhandled keyword \""
                                          << word << "\" in scope \"" << cdgScopeNames[scopes.back()]
                                          << "\"" << std::endl;
                                errorFound = true;
                            }
                            keyword.clear();
                        }
                    }
                    word.clear();
                    break;


                case CDG_MEMBER:
                    // first define the current step, either find new keyword or new scope
                    if (keyword.empty()) {
                        value.clear();
                        if (currentMember->IsKeyword(word)) {
                            keyword = word;
                        } else if (!word.empty()) {
                            std::cerr << filename << ":" << lineNumber << ": error: unexpected keyword \""
                                      << word << "\" in scope \"" << cdgScopeNames[scopes.back()]
                                      << "\"" << std::endl;
                            errorFound = true;
                        }
                        if (curlyClose) {
                            if (!currentMember->IsValid(errorMessage)) {
                                std::cerr << filename << ":" << lineNumber << ": error: " << errorMessage
                                          << " in scope \"" << cdgScopeNames[scopes.back()] << "\"" << std::endl;
                                errorFound = true;
                            }
                            scopes.pop_back();
                        }
                    } else {
                        value += word + " ";
                        if (semiColumn) {
                            // we are done for this set keyword/value
                            RemoveTrailingSpaces(value);
                            if (!currentMember->SetValue(keyword, value, errorMessage)) {
                                std::cerr << filename << ":" << lineNumber << ": error: " << errorMessage
                                          << " in scope \"" << cdgScopeNames[scopes.back()] << "\"" << std::endl;
                                errorFound = true;
                            }
                            keyword.clear();
                        }
                    }
                    word.clear();
                    break;


                case CDG_TYPEDEF:
                    // first define the current step, either find new keyword or new scope
                    if (keyword.empty()) {
                        value.clear();
                        if (currentTypedef->IsKeyword(word)) {
                            keyword = word;
                        } else if (!word.empty()) {
                            std::cerr << filename << ":" << lineNumber << ": error: unexpected keyword \""
                                      << word << "\" in scope \"" << cdgScopeNames[scopes.back()]
                                      << "\"" << std::endl;
                            errorFound = true;
                        }
                        if (curlyClose) {
                            if (!currentTypedef->IsValid(errorMessage)) {
                                std::cerr << filename << ":" << lineNumber << ": error: " << errorMessage
                                          << " in scope \"" << cdgScopeNames[scopes.back()] << "\"" << std::endl;
                                errorFound = true;
                            }
                            scopes.pop_back();
                        }
                    } else {
                        value += word + " ";
                        if (semiColumn) {
                            // we are done for this set keyword/value
                            RemoveTrailingSpaces(value);
                            if (!currentTypedef->SetValue(keyword, value, errorMessage)) {
                                std::cerr << filename << ":" << lineNumber << ": error: " << errorMessage
                                          << " in scope \"" << cdgScopeNames[scopes.back()] << "\"" << std::endl;
                                errorFound = true;
                            }
                            keyword.clear();
                        }
                    }
                    word.clear();
                    break;


                case CDG_CODE:
                default:
                    std::cerr << filename << ":" << lineNumber << ": error: invalid scope ("
                              << cdgScopeNames[scopes.back()] << ")" << std::endl;
                    errorFound = true;
                    break;
                } // end of switch scope

                // if we found some code to be parsed
                if (scopes.back() == CDG_CODE) {
                    if (curlyOpenExpected) {
                        curlyCounter = 0;
                    } else {
                        curlyCounter = 1;
                    }
                    value.clear();
                    while (input.get(currentChar) && (curlyOpenExpected || (curlyCounter > 0))) {
                        switch (currentChar) {
                        case '{':
                            curlyCounter++;
                            if (curlyOpenExpected) {
                                curlyOpenExpected = false;
                            }
                            break;
                        case '}':
                            curlyCounter--;
                            if (curlyCounter == 0) {
                                scopes.pop_back();
                                std::cout << keyword << " -> " << value << std::endl;
                                keyword.clear();
                            }
                            break;
                        default:
                            break;
                        }
                        value += currentChar;
                        parsedOutput << currentChar;
                    }
                    if (curlyOpenExpected) {
                        std::cerr << filename << ":" << lineNumber << ": error: missing opening \"}\""
                                  << " for \"" << keyword << "\" in scope \""
                                  << cdgScopeNames[scopes.back()] << "\""  << std::endl;
                        errorFound = true;
                    }
                } // end of code snippet parsing
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
            parsedOutput << '\n' << lineNumber << ": ";
        }
    } // while loop for character

    // some sanity checks
    if (scopes.back() != CDG_GLOBAL) {
        std::cerr << filename << ":" << lineNumber << ": error: invalid scope ("
                  << cdgScopeNames[scopes.back()] << ")" << std::endl;
    }
    if (curlyCounter != 0) {
        std::cerr << filename << ":" << lineNumber << ": error: missing closing \"}\"" << std::endl;
        errorFound = true;
    }

    // provides a parsed view which might help debug
    if (errorFound) {
        std::cerr << parsedOutput.str() << std::endl;
    }

    return !errorFound;
}


void cdgFile::GenerateHeader(std::ostream & outputStream) const
{
    size_t index;
    outputStream << "#ifndef _" << Name << "_h" << std::endl
                 << "#define _" << Name << "_h" << std::endl
                 << "#include <cisstMultiTask/mtsGenericObject.h>" << std::endl;

    for (index = 0; index < Includes.size(); index++) {
        outputStream << "#include " << Includes[index] << std::endl;
    }
    for (index = 0; index < Classes.size(); index++) {
        Classes[index]->GenerateHeader(outputStream);
    }
    outputStream << "#endif // _" << Name << "_h" << std::endl;
}


void cdgFile::GenerateCode(std::ostream & outputStream, const std::string & header) const
{
    size_t index;
    outputStream << "#include <" << header << ">" << std::endl
                 << "#include <cisstCommon/cmnSerializer.h>" << std::endl
                 << "#include <cisstCommon/cmnDeSerializer.h>" << std::endl;

    for (index = 0; index < Classes.size(); index++) {
        Classes[index]->GenerateCode(outputStream);
    }
}


void cdgFile::RemoveTrailingSpaces(std::string & value)
{
    while (value[(value.length() - 1)] == ' ') {
        value.resize(value.length() - 1);
    }
}
