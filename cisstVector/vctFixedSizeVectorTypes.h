/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2003-09-12

  (C) Copyright 2003-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#pragma once
#ifndef _vctFixedSizeVectorTypes_h
#define _vctFixedSizeVectorTypes_h

/*!
  \file
  \brief Typedef for fixed size vectors
*/

#include <cisstVector/vctFixedSizeVector.h>

#include <cisstVector/vctDataFunctionsFixedSizeVector.h>

/*! Define a fixed size vector of size 1 containing doubles. */
//@{
typedef vctFixedSizeVector<double, 1> vctDouble1;
typedef vctFixedSizeVector<double, 1> vct1;
//@}
/*! Define a fixed size vector of size 2 containing doubles. */
//@{
typedef vctFixedSizeVector<double, 2> vctDouble2;
typedef vctFixedSizeVector<double, 2> vct2;
//@}
/*! Define a fixed size vector of size 3 containing doubles. */
//@{
typedef vctFixedSizeVector<double, 3> vctDouble3;
typedef vctFixedSizeVector<double, 3> vct3;
//@}
/*! Define a fixed size vector of size 4 containing doubles. */
//@{
typedef vctFixedSizeVector<double, 4> vctDouble4;
typedef vctFixedSizeVector<double, 4> vct4;
//@}
/*! Define a fixed size vector of size 5 containing doubles. */
//@{
typedef vctFixedSizeVector<double, 5> vctDouble5;
typedef vctFixedSizeVector<double, 5> vct5;
//@}
/*! Define a fixed size vector of size 6 containing doubles. */
//@{
typedef vctFixedSizeVector<double, 6> vctDouble6;
typedef vctFixedSizeVector<double, 6> vct6;
//@}
/*! Define a fixed size vector of size 7 containing doubles. */
//@{
typedef vctFixedSizeVector<double, 7> vctDouble7;
typedef vctFixedSizeVector<double, 7> vct7;
//@}
/*! Define a fixed size vector of size 8 containing doubles. */
//@{
typedef vctFixedSizeVector<double, 8> vctDouble8;
typedef vctFixedSizeVector<double, 8> vct8;
//@}
/*! Define a fixed size vector of size 9 containing doubles. */
//@{
typedef vctFixedSizeVector<double, 9> vctDouble9;
typedef vctFixedSizeVector<double, 9> vct9;
//@}


/*! Define a fixed size vector of size 1 containing floats. */
typedef vctFixedSizeVector<float, 1> vctFloat1;
/*! Define a fixed size vector of size 2 containing floats. */
typedef vctFixedSizeVector<float, 2> vctFloat2;
/*! Define a fixed size vector of size 3 containing floats. */
typedef vctFixedSizeVector<float, 3> vctFloat3;
/*! Define a fixed size vector of size 4 containing floats. */
typedef vctFixedSizeVector<float, 4> vctFloat4;
/*! Define a fixed size vector of size 5 containing floats. */
typedef vctFixedSizeVector<float, 5> vctFloat5;
/*! Define a fixed size vector of size 6 containing floats. */
typedef vctFixedSizeVector<float, 6> vctFloat6;
/*! Define a fixed size vector of size 7 containing floats. */
typedef vctFixedSizeVector<float, 7> vctFloat7;
/*! Define a fixed size vector of size 8 containing floats. */
typedef vctFixedSizeVector<float, 8> vctFloat8;
/*! Define a fixed size vector of size 9 containing floats. */
typedef vctFixedSizeVector<float, 9> vctFloat9;


/*! Define a fixed size vector of size 1 containing longs. */
typedef vctFixedSizeVector<long, 1> vctLong1;
/*! Define a fixed size vector of size 2 containing longs. */
typedef vctFixedSizeVector<long, 2> vctLong2;
/*! Define a fixed size vector of size 3 containing longs. */
typedef vctFixedSizeVector<long, 3> vctLong3;
/*! Define a fixed size vector of size 4 containing longs. */
typedef vctFixedSizeVector<long, 4> vctLong4;
/*! Define a fixed size vector of size 5 containing longs. */
typedef vctFixedSizeVector<long, 5> vctLong5;
/*! Define a fixed size vector of size 6 containing longs. */
typedef vctFixedSizeVector<long, 6> vctLong6;
/*! Define a fixed size vector of size 7 containing longs. */
typedef vctFixedSizeVector<long, 7> vctLong7;
/*! Define a fixed size vector of size 8 containing longs. */
typedef vctFixedSizeVector<long, 8> vctLong8;
/*! Define a fixed size vector of size 9 containing longs. */
typedef vctFixedSizeVector<long, 9> vctLong9;


/*! Define a fixed size vector of size 1 containing unsigned longs. */
typedef vctFixedSizeVector<unsigned long, 1> vctULong1;
/*! Define a fixed size vector of size 2 containing unsigned longs. */
typedef vctFixedSizeVector<unsigned long, 2> vctULong2;
/*! Define a fixed size vector of size 3 containing unsigned longs. */
typedef vctFixedSizeVector<unsigned long, 3> vctULong3;
/*! Define a fixed size vector of size 4 containing unsigned longs. */
typedef vctFixedSizeVector<unsigned long, 4> vctULong4;
/*! Define a fixed size vector of size 5 containing unsigned longs. */
typedef vctFixedSizeVector<unsigned long, 5> vctULong5;
/*! Define a fixed size vector of size 6 containing unsigned longs. */
typedef vctFixedSizeVector<unsigned long, 6> vctULong6;
/*! Define a fixed size vector of size 7 containing unsigned longs. */
typedef vctFixedSizeVector<unsigned long, 7> vctULong7;
/*! Define a fixed size vector of size 8 containing unsigned longs. */
typedef vctFixedSizeVector<unsigned long, 8> vctULong8;
/*! Define a fixed size vector of size 9 containing unsigned longs. */
typedef vctFixedSizeVector<unsigned long, 9> vctULong9;


/*! Define a fixed size vector of size 1 containing integers. */
typedef vctFixedSizeVector<int, 1> vctInt1;
/*! Define a fixed size vector of size 2 containing integers. */
typedef vctFixedSizeVector<int, 2> vctInt2;
/*! Define a fixed size vector of size 3 containing integers. */
typedef vctFixedSizeVector<int, 3> vctInt3;
/*! Define a fixed size vector of size 4 containing integers. */
typedef vctFixedSizeVector<int, 4> vctInt4;
/*! Define a fixed size vector of size 5 containing integers. */
typedef vctFixedSizeVector<int, 5> vctInt5;
/*! Define a fixed size vector of size 6 containing integers. */
typedef vctFixedSizeVector<int, 6> vctInt6;
/*! Define a fixed size vector of size 7 containing integers. */
typedef vctFixedSizeVector<int, 7> vctInt7;
/*! Define a fixed size vector of size 8 containing integers. */
typedef vctFixedSizeVector<int, 8> vctInt8;
/*! Define a fixed size vector of size 9 containing integers. */
typedef vctFixedSizeVector<int, 9> vctInt9;


/*! Define a fixed size vector of size 1 containing unsigned integers. */
typedef vctFixedSizeVector<unsigned int, 1> vctUInt1;
/*! Define a fixed size vector of size 2 containing unsigned integers. */
typedef vctFixedSizeVector<unsigned int, 2> vctUInt2;
/*! Define a fixed size vector of size 3 containing unsigned integers. */
typedef vctFixedSizeVector<unsigned int, 3> vctUInt3;
/*! Define a fixed size vector of size 4 containing unsigned integers. */
typedef vctFixedSizeVector<unsigned int, 4> vctUInt4;
/*! Define a fixed size vector of size 5 containing unsigned integers. */
typedef vctFixedSizeVector<unsigned int, 5> vctUInt5;
/*! Define a fixed size vector of size 6 containing unsigned integers. */
typedef vctFixedSizeVector<unsigned int, 6> vctUInt6;
/*! Define a fixed size vector of size 7 containing unsigned integers. */
typedef vctFixedSizeVector<unsigned int, 7> vctUInt7;
/*! Define a fixed size vector of size 8 containing unsigned integers. */
typedef vctFixedSizeVector<unsigned int, 8> vctUInt8;
/*! Define a fixed size vector of size 9 containing unsigned integers. */
typedef vctFixedSizeVector<unsigned int, 9> vctUInt9;


/*! Define a fixed size vector of size 1 containing short integers. */
typedef vctFixedSizeVector<short, 1> vctShort1;
/*! Define a fixed size vector of size 2 containing short integers. */
typedef vctFixedSizeVector<short, 2> vctShort2;
/*! Define a fixed size vector of size 3 containing short integers. */
typedef vctFixedSizeVector<short, 3> vctShort3;
/*! Define a fixed size vector of size 4 containing short integers. */
typedef vctFixedSizeVector<short, 4> vctShort4;
/*! Define a fixed size vector of size 5 containing short integers. */
typedef vctFixedSizeVector<short, 5> vctShort5;
/*! Define a fixed size vector of size 6 containing short integers. */
typedef vctFixedSizeVector<short, 6> vctShort6;
/*! Define a fixed size vector of size 7 containing short integers. */
typedef vctFixedSizeVector<short, 7> vctShort7;
/*! Define a fixed size vector of size 8 containing short integers. */
typedef vctFixedSizeVector<short, 8> vctShort8;
/*! Define a fixed size vector of size 9 containing short integers. */
typedef vctFixedSizeVector<short, 9> vctShort9;


/*! Define a fixed size vector of size 1 containing unsigned short integers. */
typedef vctFixedSizeVector<unsigned short, 1> vctUShort1;
/*! Define a fixed size vector of size 2 containing unsigned short integers. */
typedef vctFixedSizeVector<unsigned short, 2> vctUShort2;
/*! Define a fixed size vector of size 3 containing unsigned short integers. */
typedef vctFixedSizeVector<unsigned short, 3> vctUShort3;
/*! Define a fixed size vector of size 4 containing unsigned short integers. */
typedef vctFixedSizeVector<unsigned short, 4> vctUShort4;
/*! Define a fixed size vector of size 5 containing unsigned short integers. */
typedef vctFixedSizeVector<unsigned short, 5> vctUShort5;
/*! Define a fixed size vector of size 6 containing unsigned short integers. */
typedef vctFixedSizeVector<unsigned short, 6> vctUShort6;
/*! Define a fixed size vector of size 7 containing unsigned short integers. */
typedef vctFixedSizeVector<unsigned short, 7> vctUShort7;
/*! Define a fixed size vector of size 8 containing unsigned short integers. */
typedef vctFixedSizeVector<unsigned short, 8> vctUShort8;
/*! Define a fixed size vector of size 9 containing unsigned short integers. */
typedef vctFixedSizeVector<unsigned short, 9> vctUShort9;


/*! Define a fixed size vector of size 1 containing chars. */
typedef vctFixedSizeVector<char, 1> vctChar1;
/*! Define a fixed size vector of size 2 containing chars. */
typedef vctFixedSizeVector<char, 2> vctChar2;
/*! Define a fixed size vector of size 3 containing chars. */
typedef vctFixedSizeVector<char, 3> vctChar3;
/*! Define a fixed size vector of size 4 containing chars. */
typedef vctFixedSizeVector<char, 4> vctChar4;
/*! Define a fixed size vector of size 5 containing chars. */
typedef vctFixedSizeVector<char, 5> vctChar5;
/*! Define a fixed size vector of size 6 containing chars. */
typedef vctFixedSizeVector<char, 6> vctChar6;
/*! Define a fixed size vector of size 7 containing chars. */
typedef vctFixedSizeVector<char, 7> vctChar7;
/*! Define a fixed size vector of size 8 containing chars. */
typedef vctFixedSizeVector<char, 8> vctChar8;
/*! Define a fixed size vector of size 9 containing chars. */
typedef vctFixedSizeVector<char, 9> vctChar9;

/*! Define a fixed size vector of size 1 containing unsigned chars. */
typedef vctFixedSizeVector<unsigned char, 1> vctUChar1;
/*! Define a fixed size vector of size 2 containing unsigned chars. */
typedef vctFixedSizeVector<unsigned char, 2> vctUChar2;
/*! Define a fixed size vector of size 3 containing unsigned chars. */
typedef vctFixedSizeVector<unsigned char, 3> vctUChar3;
/*! Define a fixed size vector of size 4 containing unsigned chars. */
typedef vctFixedSizeVector<unsigned char, 4> vctUChar4;
/*! Define a fixed size vector of size 5 containing unsigned chars. */
typedef vctFixedSizeVector<unsigned char, 5> vctUChar5;
/*! Define a fixed size vector of size 6 containing unsigned chars. */
typedef vctFixedSizeVector<unsigned char, 6> vctUChar6;
/*! Define a fixed size vector of size 7 containing unsigned chars. */
typedef vctFixedSizeVector<unsigned char, 7> vctUChar7;
/*! Define a fixed size vector of size 8 containing unsigned chars. */
typedef vctFixedSizeVector<unsigned char, 8> vctUChar8;
/*! Define a fixed size vector of size 9 containing unsigned chars. */
typedef vctFixedSizeVector<unsigned char, 9> vctUChar9;

/*! Define a fixed size vector of size 1 containing bools. */
typedef vctFixedSizeVector<bool, 1> vctBool1;
/*! Define a fixed size vector of size 2 containing bools. */
typedef vctFixedSizeVector<bool, 2> vctBool2;
/*! Define a fixed size vector of size 3 containing bools. */
typedef vctFixedSizeVector<bool, 3> vctBool3;
/*! Define a fixed size vector of size 4 containing bools. */
typedef vctFixedSizeVector<bool, 4> vctBool4;
/*! Define a fixed size vector of size 5 containing bools. */
typedef vctFixedSizeVector<bool, 5> vctBool5;
/*! Define a fixed size vector of size 6 containing bools. */
typedef vctFixedSizeVector<bool, 6> vctBool6;
/*! Define a fixed size vector of size 7 containing bools. */
typedef vctFixedSizeVector<bool, 7> vctBool7;
/*! Define a fixed size vector of size 8 containing bools. */
typedef vctFixedSizeVector<bool, 8> vctBool8;
/*! Define a fixed size vector of size 9 containing bools. */
typedef vctFixedSizeVector<bool, 9> vctBool9;


#endif  // _vctFixedSizeVectorTypes_h

