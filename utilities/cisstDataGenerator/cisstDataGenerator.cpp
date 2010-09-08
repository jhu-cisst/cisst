/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: $

  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <iostream>
#include "cdgData.h"

/* example:

include <cisstVector/vctFixedSizeTypes.h>
include <iostream>

typename newType

typedef vctFixedSizeVector<vctFixedSizeVector<3, double>, 3> MatrixType;
typedef IndexType Index

member double Sine Sine
member vctDouble3 Position End effector
member std::string Name Human readable name
member IndexType Index

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

    cdgData data;
    std::ifstream input(inputName.c_str());
    if (input.is_open()) {
        data.ParseFile(input, inputName);
        input.close();
    } else {
        std::cout << "Error, can't open file (read) \"" << inputName << "\"" << std::endl;
        return -1;
    }

    std::ofstream header(headerFull.c_str());
    if (header.is_open()) {
        data.GenerateHeader(header);
        header.close();
    } else {
        std::cout << "Error, can't open file (write) \"" << headerFull << "\"" << std::endl;
        return -1;
    }

    std::ofstream code(codeFull.c_str());
    if (code.is_open()) {
        data.GenerateCode(code, headerName);
        code.close();
    } else {
        std::cout << "Error, can't open file (write) \"" << codeFull << "\"" << std::endl;
        return -1;
    }

    return 0;
}
