void ExampleSTL(void) {
    typedef vctFixedSizeVector<double, 6> VectorType;

    VectorType vector1;
    int value = vector1.size();
    const VectorType::iterator end = vector1.end();
    VectorType::iterator iter;
    // fill with numbers using iterators
    for (iter = vector1.begin(); iter != end; ++iter) {
        *iter = value--;
    }
    cout << vector1 << endl;
    // sort using std::sort.  cool isn't it?
    std::sort(vector1.begin(), vector1.end());
    cout << vector1 << endl;
}
