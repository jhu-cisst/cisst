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

#ifndef _ui3VideoInterfaceFilter_h
#define _ui3VideoInterfaceFilter_h

#include <cisstStereoVision/svlFilterBase.h>
#include <cisst3DUserInterface/ui3ForwardDeclarations.h>

// Always include last!
#include <cisst3DUserInterface/ui3Export.h>

/*!
 Abstract class that provides the structure for implementing interfaces between
 the 3D renderer and video capture devices.
*/
class CISST_EXPORT ui3VideoInterfaceFilter : public svlFilterBase
{
public:
    /*!
     Constructor
    */
    ui3VideoInterfaceFilter(svlStreamType streamtype, size_t streamid, ui3BehaviorBase* behavior);

    /*!
     Destructor
    */
    virtual ~ui3VideoInterfaceFilter();
    
    unsigned int GetWidth(unsigned int videoch = 0);
    unsigned int GetHeight(unsigned int videoch = 0);

private:
    svlSampleImage* OutputImage;

    size_t StreamID;
    ui3BehaviorBase* ParentBehavior;

    int Initialize(svlSample* syncInput, svlSample* &syncOutput);
    int Process(svlProcInfo* procInfo, svlSample* syncInput, svlSample* &syncOutput);
};

#endif // _ui3VideoInterfaceFilter_h

