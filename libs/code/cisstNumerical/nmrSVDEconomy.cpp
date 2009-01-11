/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: nmrSVDEconomy.cpp,v 1.3 2007/04/26 19:33:57 anton Exp $

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

#include <cisstNumerical/nmrSVDEconomy.h>


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: nmrSVDEconomy.cpp,v $
//  Revision 1.3  2007/04/26 19:33:57  anton
//  All files in libraries: Applied new license text, separate copyright and
//  updated dates, added standard header where missing.
//
//  Revision 1.2  2006/11/20 20:33:19  anton
//  Licensing: Applied new license to cisstCommon, cisstVector, cisstNumerical,
//  cisstInteractive, cisstImage and cisstOSAbstraction.
//
//  Revision 1.1  2006/06/19 01:55:46  kapoor
//  cisstNumerical: Memory efficient methods for SVD and pseudo-inverse.
//  Currently tested and works for VCT_COL_MAJOR format only and for dynamic
//  matrices.
//
//
//
// ****************************************************************************
