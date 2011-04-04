// See license at http://www.cisst.org/cisst/license.txt


#ifndef _CMyEventHandler_h
#define _CMyEventHandler_h

#include <cisstStereoVision/svlFilterImageWindow.h>


class CMyEventHandler : public svlWindowEventHandlerBase
{
public:
    void OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid);
};


#endif // _CMyEventHandler_h

