/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2010-09-06

  (C) Copyright 2010-2023 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _cdgTypedef_h
#define _cdgTypedef_h

#include "cdgScope.h"

class cdgTypedef: public cdgScope {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    friend class cdgData;
    friend class cdgFile;

public:
    cdgTypedef(size_t lineNumber);
    cdgScope::Type GetScope(void) const override;
    cdgScope * Create(size_t lineNumber) const override;
    bool Validate(std::string & errorMessage) override;
    void GenerateHeader(std::ostream & outputStream) const override;
    void GenerateCode(std::ostream & CMN_UNUSED(outputStream)) const override {};

private:
    cdgTypedef(void); // make sure constructor with line number is always used.
};

CMN_DECLARE_SERVICES_INSTANTIATION(cdgTypedef);

#endif // _cdgTypedef_h
