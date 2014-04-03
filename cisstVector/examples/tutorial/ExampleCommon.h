/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Ofri Sadowsky
  Created on: 2003-10-15

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

void ExampleCommon(void) {
    // fill a vector with random numbers
    vctFixedSizeVector<double, 8> vector1, vector2;
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();
    randomSequence.ExtractRandomValueArray(-100.0, 100.0,
                                           vector1.Pointer(), vector1.size());

    // to fill a matrix or vector one can also use vctRandom
    vctRandom(vector2, -100.0, 100.0);

    // perform some operations such as divide by zero
    vector2.SetAll(0.0);
    vector2.Divide(vector2.Norm());
    unsigned int index;
    // look for Not A Number
    for (index = 0; index < vector2.size(); index++) {
        if (CMN_ISNAN(vector2[index])) {
	    std::cout << "vector[" << index << "] is NaN " << std::endl;
        }
    }

    // create a rotation based on an a normalized axis
    vctAxAnRot3 axisAngle(vct3(1.0, 0.0, 0.0), cmnPI / 2.0);
    vctQuatRot3 rot1(axisAngle);

    // modify this rotation with a new axis, not well normalized
    vct3 axis(1.0005, 0.0, 0.0);
    if (axis.Norm() > cmnTypeTraits<double>::Tolerance()) {
	std::cout << "Axis is not normalized wrt default tolerance" << std::endl;
    }
    cmnTypeTraits<double>::SetTolerance(0.001);
    // this method asserts that the axis is normalized
    axisAngle.Axis() = axis;
    axisAngle.Angle() = cmnPI / 2.0;
    rot1.From(axisAngle);
    cmnTypeTraits<double>::SetTolerance(cmnTypeTraits<double>::DefaultTolerance);
}
