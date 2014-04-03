/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s): Jonathan Kriss, Anton Deguet
  Created on: 2010

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <string>
#include <iostream>
#include <fstream>
#include <ccgParser.h>

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "Error, " << argv[0]
                  << " takes 2 arguments, input and output files"
                  << std::endl;
        return -1;
    }
    std::string input = argv[1];
    std::string output = argv[2];

    cmnLogger::AddChannel(std::cout);

    ccgParser * parser = new ccgParser(input);
    std::ifstream fin;
    fin.open(argv[1]);
    std::string s;

    if(!parser->ParseFile()) {
        std::cout << "An error occurred while parsing: " << input << std::endl;
    }

    parser->GenerateCode(output);

    fin.close();
}
