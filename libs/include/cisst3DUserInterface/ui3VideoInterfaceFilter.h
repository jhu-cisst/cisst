/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: ui3VideoSourceBase.h,v 1.4 2009/02/10 15:57:17 anton Exp $

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

#ifndef _ui3VideoInterfaceFilter_h
#define _ui3VideoInterfaceFilter_h

#include <cisstStereoVision.h>

#include <cisst3DUserInterface/ui3ForwardDeclarations.h>

/*!
 Abstract class that provides the structure for implementing interfaces between
 the 3D renderer and video capture devices.
*/
class ui3VideoInterfaceFilter : public svlFilterBase
{
public:
    /*!
     Constructor
    */
    ui3VideoInterfaceFilter(svlStreamType streamtype, int streamid, ui3BehaviorBase* behavior);

    /*!
     Destructor
    */
    virtual ~ui3VideoInterfaceFilter();
    
    unsigned int GetWidth(unsigned int videoch = 0);
    unsigned int GetHeight(unsigned int videoch = 0);

private:
    int StreamID;
    ui3BehaviorBase* ParentBehavior;

    int Initialize(svlSample* inputdata = 0);
    int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0);
};

#endif // _ui3VideoInterfaceFilter_h

