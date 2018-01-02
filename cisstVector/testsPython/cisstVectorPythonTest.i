/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Daniel Li, Anton Deguet
  Created on: 2009-05-20

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


%module cisstVectorPythonTestPython

%include "cisstVector/cisstVector.i"


%header %{
    // Put header files here
    #include <cisstVector/vctPython.h>
    #include "vctDynamicVectorTypemapsTest.h"
    #include "vctFixedSizeVectorTypemapsTest.h"
    #include "vctDynamicMatrixTypemapsTest.h"
    #include "vctFixedSizeMatrixTypemapsTest.h"
    #include "vctDynamicNArrayTypemapsTest.h"
%}

%include "vctDynamicVectorTypemapsTest.h"
%template(vctDynamicVectorTypemapsTest_char) vctDynamicVectorTypemapsTest<char>;
%template(vctDynamicVectorTypemapsTest_unsigned_char) vctDynamicVectorTypemapsTest<unsigned char>;
%template(vctDynamicVectorTypemapsTest_short) vctDynamicVectorTypemapsTest<short>;
%template(vctDynamicVectorTypemapsTest_unsigned_short) vctDynamicVectorTypemapsTest<unsigned short>;
%template(vctDynamicVectorTypemapsTest_int) vctDynamicVectorTypemapsTest<int>;
%template(vctDynamicVectorTypemapsTest_unsigned_int) vctDynamicVectorTypemapsTest<unsigned int>;
%template(vctDynamicVectorTypemapsTest_double) vctDynamicVectorTypemapsTest<double>;

%include "vctFixedSizeVectorTypemapsTest.h"
%template(vctFixedSizeVectorTypemapsTest_int_4) vctFixedSizeVectorTypemapsTest<int, 4>;
%template(vctFixedSizeVectorTypemapsTest_uint_4) vctFixedSizeVectorTypemapsTest<unsigned int, 4>;
%template(vctFixedSizeVectorTypemapsTest_double_4) vctFixedSizeVectorTypemapsTest<double, 4>;

%include "vctDynamicMatrixTypemapsTest.h"
%template(vctDynamicMatrixTypemapsTest_char) vctDynamicMatrixTypemapsTest<char>;
%template(vctDynamicMatrixTypemapsTest_unsigned_char) vctDynamicMatrixTypemapsTest<unsigned char>;
%template(vctDynamicMatrixTypemapsTest_short) vctDynamicMatrixTypemapsTest<short>;
%template(vctDynamicMatrixTypemapsTest_unsigned_short) vctDynamicMatrixTypemapsTest<unsigned short>;
%template(vctDynamicMatrixTypemapsTest_int) vctDynamicMatrixTypemapsTest<int>;
%template(vctDynamicMatrixTypemapsTest_unsigned_int) vctDynamicMatrixTypemapsTest<unsigned int>;
%template(vctDynamicMatrixTypemapsTest_double) vctDynamicMatrixTypemapsTest<double>;

%include "vctFixedSizeMatrixTypemapsTest.h"
%template(vctFixedSizeMatrixTypemapsTest_int_4_4) vctFixedSizeMatrixTypemapsTest<int, 4, 4>;
%template(vctFixedSizeMatrixTypemapsTest_uint_4_4) vctFixedSizeMatrixTypemapsTest<unsigned int, 4, 4>;
%template(vctFixedSizeMatrixTypemapsTest_double_4_4) vctFixedSizeMatrixTypemapsTest<double, 4, 4>;

%include "vctDynamicNArrayTypemapsTest.h"
%template(vctDynamicNArrayTypemapsTest_char_4) vctDynamicNArrayTypemapsTest<char, 4>;
%template(vctDynamicNArrayTypemapsTest_unsigned_char_4) vctDynamicNArrayTypemapsTest<unsigned char, 4>;
%template(vctDynamicNArrayTypemapsTest_short_4) vctDynamicNArrayTypemapsTest<short, 4>;
%template(vctDynamicNArrayTypemapsTest_unsigned_short_4) vctDynamicNArrayTypemapsTest<unsigned short, 4>;
%template(vctDynamicNArrayTypemapsTest_int_4) vctDynamicNArrayTypemapsTest<int, 4>;
%template(vctDynamicNArrayTypemapsTest_unsigned_int_4) vctDynamicNArrayTypemapsTest<unsigned int, 4>;
%template(vctDynamicNArrayTypemapsTest_double_4) vctDynamicNArrayTypemapsTest<double, 4>;
