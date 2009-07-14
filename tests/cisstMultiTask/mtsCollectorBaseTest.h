/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
$Id: mtsCollectorBaseTest.h 2009-03-02 mjung5 $

Author(s):  Min Yang Jung
Created on: 2009-03-02

(C) Copyright 2008-2009 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstMultiTask/mtsCollectorBase.h>
#include <cisstMultiTask/mtsStateData.h>

#include <string>

class mtsCollectorBaseTestTask : public mtsTaskPeriodic {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, 5);
protected:    
    mtsStateData<mtsDouble> TestData;

public:
    mtsCollectorBaseTestTask(const std::string & collectorName, 
                             double period = 10 * cmn_ms);
    virtual ~mtsCollectorBaseTestTask() {}

    // implementation of four methods that are pure virtual in mtsTask
    void Configure(const std::string) {}
    void Startup(void)	{}
    void Run(void)		{}
    void Cleanup(void)	{}

    void AddDataToStateTable(const std::string & dataName);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsCollectorBaseTestTask);

/*
class mtsCollectorBaseTestAux : public mtsCollectorBase {
public:
    mtsCollectorBaseTestAux(const std::string & collectorName,
                            double period = 100 * cmn_ms)
        : mtsCollectorBase(collectorName, period) {}

    bool AddSignal(const std::string & taskName, 
                   const std::string & signalName, 
                   const std::string & format = "") { return true; }

    bool RemoveSignal(const std::string & taskName, 
                      const std::string & signalName) { return true; }

    bool FindSignal(const std::string & taskName, 
                    const std::string & signalName) { return true; }

    void Collect(void) {}
};
*/

// Tester class ---------------------------------------------------------------
class mtsCollectorBaseTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(mtsCollectorBaseTest);
    {		
        // public variables and methods
        //CPPUNIT_TEST(TestGetCollectorCount);
        ////CPPUNIT_TEST_EXCEPTION(TestAddSignal, mtsCollectorBase::mtsCollectorBaseException);
        ////CPPUNIT_TEST(TestAddSignalCleanUp);
        ////CPPUNIT_TEST(TestRemoveSignal);
        ////CPPUNIT_TEST(TestFindSignal);
        //CPPUNIT_TEST(TestCleanup);
        ////CPPUNIT_TEST(TestSetTimeBaseDouble);
        ////CPPUNIT_TEST(TestSetTimeBaseInt);
        //CPPUNIT_TEST(TestStart);
        //CPPUNIT_TEST(TestStop);

        //// private variables and methods
        //CPPUNIT_TEST(TestInit);
        //CPPUNIT_TEST(TestClearTaskMap);
        ////CPPUNIT_TEST(TestCollect);
        //CPPUNIT_TEST(TestIsAnySignalRegistered);

    }
    CPPUNIT_TEST_SUITE_END();

private:
    //mtsCollector * Collector;

public:
    void setUp(void) {
        //Collector = new mtsCollector("collector", 10 * cmn_ms);
    }

    void tearDown(void) {
        //delete Collector;
    }

    // public variables and methods
    void TestGetCollectorCount(void);
    //void TestAddSignal(void);
    //void TestAddSignalCleanUp(void);
    //void TestRemoveSignal(void);	
    //void TestFindSignal(void);
    void TestCleanup(void);
    //void TestSetTimeBaseDouble(void);
    //void TestSetTimeBaseInt(void);
    void TestStart(void);
    void TestStop(void);

    // private variables and methods
    void TestInit(void);
    void TestClearTaskMap(void);
    //void TestCollect();
    void TestIsAnySignalRegistered();
};
