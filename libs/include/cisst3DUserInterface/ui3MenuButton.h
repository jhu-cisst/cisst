/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#ifndef _ui3MenuButton_h
#define _ui3MenuButton_h


#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>

#include <cisstMultiTask/mtsFunctionVoid.h>


/*!
 Typedef for pointer to member function for button callbacks.
*/
//typedef void(_classType::*ActionType)(void);
//typedef void* ui3ControlCallbackType;

/*!
 Implements a button on the menu bar. For creating a button spacer, do not call
 the Create function.
*/
class ui3MenuButton
{
    friend class ui3Manager;

public:
    /*!
     Constructor
    */
    template <class _classType>
    ui3MenuButton(const std::string & description, const std::string & iconFile,
                  void (_classType::*action)(void), _classType * classInstantiation,
                  double buttonSize):
        CheckButton(false),
        Position(0.0, 0.0, 0.0),
        CheckState(false),
        PushState(false),
        HighlightState(false),
        EnableState(false),
        Handle(0),
        ActorHandle(0),
        BMPReader(0),
        Texture(0),
        PlaneSource(0),
        Mapper(0),
        Actor(0),
        ButtonSize(buttonSize),
		IconFile(iconFile)
    {
        // Create a mtsFunction around the callback command
        this->Command = new mtsCommandVoidMethod<_classType>(action, classInstantiation, description);
        this->CallBack.Bind(this->Command);
		this->CreateVTKObjects();
    }

    /*!
     Destructor
    */
    virtual ~ui3MenuButton();

    /*!
     Creates the button and the corresponding VTK actor.
     The 3D model of the button is initialized to unit size. The menu bar
     object will take care of the proper resizing of the model.

     \param check           Set whether check button: true=check button, false=click button
     \param iconfile        Image file storing all the button states on a singe bitmap.
                            If empty string, no texture will be added.
     \return                Button handle, 0=error
    */
    bool CreateVTKObjects(void);

    vtkProp3D * GetVTKProp(void);

    /*!
     Returns the control handle of the button.

     \return                Control handle
    */
    virtual ui3Handle GetHandle();

    /*!
     Returns the handle of the VTK actor created for the button.

     \return                VTK actor handle
    */
    virtual ui3Handle GetActorHandle();

    /*!
     Sets the checked state of the check button.
     Have no effect on non-check buttons.

     \param state           Set check state: true=checked, false=unchecked
    */
    virtual void SetCheck(bool state);

    /*!
     Sets the pushed state of the button.

     \param state           Set pushed state: true=down, false=up
    */
    virtual void SetPush(bool state);

    /*!
     Sets the highlighted state of the button.

     \param state           Set highlight state: true=highlighted, false=default
    */
    virtual void SetHighlight(bool state);

    /*!
     Sets the enabled state of the button.

     \param state           Set enable state: true=enabled, false=disabled
    */
    virtual void SetEnable(bool state);

    /*!
     Returns the checked state of the check button.
     Always returns false on non-check buttons.

     \return                Checked state: true=checked, false=unchecked
    */
    virtual bool GetCheck();

    /*!
     Returns the pushed state of the button.

     \return                Pushed state: true=down, false=up
    */
    virtual bool GetPush();

    /*!
     Returns the highlighted state of the button.

     \return                Highlighted state: true=highlighted, false=default
    */
    virtual bool GetHighlight();

    /*!
     Returns the enabled state of the button.

     \return                Enabled state: true=enabled, false=disabled
    */
    virtual bool GetEnable();

    virtual vtkActor * VTKPointer(void);

    virtual void SetPosition(vct3 & position);

    virtual bool IsCursorOver(const vct2 & cursor2D);

protected:
    /*!
     Flag indicates whether the button is a check button.
    */
    bool CheckButton;
    /*!
     Vector specifies the 3D Cartesian position of the button.
    */
    vct3 Position;
    /*!
     Flag indicates check state: true=checked, false=unchecked.
    */
    bool CheckState;
    /*!
     Flag indicates push state: true=down, false=up.
    */
    bool PushState;
    /*!
     Flag indicates highlight state: true=highlighted, false=default.
    */
    bool HighlightState;
    /*!
     Flag indicates enable state: true=enabled, false=disabled.
    */
    bool EnableState;
    /*!
     Control handle of the button.
    */
    ui3Handle Handle;
    /*!
     Handle of the VTK actor created for the button.
    */
    ui3Handle ActorHandle;

    vtkBMPReader * BMPReader;
    vtkTexture * Texture;
    vtkPlaneSource * PlaneSource;
    vtkPolyDataMapper * Mapper;
    vtkActor * Actor;

    // corner of buttons in menu
    vct2 BottomLeft2D, TopRight2D;
    double ButtonSize;

    mtsFunctionVoid CallBack;
    mtsCommandVoidBase * Command;

	std::string IconFile;
};


#endif // _ui3MenuButton_h

