/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Anton Deguet
  Created on:	2009-02-04

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/



#include <cisstVector/vctTransformationTypes.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>
#include <cisst3DUserInterface.h>
// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

class Widget;

/*!  Simple cursor using a sphere for rendering.  Color, diameter and
  transparency are used to show the different states.
 */
class CISST_EXPORT cs3DObjectViewer:  public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    protected:

    public:
    /*!
        Constructor: called when instantiating behaviors
     */
        cs3DObjectViewer(void);

    /*!
        Destructor
     */
        ~cs3DObjectViewer();

        ui3VisibleObject * GetVisibleObject(void);

        void CreateControlPoints(void);
        std::list<vctFrm3> GetRelativeControlPoints(void);
        std::list<vctFrm3> GetSceneControlPoints(void);
        int FindClosestControlPoint(vctFrm3 CursorPos);
        
        typedef std::list<vctFrm3> ListFrameType;
        ListFrameType Frames;

    protected:
        ui3VisibleList * VisibleList;
        Widget * WidgetObject;
        std::list<vctFrm3> Rel_ControlPoints;

};


CMN_DECLARE_SERVICES_INSTANTIATION(cs3DObjectViewer);


