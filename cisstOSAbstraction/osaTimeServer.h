/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Peter Kazanzides
  Created on: 2008-12-30

  (C) Copyright 2008-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of osaTimeServer class
 */

#ifndef _osaTimeServer_h
#define _osaTimeServer_h

#include <chrono>
#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegister.h>

// Always include last
#include <cisstOSAbstraction/osaExport.h>

/*!
  \brief Class for relative time

  \ingroup cisstOSAbstraction

  The osaRelativeTime class is used to manage the relationship between relative
  and absolute time. Specifically, the GetRelativeTime method returns the number
  of seconds that have elapsed since the time origin was set (via a call to
  SetTimeOrigin).

  Although it might seem possible to implement this class without OS dependencies
  (e.g., by using osaGetAbsoluteTime), this is problematic on some platforms, such
  as Windows, where it is necessary to synchronize multiple time sources to get
  a high resolution time that is relative to an absolute time.  Also, even on
  non-Windows platforms, it would not have been as efficient because the
  native (OS-specific) "absolute time" structure would have to converted
  to an osaAbsoluteTime structure for every call to GetRelativeTime.

  This class has significant overlap with osaStopwatch.
*/


class CISST_EXPORT SynchronizedMonotonicClock {
public:
    // Definitions for C++ Clock named requirement
    using rep = long long;
    using period = std::nano;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<SynchronizedMonotonicClock>;
    static constexpr bool is_steady = true;

    static duration fromSeconds(double seconds) {
        const auto seconds_d = std::chrono::duration<double>(seconds);
        return std::chrono::duration_cast<duration>(seconds_d);
    }

    static time_point now() noexcept {
        auto steady_now = std::chrono::steady_clock::now();
        auto elapsed_steady = steady_now - last_steady_time_;
        
        // Apply the slew rate factor (e.g., 1.0 + slew_ppm / 1e6)
        double scaled_ticks = elapsed_steady.count() * rate_multiplier_;
        virtual_time_ += duration(static_cast<rep>(scaled_ticks));
        last_steady_time_ = steady_now;
        
        return time_point{virtual_time_};
    }

private:
    static std::chrono::steady_clock::time_point last_steady_time_;
    static duration virtual_time_;
    static double rate_multiplier_;
};

class CISST_EXPORT osaTimeServer: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    /*! Synchronize, method used to synchronize high performance
        counter with time of day. */
    void Synchronize(void);

    SynchronizedMonotonicClock::time_point epoch;

public:
    osaTimeServer();

    /*! Set the time origin for the relative time measurements to "now".
        Subsequent calls to GetRelativeTime will return the number of
        seconds that have elaspsed since this call was made. */
    void SetTimeOrigin(void);

    /*! Set the time origin based on the specified time server.
        For systems that need to synchronize to the performance counter,
        this also sets the offset (i.e., does not call Synchronize again). */
    void SetTimeOriginFrom(const osaTimeServer *other);

    /*! Get the time origin that is in effect for the relative time
        measurements.
        \param origin Returns the current origin */
    bool GetTimeOrigin(SynchronizedMonotonicClock::time_point& origin) const;

    /*! Get the number of seconds that have elapsed since the time origin.
        \returns The number of seconds */
    SynchronizedMonotonicClock::duration GetRelativeTime(void) const;

    /*! Get the number of seconds that have elapsed since the time origin.
        \returns The number of seconds */
    double GetRelativeTimeInSeconds(void) const;

   /*! Get the number of seconds that have elapsed since *the clock's epoch* ~1970~.
     \returns The number of seconds */
    double GetAbsoluteTimeInSeconds(void) const;

      /*! Get the absolute time since *the clock's epoch*   ~1970~. // TODO: do we need 1970 epoch?
     \returns The number of seconds */
    SynchronizedMonotonicClock::time_point GetAbsoluteTime(void) const;

    /*! Estimate drift between synchronized clocks (Windows only) */
    double EstimateDrift(void) const;

    /*! Convert the specified relative time (in seconds) to an absolute
        time by adding it to the time origin.
        \param relative The relative time value in seconds (input)
        \param absolute The absolute time value (output)
    */
    SynchronizedMonotonicClock::time_point RelativeToAbsolute(SynchronizedMonotonicClock::duration relative) const;

    /*! Convert the specified absolute time to a relative time
        by subtracting from the time origin and converting to seconds.
        \param absolute The absolute time value (input)
        \returns The relative time value in seconds
    */
    SynchronizedMonotonicClock::duration AbsoluteToRelative(SynchronizedMonotonicClock::time_point absolute) const;
};


CMN_DECLARE_SERVICES_INSTANTIATION(osaTimeServer)


#endif // _osaTimeServer_h

