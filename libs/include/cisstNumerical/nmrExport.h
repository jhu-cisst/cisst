/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrExport.h,v 1.8 2007/04/26 19:33:57 anton Exp $

  Author(s):	Anton Deguet
  Created on:   2001-11-04

  (C) Copyright 2001-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*! \file
    \brief Rules of exporting.
*/

// check if this module is build as a DLL
#ifdef cisstNumerical_EXPORTS
#define CISST_THIS_LIBRARY_AS_DLL
#endif

// include common defines
#include <cisstCommon/cmnExportMacros.h>

// avoid impact on other modules
#undef CISST_THIS_LIBRARY_AS_DLL


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrExport.h,v $
//  Revision 1.8  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.7  2006/11/20 20:33:19  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
//
//  Revision 1.6  2005/09/26 15:41:46  anton
//  cisst: Added modelines for emacs and vi.
//
//  Revision 1.5  2005/06/03 18:20:58  anton
//  cisstNumerical: Added license.
//
//  Revision 1.4  2004/11/08 18:06:53  anton
//  cisstNumerical: Major Doxygen update based on Ankur's notes and Ofri's
//  comments in the code.
//
//  Revision 1.3  2004/10/27 02:41:59  anton
//  cisstNumerical: Code to compile as a DLL.  Most of the code was in place but
//  never tested.  Minor additions including to disable some warnings.
//
//  Revision 1.2  2004/10/22 01:24:37  ofri
//  Cleanup following migration to cisst, and rebuild with cygwin/gcc
//
//  Revision 1.1  2004/10/21 19:52:51  ofri
//  Adding polynomial-related header files to the repository -- imported from Ofri.
//  See ticket #78.
//
//  Revision 1.4  2003/07/16 22:07:02  ofri
//  Adding serialization functionality to the polynomial classes and the
//  nmrPolynomialTermPowerIndex class
//
//  Revision 1.3  2003/07/10 15:50:16  ofri
//  Adding change history log to the files in cisstNumerical. I guess in a few files
//  there was replacement of spaces and tabs, which was recorded in CVS as
//  a big change, unfortunately. But for most, it's just adding the tags.
//
//
// ****************************************************************************

