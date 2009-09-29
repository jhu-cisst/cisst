/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeVectorProxyTest.h 75 2009-02-24 16:47:20Z adeguet1 $

  Author(s):	Anton Deguet
  Created on:	2006-06-21

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/



#ifndef _vctFixedSizeVectorProxyTest_h
#define _vctFixedSizeVectorProxyTest_h


// include everything from cisstVector
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctTypes.h>


/*! The goal of this class is to provide a good coverage of the
  different fixed size vectors available in cisstVector and different
  ways to pass them back and forth between C++ and Python.

  For each type, the class has methods to get and set values.  This
  class is to be wrapped using SWIG.  The tests are then written in
  Python using the unittest testing framework.
*/


#define ALL_METHODS_DOUBLE(size) \
    inline vctDouble##size & Double##size##Ref(void) { return Double##size; } \
    inline vctDouble##size Double##size##Copy(void) const { return Double##size; } \
    inline void Double##size##SetRefAdd10(vctDouble##size & v) { Double##size.Assign(v); v.Add(10); } \
    inline void Double##size##SetCopy(vctDouble##size v) { Double##size.Assign(v); } \
    double Double##size##SumOfElements(void) const { return Double##size.SumOfElements(); } \
    double Double##size##Add(double scalar) { Double##size.Add(scalar); return Double##size.SumOfElements(); }


#define ALL_METHODS_INT(size) \
    inline vctInt##size & Int##size##Ref(void) { return Int##size; } \
    inline vctInt##size Int##size##Copy(void) const { return Int##size; } \
    inline void Int##size##SetRefAdd10(vctInt##size & v) { Int##size.Assign(v); v.Add(10); } \
    inline void Int##size##SetCopy(vctInt##size v) { Int##size.Assign(v); } \
    int Int##size##SumOfElements(void) const { return Int##size.SumOfElements(); } \
    int Int##size##Add(int scalar) { Int##size.Add(scalar); return Int##size.SumOfElements(); }


class vctFixedSizeVectorProxyTest {
    // small utility to fill vector with index
    template <class _vectorType>
    void FillVector(_vectorType & vector) {
        unsigned int index;
        for (index = 0; index < vector.size(); index++) {
            vector[index] = index;
        }
    }

 public:
    vctDouble2 Double2;
    vctDouble3 Double3;
    vctDouble4 Double4;
    vctDouble5 Double5;
    vctDouble6 Double6;

    vctInt2 Int2;
    vctInt3 Int3;
    vctInt4 Int4;
    vctInt5 Int5;
    vctInt6 Int6;

    // Sets the data members to a specific value 
    vctFixedSizeVectorProxyTest() {
        FillVector(Double2);
        FillVector(Double3);
        FillVector(Double4);
        FillVector(Double5);
        FillVector(Double6);

        FillVector(Int2);
        FillVector(Int3);
        FillVector(Int4);
        FillVector(Int5);
        FillVector(Int6);
    }


    ~vctFixedSizeVectorProxyTest() {}

    ALL_METHODS_DOUBLE(2);
    ALL_METHODS_DOUBLE(3);
    ALL_METHODS_DOUBLE(4);
    ALL_METHODS_DOUBLE(5);
    ALL_METHODS_DOUBLE(6);

    ALL_METHODS_INT(2);
    ALL_METHODS_INT(3);
    ALL_METHODS_INT(4);
    ALL_METHODS_INT(5);
    ALL_METHODS_INT(6);

};


#endif  // _vctFixedSizeVectorProxyTest_h

