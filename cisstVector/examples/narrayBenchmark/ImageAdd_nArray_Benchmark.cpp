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
    /* create parent containers */
    NArrayType::nsize_type parentContainerSize(80, 80, 80, 80);
    NArrayType parentContainer1;
    NArrayType parentContainer2;
    parentContainer1.SetSize( parentContainerSize );
    parentContainer2.SetSize( parentContainerSize );

    /* create an overlay dynamic vector for fast fill */
    const unsigned int totalSize = parentContainer1.size();
    vctDynamicVectorRef<value_type> parentContainer1AsVector(totalSize, parentContainer1.Pointer());
    vctDynamicVectorRef<value_type> parentContainer2AsVector(totalSize, parentContainer2.Pointer());

    /* create window overlay */
    NArrayType::nsize_type windowOverlaySize(72, 72, 72, 72);
    NArrayType::nsize_type windowOverlayStart(2, 2, 2, 2);
    NArrayType::SubarrayRefType windowOverlay1 =
        parentContainer1.Subarray( windowOverlayStart, windowOverlaySize );
    NArrayType::SubarrayRefType windowOverlay2 =
        parentContainer2.Subarray( windowOverlayStart, windowOverlaySize );

    /* print configuration information */
    cout << cmnPrintf(
        "parentContainerSize : %4d\n"
        "windowOverlaySize   : %4d\n"
        "windowOverlayStart  : %4d\n")
        << parentContainerSize << windowOverlaySize << windowOverlayStart;

    /* create sum container */
    NArrayType sumContainer;
    sumContainer.SetSize( windowOverlaySize );

    /* create timers */
    osaStopwatch sumTimer, plusTimer;

    {
        std::cout << "Enter something and press <ENTER> to begin benchmark\n";
        char dummy[5];
        std::cin >> dummy;
    }

    cout << endl
         << "All times are in seconds. Times are cumulative." << endl << endl;


    cout << cmnPrintf("     %15s%15s\n")
        << "SumOf time"
        << "operator + time";
    

    const unsigned int numIterations = 30;
    unsigned int i;
    for (i = 1; i <= numIterations; ++i)
    {
        /* fill with iterator */
        vctRandom( parentContainer1AsVector, value_type(-20), value_type(20) );
        vctRandom( parentContainer2AsVector, value_type(-20), value_type(20) );

        /* sum */
        sumTimer.Start();
        sumContainer.SumOf( windowOverlay1, windowOverlay2 );
        sumTimer.Stop();

        plusTimer.Start();
        sumContainer = windowOverlay1 + windowOverlay2;
        plusTimer.Stop();

        cout << cmnPrintf("%5d%15d%15d\n")
            << i
            << sumTimer.GetElapsedTime()
            << plusTimer.GetElapsedTime();

        cout << flush;
    }

    cout << endl;

    cout << cmnPrintf("%5s%15d%15d\n\n")
         << "TOTAL"
         << sumTimer.GetElapsedTime()
         << plusTimer.GetElapsedTime();

   return 0;
}
