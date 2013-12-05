// Take any fixed size vector as input parameter
// Const or not, Reference or not
template <unsigned int _size, int _stride, class _elementType,
          class _dataPtrType>
void
FunctionFixedSizeA(vctFixedSizeConstVectorBase<_size, _stride, _elementType,
                                               _dataPtrType> & vector)
{
    vector.SetAll(0.0);
}

// take any non-const vector as input parameter, Reference or not
template <unsigned int _size, int _stride, class _elementType,
          class _dataPtrType>
void
FunctionFixedSizeB(vctFixedSizeVectorBase<_size, _stride, _elementType,
                                          _dataPtrType> & vector)
{
    std::cout << vector << std::endl;
}

// only take a vector as input parameter, can't use a Reference
template <class _elementType, unsigned int _size>
void
FunctionFixedSizeC(vctFixedSizeVector<_elementType, _size> & vector)
{
    vector.SetAll(0);
}

// this shows how to restrict to a given type of elements (float)
template <unsigned int _size, int _stride, class _dataPtrType>
void
FunctionFixedSizeD(vctFixedSizeConstVectorBase<_size, _stride, float,
                                               _dataPtrType> & vector)
{
    std::cout << vector.Norm() << std::endl;
}

// take any two fixed size vectors as input parameters
// Const or not, Reference or not, same size, same type of elements
template <unsigned int _size, class _elementType,
          int _stride1, class _dataPtrType1,
          int _stride2, class _dataPtrType2>
void
FunctionFixedSizeE(vctFixedSizeConstVectorBase<_size, _stride1, _elementType,
                                               _dataPtrType1> & vector1,
                   vctFixedSizeConstVectorBase<_size, _stride2, _elementType,
                                               _dataPtrType2> & vector2)
{
    std::cout << vector1.SumOfElements() + vector2.SumOfElements() << std::endl;
}
