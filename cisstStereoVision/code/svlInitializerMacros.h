/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
 
 Author(s):  Peter Kazanzides
 Created on: 2011
 
 (C) Copyright 201 Johns Hopkins University (JHU), All Rights Reserved.
 
 --- begin cisst license - do not edit ---
 
 This software is provided "as is" under an open source license, with
 no warranty.  The complete license can be found in license.txt and
 http://www.cisst.org/cisst/license.txt.
 
 --- end cisst license ---
 
 */

#ifndef _svlInitializerMacros_h
#define _svlInitializerMacros_h

// The "delete new" construct is currently used to ensure that the SVL components
// are added to the class register. Other approaches may be more efficient, such as:
//     (A *)0
//     A::ClassServices()->GetName()
//#define SVL_INITIALIZE(A) delete new A
#define SVL_INITIALIZE(A) A::ClassServices()

#endif // _svlInitializerMacros_h

