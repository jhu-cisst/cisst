/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Min Yang Jung
  Created on: 2009-09-03

  (C) Copyright 2009-2013 Johns Hopkins University (JHU), All Rights Reserved.

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
    std::string argSerialized;
    mtsGenericObject *arg;

public:
    mtsFunctionReadProxy() : mtsFunctionRead(), arg(0)
    {}

    mtsFunctionReadProxy(const std::string &argumentPrototypeSerialized)
        : mtsFunctionRead(), argSerialized(argumentPrototypeSerialized)
    {
        arg = Serializer.DeSerialize(argumentPrototypeSerialized);
        if (!arg)
            CMN_LOG_INIT_ERROR << "mtsFunctionReadProxy: could not deserialize argument prototype" << std::endl;
    }

    ~mtsFunctionReadProxy()
    {
        delete arg;
    }

    /*! Getter */
    inline mtsProxySerializer * GetSerializer(void) {
        return &Serializer;
    }

    inline mtsExecutionResult ExecuteSerialized(std::string &resultArgSerialized, mtsProxySerializer *serializer)
    {
        resultArgSerialized.clear();
        mtsExecutionResult ret = mtsExecutionResult::ARGUMENT_DYNAMIC_CREATION_FAILED;
        // If arg has not yet been dynamically constructed, try again because the
        // class may have been dynamically loaded since the last attempt to construct it.
        if (!arg)
            arg = Serializer.DeSerialize(argSerialized);
        if (arg) {
            ret = Execute(*arg);
            if (ret.IsOK()) {
                if (!serializer->Serialize(*arg, resultArgSerialized))
                    ret = mtsExecutionResult::SERIALIZATION_ERROR;
            }
        }
        return ret;
    }

};

#endif // _mtsFunctionReadProxy_h
