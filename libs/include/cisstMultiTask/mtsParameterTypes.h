/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsParameterTypes.h 1726 2010-08-30 05:07:54Z mjung5 $

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
#include <cisstMultiTask/mtsVector.h> // for mtsDescriptionInterface::mtsStdStringVec
#include <cisstMultiTask/mtsComponentState.h>

// Always include last!
#include <cisstMultiTask/mtsExport.h>

#ifndef SWIG
// Nominal class to provide utility functions
class mtsParameterTypes {
public:
    static void ConvertVectorStringType(const mtsStdStringVec & mtsVec, std::vector<std::string> & stdVec);
    static void ConvertVectorStringType(const std::vector<std::string> & stdVec, mtsStdStringVec & mtsVec);
};
#endif

//-----------------------------------------------------------------------------
//  Component Description
//
class CISST_EXPORT mtsDescriptionComponent: public mtsGenericObject
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    std::string ProcessName;
    std::string ComponentName;
    std::string ClassName;

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
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    std::string ProcessName;
    std::string ComponentName;
    mtsStdStringVec InterfaceRequiredNames;
    mtsStdStringVec InterfaceProvidedNames;

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
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

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
    mtsDescriptionConnection(
        const std::string & clientProcessName, 
        const std::string & clientComponentName, const std::string & clientInterfaceRequiredName,
        const std::string & serverProcessName, 
        const std::string & serverComponentName, const std::string & serverInterfaceProvidedName,
        const int connectionId = -1);

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsDescriptionConnection);

typedef std::vector<mtsDescriptionConnection> mtsDescriptionConnectionStdVec;
typedef mtsGenericObjectProxy<mtsDescriptionConnectionStdVec> mtsDescriptionConnectionVec;
CMN_DECLARE_SERVICES_INSTANTIATION(mtsDescriptionConnectionVec);

// Define stream out operator for std::vector<mtsDescriptionConnection>
inline std::ostream & operator << (std::ostream & output,
                            const mtsDescriptionConnectionStdVec & object) {
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
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

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
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

public:
    mtsComponentStateChange() {}
    mtsComponentStateChange(const std::string & processName, 
                            const std::string & componentName,
                            const mtsComponentState::Enum & newState)
                            : ProcessName(processName),
                              ComponentName(componentName),
                              NewState(newState) {}

    std::string ProcessName;
    std::string ComponentName;
    mtsComponentState::Enum NewState;

    void ToStream(std::ostream & outputStream) const;
    void SerializeRaw(std::ostream & outputStream) const;
    void DeSerializeRaw(std::istream & inputStream);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsComponentStateChange);

#endif // _mtsParameterTypes_h
