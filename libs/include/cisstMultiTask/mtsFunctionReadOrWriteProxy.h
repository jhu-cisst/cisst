/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsFunctionReadOrWriteProxy.h 567 2009-07-18 06:42:05Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2009-09-03

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights Reserved.

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

#ifndef _mtsFunctionReadOrWriteProxy_h
#define _mtsFunctionReadOrWriteProxy_h

#include <cisstMultiTask/mtsFunctionReadOrWrite.h>
#include <cisstMultiTask/mtsProxySerializer.h>

#include <cisstMultiTask/mtsExport.h>

template <class _argumentType>
class CISST_EXPORT mtsFunctionReadOrWriteProxy : public mtsFunctionReadOrWrite<_argumentType> {
protected:
    typedef mtsFunctionReadOrWrite<_argumentType> BaseFunctionType;
    mtsProxySerializer Serializer;

public:
    mtsFunctionReadOrWriteProxy()
    {}

    /*! This constructor is not used */
    mtsFunctionReadOrWriteProxy(const mtsDeviceInterface * associatedInterface, const std::string & commandName)
        : BaseFunctionType(associatedInterface, commandName)
    {}

    ~mtsFunctionReadOrWriteProxy()
    {}

    /*! Getter */
    inline mtsProxySerializer * GetSerializer() {
        return &Serializer;
    }
};

typedef mtsFunctionReadOrWriteProxy<mtsGenericObject> mtsFunctionReadProxy;
typedef mtsFunctionReadOrWriteProxy<const mtsGenericObject> mtsFunctionWriteProxy;


#endif // _mtsFunctionReadOrWriteProxy_h

