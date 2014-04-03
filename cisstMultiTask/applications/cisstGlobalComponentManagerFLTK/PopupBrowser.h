/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Min Yang Jung
  Created on: 2010-03-18

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _popupBrowser_h
#define _popupBrowser_h

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Menu.H>

#include <string>

extern void callbackVisualize(Fl_Widget*, void *userdata);

class PopupBrowser : public Fl_Browser 
{
public:
    PopupBrowser(int X,int Y,int W,int H,const char*L=0):Fl_Browser(X,Y,W,H,L) 
    {}

    int handle(int e) 
    {
        switch (e) {
            case FL_PUSH:
                // If mouse right button is clicked, popup menu on right click
                if (Fl::event_button() == FL_RIGHT_MOUSE) {
                    // Check if there is any item that a user selected
                    if (this->value() == 0) {
                        return 1;
                    }

                    // Show popup menu only for read command
                    const std::string filter("R) ");
                    const std::string selectedCommandName(text(value()));
                    if (selectedCommandName.find(filter) == std::string::npos) {
                        return 1;
                    }

                    Fl_Menu_Item popupMenu[] = {
                        { "Visualize this", 0, callbackVisualize, (void*)this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR },
                        { 0, 0, 0, 0, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_FOREGROUND_COLOR }
                    };
                    const Fl_Menu_Item * m = popupMenu->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
                    if (m) {
                        m->do_callback(0, m->user_data());
                    }
                    //return 1; // tells caller we handled this event
                }
                break;

            case FL_RELEASE:
                // If mouse right button is released
                if (Fl::event_button() == FL_RIGHT_MOUSE) {
                    return 1; // tells caller we handled this event
                }
        }

        return (Fl_Browser::handle(e));
    }
};

#endif // _popupBrowser_h
