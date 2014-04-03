/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

  --- begin cisst license - do not edit ---

  This software is provided "as is" under an open source license, with
  no warranty.  The complete license can be found in license.txt and
  http://www.cisst.org/cisst/license.txt.

  --- end cisst license ---

*/

#ifndef _cdgFile_h
#define _cdgFile_h

#include <iostream>
#include <vector>

#include "cdgGlobal.h"

class cdgFile {

    cdgGlobal * Global;

public:
    void SetHeader(const std::string & header);

    bool ParseFile(std::ifstream & input,
                   const std::string & filename);
    bool Validate(std::string & errorMessage);
    void GenerateHeader(std::ostream & outputStream) const;
    void GenerateCode(std::ostream & outputStream) const;
    void DisplaySyntax(std::ostream & outputStream) const;

protected:
    static void RemoveTrailingSpaces(std::string & value);
    void GenerateMessage(std::ostream & outputStream) const;

    std::string Filename;
    std::string Header;
    std::string HeaderGuard;
};

#endif // _cdgFile_h
