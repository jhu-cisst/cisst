/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2004-08-31

  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <fstream>

#include <cisstCommon/cmnLogger.h>


cmnLogger::cmnLogger(const char * defaultLogFileName):
    LoD(10),
    LoDMultiplexerStreambuf()
{
    LoDMultiplexerStreambuf.AddChannel(*(DefaultLogFile(defaultLogFileName)), 5);
}


std::ofstream* cmnLogger::DefaultLogFile(const char * defaultLogFileName) {
    static std::ofstream defaultLogFile(defaultLogFileName);
    return &defaultLogFile;
}


cmnLogger* cmnLogger::Instance(void) {
    // create a static variable
    static cmnLogger instance;
    return &instance;
}

