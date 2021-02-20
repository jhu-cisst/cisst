/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2021-02-19

  (C) Copyright 2021 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmInputDataConverter.h>

#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstParameterTypes/prmEventButton.h>

class prmInputDataConverterBase
{
public:
    inline prmInputDataConverterBase(const std::string & name):
        Name(name),
        FirstRun(true)
    {}

    virtual bool Update(const prmInputData & data) = 0;
    std::string Name; // for error messages
    bool FirstRun;

private:
    prmInputDataConverterBase();
};


class prmInputDataConverterProvidedButton: public prmInputDataConverterBase
{
public:
    mtsFunctionWrite Event;

    inline prmInputDataConverterProvidedButton(const std::string & name,
                                               const int inputIndex):
        prmInputDataConverterBase(name),
        Index(inputIndex)
    {}

    inline bool Update(const prmInputData & data) {
        // make sure the value exists at index
        if (data.DigitalInputs().size() < static_cast<size_t>(Index)) {
            return false;
        }
        // check if value is new, except for first run
        const bool value = data.DigitalInputs().at(Index);
        if ((value == Previous) && !FirstRun) {
            return true;
        }
        FirstRun = false;
        // figure out which event to send
        if (data.DigitalInputs().at(Index)) {
            Payload.Type() = prmEventButton::PRESSED;
        } else {
            Payload.Type() = prmEventButton::RELEASED;
        }
        Payload.SetValid(data.Valid());
        Payload.SetTimestamp(data.Timestamp());
        Event(Payload);
        // save last value to avoid repeating event
        Previous = value;
        return true;
    }

protected:
    prmEventButton Payload;
    bool Previous;
    int Index;
};

prmInputDataConverter::prmInputDataConverter(mtsComponent & component):
    mComponent(component),
    mComponentServices(component.Services())
{
}

bool prmInputDataConverter::AddProvidedButton(const std::string & interfaceName,
                                              const int & indexInput)
{
    mtsInterfaceProvided * itf = mComponent.AddInterfaceProvided(interfaceName);
    if (itf) {
        prmInputDataConverterProvidedButton * converter
            = new prmInputDataConverterProvidedButton(interfaceName,
                                                      indexInput);
        itf->AddEventWrite(converter->Event, "Button", prmEventButton());
        mConverters.push_back(converter);
        return true;
    }
    CMN_LOG_CLASS_INIT_ERROR << "AddProvidedButton: failed to create new interface \""
                             << interfaceName << "\"" << std::endl;
    return false;
}

#if CISST_HAS_JSON
bool prmInputDataConverter::ConfigureJSON(const Json::Value & jsonConfig)
{
    for (unsigned int index = 0; index < jsonConfig.size(); ++index) {
        const Json::Value jsonConverter = jsonConfig[index];
        Json::Value jsonValue = jsonConverter["type"];
        if (jsonValue.empty()) {
            CMN_LOG_CLASS_INIT_ERROR << "ConfigureJSON: \"type\" is missing for converter #" << index + 1 << std::endl;
            return false;
        }
        const std::string type = jsonValue.asString();
        // "factory"
        if (type == "interface-provided-button") {
            // get interface name and index
            jsonValue = jsonConverter["interface-name"];
            if (jsonValue.empty()) {
                CMN_LOG_CLASS_INIT_ERROR << "ConfigureJSON: \"interface-name\" is missing for converter #" << index + 1 << std::endl;
                return false;
            }
            std::string interfaceName = jsonValue.asString();
            jsonValue = jsonConverter["index-input"];
            if (jsonValue.empty()) {
                CMN_LOG_CLASS_INIT_ERROR << "ConfigureJSON: \"index-input\" is missing for converter #" << index + 1 << std::endl;
                return false;
            }
            int indexInput = jsonValue.asInt();
            // create converter
            if (!AddProvidedButton(interfaceName, indexInput)) {
                return false;
            }
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "ConfigureJSON: \"type\" is \"" << type << "\" is not supported" << std::endl;
            return false;
        }
    }
    return true;
}

bool prmInputDataConverter::Update(const prmInputData & inputData)
{
    const ConvertersType::const_iterator end = mConverters.end();
    ConvertersType::const_iterator converter = mConverters.begin();
    for (; converter != end; ++converter) {
        if (!(*converter)->Update(inputData)) {
            CMN_LOG_CLASS_RUN_ERROR << "Update: converter \"" << (*converter)->Name << "\" failed" << std::endl;
        }
    }
    return true;
}

#endif // CISST_HAS_JSON
