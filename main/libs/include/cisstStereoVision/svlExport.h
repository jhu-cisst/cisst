/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlExport.h 75 2009-02-24 16:47:20Z adeguet1 $

  Author(s): Anton Deguet
  Created on: 2007-03-08

  (C) Copyright 2007-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*! \file
    \brief Macros to export the symbols of cisstStereoVision (in a Dll).
*/

// check if this module is build as a DLL
#ifdef cisstStereoVision_EXPORTS
#define CISST_THIS_LIBRARY_AS_DLL
#endif

// include common defines
#include <cisstCommon/cmnExportMacros.h>

// avoid impact on other modules
#undef CISST_THIS_LIBRARY_AS_DLL

