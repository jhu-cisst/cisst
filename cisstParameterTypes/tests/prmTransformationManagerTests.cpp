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

#include "prmTransformationManagerTests.h"

#include <cisstVector/vctTypes.h>

//A simple graph
//  theWorld
//    |
//  offset
//    |
//   -+------+-------+------+--------+
//   suj1    suj2   suj3   suj4     suj5
//    |       |      |      |        |
//   sr1     sr2    cam    ms1      ms2

//some frames
prmTransformationFixed suj1("SetupJoints1");
prmTransformationFixed sr1("SlaveRobot1");

prmTransformationFixed suj2("SetupJoints2");
prmTransformationFixed sr2("SlaveRobot2");

prmTransformationFixed suj3("SetupJoints3");
prmTransformationFixed cam("CameraRobot");

prmTransformationFixed suj4("SetupJoints4");
prmTransformationFixed ms1("MasterRobot1");

prmTransformationFixed suj5("SetupJoints5");
prmTransformationFixed ms2("MasterRobot2");

prmTransformationFixed offset("OffsetFrame");

prmTransformationFixed dummy("DummyFrame");

void prmTransformationManagerTest::setUp(void)
{
    // this is the first test run, so clear the tree to allow multiple runs
    prmTransformationManager::Clear();

    // test addition to graph
    offset.SetReferenceFrame(&prmTransformationManager::TheWorld);
	
    // set up the rest of the graph
    suj1.SetReferenceFrame("OffsetFrame"); 
    suj2.SetReferenceFrame(&offset);
    suj3.SetReferenceFrame(&offset); 
    suj4.SetReferenceFrame(&offset);
    suj5.SetReferenceFrame(&offset);
    
    sr1.SetReferenceFrame(&suj1);
    sr2.SetReferenceFrame(&suj2);
    cam.SetReferenceFrame(&suj3);
    ms1.SetReferenceFrame(&suj4);
    ms2.SetReferenceFrame(&suj5);

    // fill in some values
    vct3 trans(0.0, 0.0, 0.0);
    vct3 tmp(1.0, 0.0, 0.0);
    tmp.NormalizedSelf();
    vctRot3 rotation(vctAxAnRot3(tmp, cmnPI / 2.0));
	
    ms1.SetTransformation(vctFrm3(rotation,trans));

    rotation = vctRot3(vctAxAnRot3(tmp, cmnPI / 4.0));
    suj4.SetTransformation(vctFrm3(rotation,trans));

    rotation = vctRot3(vctAxAnRot3(tmp, cmnPI / 4.0));
    suj3.SetTransformation(vctFrm3(rotation,trans));

    rotation = vctRot3(vctAxAnRot3(tmp, cmnPI / 2.0));
    cam.SetTransformation(vctFrm3(rotation,trans));
}


void prmTransformationManagerTest::TestAddNode(void)
{
    //This is the first test run, so clear the tree to allow multiple runs
    prmTransformationManager::Clear();

    //test addition to graph
    CPPUNIT_ASSERT(dummy.SetReferenceFrame(&prmTransformationManager::TheWorld));
}


void prmTransformationManagerTest::TestDeleteNode(void)
{
    prmTransformationFixed *test = new prmTransformationFixed("testframe");

    //attach to graph
    test->SetReferenceFrame(&prmTransformationManager::TheWorld);
    
    //this should get rid of the frame
    delete test;   
    //there is no mechanism to test this fully: we will still perform this test to check syntax/symbol definition.
    // ADV this leads to seg fault  ---- CPPUNIT_ASSERT(!prmTransformationManager::FindPath(testPointer, &prmTransformationManager::TheWorld));
}


void prmTransformationManagerTest::TestNullPtr(void)
{
    CPPUNIT_ASSERT(!ms1.SetReferenceFrame(NULL));
    CPPUNIT_ASSERT(!prmTransformationManager::TheWorld.SetReferenceFrame(&ms1));
}


void prmTransformationManagerTest::TestPathFind(void)
{
    //can't search with NULL ptrs as usual, so this checks the transformation manager directly 
    CPPUNIT_ASSERT(!prmTransformationManager::FindPath(&prmTransformationManager::TheWorld, NULL));
    CPPUNIT_ASSERT(!prmTransformationManager::FindPath(NULL, &prmTransformationManager::TheWorld));
    CPPUNIT_ASSERT(!prmTransformationManager::FindPath(NULL, &prmTransformationManager::TheWorld));
    CPPUNIT_ASSERT(prmTransformationManager::FindPath(prmTransformationManager::GetTransformationNodePtr("MasterRobot1"), &prmTransformationManager::TheWorld));
    CPPUNIT_ASSERT(prmTransformationManager::GetTransformationNodePtr("BadString") == NULL);
    CPPUNIT_ASSERT(!prmTransformationManager::FindPath(NULL, NULL));
}


void prmTransformationManagerTest::TestDotCreate(void)
{
    std::ofstream dotFile1("example1.dot");
    prmTransformationManager::ToStreamDot(dotFile1);
}


void prmTransformationManagerTest::TestStringNameTreeEval(void)
{
    vctFrm3 transformation;
    transformation = prmWRTReference("MasterRobot1", "CameraRobot");
    CPPUNIT_ASSERT(transformation.AlmostEquivalent(vctFrm3::Identity(),0.01));   
}


void prmTransformationManagerTest::TestTreeEval(void)
{
    vctFrm3 transformation, transformation1;
    transformation = prmWRTReference(&ms1, &cam);
    CPPUNIT_ASSERT(transformation.AlmostEquivalent(vctFrm3::Identity(),0.01));

    transformation1 = prmWRTReference("MasterRobot1", "CameraRobot");
    CPPUNIT_ASSERT(transformation.AlmostEquivalent(transformation1));   
}

