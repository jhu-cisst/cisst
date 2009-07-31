/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: svlStereoImageJoiner.h 545 2009-07-14 08:38:33Z adeguet1 $
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2007 

  (C) Copyright 2006-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _svlFilterStereoImageJoiner_h
#define _svlFilterStereoImageJoiner_h

#include <cisstStereoVision/svlStreamManager.h>

// Always include last!
#include <cisstStereoVision/svlExport.h>

class CISST_EXPORT svlFilterStereoImageJoiner : public svlFilterBase
{
public:
    typedef enum _Layout {
        VerticalInterlaced,
        VerticalInterlacedRL,
        SideBySide,
        SideBySideRL
    } Layout;

    svlFilterStereoImageJoiner();
    virtual ~svlFilterStereoImageJoiner();

    int SetLayout(Layout layout);

protected:
    virtual int Initialize(svlSample* inputdata = 0);
    virtual int ProcessFrame(ProcInfo* procInfo, svlSample* inputdata = 0);
    virtual int Release();

private:
    Layout ImageLayout;
};

#endif // _svlFilterStereoImageJoiner_h

