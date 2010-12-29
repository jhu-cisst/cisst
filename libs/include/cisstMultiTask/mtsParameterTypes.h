/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Min Yang Jung
  Created on: 2010-09-01

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsParameterTypes_h
#define _mtsParameterTypes_h

#include <cisstMultiTask/mtsGenericObject.h>
#include <cisstMultiTask/mtsGenericObjectProxy.h>
#include <cisstMultiTask/mtsComponentState.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>

// Always include last!
#include <cisstMultiTask/mtsExport.h>

//-----------------------------------------------------------------------------
//  Component state (mtsComponentState)
//
typedef mtsGenericObjectProxy<mtsComponentState> mtsComponentStateProxy;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentStateProxy);

//-----------------------------------------------------------------------------
//  Component Description
//
class CISST_EXPORT mtsDescriptionComponent: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    std::string ProcessName;
    std::string ComponentName;
    std::string ClassName;

    /*! Default constructor */
    mtsDescriptionComponent() {}
    /*! Copy constructor */
    mtsDescriptionComponent(const mtsDescriptionComponent &other);
    /*! Constructor process name and component name */
    mtsDescriptionComponent(const std::string &processName, const std::string &componentName);
    /*! Destructor */
    ~mtsDescriptionComponent() {}

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsDescriptionComponent);


//-----------------------------------------------------------------------------
//  Interface Description
//
class CISST_EXPORT mtsDescriptionInterface: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    std::string ProcessName;
    std::string ComponentName;
    std::vector<std::string> InterfaceRequiredNames;
    std::vector<std::string> InterfaceProvidedNames;

    /*! Default constructor */
    mtsDescriptionInterface() {}
    /*! Copy constructor */
    mtsDescriptionInterface(const mtsDescriptionInterface &other);
    /*! Destructor */
    ~mtsDescriptionInterface() {}

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsDescriptionInterface);


//-----------------------------------------------------------------------------
//  Connection Description
//
class CISST_EXPORT mtsDescriptionConnection: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
#ifndef SWIG
    struct FullInterface {
        std::string ProcessName;
        std::string ComponentName;
        std::string InterfaceName;
    };

    FullInterface Client;
    FullInterface Server;
    int ConnectionID;
#endif

    mtsDescriptionConnection() : ConnectionID(-1) {}
    mtsDescriptionConnection(const mtsDescriptionConnection &other);
    mtsDescriptionConnection(
        const std::string & clientProcessName, 
        const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, 
        const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
        const int connectionId = -1);
    ~mtsDescriptionConnection() {}

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsDescriptionConnection);

typedef std::vector<mtsDescriptionConnection> mtsDescriptionConnectionVec;
typedef mtsGenericObjectProxy<mtsDescriptionConnectionVec> mtsDescriptionConnectionVecProxy;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsDescriptionConnectionVecProxy);

// Define stream out operator for std::vector<mtsDescriptionConnection>
inline std::ostream & operator << (std::ostream & output,
                                   const std::vector<mtsDescriptionConnection> & object) {
    output << "[";
    for (size_t i = 0; i < object.size(); i++) {
        object[i].ToStream(output);
        if (i < object.size()-1)
            output << ", ";
    }
    output << "]";
    return output;
}


//-----------------------------------------------------------------------------
//  Component Status Control
//
class CISST_EXPORT mtsComponentStatusControl : public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    typedef enum { 
        COMPONENT_START = 0, 
        COMPONENT_STOP, 
        COMPONENT_RESUME 
    } ComponentStatusCommand;

    std::string ProcessName;
    std::string ComponentName;
    double      DelayInSecond;
    ComponentStatusCommand Command;

    mtsComponentStatusControl() : DelayInSecond(0.0) {}
    mtsComponentStatusControl(const mtsComponentStatusControl &other);
    ~mtsComponentStatusControl() {}

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentStatusControl);


//-----------------------------------------------------------------------------
//  Component Status Change Event
//
class CISST_EXPORT mtsComponentStateChange : public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

public:
    mtsComponentStateChange() {}
    mtsComponentStateChange(const mtsComponentStateChange &other);
    mtsComponentStateChange(const std::string & processName, 
                            const std::string & componentName,
                            const mtsComponentState & newState)
                            : ProcessName(processName),
                              ComponentName(componentName),
                              NewState(newState) {}

    std::string ProcessName;
    std::string ComponentName;
    mtsComponentState NewState;

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentStateChange);

//-----------------------------------------------------------------------------
// GetEndUserInterface (provided interface)
//

class CISST_EXPORT mtsEndUserInterfaceArg : public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    std::string UserName;
    mtsInterfaceProvided * OriginalInterface;
    mtsInterfaceProvided * EndUserInterface;

    mtsEndUserInterfaceArg() : OriginalInterface(0), EndUserInterface(0) {}
    mtsEndUserInterfaceArg(mtsInterfaceProvided * orig, const std::string &name) :
        UserName(name), OriginalInterface(orig), EndUserInterface(0) {}
    ~mtsEndUserInterfaceArg() {}

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsEndUserInterfaceArg);

//-----------------------------------------------------------------------------
// Add Observers
//

class CISST_EXPORT mtsEventHandlerList : public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

#ifndef SWIG
    template <class _CommandType>
    struct EventHandlerInfo {
        std::string EventName;
        _CommandType *HandlerPtr;
        mtsRequiredType Required;
        bool Result;
        EventHandlerInfo(const std::string &name, _CommandType *handler, mtsRequiredType required)
            : EventName(name), HandlerPtr(handler), Required(required), Result(false) {}
        ~EventHandlerInfo() {}
    };
#endif

public:
#ifndef SWIG
    typedef EventHandlerInfo<mtsCommandVoid> InfoVoid;
    typedef EventHandlerInfo<mtsCommandWriteBase> InfoWrite;
#endif

    mtsEventHandlerList() : Provided(0) {}
    mtsEventHandlerList(mtsInterfaceProvided *provided) : Provided(provided) {}
    ~mtsEventHandlerList() {}

#ifndef SWIG
    mtsInterfaceProvided *Provided;
    std::vector<InfoVoid> VoidEvents;
    std::vector<InfoWrite> WriteEvents;
#endif

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsEventHandlerList);

#endif // _mtsParameterTypes_h
