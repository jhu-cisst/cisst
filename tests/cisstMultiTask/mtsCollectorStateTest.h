/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCollectorStateTest.h 1932 2010-10-22 03:32:17Z adeguet1 $

  Author(s):  Anton Deguet
  Created on: 2011-01-06

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cisstMultiTask/mtsConfig.h>

class mtsCollectorStateTest: public CppUnit::TestFixture
{
private:
    CPPUNIT_TEST_SUITE(mtsCollectorStateTest);
    {
        CPPUNIT_TEST(TestProvidedInterfacesDevice_mtsInt);
        CPPUNIT_TEST(TestProvidedInterfacesDevice_int);
        CPPUNIT_TEST(TestProvidedInterfacesPeriodic_mtsInt);
        CPPUNIT_TEST(TestProvidedInterfacesPeriodic_int);
    }
    CPPUNIT_TEST_SUITE_END();

public:
    mtsCollectorStateTest();

    void setUp(void);
    void tearDown(void);

    template <class _clientType>
    void TestProvidedInterfaces(_clientType * client);

    template <class _elementType> void TestProvidedInterfacesDevice(void);
    void TestProvidedInterfacesDevice_mtsInt(void);
    void TestProvidedInterfacesDevice_int(void);

    template <class _elementType> void TestProvidedInterfacesPeriodic(void);
    void TestProvidedInterfacesPeriodic_mtsInt(void);
    void TestProvidedInterfacesPeriodic_int(void);
};
