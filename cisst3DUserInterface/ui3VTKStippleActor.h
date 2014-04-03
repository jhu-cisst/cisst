/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Caitlin Schneider
  Created on: 2008

  (C) Copyright 2008-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _ui3VTKStippleActor_h
#define _ui3VTKStippleActor_h

#include <vtkOpenGLActor.h>

class vtkOpenGLRenderer;

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

class CISST_EXPORT ui3VTKStippleActor : public vtkOpenGLActor
{
 public:
    static ui3VTKStippleActor * New(void);
    virtual void PrintSelf(ostream & os, vtkIndent indent);

    // Description:
    // Actual actor render method.
    void Render(vtkRenderer * renderer, vtkMapper * mapper);
    void SetStipplePercentage(int percentage);

 protected:
    ui3VTKStippleActor();
    ~ui3VTKStippleActor();

    int StipplePercentage;

    bool Debug;
};

#endif // _ui3VT


