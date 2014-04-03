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

#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkPermuteAxesImageFilter.h"
#include <cisstOSAbstraction/osaStopwatch.h>
#include <cisstCommon/cmnRandomSequence.h>
#include <cisstCommon/cmnPrintf.h>
#include <cisstVector/vctFixedSizeVector.h>
#include <cisstVector/vctPrintf.h>
#include <iostream>

using namespace std;

/* define typedefs */
typedef itk::Image<int, 4> ImageType;
typedef itk::Image<int, 3> LowerDimImageType;
typedef itk::ImageRegionIterator<ImageType> IteratorType;
typedef itk::RegionOfInterestImageFilter<ImageType, ImageType> RegionOfInterestImageFilterType;
typedef itk::ExtractImageFilter<ImageType, LowerDimImageType> ExtractImageFilterType;
typedef itk::PermuteAxesImageFilter<ImageType> PermuteAxesImageFilterType;

int main()
{
    /* create random sequence generator */
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();

    /* create parent image */
    ImageType::Pointer parentImage = ImageType::New();
    ImageType::SizeType parentImageSize = {{ 93, 94, 95, 96 }};
    ImageType::RegionType parentImageEntireRegion;
    parentImageEntireRegion.SetSize( parentImageSize );
    parentImage->SetRegions( parentImageEntireRegion );
    parentImage->Allocate();

    /* create window region */
    ImageType::SizeType windowSize = {{ 71, 72, 73, 74 }};
    ImageType::IndexType windowStart = {{ 16, 16, 16, 16 }};
    ImageType::RegionType windowRegion;
    windowRegion.SetSize( windowSize );
    windowRegion.SetIndex( windowStart );

    /* create slice region */
    ImageType::SizeType sliceSize = {{ 93, 94, 0, 96 }};
    ImageType::IndexType sliceStart = {{ 0, 0, 64, 0 }};
    ImageType::RegionType sliceRegion;
    sliceRegion.SetSize( sliceSize );
    sliceRegion.SetIndex( sliceStart );

    /* create permute order */
    PermuteAxesImageFilterType::PermuteOrderArrayType
        permuteDimensionOrder;
    permuteDimensionOrder[0] = 3;
    permuteDimensionOrder[1] = 1;
    permuteDimensionOrder[2] = 2;
    permuteDimensionOrder[3] = 0;

    /* print configuration information */
    vctFixedSizeConstVectorRef<unsigned long, 4, 1> parentContainerSize(parentImageSize.GetSize());
    vctFixedSizeConstVectorRef<unsigned long, 4, 1> windowOverlaySize(windowSize.GetSize());
    vctFixedSizeConstVectorRef<long, 4, 1> windowOverlayStart(windowStart.GetIndex());
    vctFixedSizeConstVectorRef<unsigned long, 4, 1> sliceSizeRef(sliceSize.GetSize());
    vctFixedSizeConstVectorRef<long, 4, 1> sliceStartRef(sliceStart.GetIndex());
    vctFixedSizeConstVectorRef<unsigned int, 4, 1> permuteDimensionOrderRef(permuteDimensionOrder.GetDataPointer());


    cout << cmnPrintf(
        "parentContainerSize : %4d\n"
        "windowOverlaySize   : %4d\n"
        "windowOverlayStart  : %4d\n"
        "sliceParameters     : %4d %4d\n"
        "permutationOrder    : %4d\n")
        << parentContainerSize  << windowOverlaySize << windowOverlayStart << sliceSizeRef << sliceStartRef
        << permuteDimensionOrderRef;

    /* create Region of Interest filter */
    RegionOfInterestImageFilterType::Pointer windowFilter =
        RegionOfInterestImageFilterType::New();
    windowFilter->SetInput( parentImage );
    windowFilter->SetRegionOfInterest( windowRegion );

    /* create Extract Image filter */
    ExtractImageFilterType::Pointer sliceFilter =
        ExtractImageFilterType::New();
    sliceFilter->SetInput( parentImage );
    sliceFilter->SetExtractionRegion( sliceRegion );

    /* create Permute Axes Image filter */
    PermuteAxesImageFilterType::Pointer permuteFilter =
        PermuteAxesImageFilterType::New();
    permuteFilter->SetInput( parentImage );
    permuteFilter->SetOrder( permuteDimensionOrder );

    /* create window image */
    ImageType::Pointer windowImage;
    windowImage = windowFilter->GetOutput();

    /* create slice image */
    LowerDimImageType::Pointer sliceImage;
    sliceImage = sliceFilter->GetOutput();

    /* create permutation image */
    ImageType::Pointer permuteImage;
    permuteImage = permuteFilter->GetOutput();

    /* create iterators */
    IteratorType parentImageIter(
        parentImage, parentImage->GetLargestPossibleRegion() );

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
        /* fill with iterator */
        fillTimer.Start();
        for (parentImageIter.GoToBegin();
            !parentImageIter.IsAtEnd();
            ++parentImageIter)
        {
            parentImageIter.Set( randomSequence.ExtractRandomInt(-20, 20) );
        }
        fillTimer.Stop();

        /* manually invoke pipeline update */
        parentImage->Modified();

        /* window */
        windowTimer.Start();
        windowImage->Update();
        windowTimer.Stop();

        /* slice */
        sliceTimer.Start();
        sliceImage->Update();
        sliceTimer.Stop();

        /* permute */
        permuteTimer.Start();
        permuteImage->Update();
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
