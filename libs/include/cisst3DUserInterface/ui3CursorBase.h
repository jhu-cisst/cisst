/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#ifndef _ui3CursorBase_h
#define _ui3CursorBase_h


#include <cisstVector/vctTransformationTypes.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VisibleObject.h>


/*!
  Base class for all cursors.
*/
class ui3CursorBase: public ui3VisibleObject
{
public:
    /*!
      Constructor: needs the UI Manager
    */
    inline ui3CursorBase(ui3Manager * manager):
        ui3VisibleObject(manager)
    {}

    /*!
      Destructor
    */
    virtual ~ui3CursorBase() {};

    /*! Create the VTK object used to represent the cursor in the
      scene */
    virtual bool CreateVTKObjects(void) = 0;

    /*! Tell the cursor to display itself as "pressed" or not */
    virtual void SetPressed(bool pressed) = 0;

    /*! Tell the cursor to display itself as it should when located
      above a 2D surface */
    virtual void Set2D(bool is2D) = 0;
    
    /*! Tell the cursor to display itself as "clutched" or not */
    virtual void SetClutched(bool clutched) = 0;
};


#endif // _ui3CursorBase_h
