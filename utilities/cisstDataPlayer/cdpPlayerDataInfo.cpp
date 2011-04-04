/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s): Marcin Balicki
  Created on: 2011-02-10

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cdpPlayerDataInfo.h"

CMN_IMPLEMENT_SERVICES(cdpPlayerDataInfo);


cdpPlayerDataInfo::cdpPlayerDataInfo()
{
    NameMember = "";
    DataEndMember = 0;
    DataStartMember = 0;
}


void cdpPlayerDataInfo::ToStream(std::ostream & outputStream) const
{
    outputStream << "\nName: " << this->NameMember
                 << "\nStart : " << this->DataStartMember
                 << "\nEnd : " << this->DataEndMember;
}
