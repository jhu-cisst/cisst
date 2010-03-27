/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlFilterCallback.h 1218 2010-02-22 16:08:09Z adeguet1 $

  Author(s):	Daniel Mirota
  Created on:	2010-03-26

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _svlFilterCallback_h
#define _svlFilterCallback_h

#include <cisstStereoVision.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

/*!
 Abstract class that provides the structure for implementing interfaces between
 the video capture devices and other components which implement the callback
*/

//template <class _ValueType>
typedef int (*CALLBACK_FUNC)(void * imageData, void * callbackData);


class CISST_EXPORT svlFilterCallback : public svlFilterBase
{
public:
    /*!
     Constructor
    */
    svlFilterCallback();

    /*!
     Destructor
    */
    virtual ~svlFilterCallback();
    
    unsigned int GetWidth(unsigned int videoch = 0);
    unsigned int GetHeight(unsigned int videoch = 0);

	int SetCallback(CALLBACK_FUNC callback_in,void * callbackData_in);

private:
    //int StreamID;
    //ui3BehaviorBase* ParentBehavior;
	CALLBACK_FUNC myCallback;
	void * myCallbackData;

    int Initialize(svlSample* inputdata);
    int ProcessFrame(svlProcInfo* procInfo, svlSample* inputdata);
};

#endif // _svlFilterCallback_h

