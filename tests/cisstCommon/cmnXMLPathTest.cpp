/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2011-05-26

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnXMLPathTest.h"

#include <cisstCommon/cmnXMLPath.h>
#include <cisstCommon/cmnPath.h>

void cmnXMLPathTest::TestReadExistingFile(void)
{
    cmnPath filePath;
    filePath.Add(std::string(CISST_SOURCE_ROOT) + "/tests/cisstCommon");
    std::string testFile = filePath.Find("cmnXMLPathTestFile1.xml");
    CPPUNIT_ASSERT(!(testFile == ""));

    cmnXMLPath xmlPath;
    xmlPath.SetInputSource(testFile);

    // start reading
    int intValue;
    bool boolValue = true;
    double doubleValue;
    std::string stringValue;
    bool dataFound;

    // read using const char * for path and context
    dataFound = xmlPath.GetXMLValue("data-1/data-1-1", "@intAttribute", intValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(5, intValue);

    // slightly different way (!= context/path)
    dataFound = xmlPath.GetXMLValue("data-1", "data-1-1/@boolAttribute", boolValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(false, boolValue);

    // slightly different way (!= context/path)
    dataFound = xmlPath.GetXMLValue("", "data-1/data-1-1/@doubleAttribute", doubleValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(1.234, doubleValue);

    // testing inf and -inf
    dataFound = xmlPath.GetXMLValue("data-1/data-1-1", "@doubleAttributeInf", doubleValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(std::numeric_limits<double>::infinity(), doubleValue);
    dataFound = xmlPath.GetXMLValue("data-1/data-1-1", "@doubleAttributeMinusInf", doubleValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(-1 * std::numeric_limits<double>::infinity(), doubleValue);

    // testing string
    dataFound = xmlPath.GetXMLValue("data-1/data-1-1", "@stringAttribute", stringValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(std::string("Hello!"), stringValue);

    // testing invalid context/path
    dataFound = xmlPath.GetXMLValue("doesnot/exist", "@stringAttribute", stringValue);
    CPPUNIT_ASSERT(!dataFound);

    // testing invalid attribute
    dataFound = xmlPath.GetXMLValue("data-1/data-1-1", "@notAnAttribute", stringValue);
    CPPUNIT_ASSERT(!dataFound);

    // testing search with different context/path - also using std::string for path and context
    std::stringstream context, path;
    std::stringstream value;
    for (unsigned int index = 1;
         index <= 3;
         index++) {
        // iterate thru all elements
        context.str("");
        context << "data-1/data-1-2/data-1-2-x["<< index << "]";
        // get int attribute (value between 1 and 3)
        path.str("");
        path << "@intAttribute";
        dataFound = xmlPath.GetXMLValue(context.str(), path.str(), intValue);
        CPPUNIT_ASSERT(dataFound);
        // get string (value between "value-1" and "value-3"
        path.str("");
        path << "data-1-2-x-y/@stringAttribute";
        dataFound = xmlPath.GetXMLValue(context.str(), path.str(), stringValue);
        CPPUNIT_ASSERT(dataFound);
        // compare both
        value.str("");
        value << "value-" << intValue;
        CPPUNIT_ASSERT_EQUAL(value.str(), stringValue);
    }
}
