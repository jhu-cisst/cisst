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

#ifndef _robMesh_h
#define _robMesh_h

#ifdef CISST_ODE_SUPPORT

#include <cisstRobot/robMeshODE.h>
typedef robMeshODE robMesh;

#else

#include <cisstRobot/robMeshTriangular.h>
typedef robMeshTriangular robMesh;

#endif //

#endif // _robMesh_h
