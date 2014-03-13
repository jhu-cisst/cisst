// take any dynamic matrix as input parameter
// Const or not, Reference or not
template <class _matrixOwnerType, class _elementType>
void
FunctionDynamicA(vctDynamicConstMatrixBase<_matrixOwnerType,
                                           _elementType> & matrix)
{
    std::cout << matrix << std::endl;
}

// take any non-const matrix as input parameter, Reference or not
template <class _matrixOwnerType, class _elementType>
void
FunctionDynamicB(vctDynamicMatrixBase<_matrixOwnerType,
                                      _elementType> & matrix)
{
    matrix.setsize(10, 10);
}

// only take a matrix as input parameter, can't use a Reference
template <class _elementType>
void
FunctionDynamicC(vctDynamicMatrix<_elementType> & matrix)
{
    matrix.resize(10, 10);
}

// this shows how to restrict to a given type of elements (double)
template <class _matrixOwnerType>
void
FunctionDynamicD(vctDynamicConstMatrixBase<_matrixOwnerType, double> & matrix)
{
    std::cout << matrix << std::endl;
}

// take any two dynamic matrices as input parameters
// Const or not, Reference or not, same type of elements
template <class _matrixOwnerType1, class _matrixOwnerType2, class _elementType>
void
FunctionDynamicE(vctDynamicConstMatrixBase<_matrixOwnerType1,
                                           _elementType> & matrix1,
                 vctDynamicConstMatrixBase<_matrixOwnerType2,
                                           _elementType> & matrix2)
{
    // this will throw an exception if the matrices don't have the correct sizes
    std::cout << matrix1 * matrix2 << std::endl;
}

// function with a return type
template<class _vectorOwnerType, class _elementType>
vctReturnDynamicVector<_elementType>
FunctionDynamicF(const vctDynamicConstVectorBase<_vectorOwnerType,
                                                 _elementType> & inputVector) {
    typedef _elementType value_type;
    vctDynamicVector<value_type> resultStorage(inputVector.size());
    // ...... do something to resultStorage
    return vctReturnDynamicVector<value_type>(resultStorage);
}
