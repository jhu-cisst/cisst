/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 Author(s): Marcin Balicki, Anton Deguet
 Created on: 2010-03-31

 (C) Copyright 2004-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsIntervalStatistics.h>

CMN_IMPLEMENT_SERVICES(mtsIntervalStatistics);

mtsIntervalStatistics::mtsIntervalStatistics():
    mtsGenericObject(),
    Sum(0.0),
    SumOfSquares(0.0),
    NumberOfSamples(0),
    LastUpdateTime(0.0),
    TempMax(0.0),
    TempMin(0.0),
    Avg(0.0),
    StdDev(0.0),
    Max(0.0),
    Min(0.0),
    mMinComputeTime(cmnTypeTraits<double>::MaxPositiveValue()),
    mMaxComputeTime(cmnTypeTraits<double>::MinPositiveValue()),
    StatisticsUpdatePeriod(1.0)
{
    // Get a pointer to the time server
    TimeServer = &mtsTaskManager::GetInstance()->GetTimeServer();
}


void mtsIntervalStatistics::ToStream(std::ostream & outputStream) const
{
    outputStream << "TimeStamp: " << TimestampMember
                 << " Avg: " << Avg
                 << " StdDev: " << StdDev
                 << " Max: " << Max
                 << " Min: " << Min
                 << " MinComputeTime: " << mMinComputeTime
                 << " MaxComputeTime: " << mMaxComputeTime
                 << " Period: " << StatisticsUpdatePeriod;
}


void mtsIntervalStatistics::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                        bool headerOnly, const std::string & headerPrefix) const
{
    mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    outputStream << delimiter;
    if (headerOnly) {
        outputStream << headerPrefix << "-TimeStamp" << delimiter
                     << headerPrefix << "-Avg" << delimiter
                     << headerPrefix << "-StdDev" << delimiter
                     << headerPrefix << "-Max" << delimiter
                     << headerPrefix << "-Min" << delimiter
                     << headerPrefix << "-MinComputeTime" << delimiter
                     << headerPrefix << "-MaxComputeTime" << delimiter
                     << headerPrefix << "-Period";
    } else {
        outputStream << this->TimestampMember << delimiter
                     << this->Avg << delimiter
                     << this->StdDev << delimiter
                     << this->Max << delimiter
                     << this->Min << delimiter
                     << this->mMinComputeTime << delimiter
                     << this->mMaxComputeTime << delimiter
                     << this->StatisticsUpdatePeriod;
    }
}


void mtsIntervalStatistics::SerializeRaw(std::ostream & outputStream) const
{
    cmnSerializeRaw(outputStream, TimestampMember);
    cmnSerializeRaw(outputStream, StdDev);
    cmnSerializeRaw(outputStream, Avg);
    cmnSerializeRaw(outputStream, Max);
    cmnSerializeRaw(outputStream, Min);
    cmnSerializeRaw(outputStream, StatisticsUpdatePeriod);
    cmnSerializeRaw(outputStream, Sum);
    cmnSerializeRaw(outputStream, SumOfSquares);
    cmnSerializeRaw(outputStream, NumberOfSamples);
    cmnSerializeRaw(outputStream, TempMax);
    cmnSerializeRaw(outputStream, TempMin);
    cmnSerializeRaw(outputStream, mMinComputeTime);
    cmnSerializeRaw(outputStream, mMaxComputeTime);
}

void mtsIntervalStatistics::DeSerializeRaw(std::istream & inputStream)
{
    cmnDeSerializeRaw(inputStream, TimestampMember);
    cmnDeSerializeRaw(inputStream, StdDev);
    cmnDeSerializeRaw(inputStream, Avg);
    cmnDeSerializeRaw(inputStream, Max);
    cmnDeSerializeRaw(inputStream, Min);
    cmnDeSerializeRaw(inputStream, StatisticsUpdatePeriod);
    cmnDeSerializeRaw(inputStream, Sum);
    cmnDeSerializeRaw(inputStream, SumOfSquares);
    cmnDeSerializeRaw(inputStream, NumberOfSamples);
    cmnDeSerializeRaw(inputStream, TempMax);
    cmnDeSerializeRaw(inputStream, TempMin);
    cmnDeSerializeRaw(inputStream, mMinComputeTime);
    cmnDeSerializeRaw(inputStream, mMaxComputeTime);
    //since we might be on a different computer the timing should be different
    LastUpdateTime = TimeServer->GetRelativeTime();
}

void mtsIntervalStatistics::AddSample(const double sample) {

    //check for max
    if (TempMax < sample){
        TempMax = sample;
    }
    //check for min.
    if (TempMin > sample){
        TempMin = sample;
    }

    Sum += sample; 
    SumOfSquares += sample * sample;
    NumberOfSamples++;
    //Check to see if the statistics need to be to be updated
    //reset the counters and save the data
    if (TimeServer->GetRelativeTime() > (LastUpdateTime + StatisticsUpdatePeriod)) {
        //save the max/min
        Min = TempMin;
        Max = TempMax;
        //calculate the avg.
        Avg = Sum / (double)NumberOfSamples;

        //std = sqrt(     sum(Xi^2) /N  - avg^2 )  : see std dev wiki
        StdDev = sqrt((SumOfSquares / (double)NumberOfSamples) - (Avg*Avg) );
        // CMN_LOG_CLASS_RUN_DEBUG << *this<<std::endl;

        //reset
        NumberOfSamples = 0;
        Sum = 0.0;
        SumOfSquares = 0.0;
        //Should this should be the next period?
        TempMax = sample;
        TempMin = sample;
        LastUpdateTime = TimeServer->GetRelativeTime();

        mMaxComputeTime = cmnTypeTraits<double>::MinPositiveValue();
        mMinComputeTime = cmnTypeTraits<double>::MaxPositiveValue();
    }
}

void mtsIntervalStatistics::AddComputeTime(const double computeTime)
{
    //check for max
    if (mMaxComputeTime < computeTime){
        mMaxComputeTime = computeTime;
    }
    //check for min.
    if (mMinComputeTime > computeTime){
        mMinComputeTime = computeTime;
    }
}
