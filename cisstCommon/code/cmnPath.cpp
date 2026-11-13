/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2005-04-18

  (C) Copyright 2005-2025 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnPath.h>

cmnPath::cmnPath() {}

cmnPath::cmnPath(const std::filesystem::path& path) {
    Set(path);
}

cmnPath::cmnPath(const std::string& path) : cmnPath(pathFromUTF8(path)) {}


/* On Windows, paths in a environment variable are separated by ';',
   while UNIX-like separates with ':'*/
std::list<std::filesystem::path> splitEnvVar(const std::string& value) {
    std::list<std::filesystem::path> paths;
    std::stringstream ss(value);
    std::string item;

#if (CISST_OS == CISST_WINDOWS)
    char delimiter = ';';
#else
    char delimiter = ':';
#endif

    while (std::getline(ss, item, delimiter)) {
        if (!item.empty()) {
            paths.push_back(pathFromUTF8(item));
        }
    }

    return paths;
}


void cmnPath::Set(const std::filesystem::path& path) {
    Paths.clear();
    Add(path);
}


void cmnPath::Set(const std::string& path) {
    Set(pathFromUTF8(path));
}


void cmnPath::Add(const std::filesystem::path& path, Position position) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Adding the following \""
                               << path << "\" at the "
                               << ((position == Position::HEAD) ? "beginning" : "end") << std::endl;
    const_iterator iter = (position == Position::HEAD) ? Paths.begin() : Paths.end();
    Paths.insert(iter, path);
    CMN_LOG_CLASS_INIT_VERBOSE << "Path is now set to: " << ToString() << std::endl;
}


void cmnPath::Add(const std::string& path, Position position) {
    Add(pathFromUTF8(path), position);
}


bool cmnPath::AddFromEnvironment(const std::string& variableName, Position position) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Adding path defined by the environment variable \""
                               << variableName << "\" at the "
                               << ((position == Position::HEAD) ? "beginning" : "end") << std::endl;
    std::filesystem::path variable;
#if (CISST_OS == CISST_WINDOWS)
    const wchar_t* value = _wgetenv(pathFromUTF8(variableName).c_str());
#else
    const char* value = getenv(variableName.c_str());
#endif

    if (value) {
        variable = std::filesystem::path(value);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "The environment variable \"" << variableName
                                 << "\" doesn't seem to be defined" << std::endl;
        return false;
    }

    const auto paths = splitEnvVar(pathToUTF8(value));
    for (const auto& path : paths) {
        Add(path, position);
    }

    return true;
}


bool cmnPath::AddRelativeToCisstRoot(const std::filesystem::path& relativePath, Position position) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Adding path \""
                               << relativePath << "\" relative to CISST_ROOT at the "
                               << ((position == Position::HEAD) ? "beginning" : "end") << std::endl;
    std::filesystem::path path;
    if (!cmnPath::GetCisstRoot(path)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddRelativeToCisstRoot: the environment variable \"CISST_ROOT\" doesn't seem to be defined" << std::endl;
        return false;
    }
    path = path / relativePath;
    Add(path, position);
    return true;
}


bool cmnPath::AddRelativeToCisstRoot(const std::string& relativePath, Position position) {
    return AddRelativeToCisstRoot(pathFromUTF8(relativePath), position);
}


bool cmnPath::AddRelativeToCisstShare(const std::filesystem::path& relativePath, Position position) {
    CMN_LOG_CLASS_INIT_VERBOSE << "Adding path \""
                               << relativePath << "\" relative to CISST_ROOT/share/cisst-" << cisst_VERSION_MAJOR << "." << cisst_VERSION_MINOR << "/ at the "
                               << ((position == Position::HEAD) ? "beginning" : "end") << std::endl;
    std::filesystem::path path;
    if (!cmnPath::GetCisstShare(path)) {
        CMN_LOG_CLASS_INIT_ERROR << "AddRelativeToCisstShare: the environment variable \"CISST_ROOT\" doesn't seem to be defined" << std::endl;
        return false;
    }

    path = path / relativePath;
    Add(path, position);
    return true;
}


bool cmnPath::AddRelativeToCisstShare(const std::string& relativePath, Position position) {
    return AddRelativeToCisstShare(pathFromUTF8(relativePath), position);
}


std::filesystem::path cmnPath::Find(const std::filesystem::path& filename) const
{
    return FindWithSubdirectory(pathToUTF8(filename), "");
}


std::filesystem::path cmnPath::Find(const std::string& filename) const
{
    return Find(pathFromUTF8(filename));
}


std::filesystem::path cmnPath::FindWithSubdirectory(
    const std::string& filename,
    const std::string& subdirectory
) const
{
    std::filesystem::path path = pathFromUTF8(filename);

    if (filename.empty()) {
        CMN_LOG_CLASS_RUN_WARNING << "FindWithSubdirectory called with empty filename" << std::endl;
        return path;
    }

    // first check if this file exists as absolute path
    if (path.is_absolute()
        && std::filesystem::exists(path)) {
        CMN_LOG_CLASS_RUN_VERBOSE << "Found \"" << filename << "\", it seems to be a valid absolute file name" << std::endl;
        return path;
    }

    std::filesystem::path fullName("");
    bool found = false;
    for (const auto& location : Paths) {
        if (subdirectory != "") {
            fullName = location / subdirectory / path;
            if (std::filesystem::exists(fullName)) {
                found = true;
                break;
            }
        }
        fullName = location / path;
        if (std::filesystem::exists(fullName)) {
            found = true;
            break;
        }
    }

    if (!found) {
        CMN_LOG_CLASS_RUN_WARNING << "Couldn't find file \"" << filename << "\" in path " << ToString() << std::endl;
        return "";
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "Found \"" << fullName << "\" in path " << ToString() << std::endl;
    return fullName;
}


bool cmnPath::Remove(const std::filesystem::path& directory) {
    iterator iter;
    iter = std::find(Paths.begin(), Paths.end(), directory);
    if (iter != Paths.end()) {
        Paths.erase(iter);
        CMN_LOG_CLASS_INIT_VERBOSE << "Directory " << directory << " found and removed from current path. "
                                   << "Path is now set to: " << ToString() << std::endl;
        return true;
    }
    CMN_LOG_CLASS_INIT_VERBOSE << "Directory " << directory << " not found in current path." << std::endl;
    return false;
}


bool cmnPath::Remove(const std::string& directory) {
    return Remove(pathFromUTF8(directory));
}


bool cmnPath::Has(const std::filesystem::path& directory) const {
    const_iterator iter;
    iter = std::find(Paths.begin(), Paths.end(), directory);
    if (iter != Paths.end()) {
        return true;
    }
    return false;
}

bool cmnPath::Has(const std::string& directory) const {
    return Has(pathFromUTF8(directory));
}

void cmnPath::ToStream(std::ostream& outputStream) const {
    const size_t size = Paths.size();
    const_iterator iter = Paths.begin();
    for (size_t index = 0; index < size; index++) {
        outputStream << pathToUTF8(*iter);
        if (index < (size - 1)) {
            outputStream << ";";
        }
        iter++;
    }
}


bool cmnPath::GetCisstRoot(std::filesystem::path& result)
{
#if (CISST_OS == CISST_WINDOWS)
    #include <wchar.h>
    const whcar_t* value = _wgetenv("CISST_ROOT");
    if (!value) { return false; }

    result = std::filesystem::path(value);
    return true;
#else
    const char* value = getenv("CISST_ROOT");
    if (!value) { return false; }

    result = std::filesystem::path(value);
    return true;
#endif
}


bool cmnPath::GetCisstShare(std::filesystem::path& result)
{
    std::filesystem::path root;
    if (!cmnPath::GetCisstRoot(root)) { return false; }

    std::stringstream dir_name;
    dir_name << "cisst-" << cisst_VERSION_MAJOR << "." << cisst_VERSION_MINOR;

    result = root / "share" / dir_name.str();
    return true;
}
