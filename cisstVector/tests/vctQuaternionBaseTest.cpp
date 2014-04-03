/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2007-02-05
  
  (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "vctQuaternionBaseTest.h"
#include <cisstVector/vctFixedSizeVector.h>

template<class _elementType>
void vctQuaternionBaseTest::TestConstructors(void) {

    vctQuaternionBase<vctFixedSizeVector<_elementType, 4> > testQuaternion(_elementType(1.0),
                                                                           _elementType(4.0),
                                                                           _elementType(9.5),
                                                                           _elementType(-1.3));
    
    CPPUNIT_ASSERT(testQuaternion[0] == _elementType(1.0));
    CPPUNIT_ASSERT(testQuaternion[1] == _elementType(4.0));
    CPPUNIT_ASSERT(testQuaternion[2] == _elementType(9.5));
    CPPUNIT_ASSERT(testQuaternion[3] == _elementType(-1.3));

    CPPUNIT_ASSERT(testQuaternion.X() == testQuaternion[0]);
    CPPUNIT_ASSERT(testQuaternion.Y() == testQuaternion[1]);
    CPPUNIT_ASSERT(testQuaternion.Z() == testQuaternion[2]);
    CPPUNIT_ASSERT(testQuaternion.R() == testQuaternion[3]);

    testQuaternion.X() = _elementType(1.0);
    testQuaternion.Y() = _elementType(1.0);
    testQuaternion.Z() = _elementType(1.0);
    testQuaternion.R() = _elementType(1.0);

    CPPUNIT_ASSERT(testQuaternion == _elementType(1.0));
}

void vctQuaternionBaseTest::TestConstructorsDouble(void) {
    TestConstructors<double>();
}

void vctQuaternionBaseTest::TestConstructorsFloat(void) {
    TestConstructors<float>();
}



template <class _elementType>
void vctQuaternionBaseTest::TestProduct(void) {

    typedef vctQuaternionBase<vctFixedSizeVector<_elementType, 4> > QuatType;
    QuatType qX(_elementType(1.0),
                _elementType(0.0),
                _elementType(0.0),
                _elementType(0.0));
    QuatType qY(_elementType(0.0),
                _elementType(1.0),
                _elementType(0.0),
                _elementType(0.0));
    QuatType qZ(_elementType(0.0),
                _elementType(0.0),
                _elementType(1.0),
                _elementType(0.0));
    QuatType qR(_elementType(0.0),
                _elementType(0.0),
                _elementType(0.0),
                _elementType(1.0));
    
    QuatType result;
    _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance();
    unsigned int i;

    result.ProductOf(qR, qR);
    CPPUNIT_ASSERT(result == qR);
    
    result.ProductOf(qR, qX);
    CPPUNIT_ASSERT(result == qX);
    result.ProductOf(qX, qR);
    CPPUNIT_ASSERT(result == qX);
    
    result.ProductOf(qR, qY);
    CPPUNIT_ASSERT(result == qY);
    result.ProductOf(qY, qR);
    CPPUNIT_ASSERT(result == qY);
    
    result.ProductOf(qR, qZ);
    CPPUNIT_ASSERT(result == qZ);
    result.ProductOf(qZ, qR);
    CPPUNIT_ASSERT(result == qZ);
 
    result.ProductOf(qX, qX);
    CPPUNIT_ASSERT(result == -qR);
    result.ProductOf(qY, qY);
    CPPUNIT_ASSERT(result == -qR);
    result.ProductOf(qZ, qZ);
    CPPUNIT_ASSERT(result == -qR);

    result.ProductOf(qX, qY);
    CPPUNIT_ASSERT(result == qZ);
    result.ProductOf(qY, qX);
    CPPUNIT_ASSERT(result == -qZ);

    result.ProductOf(qY, qZ);
    CPPUNIT_ASSERT(result == qX);
    result.ProductOf(qZ, qY);
    CPPUNIT_ASSERT(result == -qX);

    result.ProductOf(qZ, qX);
    CPPUNIT_ASSERT(result == qY);
    result.ProductOf(qX, qZ);
    CPPUNIT_ASSERT(result == -qY);

    QuatType q1(_elementType(2.22),
                _elementType(3.11),
                _elementType(-3.56),
                _elementType(3.1));
    QuatType q2(_elementType(4.55),
                _elementType(1.24),
                _elementType(-2.4),
                _elementType(-10.4));
    result.ProductOf(q1, q2);
    for (i = 0; i < q2.size(); i++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(result[i], (q1 * q2)[i], tolerance);
    }
}


void vctQuaternionBaseTest::TestProductDouble(void) {
    TestProduct<double>();
}

void vctQuaternionBaseTest::TestProductFloat(void) {
    TestProduct<float>();
}


template <class _elementType>
void vctQuaternionBaseTest::TestConjugate(void) {
    
    typedef vctQuaternionBase<vctFixedSizeVector<_elementType, 4> > QuatType;
    QuatType qR(_elementType(0.0),
                _elementType(0.0),
                _elementType(0.0),
                _elementType(1.0));
    QuatType q(_elementType(-2.0),
               _elementType(3.0),
               _elementType(1.0),
               _elementType(-10.0));
    QuatType qConjugate, product;
    
    qConjugate.ConjugateOf(q);
    CPPUNIT_ASSERT(q.X() == - qConjugate.X());
    CPPUNIT_ASSERT(q.Y() == - qConjugate.Y());
    CPPUNIT_ASSERT(q.Z() == - qConjugate.Z());
    CPPUNIT_ASSERT(q.R() == qConjugate.R());
    product.ProductOf(q, qConjugate);
    product.NormalizedSelf();
    CPPUNIT_ASSERT(product == qR);
    
    qConjugate = q;
    qConjugate.ConjugateSelf();
    CPPUNIT_ASSERT(q.X() == - qConjugate.X());
    CPPUNIT_ASSERT(q.Y() == - qConjugate.Y());
    CPPUNIT_ASSERT(q.Z() == - qConjugate.Z());
    CPPUNIT_ASSERT(q.R() == qConjugate.R());
    product.ProductOf(q, qConjugate);
    product.NormalizedSelf();
    CPPUNIT_ASSERT(product == qR);    
}



void vctQuaternionBaseTest::TestConjugateDouble(void) {
    TestConjugate<double>();
}

void vctQuaternionBaseTest::TestConjugateFloat(void) {
    TestConjugate<float>();
}



template <class _elementType>
void vctQuaternionBaseTest::TestNormalize(void) {
 
    typedef vctQuaternionBase<vctFixedSizeVector<_elementType, 4> > QuatType;

    QuatType q1(_elementType(2.0),
                _elementType(-5.0),
                _elementType(-2.0),
                _elementType(1.0));
    QuatType q2;

    _elementType tolerance = cmnTypeTraits<_elementType>::Tolerance();
    unsigned int i;

    q2.NormalizedOf(q1);
    CPPUNIT_ASSERT(q1.Norm() != _elementType(1.0));
    CPPUNIT_ASSERT(!q1.IsNormalized());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(_elementType(1.0), q2.Norm(), tolerance);
    CPPUNIT_ASSERT(q2.IsNormalized());
    
    q2 *= _elementType(q1.Norm());
    for (i = 0; i < q2.size(); i++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(q1[i], q2[i], tolerance);
    }
    
    q2.NormalizedOf(q1);
    q1.NormalizedSelf();
    for (i = 0; i < q2.size(); i++) {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(q1[i], q2[i], tolerance);
    }
}


void vctQuaternionBaseTest::TestNormalizeDouble(void) {
    TestNormalize<double>();
}

void vctQuaternionBaseTest::TestNormalizeFloat(void) {
    TestNormalize<float>();
}



CPPUNIT_TEST_SUITE_REGISTRATION(vctQuaternionBaseTest);

