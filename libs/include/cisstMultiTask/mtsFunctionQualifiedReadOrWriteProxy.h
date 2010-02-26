/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#ifndef _mtsFunctionQualifiedReadOrWriteProxy_h
#define _mtsFunctionQualifiedReadOrWriteProxy_h

#include <cisstMultiTask/mtsFunctionQualifiedReadOrWrite.h>
#include <cisstMultiTask/mtsProxySerializer.h>

#include <cisstMultiTask/mtsExport.h>

template <class _argumentType>
class CISST_EXPORT mtsFunctionQualifiedReadOrWriteProxy : public mtsFunctionQualifiedReadOrWrite<_argumentType> {
protected:
    typedef mtsFunctionQualifiedReadOrWrite<_argumentType> BaseFunctionType;
    mtsProxySerializer Serializer;

public:
    mtsFunctionQualifiedReadOrWriteProxy()
    {}

    /*! This constructor is not used */
    mtsFunctionQualifiedReadOrWriteProxy(const mtsDeviceInterface * associatedInterface, const std::string & commandName)
        : BaseFunctionType(associatedInterface, commandName)
    {}

    ~mtsFunctionQualifiedReadOrWriteProxy()
    {}

    /*! Getter */
    inline mtsProxySerializer * GetSerializer() {
        return &Serializer;
    }
};

typedef mtsFunctionQualifiedReadOrWriteProxy<mtsGenericObject> mtsFunctionQualifiedReadProxy;

#endif // _mtsFunctionQualifiedReadOrWriteProxy_h

