/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrSVD.cpp,v 1.5 2007/04/26 19:33:57 anton Exp $

  Author(s): Ankur Kapoor
  Created on: 2005-10-18

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstNumerical/nmrSVD.h>


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrSVD.cpp,v $
//  Revision 1.5  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.4  2006/11/20 20:33:19  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
//
//  Revision 1.3  2005/11/20 21:23:41  kapoor
//  nmrSVD: Changes to the netlib wrappers. See wiki pages SvdNotes and ticket #184.
//
//  Revision 1.2  2005/10/21 23:13:27  anton
//  nmrSVD: Working version for gcc 3.3, gcc 4.0 and VCPP7.1 (still some work
//  to do, see #186).  Corrected CMakeLists.txt (see #185).
//
//  Revision 1.1  2005/10/19 01:42:26  kapoor
//  New version of nmrSVD, replaces existing API. See ticket #184 for details.
//
//
// ****************************************************************************
