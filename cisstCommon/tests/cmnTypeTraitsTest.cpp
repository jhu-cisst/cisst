/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Ofri Sadowsky
  Created on: 2004-04-02

  (C) Copyright 2004-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cmnTypeTraitsTest.h"


template <class _elementType>
void cmnTypeTraitsTest::TestFloatingPoints(void) {

    // check the HasXyz methods
    CPPUNIT_ASSERT(cmnTypeTraits<_elementType>::HasInfinity());
    CPPUNIT_ASSERT(cmnTypeTraits<_elementType>::HasNaN());

    // get the values, tests that all symbols are instantiated
    _elementType maxpos = cmnTypeTraits<_elementType>::MaxPositiveValue();
    _elementType minpos = cmnTypeTraits<_elementType>::MinPositiveValue();
    _elementType maxneg = cmnTypeTraits<_elementType>::MaxNegativeValue();
    _elementType minneg = cmnTypeTraits<_elementType>::MinNegativeValue();
    _elementType tol = cmnTypeTraits<_elementType>::Tolerance();
    _elementType minf = cmnTypeTraits<_elementType>::MinusInfinity();
    _elementType pinf = cmnTypeTraits<_elementType>::PlusInfinity();
    _elementType nan = cmnTypeTraits<_elementType>::NaN();
    _elementType zero = _elementType(0);

    std::string name = cmnTypeTraits<_elementType>::TypeName();

    CPPUNIT_ASSERT(zero < maxpos);
    CPPUNIT_ASSERT(zero < minpos);
    CPPUNIT_ASSERT(maxpos < pinf);
    CPPUNIT_ASSERT(minpos < pinf);
    CPPUNIT_ASSERT(minpos < maxpos);
    CPPUNIT_ASSERT(zero < pinf);

    CPPUNIT_ASSERT(minneg < zero);
    CPPUNIT_ASSERT(maxneg < zero);
    CPPUNIT_ASSERT(minf < minneg);
    CPPUNIT_ASSERT(minf < maxneg);
    CPPUNIT_ASSERT(minneg < maxneg);
    CPPUNIT_ASSERT(minf < zero);

    CPPUNIT_ASSERT(minf < pinf);
    CPPUNIT_ASSERT(minneg < maxpos);

    CPPUNIT_ASSERT(pinf == pinf);
    CPPUNIT_ASSERT(!(pinf < pinf));

    CPPUNIT_ASSERT(minf == minf);
    CPPUNIT_ASSERT(!(minf < minf));

    CPPUNIT_ASSERT(!(nan == nan));
    CPPUNIT_ASSERT(nan != nan);

	CPPUNIT_ASSERT(tol <= pinf);
	CPPUNIT_ASSERT(tol <= maxpos);
	CPPUNIT_ASSERT(tol >= minpos);
}

void cmnTypeTraitsTest::TestFloatingPointsDouble(void) {
    TestFloatingPoints<double>();
}

void cmnTypeTraitsTest::TestFloatingPointsFloat(void) {
    TestFloatingPoints<float>();
}



template <class _elementType>
void cmnTypeTraitsTest::TestNonFloatingPoints(void) {

    // check the HasXyz methods, must return false
    CPPUNIT_ASSERT(!(cmnTypeTraits<_elementType>::HasInfinity()));
    CPPUNIT_ASSERT(!(cmnTypeTraits<_elementType>::HasNaN()));

    // get the values, tests that all symbols are instantiated
    _elementType maxpos = cmnTypeTraits<_elementType>::MaxPositiveValue();
    _elementType minpos = cmnTypeTraits<_elementType>::MinPositiveValue();
    _elementType maxneg = cmnTypeTraits<_elementType>::MaxNegativeValue();
    _elementType minneg = cmnTypeTraits<_elementType>::MinNegativeValue();
    _elementType tol = cmnTypeTraits<_elementType>::Tolerance();
    _elementType minf = cmnTypeTraits<_elementType>::MinusInfinity();
    _elementType pinf = cmnTypeTraits<_elementType>::PlusInfinity();
    _elementType nan = cmnTypeTraits<_elementType>::NaN();
    _elementType zero = _elementType(0);

    std::string name = cmnTypeTraits<_elementType>::TypeName();

    CPPUNIT_ASSERT(zero < maxpos);
    CPPUNIT_ASSERT(zero < minpos);
    CPPUNIT_ASSERT(maxpos == pinf);
    CPPUNIT_ASSERT(minpos >= zero);
    CPPUNIT_ASSERT(minpos < maxpos);
    CPPUNIT_ASSERT(zero < pinf);

    CPPUNIT_ASSERT(minneg < zero);
    CPPUNIT_ASSERT(maxneg < zero);
    CPPUNIT_ASSERT(minf == minneg);
    CPPUNIT_ASSERT(zero >=  maxneg);
    CPPUNIT_ASSERT(minneg < maxneg);
    CPPUNIT_ASSERT(minf < zero);

    CPPUNIT_ASSERT(minf < pinf);
    CPPUNIT_ASSERT(minneg < maxpos);

    CPPUNIT_ASSERT(pinf == pinf);
    CPPUNIT_ASSERT(!(pinf < pinf));

    CPPUNIT_ASSERT(minf == minf);
    CPPUNIT_ASSERT(!(minf < minf));

    CPPUNIT_ASSERT(nan == nan);

	CPPUNIT_ASSERT(tol <= pinf);
}

void cmnTypeTraitsTest::TestNonFloatingPointsInt(void) {
    TestNonFloatingPoints<int>();
}

void cmnTypeTraitsTest::TestNonFloatingPointsChar(void) {
    TestNonFloatingPoints<char>();
}

class Base {
    int x;
public:
    Base(int a) : x(a) {}
    virtual ~Base() {}
    int X(void) { return x; }
};

class Derived : public Base {
public:
    Derived(int a) : Base(a) {}
    ~Derived() {}
};

class SomeUnknownClass;

void cmnTypeTraitsTest::TestIsDerivedFrom(void) {
    CPPUNIT_ASSERT((cmnIsDerivedFrom<Derived, Base>::IS_DERIVED));
    CPPUNIT_ASSERT((cmnIsDerivedFrom<Base, Derived>::IS_NOT_DERIVED));
    CPPUNIT_ASSERT((cmnIsDerivedFrom<Derived, SomeUnknownClass>::IS_NOT_DERIVED));
    CPPUNIT_ASSERT((cmnIsDerivedFrom<int, Base>::IS_NOT_DERIVED));
    CPPUNIT_ASSERT((cmnIsDerivedFrom<int, SomeUnknownClass>::IS_NOT_DERIVED));
    CPPUNIT_ASSERT((cmnIsDerivedFrom<Base, int>::IS_NOT_DERIVED));
    CPPUNIT_ASSERT((cmnIsDerivedFrom<Derived, int>::IS_NOT_DERIVED));
}
