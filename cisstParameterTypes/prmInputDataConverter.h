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

#ifndef _prmInputDataConverter_h
#define _prmInputDataConverter_h

#include <cisstConfig.h> // for CISST_HAS_JSON
#include <cisstMultiTask/mtsComponent.h>
#include <cisstParameterTypes/prmInputData.h>

// Always include last
#include <cisstParameterTypes/prmExport.h>

class prmInputDataConverterBase;

/*! Class to create cisstMultiTask events/commands based on
  prmInputData.  This can be used to convert inputs from an analog to
  digital converter or Linux joystick to cisstMultiTask
  prmEventButton.  We could also add events with booleans or integers,
  and maybe converters that can take 3/6 values from joystick and
  convert to a cartesian position, orientation, velocity, twist or
  wrench. */
class CISST_EXPORT prmInputDataConverter
{
 public:
    prmInputDataConverter(mtsComponent & component);

#if CISST_HAS_JSON
    bool ConfigureJSON(const Json::Value & jsonConfig);
#endif // CISST_HAS_JSON

    bool AddProvidedButton(const std::string & interfaceName,
                           const int & indexInput);

    /*! Tell converters to update based on the latest input data. */
    bool Update(const prmInputData & inputData);

 protected:
    mtsComponent & mComponent;

    typedef std::list<prmInputDataConverterBase *> ConvertersType;
    ConvertersType mConverters;

    /*! Pointer on existing services.  This allows to use the class
      name and level of detail of the component using the
      prmInputDataConverter. */
    const cmnClassServicesBase * mComponentServices;

    /*! Methods use to emulate the cmnGenericObject interface used by
      CMN_LOG_CLASS macros. */
    //@{
    inline const cmnClassServicesBase * Services(void) const {
        return this->mComponentServices;
    }

    inline cmnLogger::StreamBufType * GetLogMultiplexer(void) const {
        return cmnLogger::GetMultiplexer();
    }
    //@}
};

#endif // _prmInputDataConverter_h
