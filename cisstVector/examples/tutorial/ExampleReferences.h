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
    std::cout << "Ref<2>(1):" << std::endl << v2 << std::endl;

    // This will throw an exception
    std::cout << "Ref<2>(3) on a vector of 4 elements will throw an out of range exception" << std::endl;
    try {
        v2.Assign(v4.Ref<2>(3));
    } catch (std::out_of_range _exception) {
        std::cout << "Exception received: " << _exception.what() << std::endl;
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
}
