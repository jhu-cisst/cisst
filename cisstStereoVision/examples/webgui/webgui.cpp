/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  
  Author(s):  Balazs Vagvolgyi
  Created on: 2009

  (C) Copyright 2006-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstStereoVision.h>


int main(void)
{
    svlWebPublisher publisher(8080, true);

    svlWebXMLObject object("note", true, true);
    object.AddNode("from", "Balazs");
    object.AddNode("to", "Anton");
    object.AddNode("heading", "Reminder");
    object.AddNode("body", "Don't forget about this weekend!");
    object.AddNode("ruler", "0");
    publisher.AddObject(&object);

    if (publisher.Start() == 0) {
        std::cout << "+ Server running; press any key to quit..." << std::endl;
        cmnGetChar();
        
        std::cout << "+ Stopping server..." << std::endl;
        publisher.Stop();
    }
    else {
        std::cout << "! There was an error while starting server." << std::endl;
    }

    std::cout << "+ Quit" << std::endl;

    return 0;
}

