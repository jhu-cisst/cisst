/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Ankur Kapoor, Peter Kazanzides, Balazs Vagvolgyi, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights Reserved.

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


//class CISST_EXPORT osaThreadSignal {
//
//    /*! Internals that are OS-dependent in some way */
//    enum {INTERNALS_SIZE = 96};    // PKAZ: this can be reduced
//    char Internals[INTERNALS_SIZE];
//
//    /*! Return the size of the actual object used by the OS.  This is
//        used for testing only. */
//    static unsigned int SizeOfInternals(void);
//
//public:
//
//    /*! Default constructor.  Checks that the internal structure
//        is large enough using CMN_ASSERT, then creates and initializes
//        the internal data structures. */
//    osaThreadSignal();
//
//    /*! Default destructor.  Destroys the internal data structures. */
//    ~osaThreadSignal();
//
//    /*! The calling thread is blocked (waits indefinitely) until another thread calls Raise. */
//    void Wait(void);
//
//    /*! The calling thread is blocked until either the timeout expires or another thread calls Raise.
//        \param timeoutInSec the timeout in seconds
//        \returns false if timeout occurred, true otherwise */
//    bool Wait(double timeoutInSec);
//
//    /*! Wakeup any thread that is waiting on this signal. */
//    void Raise(void);
//};

class CISST_EXPORT osaThreadSignal
{
public:
    osaThreadSignal();
    ~osaThreadSignal();

    void Wait(void);
    bool Wait(double timeoutInSec);
    void Raise();

private:
    /*! Internals that are OS-dependent in some way */
    enum {INTERNALS_SIZE = 96};    // PKAZ: this can be reduced
    char Internals[INTERNALS_SIZE];

    /*! Return the size of the actual object used by the OS.  This is
        used for testing only. */
    static unsigned int SizeOfInternals(void);
};

#endif // _osaThreadSignal_h

