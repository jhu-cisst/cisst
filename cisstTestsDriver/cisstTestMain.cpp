/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2003-07-28

  (C) Copyright 2003-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/Test.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/RepeatedTest.h>
#include <cppunit/ui/text/TestRunner.h>

#include <string>
#include <list>
#include <iostream>

#include "cisstTestParameters.h"


CppUnit::Test* FindTestInTestSuite(CppUnit::Test* tests, const std::string& name) {
    // try to see if this is a TestSuite
    CppUnit::TestSuite* testSuite = dynamic_cast<CppUnit::TestSuite *>(tests);
    CppUnit::Test* testFound = NULL;

    // it's a suite, check all components
    if (testSuite != NULL) {
        if (testSuite->getName() == name) {
            return testSuite;
        } else {
            std::vector<CppUnit::Test*> allTestsVector = testSuite->getTests();
            std::vector<CppUnit::Test*>::iterator testIterator;
            for (testIterator = allTestsVector.begin();
                 testIterator != allTestsVector.end();
                 testIterator++) {
                testFound = FindTestInTestSuite(*testIterator, name);
                // abort the search if found
                if (testFound) {
                    return testFound;
                }
            }
        }
    } else {
        // it's a test, get the name and test
        if (tests->getName() == name) {
            return tests;
        }
    }
    return NULL;
}


CppUnit::Test * InstantiateTests(const cisstTestParameters::TestNameContainerType & testNames)
{
    CppUnit::Test * wholeRegistry = CppUnit::TestFactoryRegistry::getRegistry().makeTest();
    if (testNames.empty())
        return wholeRegistry;

    CppUnit::TestSuite * testSuite = new CppUnit::TestSuite("");
    cisstTestParameters::TestNameContainerType::const_iterator nameIterator
        = testNames.begin();
    while (nameIterator != testNames.end()) {
        CppUnit::Test * test = FindTestInTestSuite(wholeRegistry, (*nameIterator));
        if (test != NULL) {
            testSuite->addTest(test);
        } else {
            std::cerr << "Failed to instantiate " << (*nameIterator) << std::endl;
        }

        ++nameIterator;
    }
    return testSuite;
}


/*! List all available tests in a given test suite. */
int ListAllTestsInTestSuite(CppUnit::Test* tests) {
    int count = 0;
    // try to see if this is a TestSuite
    CppUnit::TestSuite* testSuite = dynamic_cast<CppUnit::TestSuite *>(tests);
    // it's a suite, check all components
    if (testSuite != NULL) {
        std::vector<CppUnit::Test*> allTestsVector = testSuite->getTests();
        std::vector<CppUnit::Test*>::iterator testIterator;
        for (testIterator = allTestsVector.begin();
             testIterator != allTestsVector.end();
             testIterator++) {
            count += ListAllTestsInTestSuite(*testIterator);
        }
    } else {
        // it's a test, get the name
        count++;
        std::cout << tests->getName() << std::endl;
    }
    return count;
}


/*! Recursion used to fill the CMake/ctest compatible list of tests. */
int GenerateCTestFile(CppUnit::Test * tests, const std::string & programName,
                      unsigned int iterations, unsigned int instances) {
    unsigned int count = 0;
    // try to see if this is a TestSuite
    CppUnit::TestSuite* testSuite = dynamic_cast<CppUnit::TestSuite *>(tests);
    // it's a suite, check all components
    if (testSuite != NULL) {
        std::vector<CppUnit::Test*> allTestsVector = testSuite->getTests();
        std::vector<CppUnit::Test*>::iterator testIterator;
        for (testIterator = allTestsVector.begin();
             testIterator != allTestsVector.end();
             testIterator++) {
            count += GenerateCTestFile(*testIterator, programName, iterations, instances);
        }
    } else {
        // it's a test, add it to the list
        count++;
        std::cout << "add_test (\"C++: "
                  << tests->getName() << "-i" << iterations << "-o" << instances
                  << "\" " << programName
                  << " -r -i " << iterations << " -o " << instances
                  << " -t " << tests->getName()
                  << ")" << std::endl
                  << "set_tests_properties (\"C++: "
                  << tests->getName() << "-i" << iterations << "-o" << instances
                  << "\" PROPERTIES TIMEOUT " << 300 << ")"
                  << std::endl; 
    }
    return count;
}


int main(int argc, const char *argv[])
{
    cisstTestParameters testParameters;
    testParameters.ParseCmdLine(argc, argv);

    if (testParameters.GetTestRunMode() == cisstTestParameters::PRINT_HELP) {
        return cisstTestParameters::PrintHelp(argv[0]);
    }

    CppUnit::TestSuite * allTests = new CppUnit::TestSuite("All Tests");
    size_t instanceCounter = testParameters.GetNumInstances();
    if (instanceCounter == 0) {
        std::cout << "Error, the number of instances must be at least 1" << std::endl;
        return 1;
    }
    // add the first instance so we can list all tests
    allTests->addTest( InstantiateTests(testParameters.GetTestNames()) );
    --instanceCounter;

    if (testParameters.GetTestRunMode() == cisstTestParameters::LIST_TESTS) {
        ListAllTestsInTestSuite(allTests);
        return 0;
    }

    if (testParameters.GetTestRunMode() == cisstTestParameters::GENERATE_CTEST_FILE) {
        GenerateCTestFile(allTests, testParameters.GetProgramName(), testParameters.GetNumIterations(), testParameters.GetNumInstances());
        return 0;
    }

    if (testParameters.GetTestRunMode() == cisstTestParameters::RUN_AND_LIST_TESTS) {
        int testCount = ListAllTestsInTestSuite(allTests);
        std::cout << "-------------------------------- Total " << testCount
                  << " test(s)" << std::endl;
    }

    // add remaining instance to run the tests
    for (; instanceCounter > 0; --instanceCounter) {
        allTests->addTest( InstantiateTests(testParameters.GetTestNames()) );
    }

    if (testParameters.GetTestRunMode() == cisstTestParameters::RUN_TESTS ||
        testParameters.GetTestRunMode() == cisstTestParameters::RUN_AND_LIST_TESTS) {
        CppUnit::RepeatedTest * repeatedTest =
            new CppUnit::RepeatedTest(allTests, testParameters.GetNumIterations());
        CppUnit::TextUi::TestRunner runner;
        runner.addTest(repeatedTest);
        bool wasSuccessful = runner.run();
        if (wasSuccessful) {
            return 0;
        } else {
            return 1;
        }
    }

    return cisstTestParameters::PrintHelp(argv[0]);
}
