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


#include <cisst3DUserInterface/ui3VisibleList.h>

#include "cs3DObjectViewer.h"

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkOutlineSource.h>

CMN_IMPLEMENT_SERVICES(cs3DObjectViewer);


class Widget: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
    public:
    inline Widget(const std::string & name = "3DWidgetAttempt"):
        ui3VisibleObject(name),
        SphereSource(0),
        SphereMapper(0),
        OutlineSource(0),
        OutlineMapper(0),
        OutlineActor(0)
//        Position(position)
        {}

    inline bool CreateVTKObjects(void) {

        this->SphereSource = vtkSphereSource::New();
        CMN_ASSERT(this->SphereSource);
        this->SphereSource->SetRadius(3.0);

        this->SphereMapper = vtkPolyDataMapper::New();
        CMN_ASSERT(this->SphereMapper);
        this->SphereMapper->SetInputConnection(this->SphereSource->GetOutputPort());
        this->SphereMapper->ImmediateModeRenderingOn();

        //create the top control point
        this->sphere1 = vtkActor::New();
        CMN_ASSERT(this->sphere1);
        this->sphere1->SetMapper(this->SphereMapper);
        this->sphere1->GetProperty()->SetColor(1.0, 0.0, 0.0);
        this->sphere1->SetPosition(20.0, 0.0, 0.0);
        
        //create the bottom control point
        this->sphere2 = vtkActor::New();
        CMN_ASSERT(this->sphere2);
        this->sphere2->SetMapper(this->SphereMapper);
        this->sphere2->GetProperty()->SetColor(0.0, 1.0, 0.0);
        this->sphere2->SetPosition(-20.0, 0.0, 0.0);
        
        //create the right ctl point
        this->sphere3 = vtkActor::New();
        CMN_ASSERT(this->sphere3);
        this->sphere3->SetMapper(this->SphereMapper);
        this->sphere3->GetProperty()->SetColor(0.0, 0.0, 1.0);
        this->sphere3->SetPosition(0.0, 20.0, 0.0);
        
        //create the left ctl point
        this->sphere4 = vtkActor::New();
        CMN_ASSERT(this->sphere4);
        this->sphere4->SetMapper(this->SphereMapper);
        this->sphere4->GetProperty()->SetColor(1.0, 1.0, 1.0);
        this->sphere4->SetPosition(0.0, -20.0, 0.0);

        this->AddPart(this->sphere1);
        this->AddPart(this->sphere2);
        this->AddPart(this->sphere3);
        this->AddPart(this->sphere4);

        this->OutlineSource= vtkOutlineSource::New();
        this->OutlineSource->SetBounds(-15,15,-15,15,-15,15);

        this->OutlineMapper = vtkPolyDataMapper::New();
        this->OutlineMapper->SetInputConnection(OutlineSource->GetOutputPort());
        this->OutlineMapper->ImmediateModeRenderingOn();

        this->OutlineActor = vtkActor::New();
        this->OutlineActor->SetMapper(this->OutlineMapper);
        this->OutlineActor->GetProperty()->SetColor(1,1,1);
        this->OutlineActor->SetPosition(0.0,0.0,0.0);

        this->AddPart(this->OutlineActor);

//        this->SetTransformation(this->Position);

        return true;
        }



    protected:

        vtkSphereSource * SphereSource;
        vtkPolyDataMapper * SphereMapper;
        vtkActor * sphere1, *sphere2, *sphere3, *sphere4;
        vtkOutlineSource * OutlineSource;
        vtkPolyDataMapper * OutlineMapper;
        vtkActor * OutlineActor;

//         vctFrm3 Position; // initial position
};


CMN_DECLARE_SERVICES_INSTANTIATION(Widget);


//constructor
cs3DObjectViewer::cs3DObjectViewer(void):
        VisibleList(0)
{
    this->WidgetObject = new Widget();
    this->CreateControlPoints();

}

//destructor
cs3DObjectViewer::~cs3DObjectViewer()
{

}

ui3VisibleObject * cs3DObjectViewer::GetVisibleObject(void)
{
    return this->VisibleList;
}

void cs3DObjectViewer::CreateControlPoints(void)
{
    vctFrm3 ctl1, ctl2, ctl3, ctl4;
    ctl1.Translation().Assign(20.0, 0.0, 0.0);
    ctl2.Translation().Assign(-20.0, 0.0, 0.0);
    ctl3.Translation().Assign(0.0, 20.0, 0.0);
    ctl4.Translation().Assign(0.0, -20.0, 0.0);
    Rel_ControlPoints.push_back(ctl1);
    Rel_ControlPoints.push_back(ctl2);
    Rel_ControlPoints.push_back(ctl3);
    Rel_ControlPoints.push_back(ctl4);
}


std::list<vctFrm3> cs3DObjectViewer::GetRelativeControlPoints(void)
{
    return Rel_ControlPoints;
}

//this is not actually done, should create a list of absolute control point positions
std::list<vctFrm3> cs3DObjectViewer::GetSceneControlPoints(void)
{
    std::list<vctFrm3> absolControlPoints;
    //absol = this->GetTransformation * Rel_ControlPoints
    return absolControlPoints;
}

int cs3DObjectViewer::FindClosestControlPoint(vctFrm3 CursorPos)
{
    int counter = 0, toReturn = -1;
    vctFrm3 absoluteControlPoint;
    double closestDist = cmnTypeTraits<double>::MaxPositiveValue();

    Frames = this->GetSceneControlPoints();

    ListFrameType::iterator iter;
    const ListFrameType::iterator end = Frames.end();

    for(iter = Frames.begin(); iter != end; ++iter)
    {
        absoluteControlPoint = VisibleList->GetTransformation() * (*iter);
        vctDouble3 diff;
        diff.DifferenceOf(CursorPos.Translation(), absoluteControlPoint.Translation() );
        
        double absolDiff = diff.Norm();
        if(absolDiff < closestDist)
        {
            toReturn = counter;
            closestDist = absolDiff;
        }
        counter++;
    }
    
    return toReturn;
}

