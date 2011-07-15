/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet, Martin Kelly
  Created on: 2011-03-07

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstSphinx4/mtsSphinx4QtWidget.h>


mtsSphinx4QtWidget::mtsSphinx4QtWidget(void)
{
    // create the widgets
    LabelWordRecognized = new QLabel("Recognized word: ", this);
    ValueWordRecognized = new QLabel("nothing so far", this);
    LabelContext = new QLabel("Context: ", this);
    ValueContext = new QLabel("nothing so far", this);
    LabelVocabulary = new QLabel("Vocabulary: ", this);
    ValueVocabulary = new QLabel("", this);
    WordSelector = new QComboBox(this);
    TriggerButton = new QPushButton("Trigger", this);

    // configure the widgets
    LabelWordRecognized->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    LabelContext->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    LabelVocabulary->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // create a layout for the widgets
    CentralLayout = new QGridLayout(this);
    CentralLayout->setRowStretch(0, 1);
    CentralLayout->setColumnStretch(1, 1);
    CentralLayout->addWidget(LabelWordRecognized, 0, 0);
    CentralLayout->addWidget(ValueWordRecognized, 0, 1);
    CentralLayout->addWidget(LabelContext, 1, 0);
    CentralLayout->addWidget(ValueContext, 1, 1);
    CentralLayout->addWidget(LabelVocabulary, 2, 0);
    CentralLayout->addWidget(ValueVocabulary, 2, 1);
    CentralLayout->addWidget(WordSelector, 3, 0);
    CentralLayout->addWidget(TriggerButton, 3, 1);
}


void mtsSphinx4QtWidget::AddWord(QString context, QString word)
{
    ContextMap.insert(std::pair<QString,QString>(context, word));
}


void mtsSphinx4QtWidget::ContextChanged(QString context)
{
    WordSelector->clear();

    QString newVocabulary;
    // get the iterator bounds of all keys of a given value
    std::pair<std::multimap<QString,QString>::iterator,std::multimap<QString,QString>::iterator> bounds;
    bounds = ContextMap.equal_range(context);
    int keysLeft = ContextMap.count(context);
    std::multimap<QString,QString>::iterator iter;
    for (iter = bounds.first; iter != bounds.second; iter++, keysLeft--) {
        newVocabulary += iter->second;
        WordSelector->addItem(iter->second);
        if (keysLeft > 1)
            newVocabulary += '\n';
    }
    ValueContext->setText(context);
    ValueVocabulary->setText(newVocabulary);
}


void mtsSphinx4QtWidget::GetTriggeredWord(void)
{
    emit WordTriggered(WordSelector->currentText());
}
