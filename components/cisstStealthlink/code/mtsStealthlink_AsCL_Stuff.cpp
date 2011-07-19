/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Peter Kazanzides, Anton Deguet
  Created on: 2006

  (C) Copyright 2007-2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <iostream>
#include "mtsStealthlink_AsCL_Stuff.h"

//------------------------------------------------------------------------------
mtsStealthlink_AsCL_IO_Watch::mtsStealthlink_AsCL_IO_Watch(void) {
    _handler  = 0;
}


mtsStealthlink_AsCL_IO_Watch::~mtsStealthlink_AsCL_IO_Watch() {
    RemoveWatch();
}


int mtsStealthlink_AsCL_IO_Watch::AddWatch(int sock, void * func, void * client_ptr)
{
    if (callback)
        ;   // callback is already active -- what to do?
    fd = sock;
    callback = (GTK_CallBack)func;
    // Do we need to use a static wrapper for the callback?
    // Maybe not.  GDK and Xt have different callback function signatures
    // and Medtronic has no code to handle that, so maybe the callback
    // function does not use its parameters.
    // Fl::add_fd(fd, WatchCallback, client_ptr);
    return 1;
}


void mtsStealthlink_AsCL_IO_Watch::RemoveWatch(void)
{
    // Fl::remove_fd(fd);
    callback = 0;
}


mtsStealthlink_AsCL_IO_Watch::GTK_CallBack mtsStealthlink_AsCL_IO_Watch::callback = 0;


// This function is an mtsStealthlink-compatible callback because it accepts parameters
// of int and void*.  It converts to a GTK-compatible callback by only
// passing the void* parameter.
void mtsStealthlink_AsCL_IO_Watch::WatchCallback(int fd, void * data)
{
    if (callback) {
        callback(data);
    }
}


//------------------------------------------------------------------------------
mtsStealthlink_AsCL_Timeout::mtsStealthlink_AsCL_Timeout(void)
{
    _handler = 0;
}


mtsStealthlink_AsCL_Timeout::~mtsStealthlink_AsCL_Timeout(void)
{
    RemoveTimeout();
}


int mtsStealthlink_AsCL_Timeout::AddTimeout(int tmo_val, void * func, void * obj)
{
    if (callback) {
        // callback is already active -- what to do?
    }

    timeout = tmo_val / 1000.0;
    callback = (StealthFunction) func;
    dataobj = obj;

    RemoveTimeout();

    // Fl::add_timeout(timeout, MyCallback, dataobj );

    return 1;
}


void mtsStealthlink_AsCL_Timeout::RemoveTimeout(void)
{
    // Fl::remove_timeout(MyCallback, dataobj);
    callback = 0;
}

mtsStealthlink_AsCL_Timeout::StealthFunction mtsStealthlink_AsCL_Timeout::callback = 0;

double mtsStealthlink_AsCL_Timeout::timeout = 0.0;


void mtsStealthlink_AsCL_Timeout::MyCallback(void *data)
{
    // if callback function returns non-zero (true), reschedule timeout.
    if (callback) {
        int rc = callback(data);
        if (rc) {
            // Fl::repeat_timeout(timeout, MyCallback, data);
        }
    }
}


//------------------------------------------------------------------------------
mtsStealthlink_AsCL_Utils::mtsStealthlink_AsCL_Utils(void)
{}


mtsStealthlink_AsCL_Utils::~mtsStealthlink_AsCL_Utils(void)
{}


AsCL_IO_Watch * mtsStealthlink_AsCL_Utils::new_IO_Watch(void)
{
    return new mtsStealthlink_AsCL_IO_Watch;
}


AsCL_Timeout * mtsStealthlink_AsCL_Utils::new_Timeout(void)
{
    return new mtsStealthlink_AsCL_Timeout;
}

