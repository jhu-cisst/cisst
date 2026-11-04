/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Ankur Kapoor, Peter Kazanzides, Balazs Vagvolgyi, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Declaration of osaThreadSignal
  \ingroup cisstOSAbstraction
 */

#ifndef _osaThreadSignal_h
#define _osaThreadSignal_h

#include <condition_variable>
#include <mutex>
#include <thread>

// Always include last
#include <cisstOSAbstraction/osaExport.h>

class CISST_EXPORT osaThreadSignal
{
public:
    void Wait();
    bool Wait(double timeoutInSec);
    void Raise();
    void Clear();

    bool raised();

    // When Wait is called from the specified thread, the pre/post callbacks will be invoked
    // before/after the wait respectively. This is not thread-safe.
    static void SetWaitCallbacks(const std::thread::id& thread_id, void (*pre)(void), void (*post)(void));

    /*! Print to stream */
    void ToStream(std::ostream & outputStream) const;

private:
    std::condition_variable cond;

    mutable std::mutex state_mutex;
    bool state = false;

    inline static std::thread::id callback_thread_id;
    inline static void (*pre_wait_callback)(void) = nullptr;
    inline static void (*post_wait_callback)(void) = nullptr;
};

inline
std::ostream& operator<< (std::ostream& output,
                          const osaThreadSignal& signal) {
    signal.ToStream(output);
    return output;
}

#endif // _osaThreadSignal_h

