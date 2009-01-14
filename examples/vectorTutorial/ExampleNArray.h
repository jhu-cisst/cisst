void ExampleNArray(void) {
    // Define a working volume and index types
    typedef vctDynamicNArray<unsigned short, 3> InputVolumeType;
    typedef InputVolumeType::nsize_type SizeType;
    typedef InputVolumeType::nindex_type IndexType;

    // The dynamic vector library may throw exceptions,
    // (derived from std::exception)
    // so we place the operations in a try-catch block.
    try {    
        // Create a volume object and allocate memory
        SizeType volumeSize(128, 256, 256);
        InputVolumeType inputVolume(volumeSize);
        // alternative to set size and allocate
        inputVolume.SetSize(volumeSize);

        // Fill the memory with data
        vctRandom(inputVolume, 0, 10);
	
        // Random access (read) of elements
        IndexType zyxIndex(15, 120, 240);
        cout << inputVolume.Element(zyxIndex) << endl
             << inputVolume.at(zyxIndex) << endl
             << inputVolume[zyxIndex[0]] [zyxIndex[1]] [zyxIndex[2]]
             << endl;
	
        // Define a new volume type
        typedef vctDynamicNArray<double, 3> NormalizedVolumeType;
	
        // Type conversions from and existing volume, also defines the size
        NormalizedVolumeType newVolume(inputVolume);
        // alternative
        newVolume.Assign(inputVolume);
 
        // Find minimum and maximum
        double minElement, maxElement;
        newVolume.MinAndMaxElement(minElement, maxElement);
        cout << "Min and max " << minElement << " " << maxElement << endl;
        // "shift and bias"
        newVolume.Subtract(minElement); // Also available: operator -=
        newVolume.Divide(maxElement - minElement); // or operator /=
	
        // Operations with several operands
        NormalizedVolumeType noise, corrected;
        corrected.DifferenceOf(newVolume, noise);

        // Slice overlay: array.Slice(dimension, sliceIndex)
        newVolume.Slice(0, 0).SumOf(newVolume.Slice(0, 10),
                                    newVolume.Slice(0, 20));

        // Using a named object for slice overlay
        typedef NormalizedVolumeType::ConstSliceRefType InputSliceType;
        InputSliceType input1;
        input1.SliceOf(newVolume, 1, 15);
        // alternative
        input1.SetRef(newVolume.Slice(1, 15));
    
        // Layout manipulation: permutation of dimensions
        vctDynamicNArrayRef<double, 3> permutedVolume;
        permutedVolume.PermutationOf(newVolume, IndexType(1, 0, 2));
    }  // end of try block
    // catch block
    catch (std::exception Exception) {
        cerr << "Exception occured: " << Exception.what() << endl;
    }
}
