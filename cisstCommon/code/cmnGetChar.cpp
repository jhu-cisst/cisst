/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet, Balazs Vagvolgyi, Min Yang Jung
  Created on: 2009-03-26

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnAssert.h>
#include <cisstCommon/cmnGetChar.h>

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#endif // CISST_LINUX || CISST_DARWIN || CISST_SOLARIS || CISST_RTAI || CISST_QNX

#if (CISST_OS == CISST_WINDOWS)
#include <conio.h>
#endif // CISST_WINDOWS

struct cmnGetCharEnvironmentInternals {
#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
    struct  termios OldSettings;
    struct  termios NewSettings;
    int     Keyboard;
#endif // CISST_LINUX || CISST_DARWIN || CISST_SOLARIS || CISST_RTAI || CISST_QNX
};


cmnGetCharEnvironment::cmnGetCharEnvironment(void):
    Activated(false)
{
#if (CISST_OS != CISST_WINDOWS)
    Internals = new cmnGetCharEnvironmentInternals;
#endif 
}


cmnGetCharEnvironment::~cmnGetCharEnvironment(void)
{
    if (this->Activated) {
        this->DeActivate();
    }
#if (CISST_OS != CISST_WINDOWS)
    if (Internals) {
        delete Internals;
    }
#endif 
}


#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
bool cmnGetCharEnvironment::Activate(void)
{
    if (!this->Activated) {
        Internals->Keyboard = open("/dev/tty",O_RDWR);
        ioctl(Internals->Keyboard, TCGETS, &Internals->OldSettings);
        Internals->NewSettings = Internals->OldSettings;
        Internals->NewSettings.c_lflag &= !ICANON;
        Internals->NewSettings.c_lflag &= !ECHO;
        ioctl(Internals->Keyboard, TCSETS, &Internals->NewSettings);
        this->Activated = true;
        return true;
    }
    return false;
}
#endif // CISST_LINUX || CISST_SOLARIS || CISST_RTAI || CISST_QNX

#if (CISST_OS == CISST_DARWIN)
bool cmnGetCharEnvironment::Activate(void)
{
    if (!this->Activated) {
        Internals->Keyboard = open("/dev/tty",O_RDWR);
        ioctl(Internals->Keyboard, TIOCGETA, &Internals->OldSettings);
        Internals->NewSettings = Internals->OldSettings;
        Internals->NewSettings.c_lflag &= !ICANON;
        Internals->NewSettings.c_lflag &= !ECHO;
        ioctl(Internals->Keyboard, TIOCSETA, &Internals->NewSettings);
        this->Activated = true;
        return true;
    }
    return false;
}
#endif // CISST_DARWIN

#if (CISST_OS == CISST_WINDOWS)
bool cmnGetCharEnvironment::Activate(void)
{
    if (!this->Activated) {
        this->Activated = true;
        return true;
    }
    return false;
}
#endif // CISST_WINDOWS


#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
bool cmnGetCharEnvironment::DeActivate(void)
{
    if (this->Activated) {
        ioctl(Internals->Keyboard, TCSETS, &Internals->OldSettings);
        close(Internals->Keyboard);
        this->Activated = false;
        return true;
    }
    return false;
}
#endif // CISST_LINUX || CISST_SOLARIS || CISST_RTAI || CISST_QNX

#if (CISST_OS == CISST_DARWIN)
bool cmnGetCharEnvironment::DeActivate(void)
{
    if (this->Activated) {
        ioctl(Internals->Keyboard, TIOCSETA, &Internals->OldSettings);
        close(Internals->Keyboard);
        this->Activated = false;
        return true;
    }
    return false;
}
#endif // CISST_DARWIN

#if (CISST_OS == CISST_WINDOWS)
bool cmnGetCharEnvironment::DeActivate(void)
{
    if (this->Activated) {
        this->Activated = false;
        return true;
    }
    return false;
}
#endif // CISST_WINDOWS


#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
int cmnGetCharEnvironment::GetChar(void)
{
    if (this->Activated) {
        return getchar();
    }
    return 0;
}
#endif // CISST_LINUX || CISST_DARWIN ||CISST_SOLARIS || CISST_RTAI || CISST_QNX

#if (CISST_OS == CISST_WINDOWS)
int cmnGetCharEnvironment::GetChar(void)
{
    if (this->Activated) {
        return _getch();
    }
    return 0;
}
#endif // CISST_WINDOWS


int cmnGetChar(void)
{
    cmnGetCharEnvironment environment;
    environment.Activate();
    int result;
    result = environment.GetChar();
    environment.DeActivate();
    return result;
}
