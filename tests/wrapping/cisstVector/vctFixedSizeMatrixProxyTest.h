/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeMatrixProxyTest.h,v 1.6 2007/04/26 20:12:05 anton Exp $

  Author(s):	Anton Deguet
  Created on:	2006-07-18

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/



#ifndef _vctFixedSizeMatrixProxyTest_h
#define _vctFixedSizeMatrixProxyTest_h


// include everything from cisstVector
#include <cisstVector/vctFixedSizeMatrix.h>
#include <cisstVector/vctTypes.h>


/*! The goal of this class is to provide a good coverage of the
  different fixed size vectors available in cisstVector and different
  ways to pass them back and forth between C++ and Python.

  For each type, the class has methods to get and set values.  This
  class is to be wrapped using SWIG.  The tests are then written in
  Python using the unittest testing framework.
*/


#define ALL_METHODS_DOUBLE(rows, cols) \
    inline vctDouble##rows##x##cols & Double##rows##x##cols##Ref(void) { return Double##rows##x##cols; } \
    inline vctDouble##rows##x##cols Double##rows##x##cols##Copy(void) const { return Double##rows##x##cols; } \
    inline void Double##rows##x##cols##SetRefAdd10(vctDouble##rows##x##cols & m) { Double##rows##x##cols.Assign(m); m.Add(10); } \
    inline void Double##rows##x##cols##SetCopy(vctDouble##rows##x##cols m) { Double##rows##x##cols.Assign(m); } \
    double Double##rows##x##cols##SumOfElements(void) const { return Double##rows##x##cols.SumOfElements(); } \
    double Double##rows##x##cols##Add(double scalar) { Double##rows##x##cols.Add(scalar); return Double##rows##x##cols.SumOfElements(); }


#define ALL_METHODS_INT(rows, cols) \
    inline vctInt##rows##x##cols & Int##rows##x##cols##Ref(void) { return Int##rows##x##cols; } \
    inline vctInt##rows##x##cols Int##rows##x##cols##Copy(void) const { return Int##rows##x##cols; } \
    inline void Int##rows##x##cols##SetRefAdd10(vctInt##rows##x##cols & m) { Int##rows##x##cols.Assign(m); m.Add(10); } \
    inline void Int##rows##x##cols##SetCopy(vctInt##rows##x##cols m) { Int##rows##x##cols.Assign(m); } \
    int Int##rows##x##cols##SumOfElements(void) const { return Int##rows##x##cols.SumOfElements(); } \
    int Int##rows##x##cols##Add(int scalar) { Int##rows##x##cols.Add(scalar); return Int##rows##x##cols.SumOfElements(); }


class vctFixedSizeMatrixProxyTest {
    // small utility to fill matrix with 100 * c + r
    template <class _matrixType>
    void FillMatrix(_matrixType & matrix) {
        unsigned int r, c;
        for (r = 0; r < matrix.rows(); r++) {
            for (c = 0; c < matrix.cols(); c++) {
                matrix[r][c] = 100 * r + c;
            }
        }
    }

 public:
    vctDouble2x2 Double2x2;
    vctDouble3x3 Double3x3;
    vctDouble4x4 Double4x4;

    vctInt2x2 Int2x2;
    vctInt3x3 Int3x3;
    vctInt4x4 Int4x4;

    // Sets the data members to a specific value 
    vctFixedSizeMatrixProxyTest() {
        FillMatrix(Double2x2);
        FillMatrix(Double3x3);
        FillMatrix(Double4x4);

        FillMatrix(Int2x2);
        FillMatrix(Int3x3);
        FillMatrix(Int4x4);
    }


    ~vctFixedSizeMatrixProxyTest() {}

    ALL_METHODS_DOUBLE(2, 2);
    ALL_METHODS_DOUBLE(3, 3);
    ALL_METHODS_DOUBLE(4, 4);

    ALL_METHODS_INT(2, 2);
    ALL_METHODS_INT(3, 3);
    ALL_METHODS_INT(4, 4);
};


#endif  // _vctFixedSizeMatrixProxyTest_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: vctFixedSizeMatrixProxyTest.h,v $
// Revision 1.6  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.5  2006/11/20 20:33:53  anton
// Licensing: Applied new license to tests.
//
// Revision 1.4  2005/10/07 01:09:26  anton
// cisstVector Tests: Removed #include <cisstVector.h> to avoid systematic
// recompilation and useless dependencies.
//
// Revision 1.3  2005/09/26 15:41:48  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/07/22 20:08:18  anton
// Fixed size containers: Added tests for reference inputs.
//
// Revision 1.1  2005/07/19 17:11:17  anton
// vctFixedSizeMatrix: Added test for SWIG/Python support.
//
//
// ****************************************************************************

