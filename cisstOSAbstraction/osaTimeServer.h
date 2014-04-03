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

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegister.h>

// Always include last
#include <cisstOSAbstraction/osaExport.h>

// PKAZ: move this to osaGetTime.h
struct CISST_EXPORT osaAbsoluteTime {
    long sec;   // seconds
    long nsec;  // nano-seconds

    osaAbsoluteTime() : sec(0L), nsec(0L) {}
    osaAbsoluteTime(long seconds, long nanoseconds) : sec(seconds), nsec(nanoseconds) {}
    ~osaAbsoluteTime() {}

    /*! Returns absolute time in seconds relative to Jan 1, 1970. */
    double ToSeconds(void) const;

    /*! Set from time in seconds using floating point double */
    void FromSeconds(double timeInSeconds);
};


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

class CISST_EXPORT osaTimeServer: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

    friend class osaTimeServerTest;

    /*! Internals that are OS-dependent in some way */
    enum {INTERNALS_SIZE = 32};
    char Internals[INTERNALS_SIZE];

    /*! Return the size of the actual object used by the OS.  This is
        used for testing only. */
    static unsigned int SizeOfInternals(void);

    /*! Synchronize, method used to synchronize high performance
        counter with time of day. */
    void Synchronize(void);

public:
    /*! Default constructor. */
    osaTimeServer();
    /*! Destructor */
    ~osaTimeServer();

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
    bool GetTimeOrigin(osaAbsoluteTime & origin) const;

    /*! Get the number of seconds that have elapsed since the time origin.
        \returns The number of seconds */
    double GetRelativeTime(void) const;

   /*! Get the number of seconds that have elapsed since 1970.
     \returns The number of seconds */
    double GetAbsoluteTimeInSeconds(void) const;

      /*! Get the absolute time since 1970.
     \returns The number of seconds */
    osaAbsoluteTime GetAbsoluteTime(void) const;

    /*! Estimate drift between synchronized clocks (Windows only) */
    double EstimateDrift(void) const;

    /*! Convert the specified relative time (in seconds) to an absolute
        time by adding it to the time origin.
        \param relative The relative time value in seconds (input)
        \param absolute The absolute time value (output)
    */
    void RelativeToAbsolute(double relative, osaAbsoluteTime & absolute) const;

    /*! Convert the specified absolute time to a relative time
        by subtracting from the time origin and converting to seconds.
        \param absolute The absolute time value (input)
        \returns The relative time value in seconds
    */
    double AbsoluteToRelative(const osaAbsoluteTime & absolute) const;
};


CMN_DECLARE_SERVICES_INSTANTIATION(osaTimeServer)


#endif // _osaTimeServer_h

