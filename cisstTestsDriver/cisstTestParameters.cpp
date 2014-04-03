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


#include "cisstTestParameters.h"
#include <string.h> // for strcmp
#include <stdlib.h> // for atoi

void cisstTestParameters::ParseCmdLine(int argc, const char * argv[])
{
    ProgramName = argv[0];
    const size_t size = ProgramName.size();
    size_t index;
    for(index = 0; index < size; index++) {
        if (ProgramName[index] == '\\') {
            ProgramName[index]= '/';
        }
    }

    while (argc != 1) {
        if (argc < 1) {
            TestRunMode = PRINT_HELP;
            return;
        }

        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            TestRunMode = PRINT_HELP;
            return;
        }

        if (strcmp(argv[1], "--run") == 0 || strcmp(argv[1], "-r") == 0) {
            TestRunMode = RUN_TESTS;
            ++argv;
            --argc;
            continue;
        }

        if (strcmp(argv[1], "--listandrun") == 0 || strcmp(argv[1], "-R") == 0) {
            TestRunMode = RUN_AND_LIST_TESTS;
            ++argv;
            --argc;
            continue;
        }

        if (strcmp(argv[1], "--list") == 0 || strcmp(argv[1], "-l") == 0) {
            TestRunMode = LIST_TESTS;
            ++argv;
            --argc;
            continue;
        }

        if (strcmp(argv[1], "--dart") == 0 || strcmp(argv[1], "-d") == 0) {
            TestRunMode = GENERATE_CTEST_FILE;
            ++argv;
            --argc;
            continue;
        }

        if (strcmp(argv[1], "--numinstances") == 0 || strcmp(argv[1], "-o") == 0) {
            NumTestInstances = atoi(argv[2]);
            argv += 2;
            argc -= 2;
            continue;
        }

        if (strcmp(argv[1], "--numiterations") == 0 || strcmp(argv[1], "-i") == 0) {
            NumTestIterations = atoi(argv[2]);
            argv += 2;
            argc -= 2;
            continue;
        }

        if (strcmp(argv[1], "--testname") == 0 || strcmp(argv[1], "-t") == 0) {
            TestNames.push_back( argv[2] );
            argv += 2;
            argc -= 2;
            continue;
        }

        TestRunMode = PRINT_HELP;
        return;
    }
}


/*! Print help message. */
int cisstTestParameters::PrintHelp(const char* programName) {
    std::cerr
        << programName << ": Usage" << std::endl
        << "-h, --help               print this message" << std::endl
        << "-l, --list               print the available test instances" << std::endl
        << "-d, --dart               print CMake/ctest commands in DartTestfile.txt format" << std::endl 
        << "-r, --run                run the available test instances" << std::endl
        << "-R, --listandrun         print and run the available test instances" << std::endl
        << "-t, --testname [name]    add the specified test case or suite to the list" << std::endl
        << "-o, --numinstances [n]   specify the number of instances to create of each test" << std::endl
        << "-i, --numiterations [n]  specify the number of iterations for each test instance" << std::endl;

    std::cerr << std::endl << std::endl;
    std::cerr
        << "If no names are specified, all tests are instantiated." << std::endl
        << "Otherwise, the instantiated tests are those whose names or suite" << std::endl
        << "were listed and which appear in the test registry." << std::endl
        << "The number of instances and number of iterations is 1 by default" << std::endl;


    return 0;
}

