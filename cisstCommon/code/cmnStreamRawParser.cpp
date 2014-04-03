/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Peter Kazanzides

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon/cmnStreamRawParser.h>

cmnStreamRawParser::~cmnStreamRawParser()
{
    KeyListType::iterator it;
    for (it = KeyList.begin(); it != KeyList.end(); it++)
        delete (*it);
}

void cmnStreamRawParser::SetAllValid(bool val)
{
    KeyListType::iterator it;
    for (it = KeyList.begin(); it != KeyList.end(); it++)
        (*it)->SetValid(val);
}

bool cmnStreamRawParser::Parse(std::istream & inputStream)
{
    KeyListType::iterator it;
    SetAllValid(false);
    while (inputStream.good()) {
        std::string inputKey;
        inputStream >> inputKey;
        if (inputStream.fail()) break;
        // Comment lines begin with #
        if (inputKey[0] == '#') {
            inputStream.ignore(256, '\n');  // ignore rest of line
            continue;
        }
        EntryBase eb(inputKey); // MJ: support for gcc 4.6 compilation (not to have -fpermissive)
        it = KeyList.find(&eb);
        if (it == KeyList.end()) {
            CMN_LOG_INIT_WARNING << "cmnStreamRawParser: unknown keyword '" << inputKey << "'" << std::endl;
            inputStream.ignore(256, '\n');  // skip rest of this line, attempt to continue
        }
        else if (!(*it)->Parse(inputStream)) {
            CMN_LOG_INIT_ERROR << "cmnStreamRawParser: error parsing data for " << inputKey << std::endl;
            inputStream.ignore(256, '\n');  // skip rest of this line, attempt to continue
        }
    }
    bool success = true;
    for (it = KeyList.begin(); it != KeyList.end(); it++) {
        if ((*it)->isRequired() && !(*it)->isValid()) {
            CMN_LOG_INIT_WARNING << "cmnStreamRawParser: missing data for " << (*it)->GetKey() << std::endl;
            success = false;
        }
    }
    return success;
}

bool cmnStreamRawParser::IsValid(const std::string &name) const
{
    KeyListType::const_iterator it;
    EntryBase eb(name); // MJ: support for gcc 4.6 compilation (not to have -fpermissive)
    it = KeyList.find(&eb);
    if (it == KeyList.end())
        return false;
    return (*it)->isValid();
}

void cmnStreamRawParser::ToStream(std::ostream & outputStream) const
{
    KeyListType::const_iterator it;
    for (it = KeyList.begin(); it != KeyList.end(); it++) {
        (*it)->ToStream(outputStream);
        outputStream << std::endl;
    }
}
