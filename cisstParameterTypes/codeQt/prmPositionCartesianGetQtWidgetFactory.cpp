/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2020-07-10

  (C) Copyright 2020-2022 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstParameterTypes/prmPositionCartesianGetQtWidgetFactory.h>

#include <cisstCommon/cmnUnits.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsManagerComponentServices.h>
#include <cisstParameterTypes/prmPositionCartesianGetQtWidget.h>

#include <QGridLayout>

prmPositionCartesianGetQtWidgetFactory::prmPositionCartesianGetQtWidgetFactory(const std::string & componentName):
    QWidget(),
    mtsComponent(componentName),
    mNumberOfWidgets(0)
{
    mPrismaticFactor = cmn_mm;
    mRevoluteFactor = cmnPI_180;
    EnableDynamicComponentManagement();
    setupUi();
    startTimer(50); // ms
}

void prmPositionCartesianGetQtWidgetFactory::setupUi(void)
{
    // Tools in a tab
    mLayout = new QGridLayout();
    setLayout(mLayout);

    // connect event
    connect(this, SIGNAL(SignalCRTKInterfacesProvidedUpdated()),
            this, SLOT(SlotCRTKInterfacesProvidedUpdated()));
}

void prmPositionCartesianGetQtWidgetFactory::timerEvent(QTimerEvent * CMN_UNUSED(event))
{
    if (this->isHidden()) {
        return;
    }
    prmPositionCartesianGet position;
    // iterate through all sources
    SourcesType::iterator source = mSources.begin();
    const SourcesType::iterator end = mSources.end();
    for (; source != end; ++source) {
        // get position
        source->second->mFunction(position);
        source->second->mWidget->SetValue(position);
    }
}

void prmPositionCartesianGetQtWidgetFactory::AddFactorySource(const std::string & componentName,
                                                              const std::string & interfaceName)
{
    mtsDescriptionInterfaceFullName source(mtsManagerLocal::GetInstance()->GetProcessName(),
                                           componentName, interfaceName);

    // first, make sure this doesn't already exist
    FactoriesType::const_iterator found = mFactories.find(source);
    if (found != mFactories.end()) {
        CMN_LOG_CLASS_INIT_WARNING << "AddFactorySource: factory already added for component \""
                                   << componentName << "\" with interface \""
                                   << interfaceName << "\"" << std::endl;
        return;
    }
    // create new factory
    Factory * newFactory = new Factory;
    newFactory->mComponentWidget = this;
    mFactories[source] = newFactory;

    // create a new interface for the factory
    std::string reqInterfaceName = "_" + componentName + "_using_" + interfaceName + "_factory";
    mtsInterfaceRequired * required = AddInterfaceRequired(reqInterfaceName);
    if (required) {
        required->AddFunction("crtk_interfaces_provided", newFactory->mCRTKInterfacesProvided);
        required->AddEventHandlerVoid(&Factory::CRTKInterfacesProvidedUpdatedHandler,
                                      newFactory, "crtk_interfaces_provided_updated");
        // connect interface
        mConnections.Add(this->GetName(), reqInterfaceName,
                         componentName, interfaceName);
    } else {
        CMN_LOG_CLASS_INIT_ERROR << "AddFactorySource: there's already an interface name \""
                                 << reqInterfaceName << "\"" << std::endl;
        return;
    }
}

void prmPositionCartesianGetQtWidgetFactory::Factory::CRTKInterfacesProvidedUpdatedHandler(void)
{
    // event handlers are not thread safe, emit so Qt can manage thread safety
    emit mComponentWidget->SignalCRTKInterfacesProvidedUpdated();
}

void prmPositionCartesianGetQtWidgetFactory::SlotCRTKInterfacesProvidedUpdated(void)
{
    // get all CRTK interfaces for all sources...
    FactoriesType::const_iterator factory = mFactories.begin();
    const FactoriesType::const_iterator factoriesEnd = mFactories.end();
    for (; factory != factoriesEnd; ++factory) {
        typedef std::vector<mtsDescriptionInterfaceFullName> PossibleSourcesType;
        PossibleSourcesType possibleSources;
        factory->second->mCRTKInterfacesProvided(possibleSources);
        PossibleSourcesType::const_iterator source = possibleSources.begin();
        const PossibleSourcesType::const_iterator sourcesEnd = possibleSources.end();
        for (; source != sourcesEnd; ++source) {
            // make sure this source is not already in use
            SourcesType::iterator found = mSources.find(*source);
            if (found == mSources.end()) {
                // check that the source has a read command returning a cartesian position
                mtsInterfaceProvidedDescription interfaceDescription =
                    ManagerComponentServices->GetInterfaceProvidedDescription(*source);
                // linear searches are slow but this is a one time only step
                if (mtsDescriptionHasName(interfaceDescription.CommandsRead,
                                          std::string("measured_cp"))) {
                    // new, create the interface and add to list of existing sources
                    std::string interfaceName = source->ProcessName
                        + "::" + source->ComponentName + "::" + source->InterfaceName;
                    mtsInterfaceRequired * newInterface = AddInterfaceRequired(interfaceName);
                    if (newInterface) {
                        // create interface
                        Source * newSource = new Source;
                        newSource->mInterface = newInterface;
                        // create the widget
                        newSource->mWidget = new prmPositionCartesianGetQtWidget;
                        newSource->mWidget->SetPrismaticRevoluteFactors(mPrismaticFactor, mRevoluteFactor);
                        // grid position
                        const int NB_COLS = 2;
                        int position = static_cast<int>(mNumberOfWidgets);
                        int row = position / NB_COLS;
                        int col = position % NB_COLS;
                        mLayout->addWidget(newSource->mWidget, row, col);
                        mNumberOfWidgets++;
                        // add function to the interface
                        newInterface->AddFunction("measured_cp", newSource->mFunction);
                        mSources[*source] = newSource;
                        // request connection
                        ManagerComponentServices->Connect(this->GetName(), interfaceName,
                                                          source->ComponentName, source->InterfaceName);
                    } else {
                        CMN_LOG_CLASS_INIT_ERROR << "SlotCRTKInterfacesProvidedUpdated: "
                                                 << this->GetName() << ", unable to create required interface \""
                                                 << interfaceName << "\"" << std::endl;
                    }
                }
            }
        }
    }
}
