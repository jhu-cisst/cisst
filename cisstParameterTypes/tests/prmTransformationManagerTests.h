/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Rajesh Kumar
  Created on: 2008-03-03
  
  (C) Copyright 2005-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstParameterTypes/prmTransformationManager.h>

class prmTransformationManagerTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(prmTransformationManagerTest);
    CPPUNIT_TEST(TestAddNode);
    CPPUNIT_TEST(TestDeleteNode);
    CPPUNIT_TEST(TestNullPtr);
    CPPUNIT_TEST(TestPathFind);
    CPPUNIT_TEST(TestDotCreate);
    CPPUNIT_TEST(TestTreeEval);
    CPPUNIT_TEST_SUITE_END();
    
public:
    void setUp(void);
    
    void tearDown(void) {
    }
    
    /*! Test the Add method */
    void TestAddNode(void);

	/*! Test the Delete method */
    void TestDeleteNode(void);

	/*! Test improper parameters */
    void TestNullPtr(void);

	/*! Test the Find method */
    void TestPathFind(void);

	/*! Test graphviz output */
    void TestDotCreate(void);

    /*! Test string name based evaluations */
    void TestStringNameTreeEval(void);

	/*! Test WRTReference Evaluation*/
    void TestTreeEval(void);
};


CPPUNIT_TEST_SUITE_REGISTRATION(prmTransformationManagerTest);

