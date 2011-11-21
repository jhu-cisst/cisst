/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

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

#ifndef _mtsComponentWidget_h
#define _mtsComponentWidget_h

#include <QWidget>
#include <QTabWidget>

#include <cisstMultiTask/mtsInterfaceProvided.h>

// Always include last
#include <cisstMultiTask/mtsExportQt.h>

/* ! A widget that wraps a single mtsComponent. A mtsComponentWidget will create a child
   InterfaceProvidedWidget for every provided interface on the given component.
*/

// adeguet1: should this be more like a component that has a widget,
// more than a widget that has a component?  Somehow this would make
// more sense to me.
class CISST_EXPORT mtsComponentWidget : public QWidget {

    Q_OBJECT;

private:

    // adeguet1: I believe this is the component that caries the
    // provided interfaces we are connecting to.  we should not keep a
    // pointer on it and retrieve all the necessary information from
    // the component manager.
    const mtsComponent * Component;
    mtsComponent * ExecutionComponent;

    QTabWidget * TabWidget;

    // this methods queries all provided interfaces from the component
    // "observed" and will create the corresponding required
    // interfaces as well as their widgets.
    void UpdateUI(const mtsComponent & component);

public:
    mtsComponentWidget(const mtsComponent* component, bool makeWindow = false);
    //inline const InterfaceProvidedWidget* GetWidgetForInterfaceProvided(const mtsInterfaceProvided* interface) const;
    //inline InterfaceProvidedWidget* GetWidgetForInterfaceProvided(const mtsInterfaceProvided* interface);
    inline const QWidget* GetWindow(void) const;
    inline QWidget* GetWindow(void);
};

/*
  const InterfaceProvidedWidget* mtsComponentWidget::GetWidgetForInterfaceProvided(const mtsInterfaceProvided* interface) const
  {
  return (interfaceWidgets.find(interface))->second;
  }

  InterfaceProvidedWidget* mtsComponentWidget::GetWidgetForInterfaceProvided(const mtsInterfaceProvided* interface)
  {
  return interfaceWidgets[interface];
  }
*/

#endif // _mtsComponentWidget_h
