// See license at http://www.cisst.org/cisst/license.txt


#include "CMyEventHandler.h"


void CMyEventHandler::OnUserEvent(unsigned int winid, bool ascii, unsigned int eventid)
{
    if (ascii) {                                                        // Event is an ASCII character
        std::cout << static_cast<char>(eventid) << std::flush;          // Write out character
    }
    else {                                                              // Event is not an ASCII character:
                                                                        //   mouse event or special key
        switch (eventid) {
            case winInput_LBUTTONDOWN:                                  // Handle mouse button events
            case winInput_LBUTTONUP:                                    //
            case winInput_RBUTTONDOWN:                                  //
            case winInput_RBUTTONUP:                                    //
                std::cout << "Mouse button event" << std::endl;         //
            break;                                                      //

            case winInput_MOUSEMOVE:                                    // Handle mouse move and cursor position
            {                                                           //
                int x, y;                                               //
                GetMousePos(x, y);                                      //
                std::cerr << "(" << x << ", " << y << ")" << std::endl; //
            }                                                           //
            break;                                                      //

            case winInput_KEY_F1:                                       // Handle special keys
            case winInput_KEY_F2:                                       //
            case winInput_KEY_F3:                                       //
            case winInput_KEY_F4:                                       //
            case winInput_KEY_F5:                                       //
            case winInput_KEY_F6:                                       //
            case winInput_KEY_F7:                                       //
            case winInput_KEY_F8:                                       //
            case winInput_KEY_F9:                                       //
            case winInput_KEY_F10:                                      //
            case winInput_KEY_F11:                                      //
            case winInput_KEY_F12:                                      //
            case winInput_KEY_PAGEUP:                                   //
            case winInput_KEY_PAGEDOWN:                                 //
            case winInput_KEY_HOME:                                     //
            case winInput_KEY_END:                                      //
            case winInput_KEY_INSERT:                                   //
            case winInput_KEY_DELETE:                                   //
            case winInput_KEY_LEFT:                                     //
            case winInput_KEY_RIGHT:                                    //
            case winInput_KEY_UP:                                       //
            case winInput_KEY_DOWN:                                     //
                std::cout << "Special key down" << std::endl;           //
            break;                                                      //
        }
    }
}

