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

#ifndef _mtsQtWidgetComponent_h
#define _mtsQtWidgetComponent_h

#include <QWidget>
#include <QTabWidget>

#include <cisstMultiTask/mtsComponent.h>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

/* ! A widget that wraps a single mtsComponent. A mtsQtWidgetComponent will create a child
   InterfaceProvidedWidget for every provided interface on the given component.
*/

class CISST_EXPORT mtsQtWidgetComponent: public QWidget, public mtsComponent
{
    Q_OBJECT;
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_ALLOW_DEFAULT);

private:

    QTabWidget * TabWidget;

    // this methods queries all provided interfaces from the component
    // "observed" and will create the corresponding required
    // interfaces as well as their widgets.


public:
    mtsQtWidgetComponent(const std::string & name);
    // this methods queries all provided interfaces from the component
    // "observed" and will create the corresponding required
    // interfaces as well as their widgets.
    void CreateWidgetsForComponent(const mtsComponent & component);
    bool CreateWidgetsForComponent(const std::string & componentName);
    void CreateWidgetsForInterface(const mtsComponent & component, const std::string & interfaceName);
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsQtWidgetComponent);

#endif // _mtsQtWidgetComponent_h
