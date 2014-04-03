/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/*

  Author(s):  Anton Deguet
  Created on: 2008-02-10

  (C) Copyright 2008-2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <iostream>
#include <cisstRevision.h>
#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnLogger.h>


int main (void) {

    std::cout << CISST_FULL_REVISION << std::endl;

#if (CISST_OS == CISST_WINDOWS)
    std::cout << "Running Windows" << std::endl;
#elif (CISST_OS == CISST_LINUX)
    std::cout << "Running Linux" << std::endl;
#elif (CISST_OS == CISST_RTLINUX)
    std::cout << "Running RT-Linux" << std::endl;
#elif (CISST_OS == CISST_IRIX)
    std::cout << "Running SGI Irix" << std::endl;
#elif (CISST_OS == CISST_SOLARIS)
    std::cout << "Running SUN Solaris" << std::endl;
#elif (CISST_OS == CISST_LINUX_RTAI)
    std::cout << "Running RTAI Linux" << std::endl;
#elif (CISST_OS == CISST_CYGWIN)
    std::cout << "Running Cygwin" << std::endl;
#elif (CISST_OS == CISST_DARWIN)
    std::cout << "Running Mac OS X" << std::endl;
#elif (CISST_OS == CISST_QNX)
    std::cout << "Running QNX" << std::endl;
#endif

#if (CISST_COMPILER == CISST_GCC)
    std::cout << "Compiling with gcc" << std::endl;
#elif (CISST_COMPILER == CISST_SGI_CC)
    std::cout << "Compiling with SGI CC" << std::endl;
#elif (CISST_COMPILER == CISST_SUN_CC)
    std::cout << "Compiling with SUN CC" << std::endl;
#elif (CISST_COMPILER == CISST_INTEL_CC)
    std::cout << "Compiling with Intel CC" << std::endl;
#elif (CISST_COMPILER == CISST_DOTNET2003)
    std::cout << "Compiling with Microsoft .net 2003, aka 7.1" << std::endl;
#elif (CISST_COMPILER == CISST_DOTNET2005)
    std::cout << "Compiling with Microsoft .net 2005, aka 8.0" << std::endl;
#elif (CISST_COMPILER == CISST_DOTNET2008)
    std::cout << "Compiling with Microsoft .net 2008, aka 9.0" << std::endl;
#endif

#ifdef CISST_COMPILER_IS_MSVC
    std::cout << "The compiler is a Microsoft compiler" << std::endl;
#else
    std::cout << "The compiler is not a Microsoft compiler" << std::endl;
#endif

    std::cout << "Size of unsigned int: " << sizeof(unsigned int) << std::endl
              << "Size of unsigned long int: " << sizeof(unsigned long int) << std::endl
              << "Size of unsigned long long int: " << sizeof(unsigned long long int) << std::endl
              << "Size of unsigned long: " << sizeof(unsigned long) << std::endl
              << "Size of unsigned long long: " << sizeof(unsigned long long) << std::endl
              << "Size of size_t: " << sizeof(size_t) << std::endl;

    std::cout << "Please type \"0\" followed by the \"return\" key" << std::endl;
    double zero;
    std::cin >> zero;
    double infinite = 1.0 / zero;
    double nan = infinite / infinite;
    if ((!CMN_ISFINITE(infinite)) && (CMN_ISNAN(nan))) {
        std::cout << "Thank you, an infinite double looks like \""
                  << infinite << "\" and a NaN like \""
                  << nan << "\"" << std::endl;
    } else {
        std::cout << "Did you really enter \"0\"?" << std::endl;
    }

    return 0;
}
