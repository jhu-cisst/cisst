// See license at http://www.cisst.org/cisst/license.txt

#ifndef _CMyEventHandler2_h
#define _CMyEventHandler2_h

// [doc-handler-h-start]
#include <cisstStereoVision/svlWindowManagerBase.h>
#include <cisstStereoVision/svlFilterImageOverlay.h>

class CMyEventHandler2 : public svlWindowEventHandlerBase
{
public:
    CMyEventHandler2();

    void OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid);

    svlOverlayStaticText* TextBox;
    svlOverlayStaticPoly* Path;
    bool LMouseButtonPressed;
};
// [doc-handler-h-end]

#endif // _CMyEventHandler2_h
