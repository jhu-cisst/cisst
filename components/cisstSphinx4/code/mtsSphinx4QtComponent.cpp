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

#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <cisstSphinx4/mtsSphinx4QtComponent.h>

CMN_IMPLEMENT_SERVICES(mtsSphinx4QtComponent);


mtsSphinx4QtComponent::mtsSphinx4QtComponent(const std::string & componentName):
    mtsComponent(componentName)
{
    // create the cisstMultiTask interface with commands and events
    mtsInterfaceRequired * interfaceRequired = AddInterfaceRequired("Sphinx4");
    if (interfaceRequired) {
        interfaceRequired->AddFunction("GetContexts", GetContexts);
        interfaceRequired->AddFunction("GetContextWords", GetContextWords);
        interfaceRequired->AddFunction("TriggerWordFromUI", TriggerWordFromUI);
        interfaceRequired->AddEventHandlerWrite(&mtsSphinx4QtComponent::WordRecognizedHandler,
                                                this, "WordRecognized");
        interfaceRequired->AddEventHandlerVoid(&mtsSphinx4QtComponent::NoWordRecognizedHandler,
                                               this, "NoWordRecognized");
        interfaceRequired->AddEventHandlerWrite(&mtsSphinx4QtComponent::ContextChangedHandler,
                                                this, "ContextChanged");
    }
    // Connect Qt signals to slots
    QObject::connect(this, SIGNAL(WordRecognizedQSignal(QString)),
                     CentralWidget.ValueWordRecognized, SLOT(setText(QString)));

    QObject::connect(this, SIGNAL(ContextChangedQSignal(QString)),
                     &CentralWidget, SLOT(ContextChanged(QString)));

    QObject::connect(this, SIGNAL(WordAddedQSignal(QString, QString)),
                     &CentralWidget, SLOT(AddWord(QString, QString)));

    QObject::connect(CentralWidget.TriggerButton, SIGNAL(clicked()),
                     &CentralWidget, SLOT(GetTriggeredWord()));

    QObject::connect(&CentralWidget, SIGNAL(WordTriggered(QString)),
                     this, SLOT(TriggerWord(QString)));
}


void mtsSphinx4QtComponent::Start(void)
{
    stdStringVec contexts, words;
    std::string currentContext, currentWord;
    mtsExecutionResult result;
    // get list of contexts
    result = this->GetContexts(contexts);
    if (!result) {
        CMN_LOG_CLASS_INIT_ERROR << "Start: can not retrieve contexts, error \"" << result << "\"" << std::endl;
        return;
    }
    const size_t numberOfContexts = contexts.size();
    size_t numberOfWords;
    for (size_t contextIndex = 0;
         contextIndex < numberOfContexts;
         contextIndex++) {
        currentContext = contexts[contextIndex];
        result = this->GetContextWords(mtsStdString(currentContext), words);
        if (!result) {
            CMN_LOG_CLASS_INIT_ERROR << "Start: can not retrieve words for context \""
                                     << currentContext << "\", error \"" << result << "\"" << std::endl;
            return;
        }
        numberOfWords = words.size();
        for (size_t wordIndex = 0;
             wordIndex < numberOfWords;
             wordIndex++) {
            currentWord = words[wordIndex];
            emit WordAddedQSignal(QString(currentContext.c_str()), QString(currentWord.c_str()));
        }
    }
}


QWidget * mtsSphinx4QtComponent::GetWidget(void)
{
    return &CentralWidget;
}


void mtsSphinx4QtComponent::TriggerWord(QString word)
{
    TriggerWordFromUI(mtsStdString(word.toStdString()));
}


void mtsSphinx4QtComponent::WordRecognizedHandler(const mtsStdString & word)
{
    emit WordRecognizedQSignal(QString(word.Data.c_str()));
}


void mtsSphinx4QtComponent::NoWordRecognizedHandler(void)
{
    emit NoWordRecognizedQSignal();
}


void mtsSphinx4QtComponent::ContextChangedHandler(const mtsStdString & context)
{
    emit ContextChangedQSignal(QString(context.Data.c_str()));
}
