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

#ifndef _mtsFunctionWriteProxy_h
#define _mtsFunctionWriteProxy_h

#include <cisstMultiTask/mtsFunctionWrite.h>
#include "mtsProxySerializer.h"

#include <cisstMultiTask/mtsExport.h>

class CISST_EXPORT mtsFunctionWriteProxy : public mtsFunctionWrite {
protected:
    typedef mtsFunctionWrite BaseType;
    mtsProxySerializer Serializer;
    std::string argSerialized;
    mtsGenericObject *arg;

public:
    mtsFunctionWriteProxy():
        mtsFunctionWrite(true /* this is a proxy class */), arg(0)
    {}

    mtsFunctionWriteProxy(const std::string &argumentPrototypeSerialized):
        mtsFunctionWrite(true /* this is a proxy class */), argSerialized(argumentPrototypeSerialized)
    {
        arg = Serializer.DeSerialize(argumentPrototypeSerialized);
        if (!arg)
            CMN_LOG_INIT_ERROR << "mtsFunctionWriteProxy: could not deserialize argument prototype" << std::endl;
    }

    ~mtsFunctionWriteProxy()
    {
        delete arg;
    }

    /*! Getter */
    inline mtsProxySerializer * GetSerializer(void)
    {
        return &Serializer;
    }

    inline mtsExecutionResult ExecuteSerialized(const std::string &inputArgSerialized, mtsProxySerializer *serializer)
    {
        mtsExecutionResult ret = mtsExecutionResult:: ARGUMENT_DYNAMIC_CREATION_FAILED;
        // If arg has not yet been dynamically constructed, try again because the
        // class may have been dynamically loaded since the last attempt to construct it.
        // Note that we could have the deserializer dynamically create the object from
        // inputArgSerialized, but this would lead to unexpected results if the client
        // sends the incorrect type.
        if (!arg)
            arg = Serializer.DeSerialize(argSerialized);
        if (arg) {
            if (serializer->DeSerialize(inputArgSerialized, *arg))
                ret = Execute(*arg);
            else
                ret = mtsExecutionResult::DESERIALIZATION_ERROR;
        }
        return ret;
    }
};

#endif // _mtsFunctionWriteProxy_h
