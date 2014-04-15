/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2010-05-05

  (C) Copyright 2010-2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstVector/vctPlot2DOpenGLFLTK.h>

#include <FL/Fl_Menu_Button.H>

vctPlot2DOpenGLFLTK::vctPlot2DOpenGLFLTK(const int x, const int y,
                                         const int width, const int height,
                                         const char * title):
    Fl_Gl_Window(x, y, width, height, title),
    vctPlot2DOpenGL()
{
    this->Viewport.X() = width;
    this->Viewport.Y() = height;
}


void vctPlot2DOpenGLFLTK::initgl(void)
{
    vctPlot2DOpenGL::RenderInitialize();
    vctPlot2DOpenGL::RenderResize(this->Viewport.X(), this->Viewport.Y());
}


void vctPlot2DOpenGLFLTK::draw(void)
{
    if (!valid()) {
        initgl();
        valid(1);
    }
    vctPlot2DOpenGL::Render();
}


int vctPlot2DOpenGLFLTK::handle(int event) {
    int button;
#if 0
    Fl_Menu_Button * popup;
#endif
    switch(event) {
    case FL_PUSH:
        button = Fl::event_button();
#if 0
        popup = new Fl_Menu_Button(0, 0, 400, 400);
        popup->type(Fl_Menu_Button::POPUP3);
        popup->add("This|is|a popup|menu");
        popup->show();
#endif
        std::cout << "Click button " << button << std::endl;
        return 0;
        break;
    case FL_DRAG:
    case FL_RELEASE:
    case FL_FOCUS :
    case FL_UNFOCUS :
    case FL_KEYBOARD:
    case FL_SHORTCUT:
    default:
        // pass other events to the base class...
        return Fl_Gl_Window::handle(event);
  }
}
