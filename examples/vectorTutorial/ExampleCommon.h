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
            cout << "vector[" << index << "] is NaN " << endl;
        }
    }

    // create a rotation based on an a normalized axis
    vctAxAnRot3 axisAngle(vct3(1.0, 0.0, 0.0), cmnPI / 2.0);
    vctQuatRot3 rot1(axisAngle);

    // modify this rotation with a new axis, not well normalized
    vct3 axis(1.0005, 0.0, 0.0);
    if (axis.Norm() > cmnTypeTraits<double>::Tolerance()) {
        cout << "Axis is not normalized wrt default tolerance" << endl;
    }
    cmnTypeTraits<double>::SetTolerance(0.001);
    // this method asserts that the axis is normalized
    axisAngle.Axis() = axis;
    axisAngle.Angle() = cmnPI / 2.0;
    rot1.From(axisAngle);
    cmnTypeTraits<double>::SetTolerance(cmnTypeTraits<double>::DefaultTolerance);
}
