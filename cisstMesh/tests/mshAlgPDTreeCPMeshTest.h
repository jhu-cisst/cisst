/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Max Zhaoshuo Li, Anton Deguet
  Created on: 2020-07-20

  (C) Copyright 2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstVector/vctFixedSizeVectorTypes.h>

class mshAlgPDTreeCPMeshTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(mshAlgPDTreeCPMeshTest); {
    CPPUNIT_TEST(CubeCorner);
    CPPUNIT_TEST(CubeSide);
    CPPUNIT_TEST(CubeEdge);
    CPPUNIT_TEST(CubeOnEdge);
    CPPUNIT_TEST(CubeOnSide);
    CPPUNIT_TEST(CylinderTop);
    CPPUNIT_TEST(CylinderSide);
    CPPUNIT_TEST(Sphere);
    CPPUNIT_TEST(PyramidTop);
    CPPUNIT_TEST(PyramidSide);
    CPPUNIT_TEST(PyramidBottom);
    CPPUNIT_TEST(CylindricalTunnel);
    CPPUNIT_TEST(CrownValley);
    CPPUNIT_TEST(CrownPeak);
    CPPUNIT_TEST(ResetMeshConstraintValues);
    CPPUNIT_TEST(ConvertUnit);
    CPPUNIT_TEST(TransformModel);
    CPPUNIT_TEST(Round3);
  }
  CPPUNIT_TEST_SUITE_END();

 public:
  void setUp(void);

    void tearDown(void) {
    }

    void CubeCorner(void);
    void CubeSide(void);
    void CubeEdge(void);
    void CubeOnEdge(void);
    void CubeOnSide(void);
    void CylinderTop(void);
    void CylinderSide(void);
    void Sphere(void);
    void PyramidTop(void);
    void PyramidSide(void);
    void PyramidBottom(void);
    void CylindricalTunnel(void);
    void CrownValley(void);
    void CrownPeak(void);
    void ResetMeshConstraintValues(void);
    void ConvertUnit(void);
    void TransformModel(void);
    void Round3(void);

 protected:
    std::string workingDir;
    std::vector<int> faceIdx;
    vct3 tip, normal, closestPoint;
    int numIntersected;
    int nThresh = 5;
    double diagThresh = 5.0;
    double boundingDistance = 2.0;
};

CPPUNIT_TEST_SUITE_REGISTRATION(mshAlgPDTreeCPMeshTest);
