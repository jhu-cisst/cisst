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

#ifndef _cdgTypedef_h
#define _cdgTypedef_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegisterMacros.h>

class cdgTypedef: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    friend class cdgData;
    friend class cdgFile;

    std::string Type;
    std::string Name;

public:
    static bool IsKeyword(const std::string & keyword);
    bool SetValue(const std::string & keyword, const std::string & value,
                  std::string & errorMessage);
    bool IsValid(std::string & errorMessage);
    void GenerateHeader(std::ostream & outputStream) const;
};

CMN_DECLARE_SERVICES_INSTANTIATION(cdgTypedef);

#endif // _cdgTypedef_h
