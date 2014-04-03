/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2013-12-22

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/


#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnKbHit.h>

#if (CISST_OS == CISST_LINUX) || (CISST_OS == CISST_DARWIN) || (CISST_OS == CISST_SOLARIS) || (CISST_OS == CISST_LINUX_RTAI) || (CISST_OS == CISST_QNX) || (CISST_OS == CISST_LINUX_XENOMAI)
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#if (CISST_OS == CISST_QNX)
#include <sys/time.h>
#endif

int cmnKbHit(void)
{
  struct termios oldt, newt;
  struct timeval tv;
  fd_set rdfs;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  // set env to be non blocking
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~ICANON;
  newt.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  FD_ZERO(&rdfs);
  FD_SET(STDIN_FILENO, &rdfs);

  select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
  int result = FD_ISSET(STDIN_FILENO, &rdfs);

  // restore env
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

  return result;
}
#endif // CISST_LINUX || CISST_DARWIN || CISST_SOLARIS || CISST_RTAI || CISST_QNX


#if (CISST_OS == CISST_WINDOWS)
#include <conio.h>
int cmnKbHit(void) {
    return _kbhit();
}
#endif // CISST_WINDOWS
