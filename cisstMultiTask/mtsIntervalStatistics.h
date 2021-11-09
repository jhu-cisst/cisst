/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Marcin Balicki, Anton Deguet
  Created on: 2010-03-31

  (C) Copyright 2010-2019 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Statistical analysis for periodic signals
*/

#ifndef _mtsIntervalStatistics_h
#define _mtsIntervalStatistics_h

#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>
#include <cisstCommon/cmnGenericObjectProxy.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>
#include <cisstMultiTask/mtsCallableVoidMethod.h>

#include <cisstMultiTask/mtsExport.h>

/*!
  \ingroup cisstMultiTask

   Calculate the statistics (avg,std,min,max) on the vector of data that is added one sample at time.
   AddSample checks if the statistics need to be recalculated after a given period elapses (eg 1sec).

 */
class CISST_EXPORT mtsIntervalStatistics : public mtsGenericObject {

    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    /*! TimeTicks are typedef'ed as unsigned long long (64 bits)*/
    typedef unsigned long long int TimeTicksType;

    /*! Base type */
    typedef mtsGenericObject BaseType;

    /*! The measured period average. */
    inline const double & PeriodAvg(void) const {
        return mPeriodAvg;
    }

    /*! The measured standard deviation for period average. */
    inline const double & PeriodStdDev(void) const {
        return mPeriodStdDev;
    }

    /*! The measured minimum period of the task */
    inline const double & PeriodMin(void) const {
        return mPeriodMin;
    }

    /*! The measured maximum period of the task */
    inline const double & PeriodMax(void) const {
        return mPeriodMax;
    }

    /*! The measured compute time average. */
    inline const double & ComputeTimeAvg(void) const {
        return mComputeTimeAvg;
    }

    /*! The measured standard deviation for compute time average. */
    inline const double & ComputeTimeStdDev(void) const {
        return mComputeTimeStdDev;
    }

    /*! Get minimum compute time. */
    inline const double & ComputeTimeMin(void) const {
        return mComputeTimeMin;
    }

    /*! Get maximum compute time. */
    inline const double & ComputeTimeMax(void) const {
        return mComputeTimeMax;
    }

    /*! Get number of samples used for time window */
    inline const unsigned int & NumberOfSamples(void) const {
        return mNumberOfSamples;
    }

    /*! Get number of overruns, i.e. compute time greater than average
      period in previous interval. */
    inline const unsigned int & NumberOfOverruns(void) const {
        return mNumberOfOverruns;
    }

    /*! Time period between period statistics calculations */
    inline void SetStatisticsInterval(const double & time) {
        mStatisticsInterval = time;
    }

    /*! Get time period between period statistics calculations */
    inline const double & StatisticsInterval(void) const {
        return mStatisticsInterval;
    }

    /*! Add one sample to compute statistics */
    void Update(const double sample, const double computeTime);

    /*! Set callback to act on updated statistics */
    inline void SetCallback(mtsCallableVoidBase * callback) {
        mCallback = callback;
    }

    template <class __classType>
    inline void SetCallback(void (__classType::*method)(void),
                            __classType * classInstantiation) {
        this->SetCallback(new mtsCallableVoidMethod<__classType>(method, classInstantiation));
    }

    void Reset(void);

    inline void SetFromExisting(const double periodAvg,
                                const double periodStdDev,
                                const double periodMin,
                                const double periodMax,
                                const double computeTimeAvg,
                                const double computeTimeStdDev,
                                const double computeTimeMin,
                                const double computeTimeMax,
                                const unsigned int numberOfSamples,
                                const unsigned int numberOfOverruns,
                                const double statisticsInterval) {
        mPeriodAvg = periodAvg;
        mPeriodStdDev = periodStdDev;
        mPeriodMin = periodMin;
        mPeriodMax = periodMax;
        mComputeTimeAvg = computeTimeAvg;
        mComputeTimeStdDev = computeTimeStdDev;
        mComputeTimeMin = computeTimeMin;
        mComputeTimeMax = computeTimeMax;
        mNumberOfSamples = numberOfSamples;
        mNumberOfOverruns = numberOfOverruns;
        mStatisticsInterval = statisticsInterval;
    }
    
private:

    /*! Internal variables for statistics calculations */
    double mPeriodSum;
    double mPeriodSumSquares;
    double mPeriodRunningMin;
    double mPeriodRunningMax;
    double mComputeTimeSum;
    double mComputeTimeSumSquares;
    double mComputeTimeRunningMin;
    double mComputeTimeRunningMax;
    unsigned int mRunningNumberOfSamples;
    unsigned int mRunningNumberOfOverruns;
    double mLastUpdateTime;

    /*! The time server used to provide absolute and relative times. */
    const osaTimeServer * mTimeServer;

    /*! Members that can be accessed after computation, actual stats */
    double mPeriodAvg;
    double mPeriodStdDev;
    double mPeriodMin;
    double mPeriodMax;
    double mComputeTimeAvg;
    double mComputeTimeStdDev;
    double mComputeTimeMin;
    double mComputeTimeMax;
    unsigned int mNumberOfSamples;
    unsigned int mNumberOfOverruns;

    /*! configuration. */
    double mStatisticsInterval;

    /*! optional callback */
    mtsCallableVoidBase * mCallback;

public:

    mtsIntervalStatistics();
    inline ~mtsIntervalStatistics() {};

    /*! Human readable text output */
    void ToStream(std::ostream & outputStream) const;

    /*! Machine reabable text output */
    void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
                     bool headerOnly = false, const std::string & headerPrefix = "") const;

    /*! Serialize the content of the object without any extra
      information, i.e. no class type nor format version.  The
      "receiver" is supposed to already know what to expect. */
    void SerializeRaw(std::ostream & outputStream) const;

    /*! De-serialize the content of the object without any extra
      information, i.e. no class type nor format version. */
    virtual void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsIntervalStatistics)

#endif // _mtsIntervalStatistics_h
