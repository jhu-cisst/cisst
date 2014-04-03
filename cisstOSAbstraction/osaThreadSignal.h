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

#include <cisstCommon/cmnPortability.h>

// Always include last
#include <cisstOSAbstraction/osaExport.h>

class osaThreadId;

/* forward declaration for OS dependent internals */
struct osaThreadSignalInternals;

class CISST_EXPORT osaThreadSignal
{
public:
    osaThreadSignal();
    ~osaThreadSignal();

    void Wait(void);
    bool Wait(double timeoutInSec);
    void Raise(void);

    static void SetWaitCallbacks(const osaThreadId &threadId, void (*pre)(void), void (*post)(void));

    /*! Print to stream */
    void ToStream(std::ostream & outputStream) const;

private:
    /*! Internals that are OS-dependent in some way */
    osaThreadSignalInternals * Internals;

    static void (*PreCallback)(void);
    static void (*PostCallback)(void);
};

/*! Stream operator for a thread Id, see osaThreadId. */
inline
std::ostream & operator << (std::ostream & output,
                            const osaThreadSignal & threadSignal) {
    threadSignal.ToStream(output);
    return output;
}

#endif // _osaThreadSignal_h

