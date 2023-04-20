/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2005-04-18

  (C) Copyright 2005-2023 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnPath.h>
#include <cstdio>

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
        tokens++;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "Path is now set to: " << ToString() << std::endl;
}


bool cmnPath::AddFromEnvironment(const std::string & variableName, bool head) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Adding path defined by the environment variable \""
                               << variableName << "\" at the "
                               << (head ? "beginning" : "end") << std::endl;
    char * environmentVariable = 0;
    environmentVariable = getenv(variableName.c_str());
    if (environmentVariable) {
        std::string path = environmentVariable;
        this->Add(FromNative(path), head);
        return true;
    }
    CMN_LOG_CLASS_INIT_ERROR << "The environment variable \"" << variableName
                             << "\" doesn't seem to be defined" << std::endl;
    return false;
}


bool cmnPath::AddRelativeToCisstRoot(const std::string & relativePath, bool head) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Adding path \""
                               << relativePath << "\" relative to CISST_ROOT at the "
                               << (head ? "beginning" : "end") << std::endl;
    std::string path;
    if (cmnPath::GetCisstRoot(path)) {
        path = path + "/" + relativePath;
        this->Add(FromNative(path), head);
        return true;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddRelativeToCisstRoot: the environment variable \"CISST_ROOT\" doesn't seem to be defined" << std::endl;
    return false;
}


bool cmnPath::AddRelativeToCisstShare(const std::string & relativePath, bool head) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Adding path \""
                               << relativePath << "\" relative to CISST_ROOT/share/cisst-" << CISST_VERSION_MAJOR << "." << CISST_VERSION_MINOR << "/ at the "
                               << (head ? "beginning" : "end") << std::endl;
    std::string path;
    if (cmnPath::GetCisstShare(path)) {
        path = path + "/" + relativePath;
        this->Add(FromNative(path), head);
        return true;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddRelativeToCisstShare: the environment variable \"CISST_ROOT\" doesn't seem to be defined" << std::endl;
    return false;
}


std::string cmnPath::Find(const std::string & filename, short mode) const
{
    return this->FindWithSubdirectory(filename, "", mode);
}


std::string cmnPath::FindWithSubdirectory(const std::string & filename,
                                          const std::string & subdirectory,
                                          short mode) const
{
    std::string fullName("");
    const_iterator iter = Path.begin();
    const const_iterator end = Path.end();
    // first check if this file exists as absolute path
    if ((filename.size() > 0)
        && (filename[0] == '/')
        && (access(filename.c_str(), mode) == 0)) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Found \"" << filename << "\", it seems to be a valid absolute file name" << std::endl;
        return filename;
    }
    while (iter != end) {
        if (subdirectory != "") {
            fullName = (*iter) + "/" + subdirectory + "/" + filename;
            if (access(fullName.c_str(), mode) == 0) {
                break;
            }
        }
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
    const size_t size = Path.size();
    size_t index;
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

#if (CISST_OS == CISST_WINDOWS)
    #include <direct.h> // for _getcwd
#else
    #include <unistd.h> // for getcwd
#endif

std::string cmnPath::GetWorkingDirectory(void)
{
    char * buffer = 0;
    // use getcwd without a buffer, getcwd will allocate memory for us
#if (CISST_OS == CISST_WINDOWS)
    buffer = _getcwd(0, 0);
#else
    buffer = getcwd(0, 0);
#endif
    std::string result(buffer);
    // remember to free buffer
    if (buffer) {
        free(buffer);
    }
    return result;
}


bool cmnPath::GetCisstRoot(std::string & result)
{
    char * environmentVariable = 0;
    environmentVariable = getenv("CISST_ROOT");
    if (environmentVariable) {
        result = environmentVariable;
        return true;
    }
    return false;
}


bool cmnPath::GetCisstShare(std::string & result)
{
    if (cmnPath::GetCisstRoot(result)) {
        std::stringstream tmp;
        tmp << result << "/share/cisst-" << CISST_VERSION_MAJOR << "." << CISST_VERSION_MINOR;
        result = tmp.str();
        return true;
    }
    return false;
}


bool cmnPath::Exists(const std::string & fullPath, short mode)
{
    if (access(fullPath.c_str(), mode) == 0) {
        return true;
    }
    return false;
}


bool cmnPath::DeleteFile(const std::string & fullPath)
{
    if (remove(fullPath.c_str()) == 0) {
        return true;
    }
    return false;
}


bool cmnPath::RenameFile(const std::string & fullPathOld,
                         const std::string & fullPathNew)
{
    if (rename(fullPathOld.c_str(), fullPathNew.c_str()) == 0) {
        return true;
    }
    return false;
}
