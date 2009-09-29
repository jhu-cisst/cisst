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
        cout << "Size of vector1: " << vector1.size() << endl;
        
        // resize and fill the vector
        unsigned int index;
        vector1.SetSize(5);
        for (index = 0; index < vector1.size(); index++) {
            vector1[index] = index;
        }
        // look at product of elements
        cout << "Product of elements is 0? "
             << vector1.ProductOfElements() << endl;

        // create a matrix initialized with zeros
        MatrixType matrix1(7, vector1.size());
        matrix1.SetAll(0.0);

        // set the diagonal to 5.0
        matrix1.Diagonal().SetAll(5.0);

        // look at the sum/product of elements
        cout << "Sum of elements is 25? "
             << matrix1.SumOfElements() << endl;
   
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
        cout << "Maximum difference between v2 and v3: "
             << difference.MaxElement() << endl;
        // alternative solution
        cout << "Maximum difference between v2 and v3: "
             << (vector3 - vector2).MaxAbsElement() << endl;

    }  // end of try block
    // catch block
    catch (std::exception Exception) {
        cerr << "Exception occured: " << Exception.what() << endl;
    }
}

