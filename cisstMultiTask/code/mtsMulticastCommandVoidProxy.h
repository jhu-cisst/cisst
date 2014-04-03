/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Min Yang Jung
  Created on: 2009-06-24

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


/*!
  \file
  \brief Defines a command sent to multiple interfaces
*/


#ifndef _mtsMulticastCommandVoidProxy_h
#define _mtsMulticastCommandVoidProxy_h

#include <cisstMultiTask/mtsMulticastCommandVoid.h>
#include "mtsProxySerializer.h"
#include "mtsCommandProxyBase.h"

/*!
  \ingroup cisstMultiTask

  mtsMulticastCommandVoidProxy is a proxy for mtsMulticastCommandVoid.
*/
class mtsMulticastCommandVoidProxy: public mtsMulticastCommandVoid, public mtsCommandProxyBase
{
    friend class mtsComponentProxy;

public:
    typedef mtsMulticastCommandVoid BaseType;
    mtsProxySerializer Serializer;

protected:
    /*! The constructor with a name. */
    mtsMulticastCommandVoidProxy(const std::string & name): BaseType(name)
    {}

    ~mtsMulticastCommandVoidProxy() {
    }

public:
    /*! Execute all the commands in the composite. */
    virtual mtsExecutionResult Execute(mtsBlockingType CMN_UNUSED(blocking)) {
        size_t index;
        for (index = 0; index < Commands.size(); ++index) {
            Commands[index]->Execute(MTS_NOT_BLOCKING);
        }
        return mtsExecutionResult::COMMAND_SUCCEEDED;
    }

    /*! Getter */
    inline mtsProxySerializer * GetSerializer(void) {
        return &Serializer;
    }
};

#endif // _mtsMulticastCommandVoidProxy_h
