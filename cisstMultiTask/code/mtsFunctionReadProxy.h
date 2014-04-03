/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2009-09-03

  (C) Copyright 2009-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines the function proxy objects.
*/

#ifndef _mtsFunctionReadProxy_h
#define _mtsFunctionReadProxy_h

#include <cisstMultiTask/mtsFunctionRead.h>
#include "mtsProxySerializer.h"

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsFunctionReadProxy : public mtsFunctionRead {
protected:
    typedef mtsFunctionRead BaseType;
    mtsProxySerializer Serializer;

public:
    mtsFunctionReadProxy()
    {}

    ~mtsFunctionReadProxy()
    {}

    /*! Getter */
    inline mtsProxySerializer * GetSerializer(void) {
        return &Serializer;
    }
};

#endif // _mtsFunctionReadProxy_h
