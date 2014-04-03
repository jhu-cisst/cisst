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
        std::cout << inputVolume.Element(zyxIndex) << std::endl
		  << inputVolume.at(zyxIndex) << std::endl
		  << inputVolume[zyxIndex[0]] [zyxIndex[1]] [zyxIndex[2]]
		  << std::endl;

        // Define a new volume type
        typedef vctDynamicNArray<double, 3> NormalizedVolumeType;

        // Type conversions from and existing volume, also defines the size
        NormalizedVolumeType newVolume(inputVolume);
        // alternative
        newVolume.Assign(inputVolume);

        // Find minimum and maximum
        double minElement, maxElement;
        newVolume.MinAndMaxElement(minElement, maxElement);
        std::cout << "Min and max " << minElement << " " << maxElement << std::endl;
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
	std::cerr << "Exception occured: " << Exception.what() << std::endl;
    }
}
