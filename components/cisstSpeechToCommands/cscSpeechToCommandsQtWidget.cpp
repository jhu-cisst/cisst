/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cscSpeechToCommandsQtWidget.cpp 2936 2011-04-19 16:32:39Z mkelly9 $

  Author(s):  Anton Deguet, Martin Kelly
  Created on: 2011-03-07

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "cscSpeechToCommandsQtWidget.h"


cscSpeechToCommandsQtWidget::cscSpeechToCommandsQtWidget(void)
{
    // create the widgets
    LabelWordRecognized = new QLabel("Recognized word: ", this);
    ValueWordRecognized = new QLabel("nothing so far", this);
    LabelContext = new QLabel("Context: ", this);
    ValueContext = new QLabel("nothing so far", this);
    LabelVocabulary = new QLabel("Vocabulary: ", this);
    ValueVocabulary = new QLabel("", this);

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
}


void cscSpeechToCommandsQtWidget::AddWord(QString context, QString word)
{
    ContextMap.insert(context, word);
}


void cscSpeechToCommandsQtWidget::ContextChanged(QString context)
{
    QString newVocabulary;
    QMultiMap<QString, QString>::iterator iter;
    for (iter = ContextMap.find(context);
         iter != ContextMap.end() && iter.key() == context;
         iter++) {
        newVocabulary += iter.value();
        QMultiMap<QString, QString>::iterator next = iter+1;
        if (next != ContextMap.end() && next.key() == context)
            newVocabulary += '\n';
    }
    ValueContext->setText(context);
    ValueVocabulary->setText(newVocabulary);
}
