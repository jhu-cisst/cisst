/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$
  
  Author(s):  Anton Deguet
  Created on: 2007-04-08

  (C) Copyright 2007-2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#ifndef _serializationClasses_h
#define _serializationClasses_h

#include <cisstCommon/cmnPortability.h>
#include <cisstCommon/cmnClassRegister.h>

class CISST_EXPORT classA: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, 5);

public:
    double Value;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

// Add services instantiation
CMN_DECLARE_SERVICES_INSTANTIATION(classA)


class CISST_EXPORT classB: public cmnGenericObject {
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, 5);

public:
    std::string Value;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

// Add services instantiation
CMN_DECLARE_SERVICES_INSTANTIATION(classB)


#endif // _serializationClasses_h
