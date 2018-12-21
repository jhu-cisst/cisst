/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2004-08-31

  (C) Copyright 2004-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <string.h>
#include <fstream>

#include <cisstRevision.h>
#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnClassRegister.h>

// to provide some information regarding this build
#include <cisstCommon/cmnPath.h>
#include <cisstCommon/cmnUnits.h>

// Whether cmnLogger instance has been created. Since this is a built-in type,
// it should be initialized early.
bool cmnLogger::InstanceCreated = false;

// Default log file name is "cisstLog.txt" (in current directory)
std::string cmnLogger::DefaultLogFileName("cisstLog.txt");

bool cmnLogger::SetDefaultLogFileName(const std::string & defaultLogFileName)
{
    if (!IsCreated()) {
        DefaultLogFileName = defaultLogFileName;
        return true;
    }
    return false;
}

cmnLogger::cmnLogger(const std::string & defaultLogFileName):
    Mask(CMN_LOG_ALLOW_ALL),
    FunctionMask(CMN_LOG_ALLOW_ERRORS),
    LoDMultiplexerStreambuf()
{
    cmnLogger::InstanceCreated = true;
    LoDMultiplexerStreambuf.AddChannel(*(DefaultLogFile(defaultLogFileName)), CMN_LOG_ALLOW_DEFAULT);
    *(DefaultLogFile()) << cmnLogLevelToString(CMN_LOG_LEVEL_INIT_VERBOSE) << " " << CISST_FULL_REVISION << std::endl;
    std::string result = "undefined";
    cmnPath::GetCisstRoot(result);
    *(DefaultLogFile()) << cmnLogLevelToString(CMN_LOG_LEVEL_INIT_VERBOSE) << " CISST_ROOT: " << result << std::endl;
    result = "undefined";
    cmnPath::GetCisstShare(result);
    *(DefaultLogFile()) << cmnLogLevelToString(CMN_LOG_LEVEL_INIT_VERBOSE) << " cisst share: " << result << std::endl;
    *(DefaultLogFile()) << cmnLogLevelToString(CMN_LOG_LEVEL_INIT_VERBOSE) << " cmn_m: " << cmn_m << ", cmn_kg: " << cmn_kg << ", CISST_USE_SI_UNITS is set to " << CISST_USE_SI_UNITS << std::endl;
}


cmnLogger * cmnLogger::Instance(void)
{
    // create a static variable, i.e. singleton
    static cmnLogger instance;
    return &instance;
}


void cmnLogger::SetMaskInstance(cmnLogMask mask)
{
    CMN_LOG_INIT_VERBOSE << "Class cmnLogger: SetMask: overall mask set to \"" << cmnLogMaskToString(mask) << "\"" << std::endl;
    Mask = mask;
}


cmnLogMask cmnLogger::GetMaskInstance(void)
{
    return Mask;
}


void cmnLogger::SetMaskFunctionInstance(cmnLogMask mask)
{
    CMN_LOG_INIT_VERBOSE << "Class cmnLogger: SetMaskFunction: function mask set to \"" << cmnLogMaskToString(mask) << "\"" << std::endl;
    FunctionMask = mask;
}


cmnLogMask cmnLogger::GetMaskFunctionInstance(void)
{
    return FunctionMask;
}


cmnLogger::StreamBufType * cmnLogger::GetMultiplexerInstance(void)
{
    return &(LoDMultiplexerStreambuf);
}


std::ofstream * cmnLogger::DefaultLogFile(const std::string & defaultLogFileName)
{
    static std::ofstream defaultLogFile(defaultLogFileName.c_str());
    return &defaultLogFile;
}


void cmnLogger::HaltDefaultLogInstance(void)
{
    LoDMultiplexerStreambuf.RemoveChannel(*(DefaultLogFile()));
}


void cmnLogger::ResumeDefaultLogInstance(cmnLogMask newMask)
{
    LoDMultiplexerStreambuf.AddChannel(*(DefaultLogFile()), newMask);
}


void cmnLogger::AddChannelInstance(std::ostream & outputStream, cmnLogMask mask)
{
    LoDMultiplexerStreambuf.AddChannel(outputStream, mask);
}


void cmnLogger::RemoveChannelInstance(std::ostream & outputStream)
{
    LoDMultiplexerStreambuf.RemoveChannel(outputStream);
}


bool cmnLogger::SetMaskClass(const std::string & className, cmnLogMask mask)
{
    return cmnClassRegister::SetLogMaskClass(className, mask);
}


bool cmnLogger::SetMaskClassAll(cmnLogMask mask)
{
    return cmnClassRegister::SetLogMaskClassAll(mask);
}


bool cmnLogger::SetMaskClassMatching(const std::string & stringToMatch, cmnLogMask mask)
{
    return cmnClassRegister::SetLogMaskClassMatching(stringToMatch, mask);
}

const char * cmnLogger::ExtractFileName(const char * file)
{
    const char * p1 = strrchr(file, '/');
#if (CISST_OS == CISST_WINDOWS)
    const char * p2 = strrchr(file, '\\');
    if (p2 > p1) {
        return p2 + 1;
    }
#endif
    if (p1) {
        return p1 + 1;
    }
    return file;
}


void cmnLogger::KillInstance(void)
{
    cmnLogger::SetMaskClassAll(CMN_LOG_ALLOW_NONE);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_NONE);
    cmnLogger::SetMask(CMN_LOG_ALLOW_NONE);
    LoDMultiplexerStreambuf.RemoveAllChannels();
}
