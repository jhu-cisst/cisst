/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet, Min Yang Jung
  Created on: 2010-09-01

  (C) Copyright 2010-2020 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsParameterTypes_h
#define _mtsParameterTypes_h

#include <cisstCommon/cmnDataFunctions.h>
#include <cisstCommon/cmnDataFunctionsString.h>
#include <cisstCommon/cmnDataFunctionsVector.h>

#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsInterfaceCommon.h>
#include <cisstMultiTask/mtsComponentState.h>

#include <cisstMultiTask/mtsGenericObjectProxy.h>

// Always include last!
#include <cisstMultiTask/mtsExport.h>

//-----------------------------------------------------------------------------
// Add Observers
//

class CISST_EXPORT mtsEventHandlerList: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

#ifndef SWIG
    template <class _commandType>
    struct EventHandlerInfo {
        std::string EventName;
        _commandType * HandlerPointer;
        mtsRequiredType Required;
        bool Result;
        EventHandlerInfo(const std::string & name, _commandType * handler, mtsRequiredType required)
            : EventName(name), HandlerPointer(handler), Required(required), Result(false) {}
        ~EventHandlerInfo() {}
    };
#endif

public:
#ifndef SWIG
    typedef EventHandlerInfo<mtsCommandVoid> InfoVoid;
    typedef EventHandlerInfo<mtsCommandWriteBase> InfoWrite;
#endif

    mtsEventHandlerList() : Provided(0) {}
    mtsEventHandlerList(mtsInterfaceProvided * provided) : Provided(provided) {}
    ~mtsEventHandlerList() {}

#ifndef SWIG
    mtsInterfaceProvided * Provided;
    std::vector<InfoVoid> VoidEvents;
    std::vector<InfoWrite> WriteEvents;
#endif

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsEventHandlerList);

//-----------------------------------------------------------------------------
//  System-wide Thread-safe Logging
//
class CISST_EXPORT mtsLogMessage: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    enum { MAX_LOG_SIZE = 1024 };

    size_t       Length;
    char         Message[MAX_LOG_SIZE];
    std::string  ProcessName;

    /*! Default constructor */
    mtsLogMessage();
    /*! Copy constructor */
    mtsLogMessage(const mtsLogMessage & other);
    /*! Constructor */
    mtsLogMessage(const char * log, size_t len);
    /*! Destructor */
    ~mtsLogMessage() {}
    mtsLogMessage & operator = (const mtsLogMessage & other);
    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsLogMessage);

#endif // _mtsParameterTypes_h
