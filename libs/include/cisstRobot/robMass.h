#ifndef _robMass_h
#define _robMass_h

#ifdef __ODE__

#include <cisstRobot/robMassODE.h>
typedef robMassODE robMass;

#else

#include <cisstRobot/robMassGeneric.h>
typedef robMassGeneric robMass;

#endif // __ODE__

#endif // _robMass_h
