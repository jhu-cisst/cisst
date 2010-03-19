/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: P2main.cpp 1236 2010-02-26 20:38:21Z adeguet1 $ */

/*
  Author(s):  Min Yang Jung
  Created on: 2010-03-05

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include "SignalGeneratorTask.h"

#define MAX_SIGNAL_COUNT 12

/*
#include <stdio.h>
#include <time.h>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/gl.h>
//
// Simple GL window with dynamic popup menu
// erco 01/25/06
//
class MyGlWindow : public Fl_Gl_Window {
    void draw() {
        if (!valid()) {
            glLoadIdentity();
            glViewport(0,0,w(),h());
            glOrtho(-w(),w(),-h(),h(),-1,1);
        }
        glClear(GL_COLOR_BUFFER_BIT);
    }
    static void Menu_CB(Fl_Widget*, void *data) {
        char name[80];
        ((Fl_Menu_Button*)data)->item_pathname(name, sizeof(name)-1);
        fprintf(stderr, "Menu Callback: %s\n", name);
    }
    int handle(int e) {
        int ret = Fl_Gl_Window::handle(e);
        switch ( e ) {
            case FL_PUSH:
                if ( Fl::event_button() == 3 ) {
                    char tmp[80];
                    time_t t = time(NULL);
                    sprintf(tmp, "Time is %s", ctime(&t));
                    // Dynamically create menu, pop it up
                    Fl_Menu_Button menu(Fl::event_x_root(), Fl::event_y_root(), 80, 1);
                    menu.add(tmp);      // dynamic -- changes each time popup opens..
                    menu.add("Edit/Copy",  0, Menu_CB, (void*)&menu);
                    menu.add("Edit/Paste", 0, Menu_CB, (void*)&menu);
                    menu.add("Quit",       0, Menu_CB, (void*)&menu);
                    menu.popup();
                }
        }
        return(ret);
    }
public:
    // CONSTRUCTOR
    MyGlWindow(int X,int Y,int W,int H,const char*L=0) : Fl_Gl_Window(X,Y,W,H,L) {
    }
};

// MAIN
int main() {
     Fl_Window win(500, 300);
     MyGlWindow mygl(10, 10, win.w()-20, win.h()-20);
     win.show();
     return(Fl::run());
}
// Popup menu using callbacks -erco
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu.H>
#include <stdio.h>
// Callback invoked when menu item selected
void handle_menu(Fl_Widget *w, void *v) {
    if(!w || !v) return;
    switch((int)v) {
        case 1: fl_choice("Thing 1 happened", "OK", NULL, NULL); break;
        case 2: fl_choice("Thing 2 happened", "OK", NULL, NULL); break;
        case 3: fl_choice("Thing 3 happened", "OK", NULL, NULL); break;
    }
}
// Callback invoked when button pushed
void push_cb(Fl_Widget *w, void*) {
    Fl_Menu_Item rclick_menu[] = {
        { "Do Thing 1",  0, handle_menu, (void*)1 },
        { "Do Thing 2",  0, handle_menu, (void*)2 },
        { "Do Thing 3",  0, handle_menu, (void*)3 },
        { 0 }
    };
    const Fl_Menu_Item *m = rclick_menu->popup(Fl::event_x(), Fl::event_y(), 0, 0, 0);
    if ( m ) m->do_callback(w, m->user_data());
    return;
}
int main() {
    Fl_Window win(140,45,"Simple Popup Menu (using callbacks)");
    Fl_Button butt(10,10,120,25,"Push For Menu");
    butt.callback(push_cb);
    win.show();
    return(Fl::run());
}
*/

int main(int argc, char * argv[])
{
    // Set global component manager IP
    std::string globalComponentManagerIP;

    if (argc == 1) {
        globalComponentManagerIP = "localhost";
    } else if (argc == 2) {
        globalComponentManagerIP = argv[1];
    } else {
        std::cerr << "Usage: " << argv[0] << " (global component manager IP)" << std::endl;
        return 1;
    }

    std::cout << "Global component manager IP: " << globalComponentManagerIP << std::endl;

    // log configuration
    cmnLogger::SetLoD(CMN_LOG_LOD_VERY_VERBOSE);
    cmnLogger::GetMultiplexer()->AddChannel(std::cout, CMN_LOG_LOD_VERY_VERBOSE);
    // add a log per thread
    osaThreadedLogFile threadedLog("SignalGenerator");
    cmnLogger::GetMultiplexer()->AddChannel(threadedLog, CMN_LOG_LOD_VERY_VERBOSE);

    // Get local component manager instance
    mtsTaskManager * taskManager;
    try {
        taskManager = mtsTaskManager::GetInstance(globalComponentManagerIP, "SignalGenerator");
    } catch (...) {
        CMN_LOG_INIT_ERROR << "Failed to initialize local component manager" << std::endl;
        return 1;
    }

    // create signal generator component
    const double PeriodClient = 50 * cmn_ms;
    SignalGenerator * signalGenerator = new SignalGenerator("SignalGenerator", PeriodClient);
    taskManager->AddComponent(signalGenerator);

    // create the tasks, i.e. find the commands
    taskManager->CreateAll();
    // start the periodic Run
    taskManager->StartAll();
    
    while (1) {
        osaSleep(10 * cmn_ms);
    }
    
    // cleanup
    taskManager->KillAll();
    taskManager->Cleanup();

    return 0;
}
