// See license at http://www.cisst.org/cisst/license.txt


#include "CMyEventHandler2.h"


CMyEventHandler2::CMyEventHandler2() :
    TextBox(0),
    Path(0),
    LMouseButtonPressed(false)
{
}

void CMyEventHandler2::OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid)
{
    if (!ascii) {
        int x, y;
        GetMousePos(x, y);

        switch (eventid) {
            case winInput_LBUTTONDOWN:
                LMouseButtonPressed = true;
                if (Path) {
                    // Reset path
                    svlOverlayStaticPoly::Type path;
                    Path->SetPoints(path);
                    // Add starting point
                    Path->AddPoint(x, y);
                }
            break;

            case winInput_LBUTTONUP:
                LMouseButtonPressed = false;
            break;

            case winInput_MOUSEMOVE:
                if (LMouseButtonPressed && Path) {
                    Path->AddPoint(x, y);
                }
                if (TextBox) {
                    std::stringstream sstr;
                    sstr << "(" << x << ", " << y << ")";
                    TextBox->SetText(sstr.str());
                    TextBox->SetRect(svlRect(x, y, x + 80, y + 16));
                }
            break;
        }
    }
}

