/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides, Anton Deguet

  (C) Copyright 2007-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/



/*!
  \file
  \brief Defines a base function object to allow heterogeneous containers of functions.
*/


#include <cisstMultiTask/mtsFunctionBase.h>
#include <cisstOSAbstraction/osaThreadSignal.h>


mtsFunctionBase::mtsFunctionBase(const bool isProxy):
    ThreadSignal(0),
    IsProxy(isProxy)
{}


void mtsFunctionBase::SetThreadSignal(osaThreadSignal * threadSignal)
{
    this->ThreadSignal = threadSignal;
}


void mtsFunctionBase::ThreadSignalWait(void) const
{
    CMN_ASSERT(this->ThreadSignal);
    this->ThreadSignal->Wait();
}
