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

#ifndef _cdgField_h
#define _cdgField_h


#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegisterMacros.h>

#include <list>

class cdgField: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    cdgField(const std::string & keyword, const std::string & defaultValue, const bool required, const std::string & description);

    void AddPossibleValue(const std::string & possibleValue);
    const std::string & GetValue(void) const;
    bool SetValue(const std::string & value, std::string & errorMessage, const bool & allowOverwrite = false);
    bool IsValid(std::string & errorMessage) const;
    void FillInDefaults(void);
    void DisplaySyntax(std::ostream & outputStream, size_t offsetSize) const;

protected:
    std::string Keyword;
    std::string Value;
    std::string Default;
    bool Required;
    std::string Description;
    typedef std::vector<std::string> ValuesContainer;
    ValuesContainer PossibleValues;

private:
    cdgField();
};

CMN_DECLARE_SERVICES_INSTANTIATION(cdgField);

#endif // _cdgField_h
