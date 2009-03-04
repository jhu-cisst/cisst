/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3ForwardDeclarations.h,v 1.6 2009/02/16 22:36:59 anton Exp $

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

#ifndef _ui3ForwardDeclarations_h
#define _ui3ForwardDeclarations_h


#include <cisstVector/vctFixedSizeVectorTypes.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaMutex.h>

#include <map>
#include <list>
#include <string>

#define INPUT_CLICK             0
#define INPUT_DBCLICK           1
#define INPUT_PRESSED           2
#define INPUT_RELEASED          3
#define INPUT_2NDCLICK          4
#define INPUT_2NDDBCLICK        5
#define INPUT_2NDPRESSED        6
#define INPUT_2NDRELEASED       7

class ui3UIThread;
class ui3Manager;
class ui3BehaviorBase;
class ui3SceneManager;
class ui3InputDeviceBase;
class ui3VideoInterfaceFilter;
class ui3VTKRenderer;
class ui3VisibleObject;
class ui3MenuBar;
class ui3MenuButton;
class ui3Cursor;

/*!
 Typedef for control handles to identify SAW objects.
*/
typedef unsigned int ui3Handle;
/*!
 Static variable to create unique handles.
*/
static ui3Handle HandleCounter = 1;
/*!
 Creates a new handle unique for the application.
 
 \return                    Newly created unique handle
*/
static ui3Handle GetUniqueHandle() { return HandleCounter++; }


#endif // _ui3ForwardDeclarations_h

