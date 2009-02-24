/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):	Anton Deguet
  Created on:	2003-09-12

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*! 
  \file 
  \brief Typedef for fixed size vectors
*/


#ifndef _vctFixedSizeVectorTypes_h
#define _vctFixedSizeVectorTypes_h

#include <cisstVector/vctFixedSizeVector.h>

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


#endif  // _vctFixedSizeVectorTypes_h

