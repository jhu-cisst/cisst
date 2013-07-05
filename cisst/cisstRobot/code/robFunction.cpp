/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Simon Leonard
  Created on: 2009-11-11

  (C) Copyright 2009-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstRobot/robFunction.h>
#include <cisstCommon/cmnLogger.h>

robFunction::robFunction( void ):
    t1( 0.0 ),
    t2 (0.0 )
{}

robFunction::robFunction( double startTime, double stopTime ){
    Set( startTime, stopTime);
}

void robFunction::Set( double startTime, double stopTime ){

    t1 = startTime;
    t2 = stopTime;

    if( t2 < t1 ){
        CMN_LOG_RUN_ERROR << CMN_LOG_DETAILS
                          << ": t2 = " << t2 
                          << " is less than or equal to t1 = " << t1
                          << std::endl;
    
    }
}

double& robFunction::StartTime( void ) { return t1; }
double& robFunction::StopTime( void ) { return t2; }
double robFunction::Duration( void ) const{ return t2-t1; }
