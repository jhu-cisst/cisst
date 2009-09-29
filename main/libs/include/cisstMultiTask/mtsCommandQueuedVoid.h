/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsCommandQueuedVoid.h 157 2009-03-14 16:57:11Z adeguet1 $

  Author(s):  Ankur Kapoor, Peter Kazanzides, Anton Deguet
  Created on: 2005-05-03

  (C) Copyright 2005-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Define an internal command for cisstMultiTask
*/


#ifndef _mtsCommandQueuedVoid_h
#define _mtsCommandQueuedVoid_h

#include <cisstMultiTask/mtsCommandQueuedVoidBase.h>

/*! This "class" is equivalent to the base class
  mtsCommandQueuedVoidBase as we don't need any templating for the
  end-user class.  A typedef is enough, we don't need to derive from
  the base class.  We preserved the same filenames just for
  consistency. */
typedef mtsCommandQueuedVoidBase mtsCommandQueuedVoid;


#endif // _mtsCommandQueuedVoid_h

