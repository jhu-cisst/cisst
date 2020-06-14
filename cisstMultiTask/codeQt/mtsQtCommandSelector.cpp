/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda
  Created on: 2012-05-14

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <cisstMultiTask/mtsQtCommandSelector.h>

#include <QStringList>

mtsQtCommandSelector::mtsQtCommandSelector(mtsManagerGlobal* globalComponentManager, QWidget* parent)
    : QTreeWidget(parent), GlobalManager(globalComponentManager)
{
    setColumnCount(2);
    QStringList headerLabels;
    headerLabels << "Name" << "Type";
    setHeaderLabels(headerLabels);
    setSortingEnabled(true);
    BuildTree(globalComponentManager);
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(CurrentItemChanged));
    connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(ItemActivated));
}

void mtsQtCommandSelector::BuildTree(mtsManagerGlobal* globalComponentManager) {
    clear();

    QTreeWidgetItem* processItem;
    QTreeWidgetItem* componentItem;
    QTreeWidgetItem* interfaceItem;
    QTreeWidgetItem* commandItem;
    QStringList strings;

    std::vector<std::string> processNames;
    std::vector<std::string> componentNames;
    std::vector<std::string> interfaceNames;
    std::vector<std::string> commandNames;

    globalComponentManager->GetNamesOfProcesses(processNames);
    // Processes
    for(std::vector<std::string>::const_iterator processName = processNames.begin(); processName < processNames.end(); ++processName) {
        strings << processName->c_str() << "Process";
        processItem = new QTreeWidgetItem(strings);
        addTopLevelItem(processItem);
        strings.clear();
        // Components
        componentNames.clear();
        globalComponentManager->GetNamesOfComponents(*processName, componentNames);
        for(std::vector<std::string>::const_iterator componentName = componentNames.begin(); componentName < componentNames.end(); ++componentName) {
            strings << componentName->c_str() << "Component";
            componentItem = new QTreeWidgetItem(strings);
            processItem->addChild(componentItem);
            strings.clear();
            // Interfaces
            interfaceNames.clear();
            globalComponentManager->GetNamesOfInterfacesProvidedOrOutput(*processName, *componentName, interfaceNames);
            for(std::vector<std::string>::const_iterator interfaceName = interfaceNames.begin(); interfaceName < interfaceNames.end(); ++interfaceName) {
                strings << interfaceName->c_str() << "Interface";
                interfaceItem = new QTreeWidgetItem(strings);
                componentItem->addChild(interfaceItem);
                strings.clear();
                // Commands
                commandNames.clear();
                globalComponentManager->GetNamesOfCommands(*processName, *componentName, *interfaceName, commandNames);
                for(std::vector<std::string>::const_iterator commandName = commandNames.begin(); commandName < commandNames.end(); ++commandName) {
                    strings << commandName->c_str() << "Command";
                    commandItem = new QTreeWidgetItem(strings);
                    interfaceItem->addChild(commandItem);
                    strings.clear();
                }
            }
        }
    }
}

void mtsQtCommandSelector::Refresh(void) {
    BuildTree(GlobalManager);
}

void mtsQtCommandSelector::CurrentItemChanged(QTreeWidgetItem* CMN_UNUSED(current),
                                              QTreeWidgetItem* CMN_UNUSED(previous)) {
}

void mtsQtCommandSelector::ItemActivated(QTreeWidgetItem* item, int CMN_UNUSED(column)) {
    emit CommandSelected(item->text(0));
}
