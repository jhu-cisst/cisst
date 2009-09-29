/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cmnPath.cpp 433 2009-06-09 22:10:24Z adeguet1 $

  Author(s):  Anton Deguet
  Created on: 2005-04-18

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnPath.h>


CMN_IMPLEMENT_SERVICES(cmnPath);


cmnPath::cmnPath() {
    ConfigureTokenizer();
    Path.clear();
}


cmnPath::cmnPath(const std::string & path) {
    ConfigureTokenizer();
    Set(path);
}


void cmnPath::ConfigureTokenizer(void) {
    Tokenizer.SetDelimiters(";");
    Tokenizer.SetEscapeMarkers("");
}


std::string cmnPath::FromNative(const std::string & nativePath) {
    std::string internalPath = nativePath; 
#if (CISST_OS == CISST_WINDOWS)
    
#else
    const std::string::iterator end = internalPath.end();
    std::string::iterator iterator;
    for (iterator = internalPath.begin();
         iterator != end;
         iterator++) {
        if (*iterator == ':') {
            *iterator = ';';
        }
    }
#endif
    return internalPath;
}

void cmnPath::Set(const std::string & path) {
    Path.clear();
    Add(path);
}


void cmnPath::Add(const std::string & path, bool head) {
    Tokenizer.Parse(path);
    CMN_LOG_CLASS_INIT_VERBOSE << "Adding the following \""
                               << path << "\" at the "
                               << (head ? "beginning" : "end") << std::endl;
    const char* const* tokens = Tokenizer.GetTokensArray(); 
    iterator position = head ? Path.begin() : Path.end();
    while (*tokens != NULL) {
        Path.insert(position, std::string(*tokens));
        // std::cout << *tokens << std::endl;
        *tokens ++;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "Path is now set to: " << ToString() << std::endl;
}


void cmnPath::AddFromEnvironment(const std::string & variableName, bool head) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Adding path defined by the environment variable \""
                               << variableName << "\" at the "
                               << (head ? "beginning" : "end") << std::endl;
    char * environmentVariable = 0;
    environmentVariable = getenv(variableName.c_str());
    if (environmentVariable) {
        std::string path = environmentVariable;
        this->Add(FromNative(path), head);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "The environment variable \"" << variableName
                                 << "\" doesn't seem to be defined" << std::endl;
        return;
    }
}
    

std::string cmnPath::Find(const std::string & filename, short mode) const {
    std::string fullName("");
    const_iterator iter = Path.begin();
    const const_iterator end = Path.end(); 
    while (iter != end) {
        fullName = (*iter) + "/" + filename;
        if (access(fullName.c_str(), mode) == 0) {
            break;
        }
        ++iter;
    }
    if (iter == end) {
        CMN_LOG_CLASS_RUN_WARNING << "Couldn't find file \"" << filename << "\" in path " << ToString() << std::endl;
        return "";
    }
    CMN_LOG_CLASS_RUN_VERBOSE << "Found \"" << fullName << "\" in path " << ToString() << std::endl;
    return fullName;
}


bool cmnPath::Remove(const std::string & directory) {
    iterator iter;
    iter = std::find(Path.begin(), Path.end(), directory);
    if (iter != Path.end()) {
        Path.erase(iter);
        CMN_LOG_CLASS_INIT_VERBOSE << "Directory " << directory << " found and removed from current path. "
                                   << "Path is now set to: " << ToString() << std::endl;
        return true;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "Directory " << directory << " not found in current path." << std::endl;
    return false;	
}


bool cmnPath::Has(const std::string & directory) const {
    const_iterator iter;
    iter = std::find(Path.begin(), Path.end(), directory);
    if (iter != Path.end()) {
        return true;
    }
    return false;
}


void cmnPath::ToStream(std::ostream & outputStream) const {
    const unsigned int size = Path.size();
    unsigned int index;
    const_iterator iter = Path.begin();
    for (index = 0; index < size; index++) {
        outputStream << *iter;
        if (index < (size - 1)) {
            outputStream << ";";
        }
        iter++;
    }
}

const std::string &
cmnPath::DirectorySeparator()
{
#if (CISST_OS == CISST_WINDOWS)
    static const std::string separator = "\\";
#else
    static const std::string separator = "/";
#endif
    return separator;
}

