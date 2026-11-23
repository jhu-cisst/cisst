/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda
  Created on: 2012-05-14

  (C) Copyright 2012-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsQtCommandSelector.h>

#include <QStringList>

mtsQtCommandSelector::mtsQtCommandSelector(mtsManagerLocal* componentManager, QWidget* parent)
    : QTreeWidget(parent), LocalManager(componentManager)
{
    setColumnCount(2);
    QStringList headerLabels;
    headerLabels << "Name" << "Type";
    setHeaderLabels(headerLabels);
    setSortingEnabled(true);
    BuildTree(LocalManager);
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(CurrentItemChanged));
    connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(ItemActivated));
}

void mtsQtCommandSelector::BuildTree(mtsManagerLocal* componentManager) {
    clear();

    QTreeWidgetItem* componentItem;
    QTreeWidgetItem* interfaceItem;
    QTreeWidgetItem* commandItem;
    QStringList strings;

    std::vector<std::string> componentNames;
    std::vector<std::string> interfaceRequiredNames;
    std::vector<std::string> interfaceProvidedNames;
    std::vector<std::string> commandNames;

    // Components
    componentNames.clear();
    componentManager->GetNamesOfComponents(componentNames);
    for(std::vector<std::string>::const_iterator componentName = componentNames.begin(); componentName < componentNames.end(); ++componentName) {
        strings << componentName->c_str() << "Component";
        componentItem = new QTreeWidgetItem(strings);
        addTopLevelItem(componentItem);
        strings.clear();
        // Interfaces
        interfaceRequiredNames.clear();
        interfaceProvidedNames.clear();
        // Provided or output
        componentManager->GetNamesOfInterfaces(*componentName, interfaceRequiredNames, interfaceProvidedNames);
        for(std::vector<std::string>::const_iterator interfaceName = interfaceProvidedNames.begin();
            interfaceName < interfaceProvidedNames.end(); ++interfaceName) {
            strings << interfaceName->c_str() << "Interface";
            interfaceItem = new QTreeWidgetItem(strings);
            componentItem->addChild(interfaceItem);
            strings.clear();
            // Commands
            commandNames.clear();
            componentManager->GetNamesOfCommands(commandNames, *componentName, *interfaceName);
            for(std::vector<std::string>::const_iterator commandName = commandNames.begin(); commandName < commandNames.end(); ++commandName) {
                strings << commandName->c_str() << "Command";
                commandItem = new QTreeWidgetItem(strings);
                interfaceItem->addChild(commandItem);
                strings.clear();
            }
        }
    }
}

void mtsQtCommandSelector::Refresh(void) {
    BuildTree(LocalManager);
}

void mtsQtCommandSelector::CurrentItemChanged(QTreeWidgetItem* CMN_UNUSED(current),
                                              QTreeWidgetItem* CMN_UNUSED(previous)) {
}

void mtsQtCommandSelector::ItemActivated(QTreeWidgetItem* item, int CMN_UNUSED(column)) {
    emit CommandSelected(item->text(0));
}
