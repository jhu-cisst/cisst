/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Ofri Sadowsky, Daniel Li
  Created on: 2007-07-01
  
  (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstVector/vctDynamicNArray.h>
#include <cisstVector/vctDynamicNArrayRef.h>
#include <cisstVector/vctDynamicVector.h>
#include <cisstVector/vctContainerTraits.h>
#include <cisstOSAbstraction/osaStopwatch.h>
#include <cisstCommon/cmnRandomSequence.h>
#include <cisstVector/vctRandom.h>
#include <cisstVector/vctPrintf.h>
#include <cisstCommon/cmnPrintf.h>
#include <iostream>


using namespace std;

/* define typedefs */
typedef int value_type;
typedef vctDynamicNArray<value_type, 4> NArrayType;
typedef vctDynamicNArray<value_type, 3> NArrayLowerDimType;


int main()
{
    /* create parent container */
    NArrayType::nsize_type parentContainerSize(93, 94, 95, 96);
    NArrayType parentContainer;
    parentContainer.SetSize( parentContainerSize );

    /* create an overlay dynamic vector for fast fill */
    const unsigned int totalSize = parentContainer.size();
    vctDynamicVectorRef<value_type> parentContainerAsVector(totalSize, parentContainer.Pointer());

    /* create window overlay */
    NArrayType::nsize_type windowOverlaySize(71, 72, 73, 74);
    NArrayType::nsize_type windowOverlayStart(16, 16, 16, 16);
    NArrayType::SubarrayRefType windowOverlay =
        parentContainer.Subarray( windowOverlayStart, windowOverlaySize );

    /* create slice overlay */
    const unsigned int sliceDimensionIndex = 2;
    const unsigned int sliceSelectionIndex = 64;
    NArrayType::SliceRefType sliceOverlay =
        parentContainer.Slice(sliceDimensionIndex, sliceSelectionIndex);

    /* create permutation overlay */
    NArrayType::ndimension_type permuteDimensionOrder(3, 1, 2, 0);
    NArrayType::PermutationRefType permuteOverlay =
        parentContainer.Permutation( permuteDimensionOrder );

    /* print configuration information */
    cout << cmnPrintf(
        "parentContainerSize : %4d\n"
        "windowOverlaySize   : %4d\n"
        "windowOverlayStart  : %4d\n"
        "sliceParameters     : %4d %4d\n"
        "permutationOrder    : %4d\n")
        << parentContainerSize << windowOverlaySize << windowOverlayStart << sliceDimensionIndex << sliceSelectionIndex
        << permuteDimensionOrder;
    
    /* create window container */
    NArrayType windowContainer;
    windowContainer.SetSize( windowOverlay.sizes() );

    /* create slice container */
    NArrayLowerDimType sliceContainer;
    sliceContainer.SetSize( sliceOverlay.sizes() );
    
    /* create permutation container */
    NArrayType permuteContainer;
    permuteContainer.SetSize( permuteOverlay.sizes() );

    /* create iterators */
    NArrayType::iterator parentContainerIter;

    /* create timers */
    osaStopwatch fillTimer, windowTimer, sliceTimer, permuteTimer;

    {
        std::cout << "Enter something and press <ENTER> to begin benchmark\n";
        char dummy[5];
        std::cin >> dummy;
    }


    cout << endl
         << "All times are in seconds" << endl << endl;


    cout << cmnPrintf("%20s%15s%15s%15s\n")
        << "Fill time"
        << "Window time"
        << "Slice time"
        << "Permute time";
    

    const unsigned int numIterations = 30;
    unsigned int i;
    for (i = 1; i <= numIterations; ++i)
    {
        if (0) {
            fillTimer.Reset();
            windowTimer.Reset();
            sliceTimer.Reset();
            permuteTimer.Reset();
        }

        /* fill with iterator */
        fillTimer.Start();
        vctRandom(parentContainerAsVector, value_type(-20), value_type(20));
        fillTimer.Stop();

        /* window */
        windowTimer.Start();
        windowContainer.Assign( windowOverlay );
        windowTimer.Stop();

        /* slice */
        sliceTimer.Start();
        sliceContainer.Assign( sliceOverlay );
        sliceTimer.Stop();

        /* permute */
        permuteTimer.Start();
        permuteContainer.Assign( permuteOverlay );
        permuteTimer.Stop();

        cout << cmnPrintf("%5d%15d%15d%15d%15d\n")
            << i
            << fillTimer.GetElapsedTime()
            << windowTimer.GetElapsedTime()
            << sliceTimer.GetElapsedTime()
            << permuteTimer.GetElapsedTime();

        cout << flush;
    }

    cout << endl;

    cout << cmnPrintf("%5s%15d%15d%15d%15d\n\n")
        << "TOTAL"
        << fillTimer.GetElapsedTime()
        << windowTimer.GetElapsedTime()
        << sliceTimer.GetElapsedTime()
        << permuteTimer.GetElapsedTime();

   return 0;
}
