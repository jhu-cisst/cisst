/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009 

  (C) Copyright 2006-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlVidCapSrcDirectShowInputSelector_h
#define _svlVidCapSrcDirectShowInputSelector_h

#include <iostream>
#include <list>
#include "dshow.h"


class svlVidCapSrcDirectShowInputSelector
{
    typedef struct tagRouting {
        tagRouting *LeftRouting;
        tagRouting *RightRouting;
        LONG VideoInputIndex;
        LONG VideoOutputIndex;
        IAMCrossbar *Xbar;
        LONG InputPhysicalType;
        LONG OutputPhysicalType;
        int Depth;
    } _Routing;

public:
    svlVidCapSrcDirectShowInputSelector(IPin *pin);
    ~svlVidCapSrcDirectShowInputSelector();

    int GetInputCount();
    int GetInputType(int index);
    bool GetInputName(int index, std::string & name);
    bool SetInputIndex(int index);
    int GetInputIndex();

private:
    IPin *StartingPin;
    _Routing RoutingRoot;
    std::list<_Routing*> RoutingList;
    int CurrentRoutingIndex;

    bool BuildRoutingList(IPin *startinginputpin, _Routing *routing, int depth);
    bool DestroyRoutingList();
    bool GetCrossbarIPinAtIndex(IAMCrossbar *xbar, int pinindex, bool isinputpin, IPin **pin);
    bool GetCrossbarIndexFromIPin(IAMCrossbar *xbar, LONG *pinindex, bool isinputpin, IPin *pin);
};

#endif  // _svlVidCapSrcDirectShowInputSelector_h
