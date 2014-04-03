/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2009-09-03

  (C) Copyright 2009-2011 Johns Hopkins University (JHU), All Rights Reserved.

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

#ifndef _mtsFunctionReturnProxyBase_h
#define _mtsFunctionReturnProxyBase_h

#include "mtsInterfaceRequiredProxy.h"
#include "mtsProxyBaseCommon.h"
#include "mtsProxySerializer.h"

class mtsFunctionReturnProxyBase
{
protected:
    mtsInterfaceRequiredProxy * InterfaceRequired;
    mtsProxySerializer Serializer;
    mtsGenericObject * ResultPointer;
    mtsObjectIDType RemoteResultPointer;

public:
    mtsFunctionReturnProxyBase(mtsInterfaceRequired * interfaceRequired):
        ResultPointer(0)
    {
        this->InterfaceRequired = dynamic_cast<mtsInterfaceRequiredProxy *>(interfaceRequired);
    }

    ~mtsFunctionReturnProxyBase()
    {
        if (this->ResultPointer) {
            delete this->ResultPointer;
        }
    }

    /*! Getter */
    inline mtsProxySerializer * GetSerializer(void) {
        return &(this->Serializer);
    }

    inline mtsGenericObject * GetResultPointer(void) const {
        return this->ResultPointer;
    }

    inline void SetResultPointer(mtsGenericObject * genericObject) {
        this->ResultPointer = genericObject;
    }

    inline mtsObjectIDType GetRemoteResultPointer(void) const {
        return this->RemoteResultPointer;
    }

    inline void SetRemoteResultPointer(const mtsObjectIDType remoteResultPointer) {
        this->RemoteResultPointer = remoteResultPointer;
    }

    inline void SetAsLastFunction(void) {
        this->InterfaceRequired->SetLastFunction(this);
    }
};

#endif // _mtsFunctionReturnProxyBase_h
