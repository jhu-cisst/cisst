/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <string>
#include <iostream>
#include <fstream>
#include <ccgParser.h>

int main(int argc, char* argv[])
{
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
