/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010-2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <iostream>
#include "cdgFile.h"

/*
  \todo preserve order of definition for scopes in code generated
  \todo add lines to all values for debugging
  \todo use global keyword name to set include guards - maybe used for something else?
  \todo implement default values ...
  \todo strip space at end of value
  \todo add method SetValue(keyword, value) for all data types to remove logic from parser
  \todo add method ParsingDone to all data types to check which values have been set, set default for optional and complain for required field
  \todo support member::accessors flag
  \todo support member::scope flag
  \todo support member::default flag
*/

int main(int argc, char* argv[])
{
    if (argc != 6) {
        std::cout << "Error, " << argv[0]
                  << " takes 5 arguments, input file, directory and name for generated header file, directory and name for generated code file"
                  << std::endl;
        return -1;
    }

    std::string inputName = argv[1];
    std::string headerDir = argv[2];
    std::string headerName = argv[3];
    std::string codeDir = argv[4];
    std::string codeName = argv[5];

    std::string headerFull = headerDir + "/" + headerName;
    std::string codeFull = codeDir + "/" + codeName;

    cdgFile file;
    file.SetHeader(headerName);
    std::ifstream input(inputName.c_str());
    bool parseSucceeded;
    if (input.is_open()) {
        parseSucceeded = file.ParseFile(input, inputName);
        input.close();
        if (!parseSucceeded) {
            std::cout << "Error, failed to parse file \"" << inputName << "\"" << std::endl;
            return -1;
        }
    } else {
        std::cout << "Error, can't open file (read mode)\"" << inputName << "\"" << std::endl;
        return -1;
    }

    std::ofstream header(headerFull.c_str());
    if (header.is_open()) {
        std::cout << "Generating header file \"" << headerFull << "\"" << std::endl;
        file.GenerateHeader(header);
        header.close();
    } else {
        std::cout << "Error, can't open file (write) \"" << headerFull << "\"" << std::endl;
        return -1;
    }

    std::ofstream code(codeFull.c_str());
    if (code.is_open()) {
        std::cout << "Generating code file \"" << codeFull << "\"" << std::endl;
        file.GenerateCode(code);
        code.close();
    } else {
        std::cout << "Error, can't open file (write) \"" << codeFull << "\"" << std::endl;
        return -1;
    }

    return 0;
}
