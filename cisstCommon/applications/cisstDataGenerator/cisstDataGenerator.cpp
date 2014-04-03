/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010-2014 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <iostream>

#include <cisstCommon/cmnCommandLineOptions.h>
#include "cdgFile.h"


int main(int argc, char* argv[])
{
    cmnCommandLineOptions options;
    std::string inputName;
    options.AddOptionOneValue("i", "input", "input file",
                              cmnCommandLineOptions::REQUIRED_OPTION, &inputName);
    std::string headerDir;
    options.AddOptionOneValue("d", "header-directory", "destination directory for generated header file",
                              cmnCommandLineOptions::REQUIRED_OPTION, &headerDir);
    std::string headerName;
    options.AddOptionOneValue("h", "header-file", "generated header filename, can contain a subdirectory (e.g. dir/file.h)",
                              cmnCommandLineOptions::REQUIRED_OPTION, &headerName);
    std::string codeDir;
    options.AddOptionOneValue("D", "code-directory", "destination directory for generated code file",
                              cmnCommandLineOptions::REQUIRED_OPTION, &codeDir);
    std::string codeName;
    options.AddOptionOneValue("c", "code-file", "generated code filename",
                              cmnCommandLineOptions::REQUIRED_OPTION, &codeName);

    options.AddOptionNoValue("v", "verbose", "verbose output");

    options.AddOptionNoValue("s", "syntax-only", "display the data description syntax and exit", cmnCommandLineOptions::SQUASH_REQUIRED_OPTION);

    std::string errorMessage;
    if (!options.Parse(argc, argv, errorMessage)) {
        std::cerr << "Error: " << errorMessage << std::endl;
        options.PrintUsage(std::cerr);
        return -1;
    }

    cdgFile file;
    if (options.IsSet("syntax-only")) {
        file.DisplaySyntax(std::cout);
        exit(1);
    }

    bool verbose;
    verbose = options.IsSet("verbose");

    std::string headerFull = headerDir + "/" + headerName;
    std::string codeFull = codeDir + "/" + codeName;

    if (verbose) {
        std::cout << "cisstDataGenerator: generating " << headerFull << " and " << codeFull << " from " << inputName << std::endl;
    }

    file.SetHeader(headerName);
    std::ifstream input(inputName.c_str());
    bool parseSucceeded;
    if (input.is_open()) {
        parseSucceeded = file.ParseFile(input, inputName);
        input.close();
        if (!parseSucceeded) {
            std::cout << "Error, failed to parse file \"" << inputName << "\"" << std::endl;
            return -1;
        } else if (verbose) {
            std::cout << "cisstDataGenerator: parse succeeded" << std::endl;
        }
    } else {
        std::cout << "Error, can't open file (read mode)\"" << inputName << "\"" << std::endl;
        return -1;
    }

    if (!file.Validate(errorMessage)) {
        std::cout << "Error, validation failed:" << errorMessage << std::endl;
        return -1;
    }

    std::ofstream header(headerFull.c_str());
    if (header.is_open()) {
        if (verbose) {
            std::cout << "cisstDataGenerator: generating header file \"" << headerFull << "\"" << std::endl;
        }
        file.GenerateHeader(header);
        header.close();
    } else {
        std::cout << "Error, can't open file (write) \"" << headerFull << "\"" << std::endl;
        return -1;
    }

    std::ofstream code(codeFull.c_str());
    if (code.is_open()) {
        if (verbose) {
            std::cout << "cisstDataGenerator: generating code file \"" << codeFull << "\"" << std::endl;
        }
        file.GenerateCode(code);
        code.close();
    } else {
        std::cout << "Error, can't open file (write) \"" << codeFull << "\"" << std::endl;
        return -1;
    }

    return 0;
}
