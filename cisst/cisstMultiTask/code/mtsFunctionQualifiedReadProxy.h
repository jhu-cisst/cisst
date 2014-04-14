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

#ifndef _mtsFunctionQualifiedReadProxy_h
#define _mtsFunctionQualifiedReadProxy_h

#include <cisstMultiTask/mtsFunctionQualifiedRead.h>
#include "mtsProxySerializer.h"

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsFunctionQualifiedReadProxy : public mtsFunctionQualifiedRead {
protected:
    typedef mtsFunctionQualifiedRead BaseType;
    mtsProxySerializer Serializer;
    std::string arg1Serialized;
    std::string arg2Serialized;
    mtsGenericObject *arg1;
    mtsGenericObject *arg2;

public:
    mtsFunctionQualifiedReadProxy() : arg1(0), arg2(0)
    {}

    mtsFunctionQualifiedReadProxy(const std::string &arg1PrototypeSerialized,
                                  const std::string &arg2PrototypeSerialized)
        : arg1Serialized(arg1PrototypeSerialized), arg2Serialized(arg2PrototypeSerialized)
    {
        arg1 = Serializer.DeSerialize(arg1PrototypeSerialized);
        if (!arg1)
            CMN_LOG_INIT_ERROR << "mtsFunctionQualifiedReadProxy: could not deserialize argument1 prototype" << std::endl;
        arg2 = Serializer.DeSerialize(arg2PrototypeSerialized);
        if (!arg2)
            CMN_LOG_INIT_ERROR << "mtsFunctionQualifiedReadProxy: could not deserialize argument2 prototype" << std::endl;
    }
    
    ~mtsFunctionQualifiedReadProxy()
    {
        delete arg1;
        delete arg2;
    }

    /*! Getter */
    inline mtsProxySerializer * GetSerializer(void) {
        return &Serializer;
    }

    inline mtsExecutionResult ExecuteSerialized(const std::string &inputArgSerialized,
                                                std::string &resultArgSerialized)
    {
        mtsExecutionResult ret = mtsExecutionResult:: ARGUMENT_DYNAMIC_CREATION_FAILED;
        // If args have not yet been dynamically constructed, try again because the
        // classes may have been dynamically loaded since the last attempt to construct them.
        if (!arg1)
            arg1 = Serializer.DeSerialize(arg1Serialized);
        if (!arg2)
            arg2 = Serializer.DeSerialize(arg2Serialized);
        if (arg1 && arg2) {
            if (Serializer.DeSerialize(inputArgSerialized, *arg1)) {
                ret = Execute(*arg1, *arg2);
                if (ret.IsOK()) {
                    if (!Serializer.Serialize(*arg2, resultArgSerialized))
                        ret = mtsExecutionResult::SERIALIZATION_ERROR;
                }
            }
            else
                ret = mtsExecutionResult::DESERIALIZATION_ERROR;
        }
        return ret;
    }
};

#endif // _mtsFunctionQualifiedReadProxy_h
