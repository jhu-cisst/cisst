/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2017-11-30

  (C) Copyright 2017 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _vctForceTorque3DQtWidget_h
#define _vctForceTorque3DQtWidget_h

// cisst include
#include <cisstVector/vctForwardDeclarations.h>
#include <cisstVector/vctQtForwardDeclarations.h>
#include <cisstVector/vctFixedSizeVectorTypes.h>

// Always include last
#include <cisstVector/vctExportQt.h>


/*!
  Widget to visualize force axis using 3 axes in OpenGL
*/
class CISST_EXPORT vctForceTorque3DQtWidget: public vctQtOpenGLBaseWidget
{
    Q_OBJECT;

public:
    vctForceTorque3DQtWidget(void);
    inline ~vctForceTorque3DQtWidget(void) {};

    void SetValue(const vct3 & force, const vct3 & torque);

protected:
   void initializeGL(void);
   void paintGL(void);
   void resizeGL(int width, int height);
   void draw3DAxis(const float scale);

   vct3 mForce;
   vct3 mTorque;
};

#endif // _vctForceTorque3DQtWidget_h
