/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Anton Deguet
  Created on: 2004-10-31
  
  (C) Copyright 2004-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _nmrLSISolverTest_h
#define _nmrLSISolverTest_h

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <cisstNumerical/nmrLSISolver.h>

class nmrLSISolverTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(nmrLSISolverTest);
    CPPUNIT_TEST(TestSolveBookExample);
    CPPUNIT_TEST_SUITE_END();

protected:
    vctDynamicMatrix<double> InputMatrix;

public:

    void setUp() {
        InputMatrix.SetSize(15, 6, VCT_COL_MAJOR);
        InputMatrix.Assign(-.13405547,  -.20162827,  -.16930778,  -.18971990,  -.17387234,   -.4361,
                           -.10379475,  -.15766336,  -.13346256,  -.14848550,  -.13597690,   -.3437,
                           -.08779597,  -.12883867,  -.10683007,  -.12011796,  -.10932972,   -.2657,
                           .02058554,   .00335331,  -.01641270,   .00078606,   .00271659,    -.0392,
                           -.03248093,  -.01876799,   .00410639,  -.01405894,  -.01384391,   .0193,
                           .05967662,   .06667714,   .04352153,   .05740438,   .05024962,    .0747,
                           .06712457,   .07352437,   .04489770,   .06471862,   .05876455,    .0935,
                           .08687186,   .09368296,   .05672327,   .08141043,   .07302320,    .1079,
                           .02149662,   .06222662,   .07213486,   .06200069,   .05570931,    .1930,
                           .06687407,   .10344506,   .09153849,   .09508223,   .08393667,    .2058,
                           .15879069,   .18088339,   .11540692,   .16160727,   .14796479,    .2606,
                           .17642887,   .20361830,   .13057860,   .18385729,   .17005549,    .3142,
                           .11414080,   .17259611,   .14816471,   .16007466,   .14374096,    .3529,
                           .07846038,   .14669563,   .14365800,   .14003842,   .12571177,    .3615,
                           .10803175,   .16994623,   .14971519,   .15885312,   .14301547,    .3647); 
    }
    
    void tearDown()
    {}
    
    /*! Test based on the example provided with the Lawson and Hanson book */
    void TestSolveBookExample(void);
};


#endif // _nmrLSISolverTest_h

