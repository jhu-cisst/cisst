/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Praneeth Sadda
  Created on: 2012-05-24

  (C) Copyright 2012-2013 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/

#include <sstream>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QMenu>
#include <QPushButton>

#include <cisstMultiTask/mtsQtCommandSelector.h>
#include <cisstMultiTask/mtsQtScaleAndSignalTool.h>

mtsQtScaleAndSignalTool::mtsQtScaleAndSignalTool(mtsManagerGlobal * globalManager, vctPlot2DOpenGLQtWidget * visualizer, QWidget * parent)
    : QWidget(parent), Visualizer(visualizer)
{
    QLayout * layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    QSplitter * splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    layout->addWidget(splitter);

    splitter->addWidget(new mtsQtScaleEditor(visualizer));
    splitter->addWidget(new mtsQtCommandSelector(globalManager));
}

mtsQtScaleEditor::mtsQtScaleEditor(vctPlot2DOpenGLQtWidget * visualizer, QWidget * parent)
    : QTreeWidget(parent), Visualizer(visualizer), ScaleNameCounter(0)
{
    QStringList headerLabels;
    headerLabels << "Scales";
    setHeaderLabels(headerLabels);
    setSortingEnabled(true);
    BuildTree(visualizer);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ShowContextMenu(const QPoint &)));
}


void mtsQtScaleEditor::BuildTree(vctPlot2DOpenGLQtWidget * visualizer)
{
    const vctPlot2DOpenGLQtWidget::ScalesType & scales = visualizer->GetScales();
    vctPlot2DOpenGLQtWidget::ScalesType::const_iterator scalesIt = scales.begin();
    const vctPlot2DOpenGLQtWidget::ScalesType::const_iterator scalesEnd = scales.end();
    vctPlot2DOpenGLQtWidget::Scale::SignalsType::const_iterator signalsIt;
    vctPlot2DOpenGLQtWidget::Scale::SignalsType::const_iterator signalsEnd;
    QTreeWidgetItem * scaleItem;
    QStringList strings;

    for (; scalesIt != scalesEnd; ++scalesIt) {
        strings << scalesIt->second->GetName().c_str();
        scaleItem = new QTreeWidgetItem(strings);
        scaleItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
        strings.clear();
        addTopLevelItem(scaleItem);
        signalsIt = scalesIt->second->GetSignals().begin();
        signalsEnd = scalesIt->second->GetSignals().end();
        for (; signalsIt != signalsEnd; ++signalsIt) {
            strings << signalsIt->second->GetName().c_str();
            scaleItem->addChild(new QTreeWidgetItem(strings));
            strings.clear();
        }
    }
}


void mtsQtScaleEditor::NewScale(void)
{
    std::stringstream ss;
    ss << "Scale " << ++ScaleNameCounter;
    while(Visualizer->FindScale(ss.str()) != 0) {
        ss.clear();
        ss << "Scale " << ++ScaleNameCounter;
    }
    Visualizer->AddScale(ss.str());
    QStringList strings;
    strings << ss.str().c_str();
    QTreeWidgetItem * scaleItem = new QTreeWidgetItem(strings);
    scaleItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
    addTopLevelItem(scaleItem);
}


void mtsQtScaleEditor::RenameScale(void)
{
    QTreeWidgetItem * scaleItem = this->findItems(CurrentScale.toLocal8Bit().data(), Qt::MatchCaseSensitive | Qt::MatchFixedString).first();
    this->edit(this->model()->index(this->indexOfTopLevelItem(scaleItem), 0));
}


void mtsQtScaleEditor::DeleteScale(void)
{
    Visualizer->RemoveScale(CurrentScale.toLocal8Bit().data());
    QTreeWidgetItem * scaleItem = this->findItems(CurrentScale.toLocal8Bit().data(), Qt::MatchCaseSensitive | Qt::MatchFixedString).first();
    this->takeTopLevelItem(this->indexOfTopLevelItem(scaleItem));
}


void mtsQtScaleEditor::ShowContextMenu(const QPoint & point)
{
    QMenu menu(this);
    menu.addAction("&New scale", this, SLOT(NewScale()));
    QTreeWidgetItem * item = this->itemAt(point);
    if(item) {
        menu.addSeparator();
        menu.addAction("&Rename scale", this, SLOT(RenameScale()));
        menu.addAction("&Delete scale", this, SLOT(DeleteScale()));
        menu.addSeparator();
        menu.addAction("&Add signal");
        CurrentScale = item->text(0);
    }
    menu.exec(this->mapToGlobal(point));
}
