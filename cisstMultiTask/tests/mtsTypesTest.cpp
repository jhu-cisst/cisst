/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2020-06-25

  (C) Copyright 2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnUnits.h>
#include <cisstMultiTask/mtsTaskFromCallback.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>

#include "mtsTypesTest.h"

class mtsTypesTestServer: public mtsTaskFromCallback {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_ALL);

public:
	inline mtsTypesTestServer(const std::string & name = "server"):
        mtsTaskFromCallback(name, 50)
    {
        StateTable.AddData(mInt, "int");
        StateTable.AddData(mDouble, "double");
        StateTable.AddData(mStdString, "std::string");
        StateTable.AddData(mBool, "bool");
        mProvided = AddInterfaceProvided("provided");
        mIntCommandRead =
            mProvided->AddCommandReadState(StateTable, mInt, "ReadInt");
        mDoubleCommandRead =
            mProvided->AddCommandReadState(StateTable, mDouble, "ReadDouble");
        mStdStringCommandRead =
            mProvided->AddCommandReadState(StateTable, mStdString, "ReadStdString");
        mBoolCommandRead =
            mProvided->AddCommandReadState(StateTable, mBool, "ReadBool");
    }

	inline ~mtsTypesTestServer() {}

	// implementation of four methods that are pure virtual in mtsTask
    inline void Configure(const std::string &) {}
	inline void Startup(void) {}
	inline void Run(void) {}
    inline void Cleanup(void) {}

    // public for testing
    int mInt;
    double mDouble;
    std::string mStdString;
    bool mBool;

    mtsCommandRead * mIntCommandRead, * mDoubleCommandRead, * mStdStringCommandRead, * mBoolCommandRead;
    mtsInterfaceProvided * mProvided;
    mtsStateTable * GetStateTable(void) {
        return &StateTable;
    }
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsTypesTestServer);
CMN_IMPLEMENT_SERVICES(mtsTypesTestServer);

class mtsTypesTestClient: public mtsTaskFromCallback {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_ALL);

public:
	inline mtsTypesTestClient(const std::string & name = "client"):
        mtsTaskFromCallback(name, 50)
    {
        mRequired = AddInterfaceRequired("required");
    }

	inline ~mtsTypesTestClient() {}

	// implementation of four methods that are pure virtual in mtsTask
    inline void Configure(const std::string &) {}
	inline void Startup(void) {}
	inline void Run(void) {}
    inline void Cleanup(void) {}

    // public for testing
    mtsInterfaceRequired * mRequired;
    mtsFunctionRead mReadInt, mReadDouble, mReadStdString, mReadBool;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsTypesTestClient);
CMN_IMPLEMENT_SERVICES(mtsTypesTestClient);

void mtsTypesTest::setUp(void)
{
    mServer = new mtsTypesTestServer();
    mClient = new mtsTypesTestClient();
}

void mtsTypesTest::tearDown(void)
{

}

void mtsTypesTest::TestSetup(void)
{
    CPPUNIT_ASSERT(mServer);
    CPPUNIT_ASSERT(mServer->mProvided);
    CPPUNIT_ASSERT(mServer->mIntCommandRead);
    CPPUNIT_ASSERT(mServer->mDoubleCommandRead);
    CPPUNIT_ASSERT(mServer->mStdStringCommandRead);
    CPPUNIT_ASSERT(mServer->mBoolCommandRead);

    CPPUNIT_ASSERT(mClient);
    CPPUNIT_ASSERT(mClient->mRequired);
}

void mtsTypesTest::TestInt(void)
{
    CPPUNIT_ASSERT(mServer->GetStateTable()->GetAccessorByName("int"));
}

void mtsTypesTest::TestDouble(void)
{
    CPPUNIT_ASSERT(mServer->GetStateTable()->GetAccessorByName("double"));
}

void mtsTypesTest::TestStdString(void)
{
    CPPUNIT_ASSERT(mServer->GetStateTable()->GetAccessorByName("std::string"));
}

void mtsTypesTest::TestBool(void)
{
    CPPUNIT_ASSERT(mServer->GetStateTable()->GetAccessorByName("bool"));
}

CPPUNIT_TEST_SUITE_REGISTRATION(mtsTypesTest);
