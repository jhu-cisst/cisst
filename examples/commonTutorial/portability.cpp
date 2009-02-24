/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
// $Id$

#include <iostream>
#include <cisstCommon/cmnPortability.h>
using namespace std;

void portability(void) {
#if (CISST_OS == CISST_WINDOWS)
    cout << "Running Windows" << endl;
#elif (CISST_OS == CISST_LINUX)
    cout << "Running Linux" << endl;
#elif (CISST_OS == CISST_RTLINUX)
    cout << "Running RT-Linux" << endl;
#elif (CISST_OS == CISST_IRIX)
    cout << "Running SGI Irix" << endl;
#elif (CISST_OS == CISST_SOLARIS)
    cout << "Running SUN Solaris" << endl;
#elif (CISST_OS == CISST_LINUX_RTAI)
    cout << "Running RTAI Linux" << endl;
#elif (CISST_OS == CISST_CYGWIN)
    cout << "Running Cygwin" << endl;
#elif (CISST_OS == CISST_DARWIN)
    cout << "Running Mac OS X" << endl;
#endif
    
#if (CISST_COMPILER == CISST_GCC)
    cout << "Compiling with gcc" << endl;
#elif (CISST_COMPILER == CISST_SGI_CC)
    cout << "Compiling with SGI CC" << endl;
#elif (CISST_COMPILER == CISST_SUN_CC)
    cout << "Compiling with SUN CC" << endl;
#elif (CISST_COMPILER == CISST_INTEL_CC)
    cout << "Compiling with Intel CC" << endl;
#elif (CISST_COMPILER == CISST_DOTNET2003)
    cout << "Compiling with Microsoft .net 2003, aka 7.1" << endl;
#elif (CISST_COMPILER == CISST_DOTNET2005)
    cout << "Compiling with Microsoft .net 2005, aka 8.0" << endl;
#endif
    
#ifdef CISST_COMPILER_IS_MSVC
    cout << "The compiler is a Microsoft compiler" << endl;
#else
    cout << "The compiler is not a Microsoft compiler" << endl;
#endif
    
    cout << "Please type 0 followed by the \"return\" key" << endl;
    double zero;
    cin >> zero;
    double infinite = 1.0 / zero;
    double nan = infinite / infinite;
    if ((!CMN_ISFINITE(infinite)) && (CMN_ISNAN(nan))) {
        cout << "Thank you, an infinite double looks like: "
             << infinite << " and a NaN like: "
             << nan << endl;
    } else {
        cout << "Did you really enter \"0\"?" << endl;
    }
}


/*
  Author(s):  Anton Deguet
  Created on: 2008-02-10

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/
