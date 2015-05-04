/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Marcin Balicki, Anton Deguet
  Created on: 2010-03-31

  (C) Copyright 2010-2014 Johns Hopkins University (JHU), All Rights Reserved.

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

#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>
#include <cisstCommon/cmnSerializer.h>
#include <cisstCommon/cmnDeSerializer.h>
#include <cisstCommon/cmnGenericObjectProxy.h>
#include <cisstOSAbstraction/osaTimeServer.h>
#include <cisstMultiTask/mtsTaskManager.h>

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

    /*! The measured sample average. */
    inline double GetAvg(void) const {
        return Avg;
    }
    /*! The measured minimum period of the task */
    inline double GetStdDev(void) const {
        return StdDev;
    }

    /*! The measured maximum period of the task */
    inline double GetMax(void) const {
        return Max;
    }

    /*! The measured task period (difference between current Tic and
        previous Tic). */
    inline double GetMin(void) const {
        return Min;
    }

    /*! Get minimum compute time. */
    inline double MinComputeTime(void) const {
        return mMinComputeTime;
    }

    /*! Get maximum compute time. */
    inline double MaxComputeTime(void) const {
        return mMaxComputeTime;
    }

    /*! Time period between period statistics calculations */
    inline void SetStatisticsUpdatePeriod(const double & time) {
        StatisticsUpdatePeriod = time;
    }

    /*! Get time period between period statistics calculations */
    inline double GetStatisticsUpdatePeriod(void) const {
        return StatisticsUpdatePeriod;
    }

    /*! Add one sample to compute statistics */
    void AddSample(const double sample);

    void AddComputeTime(const double computeTime);

private:

    /*! Internal variables for statistics calculations*/
    double          Sum;   //name clash with original SumOfPeriods
    double          SumOfSquares;
    unsigned int    NumberOfSamples;
    double          LastUpdateTime;
    double          TempMax;
    double          TempMin;

    /*! The time server used to provide absolute and relative times. */
    const osaTimeServer * TimeServer;

    /*! time between period statistics calculations */
    double         Avg;
    double         StdDev;
    double         Max;
    double         Min;
    double mMinComputeTime;
    double mMaxComputeTime;
    double         StatisticsUpdatePeriod;

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
