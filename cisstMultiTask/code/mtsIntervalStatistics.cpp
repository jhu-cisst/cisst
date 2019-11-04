/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s): Marcin Balicki, Anton Deguet
  Created on: 2010-03-31

  (C) Copyright 2004-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisstMultiTask/mtsIntervalStatistics.h>
#include <cisstMultiTask/mtsTaskManager.h>

CMN_IMPLEMENT_SERVICES(mtsIntervalStatistics);

mtsIntervalStatistics::mtsIntervalStatistics():
    mtsGenericObject(),
    mPeriodAvg(0.0),
    mPeriodStdDev(0.0),
    mPeriodMin(0.0),
    mPeriodMax(0.0),
    mComputeTimeAvg(0.0),
    mComputeTimeStdDev(0.0),
    mComputeTimeMin(0.0),
    mComputeTimeMax(0.0),
    mNumberOfSamples(0),
    mNumberOfOverruns(0),
    mStatisticsInterval(1.0),
    mCallback(0)
{
    // Get a pointer to the time server
    mTimeServer = &mtsTaskManager::GetInstance()->GetTimeServer();
    // then reset (order is important, Reset need the time server)
    Reset();
}


void mtsIntervalStatistics::ToStream(std::ostream & outputStream) const
{
    outputStream << "TimeStamp: " << TimestampMember // 1
                 << " PeriodAvg: " << mPeriodAvg
                 << " PeriodStdDev: " << mPeriodStdDev
                 << " PeriodMin: " << mPeriodMin
                 << " PeriodMax: " << mPeriodMax // 5
                 << " ComputeTimeAvg: " << mComputeTimeAvg
                 << " ComputeTimeStdDev: " << mComputeTimeStdDev
                 << " ComputeTimeMin: " << mComputeTimeMin
                 << " ComputeTimeMax: " << mComputeTimeMax
                 << " NumberOfSamples: " << mNumberOfSamples // 10
                 << " NumberOfOverruns: " << mNumberOfOverruns
                 << " StatisticsInterval: " << mStatisticsInterval;
}


void mtsIntervalStatistics::ToStreamRaw(std::ostream & outputStream, const char delimiter,
                                        bool headerOnly, const std::string & headerPrefix) const
{
    mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
    outputStream << delimiter;
    if (headerOnly) {
        outputStream << headerPrefix << "-TimeStamp" << delimiter // 1
                     << headerPrefix << "-PeriodAvg" << delimiter
                     << headerPrefix << "-PeriodStdDev" << delimiter
                     << headerPrefix << "-PeriodMin" << delimiter
                     << headerPrefix << "-PeriodMax" << delimiter // 5
                     << headerPrefix << "-ComputeTimeAvg" << delimiter
                     << headerPrefix << "-ComputeTimeStdDev" << delimiter
                     << headerPrefix << "-ComputeTimeMin" << delimiter
                     << headerPrefix << "-ComputeTimeMax" << delimiter
                     << headerPrefix << "-NumberOfSamples" << delimiter // 10
                     << headerPrefix << "-NumberOfOverruns" << delimiter
                     << headerPrefix << "-StatisticsInterval";
    } else {
        outputStream << this->TimestampMember << delimiter // 1
                     << this->mPeriodAvg << delimiter
                     << this->mPeriodStdDev << delimiter
                     << this->mPeriodMin << delimiter
                     << this->mPeriodMax << delimiter // 5
                     << this->mComputeTimeAvg << delimiter
                     << this->mComputeTimeStdDev << delimiter
                     << this->mComputeTimeMin << delimiter
                     << this->mComputeTimeMax << delimiter
                     << this->mNumberOfSamples << delimiter // 10
                     << this->mNumberOfOverruns << delimiter
                     << this->mStatisticsInterval;
    }
}


void mtsIntervalStatistics::SerializeRaw(std::ostream & outputStream) const
{
    cmnSerializeRaw(outputStream, TimestampMember); // 1
    cmnSerializeRaw(outputStream, mPeriodAvg);
    cmnSerializeRaw(outputStream, mPeriodStdDev);
    cmnSerializeRaw(outputStream, mPeriodMin);
    cmnSerializeRaw(outputStream, mPeriodMax); // 5
    cmnSerializeRaw(outputStream, mComputeTimeAvg);
    cmnSerializeRaw(outputStream, mComputeTimeStdDev);
    cmnSerializeRaw(outputStream, mComputeTimeMin);
    cmnSerializeRaw(outputStream, mComputeTimeMax);
    cmnSerializeRaw(outputStream, mNumberOfSamples); // 10
    cmnSerializeRaw(outputStream, mNumberOfOverruns);
    cmnSerializeRaw(outputStream, mStatisticsInterval);
}

void mtsIntervalStatistics::DeSerializeRaw(std::istream & inputStream)
{
    cmnDeSerializeRaw(inputStream, TimestampMember); // 1
    cmnDeSerializeRaw(inputStream, mPeriodAvg);
    cmnDeSerializeRaw(inputStream, mPeriodStdDev);
    cmnDeSerializeRaw(inputStream, mPeriodMin);
    cmnDeSerializeRaw(inputStream, mPeriodMax); // 5
    cmnDeSerializeRaw(inputStream, mComputeTimeAvg);
    cmnDeSerializeRaw(inputStream, mComputeTimeStdDev);
    cmnDeSerializeRaw(inputStream, mComputeTimeMin);
    cmnDeSerializeRaw(inputStream, mComputeTimeMax);
    cmnDeSerializeRaw(inputStream, mNumberOfSamples); // 10
    cmnDeSerializeRaw(inputStream, mNumberOfOverruns);
    cmnDeSerializeRaw(inputStream, mStatisticsInterval);
}

void mtsIntervalStatistics::Update(const double period, const double computeTime)
{
    // update number of samples
    mRunningNumberOfSamples++;

    // check for min period
    if (period < mPeriodRunningMin) {
        mPeriodRunningMin = period;
    }
    // check for max period
    if (period > mPeriodRunningMax) {
        mPeriodRunningMax = period;
    }
    // for avg and std dev
    mPeriodSum += period;
    mPeriodSumSquares += period * period;

    // check for min compute time
    if (computeTime < mComputeTimeRunningMin) {
        mComputeTimeRunningMin = computeTime;
    }
    // check for max compute time
    if (computeTime > mComputeTimeRunningMax) {
        mComputeTimeRunningMax = computeTime;
    }
    // for avg and std dev
    mComputeTimeSum += computeTime;
    mComputeTimeSumSquares += computeTime * computeTime;

    // count overruns compared to previous average, use number of
    // samples to see if an average has already be calculated
    if ((mNumberOfSamples > 0)
        && (computeTime > mPeriodAvg)) {
        mRunningNumberOfOverruns++;
    }

    // check to see if the statistics need to be to be updated
    // reset the counters and save the data
    const double currentTime = mTimeServer->GetRelativeTime();
    if (currentTime > (mLastUpdateTime + mStatisticsInterval)) {
        // save counters
        mNumberOfSamples = mRunningNumberOfSamples;
        mNumberOfOverruns = mRunningNumberOfOverruns;

        // calculate the avg
        mPeriodAvg = mPeriodSum / static_cast<double>(mNumberOfSamples);
        // std = sqrt(sum(Xi^2) / N - avg^2): see std dev wiki
        mPeriodStdDev = sqrt((mPeriodSumSquares /
                              static_cast<double>(mNumberOfSamples)) - (mPeriodAvg * mPeriodAvg));
        // calculate the avg
        mComputeTimeAvg = mComputeTimeSum / static_cast<double>(mNumberOfSamples);
        // std = sqrt(sum(Xi^2) / N - avg^2): see std dev wiki
        mComputeTimeStdDev = sqrt((mComputeTimeSumSquares /
                                   static_cast<double>(mNumberOfSamples)) - (mComputeTimeAvg * mComputeTimeAvg));

        // save min/max
        mPeriodMin = mPeriodRunningMin;
        mPeriodMax = mPeriodRunningMax;
        mComputeTimeMin = mComputeTimeRunningMin;
        mComputeTimeMax = mComputeTimeRunningMax;

        // timestamp this data
        this->Valid() = true;
        this->Timestamp() = currentTime;

        // finally call user code to act on updated data
        if (mCallback) {
            mCallback->Execute();
        }

        // reset
        Reset();
    }
}

void mtsIntervalStatistics::Reset(void)
{
    mRunningNumberOfSamples = 0;
    mRunningNumberOfOverruns = 0;
    mPeriodSum = 0.0;
    mPeriodSumSquares = 0.0;
    mPeriodRunningMin = cmnTypeTraits<double>::MaxPositiveValue();
    mPeriodRunningMax = cmnTypeTraits<double>::MinPositiveValue();
    mComputeTimeSum = 0.0;
    mComputeTimeSumSquares = 0.0;
    mComputeTimeRunningMin = cmnTypeTraits<double>::MaxPositiveValue();
    mComputeTimeRunningMax = cmnTypeTraits<double>::MinPositiveValue();
    mLastUpdateTime = mTimeServer->GetRelativeTime();
}
