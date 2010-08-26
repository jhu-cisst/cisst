/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: parserMain.cpp 1030 2010-06-11 jkriss1 $ */

#include <string>
#include <iostream>
#include <fstream>
#include "parser.h"

int main(int argc, char* argv[])
{
    std::string filename = argv[1];

	parser * parseTool = new parser(filename);
	std::ifstream fin;
	fin.open(argv[1]);
	std::string s;

	//getline(fin, s);
	//cout << s << endl;

	if(!parseTool->parseFile())
		std::cout << "Error!" << std::endl;

	parseTool->GenerateCode();

	fin.close();
}
