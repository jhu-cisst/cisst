/*

  Author(s): Simon Leonard
  Created on: Nov 11 2009

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _robGeom_h
#define _robGeom_h

#ifdef CISST_ODE_SUPPORT

#include <cisstRobot/ode/robMeshTriangularODE.h>
typedef robMeshTriangularODE robGeom;

#else

#include <cisstRobot/robMeshTriangular.h>
typedef robMeshTriangular robGeom;

#endif // __ODE__

#endif
