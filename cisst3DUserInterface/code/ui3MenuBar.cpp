/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-06-10

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cisst3DUserInterface/ui3MenuBar.h>
#include <cisst3DUserInterface/ui3SceneManager.h>

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>


CMN_IMPLEMENT_SERVICES(ui3MenuBar);


const unsigned int MaxButtons = 5;


ui3MenuBar::ui3MenuBar(const std::string & name):
    ui3VisibleObject(name + "MenuBar"),
    ShowState(false),
    ButtonWidth(10.0),
    ButtonHeight(10.0),
    Background(0),
    Mapper(0),
    Actor(0),
    MenuInitialDepth(-1000.0) // bad, hard coded!  Anton
{
    // where does the menu go on the screen    bad, hard coded view port size 1000x1000
    MenuBottom2D = -0.95 * (500.0 / 2.0); // 5% above bottom
    MenuTop2D = MenuBottom2D + 0.08 * static_cast<double>(500.0);
 
    // size of buttons and offsets, in button_coordinates, values are button centers.
    ButtonSpacing2D = 0.5; // at least half a button between buttons
    double buttonWidth2D = 500.0 / (MaxButtons + ((MaxButtons + 1) * ButtonSpacing2D));
    double buttonHeight2D = 0.9 * (MenuTop2D - MenuBottom2D);
    ButtonSize2D = buttonWidth2D > buttonHeight2D ? buttonHeight2D : buttonWidth2D;
    ButtonSpacing2D = 0.5 * ButtonSize2D; // now set it to exactly half a button

}


ui3MenuBar::~ui3MenuBar()
{
}


bool ui3MenuBar::CreateVTKObjects(void)
{ 
    // assemble all 3D objects at depth 0.0
    this->Background = vtkCubeSource::New();
    CMN_ASSERT(this->Background);
    this->Background->SetXLength(500.0);  // hard coded, bad, width of fov
    this->Background->SetYLength(MenuTop2D - MenuBottom2D);
    this->Background->SetZLength(0.01); // very thin
    // save bounding box info for later tests
    this->BottomLeft2D.X() = -250.0; // 500 /2
    this->BottomLeft2D.Y() = -(this->MenuTop2D - this->MenuBottom2D) / 2.0;
    this->TopRight2D.X() = +250.0; // 500 /2
    this->TopRight2D.Y() = (this->MenuTop2D - this->MenuBottom2D) / 2.0;

    this->Mapper = vtkPolyDataMapper::New();
    CMN_ASSERT(this->Mapper);
    this->Mapper->SetInputConnection(this->Background->GetOutputPort());
    this->Mapper->ImmediateModeRenderingOn();

    this->Actor = vtkActor::New();
    CMN_ASSERT(this->Actor);
    this->Actor->SetMapper(this->Mapper);

    this->SetVisible(false);
    this->Actor->GetProperty()->SetColor(0.5, 0.5, 0.5);
    this->Actor->GetProperty()->SetOpacity(0.1);

    this->AddPart(this->Actor);

    // 3D position of menu assembly
    MenuCenter3D.X() = 0.0;
    MenuCenter3D.Y() = (MenuBottom2D + MenuTop2D) / 2.0;
    MenuCenter3D.Z() = this->MenuInitialDepth;
    this->SetPosition(MenuCenter3D);

    // add all buttons
    ButtonContainerType::iterator iterator;
    const ButtonContainerType::iterator end = this->Buttons.end();
   
    for (iterator = this->Buttons.begin();
         iterator != end;
         iterator++)
    {
        this->AddPart((*iterator).second->GetVTKProp());
    }

    // space all buttons to fill the menu bar
    this->SpreadButtons();

    // hide by default
    this->Hide();

    return true;
}


void ui3MenuBar::SetAllButtonsUnselected(void)
{
    ButtonContainerType::iterator iterator;
    const ButtonContainerType::iterator end = this->Buttons.end();
    for (iterator = this->Buttons.begin();
         iterator != end;
         iterator++) {
        (*iterator).second->SetHighlight(false);
    }
}


 
bool ui3MenuBar::IsPointOnMenuBar(const vctDouble3 & cursor3D, ButtonPointer & button)
{
    button = 0;
    bool isOverMenu = false;
    vctDouble2 cursor2D;
    // project 3d position on menu based on depth
    double ratio = this->MenuInitialDepth / cursor3D.Z();
    cursor2D.X() = cursor3D.X() * ratio; 
    cursor2D.Y() = cursor3D.Y() * ratio - (this->MenuTop2D + this->MenuBottom2D) / 2.0;
    // check if this value not is within the background
    if (cursor2D.GreaterOrEqual(BottomLeft2D) && cursor2D.LesserOrEqual(TopRight2D)) {
        isOverMenu = true;
        // check if the cursor is over this button
        ButtonContainerType::iterator iterator;
        const ButtonContainerType::iterator end = this->Buttons.end();
        for (iterator = this->Buttons.begin();
             iterator != end;
             iterator++) {
            if ((*iterator).second->IsCursorOver(cursor2D)) {
                (*iterator).second->SetHighlight(true);
                button = (*iterator).second;
            }
        }
    }
    return isOverMenu;
}


bool ui3MenuBar::AddCheckButton(const std::string& description, int position, const std::string& iconFile, bool state)
{
    return 0;
}


bool ui3MenuBar::AddSpacer(int position)
{
    return 0;
}


void ui3MenuBar::SetDepth(double depth)
{
  double scaleRatio = depth / this->MenuInitialDepth;
  this->MenuCenter3D.Y() = ((this->MenuBottom2D + this->MenuTop2D) / 2.0) * scaleRatio;
  this->MenuCenter3D.Z() = depth;
  this->SetPosition(this->MenuCenter3D);
  this->SetScale(scaleRatio); 
}


void ui3MenuBar::SpreadButtons(void)
{
    // Update the button coordinates.
    ButtonContainerType::iterator iterator;
    const ButtonContainerType::iterator end = this->Buttons.end();
   
    vct3 buttonCoordinates;
    unsigned int i = 0;
    const double buttonCount = static_cast<double>(this->Buttons.size());
    double firstButton = -((buttonCount - 1.0) * ButtonSize2D + (buttonCount - 2.0) * ButtonSpacing2D) / 2.0;

    for (iterator = this->Buttons.begin();
         iterator != end;
         iterator++)
    {
      buttonCoordinates.X() = firstButton + i * (ButtonSize2D + ButtonSpacing2D); // left to right
      buttonCoordinates.Y() = 0.0; // wrt the menu, center in y (heigth)
      buttonCoordinates.Z() = 0.0; // wrt the menu, center in z (same depth)

      (*iterator).second->SetPosition(buttonCoordinates);
      i++;
    }
}

