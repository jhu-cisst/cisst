/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet, Ofri Sadowsky
  Created on: 2003-11-10

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _cisstTestParameters_h
#define _cisstTestParameters_h


#include <string>
#include <list>
#include <iostream>


class cisstTestParameters
{
public:
    typedef enum { PRINT_HELP, RUN_TESTS, LIST_TESTS, GENERATE_CTEST_FILE, RUN_AND_LIST_TESTS } TestRunModeType; 
    typedef std::list<std::string> TestNameContainerType;
private:
    TestNameContainerType TestNames;
    int NumTestInstances;
    int NumTestIterations;
    TestRunModeType TestRunMode;
    std::string ProgramName;

public:
    cisstTestParameters():
        TestNames(),
        NumTestInstances(1),
        NumTestIterations(1),
        TestRunMode(PRINT_HELP)
    {}

    void ParseCmdLine(int argc, const char * argv[]);

    const TestNameContainerType & GetTestNames() const
    {
        return TestNames;
    }

    TestRunModeType GetTestRunMode() const
    {
        return TestRunMode;
    }

    int GetNumInstances() const
    {
        return NumTestInstances;
    }

    int GetNumIterations() const
    {
        return NumTestIterations;
    }

    std::string GetProgramName(void) const {
        return ProgramName;
    }

    static int PrintHelp(const char * programName);
};


#endif // _cisstTestParameters_h

