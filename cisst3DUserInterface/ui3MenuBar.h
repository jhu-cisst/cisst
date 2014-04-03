/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):	Balazs Vagvolgyi, Simon DiMaio, Anton Deguet
  Created on:	2008-05-23

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef ui3MenuBar_h
#define ui3MenuBar_h


#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VisibleObject.h>
#include <cisst3DUserInterface/ui3MenuButton.h>

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

/*!
 Implements a menu bar to show buttons on the lower part of the image.
*/
class CISST_EXPORT ui3MenuBar: public ui3VisibleObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    /*!
     Constructor
    */
    ui3MenuBar(const std::string & name);

    /*!
     Destructor
    */
    ~ui3MenuBar();

    /*!
     Checks whether the specified point is visible inside the menu bar area.

     \param point           Point coordinates [in millimeters]
    */
    typedef ui3MenuButton * ButtonPointer;
    bool IsPointOnMenuBar(const vct3 & point, ButtonPointer & button);

    /*!
     Adds a click button to the menu bar. The icon file has to contain images for
     all button states on a single bitmap.

     \param description     Textual description, tool tip
     \param position        Defines the position of button in the button list.
                            If positive, the button will be inserted to the specified position on the menu bar.
                            If -1, the button will be added to the end of the menu bar.
     \param iconfile        Image file storing all the button states on a singe bitmap.
                            If empty string, no texture will be added.
     \return                Handle assigned to the button, unique for the menu bar
    */
    template <class _classType>
    inline bool AddClickButton(const std::string& description, int position,
                               const std::string& iconFile,
                               void (_classType::*action)(void),
                               _classType * classInstantiation) {
        // create a button
        ui3MenuButton * buttonPointer = 0;
        buttonPointer = new ui3MenuButton(description, iconFile, action, classInstantiation, this->ButtonSize2D);
        CMN_ASSERT(buttonPointer);
        this->Buttons.insert(std::pair<unsigned int, ui3MenuButton *>(position, buttonPointer));
        return true; // to fix, Anton
    }

    /*!
     Adds a check button to the menu bar. The icon file has to contain images for
     all button states on a single bitmap.

     \param description     Textual description, tool tip
     \param position        Defines the position of button in the button list.
                            If positive, the button will be inserted to the specified position on the menu bar.
                            If -1, the button will be added to the end of the menu bar.
     \param iconfile        Image file storing all the button states on a singe bitmap.
                            If empty string, no texture will be added.
     \param state           Initial state: true=checked, false=unchecked
     \return                Identifier assigned to the button, unique for the menu bar
    */
    bool AddCheckButton(const std::string& description, int position, const std::string& iconfile, bool state);

    /*!
     Adds an empty button space to the menu bar.

     \param position        Defines the position of spacer in the button list.
                            If positive, the spacer will be inserted to the specified position on the menu bar.
                            If -1, the spacer will be added to the end of the menu bar.
     \return                Identifier assigned to the spacer, unique for the menu bar
    */
    bool AddSpacer(int position);

    bool CreateVTKObjects(void);
    
    inline bool UpdateVTKObjects(void) {
        return true;
    }

    void SetAllButtonsUnselected(void);

    void SetDepth(double depth);

protected:

    typedef std::multimap<unsigned int, ui3MenuButton *> ButtonContainerType;

    /*! Called by the AddButton methods to make sure buttons are spread evenly */
    void SpreadButtons(void);

    /*!
     Flag indicates whether the menu bar is rendered: true=shown, false=hidden.
    */
    bool ShowState;
    /*!
     Specifies the width of individual buttons on the menu bar.
    */
    double ButtonWidth;
    /*!
     SSpecifies the height of individual buttons on the menu bar.
    */
    double ButtonHeight;
    /*!
     Linked list of buttons in the menu bar.
    */
    ButtonContainerType Buttons;
    
    /*!
     Stores the handle of VTK actors for all the menu bar elements, including the menu
     bar and the buttons.
    */
    vtkCubeSource * Background;
    vtkPolyDataMapper * Mapper;
    vtkActor * Actor;

    double MenuInitialDepth;
    double MenuBottom2D;
    double MenuTop2D;
    double ButtonSpacing2D;
    double ButtonSize2D;

    vct3 MenuCenter3D;
    vct2 BottomLeft2D, TopRight2D;
};


CMN_DECLARE_SERVICES_INSTANTIATION(ui3MenuBar);


#endif // ui3MenuBar_h

