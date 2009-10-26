#ifndef _robBody_h
#define _robBodye_h

#ifdef __ODE__
#include <cisstRobot/robBodyODE.h>
typedef robBodyODE robBody;

#else
#include <cisstRobot/robBodyBase.h>
typedef robBodyBase robBody;

#endif

#endif
