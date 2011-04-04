
#ifndef _robRobotsKinematics_h
#define _robRobotsKinematics_h

#include <cisstVector/vctFrame4x4.h>

vctFrame4x4<double> FKineWAM7( size_t i, double t );
vctFrame4x4<double> FKinePUMA560( size_t i, double t );

#endif
