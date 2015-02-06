/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Ofri Sadowsky
  Created on: 2015-01-31

  (C) Copyright 2015 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

void ExampleReferences(void)
{
    // Create an initial vector using an existing typedef (see file vctFixedSizeVectorTypes.h)
    vct4 v4;
    for (size_t index = 0; index < v4.size(); ++index) {
        v4(index) = index;
    }

    // We can convert this vector to a matrix, either a single row or a single column
    vct4x1 colMatrix(v4.AsColMatrix());
    vct1x4 rowMatrix(v4.AsRowMatrix());
    std::cout << "AsColMatrix:" << std::endl << colMatrix << std::endl
              << "AsRowMatrix:" << std::endl << rowMatrix << std::endl;

    // Create a vector of 2 elements starting at index 1
    vct2 v2(v4.Ref<2>(1));
    std::cout << "v4: " << std::endl << v4 << std::endl
              << "v4.Ref<2>(1):" << std::endl << v2 << std::endl;

    // This will throw an exception
    std::cout << "v4.Ref<2>(3) on a vector of 4 elements will throw an out of range exception" << std::endl;
    try {
        v2.Assign(v4.Ref<2>(3));
    } catch (std::out_of_range _exception) {
        std::cout << " - exception received: " << _exception.what() << std::endl;
    }

    // The following line WILL NOT compile because the sizes don't
    // match and the compiler will catch the problem.  The error
    // message from the compiler will be tricky to understand though.
    // --------> v2 = v4.Ref<3>(0);

    vct5 v5;
    v5.SetAll(5.5);

    // Assign part of a vector to another
    v4.Ref<2>(2) = v5.Ref<2>(3);
    std::cout << "v4, last 2 elements replaced:" << std::endl << v4 << std::endl;

    // To reference elements starting from index 0, no need to specify
    // start position
    v4.Ref<2>().SetAll(-1.0);
    std::cout << "v4, first 2 elements replaced:" << std::endl << v4 << std::endl;

    // For fun, we even have a concatenation operator
    vct2 head(2.0);
    vct3 tail(3.0);
    v5 = head & tail;
    std::cout << "v5 = head & tail: " << std::endl << v5 << std::endl;

    // Dynamic vectors
    vctDoubleVec vA, vB;
    vA.SetSize(10); vA.SetAll(1.0);
    vB.SetSize(10); vB.SetAll(2.0);
    vA.Ref(4) = vB.Ref(4, 6); // assign last 4 elements of vB to first 4 of vA
    std::cout << "vA: " << std::endl << vA << std::endl;

    // Possible errors
    try {
        std::cout << "Size mismatch between dynamic vector references" << std::endl;
        vA.Ref(5) = vB.Ref(6, 3);
    } catch (std::runtime_error _exception) {
        std::cout << " - exception received: " << _exception.what() << std::endl;
    }

    try {
        std::cout << "Reference out of range" << std::endl;
        std::cout << vA.Ref(5, 6) << std::endl;
    } catch (std::out_of_range _exception) {
        std::cout << " - exception received: " << _exception.what() << std::endl;
    }

    // Mixing fixed size and dynamic vectors
    // From fixed to dynamic
    vA.Ref(4) = v4;
    std::cout << "vA.Ref(4) = v4:" << std::endl << vA << std::endl;

    // From dynamic to fixed
    vctRandom(vA, 0.0, 10.0);
    v4 = vA.Ref(4, vA.size() - 4); // last 4 elements
    std::cout << "vA: " << std::endl << vA << std::endl
              << "v4: " << std::endl << v4 << std::endl;

    v4.Ref<2>(2) = vA.Ref(2); // replace last 2 elements of v4 with first 2 from vA
    std::cout << "vA: " << std::endl << vA << std::endl
              << "v4: " << std::endl << v4 << std::endl;


    vct4x4 m4;
    vctRandom(m4, -10.0, 10.0);

    // Create a matrix of 2x2 elements starting at index 1,1
    vct2x2 m2(m4.Ref<2, 2>(1, 1));
    std::cout << "m4:" << std::endl << m4 << std::endl
              << "m4Ref<2, 2>(1, 1):" << std::endl << m2 << std::endl;

    // This will throw an exception
    std::cout << "m4.Ref<2, 2>(3, 2) on a matrix of 4 rows will throw an out of range exception" << std::endl;
    try {
        m2.Assign(m4.Ref<2, 2>(3, 2));
    } catch (std::out_of_range _exception) {
        std::cout << " - exception received: " << _exception.what() << std::endl;
    }

    vctFixedSizeMatrix<double, 5, 5> m5;
    m5.SetAll(5.5);

    // Assign part of a vector to another
    m4.Ref<2, 2>(2, 2) = m5.Ref<2, 2>(3, 3);
    std::cout << "m4, bottom right 2x2 elements replaced:" << std::endl << m4 << std::endl;

    // Dynamic matrices
    vctDoubleMat mA, mB;
    mA.SetSize(10, 10); mA.SetAll(1.0); mA(9, 9) = 10.0;
    mB.SetSize(10, 10); mB.SetAll(2.0); mB(9, 9) = 20.0;
    mA.Ref(4, 4) = mB.Ref(4, 4, 6, 6); // assign bottom right 4x4 elements of mB to top left 4x4 of mA
    std::cout << "mA: " << std::endl << mA << std::endl;

    // Possible errors
    try {
        std::cout << "Size mismatch between dynamic matrix references" << std::endl;
        mA.Ref(5, 4) = mB.Ref(6, 3);
    } catch (std::runtime_error _exception) {
        std::cout << " - exception received: " << _exception.what() << std::endl;
    }

    try {
        std::cout << "Reference out of range" << std::endl;
        std::cout << mA.Ref(5, 5, 6, 6) << std::endl;
    } catch (std::out_of_range _exception) {
        std::cout << " - exception received: " << _exception.what() << std::endl;
    }

    // Mixing fixed size and dynamic vectors
    // From fixed to dynamic
    mA.Ref(4, 4) = m4;
    std::cout << "mA.Ref(4) = m4:" << std::endl << mA << std::endl;

    // From dynamic to fixed
    vctRandom(mA, 0.0, 10.0);
    m4 = mA.Ref(4, 4, mA.rows() - 4, mA.cols() - 4); // bottom left 4x4 elements
    std::cout << "mA: " << std::endl << mA << std::endl
              << "m4: " << std::endl << m4 << std::endl;

    m4.Ref<2, 2>(2, 2) = mA.Ref(2, 2); // replace last 2x2 elements of m4 with first 2x2 from mA
    std::cout << "mA: " << std::endl << mA << std::endl
              << "m4: " << std::endl << m4 << std::endl;
}
