// See license at http://www.cisst.org/cisst/license.txt

#ifndef _CMyEventHandler_h
#define _CMyEventHandler_h

// [doc-handler-h-start]
#include <cisstStereoVision/svlWindowManagerBase.h>

class CMyEventHandler : public svlWindowEventHandlerBase
{
public:
    CMyEventHandler();

    void OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid);

private:
    svlRect ROI;
    bool LMouseButtonPressed;
};
// [doc-handler-h-end]


#endif // _CMyEventHandler_h
