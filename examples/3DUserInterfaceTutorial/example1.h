/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: example1.h,v 1.6 2009/02/23 16:55:06 anton Exp $

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


#include <cisst3DUserInterface.h>

class CExampleBehavior : public ui3BehaviorBase
{
public:
    CExampleBehavior(const std::string & name);
    ~CExampleBehavior();

    void Startup(void);
    void Cleanup(void);
    void ConfigureMenuBar(void);
    bool RunForeground(void);
    bool RunBackground(void);
    bool RunNoInput(void);
    void Configure(const std::string & configFile);
    bool SaveConfiguration(const std::string & configFile);
    ui3VisibleObject * GetVisibleObject(void) { return 0; }

protected:
    void FirstButtonCallback(void);
    void OnButton2(void);
    void OnButton3(void);

    void OnInputAction(unsigned int inputid, ui3InputDeviceBase::InputAction action);

private:
    ui3Handle hButton1;
    ui3Handle hButton2;
    ui3Handle hButton3;
    ui3Handle hSpacer;

    ui3SceneManager::VTKHandleType h3DModel;
};


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  $Log: example1.h,v $
//  Revision 1.6  2009/02/23 16:55:06  anton
//  Work in progress to support VTK scene locks.  Compiles, doesn't run.
//
//  Revision 1.5  2009/02/17 22:00:10  anton
//  Moved more code to base class ui3VisibleObject
//
//  Revision 1.4  2009/02/13 22:35:12  anton
//  Preliminary code for scene manager
//
//  Revision 1.3  2009/02/02 23:21:42  anton
//  Work in progress, work on connecting master arm to ui3Manager
//
//  Revision 1.2  2008/08/19 20:05:09  anton
//  cisst3DUserInterface: Port to cisstMultiTask, compiles, does not run!
//
//  Revision 1.1  2008/06/18 22:54:49  vagvoba
//  cisst3DUserInterface: example1 added (CMakeLists.txt, example1.h, example1.cpp)
//
//
// ****************************************************************************
