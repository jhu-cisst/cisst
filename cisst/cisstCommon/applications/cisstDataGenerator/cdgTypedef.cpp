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

#include "cdgTypedef.h"

CMN_IMPLEMENT_SERVICES(cdgTypedef);


cdgTypedef::cdgTypedef(unsigned int lineNumber):
    cdgScope(lineNumber)
{
    cdgField * field;
    field = this->AddField("name", "", true);
    CMN_ASSERT(field);

    field = this->AddField("type", "", true);
    CMN_ASSERT(field);
}


cdgScope::Type cdgTypedef::GetScope(void) const
{
    return cdgScope::CDG_TYPEDEF;
}


bool cdgTypedef::HasScope(const std::string & CMN_UNUSED(keyword),
                          cdgScope::Stack & CMN_UNUSED(scopes),
                          unsigned int CMN_UNUSED(lineNumber))
{
    return false;
}


void cdgTypedef::GenerateHeader(std::ostream & outputStream) const
{
    outputStream << "    typedef " << this->GetFieldValue("type") << " " << this->GetFieldValue("name") << ";" << std::endl;
}
