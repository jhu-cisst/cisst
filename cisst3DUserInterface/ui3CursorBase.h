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

#ifndef _ui3CursorBase_h
#define _ui3CursorBase_h


#include <cisstVector/vctTransformationTypes.h>
#include <cisst3DUserInterface/ui3VTKForwardDeclarations.h>
#include <cisst3DUserInterface/ui3ForwardDeclarations.h>
#include <cisst3DUserInterface/ui3VisibleObject.h>


// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

/*!
  Base class for all cursors.
*/
class CISST_EXPORT ui3CursorBase
{
public:
    /*!
      Constructor: needs the UI Manager
    */
    inline ui3CursorBase():
        SceneManager(0)
    {}

    /*! Position stick or line from one corner of the scene to the
      cursor to help identify it */
    enum AnchorType {NONE, TOP_LEFT, TOP_RIGHT, CENTER_LEFT, CENTER_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT};

    /*!
      Destructor
    */
    virtual ~ui3CursorBase() {};

    /*! Retrieve the visible object(s) associated to the cursor */
    virtual ui3VisibleObject * GetVisibleObject(void) = 0;

    /*! Update the cursor position */
    virtual void SetTransformation(vctDoubleFrm3 & frame) = 0;
    
    /*! Tell the cursor to display itself as "pressed" or not */
    virtual void SetPressed(bool pressed) = 0;

    /*! Tell the cursor to display itself as it should when located
      above a 2D surface */
    virtual void Set2D(bool is2D) = 0;
    
    /*! Tell the cursor to display itself as "clutched" or not */
    virtual void SetClutched(bool clutched) = 0;

    /*! Set anchor type */
    inline virtual void SetAnchor(const AnchorType & anchor) {
        this->Anchor = anchor;
    }

    /*! Get anchor type */
    inline virtual void GetAnchor(AnchorType & placeHolder) const {
        placeHolder = this->Anchor;
    }

protected:
    ui3SceneManager * SceneManager;
    AnchorType Anchor;
};


#endif // _ui3CursorBase_h
