/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Martin Kelly
  Created on: 2010-09-27

  (C) Copyright 2010-2018 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/* This is a simple echo program used for osaPipeExecTest */

#include <stdio.h>

int main(int argc, char ** argv) {
    char c;
    /* Line-buffered I/O can interfere with the pipe, so disable it */
    setvbuf(stdout, 0, _IONBF, 0);
    setvbuf(stdin, 0, _IONBF, 0);

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            printf("%s;", argv[i]);
        }
        putchar('\0');
        fflush(stdout);
    }

    while ((c = getchar()) != EOF) {
        if (c == '0') break;   // '0' means exit
        putchar(c);
        fflush(stdout);
    }
}
