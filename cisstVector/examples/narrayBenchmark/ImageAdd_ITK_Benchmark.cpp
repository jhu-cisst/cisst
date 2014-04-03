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
#include "itkAddImageFilter.h"
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
typedef itk::AddImageFilter<ImageType, ImageType, ImageType> AddImageFilterType;

int main()
{
    /* create random sequence generator */
    cmnRandomSequence & randomSequence = cmnRandomSequence::GetInstance();

    /* create parent image */
    ImageType::SizeType parentImageSize = {{ 80, 80, 80, 80 }};
    ImageType::Pointer parentImage1 = ImageType::New();
    ImageType::RegionType parentImage1EntireRegion;
    parentImage1EntireRegion.SetSize( parentImageSize );
    parentImage1->SetRegions( parentImage1EntireRegion );
    parentImage1->Allocate();
    ImageType::Pointer parentImage2 = ImageType::New();
    ImageType::RegionType parentImage2EntireRegion;
    parentImage2EntireRegion.SetSize( parentImageSize );
    parentImage2->SetRegions( parentImage2EntireRegion );
    parentImage2->Allocate();

    /* create window region */
    ImageType::SizeType windowSize = {{ 72, 72, 72, 72 }};
    ImageType::IndexType windowStart = {{ 2, 2, 2, 2 }};
    ImageType::RegionType windowRegion1;
    windowRegion1.SetSize( windowSize );
    windowRegion1.SetIndex( windowStart );
    ImageType::RegionType windowRegion2;
    windowRegion2.SetSize( windowSize );
    windowRegion2.SetIndex( windowStart );

    /* print configuration information */
    vctFixedSizeConstVectorRef<unsigned long, 4, 1> parentContainerSize(parentImageSize.GetSize());
    vctFixedSizeConstVectorRef<unsigned long, 4, 1> windowOverlaySize(windowSize.GetSize());
    vctFixedSizeConstVectorRef<long, 4, 1> windowOverlayStart(windowStart.GetIndex());


    cout << cmnPrintf(
        "parentContainerSize : %4d\n"
        "windowOverlaySize   : %4d\n"
        "windowOverlayStart  : %4d\n")
        << parentContainerSize << windowOverlaySize << windowOverlayStart;

    /* create Region of Interest filter 1 */
    RegionOfInterestImageFilterType::Pointer windowFilter1 =
        RegionOfInterestImageFilterType::New();
    windowFilter1->SetInput( parentImage1 );
    windowFilter1->SetRegionOfInterest( windowRegion1 );

    /* create Region of Interest filter 2 */
    RegionOfInterestImageFilterType::Pointer windowFilter2 =
        RegionOfInterestImageFilterType::New();
    windowFilter2->SetInput( parentImage2 );
    windowFilter2->SetRegionOfInterest( windowRegion2 );

    /* create Add Image filter */
    AddImageFilterType::Pointer sumFilter =
        AddImageFilterType::New();
    sumFilter->SetInput1( windowFilter1->GetOutput() );
    sumFilter->SetInput2( windowFilter2->GetOutput() );

    /* create window image 1 */
    ImageType::Pointer windowImage1;
    windowImage1 = windowFilter1->GetOutput();

    /* create window image 2 */
    ImageType::Pointer windowImage2;
    windowImage2 = windowFilter2->GetOutput();

    /* create sum image */
    ImageType::Pointer sumImage;
    sumImage = sumFilter->GetOutput();

    /* create iterators */
    IteratorType parentImage1Iter(
        parentImage1, parentImage1->GetLargestPossibleRegion() );
    IteratorType parentImage2Iter(
        parentImage2, parentImage2->GetLargestPossibleRegion() );

    /* create timers */
    osaStopwatch windowExtractTimer, sumWindowsTimer, extractAndSumTimer;

    {
        std::cout << "Enter something and press <ENTER> to begin benchmark\n";
        char dummy[5];
        std::cin >> dummy;
    }

    cout << endl
        << "All times are in seconds. Times are cumulative." << endl << endl;


    cout << cmnPrintf("     %15s%15s%15s\n")
        << "Window Extract"
        << "Sum Windows"
        << "Extract and sum";

    const unsigned int numIterations = 30;
    unsigned int i;
    for (i = 1; i <= numIterations; ++i)
    {
        /* fill with iterator */
        for (parentImage1Iter.GoToBegin();
            !parentImage1Iter.IsAtEnd();
            ++parentImage1Iter)
        {
            parentImage1Iter.Set( randomSequence.ExtractRandomInt(-20, 20) );
        }
        for (parentImage2Iter.GoToBegin();
            !parentImage2Iter.IsAtEnd();
            ++parentImage2Iter)
        {
            parentImage2Iter.Set( randomSequence.ExtractRandomInt(-20, 20) );
        }

        /* manually invoke pipeline update */
        parentImage1->Modified();
        parentImage2->Modified();

        /* window */
        windowExtractTimer.Start();
        windowImage1->Update();
        windowImage2->Update();
        windowExtractTimer.Stop();

        /* sum */
        sumWindowsTimer.Start();
        sumImage->Update();
        sumWindowsTimer.Stop();

        /* manually invoke pipeline update */
        parentImage1->Modified();
        parentImage2->Modified();

        /* extact and sum */
        extractAndSumTimer.Start();
        sumImage->Update();
        extractAndSumTimer.Stop();


        /* print cumulative times */
        cout << cmnPrintf("%5d%15f%15f%15f\n")
            << i
            << windowExtractTimer.GetElapsedTime()
            << sumWindowsTimer.GetElapsedTime()
            << extractAndSumTimer.GetElapsedTime();

        cout << flush;
    }

    cout << endl;

    /* print total times */
    cout << cmnPrintf("%5s%15f%15f%15f\n\n")
         << "TOTAL"
            << windowExtractTimer.GetElapsedTime()
            << sumWindowsTimer.GetElapsedTime()
            << extractAndSumTimer.GetElapsedTime();

    return 0;
}
