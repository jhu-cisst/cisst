/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda, Anton Deguet
  Created on: 2011-11-11

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsQtWidgetFactory.h>
#include <cisstCommon/cmnLogger.h>

#include <iostream>


mtsQtWidgetFactory::WidgetReadCreator::WidgetReadCreator(const std::type_info * type,
                                                         mtsQtWidgetFactory::WidgetReadCreatorFunction function)
{
    // todo: check that this type is not already defined, maybe allow to override?
    mtsQtWidgetFactory::GetWidgetReadCreators()[type] = function;
}


mtsQtWidgetFactory::WidgetWriteCreator::WidgetWriteCreator(const std::type_info * type,
                                                           mtsQtWidgetFactory::WidgetWriteCreatorFunction function)
{
    mtsQtWidgetFactory::GetWidgetWriteCreators()[type] = function;
}


mtsQtWidgetGenericObjectRead * mtsQtWidgetFactory::CreateWidgetRead(const std::type_info * type)
{
    WidgetReadCreatorMap::const_iterator creator = GetWidgetReadCreators().find(type);
    if (creator != GetWidgetReadCreators().end()) {
        return (creator->second)();
    }
    CMN_LOG_INIT_WARNING << "Class mtsQtWidgetFactory: CreateWidgetRead: can't find creator for type \"" << type->name() << "\"" << std::endl;
    return 0;
}


mtsQtWidgetGenericObjectWrite * mtsQtWidgetFactory::CreateWidgetWrite(const std::type_info * type)
{
    WidgetWriteCreatorMap::const_iterator creator = GetWidgetWriteCreators().find(type);
    if (creator != GetWidgetWriteCreators().end()) {
        return (creator->second)();
    }
    CMN_LOG_INIT_WARNING << "Class mtsQtWidgetFactory: CreateWidgetRead: can't find creator for type " << type << " (" << type->name() << ")" << std::endl;
    return 0;
}


mtsQtWidgetFactory::WidgetReadCreatorMap & mtsQtWidgetFactory::GetWidgetReadCreators(void)
{
    static WidgetReadCreatorMap mapInstance;
    return mapInstance;
}


mtsQtWidgetFactory::WidgetWriteCreatorMap & mtsQtWidgetFactory::GetWidgetWriteCreators(void)
{
    static WidgetWriteCreatorMap mapInstance;
    return mapInstance;
}
