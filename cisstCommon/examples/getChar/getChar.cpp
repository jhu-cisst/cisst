/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/*

  Author(s):  Anton Deguet
  Created on: 2009-04-01

  (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <iostream>
#include <cisstCommon/cmnGetChar.h>
#include <cisstCommon/cmnKbHit.h>

int main(void) {
    //setup an environment and loop until the correct key has been hit
    std::cout << "Hit any key, this will end when you hit \"q\"" << std::endl;
    cmnGetCharEnvironment environment;
    environment.Activate();
    char character;
    do {
        character = environment.GetChar();
        std::cout << " '" << character << "' " << std::flush;
    } while (character != 'q');
    environment.DeActivate();

    // test cmnKbHit
    std::cout << std::endl
              << "Hit any key to stop the loop" << std::endl;
    size_t counter = 0;
    while (!cmnKbHit()) {
        counter++;
        if (counter == 10000) {
            std::cout << "." << std::flush;
            counter = 0;
        }
    }

    // the key pressed for kbhit should still be accessible
    character = cmnGetChar();
    std::cout << std::endl
              << "You pressed '" << character << "' to interrupt the loop" << std::endl;

    // now, use the global function for a single key
    std::cout << "Hit any key to finish" << std::endl;
    cmnGetChar(); // we don't even use the returned value


    return 0;
}
