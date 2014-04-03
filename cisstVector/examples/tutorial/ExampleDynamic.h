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

void ExampleDynamic(void) {
    // define our prefered types
    typedef vctDynamicVector<double> VectorType;
    typedef vctDynamicMatrix<double> MatrixType;

    // The dynamic vector library may throw exceptions,
    // (derived from std::exception)
    // so we place the operations in a try-catch block.
    try {
        // create an empty vector
        VectorType vector1;
	std::cout << "Size of vector1: " << vector1.size() << std::endl;

        // resize and fill the vector
        unsigned int index;
        vector1.SetSize(5);
        for (index = 0; index < vector1.size(); index++) {
            vector1[index] = index;
        }
        // look at product of elements
        std::cout << "Product of elements is 0? "
		  << vector1.ProductOfElements() << std::endl;

        // create a matrix initialized with zeros
        MatrixType matrix1(7, vector1.size());
        matrix1.SetAll(0.0);

        // set the diagonal to 5.0
        matrix1.Diagonal().SetAll(5.0);

        // look at the sum/product of elements
        std::cout << "Sum of elements is 25? "
		  << matrix1.SumOfElements() << std::endl;

        // multiply matrix1 by vector 2
        VectorType vector2(matrix1.rows());
        vector2.ProductOf(matrix1, vector1);

        // multiply vector1 directly
        VectorType vector3(vector1.size());
        vector3.ProductOf(5.0, vector1);

        // resize vector2 while preserving the data
        vector2.resize(vector3.size());


        // vector2 and vector3 are the same?
        VectorType difference;
        difference = vector3 - vector2;
        difference.AbsSelf();
        std::cout << "Maximum difference between v2 and v3: "
		  << difference.MaxElement() << std::endl;
        // alternative solution
        std::cout << "Maximum difference between v2 and v3: "
		  << (vector3 - vector2).MaxAbsElement() << std::endl;

    }  // end of try block
    // catch block
    catch (std::exception Exception) {
	std::cerr << "Exception occured: " << Exception.what() << std::endl;
    }
}

