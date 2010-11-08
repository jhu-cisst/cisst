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

#ifndef _cdgFile_h
#define _cdgFile_h

#include <iostream>
#include <vector>

#include "cdgGlobal.h"

/*
  \todo strip space at end of value
  \todo add method SetValue(keyword, value) for all data types to remove logic from parser
  \todo add method ParsingDone to all data types to check which values have been set, set default for optional and complain for required field
  \todo support member::accessors flag
  \todo support member::scope flag
  \todo support member::default flag
  \todo support class::code snippets
 */

class cdgFile {

    cdgGlobal * Global;

public:
    bool ParseFile(std::ifstream & input,
                   const std::string & filename);

    void GenerateHeader(std::ostream & outputStream) const;

    void GenerateCode(std::ostream & outputStream,
                      const std::string & header) const;

protected:
    static void RemoveTrailingSpaces(std::string & value);
};

#endif // _cdgFile_h
