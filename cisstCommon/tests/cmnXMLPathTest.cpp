/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2011-05-26

  (C) Copyright 2011-2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnXMLPathTest.h"

#include <limits>

#include <cisstCommon/cmnPath.h>

#if CISST_HAS_QT_XML
  #include <QCoreApplication>
  QCoreApplication * GlobalQApplication;
#endif

void cmnXMLPathTest::setUp(void)
{
    // set log to see all
    cmnLogger::SetMask(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskClassMatching("cmnXMLPath", CMN_LOG_ALLOW_ALL);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_ALL);
#if CISST_HAS_QT_XML
    int argc = 1;
    char * argv = "";
    GlobalQApplication = new QCoreApplication(argc, &argv);
#endif
}


void cmnXMLPathTest::tearDown(void)
{
    // reset log
    cmnLogger::SetMask(CMN_LOG_ALLOW_DEFAULT);
    cmnLogger::SetMaskFunction(CMN_LOG_ALLOW_DEFAULT);
    cmnLogger::SetMaskClassMatching("cmnXMLPath", cmnXMLPath::InitialLoD);
    cmnLogger::SetMaskDefaultLog(CMN_LOG_ALLOW_DEFAULT);
#if CISST_HAS_QT_XML
    delete GlobalQApplication;
#endif
}


void cmnXMLPathTest::TestReadExistingFile(void)
{
    // find original file and test
    cmnPath filePath;
    filePath.Add(std::string(CISST_SOURCE_DIR) + "/cisstCommon/tests");
    std::string testFile = filePath.Find("cmnXMLPathTestFile1.xml");
    std::string schemaFile = filePath.Find("cmnXMLPathTestFile1.xsd");
    CPPUNIT_ASSERT(!(testFile == ""));
    CPPUNIT_ASSERT(!(schemaFile == ""));

    // create path and test
    cmnXMLPath xmlPath;
    xmlPath.SetInputSource(testFile);
    TestExistingFile1(xmlPath, schemaFile);
}


void cmnXMLPathTest::TestCopyReadExistingFile(void)
{
    // find original file
    cmnPath filePath;
    filePath.Add(std::string(CISST_SOURCE_DIR) + "/cisstCommon/tests");
    std::string testFile = filePath.Find("cmnXMLPathTestFile1.xml");
    std::string schemaFile = filePath.Find("cmnXMLPathTestFile1.xsd");
    CPPUNIT_ASSERT(!(testFile == ""));
    CPPUNIT_ASSERT(!(schemaFile == ""));
    // read file
    cmnXMLPath xmlPath;
    xmlPath.SetInputSource(testFile);
    // make a copy
    std::string copy = "cmnXMLPathTestFile1-copy.xml";
    xmlPath.SaveAs(copy);
    // test copy
    cmnXMLPath xmlPathCopy;
    xmlPathCopy.SetInputSource(copy);
    TestExistingFile1(xmlPath, schemaFile);
}


void cmnXMLPathTest::TestExistingFile1(cmnXMLPath & xmlPath, const std::string & schemaFile)
{
    // validate with schema
    bool validatedWithSchema = xmlPath.ValidateWithSchema(schemaFile);
    CPPUNIT_ASSERT(validatedWithSchema);

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
    dataFound = xmlPath.GetXMLValue("/", "data-1/data-1-1/@doubleAttribute", doubleValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(1.234, doubleValue);

    // testing inf and -inf
    dataFound = xmlPath.GetXMLValue("data-1", "data-1-1/@doubleAttributeInf", doubleValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(std::numeric_limits<double>::infinity(), doubleValue);
    dataFound = xmlPath.GetXMLValue("data-1/data-1-1", "@doubleAttributeMinusInf", doubleValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(-1.0 * std::numeric_limits<double>::infinity(), doubleValue);

    // testing string
    dataFound = xmlPath.GetXMLValue("data-1/data-1-1", "@stringAttribute", stringValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(std::string("Hello!"), stringValue);

    // testing invalid context/path
    dataFound = xmlPath.GetXMLValue("doesnot/exist", "@stringAttribute", stringValue);
    CPPUNIT_ASSERT(!dataFound);

    // testing invalid path
    dataFound = xmlPath.GetXMLValue("/doesnot/exist", "@stringAttribute", stringValue, "not-found");
    CPPUNIT_ASSERT(!dataFound);
    CPPUNIT_ASSERT_EQUAL(std::string("not-found"), stringValue);

    // testing invalid attribute
    dataFound = xmlPath.GetXMLValue("data-1/data-1-1", "@notAnAttribute", stringValue);
    CPPUNIT_ASSERT(!dataFound);
    dataFound = xmlPath.GetXMLValue("/data-1/data-1-1", "@notAnAttribute", stringValue, "still-not-found");
    CPPUNIT_ASSERT(!dataFound);
    CPPUNIT_ASSERT_EQUAL(std::string("still-not-found"), stringValue);

    // testing exists method on path and attribute
    CPPUNIT_ASSERT(xmlPath.Exists("data-1"));
    CPPUNIT_ASSERT(xmlPath.Exists("data-1/data-1-1"));
    CPPUNIT_ASSERT(!xmlPath.Exists("does-not-exist"));
    CPPUNIT_ASSERT(!xmlPath.Exists("data-1/does-not-exist"));

    // testing search with different context/path - also using std::string for path and context
    std::stringstream context, path;
    std::stringstream value;

    // query to count number of fields
    dataFound = xmlPath.Query("count(/data-1/data-1-2/data-1-2-x)", intValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(3, intValue);

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

        // query by attribute value
        context.str("");
        context << "data-1/data-1-2/data-1-2-x[@intAttribute="<< index << "]";
        // get int attribute (value between 1 and 3)
        path.str("");
        path << "@intAttribute";
        dataFound = xmlPath.GetXMLValue(context.str(), path.str(), intValue);
        CPPUNIT_ASSERT(dataFound);
        // get string (value "value-<index>")
        path.str("");
        path << "data-1-2-x-y/@stringAttribute";
        dataFound = xmlPath.GetXMLValue(context.str(), path.str(), stringValue);
        CPPUNIT_ASSERT(dataFound);
        // compare string to index
        value.str("");
        value << "value-" << index;
        CPPUNIT_ASSERT_EQUAL(value.str(), stringValue);
    }
}


void cmnXMLPathTest::TestWrite(void)
{
    // find original file and test
    cmnPath filePath;
    filePath.Add(std::string(CISST_SOURCE_DIR) + "/cisstCommon/tests");
    std::string testFile = filePath.Find("cmnXMLPathTestFile1.xml");
    std::string schemaFile = filePath.Find("cmnXMLPathTestFile1.xsd");
    CPPUNIT_ASSERT(!(testFile == ""));
    CPPUNIT_ASSERT(!(schemaFile == ""));

    cmnXMLPath xmlPath;
    xmlPath.SetInputSource(testFile);

    // modify
    bool dataFound;

    dataFound = xmlPath.SetXMLValue("data-1/data-1-1", "@intAttribute", 6);
    CPPUNIT_ASSERT(dataFound);

    dataFound = xmlPath.SetXMLValue("data-1", "data-1-1/@boolAttribute", true);
    CPPUNIT_ASSERT(dataFound);

    dataFound = xmlPath.SetXMLValue("", "data-1/data-1-1/@doubleAttribute", 3.14);
    CPPUNIT_ASSERT(dataFound);

    dataFound = xmlPath.SetXMLValue("data-1/data-1-1", "@doubleAttributeInf", -1.0 * std::numeric_limits<double>::infinity());
    CPPUNIT_ASSERT(dataFound);
    dataFound = xmlPath.SetXMLValue("data-1/data-1-1", "@doubleAttributeMinusInf", std::numeric_limits<double>::infinity());
    CPPUNIT_ASSERT(dataFound);

    dataFound = xmlPath.SetXMLValue("data-1/data-1-1", "@stringAttribute", std::string("Goodbye!"));
    CPPUNIT_ASSERT(dataFound);

    // test using current path
    TestModifiedFile1(xmlPath, schemaFile);

    // save and test
    std::string copy = "cmnXMLPathTestFile1-modified.xml";
    xmlPath.SaveAs(copy);
    cmnXMLPath xmlPathCopy;
    xmlPathCopy.SetInputSource(copy);
    TestModifiedFile1(xmlPathCopy, schemaFile);
}


void cmnXMLPathTest::TestModifiedFile1(cmnXMLPath & xmlPath, const std::string & schemaFile)
{
    // validate with schema
    bool validatedWithSchema = xmlPath.ValidateWithSchema(schemaFile);
    CPPUNIT_ASSERT(validatedWithSchema);

    // start reading
    int intValue;
    bool boolValue = true;
    double doubleValue;
    std::string stringValue;
    bool dataFound;

    // read using const char * for path and context
    dataFound = xmlPath.GetXMLValue("data-1/data-1-1", "@intAttribute", intValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(6, intValue);

    // slightly different way (!= context/path)
    dataFound = xmlPath.GetXMLValue("data-1", "data-1-1/@boolAttribute", boolValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(true, boolValue);

    // slightly different way (!= context/path)
    dataFound = xmlPath.GetXMLValue("/", "data-1/data-1-1/@doubleAttribute", doubleValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(3.14, doubleValue);

    // testing inf and -inf
    dataFound = xmlPath.GetXMLValue("data-1/data-1-1", "@doubleAttributeInf", doubleValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(-1.0 * std::numeric_limits<double>::infinity(), doubleValue);
    dataFound = xmlPath.GetXMLValue("data-1/data-1-1", "@doubleAttributeMinusInf", doubleValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(std::numeric_limits<double>::infinity(), doubleValue);

    // testing string
    dataFound = xmlPath.GetXMLValue("data-1/data-1-1", "@stringAttribute", stringValue);
    CPPUNIT_ASSERT(dataFound);
    CPPUNIT_ASSERT_EQUAL(std::string("Goodbye!"), stringValue);
}
