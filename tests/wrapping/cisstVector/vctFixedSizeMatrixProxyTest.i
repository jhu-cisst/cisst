/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: vctFixedSizeMatrixProxyTest.i,v 1.6 2007/04/26 20:12:05 anton Exp $

  Author(s):  Anton Deguet
  Created on: 2005-07-19

  (C) Copyright 2005-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


%module vctFixedSizeMatrixProxyTestPython

%header %{
    #include "cisstVector/cisstVector.i.h"
    #include "vctFixedSizeMatrixProxyTest.h"
%}


%include "std_string.i"

%import "cisstCommon/cisstCommon.i"
%import "cisstVector/cisstVector.i"

%include "vctFixedSizeMatrixProxyTest.h"


/*
  $Log: vctFixedSizeMatrixProxyTest.i,v $
  Revision 1.6  2007/04/26 20:12:05  anton
  All files in tests: Applied new license text, separate copyright and
  updated dates, added standard header where missing.

  Revision 1.5  2006/11/20 20:33:53  anton
  Licensing: Applied new license to tests.

  Revision 1.4  2005/09/26 15:41:48  anton
  cisst: Added modelines for emacs and vi.

  Revision 1.3  2005/09/06 16:27:22  anton
  wrapping tests: Added license.

  Revision 1.2  2005/09/01 06:27:10  anton
  cisst wrapping tests: Minor updates to reflect new organization of SWIG
  interface files.

  Revision 1.1  2005/07/19 17:11:17  anton
  vctFixedSizeMatrix: Added test for SWIG/Python support.
*/
