/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2020-07-10

  (C) Copyright 2020-2021 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _prmPositionCartesianGetQtWidgetFactory_h
#define _prmPositionCartesianGetQtWidgetFactory_h

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsDelayedConnections.h>
#include <cisstParameterTypes/prmPositionCartesianGetQtWidget.h>

#include <QWidget>

// Always include last
#include <cisstParameterTypes/prmExportQt.h>

class QGridLayout;

class CISST_EXPORT prmPositionCartesianGetQtWidgetFactory: public QWidget, public mtsComponent
{
    Q_OBJECT;

public:
    prmPositionCartesianGetQtWidgetFactory(const std::string & componentName);
    ~prmPositionCartesianGetQtWidgetFactory(void) {};

    inline void SetPrismaticRevoluteFactors(const double & prismatic, const double & revolute) {
        mPrismaticFactor = prismatic;
        mRevoluteFactor = revolute;
    }

    void AddFactorySource(const std::string & componentName,
                          const std::string & interfaceName);

    inline virtual void Connect(void) {
        mConnections.Connect();
    }

protected:
    void setupUi(void);
    void timerEvent(QTimerEvent * event);

    double mPrismaticFactor, mRevoluteFactor;

    mtsDelayedConnections mConnections;

    class Factory {
    public:
        prmPositionCartesianGetQtWidgetFactory * mComponentWidget;
        mtsFunctionRead mCRTKInterfacesProvided;
        void CRTKInterfacesProvidedUpdatedHandler(void);
    };

    typedef std::map<mtsDescriptionInterfaceFullName, Factory *> FactoriesType;
    FactoriesType mFactories;

    class Source {
    public:
        mtsInterfaceRequired * mInterface;
        mtsFunctionRead mFunction;
        prmPositionCartesianGetQtWidget * mWidget;
    };

    typedef std::map<mtsDescriptionInterfaceFullName, Source *> SourcesType;
    SourcesType mSources;

    // Qt stuff
    int mNumberOfWidgets;
    QGridLayout * mLayout;

signals:
    void SignalCRTKInterfacesProvidedUpdated(void);

protected slots:
    void SlotCRTKInterfacesProvidedUpdated(void);

};

#endif // _prmPositionCartesianGetQtWidgetFactory_h
